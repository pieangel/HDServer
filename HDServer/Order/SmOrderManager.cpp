#include "pch.h"
#include "SmOrderManager.h"
#include "SmOrder.h"
SmOrderManager::SmOrderManager()
{

}

SmOrderManager::~SmOrderManager()
{

}

void SmOrderManager::AddFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	_FilledOrderMap[order->OrderNo] = order;
}

void SmOrderManager::AddAcceptedOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	_AcceptedOrderMap[order->OrderNo] = order;
}

void SmOrderManager::OnOrderAccepted(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 접수 확인 목록에 추가 한다.
	_AcceptedOrderMap[order->OrderNo] = order;
}

void SmOrderManager::OnOrderFilled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 주문확인 목록에서 제거해 준다.
	auto it = _AcceptedOrderMap.find(order->OrderNo);
	if (it != _AcceptedOrderMap.end()) {
		_AcceptedOrderMap.erase(it);
	}
	// 체결 확인 목록에 추가해 준다.
	_FilledOrderMap[order->OrderNo] = order;
}

void SmOrderManager::OnOrderSettled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 체결확인 목록에서 제거해 준다.
	auto it = _FilledOrderMap.find(order->OrderNo);
	if (it != _FilledOrderMap.end()) {
		_FilledOrderMap.erase(it);
	}
	// 청산 확인 목록에 추가해 준다.
	_SettledMap[order->OrderNo] = order;
}

std::shared_ptr<SmOrder> SmOrderManager::AddOrder(int orderNo)
{
	std::shared_ptr<SmOrder> order = nullptr;
	auto it = _OrderMap.find(orderNo);
	if (it != _OrderMap.end())
		return it->second;
	_OrderMap[orderNo] = order;
	return order;
}

void SmOrderManager::AddOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	auto it = _OrderMap.find(order->OrderNo);
	if (it != _OrderMap.end())
		return;
	_OrderMap[order->OrderNo] = order;
}

std::shared_ptr<SmOrder> SmOrderManager::FindOrder(int order_no)
{
	auto it = _OrderMap.find(order_no);
	if (it != _OrderMap.end()) {
		return it->second;
	}

	return nullptr;
}

void SmOrderManager::OnOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	AddOrder(order);
}
