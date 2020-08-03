#include "pch.h"
#include "SmRealtimeHogaManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "SmCallbackManager.h"
#include "../Log/loguru.hpp"
//#include "../Network/SmSessionManager.h"

SmRealtimeHogaManager::SmRealtimeHogaManager()
{
	_BatchProcess = true;
	_Available = true;
	StartTaskThread();
}


SmRealtimeHogaManager::~SmRealtimeHogaManager()
{
	// 쓰레드를 종료 하기 전에 반드시 큐에 들어오는 것을 막아야 한다.
	// 만일 큐에 들어오는 것을 막지 않고 쓰레드를 종료하면 절대로 쓰레드가 종료되지 않는다.
	StopThread();
}

void SmRealtimeHogaManager::StartTaskThread()
{
	_FutTaskThread = std::thread(&SmRealtimeHogaManager::ProcessFutTask, this);
	_OptTaskThread = std::thread(&SmRealtimeHogaManager::ProcessOptTask, this);
	_ProTaskThread = std::thread(&SmRealtimeHogaManager::ProcessProTask, this);
}

void SmRealtimeHogaManager::ClearTasks()
{
	//_TaskQueue.flush();
	_FutTaskQueue.flush();
	_OptTaskQueue.flush();
	_ProTaskQueue.flush();
}

void SmRealtimeHogaManager::AddTask(SmHogaItem&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;

	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_TaskQueue.try_add(item);
}

void SmRealtimeHogaManager::AddFutTask(SmHogaItem&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;

	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_FutTaskQueue.try_add(item);
}

void SmRealtimeHogaManager::AddOptTask(SmHogaItem&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;

	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_OptTaskQueue.try_add(item);
}

void SmRealtimeHogaManager::AddProTask(SmHogaItem&& item)
{
	// 비활성화 되면 수집하지 않는다.
	if (!_Enable)
		return;

	// 추가하고 즉시 반환한다. 혹시 큐가 이용할수 없는 상황이라도 반환한다.
	// 반드시 추가하는 경우에는 add() 함수를 이용한다.
	_ProTaskQueue.try_add(item);
}

