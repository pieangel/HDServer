#include "pch.h"
#include "SmCorrelationManager.h"
#include "ta_func.h"
#include <string>
#include <vector>
#include <iostream>
#include "../sklearn/preprocessing.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include <memory>
#include "../Market/SmMarketManager.h"
#include "../Market/SmMarket.h"	
#include "../Market/SmProduct.h"
#include "../Chart/SmChartData.h"
#include "../Chart/SmChartDataManager.h"
#include "../Chart/SmChartDefine.h"
#include "../Database/SmMongoDBManager.h"
#include <array>
#include "../Service/SmProtocolManager.h"
#include "../Log/loguru.hpp"

double suma(std::vector<double> a)
{
	double s = 0;
	for (size_t i = 0; i < a.size(); i++)
	{
		s = s + a[i];
	}
	return s;
}

double mean(std::vector<double> a)
{
	return suma(a) / a.size();
}



double sqsum(std::vector<double> a)
{
	double s = 0;
	for (size_t i = 0; i < a.size(); i++)
	{
		s = s + pow(a[i], 2);
	}
	return s;
}

double stdev(std::vector<double> nums)
{
	double N = nums.size();
	return pow(sqsum(nums) / N - pow(suma(nums) / N, 2), 0.5);
}



std::vector<double> operator-(std::vector<double> a, double b)
{
	std::vector<double> retvect;
	for (size_t i = 0; i < a.size(); i++)
	{
		retvect.push_back(a[i] - b);
	}
	return retvect;
}

std::vector<double> operator*(std::vector<double> a, std::vector<double> b)
{
	std::vector<double> retvect;
	for (size_t i = 0; i < a.size(); i++)
	{
		retvect.push_back(a[i] * b[i]);
	}
	return retvect;
}

double pearsoncoeff(std::vector<double> X, std::vector<double> Y)
{
	return suma((X - mean(X)) * (Y - mean(Y))) / (X.size() * stdev(X) * stdev(Y));
}

SmCorrelationManager::SmCorrelationManager()
{
	_Pool = new ThreadPool(30);
	_Pool->init();
}

SmCorrelationManager::~SmCorrelationManager()
{
	if (_Pool) {
		_Pool->shutdown();
		delete _Pool;
		_Pool = nullptr;
	}
}

int SmCorrelationManager::CheckStochastic(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return 0;
	std::vector<double> close_array = chart_data->GetClose();
	if (close_array.size() == 0)
		return 0;

	std::vector<double> high_array = chart_data->GetHigh();
	std::vector<double> low_array = chart_data->GetLow();

	// Red in the chart - buy
	std::array<double, 1500> slow_k;
	// Blue in the chart - sell
	std::array<double, 1500> slow_d;

	int begin;
	int length;

	int retCode = TA_STOCH(0, close_array.size() - 1, high_array.data(), low_array.data(), close_array.data(), 12, 5, TA_MAType::TA_MAType_SMA, 5, TA_MAType::TA_MAType_SMA, &begin, &length, slow_k.data(), slow_d.data());

	

	int result = 0;

	if (slow_k[length - 1] > slow_d[length - 1])
		result = 1;
	else if (slow_k[length - 1] < slow_d[length - 1])
		result = -1;
	else
		result = 0;

	CString msg;
	msg.Format("Stochastic symbol %s, cycle = %d, result = %d\n", chart_data->SymbolCode().c_str(), chart_data->Cycle(), result);
	TRACE(msg);

	return result;
}

int SmCorrelationManager::CheckMacd(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return 0;
	std::vector<double> close_array = chart_data->GetClose();
	if (close_array.size() == 0)
		return 0;

	std::vector<double> high_array = chart_data->GetHigh();
	std::vector<double> low_array = chart_data->GetLow();

	// Red in the chart
	std::array<double, 1500> macd;
	// Blue in the chart
	std::array<double, 1500> signal;
	std::array<double, 1500> histo;

	int begin;
	int length;

	int retCode = TA_MACD(0, close_array.size() - 1, close_array.data(), 12, 26, 9,&begin, &length, macd.data(), signal.data(), histo.data());

	int result = 0;

	if (macd[length - 1] > signal[length - 1])
		result = 1;
	else if (macd[length - 1] < signal[length - 1])
		result = -1;
	else
		result = 0;

	CString msg;
	msg.Format("macd symbol %s, cycle = %d, result = %d\n", chart_data->SymbolCode().c_str(), chart_data->Cycle(), result);
	TRACE(msg);

	return result;
}

