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
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;
	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_TaskQueue.try_add(item);
}

void SmRealtimeQuoteManager::StopThread()
{
	// 먼저 작업 목록을 비운다.
	ClearTasks();
	SmQuoteData arg;
	// 끝내라는 명령
	arg.command = 0;
	_FutTaskQueue.add(arg);
	_FutAvailable = true;

	// 쓰레드가 끝날때까지 기다린다.
	if (_FutTaskThread.joinable())
		_FutTaskThread.join();

	_OptTaskQueue.add(arg);
	_OptAvailable = true;

	// 쓰레드가 끝날때까지 기다린다.
	if (_OptTaskThread.joinable())
		_OptTaskThread.join();

	_ProTaskQueue.add(arg);
	_ProAvailable = true;

	// 쓰레드가 끝날때까지 기다린다.
	if (_ProTaskThread.joinable())
		_ProTaskThread.join();
}

void SmRealtimeQuoteManager::AddFutTask(SmQuoteData&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;
	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_FutTaskQueue.try_add(item);
}

void SmRealtimeQuoteManager::AddOptTask(SmQuoteData&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;
	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_OptTaskQueue.try_add(item);
}

void SmRealtimeQuoteManager::AddProTask(SmQuoteData&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;
	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
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

				// 종료 명령이 아닐때는 데이터를 한꺼번에 처리한다.
				bool result = ExecuteTask(arr, taken);
				// 종료 명령일 때는 쓰레드를 종료 한다.
				if (!result)
					break;
			}
		}
		else {
			SmQuoteData data;
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

void SmRealtimeQuoteManager::ExecuteTask(SmQuoteData&& item)
{
	try {

	// 종료 명령이면 반환한다.
	if (item.command == 0)
		return;

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(item.symbol_code);
	if (!sym)
		return;
	// 여기서 차트데이터를 업데이트 한다.
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
	

	// 관련된 윈도우에 메시지를 보낸다.
	SmCallbackManager::GetInstance()->OnWndQuoteEvent(sym.get());

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

bool SmRealtimeQuoteManager::ExecuteTask(std::array<SmQuoteData, QuoteArraySize>& arr, int taken)
{
	try {
		//LOG_F(INFO, _T("taken = %d"), taken);
	std::set<std::shared_ptr<SmSymbol>> symbol_set;
	for (int i = 0; i < taken; ++i) {
		SmQuoteData item = arr.at(i);

		// 종료 명령이면 거짓을 반환한다.
		if (item.command == 0)
			return false;

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(item.symbol_code);
		if (!sym)
			continue;

		// 여기서 차트데이터를 업데이트 한다.
		sym->UpdateMinData(item);
		sym->UpdateTickData(item);
		sym->UpdateChartData(item);
		// 집합에 심볼을 추가한다.
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

		// 손절, 익절, 스탑 주문을 체크한다.
		SmCallbackManager::GetInstance()->OnQuoteEvent(sym.get());
	}

	// 창은 한꺼번에 업데이트 한다.
	for (auto it = symbol_set.begin(); it != symbol_set.end(); ++it) {
		std::shared_ptr<SmSymbol> sym = *it;

		// 관련된 윈도우에 메시지를 보낸다.
		SmCallbackManager::GetInstance()->OnWndQuoteEvent(sym.get());
		//SmSessionManager::GetInstance()->SendReqUpdateQuote(sym);
	}

	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
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

				// 종료 명령이 아닐때는 데이터를 한꺼번에 처리한다.
				bool result = ExecuteTask(arr, taken);
				// 종료 명령일 때는 쓰레드를 종료 한다.
				if (!result)
					break;
			}
		}
		else {
			SmQuoteData data;
			auto status = _FutTaskQueue.take(data);
			// 상태가 좋으면 데이터를 처리한다.
			if (status == BlockingCollectionStatus::Ok) {
				// 종료 명령일 때는 쓰레드를 종료한다.
				if (data.command == 0)
					break;
				// 종료 명령이 아닐때는 데이터를 처리한다.
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

				// 종료 명령이 아닐때는 데이터를 한꺼번에 처리한다.
				bool result = ExecuteTask(arr, taken);
				// 종료 명령일 때는 쓰레드를 종료 한다.
				if (!result)
					break;
			}
		}
		else {
			SmQuoteData data;
			auto status = _OptTaskQueue.take(data);
			// 상태가 좋으면 데이터를 처리한다.
			if (status == BlockingCollectionStatus::Ok) {
				// 종료 명령일 때는 쓰레드를 종료한다.
				if (data.command == 0)
					break;
				// 종료 명령이 아닐때는 데이터를 처리한다.
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

				// 종료 명령이 아닐때는 데이터를 한꺼번에 처리한다.
				bool result = ExecuteTask(arr, taken);
				// 종료 명령일 때는 쓰레드를 종료 한다.
				if (!result)
					break;
			}
		}
		else {
			SmQuoteData data;
			auto status = _ProTaskQueue.take(data);
			// 상태가 좋으면 데이터를 처리한다.
			if (status == BlockingCollectionStatus::Ok) {
				// 종료 명령일 때는 쓰레드를 종료한다.
				if (data.command == 0)
					break;
				// 종료 명령이 아닐때는 데이터를 처리한다.
				ExecuteTask(std::move(data));
			}
		}

		_ProAvailable = true;
	}
}
