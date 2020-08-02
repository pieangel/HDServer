#pragma once
#include <vector>
#include <map>
#include "../Global/TemplateSingleton.h"
#include <string>
#include <map>
#include "ThreadPool.h"
#include "../Chart/SmChartDefine.h"
class SmChartData;
class SmCorrelationManager : public TemplateSingleton<SmCorrelationManager>
{
public:
	SmCorrelationManager();
	~SmCorrelationManager();
	int CheckStochastic(std::shared_ptr<SmChartData> chart_data);
	int CheckMacd(std::shared_ptr<SmChartData> chart_data);
	int CheckSar(std::shared_ptr<SmChartData> chart_data);
	int CheckSma(std::shared_ptr<SmChartData> chart_data);
	int CheckChannel(std::shared_ptr<SmChartData> chart_data);
	int CheckCompositeMa(std::shared_ptr<SmChartData> chart_data);
	void RequestCorrelation(SmCorrelationRequest& req);
public:
	void TestLib();
	void GetSymbolList();
private:
	void ProcessCorrelation(SmCorrelationRequest& req);
	void SendResult(double result, SmCorrelationRequest& req_cor);
	std::vector<std::vector<double>> _RowValues;
	std::vector<std::vector<double>> _ColValues;
	std::map<std::pair<int, int>, double> _CorMap;
	ThreadPool* _Pool = nullptr;
};

