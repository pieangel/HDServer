//#include "pch.h"
#include "SmTimeSeriesServiceManager.h"
#include "../Database/SmTimeSeriesDBManager.h"
#include "../Util/VtStringUtil.h"
#include "../User/SmUser.h"
#include "../User/SmUserManager.h"
#include "../Json/json.hpp"
#include <vector>
#include "../HDCtrl/SmHdClient.h"
#include "../Log/loguru.hpp"
#include "../Chart/SmChartData.h"
#include "../Util/SmUtil.h"
#include <chrono>
#include "../Chart/SmChartDataManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Service/SmServiceDefine.h"
#include "../Util/SmUtfUtil.h"
#include "../Server/SmWebsocketSession.h"
#include "../Server/SmSessionManager.h"
#include "../Global/SmGlobal.h"
#include "../Database/SmMongoDBManager.h"

using namespace std::chrono;
using namespace nlohmann;

SmTimeSeriesServiceManager::SmTimeSeriesServiceManager()
{
	
}

SmTimeSeriesServiceManager::~SmTimeSeriesServiceManager()
{

}

void SmTimeSeriesServiceManager::OnUnregisterCycleDataRequest(SmChartDataRequest&& data_req)
{
	// 기존 목록에 요청이 있는지 확인한다.
	auto it = _CycleDataReqMap.find(data_req.GetDataKey());
	if (it != _CycleDataReqMap.end()) {
		return;
	}

	std::shared_ptr<SmChartData> chart_data = it->second;
	chart_data->RemoveUser(data_req.user_id);
	size_t user_count = chart_data->GetUserCount();
	if (user_count == 0) {
		// 사용자가 없을 경우에는 타이머를 정지시켜 준다.
		auto tit = _CycleDataReqTimerMap.find(data_req.GetDataKey());
		if (tit != _CycleDataReqTimerMap.end()) {
			auto timer_id = tit->second;
			_Timer.remove(timer_id);
			_CycleDataReqTimerMap.erase(tit);
		}
		// 차트 객체도 삭제해 준다.
		_CycleDataReqMap.erase(it);
	}
}

void SmTimeSeriesServiceManager::OnRegisterCycleDataRequest(SmChartDataRequest&& data_req)
{
	auto it = _CycleDataReqMap.find(data_req.GetDataKey());
	// 이미 차트 데이터에 대한 요청이 있는 경우에는 그 차트데이터에 사용자 아이디만 추가한다.
	if (it != _CycleDataReqMap.end()) {
		it->second->AddUser(data_req.user_id);
		return;
	}
	// 차트 데이터를 등록해 준다.
	std::shared_ptr<SmChartData> chart_data = AddCycleDataReq(data_req);
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(data_req);
	// 차트 데이터 타이머 서비스를 등록해 준다.
	RegisterTimer(chart_data);
}

void SmTimeSeriesServiceManager::OnChartDataRequest(SmChartDataRequest&& data_req)
{
	SendChartDataFromDB(std::move(data_req));
}

void SmTimeSeriesServiceManager::RegisterCycleChartDataRequest(SmChartDataRequest data_req)
{
	auto it = _CycleDataReqMap.find(data_req.GetDataKey());
	// 이미 차트 데이터에 대한 요청이 있는 경우에는 그 차트데이터에 사용자 아이디만 추가한다.
	if (it != _CycleDataReqMap.end()) {
		it->second->AddUser(data_req.user_id);
		return;
	}
	// 차트 데이터를 등록해 준다.
	std::shared_ptr<SmChartData> chart_data = AddCycleDataReq(data_req);
	// 차트 데이터 타이머 서비스를 등록해 준다.
	RegisterTimer(chart_data);
}

