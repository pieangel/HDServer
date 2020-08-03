#include "pch.h"
#include "SmChartData.h"
#include "../User/SmUserManager.h"
#include "../Database/SmTimeSeriesDBManager.h"
#include "../Json/json.hpp"
#include "../Log/loguru.hpp"
#include "../HDCtrl/SmHdClient.h"
#include "../Util/SmUtil.h"
#include "../Util/VtStringUtil.h"
#include "../Service/SmServiceDefine.h"

using namespace nlohmann;
void SmChartData::GetChartDataFromDB()
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string;// = "show series on abroad_future";
	query_string.append("SELECT \"c\", \"h\", \"l\", \"o\", \"v\" FROM \"");
	query_string.append(GetDataKey());
	query_string.append("\" WHERE ");
	std::string resp = dbMgr->ExecQuery(query_string);
	CString msg;
	msg.Format("resp len = %d", resp.length());
	TRACE(msg);
	try
	{
		auto json_object = json::parse(resp);
		auto it = json_object.find("error");
		if (it != json_object.end()) {
			std::string err_msg = json_object["error"];
			TRACE(err_msg.c_str());
			LOG_F(INFO, "Query Error", err_msg);
			return;
		}
		auto series = json_object["results"][0]["series"];
		if (series.is_null()) {
			
			return;
		}
		auto a = json_object["results"][0]["series"][0]["values"];
		
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
		LOG_F(INFO, "Query Error", error);
	}
}

void SmChartData::GetChartDataFromServer()
{
	SmChartDataRequest req;
	req.reqType = SmChartDataReqestType::FIRST;
	req.symbolCode = _SymbolCode;
	req.chartType = _ChartType;
	req.cycle = _Cycle;
	req.count = _DataQueueSize;
	req.next = 0;
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(req);
}

void SmChartData::GetCyclicDataFromServer()
{
	SmChartDataRequest req;
	req.reqType = SmChartDataReqestType::CYCLE;
	req.symbolCode = _SymbolCode;
	req.chartType = _ChartType;
	req.cycle = _Cycle;
	req.count = _CycleDataSize;
	req.next = 0;
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(req);
}

void SmChartData::SendCyclicChartDataToUsers()
{
	CString msg;
	msg.Format("차트데이터 업데이트 됨 %d\n", 0);
	//TRACE(msg);

	json send_object;
	send_object["res_id"] = SmProtocol::res_chart_cycle_data;
	send_object["symbol_code"] = _SymbolCode;
	send_object["chart_type"] = (int)_ChartType;
	send_object["cycle"] = _Cycle;
	send_object["total_count"] = _CycleDataSize;
	auto it = _DataItemList.begin();
	std::advance(it, _DataItemList.size() - _CycleDataSize);
	int k = 0;
	// 가장 최근데이터를 보낸다. 가장 최근 데이터를 먼저 넣는다.
	for (; it != _DataItemList.end(); ++it) {
		SmChartDataItem item = *it;
		std::string time = item.date + item.time;
		CString msg;
		msg.Format("send cycle data ::symbol = %s, dt = %s , o = %d, h = %d, l= %d, c = %d\n", _SymbolCode.c_str(), time.c_str(), item.o, item.h, item.l, item.c);
		//TRACE(msg);
		std::string date_time = VtStringUtil::GetLocalTimeByDatetimeString(time);
		send_object["data"][k++] = {
			{ "date_time",  date_time },
			{ "high", item.h },
			{ "low",  item.l },
			{ "open",  item.o },
			{ "close",  item.c },
			{ "volume",  item.v }
		};
		if (k == _CycleDataSize)
			break;
	}

	std::string content = send_object.dump(4);
	SmUserManager* userMgr = SmUserManager::GetInstance();
	// 등록된 유저들에게 차트 데이터를 보낸다.
	for (auto it = _UserList.begin(); it != _UserList.end(); ++it) {
		userMgr->SendResultMessage(*it, content);
	}
}

std::vector<double> SmChartData::GetClose()
{
	std::vector<double> data_list;
	for(auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		data_list.push_back(data.c);
	}
	return data_list;
}

std::vector<double> SmChartData::GetOpen()
{
	std::vector<double> data_list;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		data_list.push_back(data.c);
	}
	return data_list;
}

std::vector<double> SmChartData::GetHigh()
{
	std::vector<double> data_list;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		data_list.push_back(data.h);
	}
	return data_list;
}

std::vector<double> SmChartData::GetLow()
{
	std::vector<double> data_list;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		data_list.push_back(data.l);
	}
	return data_list;
}

std::vector<double> SmChartData::GetVolume()
{
	std::vector<double> data_list;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		data_list.push_back(data.v);
	}
	return data_list;
}

void SmChartData::AddChartData(SmChartDataItem&& data)
{
	std::string date_time;
	date_time.append(data.date);
	date_time.append(data.time);
	_DataMap[date_time] = data;

	// 일정한 갯수가 넘어가면 이전 데이터는 제거해 준다.
	if (_DataMap.size() > 3) {
		auto it = _DataMap.begin();
		_DataMap.erase(it);
	}
}

