#include "pch.h"
#include "SmAccount.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Order/SmOrder.h"
#include "../Position/SmPosition.h"
#include "../Service/SmServiceDefine.h"
#include "../Position/SmFee.h"
#include "../Position/SmTotalPositionManager.h"
#include "../Json/json.hpp"
using namespace nlohmann;

void SmAccount::Reset()
{
	_InitialBalance = 100000000;
	// 매매로 이루어진 수익 - 당일 매매로 인한 수익
	_TradePL = 0.0;
	// 현재 청산되지 않은 주문으로 인한 평가 손익
	_OpenPL = 0.0;
	// 누적된 매매 수익 - 초기 자본금에서 시작된 이후 모든 수익을 더한 값임
	_TotalTradePL = 0.0;
	// 당일 통합 수수료를 나타낸다. 누적 수수료는 다르게 확인할 수 있다.
	_FeeCount = 0;
}

std::string SmAccount::GetAccountInfoByJSon()
{
	json account;
	account["res_id"] = SmProtocol::res_account_list;
	account["account_no"] = _AccountNo;
	account["account_name"] = _AccountName;
	account["user_id"] = _UserID;
	account["password"] = _Password;
	account["initial_balance"] = _InitialBalance;
	account["trade_profit_loss"] = _TradePL;
	account["open_profit_loss"] = _OpenPL;
	account["total_trade_profit_loss"] = _TotalTradePL;
	
	return account.dump();
}