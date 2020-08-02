#include "pch.h"
#include "SmTimeSeriesDBManager.h"
#include <ctime>
#include "../Util/VtStringUtil.h"
#include <cstring> //memset
#include <sstream>
#include <iostream>
#include <algorithm>
#include <boost/asio.hpp>
#include "../Json/json.hpp"
#include "../Symbol/SmSymbol.h"
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp> 
#include <boost/iostreams/code_converter.hpp>
#include "../Util/UtfConverter.h"
#include <boost/locale.hpp>
#include <iostream>

namespace io = boost::iostreams;
using namespace nlohmann;

namespace ip = boost::asio::ip;

SmTimeSeriesDBManager::SmTimeSeriesDBManager()
{
	std::vector<std::string> addr_vec = GetIPAddress("signalmaster.iptime.org");
	_Ip = addr_vec[0];
	_Port = 8086;
	_DatabaseName = "abroad_future";
	_Id = "angelpie";
	_Password = "orion1";

	//_ServerInfo = new influxdb_cpp::server_info(_Ip, _Port, _DatabaseName, _Id, _Password);
}

SmTimeSeriesDBManager::SmTimeSeriesDBManager(std::string ip, int port, std::string db, std::string id, std::string pwd)
{
	_Ip = ip;
	_Port = port;
	_DatabaseName = db;
	_Id = id;
	_Password = pwd;

	//_ServerInfo = new influxdb_cpp::server_info(_Ip, _Port, _DatabaseName, _Id, _Password);
}

SmTimeSeriesDBManager::~SmTimeSeriesDBManager()
{
// 	if (_ServerInfo) {
// 		delete _ServerInfo;
// 		_ServerInfo = nullptr;
// 	}
}

std::string SmTimeSeriesDBManager::ExecQuery(std::string query_string)
{
	std::string resp;
// 	if (!_ServerInfo)
// 		return resp;
	//influxdb_cpp::query(resp, query_string, *_ServerInfo);
	return resp;
}

void SmTimeSeriesDBManager::OnChartDataItem(SmChartDataItem data_item)
{
// 	std::string date_time = data_item.date + data_item.time;
// 	std::time_t utc = VtStringUtil::GetUTCTimestamp(date_time);
// 	std::string  meas = data_item.GetDataKey();
// 	std::string resp;
// 	int ret = influxdb_cpp::builder()
// 		.meas(meas)
// 		.tag("symbol_code", data_item.symbolCode)
// 		.tag("chart_type", std::to_string((int)data_item.chartType))
// 		.tag("cycle", std::to_string(data_item.cycle))
// 		.field("local_date", data_item.date)
// 		.field("local_time", data_item.time)
// 		.field("h", data_item.h)
// 		.field("l", data_item.l)
// 		.field("o", data_item.o)
// 		.field("c", data_item.c)
// 		.field("v", data_item.v)
// 		.timestamp(utc * 1000000000)
// 		.post_http(*_ServerInfo, &resp);
}

void SmTimeSeriesDBManager::CreateDataBase(std::string db_name)
{
// 	std::string resp;
// 	if (!_ServerInfo)
// 		return;
// 	influxdb_cpp::create_db(resp, db_name, *_ServerInfo);
}

std::vector<std::string> SmTimeSeriesDBManager::GetIPAddress(std::string host_name)
{
	boost::asio::io_service io_service;
	ip::tcp::resolver resolver(io_service);

	std::string h = host_name;
	std::cout << "Host name is " << h << '\n';
	std::cout << "IP addresses are: \n";
	std::stringstream ss;
	std::vector<std::string> addr_vec;
	std::for_each(resolver.resolve({ h, "" }), {}, [ &ss, &addr_vec](const auto& re) {
		ss << re.endpoint().address();
		std::string addr = ss.str();
		addr_vec.push_back(addr);
		ss.str("");
		});
	return addr_vec;
}

