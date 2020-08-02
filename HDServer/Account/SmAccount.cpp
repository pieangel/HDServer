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
	// �Ÿŷ� �̷���� ���� - ���� �Ÿŷ� ���� ����
	_TradePL = 0.0;
	// ���� û����� ���� �ֹ����� ���� �� ����
	_OpenPL = 0.0;
	// ������ �Ÿ� ���� - �ʱ� �ں��ݿ��� ���۵� ���� ��� ������ ���� ����
	_TotalTradePL = 0.0;
	// ���� ���� �����Ḧ ��Ÿ����. ���� ������� �ٸ��� Ȯ���� �� �ִ�.
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