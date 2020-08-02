#include "pch.h"
#include "SmTotalOrderManager.h"
#include "SmOrderNumberGenerator.h"
#include "SmOrder.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmAccountOrderManager.h"
#include "../User/SmUserManager.h"
#include "../Json/json.hpp"
#include "../Service/SmServiceDefine.h"
#include "../Account/SmAccountManager.h"
#include "../Account/SmAccount.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Position/SmPosition.h"
#include "../Database/SmMongoDBManager.h"
#include "../Global/SmGlobal.h"
#include "../Server/SmSessionManager.h"
#include "../Database/SmMongoDBManager.h"
#include "../Config/SmConfigManager.h"
#include "../Xml/pugixml.hpp"

using namespace std::chrono;
using namespace nlohmann;

double SmTotalOrderManager::FeeForDomestic = 5000.0;
double SmTotalOrderManager::FeeForAbroad = 4.0;

void SmTotalOrderManager::LoadFees()
{
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();
	std::string configPath = appPath;
	configPath.append(_T("\\Config\\Config.xml"));
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(configPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node fee_info = doc.child("application").child("fee_info");

	FeeForDomestic = fee_info.child("domestic").text().as_double();
	FeeForAbroad = fee_info.child("abroad").text().as_double();

}

SmTotalOrderManager::SmTotalOrderManager()
{

}

SmTotalOrderManager::~SmTotalOrderManager()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it) {
		delete it->second;
	}
}

void SmTotalOrderManager::OnRequestOrder(SmOrderRequest&& req)
{
	SmAccountManager* acntMgr = SmAccountManager::GetInstance();
	std::shared_ptr<SmAccount> acnt = acntMgr->FindAccount(req.AccountNo);
	if (!acnt) {
		SendError(SmOrderError::BadAccount, req);
		return;
	}
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(req.SymbolCode);
	if (!sym) {
		SendError(SmOrderError::BadSymbol, req);
		return;
	}
	std::shared_ptr<SmOrder> order = CreateOrder();
	order->AccountNo = req.AccountNo;
	order->SymbolCode = req.SymbolCode;
	order->UserID = req.UserID;
	order->Position = req.Position;
	order->PriceType = req.PriceType;
	order->FilledCondition = req.FillCondition;
	order->OrderPrice = req.OrderPrice;
	order->OrderAmount = req.OrderAmount;
	order->OriOrderNo = req.OriOrderNo;
	order->RequestID = req.RequestID;
	order->OrderType = req.OrderType;
	order->FundName = req.FundName;
	order->StrategyName = req.StrategyName;
	order->ParentAccountNo = req.ParentAccountNo;
	order->SystemName = req.SystemName;
	OnOrder(order);
	switch (req.OrderType)
	{
	case SmOrderType::New:
		OnOrderNew(order);
		break;
	case SmOrderType::Modify:
		OnOrderModify(order);
		break;
	case SmOrderType::Cancel:
		OnOrderCancel(order);
		break;
	default:
		break;
	}
}

void SmTotalOrderManager::AddFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	acntOrderMgr->AddFilledOrder(order);
	SmOrderManager::AddFilledOrder(order);
}

void SmTotalOrderManager::AddAcceptedOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	// 주문상태 접수
	acntOrderMgr->AddAcceptedOrder(order);
	SmOrderManager::AddAcceptedOrder(order);
}

std::shared_ptr<SmOrder> SmTotalOrderManager::CreateOrder()
{
	int order_no = SmOrderNumberGenerator::GetID();
	// 여기서 현재 주문 번호를 데이터베이스에 저장해 놓는다.
	// 여기에 저장된 마지막 주문 번호는 다음에 읽어 와서 사용하게 된다.
	SmMongoDBManager::GetInstance()->SaveCurrentOrderNo(order_no);
	std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();
	order->OrderNo = order_no;
	return order;
}

void SmTotalOrderManager::OnOrderNew(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	CheckFilled(order);
}

void SmTotalOrderManager::OnOrderModify(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	CheckFilled(order);
	SendConfirmModify(order);
}

void SmTotalOrderManager::OnOrderCancel(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SendConfirmCancel(order);
}

void SmTotalOrderManager::CheckFilled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 시장가 주문 처리
	if (order->PriceType == SmPriceType::Market) {
		OnOrderAccepted(order);
		OnOrderFilled(order);
	} //지정가 주문 처리
	else if (order->PriceType == SmPriceType::Price) {
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
		if (order->Position == SmPositionType::Buy) {
			if (order->OrderPrice >= sym->Quote.Close) {
				OnOrderFilled(order);
			}
			else {
				OnOrderAccepted(order);
			}
		}
		else if (order->Position == SmPositionType::Sell) {
			if (order->OrderPrice <= sym->Quote.Close) {
				OnOrderFilled(order);
			}
			else {
				OnOrderAccepted(order);
			}
		}
	}
	else {
		SendError(SmOrderError::BadPriceType, *order);
	}
}

