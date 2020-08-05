#include "pch.h"
#include "SmChartDataManager.h"
#include "SmChartData.h"
#include "../Service/SmTimeSeriesServiceManager.h"
#include "../Database/SmMongoDBManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "../HDCtrl/HdClient.h"

SmChartDataManager::SmChartDataManager()
{
	_BatchProcess = false;
	_Available = true;
	StartTaskThread();

	//_Pool = new ThreadPool(30);
	//_Pool->init();
}

SmChartDataManager::~SmChartDataManager()
{
	// 쓰레드를 종료 하기 전에 반드시 큐에 들어오는 것을 막아야 한다.
	// 만일 큐에 들어오는 것을 막지 않고 쓰레드를 종료하면 절대로 쓰레드가 종료되지 않는다.
	StopThread();

// 	if (_Pool) {
// 		_Pool->shutdown();
// 		delete _Pool;
// 		_Pool = nullptr;
// 	}
}

void SmChartDataManager::AddChartData(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return;

	auto it = _ChartDataMap.find(chart_data->GetDataKey());
	if (it == _ChartDataMap.end()) {
		_ChartDataMap[chart_data->GetDataKey()] = chart_data;
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(chart_data->SymbolCode());
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(chart_data->GetDataKey(), chart_data);
		}
	}
}

std::shared_ptr<SmChartData> SmChartDataManager::AddChartData(SmChartDataRequest data_req)
{
	std::shared_ptr<SmChartData> chartData = FindChartData(data_req.GetDataKey());
	if (!chartData) {
		chartData = std::make_shared<SmChartData>();
		chartData->SymbolCode(data_req.symbolCode);
		chartData->ChartType(data_req.chartType);
		chartData->Cycle(data_req.cycle);
		_ChartDataMap[data_req.GetDataKey()] = chartData;
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(data_req.symbolCode);
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(data_req.GetDataKey(), chartData);
		}
	}

	return chartData;
}

std::shared_ptr<SmChartData> SmChartDataManager::AddChartData(SmChartDataItem data_item)
{
	std::shared_ptr<SmChartData> chartData = FindChartData(data_item.GetDataKey());
	if (!chartData) {
		chartData = std::make_shared<SmChartData>();
		chartData->SymbolCode(data_item.symbolCode);
		chartData->ChartType(data_item.chartType);
		chartData->Cycle(data_item.cycle);
		_ChartDataMap[data_item.GetDataKey()] = chartData;

		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(data_item.symbolCode);
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(data_item.GetDataKey(), chartData);
		}
	}

	return chartData;
}

std::shared_ptr<SmChartData> SmChartDataManager::AddChartData(std::string symbol_code, int chart_type, int cycle)
{
	std::string data_key = SmChartData::MakeDataKey(symbol_code, chart_type, cycle);
	std::shared_ptr<SmChartData> chartData = FindChartData(data_key);
	if (!chartData) {
		chartData = std::make_shared<SmChartData>();
		chartData->SymbolCode(symbol_code);
		chartData->ChartType((SmChartType)chart_type);
		chartData->Cycle(cycle);
		_ChartDataMap[data_key] = chartData;

		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(symbol_code);
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(data_key, chartData);
		}
	}

	return chartData;
}

std::shared_ptr<SmChartData> SmChartDataManager::AddChartData(SmChartDataItem&& data_item)
{
	std::shared_ptr<SmChartData> chartData = FindChartData(data_item.GetDataKey());
	if (!chartData) {
		chartData = std::make_shared<SmChartData>();
		chartData->SymbolCode(data_item.symbolCode);
		chartData->ChartType(data_item.chartType);
		chartData->Cycle(data_item.cycle);
		_ChartDataMap[data_item.GetDataKey()] = chartData;

		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(data_item.symbolCode);
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(data_item.GetDataKey(), chartData);
		}
	}

	return chartData;
}

std::shared_ptr<SmChartData> SmChartDataManager::FindChartData(std::string data_key)
{
	auto it = _ChartDataMap.find(data_key);
	if (it != _ChartDataMap.end()) {
		return it->second;
	}

	return nullptr;
}


void SmChartDataManager::StartTaskThread()
{
	_TaskThread = std::thread(&SmChartDataManager::ProcessTask, this);
}

void SmChartDataManager::ClearTasks()
{
	_TaskQueue.flush();
}

void SmChartDataManager::AddTask(SmChartDataRequest&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;

	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_TaskQueue.try_add(item);
}

