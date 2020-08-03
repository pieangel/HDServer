#include "pch.h"
#include "SmRealtimeQuoteManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "SmCallbackManager.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "../Order/VtOrderManager.h"
#include <array>
#include <set>
#include "../Chart/SmChartDataManager.h"
#include "../Log/loguru.hpp"
#include "../Server/SmSessionManager.h"

SmRealtimeQuoteManager::SmRealtimeQuoteManager()
{
	_BatchProcess = true;
	_Available = true;
	StartTaskThread();
}


SmRealtimeQuoteManager::~SmRealtimeQuoteManager()
{
	StopThread();
}

void SmRealtimeQuoteManager::StartTaskThread()
{
	//_TaskThread = std::thread(&SmRealtimeQuoteManager::ProcessTask, this);

	_FutTaskThread = std::thread(&SmRealtimeQuoteManager::ProcessFutTask, this);
	_OptTaskThread = std::thread(&SmRealtimeQuoteManager::ProcessOptTask, this);
	_ProTaskThread = std::thread(&SmRealtimeQuoteManager::ProcessProTask, this);
}

void SmRealtimeQuoteManager::ClearTasks()
{
	//_TaskQueue.flush();
	_FutTaskQueue.flush();
	_OptTaskQueue.flush();
	_ProTaskQueue.flush();
}

void SmRealtimeQuoteManager::AddTask(SmQuoteData&& item)
{
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;
	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_TaskQueue.try_add(item);
}

void SmRealtimeQuoteManager::StopThread()
{
	// ���� �۾� ����� ����.
	ClearTasks();
	SmQuoteData arg;
	// ������� ���
	arg.command = 0;
	_FutTaskQueue.add(arg);
	_FutAvailable = true;

	// �����尡 ���������� ��ٸ���.
	if (_FutTaskThread.joinable())
		_FutTaskThread.join();

	_OptTaskQueue.add(arg);
	_OptAvailable = true;

	// �����尡 ���������� ��ٸ���.
	if (_OptTaskThread.joinable())
		_OptTaskThread.join();

	_ProTaskQueue.add(arg);
	_ProAvailable = true;

	// �����尡 ���������� ��ٸ���.
	if (_ProTaskThread.joinable())
		_ProTaskThread.join();
}

void SmRealtimeQuoteManager::AddFutTask(SmQuoteData&& item)
{
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;
	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_FutTaskQueue.try_add(item);
}

void SmRealtimeQuoteManager::AddOptTask(SmQuoteData&& item)
{
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;
	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_OptTaskQueue.try_add(item);
}

void SmRealtimeQuoteManager::AddProTask(SmQuoteData&& item)
{
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;
	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_ProTaskQueue.try_add(item);
}

