#pragma once
#include <string>
#include <map>
#include <memory>
struct SmPosition;
class SmAccountPositionManager
{
public:
	~SmAccountPositionManager();
	std::shared_ptr<SmPosition> FindPosition(std::string symbolCode);
	void AddPosition(std::shared_ptr<SmPosition> posi);
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
private:
	std::string _AccountNo;
	std::map<std::string, std::shared_ptr<SmPosition>> _PositionMap;
};

