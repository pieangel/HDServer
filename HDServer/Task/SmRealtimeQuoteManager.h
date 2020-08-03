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
	// arr : �����Ͱ� ��� �ִ� �迭, taken : ���� �����Ͱ� ��� �ִ� ����
	bool ExecuteTask(std::array<SmQuoteData, QuoteArraySize>& arr, int taken);

	// ���� ���� �ü� ó�� ������
	std::thread _FutTaskThread;
	// ���� �ɼ� �ü� ó�� ������
	std::thread _OptTaskThread;
	// ��ǰ���� �ü� ó�� ������
	std::thread _ProTaskThread;


	// ���� ���� �ü� ť
	BlockingCollection<SmQuoteData> _FutTaskQueue;
	// ���� �ɼ� �ü� ť
	BlockingCollection<SmQuoteData> _OptTaskQueue;
	// ���� ��ǰ���� �ü� ť
	BlockingCollection<SmQuoteData> _ProTaskQueue;

	bool _FutAvailable = true;
	bool _OptAvailable = true;
	bool _ProAvailable = true;
	// �������� �ü� ó�� ������ �Լ�
	void ProcessFutTask();
	// ���� �ɼ� �ü� ó�� ������ �Լ�
	void ProcessOptTask();
	// ���� ��ǰ ���� �ü� ó�� ������ �Լ�
	void ProcessProTask();
};