void SmTimeSeriesDBManager::UserTest()
{
// 	std::string resp;
// 	
// 	std::time_t fixed_time = 1559941200;
// 	std::string id = "angelpie";
// 	std::string pwd = "orion1";
// 	std::string cert = "123456";
// 	int ret = influxdb_cpp::builder()
// 		.meas("user_info")
// 		.tag("id", id)
// 		.field("pwd", pwd)
// 		.field("cert", cert)
// 		.timestamp(fixed_time * 1000000000)
// 		.post_http(*_ServerInfo, &resp);
// 	
// 	
// 
// 	std::string qry = "DROP MEASUREMENT \"user_info\"";
// 	//resp = ExecQuery(qry);
// 	// 태크가 있을 때는 반드시 태그외에 필드를 하나는 포함해야 한다.
// 	//The query selects the level description field, the location tag, and the water_level field. Note that the SELECT clause must specify at least one field when it includes a tag.
// 	//std::string query_string = "select \"tag_id\" from \"user_info\" where  \"tag_id\" = \'angelpie\'";
// 	std::string query_string = "select \"id\"  from \"user_info\" where  \"id\" = \'angelpie\'";
// 	resp = ExecQuery(query_string);
// 
// 	std::string result = resp;
// 
// 	auto json_object = json::parse(resp);
// 	auto values = json_object["results"][0]["series"];
// 	if (!values.is_null()) {
// 		auto columns = json_object["results"][0]["series"][0]["columns"];
// 		std::string str_pwd = values[0][1];
// 		auto b = values;
// 	}
// 
// 	values = json_object["results"][0]["series"][0]["values"];
// 	std::string str_id = values[0][1];
}

void SmTimeSeriesDBManager::AddUserToDatabase(std::string id, std::string pwd)
{
// 	std::string resp;
// 
// 	std::time_t fixed_time = 1559941200;
// 	//std::string id = "angelpie";
// 	//std::string pwd = "orion1";
// 	std::string cert = "123456";
// 	int ret = influxdb_cpp::builder()
// 		.meas("user_info")
// 		.tag("id", id)
// 		.field("pwd", pwd)
// 		.field("cert", cert)
// 		.timestamp(fixed_time * 1000000000)
// 		.post_http(*_ServerInfo, &resp);
}

std::pair<std::string, std::string> SmTimeSeriesDBManager::GetUserInfo(std::string id)
{
	// 태크가 있을 때는 반드시 태그외에 필드를 하나는 포함해야 한다.
	//The query selects the level description field, the location tag, and the water_level field. Note that the SELECT clause must specify at least one field when it includes a tag.
	std::string query_string = "select \"id\"  , \"pwd\" from \"user_info\" where  \"id\" = \'" + id + "\'";
	std::string resp = ExecQuery(query_string);
	std::string str_id = "";
	std::string str_pwd = "";
	auto json_object = json::parse(resp);
	auto values = json_object["results"][0]["series"];
	if (values.is_null()) {
		return std::make_pair(str_id, str_pwd);
	}

	values = json_object["results"][0]["series"][0]["values"];
	//str_id = (std::string)(values[0][1]);
	//str_pwd = (std::string)(values[0][2]);
	return std::make_pair(str_id, str_pwd);
}

void SmTimeSeriesDBManager::GetChartData()
{
// 	std::string query_string = ""; 
// 	std::string resp = ExecQuery(query_string);
// 	CString msg;
// 	msg.Format(_T("resp length = %d"), resp.length());
// 	TRACE(msg);
// 	try
// 	{
// 		auto json_object = json::parse(resp);
// 		auto a = json_object["results"][0]["series"][0]["values"];
// 		for (size_t i = 0; i < a.size(); i++) {
// 			auto val = a[i];
// 			std::string time = val[0];
// 			int h = 0, l = 0, o = 0, c = 0, v = 0;
// 			h = val[1];
// 			l = val[4];
// 			o = val[5];
// 			c = val[6];
// 			//v = val[7];
// 
// 			std::string local_time = VtStringUtil::GetLocalTime(time);
// 
// 			msg.Format(_T("index = %d, date_time = %s, lc = %s, h = %d, l = %d, o = %d, c = %d\n"), i, time.c_str(), local_time.c_str(), h, l, o, c);
// 			TRACE(msg);
// 		}
// 	}
// 	catch (const std::exception& e)
// 	{
// 		std::string error = e.what();
// 	}
}

