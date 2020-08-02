#pragma once
#include "hdfcommagent.h"
#include <string>
#include <map>
#include <queue>
#include <mutex>
#include "../Chart/SmChartDefine.h"

#pragma once
// 계좌
const CString DEF_ACCT_INFO = "g11004.AQ0101%";
const CString DEF_AVAILABLE_CODE_LIST = _T("g11002.DQ1211&");
// 국내
const CString DEF_TR_CODE = "g11002.DQ0622&";
const CString DEF_FID_CODE = "s20001";
const CString DEF_ORD_CODE_NEW = "g12001.DO1601&";
const CString DEF_ORD_CODE_MOD = "g12001.DO1901&";
const CString DEF_ORD_CODE_CNL = "g12001.DO1701&";

const CString DEF_ORD_CODE_NEW_CME = "g12001.DO2201&";	//CME 신규주문
const CString DEF_ORD_CODE_CNL_CME = "g12001.DO2001&";	//CME 취소
const CString DEF_ORD_CODE_MOD_CME = "g12001.DO2101&";	//CME 정정(가격,조건)주문

const CString DEF_CHE_LIST = "g11002.DQ0107&";
const CString DEF_MICHE_LIST = "g11002.DQ0104&";

const CString DefPutOrder = "g12001.DO1601&";
const CString DefModifyOrder = "g12001.DO1901&";
const CString DefCancelOrder = "g12001.DO1701&";
const CString DefPutCmeOrder = "g12001.DO2201&";	//CME 신규주문
const CString DefModifyCmeOrder = "g12001.DO2001&";	//CME 취소
const CString DefCancelCmeOrder = "g12001.DO2101&";	//CME 정정(가격,조건)주문


const CString DefAcceptedHistory = "g11002.DQ0104&";
const CString DefFilledHistory = "g11002.DQ0107&";
const CString DefOutstandingHistory = "g11002.DQ0110&";
const CString DefOutstanding = "g11002.DQ1305&";
const CString DefCmeAcceptedHistory = "g11002.DQ0116&";
const CString DefCmeFilledHistory = "g11002.DQ0119&";
const CString DefCmeOutstandingHistory = "g11002.DQ0122&";
const CString DefCmeOutstanding = "g11002.DQ1306&";
const CString DefCmeAsset = "g11002.DQ0125&";
const CString DefCmePureAsset = "g11002.DQ1303&";
const CString DefAsset = "g11002.DQ0217&";
const CString DefDeposit = "g11002.DQ0242&";
const CString DefDailyProfitLoss = "g11002.DQ0502&";
const CString DefFilledHistoryTable = "g11002.DQ0509&";
const CString DefAccountProfitLoss = "g11002.DQ0521&";
const CString DefSymbolCode = "g11002.DQ0622&";
const CString DefTradableCodeTable = "g11002.DQ1211&";
const CString DefApiCustomerProfitLoss = "g11002.DQ1302&";
const CString DefChartData = "v90003";
const CString DefCurrentQuote = "l41600";
const CString DefDailyQuote = "l41601";
const CString DefTickQuote = "l41602";
const CString DefSecondQutoe = "l41603";
const CString DefSymbolMaster = "s20001";
const CString DefStockFutureSymbolMaster = "s31001";
const CString DefIndustryMaster = "s10001";
const CString DefServerTime = "o44011";

// 해외
const CString DEF_HW_JANGO = "g11004.AQ0605%";
const CString DEF_HW_FID_CODE = "o51000";
const CString DEF_HW_ORD_CODE_NEW = "g12003.AO0401%";
const CString DEF_HW_ORD_CODE_MOD = "g12003.AO0402%";
const CString DEF_HW_ORD_CODE_CNL = "g12003.AO0403%";
const CString DEF_HW_MSTINFO = "o51211";
const CString DefAbChartData = "o51200";
const CString DefAbSiseData = "o51000";
const CString DefAbHogaData = "o51010";
const CString DefAbsChartData2 = "o44005";

// FX마진	//@lhe 2012.06.22
const CString DEF_FX_JANGO = "g11004.AQ0901%";		// 자산내역
const CString DEF_FX_FID_CODE = "x00001";				// FX마스터
const CString DEF_FX_ORD_CODE_NEW = "g12003.AO0501%";	// 예약,시장가,청산주문
const CString DEF_FX_ORD_CODE_MOD = "g12003.AO0502%";	// 정정
const CString DEF_FX_ORD_CODE_CNL = "g12003.AO0503%";	// 취소주문

// VtHdCtrl dialog
class SmHdClient;
struct HdHogaItem {
	CString strBuyHoga;
	CString strSellHoga;
	CString strBuyHogaQty;
	CString strSellHogaQty;
	CString strBuyHogaCnt;
	CString strSellHogaCnt;
};

struct HdHoga {
	CString strSymbolCode;
	HdHogaItem Items[5];
};
class SmHdCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(SmHdCtrl)

public:
	SmHdCtrl(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmHdCtrl();
	CHDFCommAgent m_CommAgent;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HD_CTRL };
#endif

	SmHdClient* Client() const { return _Client; }
	void Client(SmHdClient* val) { _Client = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnDataRecv(CString sTrCode, LONG nRqID);
	afx_msg void OnGetBroadData(CString strKey, LONG nRealType);
	afx_msg void OnGetMsg(CString strCode, CString strMsg);
	afx_msg void OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg);
private:
	CString m_sUserId;
	CString m_sUserPw;
	CString m_sAuthPw;
	CString m_sAcntPw;
	SmHdClient* _Client = nullptr;
public:
	int Init();
	int LogIn(CString id, CString pwd, CString cert);
	int LogOut(CString id);
	void LogIn();
	void LogOut();
	void RegisterProduct(std::string symCode);
	void UnregisterProduct(std::string symCode);
	void DownloadMasterFiles(std::string param);
	void GetSiseData(std::string symCode);
	void GetHogaData(std::string symCode);
	void RequestChartData(SmChartDataRequest req);
private:
	void GetChartData(SmChartDataRequest req);
	void OnRcvdAbroadHoga(CString& strKey, LONG& nRealType);
	void OnRcvdAbroadSise(CString& strKey, LONG& nRealType);
	void OnRcvdAbroadSiseByReq(CString& sTrCode, LONG& nRqID);
	void OnRcvdAbroadHogaByReq(CString& sTrCode, LONG& nRqID);
	// 해외 차트 데이터 수신 
	void OnRcvdAbroadChartData(CString& sTrCode, LONG& nRqID);
	// 해외 차트 데이터 수신
	void OnRcvdAbroadChartData2(CString& sTrCode, LONG& nRqID);
	void GetChartDataShortCycle(SmChartDataRequest req);
	void GetChartDataLongCycle(SmChartDataRequest req);
	// 국내 차트 데이터
	void GetChartDataForDomestic(SmChartDataRequest req);
	// 국내 차트 데이터 수신
	void OnRcvdDomesticChartData(CString& sTrCode, LONG& nRqID);
	// 큐에서 차트 데이터 요청
	void RequestChartDataFromQ();
	/// <summary>
	/// 차트 데이터 요청 맵 
	/// 키 : 요청 번호
	/// 값 : 요청 데이터 고유 키 값
	/// </summary>
	std::map<int, SmChartDataRequest> _ChartDataReqMap;
	std::map<int, std::string> _SiseDataReqMap;
	std::queue< SmChartDataRequest> _ChartDataReqQueue;
	std::mutex _mutex;
};