void SmTotalOrderManager::SendResponse(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	CString msg;
	json send_object;
	if (order->OrderState == SmOrderState::Accepted)
		send_object["res_id"] = SmProtocol::res_order_accepted;
	else if (order->OrderState == SmOrderState::Filled)
		send_object["res_id"] = SmProtocol::res_order_filled;
	send_object["request_id"] = order->RequestID;
	send_object["account_no"] = order->AccountNo;
	send_object["order_type"] = (int)order->OrderType;
	send_object["position_type"] = order->Position;
	send_object["price_type"] = (int)order->PriceType;
	send_object["symbol_code"] = order->SymbolCode;
	send_object["order_price"] = order->OrderPrice;
	send_object["order_no"] = order->OrderNo;
	send_object["order_amount"] = order->OrderAmount;
	send_object["ori_order_no"] = order->OriOrderNo;
	send_object["filled_date"] = order->FilledDate;
	send_object["filled_time"] = order->FilledTime;
	send_object["order_date"] = order->OrderDate;
	send_object["order_time"] = order->OrderTime;
	send_object["filled_qty"] = order->FilledQty;
	send_object["filled_price"] = order->FilledPrice;
	send_object["order_state"] = (int)order->OrderState;
	send_object["filled_condition"] = (int)order->FilledCondition;
	send_object["symbol_decimal"] = order->SymbolDecimal;
	send_object["remain_qty"] = order->RemainQty;
	send_object["strategy_name"] = order->StrategyName;
	send_object["system_name"] = order->SystemName;
	send_object["fund_name"] = order->FundName;
	for (size_t i = 0; i < order->SettledOrders.size(); ++i) {
		send_object["settled_orders"][i] = order->SettledOrders[i];
	}

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmTotalOrderManager::SendResponse(std::shared_ptr<SmOrder> order, SmProtocol protocol)
{
	if (!order)
		return;

	CString msg;
	json send_object;
	send_object["res_id"] = protocol;
	send_object["request_id"] = order->RequestID;
	send_object["account_no"] = order->AccountNo;
	send_object["order_type"] = (int)order->OrderType;
	send_object["position_type"] = (int)order->Position;
	send_object["price_type"] = (int)order->PriceType;
	send_object["symbol_code"] = order->SymbolCode;
	send_object["order_price"] = order->OrderPrice;
	send_object["order_no"] = order->OrderNo;
	send_object["order_amount"] = order->OrderAmount;
	send_object["ori_order_no"] = order->OriOrderNo;
	send_object["filled_date"] = order->FilledDate;
	send_object["filled_time"] = order->FilledTime;
	send_object["order_date"] = order->OrderDate;
	send_object["order_time"] = order->OrderTime;
	send_object["filled_qty"] = order->FilledQty;
	send_object["filled_price"] = order->FilledPrice;
	send_object["order_state"] = (int)order->OrderState;
	send_object["filled_condition"] = (int)order->FilledCondition;
	send_object["symbol_decimal"] = order->SymbolDecimal;
	send_object["remain_qty"] = order->RemainQty;
	send_object["strategy_name"] = order->StrategyName;
	send_object["system_name"] = order->SystemName;
	send_object["fund_name"] = order->FundName;
	for (size_t i = 0; i < order->SettledOrders.size(); ++i) {
		send_object["settled_orders"][i] = order->SettledOrders[i];
	}

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmTotalOrderManager::SendRemain(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmTotalPositionManager* totalPosiMgr = SmTotalPositionManager::GetInstance();
	std::shared_ptr<SmPosition> posi = totalPosiMgr->FindPosition(order->AccountNo, order->SymbolCode);

	CString msg;
	json send_object;
	send_object["res_id"] = SmProtocol::res_symbol_position;
	send_object["created_date"] = posi->CreatedDate;
	send_object["created_time"] = posi->CreatedTime;
	send_object["symbol_code"] = posi->SymbolCode;
	send_object["fund_name"] = posi->FundName;
	send_object["account_no"] = posi->AccountNo;
	send_object["position_type"] = posi->Position;
	send_object["open_qty"] = posi->OpenQty;
	send_object["fee_count"] = posi->FeeCount;
	send_object["trade_pl"] = posi->TradePL;
	send_object["avg_price"] = posi->AvgPrice;
	send_object["cur_price"] = posi->CurPrice;
	send_object["open_pl"] = posi->OpenPL;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmTotalOrderManager::SendError(SmOrderError ErrorCode)
{
	json send_object;
	
	send_object["res_id"] = SmProtocol::res_order_error;
	
}

void SmTotalOrderManager::SendError(SmOrderError ErrorCode, SmOrderRequest& req)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_order_error;
	send_object["error_code"] = (int)ErrorCode;
	send_object["symbol_code"] = req.SymbolCode;
	send_object["fund_name"] = req.FundName;
	send_object["account_no"] = req.AccountNo;
	send_object["request_id"] = req.RequestID;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(req.UserID, content);
}

void SmTotalOrderManager::SendError(SmOrderError ErrorCode, SmOrder& order)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_order_error;
	send_object["symbol_code"] = order.SymbolCode;
	send_object["fund_name"] = order.FundName;
	send_object["account_no"] = order.AccountNo;
	send_object["request_id"] = order.RequestID;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order.UserID, content);
}

