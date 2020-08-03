#pragma once
#include "../Global/TemplateSingleton.h"
#include <thread>
#include "BlockingCollection.h"
#include "../Symbol/SmHogaDefine.h"
#include <array>
using namespace code_machina;
const int HogaArraySize = 100;
class SmRealtimeHogaManager : public TemplateSingleton<SmRealtimeHogaManager>
{
public:
	SmRealtimeHogaManager();
	~SmRealtimeHogaManager();
	void StartTaskThread();
	void ClearTasks();
	void AddTask(SmHogaItem&& item);
	void AddFutTask(SmHogaItem&& item);
	void AddOptTask(SmHogaItem&& item);
	void AddProTask(SmHogaItem&& item);
	void StopThread();
	bool Enable() const { return _Enable; }
	void Enable(bool val) { _Enable = val; }
private:
	bool _Enable = true;
	bool _BatchProcess = true;
	void ProcessTask();
	std::thread _TaskThread;

	// 국내 선물 호가 처리 쓰레드
	std::thread _FutTaskThread;
	// 국내 옵션 호가 처리 쓰레드
	std::thread _OptTaskThread;
	// 상품선물 호가 처리 쓰레드
	std::thread _ProTaskThread;

	BlockingCollection<SmHogaItem> _TaskQueue;

	// 국내 선물 호가 큐
	BlockingCollection<SmHogaItem> _FutTaskQueue;
	// 국내 옵션 호가 큐
	BlockingCollection<SmHogaItem> _OptTaskQueue;
	// 국내 상품선물 호가 큐
	BlockingCollection<SmHogaItem> _ProTaskQueue;

	bool _Available = true;
	bool _FutAvailable = true;
	bool _OptAvailable = true;
	bool _ProAvailable = true;
	// 국내선물 호가 처리 쓰레드 함수
	void ProcessFutTask();
	// 국내 옵션 호가 처리 쓰레드 함수
	void ProcessOptTask();
	// 국내 상품 선물 호가 처리 쓰레드 함수
	void ProcessProTask();
	void ExecuteTask(SmHogaItem&& item);
	// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
	bool ExecuteTask(std::array<SmHogaItem, HogaArraySize>& arr, int taken);
};


