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
	// 접수확인된 주문들을 보낸다.
	void SendAcceptedOrderList(int session_id, std::string account_no);
	// 접수확인된 주문들을 보낸다.
	void SendAcceptedOrderList(int session_id, std::vector<std::string> account_no_vec);
	// 체결된(이미 청산된 주문은 체외) 주문들을 보낸다.
	void SendFilledOrderList(int session_id, std::string account_no);
	void SendFilledOrderList(int session_id, std::vector<std::string> account_no_vec);
	// 주문 목록을 최신 것 부터 보낸다.
	void SendOrderList(int session_id, std::string account_no, int count = 50);
	void SendResponse(std::shared_ptr<SmOrder> order, SmProtocol protocol);
	void SendOrderList(int session_id, std::string account_no, std::string date_time);
	// 주문이 처음 도착했을 때 이벤트 처리
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	// 주문 접수확인 때 이벤트 처리
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	// 주문 체결 확인 때 이벤트 처리
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	// 주문 청산 확인 이벤트 처리
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