void SmTotalOrderManager::OnOrderAccepted(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	// 주문상태 접수
	acntOrderMgr->OnOrderAccepted(order);
	SmOrderManager::OnOrderAccepted(order);
	SendResponse(order, SmProtocol::res_order_accepted);
}

void SmTotalOrderManager::OnOrderFilled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	acntOrderMgr->OnOrderFilled(order);
	SmOrderManager::OnOrderFilled(order);
}

void SmTotalOrderManager::OnOrderSettled(std::shared_ptr<SmOrder> order)
{
	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	acntOrderMgr->OnOrderSettled(order);
	SmOrderManager::OnOrderSettled(order);
}

void SmTotalOrderManager::OnOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	acntOrderMgr->OnOrder(order);
	SmOrderManager::OnOrder(order);
	if (order->OrderType == SmOrderType::New)
		SendResponse(order, SmProtocol::res_order_new);
	else if (order->OrderType == SmOrderType::Modify)
		SendResponse(order, SmProtocol::res_order_modify);
	else if (order->OrderType == SmOrderType::Cancel)
		SendResponse(order, SmProtocol::res_order_cancel);
}

SmAccountOrderManager* SmTotalOrderManager::FindAddOrderManager(std::string acntNo)
{
	SmAccountOrderManager* acntMgr = nullptr;
	auto it = _OrderManagerMap.find(acntNo);
	if (it == _OrderManagerMap.end()) {
		acntMgr = new SmAccountOrderManager();
		acntMgr->AccountNo(acntNo);
		_OrderManagerMap[acntNo] = acntMgr;
		return acntMgr;
	}

	return it->second;
}

void SmTotalOrderManager::SendConfirmModify(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 원래 주문을 찾는다.
	std::shared_ptr<SmOrder> oriOrder = FindOrder(order->OriOrderNo);
	// 원래 주문 상태를 변경한다.
	oriOrder->OrderState = SmOrderState::ConfirmModify;
	// 여기서 주문 상태를 데이터베이스에 저장해 준다.
	SmMongoDBManager::GetInstance()->ChangeOrderState(oriOrder);
	SendResponse(order);
}

void SmTotalOrderManager::SendConfirmCancel(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 원래 주문을 찾는다.
	std::shared_ptr<SmOrder> oriOrder = FindOrder(order->OriOrderNo);
	// 원래 주문 상태를 변경한다.
	oriOrder->OrderState = SmOrderState::ConfirmCancel;
	// 여기서 주문 상태를 데이터베이스에 저장해 준다.
	SmMongoDBManager::GetInstance()->ChangeOrderState(oriOrder);
	SendResponse(order);
}

