#include "pch.h"
#include "SmAIIndicatorManager.h"
#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include "../Chart/SmChartData.h"
#include "../Chart/SmChartDataManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Ai/SmCorrelationManager.h"
#include "../Json/json.hpp"
#include "../Global/SmGlobal.h"
#include "../Service/SmServiceDefine.h"
#include "../Service/SmProtocolManager.h"
#include "../Database/SmMongoDBManager.h"
//#include "SmSessionManager.h"
//#include "SmWebsocketSession.h"
#include <string>
using namespace nlohmann;

SmAIIndicatorManager::SmAIIndicatorManager()
{
	_Pool = new ThreadPool(3);
	_Pool->init();
}

SmAIIndicatorManager::~SmAIIndicatorManager()
{
	if (_Pool) {
		_Pool->shutdown();
		delete _Pool;
		_Pool = nullptr;
	}
}

std::vector<int> SmAIIndicatorManager::GetAIIndicator(std::string symbol_code, int session_id)
{
	std::vector<std::shared_ptr<SmChartData>> chart_vector;
	SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
	std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(symbol_code, (int)SmChartType::MIN, 5);
	chart_vector.push_back(chart_data);

	chart_data = chartDataMgr->AddChartData(symbol_code, (int)SmChartType::MIN, 10);
	chart_vector.push_back(chart_data);

	chart_data = chartDataMgr->AddChartData(symbol_code, (int)SmChartType::MIN, 15);
	chart_vector.push_back(chart_data);

	chart_data = chartDataMgr->AddChartData(symbol_code, (int)SmChartType::MIN, 30);
	chart_vector.push_back(chart_data);

	chart_data = chartDataMgr->AddChartData(symbol_code, (int)SmChartType::MIN, 60);
	chart_vector.push_back(chart_data);

	chart_data = chartDataMgr->AddChartData(symbol_code, (int)SmChartType::DAY, 1);
	chart_vector.push_back(chart_data);

	// Creates a copy of classInstace, use std::ref(classInstance) to force pass by ref
	auto function = std::bind(&SmAIIndicatorManager::GetChartData, std::ref(*this), chart_vector, session_id);

	auto future2 = _Pool->submit(function);

	// Wait for multiplication output to finish
	std::vector<int> res = future2.get();

	return res;
}

void SmAIIndicatorManager::RequestIndicator(SmIndicatorRequest req)
{
	SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
	std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req.symbolCode, (int)req.chartType, req.cycle);
	if (chart_data->Received()) { // 차트데이터가 있으면 바로 계산해서 결과를 반환한다.
		// 쓰레드로 바로 처리해 버린다.
		auto function = std::bind(&SmAIIndicatorManager::CalcIndicator, std::ref(*this), chart_data, req);
		_Pool->submit(function);
	}
	else { // 차트 데이터가 없으면 대기 큐에 넣고 차트데이터를 요청한다.
		//std::string data_key = req.GetDataKey();
		//_RequestMap[data_key] = req;
		//RequestChartData(req);
		SmChartDataRequest chart_req;
		chart_req.symbolCode = req.symbolCode;
		chart_req.chartType = req.chartType;
		chart_req.cycle = req.cycle;
		chart_req.count = 1500;
		if (req.chartType == SmChartType::DAY) {
			std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(req.symbolCode);
			chart_req.product_code = symbol->CategoryCode();
		}

		SmMongoDBManager::GetInstance()->GetChartData(chart_req);

		// 쓰레드로 바로 처리해 버린다.
		auto function = std::bind(&SmAIIndicatorManager::CalcIndicator, std::ref(*this), chart_data, req);
		_Pool->submit(function);
	}
}

void SmAIIndicatorManager::OnReceivedChartData(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return;
	auto it = _RequestMap.find(chart_data->GetDataKey());
	if (it == _RequestMap.end())
		return;
	SmIndicatorRequest ind_req = it->second;
	// 쓰레드로 처리해 버린다.
	auto function = std::bind(&SmAIIndicatorManager::CalcIndicator, std::ref(*this), chart_data, ind_req);
	_Pool->submit(function);
}

void SmAIIndicatorManager::SendResult(int result, SmIndicatorRequest ind_req)
{
	/*
	json send_object;
	send_object["res_id"] = SmProtocol::res_indicator;
	send_object["chart_type"] = (int)ind_req.chartType;
	send_object["symbol_code"] = ind_req.symbolCode;
	send_object["cycle"] = ind_req.cycle;
	send_object["indicator_type"] = (int)ind_req.indicatorType;
	send_object["result"] = result;

	std::string content = send_object.dump();
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(ind_req.session_id, content);
	*/
	SmProtocolManager::GetInstance()->SendIndicatorResult(result, ind_req);
}

void SmAIIndicatorManager::RequestChartData(SmIndicatorRequest ind_req)
{
	SmChartDataRequest req;
	req.symbolCode = ind_req.symbolCode;
	req.chartType = ind_req.chartType;
	req.cycle = ind_req.cycle;
	req.count = 1500;
	if (ind_req.chartType == SmChartType::DAY) {
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(ind_req.symbolCode);
		req.product_code = symbol->CategoryCode();
	}
	SmChartDataManager::GetInstance()->AddTask(std::move(req));
}

void SmAIIndicatorManager::CalcIndicator(std::shared_ptr<SmChartData> chart_data, SmIndicatorRequest ind_req)
{
	int result = 0;
	SmCorrelationManager* corMgr = SmCorrelationManager::GetInstance();
	switch (ind_req.indicatorType)
	{
	case SmIndicatorType::SimpleMa:
		result = corMgr->CheckSma(chart_data);
		break;
	case SmIndicatorType::Macd:
		result = corMgr->CheckMacd(chart_data);
		break;
	case SmIndicatorType::Stochastic:
		result = corMgr->CheckStochastic(chart_data);
		break;
	case SmIndicatorType::Sar:
		result = corMgr->CheckSar(chart_data);
		break;
	case SmIndicatorType::Channel:
		result = corMgr->CheckChannel(chart_data);
		break;
	case SmIndicatorType::CompositeMa:
		result = corMgr->CheckCompositeMa(chart_data);
		break;
	default:
		break;
	}

	SendResult(result, ind_req);
}

std::vector<int> SmAIIndicatorManager::GetChartData(std::vector<std::shared_ptr<SmChartData>> chart_vector, int session_id)
{
	std::vector<int> result;
	// 차트 데이터를 먼저 요청한다.
	SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
	for (auto it = chart_vector.begin(); it != chart_vector.end(); ++it) {
		std::shared_ptr<SmChartData> chart_data = *it;

		SmChartDataRequest req;
		req.symbolCode = chart_data->SymbolCode();
		req.chartType = chart_data->ChartType();
		req.cycle = chart_data->Cycle();
		req.count = 1500;
		req.session_id = session_id;
		if (chart_data->ChartType() == SmChartType::DAY) {
			std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(chart_data->SymbolCode());
			req.product_code = symbol->CategoryCode();
		}
		SmChartDataManager::GetInstance()->AddTask(std::move(req));
	}

	return result;
}
