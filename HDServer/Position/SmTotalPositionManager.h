#pragma once
#include <string>
#include <map>
#include <vector>
#include "../Global/TemplateSingleton.h"
struct SmPosition;
class SmAccountPositionManager;
struct SmOrder;
class SmAccount;
class SmTotalPositionManager : public TemplateSingleton<SmTotalPositionManager>
{
public:
	~SmTotalPositionManager();
	std::shared_ptr<SmPosition> CreatePosition(std::shared_ptr<SmOrder> order);
	std::shared_ptr<SmPosition> FindPosition(std::string acntNo, std::string symbolCode);
	std::shared_ptr<SmPosition> FindAddPosition(std::string acntNo, std::string symbolCode);
	void AddPosition(std::shared_ptr<SmPosition> posi);
	void SendPositionList(int session_id, std::string account_no);
	void SendPositionList(int session_id, std::vector<std::string> account_no_list);
	void CheckUptoDate(std::shared_ptr<SmPosition> posi);
private:
	std::shared_ptr<SmPosition> CreatePosition(std::string account_no, std::string symbol_code);
	std::map<std::string, SmAccountPositionManager*> _AccountPositionManagerMap;
	SmAccountPositionManager* FindAddAccountPositionManager(std::string accountNo);
};