void SmTimeSeriesServiceManager::SendChartData(SmChartDataRequest data_req, std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return;
	int split_size = _SendDataSplitSize;
	int cur_count = 0;
	int start_index = 0;
	int end_index = 0;
	std::vector<SmSimpleChartDataItem> dataVec;
	std::map<std::string, SmChartDataItem>& data = chart_data->GetDataItemList();
	int i = 0;
	for (auto it = data.begin(); it != data.end(); ++it) {
		end_index = i;
		SmChartDataItem srcItem = it->second;
		std::string date_time = srcItem.date + srcItem.time;
		std::string local_date_time = VtStringUtil::GetLocalTimeByDatetimeString(date_time);
		SmSimpleChartDataItem item;
		item.date_time = date_time;
		item.c = srcItem.c;
		item.h = srcItem.h;
		item.l = srcItem.l;
		item.o = srcItem.o;
		item.v = srcItem.v;
		dataVec.push_back(item);
		cur_count++;
		if (cur_count % split_size == 0) {
			SendChartData(dataVec, data_req, data.size(), start_index, end_index);
			if (end_index != data.size() - 1)
				start_index = end_index;
			dataVec.clear();
		}
		i++;
	}
	if (data.size() % split_size != 0) {
		SendChartData(dataVec, data_req, data.size(), start_index, end_index);
	}
}

void SmTimeSeriesServiceManager::SendChartData(SmChartDataRequest data_req, SmChartDataItem item)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_chart_data_onebyone;
	send_object["total_count"] = item.total_count;
	send_object["current_count"] = item.current_count;
	send_object["data_key"] = data_req.GetDataKey();
	send_object["symbol_code"] = data_req.symbolCode;
	send_object["chart_type"] = data_req.chartType;
	send_object["cycle"] = data_req.cycle;
	send_object["date_time"] = item.date + item.time;
	send_object["o"] = item.o;
	send_object["h"] = item.h;
	send_object["l"] = item.l;
	send_object["c"] = item.c;
	send_object["v"] = item.v;
	std::string values;
	values.append(std::to_string(item.o));
	values.append(",");
	values.append(std::to_string(item.h));
	values.append(",");
	values.append(std::to_string(item.l));
	values.append(",");
	values.append(std::to_string(item.c));
	send_object["values"] = values;
	
	std::string content = send_object.dump();
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(data_req.session_id, content);
}

void SmTimeSeriesServiceManager::SendChartData(int session_id, SmChartDataItem item)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_chart_data_onebyone;
	send_object["total_count"] = item.total_count;
	send_object["current_count"] = item.current_count;
	send_object["data_key"] = item.GetDataKey();
	send_object["symbol_code"] = item.symbolCode;
	send_object["chart_type"] = item.chartType;
	send_object["cycle"] = item.cycle;
	send_object["date_time"] = item.date_time;
	send_object["o"] = item.o;
	send_object["h"] = item.h;
	send_object["l"] = item.l;
	send_object["c"] = item.c;
	send_object["v"] = item.v;
	std::string values;
	values.append(std::to_string(item.o));
	values.append(",");
	values.append(std::to_string(item.h));
	values.append(",");
	values.append(std::to_string(item.l));
	values.append(",");
	values.append(std::to_string(item.c));
	send_object["values"] = values;

	std::string content = send_object.dump();
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(session_id, content);
}

void SmTimeSeriesServiceManager::SendChartData(int session_id, std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return;
	std::map<std::string, SmChartDataItem>& dataMap = chart_data->GetDataMap();
	int total_count = (int)dataMap.size();
	int current_count = 1;
	for (auto element : dataMap) {
		SmChartDataItem item = element.second;
		item.total_count = total_count;
		item.current_count = current_count++;
		SendChartData(session_id, item);
	}
}

void SmTimeSeriesServiceManager::BroadcastChartData(SmChartDataItem item)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_chart_cycle_data;
	send_object["data_key"] = item.GetDataKey();
	send_object["symbol_code"] = item.symbolCode;
	send_object["chart_type"] = item.chartType;
	send_object["cycle"] = item.cycle;
	send_object["date_time"] = item.date_time;
	send_object["o"] = item.o;
	send_object["h"] = item.h;
	send_object["l"] = item.l;
	send_object["c"] = item.c;
	send_object["v"] = item.v;
	std::string values;
	values.append(std::to_string(item.o));
	values.append(",");
	values.append(std::to_string(item.h));
	values.append(",");
	values.append(std::to_string(item.l));
	values.append(",");
	values.append(std::to_string(item.c));
	send_object["values"] = values;

	std::string content = send_object.dump();
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(content);
}

