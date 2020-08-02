#include "SmRealtimeSymbolServiceManager.h"
#include "../User/SmUser.h"
#include "../Server/SmWebsocketSession.h"
#include "../Symbol/SmSymbolManager.h"
#include "../User/SmUserManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Market/SmMarketManager.h"
#include "../HDCtrl/SmHdClient.h"

void SmRealtimeSymbolServiceManager::RegisterAllRecentSymbol()
{
	SmHdClient* client = SmHdClient::GetInstance();
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::vector<std::shared_ptr<SmSymbol>> symVec = mrktMgr->GetRecentMonthSymbolList();
	for (auto it = symVec.begin(); it != symVec.end(); ++it) {
		client->RegisterProduct((*it)->SymbolCode());
	}
}

void SmRealtimeSymbolServiceManager::RegisterSymbol(std::string user_id, std::string symCode)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	std::shared_ptr<SmUser> user = userMgr->FindUser(user_id);
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(symCode);
	if (!user || !sym)
		return;
	RegisterSymbol(user, sym);
}

void SmRealtimeSymbolServiceManager::RegisterSymbol(std::shared_ptr<SmUser> user, std::shared_ptr<SmSymbol> sym)
{
	if (!user || !sym)
		return;
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto it = _ServiceMap.find(sym);
		if (it != _ServiceMap.end()) {
			SmUserMap& userMap = it->second;
			userMap[user] = user;
		}
		else {
			SmUserMap userMap;
			userMap[user] = user;
			_ServiceMap[sym] = userMap;
		}
	}
}

void SmRealtimeSymbolServiceManager::UnregisterSymbol(std::string user_id, std::string symCode)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	std::shared_ptr<SmUser> user = userMgr->FindUser(user_id);
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(symCode);
	if (!user || !sym)
		return;
	UnregisterSymbol(user, sym);
}

void SmRealtimeSymbolServiceManager::UnregisterSymbol(std::shared_ptr<SmUser> user, std::shared_ptr<SmSymbol> sym)
{
	if (!user || !sym)
		return;
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto it = _ServiceMap.find(sym);
		if (it != _ServiceMap.end()) {
			SmUserMap& userMap = it->second;
			auto itu = userMap.find(user);
			if (itu != userMap.end()) {
				userMap.erase(itu);
			}
		}
	}
}

void SmRealtimeSymbolServiceManager::UnregisterAllSymbol(std::string user_id)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	std::shared_ptr<SmUser> user = userMgr->FindUser(user_id);
	if (!user)
		return;
	UnregisterAllSymbol(user);
}

void SmRealtimeSymbolServiceManager::UnregisterAllSymbol(std::shared_ptr<SmUser> user)
{
	if (!user)
		return;
	{
		std::lock_guard<std::mutex> lock(_mutex);

		for (auto it = _ServiceMap.begin(); it != _ServiceMap.end(); ++it) {
			SmUserMap& userMap = it->second;
			auto itu = userMap.find(user);
			if (itu != userMap.end()) {
				userMap.erase(itu);
			}
		}
	}
}

void SmRealtimeSymbolServiceManager::Register(std::shared_ptr<SmUser> user)
{
	if (!user)
		return;
	_UserMap[user->Id()] = user;
}

void SmRealtimeSymbolServiceManager::Unregister(std::string user_id)
{
	auto it = _UserMap.find(user_id);
	if (it != _UserMap.end()) {
		_UserMap.erase(it);
	}
}

void SmRealtimeSymbolServiceManager::SendInfo()
{
	
}

void SmRealtimeSymbolServiceManager::BroadcastSise()
{
	for (auto it = _ServiceMap.begin(); it != _ServiceMap.end(); ++it) {
		SendSise(it->first, it->second);
	}
}

void SmRealtimeSymbolServiceManager::BroadcastHoga()
{
	for (auto it = _ServiceMap.begin(); it != _ServiceMap.end(); ++it) {
		SendHoga(it->first, it->second);
	}
}

void SmRealtimeSymbolServiceManager::SendSise(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap)
{
	
}

void SmRealtimeSymbolServiceManager::SendHoga(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap)
{
	
}
