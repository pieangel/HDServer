#include "SmSymbolOrderManager.h"
#include "SmOrder.h"
#include "SmOrderManager.h"
#include "../Account/SmAccountManager.h"
#include "../Account/SmAccount.h"
#include "../Position/SmPosition.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include <math.h>
#include "../Position/SmTotalPositionManager.h"
#include "../Util/VtStringUtil.h"
#include "../Database/SmMongoDBManager.h"
#include "../Order/SmTotalOrderManager.h"
#include "../Service/SmServiceDefine.h"
#include "../Position/SmFee.h"
#include "../User/SmUserManager.h"
#include "../Json/json.hpp"
#include "../Log/loguru.hpp"
using namespace nlohmann;

SmSymbolOrderManager::SmSymbolOrderManager()
{

}

SmSymbolOrderManager::~SmSymbolOrderManager()
{

}
void SmSymbolOrderManager::OnOrderAccepted(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	if (order->SymbolCode.compare(_SymbolCode) != 0)
		return;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	// 접수된 가격을 넣어 준다.
	order->AcceptedPrice = sym->Quote.Close;
	// 주문 상태를 변경한다.
	order->OrderState = SmOrderState::Accepted;
	SmMongoDBManager::GetInstance()->OnAcceptedOrder(order);
	// 접수 확인 목록에 넣어 준다.
	SmOrderManager::OnOrderAccepted(order);
}
void SmSymbolOrderManager::OnOrderFilled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	// 체결 날짜를 넣어 준다.
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	order->FilledDate = date_time.first;
	order->FilledTime = date_time.second;
	// 체결 가격을 넣어 준다. 체결 가격은 현재 가격이다.
	order->FilledPrice = sym->Quote.Close;
	// 신규 주문상태 체결
	order->OrderState = SmOrderState::Filled;
	// 주문 갯수를 체결 갯수에 넣어 준다.
	order->FilledQty = order->OrderAmount;
	// 잔고 갯수를 넣어 준다.
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	order->RemainQty = order->FilledQty * buho;
	// 여기서 주문 상태를 데이터베이스에 저장해 준다.
	SmMongoDBManager::GetInstance()->OnFilledOrder(order);
	// 포지션을 계산한다.
	CalcPosition(order);
}

void SmSymbolOrderManager::OnOrderSettled(std::shared_ptr<SmOrder> order)
{
	
}

void SmSymbolOrderManager::OnOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 원장 접수 상태
	order->OrderState = SmOrderState::Ledger;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	// 주문 날짜를 넣어 준다.
	order->OrderDate = date_time.first;
	order->OrderTime = date_time.second;
	// 여기서 데이터 베이스에 주문을 저장해 준다.
	SmMongoDBManager::GetInstance()->AddOrder(order);
	// 주문 목록에 넣어 준다.
	SmOrderManager::OnOrder(order);
}

void SmSymbolOrderManager::CalcFee(std::shared_ptr<SmPosition> posi, std::shared_ptr<SmAccount> acnt, std::shared_ptr<SmOrder> filledOrder)
{
	if (!posi || !filledOrder || !acnt)
		return;
	std::shared_ptr<SmFee> fee = std::make_shared<SmFee>();
	fee->AccountNo = filledOrder->AccountNo;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	fee->Date = date_time.first;
	fee->Time = date_time.second;
	fee->SymbolCode = filledOrder->SymbolCode;

	if (std::isdigit(posi->SymbolCode.at(2))) { // 국내 상품
		posi->FeeCount += filledOrder->FilledQty;
		fee->FeeCount = filledOrder->FilledQty;
		LOG_F(INFO, "CalcFee: symbol_code = %s, feeCount = %d", posi->SymbolCode.c_str(), posi->FeeCount);
	}
	else { // 해외 상품
		posi->FeeCount += filledOrder->FilledQty;
		fee->FeeCount = filledOrder->FilledQty;
		LOG_F(INFO, "CalcFee: symbol_code = %s, feeCount = %d", posi->SymbolCode.c_str(), posi->FeeCount);
	}

	// 계좌에 수수료를 저장한다.
	acnt->UpdateFee(fee->FeeCount);
	LOG_F(INFO, "CalcFee2: symbol_code = %s, feeCount = %d", posi->SymbolCode.c_str(), posi->FeeCount);
	// 데이터베이스에 수수료를 저장한다.
	SmMongoDBManager::GetInstance()->SaveFee(fee);
}