void SmTimeSeriesDBManager::SaveQuoteItem(SmQuote&& qitem)
{
// 	auto cur_date_time = VtStringUtil::GetCurrentDateTime();
// 	std::string date_time = cur_date_time.first + cur_date_time.second;
// 	std::time_t utc = VtStringUtil::GetUTCTimestamp(date_time);
// 	long long nanos = VtStringUtil::GetCurrentNanoseconds();
// 	std::string  meas = qitem.SymbolCode + "_quote";
// 	std::string resp;
// 	int ret = influxdb_cpp::builder()
// 		.meas(meas)
// 		.tag("symbol_code", qitem.SymbolCode)
// 		.field("sign_to_preday", qitem.SignToPreDay)
// 		.field("to_preday", qitem.GapFromPreDay)
// 		.field("ratio_to_preday", qitem.RatioToPreday)
// 		.field("sign", qitem.Sign)
// 		.field("origin_time", qitem.OriginTime)
// 		.field("h", qitem.High)
// 		.field("l", qitem.Low)
// 		.field("o", qitem.Open)
// 		.field("c", qitem.Close)
// 		.field("v", qitem.Volume)
// 		.timestamp(utc * 1000000000 + nanos)
// 		.post_http(*_ServerInfo, &resp);
}

void SmTimeSeriesDBManager::SaveCurrentQuoteItem(SmQuote&& qitem)
{
// 	auto cur_date_time = VtStringUtil::GetCurrentDateTime();
// 	std::time_t fixed_time = 1559941200;
// 	std::string date_time = cur_date_time.first + cur_date_time.second;
// 	std::time_t utc = VtStringUtil::GetUTCTimestamp(date_time);
// 	long long nanos = VtStringUtil::GetCurrentNanoseconds();
// 	std::string  meas = qitem.SymbolCode + "_current";
// 	std::string resp;
// 	int ret = influxdb_cpp::builder()
// 		.meas(meas)
// 		.tag("symbol_code", qitem.SymbolCode)
// 		.field("sign_to_preday", qitem.SignToPreDay)
// 		.field("to_preday", qitem.GapFromPreDay)
// 		.field("ratio_to_preday", qitem.RatioToPreday)
// 		.field("sign", qitem.Sign)
// 		.field("origin_time", qitem.OriginTime)
// 		.field("h", qitem.High)
// 		.field("l", qitem.Low)
// 		.field("o", qitem.Open)
// 		.field("c", qitem.Close)
// 		.field("v", qitem.Volume)
// 		.timestamp(fixed_time * 1000000000)
// 		.post_http(*_ServerInfo, &resp);
}

void SmTimeSeriesDBManager::SaveHogaItem(SmHoga&& qitem)
{
// 	auto cur_date_time = VtStringUtil::GetCurrentDateTime();
// 	std::string date_time = cur_date_time.first + cur_date_time.second;
// 	std::time_t utc = VtStringUtil::GetUTCTimestamp(date_time);
// 	long long nanos = VtStringUtil::GetCurrentNanoseconds();
// 	std::string  meas = qitem.SymbolCode + "_hoga";
// 	std::string resp;
// 	int ret = influxdb_cpp::builder()
// 		.meas(qitem.SymbolCode)
// 		.tag("symbol_code", qitem.SymbolCode)
// 		.field("domestic_date", qitem.DomesticDate)
// 		.field("domestic_time", qitem.DomesticTime)
// 		.field("server_time", qitem.Time)
// 		.field("total_buy_qty", qitem.TotBuyQty)
// 		.field("total_buy_cnt", qitem.TotBuyCnt)
// 		.field("total_sell_qty", qitem.TotSellQty)
// 		.field("total_sell_cnt", qitem.TotSellCnt)
// 		.field("buy_hoga_price1", qitem.Ary[0].BuyPrice)
// 		.field("buy_hoga_qty1", qitem.Ary[0].BuyQty)
// 		.field("buy_hoga_cnt1", qitem.Ary[0].BuyCnt)
// 		.field("sell_hoga_price1", qitem.Ary[0].SellPrice)
// 		.field("sell_hoga_qty1", qitem.Ary[0].SellQty)
// 		.field("sell_hoga_cnt1", qitem.Ary[0].SellCnt)
// 
// 		.field("buy_hoga_price2", qitem.Ary[1].BuyPrice)
// 		.field("buy_hoga_qty2", qitem.Ary[1].BuyQty)
// 		.field("buy_hoga_cnt2", qitem.Ary[1].BuyCnt)
// 		.field("sell_hoga_price2", qitem.Ary[1].SellPrice)
// 		.field("sell_hoga_qty2", qitem.Ary[1].SellQty)
// 		.field("sell_hoga_cnt2", qitem.Ary[1].SellCnt)
// 
// 		.field("buy_hoga_price3", qitem.Ary[2].BuyPrice)
// 		.field("buy_hoga_qty3", qitem.Ary[2].BuyQty)
// 		.field("buy_hoga_cnt3", qitem.Ary[2].BuyCnt)
// 		.field("sell_hoga_price3", qitem.Ary[2].SellPrice)
// 		.field("sell_hoga_qty3", qitem.Ary[2].SellQty)
// 		.field("sell_hoga_cnt3", qitem.Ary[2].SellCnt)
// 
// 		.field("buy_hoga_price4", qitem.Ary[3].BuyPrice)
// 		.field("buy_hoga_qty4", qitem.Ary[3].BuyQty)
// 		.field("buy_hoga_cnt4", qitem.Ary[3].BuyCnt)
// 		.field("sell_hoga_price4", qitem.Ary[3].SellPrice)
// 		.field("sell_hoga_qty4", qitem.Ary[3].SellQty)
// 		.field("sell_hoga_cnt4", qitem.Ary[3].SellCnt)
// 
// 		.field("buy_hoga_price5", qitem.Ary[4].BuyPrice)
// 		.field("buy_hoga_qty5", qitem.Ary[4].BuyQty)
// 		.field("buy_hoga_cnt5", qitem.Ary[4].BuyCnt)
// 		.field("sell_hoga_price5", qitem.Ary[4].SellPrice)
// 		.field("sell_hoga_qty5", qitem.Ary[4].SellQty)
// 		.field("sell_hoga_cnt5", qitem.Ary[4].SellCnt)
// 		.timestamp(utc * 1000000000 + nanos)
// 		.post_http(*_ServerInfo, &resp);
}

