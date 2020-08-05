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
// 차트데이터는 기본적으로 서버에서 가져온다.
// 각 차트데이터는 타이머가 등록되어 있어 주기가 되면 서버에 데이터를 요청한다.
// 서버에서 수신된 데이터는 차트데이터를 갱신하고 각 클라이언트에 보낸다.
// 단, 틱데이터는 틱 갯수가 차면 바로 요청을 한다. 
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
	std::shared_ptr<SmChartData> AddChartData(SmChartDataItem&& data_item);
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

