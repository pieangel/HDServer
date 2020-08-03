#pragma once

#include "../Global/TemplateSingleton.h"
#include "../Global/EbGlobal.h"
#include <map>
// �������� �����͸� �������� �۾��� �׷캰�� ����ȴ�.
// �ϳ��� �۾� �׷��� ť�� �ְ� �� �۾� �׷��� �ϼ��� �Ǹ� ���� �۾� �׷��� �����Ѵ�.
// �۾� �׷��� ����Ǵ� ������ ���α׷����ٸ� ���ؼ� ǥ�õȴ�.
class VtProgressDlg;

class ViDataCollectorDlg;

class ViServerDataReceiver : public TemplateSingleton<ViServerDataReceiver>
{
private:
	// ���� ���¸� ��Ÿ����.
	SmAppState _AppState = SmAppState::None;
	// Key : id, value : argument
	std::map<int, SmTaskArg> 	_RequestMap;
	void ExecRequest(SmTaskArg& arg);
	// ���� ��Ȳ�� ���³��� ��ȭ����
	//VtProgressDlg* _ProgressDlg;
	std::shared_ptr<VtProgressDlg> _ProgressDlg;
	// ���� �۾� ���� �۾��� ������ ����Ѵ�.
	SmTaskInfo _TaskInfo;
	// ���� �۾� ���¸� ǥ���Ѵ�.
	void SetTaskState();
	// ���� �׷� �۾��� �����Ѵ�.
	void DoNextGroupTask();
public:
	SmAppState AppState() const { return _AppState; }
	void AppState(SmAppState val) { _AppState = val; }
	void InitRequestQ();
	// ť�� ���� �ִ� �۾��� �� ������ �۾��� ���� �մϴ�.
	// ť�� ��� ������ �ƹ��ϵ� ���� �ʽ��ϴ�.
	void ExecNext();
	SmTaskArg* FindTask(int reqId);
	void OnTaskComplete(int reqId);
	void OnTaskError(int reqId);
	int GetTaskCount() {
		return _RequestMap.size();
	}
	// �������� �ɺ� ������ ��û ����� �����.
	void MakeDmSymbolMasterRequests();
	// �������� �ɺ� �ڵ� ��û ����� �����.
	void MakeDmSymbolCodeRequests();
	// �ɺ������� ��û ����� �����.
	void MakeSymbolMasterRequests();
	// �ü� ��û ����� �����.
	void MakeSymbolSiseRequests();
	// ȣ�� ��û ����� �����.
	void MakeSymbolHogaRequests();
	// �ɺ� ��û ����� �����.
	void MakeSymbolRequests();
	// ��Ź �ܰ� ��û ����� �����.
	void MakeAccountAssetRequests();
	// �Ϻ� ������ �����ɴϴ�.
	void MakeAccoutProfitLoss();
	// �ܰ� �� �򰡼��� ��û ����� �����.
	void MakeRemainRequests();
	// ���º� ��ü�� �ֹ� ��û ����� �����.
	void MakeAccountOrders();
	// �������� ���� �������� ������ �����Ų��.
	void EndAllTask();
	// ��Ʈ������ ��û ����� �����.
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
