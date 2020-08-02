#pragma once
#include "../Global/TemplateSingleton.h"
#include "SmOrderManager.h"
#include "SmOrderDefine.h"
#include "../Service/SmServiceDefine.h"
#include <map>
#include <string>
struct SmOrder;
class SmAccountOrderManager;
class SmTotalOrderManager : public TemplateSingleton<SmTotalOrderManager>, public SmOrderManager
{
public:
	static double FeeForAbroad;
	static double FeeForDomestic;
	void LoadFees();
	SmTotalOrderManager();
	virtual ~SmTotalOrderManager();
	void OnRequestOrder(SmOrderRequest&& req);
	virtual void AddFilledOrder(std::shared_ptr<SmOrder> order);
	virtual void AddAcceptedOrder(std::shared_ptr<SmOrder> order);
	// ����Ȯ�ε� �ֹ����� ������.
	void SendAcceptedOrderList(int session_id, std::string account_no);
	// ����Ȯ�ε� �ֹ����� ������.
	void SendAcceptedOrderList(int session_id, std::vector<std::string> account_no_vec);
	// ü���(�̹� û��� �ֹ��� ü��) �ֹ����� ������.
	void SendFilledOrderList(int session_id, std::string account_no);
	void SendFilledOrderList(int session_id, std::vector<std::string> account_no_vec);
	// �ֹ� ����� �ֽ� �� ���� ������.
	void SendOrderList(int session_id, std::string account_no, int count = 50);
	void SendResponse(std::shared_ptr<SmOrder> order, SmProtocol protocol);
	void SendOrderList(int session_id, std::string account_no, std::string date_time);
	// �ֹ��� ó�� �������� �� �̺�Ʈ ó��
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	// �ֹ� ����Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	// �ֹ� ü�� Ȯ�� �� �̺�Ʈ ó��
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	// �ֹ� û�� Ȯ�� �̺�Ʈ ó��
	virtual void OnOrderSettled(std::shared_ptr<SmOrder> order);
protected:
	std::shared_ptr<SmOrder> CreateOrder();
	void OnOrderNew(std::shared_ptr<SmOrder> order);
	void OnOrderModify(std::shared_ptr<SmOrder> order);
	void OnOrderCancel(std::shared_ptr<SmOrder> order);
	void CheckFilled(std::shared_ptr<SmOrder> order);
	void SendResponse(std::shared_ptr<SmOrder> order);
	void SendRemain(std::shared_ptr<SmOrder> order);
	void SendError(SmOrderError ErrorCode);
	void SendError(SmOrderError ErrorCode, SmOrderRequest& req);
	void SendError(SmOrderError ErrorCode, SmOrder& order);
	
	SmAccountOrderManager* FindAddOrderManager(std::string acntNo);
	std::map<std::string, SmAccountOrderManager*> _OrderManagerMap;
	void SendConfirmModify(std::shared_ptr<SmOrder> order);
	void SendConfirmCancel(std::shared_ptr<SmOrder> order);
};

