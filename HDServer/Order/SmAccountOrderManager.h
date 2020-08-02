#pragma once
#include "../Global/TemplateSingleton.h"
#include "SmOrderManager.h"
#include <string>
#include <map>
struct SmOrder;
class SmSymbolOrderManager;
class SmAccountOrderManager : public SmOrderManager
{
public:
	SmAccountOrderManager();
	~SmAccountOrderManager();
	virtual void AddFilledOrder(std::shared_ptr<SmOrder> order);
	virtual void AddAcceptedOrder(std::shared_ptr<SmOrder> order);
	// 주문이 처음 도착했을 때 이벤트 처리
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	// 주문 접수확인 때 이벤트 처리
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	// 주문 체결 확인 때 이벤트 처리
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	// 주문 청산 확인 이벤트 처리
	virtual void OnOrderSettled(std::shared_ptr<SmOrder> order);
	SmSymbolOrderManager* FindAddOrderManager(std::string symCode);
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
private:
	std::string _AccountNo;
	std::map<std::string, SmSymbolOrderManager*> _OrderManagerMap;
};

