#pragma once
#include "../Global/TemplateSingleton.h"
#include "../Timer/cpptime.h"
#include "../Chart/SmChartDefine.h"
class SmSymbol;
class SmTimeSeriesCollector : public TemplateSingleton<SmTimeSeriesCollector>
{
public:
	SmTimeSeriesCollector();
	~SmTimeSeriesCollector();
	void CollectRecentMonthSymbolChartData();
	void OnChartDataItem(SmChartDataItem data_item);
	void StartCollectChartData();
	void StartCollectSiseData();
	void StartCollectHogaData();
	void GetChartData(SmChartDataRequest&& data_req);
	void GetRecentMonthSymbolList();
private:
	CppTime::Timer _Timer;
	CppTime::timer_id _ChartDataTimerId;
	CppTime::timer_id _SiseTimerId;
	CppTime::timer_id _MinTimerId;
	CppTime::timer_id _HogaTimerId;
	void OnTimer();
	void OnEveryMinute();
	void OnSiseTimer();
	void OnHogaTimer();
	size_t _Index = 0;
	size_t _SiseIndex = 0;
	size_t _HogaIndex = 0;
	std::vector<std::shared_ptr<SmSymbol>> _RecentMonthSymbolVec;
};

