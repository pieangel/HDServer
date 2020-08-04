#include "pch.h"
#include "SmTimeSeriesCollector.h"
#include "../Market/SmMarketManager.h"
#include <chrono>
#include <vector>
#include "../Symbol/SmSymbol.h"
#include "../Json/json.hpp"
#include <ctime>
#include "../Database/SmTimeSeriesDBManager.h"
#include "../Util/VtStringUtil.h"
#include "../User/SmUserManager.h"
#include "../Service/SmTimeSeriesServiceManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Market/SmMarketManager.h"
#include "../Util/SmUtil.h"
#include "../Symbol/SmSymbol.h"
using namespace nlohmann;


SmTimeSeriesCollector::SmTimeSeriesCollector()
{
	GetRecentMonthSymbolList();
}

SmTimeSeriesCollector::~SmTimeSeriesCollector()
{
	
}

void SmTimeSeriesCollector::CollectRecentMonthSymbolChartData()
{
	if (_Index >= _RecentMonthSymbolVec.size())
		return;
	std::shared_ptr<SmSymbol> sym = _RecentMonthSymbolVec[_Index];
	SmChartDataRequest req;
	req.symbolCode = sym->SymbolCode();
	req.chartType = SmChartType::MIN;
	req.cycle = 1;
	req.count = 1500;
	req.next = 0;
	//SmHdClient* client = SmHdClient::GetInstance();
	//client->GetChartData(req);
	_Index++;
	if (_Index == _RecentMonthSymbolVec.size()) {
		_Timer.remove(_ChartDataTimerId);
	}
}

void SmTimeSeriesCollector::OnChartDataItem(SmChartDataItem data_item)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	dbMgr->OnChartDataItem(data_item);
}

void SmTimeSeriesCollector::StartCollectChartData()
{
	int waitTime = 2;
	_ChartDataTimerId = _Timer.add(std::chrono::seconds(waitTime - 1), [this](CppTime::timer_id) { OnTimer(); }, std::chrono::seconds(3));
}

void SmTimeSeriesCollector::StartCollectSiseData()
{
	_SiseTimerId = _Timer.add(std::chrono::seconds(0), [this](CppTime::timer_id) { OnSiseTimer(); }, std::chrono::seconds(1));
}

void SmTimeSeriesCollector::StartCollectHogaData()
{
	_HogaTimerId = _Timer.add(std::chrono::seconds(0), [this](CppTime::timer_id) { OnHogaTimer(); }, std::chrono::seconds(1));
}

void SmTimeSeriesCollector::GetChartData(SmChartDataRequest&& data_req)
{
	//SmHdClient* client = SmHdClient::GetInstance();
	//client->GetChartData(data_req);
}

void SmTimeSeriesCollector::GetRecentMonthSymbolList()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	_RecentMonthSymbolVec = mrktMgr->GetRecentMonthSymbolList();
}

void SmTimeSeriesCollector::OnTimer()
{
	CollectRecentMonthSymbolChartData();
}

void SmTimeSeriesCollector::OnEveryMinute()
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	_RecentMonthSymbolVec = mrktMgr->GetRecentMonthSymbolList();
	std::string curTime = SmUtil::GetUTCDateTimeStringForNowMin();
	std::string prevTime = SmUtil::GetUTCDateTimeStringForPreMin(2);

	for (auto it = _RecentMonthSymbolVec.begin(); it != _RecentMonthSymbolVec.end(); ++it) {
		std::shared_ptr<SmSymbol> sym = *it;
		std::string  meas = sym->SymbolCode() + "_quote";

		std::string query_string = ""; // "select * from \"chart_data\" where \"symbol_code\" = \'CLN19\' AND \"chart_type\" = \'5\' AND \"cycle\" = \'1\'";
		std::string str_cycle = std::to_string(1);
		std::string str_chart_type = std::to_string(5);
		query_string.append("SELECT FIRST(\"c\"),  LAST(\"c\"), MIN(\"c\"), MAX(\"c\") FROM \"");
		query_string.append(meas);
		query_string.append("\" WHERE time >= \'");
		query_string.append(prevTime);
		query_string.append("\' AND time <= \'");
		query_string.append(curTime);
		query_string.append("\' GROUP BY time(1m) fill(previous)");
		std::string resp = dbMgr->ExecQuery(query_string);
		CString msg;
		msg.Format(_T("resp length = %d"), resp.length());
		TRACE(msg);
	}
}

void SmTimeSeriesCollector::OnSiseTimer()
{
	if (_SiseIndex >= _RecentMonthSymbolVec.size())
		return;
	std::shared_ptr<SmSymbol> sym = _RecentMonthSymbolVec[_SiseIndex];
	//SmHdClient * client = SmHdClient::GetInstance();
	//client->GetSiseData(sym->SymbolCode());
	_SiseIndex++;
	if (_SiseIndex == _RecentMonthSymbolVec.size()) {
		_Timer.remove(_SiseTimerId);
	}
}

void SmTimeSeriesCollector::OnHogaTimer()
{
	if (_HogaIndex >= _RecentMonthSymbolVec.size())
		return;
	std::shared_ptr<SmSymbol> sym = _RecentMonthSymbolVec[_HogaIndex];
	//SmHdClient* client = SmHdClient::GetInstance();
	//client->GetHogaData(sym->SymbolCode());
	_HogaIndex++;
	if (_HogaIndex == _RecentMonthSymbolVec.size()) {
		_Timer.remove(_HogaTimerId);
	}
}
