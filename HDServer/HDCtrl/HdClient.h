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
	// 시스템 요청 맵
	std::map<int, SmTaskArg> _RequestMap;
	// 차트데이터 요청맵
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

	// 일이 끝났을때 나머지를 처리 한다.
	void OnTaskComplete(int nRqId);

	// 국내 선물 종목 리스트를 가져오는 함수
	int GetDmSymbolCodeList(SmTaskArg& arg);
	void OnDmSymbolCode(CString& sTrCode, LONG& nRqID);

	// 국내 선물 종목 마스터를 가져오는 함수
	int GetDmSymbolMaster(SmTaskArg& arg);
	void OnDmSymbolMaster(CString& sTrCode, LONG& nRqID);

	// 해외 선물 심볼 정보를 가져오는 함수
	int GetAbSymbolMaster(SmTaskArg& arg);
	void OnAbroadSymbolMaster(CString& sTrCode, LONG& nRqID);
	// 해외 종목 현재가를 가져오는 함수
	int GetAbSymbolSise(SmTaskArg& arg);
	void OnAbroadSymbolSise(CString& sTrCode, LONG& nRqID);
	// 해외 종목 호가를 가져오는 함수
	int GetAbSymbolHoga(SmTaskArg& arg);
	void OnAbroadSymbolHoga(CString& sTrCode, LONG& nRqID);

	// 해외 자산
	int GetAbAsset(SmTaskArg& arg);
	void OnGetAbAsset(CString& sTrCode, LONG& nRqID);

	// 해외 계좌별 손익
	int GetAbDeposit(SmTaskArg& arg);
	void OnGetAbDeposit(CString& sTrCode, LONG& nRqID);

	// 해외 종목별 손익
	int GetAbAccountProfitLoss(SmTaskArg& arg);
	void OnGetAbAccountProfitLoss(CString& sTrCode, LONG& nRqID);

	// 해외 미결제
	int GetAbOutStanding(SmTaskArg& arg);
	void OnGetAbOutStanding(CString& sTrCode, LONG& nRqID);

	// 해외 미체결
	int GetAbAccepted(SmTaskArg& arg);
	void OnGetAbAccepted(CString& sTrCode, LONG& nRqID);

	// 해외 차트데이터
	int GetAbChartData(SmTaskArg& arg);
	void OnRcvdAbroadChartData(CString& sTrCode, LONG& nRqID);
	void OnRcvdAbroadChartData2(CString& sTrCode, LONG& nRqID);
	void OnRcvdDomesticChartData(CString& sTrCode, LONG& nRqID);

	// 해외 신규 주문 서버 응답
	void OnNewAbOrderHd(CString& sTrCode, LONG& nRqID);
	// 해외 정정 주문 서버 응답
	void OnModifyAbOrderHd(CString& sTrCode, LONG& nRqID);
	// 해외 취소 주문 서버 응답
	void OnCancelAbOrderHd(CString& sTrCode, LONG& nRqID);
	// 아래는 실시간 메시지 
	// 해외 주문 접수확인
	void OnAbOrderAcceptedHd(CString& strKey, LONG& nRealType);
	// 해외 주문 미체결 
	void OnAbOrderUnfilledHd(CString& strKey, LONG& nRealType);
	// 해외 주문 체결
	void OnAbOrderFilledHd(CString& strKey, LONG& nRealType);

	// 상품 등록
	void RegisterProduct(CString symCode);
	// 상품 등록 해제
	void UnregisterProduct(CString symCode);
	// 계좌 등록
	void RegisterAccount(CString accountNo);
	// 계좌 등록 해제
	void UnregisterAccount(CString accountNo);

	// 해외 실시간 시세
	void OnAbRealtimeSise(CString& strKey, LONG& nRealType);
	// 해외 실시간 호가
	void OnAbRealtimeHoga(CString& strKey, LONG& nRealType);

	void OnFutureHoga(CString& strKey, LONG& nRealType);
	void OnOptionHoga(CString& strKey, LONG& nRealType);
	void OnProductHoga(CString& strKey, LONG& nRealType);

	void OnRealFutureQuote(CString& strKey, LONG& nRealType);
	void OnRealOptionQuote(CString& strKey, LONG& nRealType);
	void OnRealProductQuote(CString& strKey, LONG& nRealType);
};

