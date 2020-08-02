#pragma once
#include "../Global/TemplateSingleton.h"
#include <vector>
#include <string>
#include <map>
#include "ThreadPool.h"
#include "../Chart/SmChartDefine.h"
class SmChartData;
class SmAIIndicatorManager : public TemplateSingleton<SmAIIndicatorManager>
{
public:
	SmAIIndicatorManager();
	~SmAIIndicatorManager();

public:
	std::vector<int> GetAIIndicator(std::string symbol_code, int session_id);
	void RequestIndicator(SmIndicatorRequest req);
	void OnReceivedChartData(std::shared_ptr<SmChartData> chart_data);
private:
	void SendResult(int result, SmIndicatorRequest ind_req);
	void RequestChartData(SmIndicatorRequest ind_req);
	void CalcIndicator(std::shared_ptr<SmChartData> chart_data, SmIndicatorRequest ind_req);
	ThreadPool* _Pool = nullptr;
	std::vector<int> GetChartData(std::vector<std::shared_ptr<SmChartData>> chart_vector, int session_id);
	std::map<std::string, SmIndicatorRequest> _RequestMap;
};

