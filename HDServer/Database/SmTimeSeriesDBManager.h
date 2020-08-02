#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include <vector>
#include "../Chart/SmChartDefine.h"
#include "../Symbol/SmQuoteDefine.h"
#include "../Symbol/SmHogaDefine.h"
namespace influxdb_cpp
{
	struct server_info;
};
class SmSymbol;
class SmTimeSeriesDBManager : public TemplateSingleton<SmTimeSeriesDBManager>
{
public:
	SmTimeSeriesDBManager(std::string ip, int port, std::string db, std::string id, std::string pwd);
	SmTimeSeriesDBManager();
	~SmTimeSeriesDBManager();
	std::string Id() const { return _Id; }
	void Id(std::string val) { _Id = val; }
	std::string Password() const { return _Password; }
	void Password(std::string val) { _Password = val; }
	std::string DatabaseName() const { return _DatabaseName; }
	void DatabaseName(std::string val) { _DatabaseName = val; }
	std::string Ip() const { return _Ip; }
	void Ip(std::string val) { _Ip = val; }
	int Port() const { return _Port; }
	void Port(int val) { _Port = val; }
	std::string ExecQuery(std::string query_string);
	influxdb_cpp::server_info* ServerInfo() const { return _ServerInfo; }
	void OnChartDataItem(SmChartDataItem data_item);
	void CreateDataBase(std::string db_name);
	std::vector<std::string> GetIPAddress(std::string host_name);
	void UserTest();
	void AddUserToDatabase(std::string id, std::string pwd);
	std::pair<std::string, std::string> GetUserInfo(std::string id);
	void GetChartData();
	void SaveQuoteItem(SmQuote&& qitem);
	void SaveCurrentQuoteItem(SmQuote&& qitem);
	void SaveHogaItem(SmHoga&& qitem);
	void SaveSymbol(SmSymbol* sym);
	void GetSymbolMaster(std::string symCode);
	void DeleteMeasurement(std::string measure_name);
private:
	std::string _Id;
	std::string _Password;
	std::string _DatabaseName;
	std::string _Ip;
	int         _Port;
	influxdb_cpp::server_info* _ServerInfo = nullptr;
};