void SmRealtimeQuoteManager::ProcessTask()
{
	while (true) {
		if (!_Available)
			continue;

		_Available = false;
		
		if (_BatchProcess) {
			std::array<SmQuoteData, QuoteArraySize> arr;

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
			SmQuoteData data;
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

void SmRealtimeQuoteManager::ExecuteTask(SmQuoteData&& item)
{
	try {

	// ���� ����̸� ��ȯ�Ѵ�.
	if (item.command == 0)
		return;

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(item.symbol_code);
	if (!sym)
		return;
	// ���⼭ ��Ʈ�����͸� ������Ʈ �Ѵ�.
	sym->UpdateMinData(item);
	sym->UpdateTickData(item);
	sym->UpdateChartData(item);

	sym->Quote.Close = std::stoi(item.close);
	sym->Quote.Open = std::stoi(item.open);
	sym->Quote.High = std::stoi(item.high);
	sym->Quote.Low = std::stoi(item.low);
	sym->Quote.OriginTime = item.time;
	sym->Quote.GapFromPreDay = std::stoi(item.to_preday);
	sym->Quote.RatioToPreday = std::stoi(item.up_down_rate);
	sym->Quote.SignToPreDay = std::stoi(item.up_down);
	sym->Quote.accVolume = std::stoi(item.acc_vol);
	

	// ���õ� �����쿡 �޽����� ������.
	SmCallbackManager::GetInstance()->OnWndQuoteEvent(sym.get());

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s �˼����� ����"), __FUNCTION__);
	}
}

bool SmRealtimeQuoteManager::ExecuteTask(std::array<SmQuoteData, QuoteArraySize>& arr, int taken)
{
	try {
		//LOG_F(INFO, _T("taken = %d"), taken);
	std::set<std::shared_ptr<SmSymbol>> symbol_set;
	for (int i = 0; i < taken; ++i) {
		SmQuoteData item = arr.at(i);

		// ���� ����̸� ������ ��ȯ�Ѵ�.
		if (item.command == 0)
			return false;

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(item.symbol_code);
		if (!sym)
			continue;

		// ���⼭ ��Ʈ�����͸� ������Ʈ �Ѵ�.
		sym->UpdateMinData(item);
		sym->UpdateTickData(item);
		sym->UpdateChartData(item);
		// ���տ� �ɺ��� �߰��Ѵ�.
		symbol_set.insert(sym);
		sym->Quote.Close = std::stoi(item.close);
		sym->Quote.Open = std::stoi(item.open);
		sym->Quote.High = std::stoi(item.high);
		sym->Quote.Low = std::stoi(item.low);
		sym->Quote.OriginTime = item.time;
 		sym->Quote.GapFromPreDay = std::stoi(item.to_preday);
 		sym->Quote.RatioToPreday = (item.up_down_rate);
 		sym->Quote.SignToPreDay = (item.up_down);
 		sym->Quote.accVolume = std::stoi(item.acc_vol);

		

		CString msg;
		msg.Format(_T(" OnFutureHoga symbol_code = %s, time = %s\n"), item.symbol_code.c_str(), item.acc_vol.c_str());
		TRACE(msg);

		// ����, ����, ��ž �ֹ��� üũ�Ѵ�.
		SmCallbackManager::GetInstance()->OnQuoteEvent(sym.get());
	}

	// â�� �Ѳ����� ������Ʈ �Ѵ�.
	for (auto it = symbol_set.begin(); it != symbol_set.end(); ++it) {
		std::shared_ptr<SmSymbol> sym = *it;

		// ���õ� �����쿡 �޽����� ������.
		SmCallbackManager::GetInstance()->OnWndQuoteEvent(sym.get());
		//SmSessionManager::GetInstance()->SendReqUpdateQuote(sym);
	}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s �˼����� ����"), __FUNCTION__);
	}

	return true;
}

void SmRealtimeQuoteManager::ProcessFutTask()
{
	while (true) {
		if (!_FutAvailable)
			continue;

		_FutAvailable = false;

		if (_BatchProcess) {
			std::array<SmQuoteData, QuoteArraySize> arr;

			size_t taken;

			auto status = _FutTaskQueue.take_bulk(arr.begin(), arr.size(), taken);
			if (status == BlockingCollectionStatus::Ok) {

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmQuoteData data;
			auto status = _FutTaskQueue.take(data);
			// ���°� ������ �����͸� ó���Ѵ�.
			if (status == BlockingCollectionStatus::Ok) {
				// ���� ����� ���� �����带 �����Ѵ�.
				if (data.command == 0)
					break;
				// ���� ����� �ƴҶ��� �����͸� ó���Ѵ�.
				ExecuteTask(std::move(data));
			}
		}

		_FutAvailable = true;
	}
}

void SmRealtimeQuoteManager::ProcessOptTask()
{
	while (true) {
		if (!_OptAvailable)
			continue;

		_OptAvailable = false;

		if (_BatchProcess) {
			std::array<SmQuoteData, QuoteArraySize> arr;

			size_t taken;

			auto status = _OptTaskQueue.take_bulk(arr.begin(), arr.size(), taken);
			if (status == BlockingCollectionStatus::Ok) {

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmQuoteData data;
			auto status = _OptTaskQueue.take(data);
			// ���°� ������ �����͸� ó���Ѵ�.
			if (status == BlockingCollectionStatus::Ok) {
				// ���� ����� ���� �����带 �����Ѵ�.
				if (data.command == 0)
					break;
				// ���� ����� �ƴҶ��� �����͸� ó���Ѵ�.
				ExecuteTask(std::move(data));
			}
		}

		_OptAvailable = true;
	}
}

void SmRealtimeQuoteManager::ProcessProTask()
{
	while (true) {
		if (!_ProAvailable)
			continue;

		_ProAvailable = false;

		if (_BatchProcess) {
			std::array<SmQuoteData, QuoteArraySize> arr;

			size_t taken;

			auto status = _ProTaskQueue.take_bulk(arr.begin(), arr.size(), taken);
			if (status == BlockingCollectionStatus::Ok) {

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmQuoteData data;
			auto status = _ProTaskQueue.take(data);
			// ���°� ������ �����͸� ó���Ѵ�.
			if (status == BlockingCollectionStatus::Ok) {
				// ���� ����� ���� �����带 �����Ѵ�.
				if (data.command == 0)
					break;
				// ���� ����� �ƴҶ��� �����͸� ó���Ѵ�.
				ExecuteTask(std::move(data));
			}
		}

		_ProAvailable = true;
	}
}
