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
#include "../Util/SmUtil.h"
#include <chrono>
using namespace std::chrono;

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
	// �����带 ���� �ϱ� ���� �ݵ�� ť�� ������ ���� ���ƾ� �Ѵ�.
	// ���� ť�� ������ ���� ���� �ʰ� �����带 �����ϸ� ����� �����尡 ������� �ʴ´�.
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
		//CreateTimer(chart_data);
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(chart_data->SymbolCode());
		// �ɺ��� ��Ʈ �����͸� �߰��� �ش�. 
		// ���� ������Ʈ�� ���ؼ� �� �ʿ��ϴ�.
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
		//CreateTimer(chartData);
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(data_req.symbolCode);
		// �ɺ��� ��Ʈ �����͸� �߰��� �ش�. 
		// ���� ������Ʈ�� ���ؼ� �� �ʿ��ϴ�.
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
		//CreateTimer(chartData);
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(data_item.symbolCode);
		// �ɺ��� ��Ʈ �����͸� �߰��� �ش�. 
		// ���� ������Ʈ�� ���ؼ� �� �ʿ��ϴ�.
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
		//CreateTimer(chartData);
		_ChartDataMap[data_key] = chartData;

		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(symbol_code);
		// �ɺ��� ��Ʈ �����͸� �߰��� �ش�. 
		// ���� ������Ʈ�� ���ؼ� �� �ʿ��ϴ�.
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
		// �ɺ��� ��Ʈ �����͸� �߰��� �ش�. 
		// ���� ������Ʈ�� ���ؼ� �� �ʿ��ϴ�.
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
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;

	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_TaskQueue.try_add(item);
}

void SmChartDataManager::StopThread()
{
	// ���̻� �Է��� ���� �ʰ� �Ѵ�.
	_Enable = false;
	// ���� �۾� ����� ����.
	ClearTasks();
	SmChartDataRequest arg;
	// ������� ���
	arg.command = 0;
	std::thread::id id = _TaskThread.get_id();
	_TaskQueue.add(arg);
	_Available = true;

	// �����尡 ���������� ��ٸ���.
	if (_TaskThread.joinable())
		_TaskThread.join();
}

void SmChartDataManager::RequestChartData(SmChartDataRequest&& req)
{
	AddTask(std::move(req));
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

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmChartDataRequest data;
			auto status = _TaskQueue.take(data);
			// ���°� ������ �����͸� ó���Ѵ�.
			if (status == BlockingCollectionStatus::Ok) {
				// ���� ����� ���� �����带 �����Ѵ�.
				if (data.command == 0)
					break;
				// ���� ����� �ƴҶ��� �����͸� ó���Ѵ�.
				ExecuteTask(std::move(data));
			}
		}

		_Available = true;
	}
}

void SmChartDataManager::ExecuteTask(SmChartDataRequest&& req)
{
	// �ϵ����ʹ� ��ǰ �ڵ带 ���� �ʿ�� �Ѵ�.
	if (req.chartType == SmChartType::DAY) {
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(req.symbolCode);
		if (symbol) {
			req.product_code = symbol->CategoryCode();
		}
	}

	CString msg;
	msg.Format("��Ʈ������ ��û symbol_code = %s, chart_type = %d, cycle = %d\n", req.symbolCode.c_str(), (int)req.chartType, req.cycle);
	TRACE(msg);
	// ��Ʈ������ ����� �ش�.
	std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(req.symbolCode, (int)req.chartType, req.cycle);
	// �����Ͱ� ������ ������ ��û�� �Ѵ�. �׸��� ����Ŭ �������� ��� ������ ��û�� �Ѵ�.
	if (!chart_data->Received() || req.reqType == SmChartDataReqestType::CYCLE) {
		HdClient::GetInstance()->GetChartData(req);
		std::this_thread::sleep_for(std::chrono::milliseconds(400));
	}
	else { // �����Ͱ� ������ �ٷ� ������.
		chart_data->SendChartData(req.session_id);
	}
}

bool SmChartDataManager::ExecuteTask(std::array<SmChartDataRequest, ChartArraySize>& arr, int taken)
{
	std::map<std::string, SmChartDataRequest> request_set;
	for (int i = 0; i < taken; ++i) {
		SmChartDataRequest item = arr.at(i);

		// ���� ����̸� ������ ��ȯ�Ѵ�.
		if (item.command == 0)
			return false;

		std::string data_key = item.GetDataKey();
		request_set[data_key] = item;
	}

	for (auto it = request_set.begin(); it != request_set.end(); ++it) {
		SmChartDataRequest item = it->second;
		HdClient::GetInstance()->GetChartData(item);
		std::this_thread::sleep_for(std::chrono::milliseconds(700));
	}

	return true;
}

void SmChartDataManager::ProcessChartData(SmChartDataRequest& req)
{
	// �ϵ����ʹ� ��ǰ �ڵ带 ���� �ʿ�� �Ѵ�.
	if (req.chartType == SmChartType::DAY) {
		std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(req.symbolCode);
		if (symbol) {
			req.product_code = symbol->CategoryCode();
		}
	}
	// ��Ʈ������ ����� �ش�.
	std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(req.symbolCode, (int)req.chartType, req.cycle);
	// �����Ͱ� ������ ������ ��û�� �Ѵ�. �׸��� ����Ŭ �������� ��� ������ ��û�� �Ѵ�.
	if (!chart_data->Received() || req.reqType == SmChartDataReqestType::CYCLE) {
		HdClient::GetInstance()->GetChartData(req);
	}
	else { // �����Ͱ� ������ �ٷ� ������.
		chart_data->SendChartData(req.session_id);
	}
}

void SmChartDataManager::CreateTimer(std::shared_ptr<SmChartData> chart_data)
{
	auto it = _TimerMap.find(chart_data->GetDataKey());
	if (it != _TimerMap.end())
		return;

	// ƽ�����ʹ� ó������ �ʴ´�. 
	if (chart_data->ChartType() == SmChartType::TICK)
		return;

	std::vector<int> date_time = SmUtil::GetLocalDateTime();
	int minMod = date_time[4] % chart_data->Cycle();
	int waitTime = chart_data->Cycle() * 60 - (minMod * 60 + date_time[5]);
	// Add to the timer.
	auto id = _Timer.add(seconds(waitTime), std::bind(&SmChartData::OnTimer, chart_data), seconds(chart_data->Cycle() * 60));
	// Add to the request map.
	_TimerMap[chart_data->GetDataKey()] = id;
}

