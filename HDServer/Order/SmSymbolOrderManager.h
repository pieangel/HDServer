#pragma once
#include "SmOrderManager.h"
#include <string>
#include "../Service/SmServiceDefine.h"
struct SmOrder;
struct SmPosition;
class SmAccount;
class SmSymbolOrderManager : public SmOrderManager
{
public:
	SmSymbolOrderManager();
	~SmSymbolOrderManager();
	// �ֹ��� ó�� �������� �� �̺�Ʈ ó��
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	// �ֹ� ����Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	// �ֹ� ü�� Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	// �ֹ� û�� Ȯ�� �̺�Ʈ ó��
	virtual void OnOrderSettled(std::shared_ptr<SmOrder> order);
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
private:
	void CalcFee(std::shared_ptr<SmPosition> posi, std::shared_ptr<SmAccount> acnt, std::shared_ptr<SmOrder> filledOrder);
	double _TotalFee = 0.0;
	std::string _SymbolCode;
	void CalcPosition(std::shared_ptr<SmOrder> order);
	int CalcRemainOrder(std::shared_ptr<SmOrder> newOrder);
	int CalcTotalRemain();
	void SendRemain(std::shared_ptr<SmOrder> order, std::shared_ptr<SmAccount> acnt);
	void SendResponse(std::shared_ptr<SmOrder> order, SmProtocol protocol);
};

