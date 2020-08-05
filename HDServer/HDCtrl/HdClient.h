#pragma once
#include "../Global/TemplateSingleton.h"
#include "../Global/EbGlobal.h"
#include "../Chart/SmChartDefine.h"
//#include "../Order/VtOrder.h"
#include <string>
#include <map>
class HdCtrl;
class HdClient : public TemplateSingleton<HdClient>
{
public:
	HdClient();
	~HdClient();
	bool Enable() const { return _Enable; }
	void Enable(bool val) { _Enable = val; }
private:
	HdCtrl* _HdCtrl = nullptr;
	// �ý��� ��û ��
	std::map<int, SmTaskArg> _RequestMap;
	// ��Ʈ������ ��û��
	std::map<int, SmChartDataRequest> _ChartDataReqMap;


	int GetChartDataShortCycle(SmChartDataRequest req);
	int GetChartDataLongCycle(SmChartDataRequest req);
	int GetChartDataForDomestic(SmChartDataRequest req);
	int GetChartData(SmChartDataRequest req);

	bool _Enable = true;
public:
	int Init();
	int Login(std::string id, std::string pwd, std::string cert);
	void OnAccountListReceived(CString& sTrCode, LONG& nRqID);
	void DownloadAbroadSymbolFiles();

	void OnDataRecv(CString& sTrCode, LONG& nRqID);
	void OnGetBroadData(CString& strKey, LONG& nRealType);
	void OnGetMsg(CString& strCode, CString& strMsg);
	void OnGetMsgWithRqId(int& nRqId, CString& strCode, CString& strMsg);
	void ReadSymbolInfo(std::string symbol_code);

	// ���� �������� �������� ó�� �Ѵ�.
	void OnTaskComplete(int nRqId);

	// ���� ���� ���� ����Ʈ�� �������� �Լ�
	int GetDmSymbolCodeList(SmTaskArg& arg);
	void OnDmSymbolCode(CString& sTrCode, LONG& nRqID);

	// ���� ���� ���� �����͸� �������� �Լ�
	int GetDmSymbolMaster(SmTaskArg& arg);
	void OnDmSymbolMaster(CString& sTrCode, LONG& nRqID);

	// �ؿ� ���� �ɺ� ������ �������� �Լ�
	int GetAbSymbolMaster(SmTaskArg& arg);
	void OnAbroadSymbolMaster(CString& sTrCode, LONG& nRqID);
	// �ؿ� ���� ���簡�� �������� �Լ�
	int GetAbSymbolSise(SmTaskArg& arg);
	void OnAbroadSymbolSise(CString& sTrCode, LONG& nRqID);
	// �ؿ� ���� ȣ���� �������� �Լ�
	int GetAbSymbolHoga(SmTaskArg& arg);
	void OnAbroadSymbolHoga(CString& sTrCode, LONG& nRqID);

	// �ؿ� �ڻ�
	int GetAbAsset(SmTaskArg& arg);
	void OnGetAbAsset(CString& sTrCode, LONG& nRqID);

	// �ؿ� ���º� ����
	int GetAbDeposit(SmTaskArg& arg);
	void OnGetAbDeposit(CString& sTrCode, LONG& nRqID);

	// �ؿ� ���� ����
	int GetAbAccountProfitLoss(SmTaskArg& arg);
	void OnGetAbAccountProfitLoss(CString& sTrCode, LONG& nRqID);

	// �ؿ� �̰���
	int GetAbOutStanding(SmTaskArg& arg);
	void OnGetAbOutStanding(CString& sTrCode, LONG& nRqID);

	// �ؿ� ��ü��
	int GetAbAccepted(SmTaskArg& arg);
	void OnGetAbAccepted(CString& sTrCode, LONG& nRqID);

	// �ؿ� ��Ʈ������
	int GetAbChartData(SmTaskArg& arg);
	void OnRcvdAbroadChartData(CString& sTrCode, LONG& nRqID);
	void OnRcvdAbroadChartData2(CString& sTrCode, LONG& nRqID);
	void OnRcvdDomesticChartData(CString& sTrCode, LONG& nRqID);

	// �ؿ� �ű� �ֹ� ���� ����
	void OnNewAbOrderHd(CString& sTrCode, LONG& nRqID);
	// �ؿ� ���� �ֹ� ���� ����
	void OnModifyAbOrderHd(CString& sTrCode, LONG& nRqID);
	// �ؿ� ��� �ֹ� ���� ����
	void OnCancelAbOrderHd(CString& sTrCode, LONG& nRqID);
	// �Ʒ��� �ǽð� �޽��� 
	// �ؿ� �ֹ� ����Ȯ��
	void OnAbOrderAcceptedHd(CString& strKey, LONG& nRealType);
	// �ؿ� �ֹ� ��ü�� 
	void OnAbOrderUnfilledHd(CString& strKey, LONG& nRealType);
	// �ؿ� �ֹ� ü��
	void OnAbOrderFilledHd(CString& strKey, LONG& nRealType);

	// ��ǰ ���
	void RegisterProduct(CString symCode);
	// ��ǰ ��� ����
	void UnregisterProduct(CString symCode);
	// ���� ���
	void RegisterAccount(CString accountNo);
	// ���� ��� ����
	void UnregisterAccount(CString accountNo);

	// �ؿ� �ǽð� �ü�
	void OnAbRealtimeSise(CString& strKey, LONG& nRealType);
	// �ؿ� �ǽð� ȣ��
	void OnAbRealtimeHoga(CString& strKey, LONG& nRealType);

	void OnFutureHoga(CString& strKey, LONG& nRealType);
	void OnOptionHoga(CString& strKey, LONG& nRealType);
	void OnProductHoga(CString& strKey, LONG& nRealType);

	void OnRealFutureQuote(CString& strKey, LONG& nRealType);
	void OnRealOptionQuote(CString& strKey, LONG& nRealType);
	void OnRealProductQuote(CString& strKey, LONG& nRealType);
};

