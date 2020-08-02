#pragma once
#include <map>
class SmOrder;
class SmOrderManager
{
public:
	SmOrderManager();
	virtual ~SmOrderManager();
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
protected:
	std::map<int, SmOrder*> _OrderMap;
	std::map<int, SmOrder*> _AcceptedOrderMap;
	std::map<int, SmOrder*> _FilledOrderMap;
	std::map<int, SmOrder*> _SettledMap;
	SmOrder* AddOrder(int orderNo);
};
