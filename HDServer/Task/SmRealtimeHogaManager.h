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

	// ���� ���� ȣ�� ó�� ������
	std::thread _FutTaskThread;
	// ���� �ɼ� ȣ�� ó�� ������
	std::thread _OptTaskThread;
	// ��ǰ���� ȣ�� ó�� ������
	std::thread _ProTaskThread;

	BlockingCollection<SmHogaItem> _TaskQueue;

	// ���� ���� ȣ�� ť
	BlockingCollection<SmHogaItem> _FutTaskQueue;
	// ���� �ɼ� ȣ�� ť
	BlockingCollection<SmHogaItem> _OptTaskQueue;
	// ���� ��ǰ���� ȣ�� ť
	BlockingCollection<SmHogaItem> _ProTaskQueue;

	bool _Available = true;
	bool _FutAvailable = true;
	bool _OptAvailable = true;
	bool _ProAvailable = true;
	// �������� ȣ�� ó�� ������ �Լ�
	void ProcessFutTask();
	// ���� �ɼ� ȣ�� ó�� ������ �Լ�
	void ProcessOptTask();
	// ���� ��ǰ ���� ȣ�� ó�� ������ �Լ�
	void ProcessProTask();
	void ExecuteTask(SmHogaItem&& item);
	// arr : �����Ͱ� ��� �ִ� �迭, taken : ���� �����Ͱ� ��� �ִ� ����
	bool ExecuteTask(std::array<SmHogaItem, HogaArraySize>& arr, int taken);
};


