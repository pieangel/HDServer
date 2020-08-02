#include "pch.h"
#include "SmAccountOrderManager.h"
#include "SmSymbolOrderManager.h"
#include "SmOrder.h"
SmAccountOrderManager::SmAccountOrderManager()
{

}

SmAccountOrderManager::~SmAccountOrderManager()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it) {
		delete it->second;
	}
}

void SmAccountOrderManager::AddFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmSymbolOrderManager* orderMgr = FindAddOrderManager(order->SymbolCode);
	orderMgr->AddFilledOrder(order);
	SmOrderManager::AddFilledOrder(order);
}

void SmAccountOrderManager::AddAcceptedOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmSymbolOrderManager* orderMgr = FindAddOrderManager(order->SymbolCode);
	orderMgr->AddAcceptedOrder(order);
	SmOrderManager::AddAcceptedOrder(order);
}

void SmAccountOrderManager::OnOrderAccepted(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmSymbolOrderManager* orderMgr = FindAddOrderManager(order->SymbolCode);
	orderMgr->OnOrderAccepted(order);
	SmOrderManager::OnOrderAccepted(order);
}

void SmAccountOrderManager::OnOrderFilled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmSymbolOrderManager* orderMgr = FindAddOrderManager(order->SymbolCode);
	orderMgr->OnOrderFilled(order);
	SmOrderManager::OnOrderFilled(order);
}

void SmAccountOrderManager::OnOrderSettled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmOrderManager::OnOrderSettled(order);
}

void SmAccountOrderManager::OnOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmSymbolOrderManager* orderMgr = FindAddOrderManager(order->SymbolCode);
	orderMgr->OnOrder(order);
	SmOrderManager::OnOrder(order);
}

SmSymbolOrderManager* SmAccountOrderManager::FindAddOrderManager(std::string symCode)
{
	SmSymbolOrderManager* acntMgr = nullptr;
	auto it = _OrderManagerMap.find(symCode);
	if (it == _OrderManagerMap.end()) {
		acntMgr = new SmSymbolOrderManager();
		acntMgr->SymbolCode(symCode);
		_OrderManagerMap[symCode] = acntMgr;
		return acntMgr;
	}

	return it->second;
}