void SmTimeSeriesServiceManager::ResendChartDataRequest(SmChartDataRequest req)
{
	int service_req_id = _SvcNoGen.GetID();
	_HistoryDataReqMap[service_req_id] = req;
	json send_object;
	send_object["req_id"] = (int)SmProtocol::req_chart_data_from_main_server;
	send_object["req_session_id"] = req.session_id;
	send_object["symbol_code"] = req.symbolCode;
	send_object["chart_type"] = (int)req.chartType;
	send_object["cycle"] = req.cycle;
	send_object["count"] = req.count;
	std::string content = send_object.dump(4);
	char buffer[512];
	sprintf(buffer, "%s", req.GetDataKey().c_str());

	OutputDebugString(buffer);
	LOG_F(INFO, "ResendChartDataRequest", buffer);
	SendRequestToSiseServer(content);
}

void SmTimeSeriesServiceManager::ResendChartCycleDataRequest(SmChartDataRequest req)
{
	int service_req_id = _SvcNoGen.GetID();
	_HistoryDataReqMap[service_req_id] = req;
	json send_object;
	send_object["req_id"] = (int)SmProtocol::req_register_chart_cycle_data;
	send_object["symbol_code"] = req.symbolCode;
	send_object["chart_type"] = (int)req.chartType;
	send_object["cycle"] = req.cycle;
	send_object["count"] = req.count;
	std::string content = send_object.dump(4);
	char buffer[512];
	sprintf(buffer, "%s", req.GetDataKey().c_str());

	OutputDebugString(buffer);
	LOG_F(INFO, "ResendChartCycleDataRequest", buffer);
	SendRequestToSiseServer(content);
}

void SmTimeSeriesServiceManager::OnChartDataReceived(SmChartDataRequest&& data_req)
{
	//OnChartDataRequest(std::move(data_req));
}

void SmTimeSeriesServiceManager::OnSiseDataRequest(SmSiseDataRequest&& sise_req)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(sise_req.symbol_code);
	if (!sym)
		return;
	//std::string content = sym->GetQuoteByJson();
	//SmUserManager* userMgr = SmUserManager::GetInstance();
	//userMgr->SendResultMessage(sise_req.user_id, content);
	std::string symbol_code = sise_req.symbol_code;
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->SendQuote(symbol_code);
}

void SmTimeSeriesServiceManager::OnHogaDataRequest(SmHogaDataRequest&& hoga_req)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(hoga_req.symbol_code);
	if (!sym)
		return;
	std::string symbol_code = hoga_req.symbol_code;
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->SendHoga(symbol_code);
}

void SmTimeSeriesServiceManager::OnSymbolMasterRequest(SmSymbolMasterRequest&& master_req)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(master_req.symbol_code);
	if (!sym)
		return;
	json send_object;
	send_object["res_id"] = SmProtocol::res_symbol_master;
	send_object["symbol_code"] = sym->SymbolCode();
	send_object["name_kr"] = SmUtfUtil::AnsiToUtf8((char*)sym->Name().c_str());
	send_object["name_en"] = sym->NameEn().c_str();
	send_object["category_code"] = sym->CategoryCode();
	send_object["market_name"] = SmUtfUtil::AnsiToUtf8((char*)sym->MarketName().c_str());
	send_object["decimal"] = sym->Decimal();
	send_object["contract_unit"] = sym->CtrUnit();
	send_object["seungsu"] = sym->Seungsu();
	send_object["tick_size"] = sym->TickSize();
	send_object["tick_value"] = sym->TickValue();

	std::string content = send_object.dump(4);
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(master_req.user_id, content);
}

