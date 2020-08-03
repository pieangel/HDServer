#pragma once
#include "../Global/TemplateSingleton.h"
#include <thread>
#include <array>
#include "BlockingCollection.h"
#include "../Symbol/SmQuoteDefine.h"
using namespace code_machina;
const int QuoteArraySize = 100;
class SmRealtimeQuoteManager : public TemplateSingleton<SmRealtimeQuoteManager>
{
public:
	SmRealtimeQuoteManager();
	~SmRealtimeQuoteManager();
	void StartTaskThread();
	void ClearTasks();
	void AddTask(SmQuoteData&& item);
	void StopThread();
	bool Enable() const { return _Enable; }
	void Enable(bool val) { _Enable = val; }

	void AddFutTask(SmQuoteData&& item);
	void AddOptTask(SmQuoteData&& item);
	void AddProTask(SmQuoteData&& item);
private:
	bool _Enable = true;
	bool _BatchProcess = true;
	void ProcessTask();
	std::thread _TaskThread;
	BlockingCollection<SmQuoteData> _TaskQueue;
	bool _Available = true;
	void ExecuteTask(SmQuoteData&& item);
	// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
	bool ExecuteTask(std::array<SmQuoteData, QuoteArraySize>& arr, int taken);

	// 국내 선물 시세 처리 쓰레드
	std::thread _FutTaskThread;
	// 국내 옵션 시세 처리 쓰레드
	std::thread _OptTaskThread;
	// 상품선물 시세 처리 쓰레드
	std::thread _ProTaskThread;


	// 국내 선물 시세 큐
	BlockingCollection<SmQuoteData> _FutTaskQueue;
	// 국내 옵션 시세 큐
	BlockingCollection<SmQuoteData> _OptTaskQueue;
	// 국내 상품선물 시세 큐
	BlockingCollection<SmQuoteData> _ProTaskQueue;

	bool _FutAvailable = true;
	bool _OptAvailable = true;
	bool _ProAvailable = true;
	// 국내선물 시세 처리 쓰레드 함수
	void ProcessFutTask();
	// 국내 옵션 시세 처리 쓰레드 함수
	void ProcessOptTask();
	// 국내 상품 선물 시세 처리 쓰레드 함수
	void ProcessProTask();
};