void SmChartData::AddData(SmChartDataItem& data_item)
{
	
	try
	{
		auto it = _DataMap.find(data_item.date_time);
		if (it != _DataMap.end()) {
			SmChartDataItem& data = it->second;
			data.c = data_item.c;
			data.o = data_item.o;
			data.h = data_item.h;
			data.l = data_item.l;
			data.v = data_item.v;
			return;
		}
		_DataMap.insert(std::make_pair(data_item.date_time, data_item));
		
		size_t count = _DataMap.size();
		if (count > _DataQueueSize) {
			// 큐의 크기를 넘어서면 맨 과거 데이터를 제거해 준다.
			auto it = _DataMap.begin();
			_DataMap.erase(it);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
	
}

std::pair<int, int> SmChartData::GetCycleByTimeDif()
{
	if (_DataItemList.size() < 2)
		return std::make_pair(0,0);
	// 맨마지막 요소를 가져온다.
	auto it = _DataItemList.rbegin();
	SmChartDataItem newItem = *it++;
	SmChartDataItem oldItem = *it;

	std::string new_datetime = newItem.date + newItem.time;
	std::string old_datetime = oldItem.date + oldItem.time;
	// 주기를 초로 환산함
	double cycleSeconds = SmUtil::GetDifTimeBySeconds(new_datetime, old_datetime);
	// 현재 시간에서 마지막 시간까지 대기해야 하는 시간을 초로 환산함
	double waitSeconds = SmUtil::GetDifTimeForNow(new_datetime);
	return std::make_pair((int)cycleSeconds, (int)waitSeconds);
}

void SmChartData::OnChartDataUpdated()
{
	int k = 0;
	CString msg;
	for (auto it = _DataItemList.rbegin(); it != _DataItemList.rend(); ++it) {
		SmChartDataItem item = *it;
		msg.Format(_T(">>>>>>>>>>>>>>>>>>> size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), item.date.c_str(), item.time.c_str(), item.o, item.h, item.l, item.c, item.v);
		//TRACE(msg);

		k++;

		if (k == _CycleDataSize)
			break;
	}
	// 여기서 새로운 데이터를 보낸다.
	SendCyclicChartDataToUsers();
}

void SmChartData::PushChartDataItemToBack(SmChartDataItem data)
{
	CString msg;
	

	_DataItemList.push_back(data);
	if (_DataItemList.size() > _DataQueueSize) {
		_DataItemList.pop_front();
	}

	//msg.Format(_T("pushed data :: size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
	//TRACE(msg);

	//GetCycleByTimeDif();
}

void SmChartData::PushChartDataItemToFront(SmChartDataItem data)
{
	CString msg;


	_DataItemList.push_front(data);
	if (_DataItemList.size() > _DataQueueSize) {
		_DataItemList.pop_back();
	}

	//msg.Format(_T("pushed data :: size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
	//TRACE(msg);

	//GetCycleByTimeDif();
}

void SmChartData::UpdateChartData(SmChartDataItem data)
{
	// 부정한 데이터가 있을 때는 업데이트 하지 않는다.
	if (data.o == 0 || data.h == 0 || data.l == 0 || data.c == 0)
		return;

	CString msg;
	msg.Format(_T("UpdateChartData:: size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
	//TRACE(msg);
	int count = 0;
	for (auto it = _DataItemList.rbegin(); it != _DataItemList.rend(); ++it) {
		SmChartDataItem& item = *it;
		std::string old_date_time = item.date + item.time;
		std::string new_date_time = data.date + data.time;
		// 데이터가 있으면 값을 갱신하고 빠져 나간다.
		if (old_date_time.compare(new_date_time) == 0) {
			item.h = data.h;
			item.l = data.l;
			item.o = data.o;
			item.c = data.c;
			item.v = data.v;
			return;
		}
		count++;
		if (count == _CycleDataSize)
			break;
	}

	//SmChartDataItem last = _DataItemList.back();
	//std::string last_date_time = last.date + last.time;
	//std::string new_date_time = data.date + data.time;
	//if (new_date_time.compare(last_date_time) <= 0)
	//	return;


	for (auto it = _DataItemList.end(); it != _DataItemList.begin(); --it) {
		auto index = it;
		if (index == _DataItemList.begin())
			break;

		--index;
		SmChartDataItem& item = *index;
		std::string old_date_time = item.date + item.time;
		std::string new_date_time = data.date + data.time;
		// 날짜가 더 큰 경우 삽입하고 빠져 나간다.
		if (new_date_time.compare(new_date_time) > 0) {
			_DataItemList.insert(it, data);
			break;
		}
	}

	// 데이터가 없으면 새로우 데이터로 간주하고 맨 뒤(가장 최신 데이터)에 붙인다.
	_DataItemList.push_back(data);
	if (_DataItemList.size() > _DataQueueSize) {
		_DataItemList.pop_front();
	}
	
}

void SmChartData::UpdateChartData(SmQuoteData tick_data)
{
	std::string time = tick_data.time.substr(0, 4);
	int close = std::stoi(tick_data.close);
	auto it = _DataMap.find(time);
	if (it == _DataMap.end()) {
		return;
	}

	SmChartDataItem& item = it->second;
	if (item.o == 0) {
		item.o = close;
	}
	item.c = close;
	if (item.h == 0)
		item.h = close;
	else {
		if (close > item.h)
			item.h = close;
	}

	if (item.l == 0)
		item.l = close;
	else {
		if (close < item.l)
			item.l = close;
	}
}

void SmChartData::RemoveUser(std::string user_id)
{
	auto it = _UserList.find(user_id);
	if (it != _UserList.end()) {
		_UserList.erase(it);
	}
}

void SmChartData::OnTimer()
{
	GetCyclicDataFromServer();
}

SmChartDataItem* SmChartData::GetChartDataItem(std::string date_time)
{
	auto it = _DataMap.find(date_time);
	if (it != _DataMap.end())
		return &it->second;

	return nullptr;
}