void SmTimeSeriesServiceManager::OnCompleteChartData(SmChartDataRequest data_req, std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return;
	// 차트데이터를 받았음을 표시해 준다.
	chart_data->Received(true);
	// 주기데이터를 먼저 등록해 준다.
	RegisterCycleChartDataRequest(data_req);
	// 차트데이터를 보낸다.
	SendChartData(std::move(data_req), chart_data);
}

void SmTimeSeriesServiceManager::SendChartData(std::vector<SmSimpleChartDataItem>& dataVec, SmChartDataRequest req, int totalCount, int startIndex, int endIndex)
{
	if (dataVec.size() == 0)
		return;

	//CString msg;
	json send_object;
	send_object["res_id"] = SmProtocol::res_chart_data;
	send_object["chart_id"] = req.chart_id;
	send_object["symbol_code"] = req.symbolCode;
	send_object["chart_type"] = (int)req.chartType;
	send_object["cycle"] = req.cycle;
	send_object["total_count"] = totalCount;
	send_object["cur_count"] = (int)dataVec.size();
	send_object["start_index"] = startIndex;
	send_object["end_index"] = endIndex;
	for (size_t i = 0; i < dataVec.size(); ++i) {
		SmSimpleChartDataItem item = dataVec[i];
		std::string date = item.date_time;
		send_object["data"][i] = {
			{ "date_time",  date },
			{ "high", item.h },
			{ "low",  item.l },
			{ "open",  item.o },
			{ "close",  item.c },
			{ "volume",  item.v }
		};
		
		//msg.Format("SendChartData :: date_time = %s\n", date.c_str());
		//TRACE(msg);
	}

	std::string content = send_object.dump();
	//SmUserManager* userMgr = SmUserManager::GetInstance();
	//userMgr->SendResultMessage(req.user_id, content);
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(req.session_id, content);
}

std::shared_ptr<SmChartData> SmTimeSeriesServiceManager::AddCycleDataReq(SmChartDataRequest data_req)
{
	SmChartDataManager* chrartDataMgr = SmChartDataManager::GetInstance();
	std::shared_ptr<SmChartData> chartData = chrartDataMgr->AddChartData(data_req);
	chartData->AddUser(data_req.user_id);
	_CycleDataReqMap[data_req.GetDataKey()] = chartData;

	return chartData;
}

void SmTimeSeriesServiceManager::RegisterTimer(std::shared_ptr<SmChartData> chartData)
{
	if (!chartData)
		return;
	// 주기 데이터 목록에서 찾아 봐서 없으면 수행하지 않는다.
	auto it = _CycleDataReqTimerMap.find(chartData->GetDataKey());
	if (it != _CycleDataReqTimerMap.end())
		return;
	std::pair<int, int> timer_times = chartData->GetCycleByTimeDif();
	// 주기가 0이면 오류 이므로 처리하지 않는다.
	if (timer_times.first == 0)
		return;
	// 대기시간 
	int waitTime = timer_times.second;
	// 주기를 초로 환산해서 대입한다.
	// Add to the timer.
	auto id = _Timer.add(seconds(waitTime), std::bind(&SmChartData::OnTimer, chartData), seconds(timer_times.first));
	// Add to the request map.
	_CycleDataReqTimerMap[chartData->GetDataKey()] = id;
}