void SmRealtimeHogaManager::StopThread()
{
	// 먼저 작업 목록을 비운다.
	ClearTasks();
	SmHogaItem arg;
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

void SmRealtimeHogaManager::ProcessTask()
{
	while (true) {
		if (!_Available) { continue; }
		_Available = false;

		
		if (_BatchProcess) {
			std::array<SmHogaItem, HogaArraySize> arr;

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
			SmHogaItem data;
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

void SmRealtimeHogaManager::ProcessFutTask()
{
	while (true) {
		if (!_FutAvailable) { continue; }
		_FutAvailable = false;


		if (_BatchProcess) {
			std::array<SmHogaItem, HogaArraySize> arr;

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
			SmHogaItem data;
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

void SmRealtimeHogaManager::ProcessOptTask()
{
	while (true) {
		if (!_OptAvailable) { continue; }
		_OptAvailable = false;


		if (_BatchProcess) {
			std::array<SmHogaItem, HogaArraySize> arr;

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
			SmHogaItem data;
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

void SmRealtimeHogaManager::ProcessProTask()
{
	while (true) {
		if (!_ProAvailable) { continue; }
		_ProAvailable = false;


		if (_BatchProcess) {
			std::array<SmHogaItem, HogaArraySize> arr;

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
			SmHogaItem data;
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

void SmRealtimeHogaManager::ExecuteTask(SmHogaItem&& item)
{
	try {
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(item.symbol_code);
		if (!sym)
			return;

		if (item.sell_price1.length() > 0)
			sym->Hoga.Ary[0].SellPrice = std::stoi(item.sell_price1);
		else
			sym->Hoga.Ary[0].SellPrice = 0;

		if (item.buy_price1.length() > 0)
			sym->Hoga.Ary[0].BuyPrice = std::stoi(item.buy_price1);
		else
			sym->Hoga.Ary[0].BuyPrice = 0;
		sym->Hoga.Ary[0].SellQty = std::stoi(item.sell_qty1);
		sym->Hoga.Ary[0].BuyQty = std::stoi(item.buy_qty1);
		sym->Hoga.Ary[0].SellCnt = std::stoi(item.sell_count1);
		sym->Hoga.Ary[0].BuyCnt = std::stoi(item.buy_count1);


		if (item.sell_price2.length() > 0)
			sym->Hoga.Ary[1].SellPrice = std::stoi(item.sell_price2);
		else
			sym->Hoga.Ary[1].SellPrice = 0;

		if (item.buy_price2.length() > 0)
			sym->Hoga.Ary[1].BuyPrice = std::stoi(item.buy_price2);
		else
			sym->Hoga.Ary[0].BuyPrice = 0;
		sym->Hoga.Ary[1].SellQty = std::stoi(item.sell_qty2);
		sym->Hoga.Ary[1].BuyQty = std::stoi(item.buy_qty2);
		sym->Hoga.Ary[1].SellCnt = std::stoi(item.sell_count2);
		sym->Hoga.Ary[1].BuyCnt = std::stoi(item.buy_count2);


		if (item.sell_price3.length() > 0)
			sym->Hoga.Ary[2].SellPrice = std::stoi(item.sell_price3);
		else
			sym->Hoga.Ary[2].SellPrice = 0;

		if (item.buy_price3.length() > 0)
			sym->Hoga.Ary[2].BuyPrice = std::stoi(item.buy_price3);
		else
			sym->Hoga.Ary[2].BuyPrice = 0;
		sym->Hoga.Ary[2].SellQty = std::stoi(item.sell_qty3);
		sym->Hoga.Ary[2].BuyQty = std::stoi(item.buy_qty3);
		sym->Hoga.Ary[2].SellCnt = std::stoi(item.sell_count3);
		sym->Hoga.Ary[2].BuyCnt = std::stoi(item.buy_count3);

		if (item.sell_price4.length() > 0)
			sym->Hoga.Ary[3].SellPrice = std::stoi(item.sell_price4);
		else
			sym->Hoga.Ary[3].SellPrice = 0;

		if (item.buy_price4.length() > 0)
			sym->Hoga.Ary[3].BuyPrice = std::stoi(item.buy_price4);
		else
			sym->Hoga.Ary[3].BuyPrice = 0;

		sym->Hoga.Ary[3].SellQty = std::stoi(item.sell_qty4);
		sym->Hoga.Ary[3].BuyQty = std::stoi(item.buy_qty4);
		sym->Hoga.Ary[3].SellCnt = std::stoi(item.sell_count4);
		sym->Hoga.Ary[3].BuyCnt = std::stoi(item.buy_count4);

		if (item.sell_price5.length() > 0)
			sym->Hoga.Ary[4].SellPrice = std::stoi(item.sell_price5);
		else
			sym->Hoga.Ary[4].SellPrice = 0;

		if (item.buy_price5.length() > 0)
			sym->Hoga.Ary[4].BuyPrice = std::stoi(item.buy_price5);
		else
			sym->Hoga.Ary[4].BuyPrice = 0;

		sym->Hoga.Ary[4].SellQty = std::stoi(item.sell_qty5);
		sym->Hoga.Ary[4].BuyQty = std::stoi(item.buy_qty5);
		sym->Hoga.Ary[4].SellCnt = std::stoi(item.sell_count5);
		sym->Hoga.Ary[4].BuyCnt = std::stoi(item.buy_count5);

		sym->Hoga.TotSellQty = std::stoi(item.tot_sell_qty);
		sym->Hoga.TotBuyQty = std::stoi(item.tot_buy_qty);
		sym->Hoga.TotSellCnt = std::stoi(item.tot_sell_count);
		sym->Hoga.TotBuyCnt = std::stoi(item.tot_buy_count);

		sym->Hoga.Time = item.time;

		//VtSystemManager::GetInstance()->UpdateRealtimeArgs(sym);

		// 관련된 창에 메시지를 보낸다.
		SmCallbackManager::GetInstance()->OnWndHogaEvent(sym.get());
	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s 알수없는 오류"), __FUNCTION__);
	}
}

bool SmRealtimeHogaManager::ExecuteTask(std::array<SmHogaItem, HogaArraySize>& arr, int taken)
{
	try {
	std::set<std::shared_ptr<SmSymbol>> symbol_set;
	for (int i = 0; i < taken; ++i) {
		SmHogaItem item = arr.at(i);

		// 종료 명령이면 거짓을 반환한다.
		if (item.command == 0)
			return false;

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(item.symbol_code);
		if (!sym)
			continue;

		SmCallbackManager::GetInstance()->OnWndHogaEvent(sym.get());

		// 집합에 심볼을 추가한다.
		symbol_set.insert(sym);

		if (item.sell_price1.length() > 0)
			sym->Hoga.Ary[0].SellPrice = std::stoi(item.sell_price1);
		else
			sym->Hoga.Ary[0].SellPrice = 0;

		if (item.buy_price1.length() > 0)
			sym->Hoga.Ary[0].BuyPrice = std::stoi(item.buy_price1);
		else
			sym->Hoga.Ary[0].BuyPrice = 0;
		sym->Hoga.Ary[0].SellQty = std::stoi(item.sell_qty1);
		sym->Hoga.Ary[0].BuyQty = std::stoi(item.buy_qty1);
		sym->Hoga.Ary[0].SellCnt = std::stoi(item.sell_count1);
		sym->Hoga.Ary[0].BuyCnt = std::stoi(item.buy_count1);


		if (item.sell_price2.length() > 0)
			sym->Hoga.Ary[1].SellPrice = std::stoi(item.sell_price2);
		else
			sym->Hoga.Ary[1].SellPrice = 0;

		if (item.buy_price2.length() > 0)
			sym->Hoga.Ary[1].BuyPrice = std::stoi(item.buy_price2);
		else
			sym->Hoga.Ary[0].BuyPrice = 0;
		sym->Hoga.Ary[1].SellQty = std::stoi(item.sell_qty2);
		sym->Hoga.Ary[1].BuyQty = std::stoi(item.buy_qty2);
		sym->Hoga.Ary[1].SellCnt = std::stoi(item.sell_count2);
		sym->Hoga.Ary[1].BuyCnt = std::stoi(item.buy_count2);


		if (item.sell_price3.length() > 0)
			sym->Hoga.Ary[2].SellPrice = std::stoi(item.sell_price3);
		else
			sym->Hoga.Ary[2].SellPrice = 0;

		if (item.buy_price3.length() > 0)
			sym->Hoga.Ary[2].BuyPrice = std::stoi(item.buy_price3);
		else
			sym->Hoga.Ary[2].BuyPrice = 0;
		sym->Hoga.Ary[2].SellQty = std::stoi(item.sell_qty3);
		sym->Hoga.Ary[2].BuyQty = std::stoi(item.buy_qty3);
		sym->Hoga.Ary[2].SellCnt = std::stoi(item.sell_count3);
		sym->Hoga.Ary[2].BuyCnt = std::stoi(item.buy_count3);

		if (item.sell_price4.length() > 0)
			sym->Hoga.Ary[3].SellPrice = std::stoi(item.sell_price4);
		else
			sym->Hoga.Ary[3].SellPrice = 0;

		if (item.buy_price4.length() > 0)
			sym->Hoga.Ary[3].BuyPrice = std::stoi(item.buy_price4);
		else
			sym->Hoga.Ary[3].BuyPrice = 0;

		sym->Hoga.Ary[3].SellQty = std::stoi(item.sell_qty4);
		sym->Hoga.Ary[3].BuyQty = std::stoi(item.buy_qty4);
		sym->Hoga.Ary[3].SellCnt = std::stoi(item.sell_count4);
		sym->Hoga.Ary[3].BuyCnt = std::stoi(item.buy_count4);

		if (item.sell_price5.length() > 0)
			sym->Hoga.Ary[4].SellPrice = std::stoi(item.sell_price5);
		else
			sym->Hoga.Ary[4].SellPrice = 0;

		if (item.buy_price5.length() > 0)
			sym->Hoga.Ary[4].BuyPrice = std::stoi(item.buy_price5);
		else
			sym->Hoga.Ary[4].BuyPrice = 0;

		sym->Hoga.Ary[4].SellQty = std::stoi(item.sell_qty5);
		sym->Hoga.Ary[4].BuyQty = std::stoi(item.buy_qty5);
		sym->Hoga.Ary[4].SellCnt = std::stoi(item.sell_count5);
		sym->Hoga.Ary[4].BuyCnt = std::stoi(item.buy_count5);

		sym->Hoga.TotSellQty = std::stoi(item.tot_sell_qty);
		sym->Hoga.TotBuyQty = std::stoi(item.tot_buy_qty);
		sym->Hoga.TotSellCnt = std::stoi(item.tot_sell_count);
		sym->Hoga.TotBuyCnt = std::stoi(item.tot_buy_count);

		sym->Hoga.Time = item.time;

		//VtSystemManager::GetInstance()->UpdateRealtimeArgs(sym);
	}

	// 창은 한꺼번에 업데이트 한다.
	for (auto it = symbol_set.begin(); it != symbol_set.end(); ++it) {
		std::shared_ptr<SmSymbol> sym = *it;
		// 관련된 창에 메시지를 보낸다.
		SmCallbackManager::GetInstance()->OnWndHogaEvent(sym.get());
		//SmSessionManager::GetInstance()->SendReqUpdateHoga(sym);
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