void SmTimeSeriesDBManager::SaveSymbol(SmSymbol* sym)
{
// 	if (!sym)
// 		return;
// 	std::time_t utc = VtStringUtil::GetUTCTimestampByDate();
// 	std::string name_kr = sym->Name();
// 	std::wstring utf8_name;
// 	utf8_name.assign(name_kr.begin(), name_kr.end());
// 	std::string utf8_string = UtfConverter::ToUtf8(utf8_name);
// 	std::string  meas = "symbol_master";
// 	std::string resp;
// 	int ret = influxdb_cpp::builder()
// 		.meas(meas)
// 		.tag("symbol_code", sym->SymbolCode())
// 		.field("name_kr", utf8_string)
// 		.field("name_en", sym->NameEn())
// 		.field("decimal", sym->Decimal())
// 		.field("seungsu", sym->Seungsu())
// 		.field("contract_unit", sym->CtrUnit())
// 		.field("tick_size", sym->TickSize())
// 		.field("tick_value", sym->TickValue())
// 		.timestamp(utc * 1000000000)
// 		.post_http(*_ServerInfo, &resp);
}

void SmTimeSeriesDBManager::GetSymbolMaster(std::string symCode)
{
// 	std::string query_string = "";
// 	std::string str_cycle = std::to_string(1);
// 	std::string str_chart_type = std::to_string(5);
// 	query_string.append("SELECT * FROM \"");
// 	query_string.append("symbol_master");
// 	query_string.append("\" WHERE \"symbol_code\" = \'");
// 	query_string.append(symCode);
// 	query_string.append("\'");
// 	std::string resp = ExecQuery(query_string);
// 	CString msg;
// 	msg.Format(_T("resp length = %d"), resp.length());
// 	TRACE(msg);
// 	try
// 	{
// 		auto json_object = json::parse(resp);
// 		auto a = json_object["results"][0]["series"][0]["values"];
// 		for (size_t i = 0; i < a.size(); i++) {
// 			auto val = a[i];
// 			std::string time = val[0];
// 			int h = 0, l = 0, o = 0, c = 0, v = 0;
// 			h = val[1];
// 			l = val[2];
// 			o = val[5];
// 			c = val[8];
// 			//v = val[7];
// 
// 			std::string local_time = VtStringUtil::GetLocalTime(time);
// 
// 			msg.Format(_T("index = %d, date_time = %s, lc = %s, h = %d, l = %d, o = %d, c = %d\n"), i, time.c_str(), local_time.c_str(), h, l, o, c);
// 			TRACE(msg);
// 		}
// 	}
// 	catch (const std::exception& e)
// 	{
// 		std::string error = e.what();
// 	}
}

void SmTimeSeriesDBManager::DeleteMeasurement(std::string measure_name)
{
	std::string query_string = "DROP MEASUREMENT ";
	query_string.append("\"");
	query_string.append(measure_name);
	query_string.append("\"");
	std::string resp = ExecQuery(query_string);
}