void SmTimeSeriesServiceManager::SendChartDataFromDB(SmChartDataRequest&& data_req)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string;// = "show series on abroad_future";
	query_string.append("SELECT \"c\", \"h\", \"l\", \"o\", \"v\" , \"local_date\" , \"local_time\" FROM \"");
	query_string.append(data_req.GetDataKey());
	query_string.append("\" ");
	query_string.append("LIMIT ");
	query_string.append(std::to_string(data_req.count));
	//query_string.append("where time >= '2019-06-05T07:12:00Z'");
	std::string resp = dbMgr->ExecQuery(query_string);
	//CString msg;
	//msg.Format("resp len = %d", resp.length());
	//TRACE(msg);
	SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
	std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(data_req);
	try
	{
		auto json_object = json::parse(resp);
		auto it = json_object.find("error");
		if (it != json_object.end()) {
			std::string err_msg = json_object["error"];
			//TRACE(err_msg.c_str());
			LOG_F(INFO, "Query Error", err_msg);
			return;
		}
		auto series = json_object["results"][0]["series"];
		if (series.is_null()) {
			ResendChartDataRequest(data_req);
			return;
		}
		auto a = json_object["results"][0]["series"][0]["values"];
		int split_size = _SendDataSplitSize;
		int cur_count = 0;
		int start_index = 0;
		int end_index = 0;
		std::vector<SmSimpleChartDataItem> dataVec;
		//SmUserManager* userMgr = SmUserManager::GetInstance();
		for (size_t i = 0; i < a.size(); i++) {
			end_index = i;
			auto val = a[i];
			std::string time = val[0];
			std::string local_date_time = VtStringUtil::GetLocalTime(time);
			SmSimpleChartDataItem item;
			item.date_time = local_date_time;
			item.c = val[1];
			item.h = val[2];
			item.l = val[3];
			item.o = val[4];
			item.v = val[5];
			std::string local_date = "";
			std::string local_time = "";
			if (!val[6].is_null())
				local_date = val[6].get<std::string>();
			if (!val[7].is_null())
				local_time = val[7].get<std::string>();
			//msg.Format(_T("index = %d, datetime = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), i, local_date_time.c_str(), item.o, item.h, item.l, item.c, item.v);
			//TRACE(msg);

			SmChartDataItem data;
			data.symbolCode = data_req.symbolCode;
			data.chartType = data_req.chartType;
			data.cycle = data_req.cycle;
			data.date = local_date;
			data.time = local_time;
			data.h = item.h;
			data.l = item.l;
			data.o = item.o;
			data.c = item.c;
			data.v = item.v;
			chart_data->PushChartDataItemToBack(data);

			dataVec.push_back(item);
			cur_count++;
			if (cur_count % split_size == 0) {
				SendChartData(dataVec, data_req, a.size(), start_index, end_index);
				if (end_index != a.size() - 1)
					start_index = end_index;
				dataVec.clear();
			}
		}
		if (a.size() % split_size != 0) {
			SendChartData(dataVec, data_req, a.size(), start_index, end_index);
		}
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
		LOG_F(INFO, "Query Error", error);
	}
}

void SmTimeSeriesServiceManager::GetChartDataFromSourceServer(SmChartDataRequest&& data_req)
{
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(data_req);
}


void SmTimeSeriesServiceManager::ClearSiseSocket(SmWebsocketSession* session)
{
	if (!session || !_SiseSocketVector.size() == 0)
		return;
	
	for (auto it = _SiseSocketVector.begin(); it != _SiseSocketVector.end(); ++it) {
		SmWebsocketSession* socket = *it;
		if (session->SessionID() == socket->SessionID()) {
			_SiseSocketVector.erase(it);
		}
	}
}


void SmTimeSeriesServiceManager::SendRequestToSiseServer(std::string message)
{
	if (_SiseSocketVector.size() == 0)
		return;

	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		v.reserve(_SiseSocketVector.size());
		for (auto it = _SiseSocketVector.begin(); it != _SiseSocketVector.end(); ++it) {
			SmWebsocketSession* socket = *it;
			v.emplace_back(socket->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
	// pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);
}

void SmTimeSeriesServiceManager::OnReqRegisterSiseSocket(SmWebsocketSession* socket)
{
	if (!socket)
		return;
	// 시세 소켓 타입으로 전환시켜 준다.
	socket->Type(1);
	_SiseSocketVector.push_back(socket);

	std::string result_message = "sise socket registered successfully!";

	json res = {
		{"res_id", (int)SmProtocol::res_register_sise_socket},
		{"result_msg", result_message}
	};

	std::string message = res.dump(4);

	SendRequestToSiseServer(message);
}
