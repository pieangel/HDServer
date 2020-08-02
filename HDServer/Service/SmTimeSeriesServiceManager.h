#pragma once
#include "../Global/TemplateSingleton.h"
#include "../Chart/SmChartDefine.h"
#include <vector>
#include <map>
#include <string>
#include "../Timer/cpptime.h"
#include "SmServieReqNumGenerator.h"

class SmChartData;
class SmWebsocketSession;
class SmTimeSeriesServiceManager : public TemplateSingleton<SmTimeSeriesServiceManager>
{
public:
	SmTimeSeriesServiceManager();
	~SmTimeSeriesServiceManager();
	void OnUnregisterCycleDataRequest(SmChartDataRequest&& data_req);
	void OnRegisterCycleDataRequest(SmChartDataRequest&& data_req);
	void OnChartDataRequest(SmChartDataRequest&& data_req);
	void OnChartDataReceived(SmChartDataRequest&& data_req);
	void OnSiseDataRequest(SmSiseDataRequest&& sise_req);
	void OnHogaDataRequest(SmHogaDataRequest&& hoga_req);
	void OnSymbolMasterRequest(SmSymbolMasterRequest&& master_req);
	int SendDataSplitSize() const { return _SendDataSplitSize; }
	void SendDataSplitSize(int val) { _SendDataSplitSize = val; }
	void OnCompleteChartData(SmChartDataRequest data_req, std::shared_ptr<SmChartData> chart_data);
	//SmWebsocketSession* SisiSocket() const { return _SisiSocket; }
	//void SisiSocket(SmWebsocketSession* val) { _SisiSocket = val; }
	void ClearSiseSocket(SmWebsocketSession* session);
	void OnReqRegisterSiseSocket(SmWebsocketSession* socket);
	void ResendChartDataRequest(SmChartDataRequest req);
	void ResendChartCycleDataRequest(SmChartDataRequest req);
	void SendChartData(SmChartDataRequest data_req, std::shared_ptr<SmChartData> chart_data);
	void SendChartData(SmChartDataRequest data_req, SmChartDataItem item);
	void SendChartData(int session_id, SmChartDataItem item);
	void BroadcastChartData(SmChartDataItem item);
	void SendChartData(int session_id, std::shared_ptr<SmChartData> chart_data);
	std::vector<SmWebsocketSession*> SiseSocketVector() const { return _SiseSocketVector; }
	void SiseSocketVector(std::vector<SmWebsocketSession*> val) { _SiseSocketVector = val; }
	int GetSiseSocketCount() {
		return _SiseSocketVector.size();
	}
private:
	SmServieReqNumGenerator _SvcNoGen;
	void RegisterCycleChartDataRequest(SmChartDataRequest data_req);
	void SendChartData(std::vector<SmSimpleChartDataItem>& dataVec, SmChartDataRequest req, int totalCount, int startIndex, int endIndex);
	std::map<int, SmChartDataRequest> _HistoryDataReqMap;
	std::map<std::string, CppTime::timer_id> _CycleDataReqTimerMap;
	int _SendDataSplitSize = 20;
	CppTime::Timer _Timer;
	std::map<std::string, std::shared_ptr<SmChartData>> _CycleDataReqMap;
	std::shared_ptr<SmChartData> AddCycleDataReq(SmChartDataRequest data_req);
	void RegisterTimer(std::shared_ptr<SmChartData> chartData);
	void SendChartDataFromDB(SmChartDataRequest&& data_req);
	// 증권사나 데이터 제공 업체에 직접 데이터를 요청한다.
	void GetChartDataFromSourceServer(SmChartDataRequest&& data_req);
	
	//SmWebsocketSession* _SisiSocket = nullptr;
	std::vector<SmWebsocketSession*>  _SiseSocketVector;
	void SendRequestToSiseServer(std::string message);
	std::mutex _mutex;
};

