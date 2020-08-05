#pragma once
#include "../Global/TemplateSingleton.h"
#include "../Global/EbGlobal.h"
#include "../Timer/cpptime.h"
#include <map>
class SmServerRequestManager : public TemplateSingleton<SmServerRequestManager>
{
public:
	SmServerRequestManager();
	~SmServerRequestManager();
	// �۾� �Ϸ�Ǿ��� �� ȣ���
	void OnTaskComplete(int reqId);
	// ���� ������ �� ȣ���
	void OnTaskError(int reqId);
	// ���� ���¸� �����´�.
	void GetAccountAsset(std::string account_no);
	// �̴��ܰ� �����´�. - ���� ���¿� ���� ���� ��Ȳ�� �����´�.
	void GetMiniJango(std::string account_no);
	// �ɺ� �����͸� �����´�.
	void GetSymbolMaster(std::string symbol_code);
	// ��� �ɺ��� �ֱٿ� ��Ʈ�����͸� �����´�.
	void GetRecentChartData();
private:
	// ť�� ���� �ִ� �۾��� �� ������ �۾��� ���� �մϴ�.
	// ť�� ��� ������ �ƹ��ϵ� ���� �ʽ��ϴ�.
	void ExecNext();

	// ���� �۾� ���� �۾��� ������ ����Ѵ�.
	SmTaskInfo _TaskInfo;
	// �۾� ��û ��
	std::map<int, SmTaskArg> 	_RequestMap;
	// ��û �۾��� �����Ѵ�.
	void ExecRequest(SmTaskArg& arg);
	// �۾� ���̵�
	static int _TaskId;
	static int GetId() {
		return _TaskId++;
	}

	void StartTimer(int interval);
	void StopTimer(size_t id);
	void OnTimer();
	CppTime::Timer _Timer;
	std::map<size_t, CppTime::timer_id> _CycleDataReqTimerMap;
};