void SmSymbolOrderManager::CalcPosition(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmAccountManager* acntMgr = acntMgr->GetInstance();
	std::shared_ptr<SmAccount> acnt = acntMgr->FindAccount(order->AccountNo);
	if (!acnt)
		return;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	SmMongoDBManager* dbMgr = SmMongoDBManager::GetInstance();

	SmTotalPositionManager* posiMgr = SmTotalPositionManager::GetInstance();
	// 계좌에서 현재 포지션을 가져온다.
	std::shared_ptr<SmPosition> posi = posiMgr->FindPosition(order->AccountNo, order->SymbolCode);
	// 포지션을 최신 날짜에 맞게 업데이트 한다.
	posiMgr->CheckUptoDate(posi);
	double curClose = sym->Quote.Close / pow(10, sym->Decimal());
	double current_profit_loss = 0.0;
	// 주문 포지션에 따른 부호 결정
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	if (!posi) { // 포지션이 없는 경우
		posi = posiMgr->CreatePosition(order);
		posi->OpenQty = buho * order->FilledQty;
		posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
	}
	else { // 포지션이 있는 경우
		double filledPrice = order->FilledPrice / pow(10, sym->Decimal());
		if (order->Position == SmPositionType::Sell) { //체결수량이 매도	
			if (posi->OpenQty > 0) { // 보유수량이 매수			
				if (posi->OpenQty >= order->FilledQty) { //보유수량이 크거나 같은 경우
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					// 실현손익 발생 - 데이터베이스에 저장한다.
					current_profit_loss = double(-order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					// 여기서 데이터베이스에 저장을 해준다.
					posi->TradePL += current_profit_loss;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss, order->UserID, order->FilledQty, filledPrice);
				}
				else { //체결수량이 큰 경우
					// 실현손익 발생 - 데이터베이스에 저장한다.
					current_profit_loss = double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->TradePL += current_profit_loss;
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss, order->UserID, posi->OpenQty, filledPrice);
				}
			}
			else { // 보유수량이 매도 ( 보유수량이매도/체결수량이매도 인 경우)
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice - order->FilledQty * filledPrice) / (posi->OpenQty - order->FilledQty));
				posi->OpenQty = posi->OpenQty - order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
			}
		}
		else { //체결수량이 매수
			if (posi->OpenQty >= 0) { // 보유수량이 매수/체결수량이매수 인 경우
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice + order->FilledQty * filledPrice) / (posi->OpenQty + order->FilledQty));
				posi->OpenQty = posi->OpenQty + order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
			}
			else { //보유수량이 매도
				if (abs(posi->OpenQty) >= order->FilledQty) { //보유수량이 큰경우
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					// 실현손익 발생 - 데이터베이스에 저장한다.
					current_profit_loss = double(order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					posi->TradePL += current_profit_loss;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss, order->UserID, order->FilledQty, filledPrice);
				}
				else { //체결수량이 큰 경우		
					// 실현손익 발생 - 데이터베이스에 저장한다.
					current_profit_loss = double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->TradePL += current_profit_loss;
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss, order->UserID, posi->OpenQty, filledPrice);
				}
			}
		}
	}
	// 포지션 현재가를 넣어준다.
	posi->CurPrice = curClose;
	// 주문 잔고를 계산한다.
	int totalRemain = CalcRemainOrder(order);
	if (totalRemain == posi->OpenQty) {
		// 잔고 수량에 따라 포지션을 다시 결정해 준다.
		if (posi->OpenQty > 0) { // 매수 포지션 설정 : 양수임
			posi->Position = SmPositionType::Buy;
		}
		else if (posi->OpenQty < 0) { // 매도 포지션 설정 : 음수임
			posi->Position = SmPositionType::Sell;
		}
		else { // 모든 포지션 상실 0임
			posi->Position = SmPositionType::None;
		}
	}
	else { // 주문 잔고와 포지션 잔고가 맞지 않을 경우 심각한 오류이다.
		int error = -1;
		error = error;
		LOG_F(INFO, "주문계산 오류 발생 계좌 : %s, 종목 : %s", posi->AccountNo.c_str(), posi->SymbolCode.c_str());
	}

	// 체결된 주문만 보낸다. 청산된 주문은 이미 처리되었음.
	if (order->OrderState == SmOrderState::Filled)
		SendResponse(order, SmProtocol::res_order_filled);
	// 포지션에 수수료를 부과한다.
	// 수수료는 현재 체결된 주문의 갯수에 따라 부과된다.
	CalcFee(posi, acnt, order);
	dbMgr->UpdateAccountInfo(acnt);
	// 잔고를 보낸다.
	SendRemain(order, acnt);

	// 여기서 데이터베이스의 포지션을 상태를 업데이트한다.
	SmMongoDBManager::GetInstance()->UpdatePosition(posi);
}
// 이 함수는 남아있는 주문과 새로운 주문 처리를 하여 남아있는 주문을 계산한다.
// 주문은 먼저 들어온 주문이 먼저 처리 된다.
int SmSymbolOrderManager::CalcRemainOrder(std::shared_ptr<SmOrder> newOrder)
{
	// 잔고 주문이 없다면 맨뒤에 추가하고 나간다.
	// 잔고 주문은 오래된 주문이 맨 앞에 있다.
	if (_FilledOrderMap.size() == 0) {
		_FilledOrderMap[newOrder->OrderNo] = newOrder;
		return CalcTotalRemain();
	}
	// 잔고 주문이 남아 있을 경우 처리 - 주문 번호가 빠른 것 부터 처리한다.
	for (auto it = _FilledOrderMap.begin(); it != _FilledOrderMap.end(); ++it) {
		std::shared_ptr<SmOrder> oldOrder = it->second;
		
		// 현재 들어온 주문이 맨 마지막 주문과 같으면 바로 나간다.
		// 새로운 주문은 결과를 보고 밑에서 다시 처리한다.
		if (oldOrder->Position == newOrder->Position) {
			break;
		}
		// 들어온 주문과 현재 주문과 갯수가 같을 경우
		if (std::abs(newOrder->RemainQty) == std::abs(oldOrder->RemainQty)) {
			// 기존 주문에 대한 처리
			// 사용자 아이디 복사
			//oldOrder->UserID = newOrder->UserID;
			// 잔고 초기화
			oldOrder->RemainQty = 0;
			// 청산확인 설정
			oldOrder->OrderState = SmOrderState::Settled;
			// 새로운 주문이 청산 시킨 주문은 목록에 넣어 준다.
			newOrder->SettledOrders.push_back(oldOrder->OrderNo);
			// 청산 주문 메시지를 보낸다.
			SmTotalOrderManager::GetInstance()->SendResponse(oldOrder, SmProtocol::res_order_settled);
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(oldOrder);
			// 체결확인 목록에서 지워준다.
			_FilledOrderMap.erase(it);
			_SettledMap[oldOrder->OrderNo] = oldOrder;
			SmTotalOrderManager::GetInstance()->OnOrderSettled(oldOrder);
			// 새로운 주문에 대한 처리
			// 잔고 설정
			newOrder->RemainQty = 0;
			// 청산확인 설정
			newOrder->OrderState = SmOrderState::Settled;
			// 청산 주문 메시지를 보낸다.
			SmTotalOrderManager::GetInstance()->SendResponse(newOrder, SmProtocol::res_order_settled);
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(newOrder);
			// 청산확인 목록에 추가해 준다.
			SmTotalOrderManager::GetInstance()->OnOrderSettled(newOrder);
			// 청산확인 목록에 추가해 준다.
			_SettledMap[newOrder->OrderNo] = newOrder;
			break;
		} 
		// 신규 주문잔고가 더 클 경우 - 이 경우 신규 주문의 잔고는 줄어든다.
		// 기존 주문은 잔고가 0이 된다.
		else if (std::abs(newOrder->RemainQty) > std::abs(oldOrder->RemainQty)) {
			// 신규 주문 잔고를 조절해 준다.
			newOrder->RemainQty += oldOrder->RemainQty;
			// 기존 주문에 대한 처리
			// 잔고 초기화
			oldOrder->RemainQty = 0;
			// 청산확인 설정
			oldOrder->OrderState = SmOrderState::Settled;
			// 새로운 주문이 청산 시킨 주문은 목록에 넣어 준다.
			newOrder->SettledOrders.push_back(oldOrder->OrderNo);
			// 청산 주문 메시지를 보낸다.
			SmTotalOrderManager::GetInstance()->SendResponse(oldOrder, SmProtocol::res_order_settled);
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(oldOrder);
			// 기존 주문을 목록에서 지워준다.
			it = _FilledOrderMap.erase(it);
			// 청산확인 목록에 추가해 준다.
			_SettledMap[oldOrder->OrderNo] = oldOrder;
			// 청산확인 목록에 추가해 준다.
			SmTotalOrderManager::GetInstance()->OnOrderSettled(oldOrder);
			// iterator를 하나 후퇴시킨다.
			--it;
		} 
		// 신규 주문잔고가 더 작을 경우 - 이 경우 신규 주문은 잔고가 0이된다.
		// 기존 주문은 신규 주문으로 잔고가 줄어든다.
		else if (std::abs(newOrder->RemainQty) < std::abs(oldOrder->RemainQty)) {
			// 기존 주문에 대한 처리
			oldOrder->RemainQty += newOrder->RemainQty;
			// 신규 주문 처리
			// 신규 주문 잔고 초기화
			newOrder->RemainQty = 0;
			// 청산확인 설정
			newOrder->OrderState = SmOrderState::Settled;
			// 청산 주문 메시지를 보낸다.
			SmTotalOrderManager::GetInstance()->SendResponse(oldOrder, SmProtocol::res_order_settled);
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(newOrder);
			// 청산확인 목록에 추가해 준다.
			SmTotalOrderManager::GetInstance()->OnOrderSettled(newOrder);
			// 청산확인 목록에 추가해 준다.
			_SettledMap[newOrder->OrderNo] = newOrder;
			break;
		}
	}

	// 신규 주문이 잔고가 남아 있다면 잔고 목록에 추가한다.
	if (newOrder->RemainQty != 0 && newOrder->OrderState == SmOrderState::Filled) {
		// 체결 목록에 넣어 준다.
		SmOrderManager::OnOrderFilled(newOrder);
	}

	return CalcTotalRemain();
}

