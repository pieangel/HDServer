#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "../Global/TemplateSingleton.h"
class SmSymbol;
class SmUser;
class SmRealtimeSymbolServiceManager : public TemplateSingleton<SmRealtimeSymbolServiceManager>
{
private:
	std::mutex _mutex;
	std::shared_ptr<SmSymbol> _Symbol = nullptr;
	std::map<std::string, std::shared_ptr<SmUser>> _UserMap;
	typedef std::map<std::shared_ptr<SmUser>, std::shared_ptr<SmUser>> SmUserMap;
	std::map<std::shared_ptr<SmSymbol>, SmUserMap> _ServiceMap;
public:
	void RegisterAllRecentSymbol();
	void RegisterSymbol(std::string user_id, std::string symCode);
	void UnregisterSymbol(std::string user_id, std::string symCode);
	void UnregisterAllSymbol(std::string user_id);


	void Register(std::shared_ptr<SmUser> user);
	void Unregister(std::string user_id);
	void SendInfo();
	std::shared_ptr<SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<SmSymbol> val) { _Symbol = val; }
	void BroadcastSise();
	void BroadcastHoga();
private:
	void RegisterSymbol(std::shared_ptr<SmUser> user, std::shared_ptr<SmSymbol> sym);
	void UnregisterSymbol(std::shared_ptr<SmUser> user, std::shared_ptr<SmSymbol> sym);
	void UnregisterAllSymbol(std::shared_ptr<SmUser> user);

	void SendSise(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap);
	void SendHoga(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap);
};