int SmCorrelationManager::CheckSar(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return 0;
	std::vector<double> close_array = chart_data->GetClose();
	if (close_array.size() == 0)
		return 0;

	std::vector<double> high_array = chart_data->GetHigh();
	std::vector<double> low_array = chart_data->GetLow();

	// Red in the chart
	std::array<double, 1500> sar;
	

	int begin;
	int length;

	int retCode = TA_SAR(0, close_array.size() - 1, high_array.data(), low_array.data(), 0.02, 0.2, &begin, &length, sar.data());

	int result = 0;

	if (sar[length - 1] > close_array.back()) // blue in the chart
		result = -1;
	else if (sar[length - 1] < close_array.back()) // red in the chart
		result = 1;
	else
		result = 0;

	CString msg;
	msg.Format("CheckSar symbol %s, cycle = %d, result = %d\n", chart_data->SymbolCode().c_str(), chart_data->Cycle(), result);
	TRACE(msg);

	return result;
}

int SmCorrelationManager::CheckSma(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return 0;
	std::vector<double> close_array = chart_data->GetClose();
	if (close_array.size() == 0)
		return 0;

	std::vector<double> high_array = chart_data->GetHigh();
	std::vector<double> low_array = chart_data->GetLow();

	// Red in the chart
	std::array<double, 1500> out;


	int begin;
	int length;

	int retCode = TA_SMA(0, close_array.size() - 1, close_array.data(), 20, &begin, &length, out.data());

	CString msg;
	msg.Format("CheckSma symbol %s, cycle = %d\n", chart_data->SymbolCode().c_str(), chart_data->Cycle());
	TRACE(msg);

	if (out[length - 1] > close_array.back()) // blue in the chart
		return -1;
	else if (out[length - 1] < close_array.back()) // red in the chart
		return 1;
	else
		return 0;
}

int SmCorrelationManager::CheckChannel(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return 0;
	std::vector<double> close_array = chart_data->GetClose();
	if (close_array.size() == 0)
		return 0;

	std::vector<double> high_array = chart_data->GetHigh();
	std::vector<double> low_array = chart_data->GetLow();

	std::array<double, 1500> max;
	std::array<double, 1500> min;;

	int begin;
	int length;

	int retCode = TA_MAX(0, close_array.size() - 1, close_array.data(), 20, &begin, &length, max.data());
	retCode = TA_MAX(0, close_array.size() - 1, close_array.data(), 20, &begin, &length, min.data());

	double close = close_array.back();
	if (close > max[length - 1]) // red in the chart
		return 1;
	if (close < min[length - 1]) // blue in the chart
		return  -1;

	return 0;
}