void SmTotalOrderManager::SendAcceptedOrderList(int session_id, std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list = SmMongoDBManager::GetInstance()->GetAcceptedOrderList(account_no);
	json send_object;
	send_object["res_id"] = SmProtocol::res_accepted_order_list;
	send_object["total_order_count"] = (int)order_list.size();
	for (size_t j = 0; j < order_list.size(); ++j) {
		std::shared_ptr<SmOrder> order = order_list[j];
		send_object["order"][j] = {
			{ "account_no",  order->AccountNo },
			{ "order_type", order->OrderType },
			{ "position_type",  order->Position },
			{ "price_type",  order->PriceType },
			{ "symbol_code",  order->SymbolCode },
			{ "order_price",  order->OrderPrice },
			{ "order_no",  order->OrderNo },
			{ "order_amount",  order->OrderAmount },
			{ "ori_order_no",  order->OriOrderNo },
			{ "filled_date",  order->FilledDate },
			{ "filled_time",  order->FilledTime },
			{ "order_date",  order->OrderDate },
			{ "order_time",  order->OrderTime },
			{ "filled_qty",  order->FilledQty },
			{ "filled_price",  order->FilledPrice },
			{ "order_state",  order->OrderState },
			{ "filled_condition",  order->FilledCondition },
			{ "symbol_decimal",  order->SymbolDecimal },
			{ "remain_qty",  order->RemainQty },
			{ "strategy_name",  order->StrategyName },
			{ "system_name",  order->SystemName },
			{ "fund_name",  order->FundName }
		};

	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

void SmTotalOrderManager::SendAcceptedOrderList(int session_id, std::vector<std::string> account_no_vec)
{
	std::vector<std::shared_ptr<SmOrder>> total_order_list;
	for (size_t i = 0; i < account_no_vec.size(); ++i) {
		std::vector<std::shared_ptr<SmOrder>> order_list = SmMongoDBManager::GetInstance()->GetAcceptedOrderList(account_no_vec[i]);
		total_order_list.insert(total_order_list.end(), order_list.begin(), order_list.end());
	}
	json send_object;
	send_object["res_id"] = SmProtocol::res_accepted_order_list;
	send_object["total_order_count"] = (int)total_order_list.size();
	for (size_t j = 0; j < total_order_list.size(); ++j) {
		std::shared_ptr<SmOrder> order = total_order_list[j];
		send_object["order"][j] = {
			{ "account_no",  order->AccountNo },
			{ "order_type", order->OrderType },
			{ "position_type",  order->Position },
			{ "price_type",  order->PriceType },
			{ "symbol_code",  order->SymbolCode },
			{ "order_price",  order->OrderPrice },
			{ "order_no",  order->OrderNo },
			{ "order_amount",  order->OrderAmount },
			{ "ori_order_no",  order->OriOrderNo },
			{ "filled_date",  order->FilledDate },
			{ "filled_time",  order->FilledTime },
			{ "order_date",  order->OrderDate },
			{ "order_time",  order->OrderTime },
			{ "filled_qty",  order->FilledQty },
			{ "filled_price",  order->FilledPrice },
			{ "order_state",  order->OrderState },
			{ "filled_condition",  order->FilledCondition },
			{ "symbol_decimal",  order->SymbolDecimal },
			{ "remain_qty",  order->RemainQty },
			{ "strategy_name",  order->StrategyName },
			{ "system_name",  order->SystemName },
			{ "fund_name",  order->FundName }
		};

	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

void SmTotalOrderManager::SendFilledOrderList(int session_id, std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list = SmMongoDBManager::GetInstance()->GetFilledOrderList(account_no);
	json send_object;
	send_object["res_id"] = SmProtocol::res_filled_order_list;
	send_object["total_order_count"] = (int)order_list.size();
	for (size_t j = 0; j < order_list.size(); ++j) {
		std::shared_ptr<SmOrder> order = order_list[j];
		send_object["order"][j] = {
			{ "account_no",  order->AccountNo },
			{ "order_type", order->OrderType },
			{ "position_type",  order->Position },
			{ "price_type",  order->PriceType },
			{ "symbol_code",  order->SymbolCode },
			{ "order_price",  order->OrderPrice },
			{ "order_no",  order->OrderNo },
			{ "order_amount",  order->OrderAmount },
			{ "ori_order_no",  order->OriOrderNo },
			{ "filled_date",  order->FilledDate },
			{ "filled_time",  order->FilledTime },
			{ "order_date",  order->OrderDate },
			{ "order_time",  order->OrderTime },
			{ "filled_qty",  order->FilledQty },
			{ "filled_price",  order->FilledPrice },
			{ "order_state",  order->OrderState },
			{ "filled_condition",  order->FilledCondition },
			{ "symbol_decimal",  order->SymbolDecimal },
			{ "remain_qty",  order->RemainQty },
			{ "strategy_name",  order->StrategyName },
			{ "system_name",  order->SystemName },
			{ "fund_name",  order->FundName }
		};

	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

void SmTotalOrderManager::SendFilledOrderList(int session_id, std::vector<std::string> account_no_vec)
{
	std::vector<std::shared_ptr<SmOrder>> total_order_list;
	for (size_t i = 0; i < account_no_vec.size(); ++i) {
		std::vector<std::shared_ptr<SmOrder>> order_list = SmMongoDBManager::GetInstance()->GetFilledOrderList(account_no_vec[i]);
		total_order_list.insert(total_order_list.end(), order_list.begin(), order_list.end());
	}
	json send_object;
	send_object["res_id"] = SmProtocol::res_filled_order_list;
	send_object["total_order_count"] = (int)total_order_list.size();
	for (size_t j = 0; j < total_order_list.size(); ++j) {
		std::shared_ptr<SmOrder> order = total_order_list[j];
		send_object["order"][j] = {
			{ "account_no",  order->AccountNo },
			{ "order_type", order->OrderType },
			{ "position_type",  order->Position },
			{ "price_type",  order->PriceType },
			{ "symbol_code",  order->SymbolCode },
			{ "order_price",  order->OrderPrice },
			{ "order_no",  order->OrderNo },
			{ "order_amount",  order->OrderAmount },
			{ "ori_order_no",  order->OriOrderNo },
			{ "filled_date",  order->FilledDate },
			{ "filled_time",  order->FilledTime },
			{ "order_date",  order->OrderDate },
			{ "order_time",  order->OrderTime },
			{ "filled_qty",  order->FilledQty },
			{ "filled_price",  order->FilledPrice },
			{ "order_state",  order->OrderState },
			{ "filled_condition",  order->FilledCondition },
			{ "symbol_decimal",  order->SymbolDecimal },
			{ "remain_qty",  order->RemainQty },
			{ "strategy_name",  order->StrategyName },
			{ "system_name",  order->SystemName },
			{ "fund_name",  order->FundName }
		};

	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

void SmTotalOrderManager::SendOrderList(int session_id, std::string account_no, int count)
{
	std::vector<std::shared_ptr<SmOrder>> order_list = SmMongoDBManager::GetInstance()->GetFilledOrderList(account_no);
	json send_object;
	send_object["res_id"] = SmProtocol::res_order_list;
	send_object["total_order_count"] = (int)order_list.size();
	for (size_t j = 0; j < order_list.size(); ++j) {
		std::shared_ptr<SmOrder> order = order_list[j];
		send_object["order"][j] = {
			{ "account_no",  order->AccountNo },
			{ "order_type", order->OrderType },
			{ "position_type",  order->Position },
			{ "price_type",  order->PriceType },
			{ "symbol_code",  order->SymbolCode },
			{ "order_price",  order->OrderPrice },
			{ "order_no",  order->OrderNo },
			{ "order_amount",  order->OrderAmount },
			{ "ori_order_no",  order->OriOrderNo },
			{ "filled_date",  order->FilledDate },
			{ "filled_time",  order->FilledTime },
			{ "order_date",  order->OrderDate },
			{ "order_time",  order->OrderTime },
			{ "filled_qty",  order->FilledQty },
			{ "filled_price",  order->FilledPrice },
			{ "order_state",  order->OrderState },
			{ "filled_condition",  order->FilledCondition },
			{ "symbol_decimal",  order->SymbolDecimal },
			{ "remain_qty",  order->RemainQty },
			{ "strategy_name",  order->StrategyName },
			{ "system_name",  order->SystemName },
			{ "fund_name",  order->FundName }
		};

	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

void SmTotalOrderManager::SendOrderList(int session_id, std::string account_no, std::string date_time)
{
	std::vector<std::shared_ptr<SmOrder>> order_list = SmMongoDBManager::GetInstance()->GetOrderList(account_no);
	json send_object;
	send_object["res_id"] = SmProtocol::res_order_list;
	send_object["total_order_count"] = (int)order_list.size();
	for (size_t j = 0; j < order_list.size(); ++j) {
		std::shared_ptr<SmOrder> order = order_list[j];
		send_object["order"][j] = {
			{ "account_no",  order->AccountNo },
			{ "order_type", order->OrderType },
			{ "position_type",  order->Position },
			{ "price_type",  order->PriceType },
			{ "symbol_code",  order->SymbolCode },
			{ "order_price",  order->OrderPrice },
			{ "order_no",  order->OrderNo },
			{ "order_amount",  order->OrderAmount },
			{ "ori_order_no",  order->OriOrderNo },
			{ "filled_date",  order->FilledDate },
			{ "filled_time",  order->FilledTime },
			{ "order_date",  order->OrderDate },
			{ "order_time",  order->OrderTime },
			{ "filled_qty",  order->FilledQty },
			{ "filled_price",  order->FilledPrice },
			{ "order_state",  order->OrderState },
			{ "filled_condition",  order->FilledCondition },
			{ "symbol_decimal",  order->SymbolDecimal },
			{ "remain_qty",  order->RemainQty },
			{ "strategy_name",  order->StrategyName },
			{ "system_name",  order->SystemName },
			{ "fund_name",  order->FundName }
		};

	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}
