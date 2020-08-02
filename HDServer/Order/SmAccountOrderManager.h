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
	// �ֹ��� ó�� �������� �� �̺�Ʈ ó��
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	// �ֹ� ����Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	// �ֹ� ü�� Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	// �ֹ� û�� Ȯ�� �̺�Ʈ ó��
	virtual void OnOrderSettled(std::shared_ptr<SmOrder> order);
	SmSymbolOrderManager* FindAddOrderManager(std::string symCode);
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
private:
	std::string _AccountNo;
	std::map<std::string, SmSymbolOrderManager*> _OrderManagerMap;
};

