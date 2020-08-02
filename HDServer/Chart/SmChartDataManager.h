#pragma once
#include <string>
#include <map>
#include <memory>
#include "../Global/TemplateSingleton.h"
#include "../Chart/SmChartDefine.h"
#include "../Task/BlockingCollection.h"
#include <thread>
#include <array>
#include "ThreadPool.h"
using namespace code_machina;
const int ChartArraySize = 100;

class SmChartData;
class SmChartDataManager : public TemplateSingleton<SmChartDataManager>
{
public:
	SmChartDataManager();
	~SmChartDataManager();
	std::shared_ptr<SmChartData> AddChartData(SmChartDataRequest data_req);
	std::shared_ptr<SmChartData> AddChartData(std::string symbol_code, int chart_type, int cycle);
	void AddChartData(std::shared_ptr<SmChartData> chart_data);
	std::shared_ptr<SmChartData> FindChartData(std::string data_key);
	std::shared_ptr<SmChartData> AddChartData(SmChartDataItem data_item);
	std::map<std::string, std::shared_ptr<SmChartData>> GetChartDataMap() {
		return _ChartDataMap;
	}

	void StartTaskThread();
	void ClearTasks();
	void AddTask(SmChartDataRequest&& item);
	void StopThread();
	bool Enable() const { return _Enable; }
	void Enable(bool val) { _Enable = val; }
	void RequestChartData(SmChartDataRequest&& req);
private:
	std::map<std::string, std::shared_ptr<SmChartData>> _ChartDataMap;

	bool _Enable = true;
	bool _BatchProcess = true;
	void ProcessTask();
	std::thread _TaskThread;
	BlockingCollection<SmChartDataRequest> _TaskQueue;
	bool _Available = true;
	void ExecuteTask(SmChartDataRequest&& item);
	// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
	bool ExecuteTask(std::array<SmChartDataRequest, ChartArraySize>& arr, int taken);

	ThreadPool* _Pool = nullptr;
	void ProcessChartData(SmChartDataRequest& req);
};