int SmSymbolOrderManager::CalcTotalRemain()
{
	int totalRemain = 0;
	for (auto it = _FilledOrderMap.begin(); it != _FilledOrderMap.end(); ++it) {
		totalRemain += it->second->RemainQty;
	}

	return totalRemain;
}

void SmSymbolOrderManager::SendRemain(std::shared_ptr<SmOrder> order, std::shared_ptr<SmAccount> acnt)
{
	if (!order || !acnt)
		return;
	SmTotalPositionManager* totalPosiMgr = SmTotalPositionManager::GetInstance();
	std::shared_ptr<SmPosition> posi = totalPosiMgr->FindPosition(order->AccountNo, order->SymbolCode);

	json send_object;
	send_object["res_id"] = SmProtocol::res_symbol_position;
	send_object["created_date"] = posi->CreatedDate;
	send_object["created_time"] = posi->CreatedTime;
	send_object["symbol_code"] = posi->SymbolCode;
	send_object["fund_name"] = posi->FundName;
	send_object["account_no"] = posi->AccountNo;
	send_object["position_type"] = posi->Position;
	send_object["open_qty"] = posi->OpenQty;
	send_object["symbol_fee_count"] = posi->FeeCount;
	send_object["trade_pl"] = posi->TradePL;
	send_object["avg_price"] = posi->AvgPrice;
	send_object["cur_price"] = posi->CurPrice;
	send_object["open_pl"] = posi->OpenPL;
	send_object["account_fee_count"] = acnt->FeeCount();
	send_object["account_trade_pl"] = acnt->TradePL();
	send_object["account_total_trade_pl"] = acnt->TotalTradePL();

	LOG_F(INFO, "SendRemain: symbol_code = %s, feeCount = %d", posi->SymbolCode.c_str(), posi->FeeCount);

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmSymbolOrderManager::SendResponse(std::shared_ptr<SmOrder> order, SmProtocol protocol)
{
	if (!order)
		return;

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
