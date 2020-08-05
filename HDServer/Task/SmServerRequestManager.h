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
	// 작업 완료되었을 때 호출됨
	void OnTaskComplete(int reqId);
	// 오류 생겼을 때 호출됨
	void OnTaskError(int reqId);
	// 계좌 상태를 가져온다.
	void GetAccountAsset(std::string account_no);
	// 미니잔고를 가져온다. - 계좌 상태와 종목별 손익 현황을 가져온다.
	void GetMiniJango(std::string account_no);
	// 심볼 마스터를 가져온다.
	void GetSymbolMaster(std::string symbol_code);
	// 모든 심볼의 최근월 차트데이터를 가져온다.
	void GetRecentChartData();
private:
	// 큐에 남아 있는 작업중 젤 상위의 작업을 실행 합니다.
	// 큐가 비어 있으면 아무일도 하지 않습니다.
	void ExecNext();

	// 현재 작업 중인 작업의 정보를 기억한다.
	SmTaskInfo _TaskInfo;
	// 작업 요청 맵
	std::map<int, SmTaskArg> 	_RequestMap;
	// 요청 작업을 수행한다.
	void ExecRequest(SmTaskArg& arg);
	// 작업 아이디
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