void SmChartDataManager::StopThread()
{
	// 더이상 입력을 받지 않게 한다.
	_Enable = false;
	// 먼저 작업 목록을 비운다.
	ClearTasks();
	SmChartDataRequest arg;
	// 끝내라는 명령
	arg.command = 0;
	std::thread::id id = _TaskThread.get_id();
	_TaskQueue.add(arg);
	_Available = true;

	// 쓰레드가 끝날때까지 기다린다.
	if (_TaskThread.joinable())
		_TaskThread.join();
}

void SmChartDataManager::RequestChartData(SmChartDataRequest&& req)
{
	// 쓰레드로 바로 처리해 버린다.
	//auto function = std::bind(&SmChartDataManager::ProcessChartData, std::ref(*this), req);
	//_Pool->submit(function);
	ProcessChartData(req);
}

void SmChartDataManager::ProcessTask()
{
	while (true) {
		if (!_Available) { continue; }
		_Available = false;


		if (_BatchProcess) {
			std::array<SmChartDataRequest, ChartArraySize> arr;

			size_t taken;

			auto status = _TaskQueue.take_bulk(arr.begin(), arr.size(), taken);
			if (status == BlockingCollectionStatus::Ok) {

				// 종료 명령이 아닐때는 데이터를 한꺼번에 처리한다.
				bool result = ExecuteTask(arr, taken);
				// 종료 명령일 때는 쓰레드를 종료 한다.
				if (!result)
					break;
			}
		}
		else {
			SmChartDataRequest data;
			auto status = _TaskQueue.take(data);
			// 상태가 좋으면 데이터를 처리한다.
			if (status == BlockingCollectionStatus::Ok) {
				// 종료 명령일 때는 쓰레드를 종료한다.
				if (data.command == 0)
					break;
				// 종료 명령이 아닐때는 데이터를 처리한다.
				ExecuteTask(std::move(data));
			}
		}

		_Available = true;
	}
}

void SmChartDataManager::ExecuteTask(SmChartDataRequest&& req)
{
	std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(req.symbolCode, (int)req.chartType, req.cycle);
	size_t data_count = chart_data->GetDataCount();
	// 데이터가 없으면 증권사 서버에 요청을 한다.
	if (data_count == 0) {
		SmMongoDBManager::GetInstance()->SaveChartDataRequest(req);
		// 데이터베이스에서 차트데이터를 보낸다.
		SmMongoDBManager::GetInstance()->LoadChartData(req);
		// 여기서 차트 사이클 데이터를 요청한다.
		SmTimeSeriesServiceManager::GetInstance()->ResendChartCycleDataRequest(req);
	}
	else { // 데이터가 있으면 바로 보낸다.
		SmTimeSeriesServiceManager::GetInstance()->SendChartData(req.session_id, chart_data);
	}
}

bool SmChartDataManager::ExecuteTask(std::array<SmChartDataRequest, ChartArraySize>& arr, int taken)
{
	std::map<std::string, SmChartDataRequest> request_set;
	for (int i = 0; i < taken; ++i) {
		SmChartDataRequest item = arr.at(i);

		// 종료 명령이면 거짓을 반환한다.
		if (item.command == 0)
			return false;

		std::string data_key = item.GetDataKey();
		request_set[data_key] = item;
	}

	for (auto it = request_set.begin(); it != request_set.end(); ++it) {
		SmChartDataRequest item = it->second;
		//SmHdClient::GetInstance()->GetChartData(item);
		//std::this_thread::sleep_for(std::chrono::milliseconds(700));
	}

	return true;
}

void SmChartDataManager::ProcessChartData(SmChartDataRequest& req)
{
	// 일데이터는 상품 코드를 따로 필요로 한다.
	if (req.chartType == SmChartType::DAY) {
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(req.symbolCode);
		if (symbol) {
			req.product_code = symbol->CategoryCode();
		}
	}
	// 차트데이터 등록해 준다.
	std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(req.symbolCode, (int)req.chartType, req.cycle);
	// 데이터가 없으면 데이터베이스에 요청을 한다.
	if (!chart_data->Received()) {
		SmMongoDBManager::GetInstance()->SaveChartDataRequest(req);
		// 데이터베이스에서 차트데이터를 보낸다.
		SmMongoDBManager::GetInstance()->LoadChartData(req);
		// 여기서 차트 사이클 데이터를 요청한다.
		SmTimeSeriesServiceManager::GetInstance()->ResendChartCycleDataRequest(req);
	}
	else { // 데이터가 있으면 바로 보낸다.
		SmTimeSeriesServiceManager::GetInstance()->SendChartData(req.session_id, chart_data);
	}
}

