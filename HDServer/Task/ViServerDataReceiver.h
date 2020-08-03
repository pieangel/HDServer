#pragma once

#include "../Global/TemplateSingleton.h"
#include "../Global/EbGlobal.h"
#include <map>
// 서버에서 데이터를 가져오는 작업은 그룹별로 진행된다.
// 하나의 작업 그룹을 큐에 넣고 그 작업 그룹이 완성이 되면 다음 작업 그룹을 진행한다.
// 작업 그룹이 진행되는 과정은 프로그레스바를 통해서 표시된다.
class VtProgressDlg;

class ViDataCollectorDlg;

class ViServerDataReceiver : public TemplateSingleton<ViServerDataReceiver>
{
private:
	// 앱의 상태를 나타낸다.
	SmAppState _AppState = SmAppState::None;
	// Key : id, value : argument
	std::map<int, SmTaskArg> 	_RequestMap;
	void ExecRequest(SmTaskArg& arg);
	// 진행 상황을 나태내는 대화상자
	//VtProgressDlg* _ProgressDlg;
	std::shared_ptr<VtProgressDlg> _ProgressDlg;
	// 현재 작업 중인 작업의 정보를 기억한다.
	SmTaskInfo _TaskInfo;
	// 현재 작업 상태를 표시한다.
	void SetTaskState();
	// 다음 그룹 작업을 시작한다.
	void DoNextGroupTask();
public:
	SmAppState AppState() const { return _AppState; }
	void AppState(SmAppState val) { _AppState = val; }
	void InitRequestQ();
	// 큐에 남아 있는 작업중 젤 상위의 작업을 실행 합니다.
	// 큐가 비어 있으면 아무일도 하지 않습니다.
	void ExecNext();
	SmTaskArg* FindTask(int reqId);
	void OnTaskComplete(int reqId);
	void OnTaskError(int reqId);
	int GetTaskCount() {
		return _RequestMap.size();
	}
	// 국내선물 심볼 마스터 요청 목록을 만든다.
	void MakeDmSymbolMasterRequests();
	// 국내선물 심볼 코드 요청 목록을 만든다.
	void MakeDmSymbolCodeRequests();
	// 심볼마스터 요청 목록을 만든다.
	void MakeSymbolMasterRequests();
	// 시세 요청 목록을 만든다.
	void MakeSymbolSiseRequests();
	// 호가 요청 목록을 만든다.
	void MakeSymbolHogaRequests();
	// 심볼 요청 목록을 만든다.
	void MakeSymbolRequests();
	// 예탁 잔고 신청 목록을 만든다.
	void MakeAccountAssetRequests();
	// 일별 손익을 가져옵니다.
	void MakeAccoutProfitLoss();
	// 잔고 및 평가손익 신청 목록을 만든다.
	void MakeRemainRequests();
	// 계좌별 미체결 주문 요청 목록을 만든다.
	void MakeAccountOrders();
	// 서버에서 정보 가져오는 과정을 종료시킨다.
	void EndAllTask();
	// 차트데이터 요청 목록을 만든다.
	void MakeChartDataRequests();

	void StartGetSymbolMaster();
	void StartGetSymbol();
	void StartGetCSise();
	void StartGetHoga();
	void StartGetAccountAsset();
	void StartGetProductRemain();
	void StartGetAccountOrder();
	void StartGetAccountProfitLoss();
	void StartGetChartData();

	void StartGetDmSymbolCode();
	void StartGetDmSymbolMaster();
	static int _TaskId;
	static int GetId() {
		return _TaskId++;
	}
	std::shared_ptr<VtProgressDlg> ProgressDlg() const { return _ProgressDlg; }
	void ProgressDlg(std::shared_ptr<VtProgressDlg> val) { _ProgressDlg = val; }
};
