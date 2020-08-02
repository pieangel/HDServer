#include "pch.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
#include "../Database/SmMongoDBManager.h"
#include <set>
#include "../User/SmUserManager.h"
#include "../Global/SmGlobal.h"
#include "../Server/SmSessionManager.h"
#include "../Service/SmServiceDefine.h"
#include "../Util/SmUtfUtil.h"
#include "../Json/json.hpp"
using namespace nlohmann;

SmAccountManager::SmAccountManager()
{

}

SmAccountManager::~SmAccountManager()
{

}

std::shared_ptr<SmAccount> SmAccountManager::AddAccount(std::string accountNo, std::string accountName, std::string userID)
{
	std::shared_ptr<SmAccount> acnt = FindAccount(accountNo);
	if (acnt)
		return acnt;

	acnt = std::make_shared<SmAccount>();
	acnt->AccountNo(accountNo);
	acnt->AccountName(accountName);
	acnt->UserID(userID);
	_AccountMap[accountNo] = acnt;
	return acnt;
}

void SmAccountManager::AddAccount(std::shared_ptr<SmAccount> account)
{
	if (!account)
		return;
	_AccountMap[account->AccountNo()] = account;

}

std::shared_ptr<SmAccount> SmAccountManager::FindAccount(std::string accountNo)
{
	auto it = _AccountMap.find(accountNo);
	if (it != _AccountMap.end()) {
		return it->second;
	}

	return nullptr;
}

/*
std::shared_ptr<SmAccount> SmAccountManager::FindAddAccount(std::string accountNo)
{
	auto it = _AccountMap.find(accountNo);
	if (it != _AccountMap.end())
		return it->second;
	std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
	acnt->AccountNo(accountNo);
	_AccountMap[accountNo] = acnt;
	return acnt;
}
*/

void SmAccountManager::LoadAccountFromDB()
{
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->LoadAccountList();
}

std::shared_ptr<SmAccount> SmAccountManager::CreateAccount(std::string user_id, std::string password, int type)
{
	std::string account_no = _NumGen.GetNewAccountNumber(type);
	std::shared_ptr<SmAccount> acnt = FindAccount(account_no);
	if (acnt)
		return acnt;

	acnt = std::make_shared<SmAccount>();
	// ���� ����
	acnt->AccountType(type);
	// ���� ��ȣ
	acnt->AccountNo(account_no);
	// ����� ���̵�
	acnt->UserID(user_id);
	// ���� ��й�ȣ
	acnt->Password(password);
	// �ʿ� �����Ѵ�.
	_AccountMap[account_no] = acnt;
	// �⺻�����Ḧ �����Ѵ�.
	if (type == 0) // �ؿ� ������
		acnt->Fee(4); // 4�޷�
	else
		acnt->Fee(1000); // 1000��

	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->SaveAccountInfo(acnt);

	return acnt;
}

void SmAccountManager::SendAccountList(int session_id, std::string user_id)
{
	std::vector<std::shared_ptr<SmAccount>> account_list = SmMongoDBManager::GetInstance()->GetAccountList(user_id);
	json send_object;
	send_object["res_id"] = SmProtocol::res_account_list;
	send_object["total_account_count"] = (int)account_list.size();
	for (size_t j = 0; j < account_list.size(); ++j) {
		std::shared_ptr<SmAccount> acnt = account_list[j];
		send_object["account"][j] = {
			{ "account_type",  acnt->AccountType() },
			{ "account_no",  acnt->AccountNo() },
			{ "account_name", SmUtfUtil::AnsiToUtf8((char*)acnt->AccountName().c_str()) },
			{ "user_id",  acnt->UserID() },
			{ "password",  acnt->Password() },
			{ "initial_balance",  acnt->InitialBalance() },
			{ "trade_profit_loss",  acnt->TradePL() },
			{ "open_profit_loss",  acnt->OpenPL() },
			{ "fee_count",  acnt->FeeCount() },
			{ "total_trade_pl",  acnt->TotalTradePL() }
		};
		
	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}

int SmAccountManager::ResetAccount(std::string account_no)
{
	int result = 0;

	std::shared_ptr<SmAccount> account = FindAccount(account_no);
	// ���� ���� ���� 
	if (!account)
		return -1;
	// ���¸� �ʱ�ȭ �Ѵ�.
	account->Reset();
	// �����ͺ��̽��� ������ ������Ʈ �Ѵ�. 
	SmMongoDBManager::GetInstance()->UpdateAccountInfo(account);

	return result;
}
