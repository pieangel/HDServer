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
	// �����带 ���� �ϱ� ���� �ݵ�� ť�� ������ ���� ���ƾ� �Ѵ�.
	// ���� ť�� ������ ���� ���� �ʰ� �����带 �����ϸ� ����� �����尡 ������� �ʴ´�.
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
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;

	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_TaskQueue.try_add(item);
}

void SmRealtimeHogaManager::AddFutTask(SmHogaItem&& item)
{
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;

	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_FutTaskQueue.try_add(item);
}

void SmRealtimeHogaManager::AddOptTask(SmHogaItem&& item)
{
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;

	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_OptTaskQueue.try_add(item);
}

void SmRealtimeHogaManager::AddProTask(SmHogaItem&& item)
{
	// ��Ȱ��ȭ �Ǹ� �������� �ʴ´�.
	if (!_Enable)
		return;

	// �߰��ϰ� ��� ��ȯ�Ѵ�. Ȥ�� ť�� �̿��Ҽ� ���� ��Ȳ�̶� ��ȯ�Ѵ�.
	// �ݵ�� �߰��ϴ� ��쿡�� add() �Լ��� �̿��Ѵ�.
	_ProTaskQueue.try_add(item);
}

void SmRealtimeHogaManager::StopThread()
{
	// ���� �۾� ����� ����.
	ClearTasks();
	SmHogaItem arg;
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

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmHogaItem data;
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

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmHogaItem data;
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

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmHogaItem data;
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

				// ���� ����� �ƴҶ��� �����͸� �Ѳ����� ó���Ѵ�.
				bool result = ExecuteTask(arr, taken);
				// ���� ����� ���� �����带 ���� �Ѵ�.
				if (!result)
					break;
			}
		}
		else {
			SmHogaItem data;
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

		// ���õ� â�� �޽����� ������.
		SmCallbackManager::GetInstance()->OnWndHogaEvent(sym.get());
	}
	catch (std::exception& e) {
		LOG_F(ERROR, _T(" %s, MSG : %s"), __FUNCTION__, e.what());
	}
	catch (...) {
		LOG_F(ERROR, _T(" %s �˼����� ����"), __FUNCTION__);
	}
}

bool SmRealtimeHogaManager::ExecuteTask(std::array<SmHogaItem, HogaArraySize>& arr, int taken)
{
	try {
	std::set<std::shared_ptr<SmSymbol>> symbol_set;
	for (int i = 0; i < taken; ++i) {
		SmHogaItem item = arr.at(i);

		// ���� ����̸� ������ ��ȯ�Ѵ�.
		if (item.command == 0)
			return false;

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(item.symbol_code);
		if (!sym)
			continue;

		SmCallbackManager::GetInstance()->OnWndHogaEvent(sym.get());

		// ���տ� �ɺ��� �߰��Ѵ�.
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

	// â�� �Ѳ����� ������Ʈ �Ѵ�.
	for (auto it = symbol_set.begin(); it != symbol_set.end(); ++it) {
		std::shared_ptr<SmSymbol> sym = *it;
		// ���õ� â�� �޽����� ������.
		SmCallbackManager::GetInstance()->OnWndHogaEvent(sym.get());
		//SmSessionManager::GetInstance()->SendReqUpdateHoga(sym);
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
