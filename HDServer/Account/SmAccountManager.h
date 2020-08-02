#pragma once
#include "../Global/TemplateSingleton.h"
#include <map>
#include <string>
#include "SmAccountNumGenerator.h"
class SmAccount;
class SmAccountManager : public TemplateSingleton<SmAccountManager>
{
public:
	SmAccountManager();
	~SmAccountManager();
	void AddAccount(std::shared_ptr<SmAccount> account);
	std::shared_ptr<SmAccount> AddAccount(std::string accountNo, std::string accountName, std::string userID);
	std::shared_ptr<SmAccount> FindAccount(std::string accountNo);
	//std::shared_ptr<SmAccount> FindAddAccount(std::string accountNo);
	void LoadAccountFromDB();
	// 계좌를 생성한다.
	// type : 0 -> 해외, 1 -> 국내
	std::shared_ptr<SmAccount> CreateAccount(std::string user_id, std::string password, int type = 0);
	void SendAccountList(int session_id, std::string user_id);
	int ResetAccount(std::string account_no);
private:
	SmAccountNumGenerator _NumGen;
	std::map<std::string, std::shared_ptr<SmAccount>> _AccountMap;
};

