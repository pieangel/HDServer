#include "pch.h"
#include "SmTotalPositionManager.h"
#include "SmPosition.h"
#include "../Order/SmOrder.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "SmAccountPositionManager.h"
#include "../Util/VtStringUtil.h"
#include "../Database/SmMongoDBManager.h"
#include "../Service/SmServiceDefine.h"
#include "../Global/SmGlobal.h"
#include "../Server/SmSessionManager.h"
#include "../Json/json.hpp"
using namespace nlohmann;
SmTotalPositionManager::~SmTotalPositionManager()
{
	for (auto it = _AccountPositionManagerMap.begin(); it != _AccountPositionManagerMap.end(); ++it) {
		delete it->second;
	}
}

std::shared_ptr<SmPosition> SmTotalPositionManager::CreatePosition(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return nullptr;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return nullptr;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();

	std::shared_ptr<SmPosition> position = std::make_shared<SmPosition>();
	position->CreatedDate = date_time.first;
	position->CreatedTime = date_time.second;
	position->AccountNo = order->AccountNo;
	position->SymbolCode = order->SymbolCode;
	position->Position = order->Position;
	position->OpenQty = order->FilledQty;
	position->AvgPrice = order->FilledPrice / pow(10, sym->Decimal());
	position->CurPrice = order->FilledPrice / pow(10, sym->Decimal());
	position->FundName = order->FundName;
	position->UserID = order->UserID;

	AddPosition(position);

	return position;
}

std::shared_ptr<SmPosition> SmTotalPositionManager::CreatePosition(std::string account_no, std::string symbol_code)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(symbol_code);
	if (!sym)
		return nullptr;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();

	std::shared_ptr<SmPosition> position = std::make_shared<SmPosition>();
	position->CreatedDate = date_time.first;
	position->CreatedTime = date_time.second;
	position->AccountNo = account_no;
	position->SymbolCode =symbol_code;

	AddPosition(position);

	return position;
}

std::shared_ptr<SmPosition> SmTotalPositionManager::FindPosition(std::string acntNo, std::string symbolCode)
{
	auto it = _AccountPositionManagerMap.find(acntNo);
	if (it == _AccountPositionManagerMap.end())
		return nullptr;
	SmAccountPositionManager* acntPosiMgr = it->second;
	return acntPosiMgr->FindPosition(symbolCode);
}

std::shared_ptr<SmPosition> SmTotalPositionManager::FindAddPosition(std::string acntNo, std::string symbolCode)
{
	std::shared_ptr<SmPosition> posi = FindPosition(acntNo, symbolCode);
	if (!posi) {
		posi = CreatePosition(acntNo, symbolCode);
	}

	return posi;
}

void SmTotalPositionManager::AddPosition(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;
	SmAccountPositionManager* acntPosiMgr = FindAddAccountPositionManager(posi->AccountNo);
	acntPosiMgr->AddPosition(posi);
	// 여기서 데이터 베이스에 포지션을 저장해 준다.
	SmMongoDBManager::GetInstance()->AddPosition(posi);
}

void SmTotalPositionManager::SendPositionList(int session_id, std::string account_no)
{
	std::vector<std::shared_ptr<SmPosition>> position_list = SmMongoDBManager::GetInstance()->GetPositionList(account_no);
	json send_object;
	send_object["res_id"] = SmProtocol::res_position_list;
	send_object["total_position_count"] = (int)position_list.size();
	for (size_t j = 0; j < position_list.size(); ++j) {
		std::shared_ptr<SmPosition> position = position_list[j];
		// 잔고가 있는 것만 보낸다.
		if (position->OpenQty != 0) {
			send_object["position"][j] = {
				{ "created_date",  position->CreatedDate },
				{ "created_time", position->CreatedTime },
				{ "symbol_code",  position->SymbolCode },
				{ "position_type",  (int)position->Position },
				{ "account_no",  position->AccountNo },
				{ "open_qty",  position->OpenQty },
				{ "fee_count",  position->FeeCount },
				{ "trade_profitloss",  position->TradePL },
				{ "average_price",  position->AvgPrice },
				{ "current_price",  position->CurPrice },
				{ "open_profitloss",  position->OpenPL }
			};
		}
	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

void SmTotalPositionManager::SendPositionList(int session_id, std::vector<std::string> account_no_list)
{
	std::vector<std::shared_ptr<SmPosition>> total_position_list;
	for (size_t i = 0; i < account_no_list.size(); ++i) {
		std::vector<std::shared_ptr<SmPosition>> order_list = SmMongoDBManager::GetInstance()->GetPositionList(account_no_list[i]);
		total_position_list.insert(total_position_list.end(), order_list.begin(), order_list.end());
	}

	json send_object;
	send_object["res_id"] = SmProtocol::res_position_list;
	int total_count = 0;
	for (size_t j = 0; j < total_position_list.size(); ++j) {
		std::shared_ptr<SmPosition> position = total_position_list[j];
		// 잔고가 있는 것만 보낸다.
		if (position->OpenQty != 0) {
			send_object["position"][total_count] = {
				{ "created_date",  position->CreatedDate },
				{ "created_time", position->CreatedTime },
				{ "symbol_code",  position->SymbolCode },
				{ "position_type",  (int)position->Position },
				{ "account_no",  position->AccountNo },
				{ "open_qty",  position->OpenQty },
				{ "fee_count",  position->FeeCount },
				{ "trade_profitloss",  position->TradePL },
				{ "average_price",  position->AvgPrice },
				{ "current_price",  position->CurPrice },
				{ "open_profitloss",  position->OpenPL }
			};
			total_count++;
		}
	}

	send_object["total_position_count"] = total_count;

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

void SmTotalPositionManager::CheckUptoDate(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	if (posi->CreatedDate.compare(date_time.first) != 0) {
		posi->CreatedDate = date_time.first;
		posi->CreatedTime = date_time.second;
		SmMongoDBManager::GetInstance()->UpdatePosition(posi);
	}

}

SmAccountPositionManager* SmTotalPositionManager::FindAddAccountPositionManager(std::string accountNo)
{
	auto it = _AccountPositionManagerMap.find(accountNo);
	if (it != _AccountPositionManagerMap.end())
		return it->second;
	SmAccountPositionManager* acntPosiMgr = new SmAccountPositionManager();
	acntPosiMgr->AccountNo(accountNo);
	_AccountPositionManagerMap[accountNo] = acntPosiMgr;
	return acntPosiMgr;
}