int SmCorrelationManager::CheckCompositeMa(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return 0;
	std::vector<double> close_array = chart_data->GetClose();
	if (close_array.size() == 0)
		return 0;

	std::vector<double> high_array = chart_data->GetHigh();
	std::vector<double> low_array = chart_data->GetLow();

	// Red in the chart
	std::array<double, 1500> ma20;
	std::array<double, 1500> ma40;
	std::array<double, 1500> ma60;
	std::array<double, 1500> ma120;


	int begin;
	int length20;
	int length40;
	int length60;
	int length120;

	int retCode = TA_SMA(0, close_array.size() - 1, close_array.data(), 20, &begin, &length20, ma20.data());
	retCode = TA_SMA(0, close_array.size() - 1, close_array.data(), 40, &begin, &length40, ma40.data());
	retCode = TA_SMA(0, close_array.size() - 1, close_array.data(), 60, &begin, &length60, ma60.data());
	retCode = TA_SMA(0, close_array.size() - 1, close_array.data(), 120, &begin, &length120, ma120.data());

	int signal = 0;

	try {

	// 종가가 120이평보다 클 때
	if (close_array.back() > ma120[ma120.size() - 120]) {
		// 종가가 20, 40, 60 이평보다 모두 작을 때 매도
		if (close_array.back() < ma20[ma20.size() - 20] && close_array.back() < ma40[ma40.size() - 40] && close_array.back() < ma60[ma60.size() - 60]) {
			// 약매도
			signal = -1;
		}

		// 종가가 20, 40, 60 이평보다 모두 클 때 매수
		if (close_array.back() > ma20[ma20.size() - 20] && close_array.back() > ma40[ma40.size() - 40] && close_array.back() > ma60[ma60.size() - 60]) {
			// 강매수
			signal = 2;
		}

		if (close_array.back() >= ma20[ma20.size() - 20] && close_array.back() <= ma60[ma60.size() - 60]) {
			signal = 0;
		}
	}

	// 종가가 120이평보다 작거나 같을 때
	if (close_array.back() <= ma120[ma120.size() - 120]) {
		// 종가가 20, 40, 60 이평보다 모두 작을 때 매도
		if (close_array.back() < ma20[ma20.size() - 20] && close_array.back() < ma40[ma40.size() - 40] && close_array.back() < ma60[ma60.size() - 60]) {
			// 강력 매도
			signal = -2;
		}

		// 종가가 20, 40, 60 이평보다 모두 클 때 매수
		if (close_array.back() > ma20[ma20.size() - 20] && close_array.back() > ma40[ma40.size() - 40] && close_array.back() > ma60[ma60.size() - 60]) {
			// 약매수
			signal = 1;
		}

		if (close_array.back() >= ma20[ma20.size() - 20] && close_array.back() <= ma60[ma60.size() - 60]) {
			signal = 0;
		}
	}

	if ((close_array.back() >= ma20[ma20.size() - 20] && close_array.back() <= ma40[ma40.size() - 40]) ||
		(close_array.back() >= ma40[ma40.size() - 40] && close_array.back() <= ma60[ma60.size() - 60])) {
		signal = 0;
	}


	CString msg;
	msg.Format("CheckCompositeSma symbol %s, cycle = %d, result = %d\n", chart_data->SymbolCode().c_str(), chart_data->Cycle(), signal);
	TRACE(msg);

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
	
	return signal;
}

void SmCorrelationManager::RequestCorrelation(SmCorrelationRequest& req)
{
	// 쓰레드로 바로 처리해 버린다.
	auto function = std::bind(&SmCorrelationManager::ProcessCorrelation, std::ref(*this), req);
	_Pool->submit(function);
}

void SmCorrelationManager::TestLib()
{
	int startIdx = 0, endIdx = 1499;
	int outBegIdx = 0, outNbElement = 0;
		
	double input[1500];
	for (int i = 0; i < 1500; ++i) {
		input[i] = std::rand() % 50 + 1000;
	}
	double OutPut[1500];
	std::array<double, 1500> another_output;
	another_output.fill(0);
	std::memset(OutPut, 0x00, sizeof(OutPut));
	int retCode = TA_MA(startIdx,
		endIdx,
		input,
		20,
		TA_MAType::TA_MAType_SMA,
		&outBegIdx,
		&outNbElement,
		another_output.data());
	
	std::vector<std::string> categorical_data = { "GERMANY", "FRANCE", "ITALY" };
	LabelEncoder<std::string> encoder(categorical_data);
	std::vector<unsigned long int> numerical_data = encoder.fit_transorm();
	for (size_t i = 0; i < categorical_data.size(); i++)
	{
		std::cout << categorical_data[i] << " - " << numerical_data[i] << "\n";
	}


}

void SmCorrelationManager::GetSymbolList()
{
	std::vector<std::shared_ptr<SmSymbol>> col_symbol_list;
	std::vector<std::shared_ptr<SmSymbol>> row_symbol_list;
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::shared_ptr<SmMarket> market = marketMgr->FindMarket("지수");
	std::shared_ptr<SmProduct> product = market->FindProduct("YM");
	col_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("지수");
	product = market->FindProduct("SP");
	row_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("통화");
	product = market->FindProduct("RP");
	col_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("통화");
	product = market->FindProduct("6A");
	row_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("금리");
	product = market->FindProduct("ZN");
	col_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("금리");
	product = market->FindProduct("ZF");
	row_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("에너지");
	product = market->FindProduct("CL");
	col_symbol_list.push_back(product->GetRecentMonthSymbol());
	market = marketMgr->FindMarket("에너지");
	product = market->FindProduct("NG");
	row_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("금속");
	product = market->FindProduct("SI");
	col_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("금속");
	product = market->FindProduct("GC");
	row_symbol_list.push_back(product->GetRecentMonthSymbol());

	market = marketMgr->FindMarket("농산물");
	product = market->FindProduct("ZC");
	col_symbol_list.push_back(product->GetRecentMonthSymbol());
	market = marketMgr->FindMarket("농산물");
	product = market->FindProduct("ZW");
	row_symbol_list.push_back(product->GetRecentMonthSymbol());

	size_t min_size = 300;
	for (size_t i = 0; i < col_symbol_list.size(); ++i) {
		std::shared_ptr<SmSymbol> symbol = col_symbol_list[i];
		// chart type : day, cycle : 1
		SmChartDataRequest req;
		req.symbolCode = symbol->SymbolCode();
		req.chartType = SmChartType::DAY;
		req.cycle = 1;
		req.count = 108;
		req.product_code = symbol->CategoryCode();
		std::vector<double> data = SmMongoDBManager::GetInstance()->GetDailyData(req);
		// 표준화 한다.
		data = preprocessing::normalize(data);
		if (data.size() < min_size)
			min_size = data.size();
		_ColValues.push_back(data);
	}

	
	for (size_t i = 0; i < row_symbol_list.size(); ++i) {
		std::shared_ptr<SmSymbol> symbol = row_symbol_list[i];
		// chart type : day, cycle : 1
		SmChartDataRequest req;
		req.symbolCode = symbol->SymbolCode();
		req.chartType = SmChartType::DAY;
		req.cycle = 1;
		req.count = 108;
		req.product_code = symbol->CategoryCode();
		std::vector<double> data = SmMongoDBManager::GetInstance()->GetDailyData(req);
		// 표준화 한다.
		data = preprocessing::normalize(data);
		if (data.size() < min_size)
			min_size = data.size();
		_RowValues.push_back(data);
	}	

	for (size_t i = 0; i < _ColValues.size(); ++i) {
		for (size_t j = 0; j < _RowValues.size(); ++j) {
			double cor = pearsoncoeff(_ColValues[i], _RowValues[j]);
			_CorMap[std::make_pair(i, j)] = cor;
		}
	}
}

void SmCorrelationManager::ProcessCorrelation(SmCorrelationRequest& req_cor)
{
	SmChartDataRequest req;
	req.chartType = SmChartType::DAY;
	req.cycle = 1;
	req.count = 108;
	req.product_code = req_cor.symbol_code_col;
	std::vector<double> col_data = SmMongoDBManager::GetInstance()->GetDailyData(req);
	if (col_data.size() == 0) {
		SendResult(0.0f, req_cor);
		return;
	}

	req.chartType = SmChartType::DAY;
	req.cycle = 1;
	req.count = 108;
	req.product_code = req_cor.symbol_code_row;
	std::vector<double> row_data = SmMongoDBManager::GetInstance()->GetDailyData(req);
	if (row_data.size() == 0) {
		SendResult(0.0f, req_cor);
		return;
	}

	// 표준화 한다.
	col_data = preprocessing::normalize(col_data);
	// 표준화 한다.
	row_data = preprocessing::normalize(row_data);

	double cor = pearsoncoeff(col_data, row_data);

	SendResult(cor, req_cor);
}

void SmCorrelationManager::SendResult(double result, SmCorrelationRequest& req_cor)
{
	SmProtocolManager::GetInstance()->SendCorrelationResult(result, req_cor);
}



