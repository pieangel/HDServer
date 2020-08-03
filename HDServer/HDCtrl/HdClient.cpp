#include "pch.h"
#include "../resource.h"
#include "HdClient.h"
#include "HdCtrl.h"
//#include "../Account/VtAccount.h"
//#include "../Account/VtAccountManager.h"
#include "hdfcommagent.h"
//#include "../MainFrm.h"
#include "../Global/EbGlobal.h"
#include "../Util/VtStringUtil.h"
#include "HDDefine.h"
#include "../Task/ViServerDataReceiver.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Symbol/SmSymbol.h"
#include "../Global/SmGlobal.h"
#include "../Symbol/SmRealtimeRegisterManager.h"
//#include "../Order/VtTotalOrderManager.h"
//#include "../Order/VtOrderManager.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "../Order/VtProductOrderManager.h"
#include "../Format/format.h"
#include "../Chart/SmChartDataManager.h"
#include "../Chart/SmChartData.h"
#include "../Task/SmRealtimeHogaManager.h"
#include "../Task/SmRealtimeQuoteManager.h"
#include "../MainFrm.h"
#include "../Market/SmMarketManager.h"
#include "../Market/SmProduct.h"
#include "../Market/SmMarket.h"
#include "../Symbol/HdProductInfo.h"

HdClient::HdClient()
{
	_HdCtrl = new HdCtrl();
	_HdCtrl->Create(IDD_HD_CTRL);
	_HdCtrl->ShowWindow(SW_HIDE);
	_HdCtrl->Client(this);
}

HdClient::~HdClient()
{
	if (_HdCtrl) {
		_HdCtrl->DestroyWindow();
		delete _HdCtrl;
		_HdCtrl = nullptr;
	}
}

int HdClient::GetChartDataShortCycle(SmChartDataRequest req)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string temp;
	std::string reqString;
	// 종목 코드 32 자리
	temp = VtStringUtil::PadRight(req.symbolCode, ' ', 32);
	reqString.append(temp);

	std::string str = VtStringUtil::getCurentDate();
	CString msg;
	//msg.Format("%s \n", str.c_str());
	//TRACE(msg);
	reqString.append(str);
	reqString.append(str);
	reqString.append(_T("9999999999"));

	if (req.next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	if (req.chartType == SmChartType::TICK)
		reqString.append("1");
	else if (req.chartType == SmChartType::MIN)
		reqString.append("2");
	else if (req.chartType == SmChartType::DAY)
		reqString.append("3");
	else if (req.chartType == SmChartType::WEEK)
		reqString.append("4");
	else if (req.chartType == SmChartType::MON)
		reqString.append("5");
	else
		reqString.append("2");

	temp = VtStringUtil::PadLeft(req.cycle, '0', 2);
	reqString.append(temp);

	temp = VtStringUtil::PadLeft(req.count, '0', 5);
	reqString.append(temp);

	CString sInput = reqString.c_str();
	CString strNextKey = _T("");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";
	int nRqID = m_CommAgent.CommFIDRqData(DefAbChartData, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

	_ChartDataReqMap[nRqID] = req;
	return nRqID;
}

int HdClient::GetChartDataLongCycle(SmChartDataRequest req)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string temp;
	std::string reqString;
	// 최초 요청시 18자리 공백
	reqString.append("                  ");

	temp = VtStringUtil::PadRight(req.symbolCode, ' ', 32);
	reqString.append(temp);

	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%Y%m%d-%H:%M:%S", timeinfo);
	std::string str(buffer);
	CString msg;
	msg.Format("%s \n", str.c_str());
	//TRACE(msg);
	//reqString.append(curDate);
	//reqString.append(curDate);
	//reqString.append(_T("99999999"));
	reqString.append(_T("99999999"));
	reqString.append(_T("9999999999"));

	if (req.next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	if (req.chartType == SmChartType::TICK)
		reqString.append("6");
	else if (req.chartType == SmChartType::MIN)
		reqString.append("2");
	else if (req.chartType == SmChartType::DAY)
		reqString.append("3");
	else if (req.chartType == SmChartType::WEEK)
		reqString.append("4");
	else if (req.chartType == SmChartType::MON)
		reqString.append("5");
	else
		reqString.append("2");

	temp = VtStringUtil::PadLeft(req.cycle, '0', 3);
	reqString.append(temp);

	temp = VtStringUtil::PadLeft(req.count, '0', 5);
	reqString.append(temp);

	reqString.append(_T("1"));
	reqString.append(_T("1"));


	CString sInput = reqString.c_str();
	CString strNextKey = _T("");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";
	
	int nRqID = m_CommAgent.CommRqData(DefAbsChartData2, sInput, sInput.GetLength(), strNextKey);
	_ChartDataReqMap[nRqID] = req;
	return nRqID;
}

int HdClient::GetChartDataForDomestic(SmChartDataRequest req)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string temp;
	std::string reqString;

	temp = VtStringUtil::PadRight(req.symbolCode, ' ', 15);
	reqString.append(temp);

	std::string str = VtStringUtil::getCurentDate();
	reqString.append(str);

	reqString.append(_T("999999"));

	temp = VtStringUtil::PadLeft(req.count, '0', 4);
	reqString.append(temp);

	temp = VtStringUtil::PadLeft(req.cycle, '0', 3);
	reqString.append(temp);

	if (req.chartType == SmChartType::TICK)
		reqString.append("0");
	else if (req.chartType == SmChartType::MIN)
		reqString.append("1");
	else if (req.chartType == SmChartType::DAY)
		reqString.append("2");
	else if (req.chartType == SmChartType::WEEK)
		reqString.append("3");
	else if (req.chartType == SmChartType::MON)
		reqString.append("4");
	else
		reqString.append("1");

	if (req.next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	temp = VtStringUtil::PadRight(req.reqKey, ' ', 21);
	reqString.append(temp);

	reqString.append(_T("0"));
	reqString.append(_T("0"));
	reqString.append(_T("00"));
	reqString.append(_T("000000"));
	reqString.append(_T(" "));

	if (req.seq == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	CString sTrCode = "v90003";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), "");
	_ChartDataReqMap[nRqID] = req;

	return nRqID;
}

int HdClient::GetChartData(SmChartDataRequest req)
{
	if (req.GetDataKey().length() < 8)
		return -1;
	if (std::isdigit(req.symbolCode.at(2))) {
		if (req.symbolCode.length() < 8)
			return -1;
		return GetChartDataForDomestic(req);
	}
	else {

		if (req.symbolCode.length() < 4)
			return -1;
		if (req.chartType == SmChartType::TICK)
			return GetChartDataLongCycle(req);
		else if (req.chartType == SmChartType::MIN)
			return GetChartDataShortCycle(req);
		else if (req.chartType == SmChartType::DAY)
			return GetChartDataLongCycle(req);
		else if (req.chartType == SmChartType::WEEK)
			return GetChartDataLongCycle(req);
		else if (req.chartType == SmChartType::MON)
			return GetChartDataLongCycle(req);
		else
			return GetChartDataShortCycle(req);
	}

	return -1;
}

// bool HdClient::CheckPassword(HdOrderRequest& request)
// {
// 	if (!request.Password.empty() && request.Password.length() == 4) {
// 		return true;
// 	}
// 	else {
// 		AfxMessageBox(_T("계좌에 비밀번호를 설정하지 않았습니다. 주문할수 없습니다."));
// 		return false;
// 	}
// }

int HdClient::Init()
{
	return _HdCtrl->Init();
}

int HdClient::Login(std::string id, std::string pwd, std::string cert)
{
	return _HdCtrl->Login(id, pwd, cert);
}

void HdClient::OnAccountListReceived(CString& sTrCode, LONG& nRqID)
{
	CString strMsg = "계좌정보 수신완료!!!";
	//WriteLog(strMsg);

	// 계좌 구분 추가. - 20140331 sivas
	typedef	struct
	{
		char 	szAcctNo[11];		// 계좌번호
		char	szAcctNm[30];		// 계좌명
		char	szAcctGb[01];		// 계좌구분  '1': 해외, '2': FX, '9':국내
	}HDF_ACCOUNT_UNIT;

	typedef struct
	{
		char szCount[5];
		HDF_ACCOUNT_UNIT* pHdfAccUnit;
	}HDF_ACCOUNT_INFO;

	HDF_ACCOUNT_INFO* pHdfAccInfo = NULL;
	HDF_ACCOUNT_UNIT* pHdfAccUnit = NULL;
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	CString strRcvBuff = m_CommAgent.CommGetAccInfo();

// 	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
// 	pHdfAccInfo = (HDF_ACCOUNT_INFO*)strRcvBuff.GetBuffer();
// 	CString strCount(pHdfAccInfo->szCount, sizeof(pHdfAccInfo->szCount));
// 	for (int i = 0; i < atoi(strCount); i++) {
// 		pHdfAccUnit = (HDF_ACCOUNT_UNIT*)(pHdfAccInfo->szCount + sizeof(pHdfAccInfo->szCount) + (sizeof(HDF_ACCOUNT_UNIT) * i));
// 		CString strAcctNo(pHdfAccUnit->szAcctNo, sizeof(pHdfAccUnit->szAcctNo));
// 		CString strAcctNm(pHdfAccUnit->szAcctNm, sizeof(pHdfAccUnit->szAcctNm));
// 		CString strAcctGb(pHdfAccUnit->szAcctGb, sizeof(pHdfAccUnit->szAcctGb));// 계좌 구분 추가. - 20140331 sivas
// 
// 		strMsg.Format("[%s][%s][%s]\n", strAcctNo, strAcctNm, strAcctGb);
// 		//WriteLog(strMsg);
// 		//TRACE(strMsg);
// 		if (strAcctGb.Compare(_T("1")) == 0) {
// 			VtAccountInfo acnt_info;
// 			acnt_info.account_no = (LPCTSTR)strAcctNo.TrimRight();
// 			acnt_info.account_name = (LPCTSTR)strAcctNm.TrimRight();
// 			acnt_info.account_type = _ttoi(strAcctGb);
// 			acntMgr->ServerAccountMap[acnt_info.account_no] = acnt_info;
// 			//VtRealtimeRegisterManager* regMgr = VtRealtimeRegisterManager::GetInstance();
// 			//regMgr->RegisterAccount(acnt_info.account_no);
// 		}
// 
// 	}
}

void HdClient::DownloadAbroadSymbolFiles()
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	m_CommAgent.CommReqMakeCod("futures", 0);
}

void HdClient::OnDataRecv(CString& sTrCode, LONG& nRqID)
{
	if (sTrCode == DefAccountList) {
		OnAccountListReceived(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbSymbolMaster) {
		OnAbroadSymbolMaster(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbSymbolSise) {
		OnAbroadSymbolSise(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbSymbolHoga) {
		OnAbroadSymbolHoga(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbAsset) {
		OnGetAbAsset(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbAccountProfitLoss) {
		OnGetAbDeposit(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbSymbolProfitLoss) {
		OnGetAbAccountProfitLoss(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbOutstanding) {
		OnGetAbOutStanding(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbAccepted) {
		OnGetAbAccepted(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbChartData) {
		OnRcvdAbroadChartData(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbsChartData2) {
		OnRcvdAbroadChartData2(sTrCode, nRqID);
	}
	else if (sTrCode == DefSymbolCode) {
		OnDmSymbolCode(sTrCode, nRqID);
	}
	else if (sTrCode == DefSymbolMaster) {
		OnDmSymbolMaster(sTrCode, nRqID);
	}
}

void HdClient::OnGetBroadData(CString& strKey, LONG& nRealType)
{
	if (!_Enable)
		return;

	switch (nRealType)
	{
	case 296: // 해외 주문 접수
		OnAbOrderAcceptedHd(strKey, nRealType);
		break;
	case 286: // 해외 주문 미체결
		OnAbOrderUnfilledHd(strKey, nRealType);
		break;
	case 289: // 해외 주문 체결
		OnAbOrderFilledHd(strKey, nRealType);
		break;
	case 76: // 해외 실시간 호가
		OnAbRealtimeHoga(strKey, nRealType);
		break;
	case 82: // 해외 실시간 체결
		OnAbRealtimeSise(strKey, nRealType);
		break;
	case 51:	// 선물호가
		OnFutureHoga(strKey, nRealType);
	break;
	case 52:	// 옵션호가
		OnOptionHoga(strKey, nRealType);
	break;
	case 58: //상품선물호가실시간
		OnProductHoga(strKey, nRealType);
	break;
	case 65:	// 선물체결
		OnRealFutureQuote(strKey, nRealType);
	break;
	case 71:	// 상품선물체결
		OnRealProductQuote(strKey, nRealType);
	break;
	case 66:	// 옵션체결
		OnRealOptionQuote(strKey, nRealType);
	break;
	default:
		break;
	}
}

void HdClient::OnGetMsg(CString& strCode, CString& strMsg)
{

}

void HdClient::OnGetMsgWithRqId(int& nRqId, CString& strCode, CString& strMsg)
{
	CString msg;
	msg.Format("OnGetMsgWithRqId :: id = %d, Code = %s, Msg= %s\n", nRqId, strCode, strMsg);
	TRACE(msg);
	
	// 해외 종목 메모리 로드가 끝나면 다음 과정을 시작한다.
	if (strCode.Compare("0332") == 0) {
		CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
		mf->StartProcess();
	}
	if (_ttoi(strCode) != 0) {
		OnTaskComplete(nRqId);
	}
}

void HdClient::ReadSymbolInfo(std::string symbol_code)
{

}

void HdClient::OnTaskComplete(int nRqId)
{
	auto it = _RequestMap.find(nRqId);
	if (it != _RequestMap.end()) {
		SmTaskArg& arg = it->second;
		ViServerDataReceiver::GetInstance()->OnTaskComplete(arg.ArgId);
		_RequestMap.erase(it);
	}
}

int HdClient::GetDmSymbolCodeList(SmTaskArg& arg)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string product_code = arg.GetArg("product_code");

	CString sTrCode = DefSymbolCode;
	CString sInput = product_code.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	_RequestMap[nRqID] = arg;
	return nRqID;
}

void HdClient::OnDmSymbolCode(CString& sTrCode, LONG& nRqID)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	if (nRepeatCnt > 0) {
		std::shared_ptr<SmSymbol> sym = nullptr;
		for (int i = 0; i < nRepeatCnt; i++) {
			CString sData = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
			TRACE(sData);
			TRACE(_T("\n"));

			std::string symCode = sData.Trim();
			std::string product_code = symCode.substr(0, 3);
			std::shared_ptr<SmProduct> product = SmMarketManager::GetInstance()->FindProduct(product_code);
			if (product) {
				sym = product->AddSymbol(symCode);
				product->AddToYearMonth(sym->SymbolCode(), sym);
			}

		}
		
	}

	OnTaskComplete(nRqID);
}

int HdClient::GetDmSymbolMaster(SmTaskArg& arg)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string symbol_code = arg.GetArg("symbol_code");

	CString sInput = symbol_code.c_str();
	sInput.Append(_T("40001"));
	CString sReqFidInput = _T("000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232");
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommFIDRqData(DefSymbolMaster, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	_RequestMap[nRqID] = arg;
	return nRqID;
}

void HdClient::OnDmSymbolMaster(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	CString	strData000 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "단축코드");

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol((LPCTSTR)strData000.Trim());
	if (!sym) {
		OnTaskComplete(nRqID);
		return;
	}

	CString	strData001 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString	strData002 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "한글종목명");
	CString	strData003 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목한글약명");
	CString	strData004 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "영문종목명");
	CString	strData005 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "축약종목명");
	CString strCom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비");
	CString strUpRate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "등락률");

	//LOG_F(INFO, _T("종목코드 = %s"), strData001);
	TRACE(strData001);
	TRACE(_T("\n"));
	std::string code = sym->SymbolCode().substr(0, 3);
	strCom.TrimRight();
	strUpRate.TrimRight();


	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name;
	market_name = "국내선물";
	sym->Name((LPCTSTR)strData002);
	sym->NameEn((LPCTSTR)strData004);
	sym->CategoryCode(code);
	sym->MarketName(market_name);

	std::shared_ptr<HdProductInfo> prdtInfo = symMgr->FindProductInfo(code);
	if (prdtInfo) {
		sym->Decimal(prdtInfo->decimal);
		sym->Seungsu(prdtInfo->tradeWin);
		sym->TickValue(prdtInfo->tickValue);
		sym->TickSize(_ttof(prdtInfo->tickSize.c_str()));
	}




	CString	strData049 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "거래단위");

	CString	strData050 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "체결시간");
	CString	strData051 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "시가");
	CString	strData053 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "고가");
	CString	strData054 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "저가");
	CString strPreClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일종가");
	CString strPreHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일고가");
	CString strPreLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일저가");


	sym->Quote.Close = _ttoi(strData051);
	sym->Quote.Open = _ttoi(strData052);
	sym->Quote.High = _ttoi(strData053);
	sym->Quote.Low = _ttoi(strData054);
	sym->Quote.PreClose = _ttoi(strPreClose);

	std::string midCode = sym->SymbolCode().substr(0, 3);
	if (midCode.compare(_T("101")) == 0) {
		symMgr->Kospi200Current = sym->Quote.Close;
	}

	CString	strData075 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "호가수신시간");
	CString	strData076 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량1");
	CString	strData079 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량1");
	CString	strData080 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수1");

	sym->Hoga.Ary[0].SellPrice = _ttoi(strData076);
	sym->Hoga.Ary[0].BuyPrice = _ttoi(strData077);
	sym->Hoga.Ary[0].SellQty = _ttoi(strData078);
	sym->Hoga.Ary[0].BuyQty = _ttoi(strData079);
	sym->Hoga.Ary[0].SellCnt = _ttoi(strData080);
	sym->Hoga.Ary[0].BuyCnt = _ttoi(strData081);


	CString	strData082 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량2");
	CString	strData085 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량2");
	CString	strData086 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수2");

	sym->Hoga.Ary[1].SellPrice = _ttoi(strData082);
	sym->Hoga.Ary[1].BuyPrice = _ttoi(strData083);
	sym->Hoga.Ary[1].SellQty = _ttoi(strData084);
	sym->Hoga.Ary[1].BuyQty = _ttoi(strData085);
	sym->Hoga.Ary[1].SellCnt = _ttoi(strData086);
	sym->Hoga.Ary[1].BuyCnt = _ttoi(strData087);
	CString	strData088 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량3");
	CString	strData091 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량3");
	CString	strData092 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수3");

	sym->Hoga.Ary[2].SellPrice = _ttoi(strData088);
	sym->Hoga.Ary[2].BuyPrice = _ttoi(strData089);
	sym->Hoga.Ary[2].SellQty = _ttoi(strData090);
	sym->Hoga.Ary[2].BuyQty = _ttoi(strData091);
	sym->Hoga.Ary[2].SellCnt = _ttoi(strData092);
	sym->Hoga.Ary[2].BuyCnt = _ttoi(strData093);
	CString	strData094 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량4");
	CString	strData097 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량4");
	CString	strData098 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수4");

	sym->Hoga.Ary[3].SellPrice = _ttoi(strData094);
	sym->Hoga.Ary[3].BuyPrice = _ttoi(strData095);
	sym->Hoga.Ary[3].SellQty = _ttoi(strData096);
	sym->Hoga.Ary[3].BuyQty = _ttoi(strData097);
	sym->Hoga.Ary[3].SellCnt = _ttoi(strData098);
	sym->Hoga.Ary[3].BuyCnt = _ttoi(strData099);
	CString	strData100 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량5");
	CString	strData103 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량5");
	CString	strData104 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수5");
	sym->Hoga.Ary[4].SellPrice = _ttoi(strData100);
	sym->Hoga.Ary[4].BuyPrice = _ttoi(strData101);
	sym->Hoga.Ary[4].SellQty = _ttoi(strData102);
	sym->Hoga.Ary[4].BuyQty = _ttoi(strData103);
	sym->Hoga.Ary[4].SellCnt = _ttoi(strData104);
	sym->Hoga.Ary[4].BuyCnt = _ttoi(strData105);

	CString	strData106 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총수량");
	CString	strData107 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총수량");
	CString	strData108 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총건수");

	sym->Hoga.TotSellQty = _ttoi(strData106);
	sym->Hoga.TotBuyQty = _ttoi(strData107);
	sym->Hoga.TotSellCnt = _ttoi(strData108);
	sym->Hoga.TotBuyCnt = _ttoi(strData109);

	OnTaskComplete(nRqID);
}

int HdClient::GetAbSymbolMaster(SmTaskArg& arg)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string symbol_code = arg.GetArg("symbol_code");
	std::string temp;
	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	CString sInput = temp.c_str();
	CString sReqFidInput = "000001002003004005006007008009010011012013014015016017018019020021022023";
	CString strNextKey = m_CommAgent.CommGetNextKey(0, "");
	int nRqID = m_CommAgent.CommFIDRqData(DefAbSymbolMaster, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

	CString msg;
	msg.Format("nRqID = %d\n", nRqID);
	TRACE(msg);

	_RequestMap[nRqID] = arg;

	return nRqID;
}

void HdClient::OnAbroadSymbolMaster(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();


	CString m_strBusinessDay = m_CommAgent.CommGetBusinessDay(1);

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strSymbolCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		CString strLastTradeDay = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "최종거래일");
		CString strExchange = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래소");
		CString strExpireDay = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "만기일");
		CString strPriceTag = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "가격표시");
		CString strStartTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장시작시간(CME)");
		CString strEndTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장종료시간(CME)");
		CString strLocalStartTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장시작시간(한국)");
		CString strLocalEndTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장종료시간(한국)");
		CString strCurrency = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래통화");
		CString strProduct = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "상품구분");
		CString strTickSize = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "ticksize");
		CString strTickValue = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "tickvalue");
		CString strNeedMoney = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "증거금");
		CString strContractUnit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계약단위");
		CString strRemainMoney = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "유지증거금");

		strSymbolCode = strSymbolCode.Trim();
		CString NearMonth = m_CommAgent.CommGetHWInfo(strSymbolCode, 14);
		CString SeriesNmKor = m_CommAgent.CommGetHWInfo(strSymbolCode, 13);
		CString SeriesNm = m_CommAgent.CommGetHWInfo(strSymbolCode, 12);
		CString Decimal = m_CommAgent.CommGetHWInfo(strSymbolCode, 5);

		CString msg;
		msg.Format("symbol_code = %s, decimal = %s\n", strSymbolCode, strPriceTag);
		TRACE(msg);
		int decimal = 0;
		int found = strPriceTag.Find("/");
		if (found < 0) {
			decimal = 0;
		}
		else {
			CString price;
			price = strPriceTag.Right(strPriceTag.GetLength() - found - 1);
			std::string temp;
			temp = price;
			size_t n = std::count(temp.begin(), temp.end(), '0');
			msg.Format("decimal = %d\n", n);
			TRACE(msg);
			decimal = n;
		}

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> symbol = symMgr->FindSymbol((LPCTSTR)strSymbolCode.Trim());

		if (symbol) {
			symbol->NearMonth( _ttoi(NearMonth));
			symbol->Name( (LPCTSTR)SeriesNmKor);
			symbol->NameEn( (LPCTSTR)SeriesNm);
			// 소수점
			symbol->Decimal( _ttoi(Decimal));
			// 숭수
			symbol->Seungsu( _ttoi(strContractUnit));
			// 계약 크기
			symbol->CtrUnit ( _ttof(strContractUnit));
			// 틱 가치
			symbol->TickValue( _ttof(strTickValue));
			// 틱 크기
			symbol->TickSize ( _ttof(strTickSize));
			// 정수 틱 크기를 계산하여 넣어 준다.
			//symbol->intTickSize = (int)(symbol->TickSize * std::pow(10, symbol->Decimal));
			symbol->LastDate( (LPCTSTR)strExpireDay);


			symbol->StartHour(_ttoi(strLocalStartTime.Left(2)));
			symbol->StartMin(_ttoi(strLocalStartTime.Mid(2, 2)));
			symbol->StartSecond(_ttoi(strLocalStartTime.Mid(4, 2)));
			symbol->EndHour(_ttoi(strLocalEndTime.Left(2)));
			symbol->EndMin(_ttoi(strLocalEndTime.Mid(2, 2)));
			symbol->EndSecond(_ttoi(strLocalEndTime.Mid(4, 2)));
		}
	}

	OnTaskComplete(nRqID);
}

int HdClient::GetAbSymbolSise(SmTaskArg& arg)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string symbol_code = arg.GetArg("symbol_code");
	std::string temp;
	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	CString sInput = temp.c_str();
	CString sReqFidInput = "000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037";
	CString strNextKey = m_CommAgent.CommGetNextKey(0, "");
	int nRqID = m_CommAgent.CommFIDRqData(DefAbSymbolSise, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

	CString msg;
	msg.Format("symbol_code = %s\n", symbol_code.c_str());
	TRACE(msg);

	_RequestMap[nRqID] = arg;

	return nRqID;
}

void HdClient::OnAbroadSymbolSise(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString	strData000 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol((LPCTSTR)strData000.Trim());
	if (!sym)
		return;


	CString	strData002 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "한글종목명");
	CString strCom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비");
	CString strComGubun = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비구분");
	CString strUpRate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비등락율");


	CString msg;
	msg.Format("종목코드 = %s, 한글종목명=%s\n", strData000, strData002);
	TRACE(msg);

	strCom.TrimRight();
	strUpRate.TrimRight();


	sym->Name((LPCTSTR)strData002.TrimRight());

	CString	strData050 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "체결시간");
	CString	strData051 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "시가");
	CString	strData053 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "고가");
	CString	strData054 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "저가");
	CString	strData055 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "누적거래량");


	sym->AccAmount( _ttoi(strData055.TrimRight()));
	sym->Quote.Close = _ttoi(strData051);
	sym->Quote.Open = _ttoi(strData052);
	sym->Quote.High = _ttoi(strData053);
	sym->Quote.Low = _ttoi(strData054);

	OnTaskComplete(nRqID);
}

int HdClient::GetAbSymbolHoga(SmTaskArg& arg)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string symbol_code = arg.GetArg("symbol_code");


	std::string temp;
	temp = VtStringUtil::PadRight(symbol_code, ' ', 32);
	CString sInput = temp.c_str();
	CString sReqFidInput = _T("000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061");
	CString strNextKey = m_CommAgent.CommGetNextKey(0, "");
	int nRqID = m_CommAgent.CommFIDRqData(DefAbSymbolHoga, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

	CString msg;
	msg.Format("nRqID = %d\n", nRqID);
	TRACE(msg);

	_RequestMap[nRqID] = arg;

	return nRqID;
}

void HdClient::OnAbroadSymbolHoga(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	CString	strData000 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol((LPCTSTR)strData000.Trim());
	if (!sym)
		return;

	CString	strData002 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "호가시간");



	CString	strData075 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "호가수신시간");
	CString	strData076 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량1");
	CString	strData079 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량1");
	CString	strData080 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수1");

	sym->Hoga.Ary[0].SellPrice = _ttoi(strData076);
	sym->Hoga.Ary[0].BuyPrice = _ttoi(strData077);
	sym->Hoga.Ary[0].SellQty = _ttoi(strData078);
	sym->Hoga.Ary[0].BuyQty = _ttoi(strData079);
	sym->Hoga.Ary[0].SellCnt = _ttoi(strData080);
	sym->Hoga.Ary[0].BuyCnt = _ttoi(strData081);


	CString	strData082 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량2");
	CString	strData085 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량2");
	CString	strData086 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수2");

	sym->Hoga.Ary[1].SellPrice = _ttoi(strData082);
	sym->Hoga.Ary[1].BuyPrice = _ttoi(strData083);
	sym->Hoga.Ary[1].SellQty = _ttoi(strData084);
	sym->Hoga.Ary[1].BuyQty = _ttoi(strData085);
	sym->Hoga.Ary[1].SellCnt = _ttoi(strData086);
	sym->Hoga.Ary[1].BuyCnt = _ttoi(strData087);
	CString	strData088 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량3");
	CString	strData091 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량3");
	CString	strData092 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수3");

	sym->Hoga.Ary[2].SellPrice = _ttoi(strData088);
	sym->Hoga.Ary[2].BuyPrice = _ttoi(strData089);
	sym->Hoga.Ary[2].SellQty = _ttoi(strData090);
	sym->Hoga.Ary[2].BuyQty = _ttoi(strData091);
	sym->Hoga.Ary[2].SellCnt = _ttoi(strData092);
	sym->Hoga.Ary[2].BuyCnt = _ttoi(strData093);
	CString	strData094 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량4");
	CString	strData097 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량4");
	CString	strData098 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수4");

	sym->Hoga.Ary[3].SellPrice = _ttoi(strData094);
	sym->Hoga.Ary[3].BuyPrice = _ttoi(strData095);
	sym->Hoga.Ary[3].SellQty = _ttoi(strData096);
	sym->Hoga.Ary[3].BuyQty = _ttoi(strData097);
	sym->Hoga.Ary[3].SellCnt = _ttoi(strData098);
	sym->Hoga.Ary[3].BuyCnt = _ttoi(strData099);
	CString	strData100 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가잔량5");
	CString	strData103 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가잔량5");
	CString	strData104 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수5");
	sym->Hoga.Ary[4].SellPrice = _ttoi(strData100);
	sym->Hoga.Ary[4].BuyPrice = _ttoi(strData101);
	sym->Hoga.Ary[4].SellQty = _ttoi(strData102);
	sym->Hoga.Ary[4].BuyQty = _ttoi(strData103);
	sym->Hoga.Ary[4].SellCnt = _ttoi(strData104);
	sym->Hoga.Ary[4].BuyCnt = _ttoi(strData105);

	CString	strData106 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총잔량");
	CString	strData107 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총잔량");
	CString	strData108 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총건수");

	sym->Hoga.TotSellQty = _ttoi(strData106);
	sym->Hoga.TotBuyQty = _ttoi(strData107);
	sym->Hoga.TotSellCnt = _ttoi(strData108);
	sym->Hoga.TotBuyCnt = _ttoi(strData109);

	OnTaskComplete(nRqID);
}


int HdClient::GetAbAsset(SmTaskArg& arg)
{
	std::string account_no = arg.GetArg("account_no");
	std::string password = arg.GetArg("password");

	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	std::string reqString;
	std::string temp;
	// 계좌 번호
	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	reqString.append(temp);
	// 비밀번호
	temp = VtStringUtil::PadRight(password, ' ', 8);
	reqString.append(temp);

	CString sInput = reqString.c_str();
	//LOG_F(INFO, _T("AbGetAsset code = %s, input = %s"), DEF_Ab_Asset, sInput);
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(DefAbAsset, sInput, sInput.GetLength(), strNextKey);
	_RequestMap[nRqID] = arg;

	return nRqID;
}

void HdClient::OnGetAbAsset(CString& sTrCode, LONG& nRqID)
{
	CString strMsg;
	strMsg.Format("해외 예탁자산 및 증거금 응답\n");
	TRACE(strMsg);
	std::string account_no;
	auto it = _RequestMap.find(nRqID);
	if (it != _RequestMap.end()) {
		SmTaskArg& arg = it->second;
		account_no = arg.GetArg("account_no");
	}
	else {
		OnTaskComplete(nRqID);
		return;
	}

// 	VtAccount* account = VtAccountManager::GetInstance()->FindAccount(account_no);
// 	if (!account) {
// 		OnTaskComplete(nRqID);
// 		return;
// 	}

	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	
	//double	Open_pos_mgn = 0;	/* 미결제증거금													*/
	//double	Ord_mgn = 0;	/* 주문증거금													*/
	//double	Trst_mgn = 0;	/* 위탁증거금													*/
	//double	Mnt_mgn = 0;	/* 유지증거금													*/
	

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strData1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "통화구분");
		CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁금총액");
		CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁금잔액");
		CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "미결제증거금");
		CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문증거금");
		CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금");
		CString strData7 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "유지증거금");
		CString strData8 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "청산손익");
		CString strData9 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "선물옵션수수료");
		CString strData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가손익");
		CString strData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가예탁총액");
		CString strData12 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "추가증거금");
		CString strData13 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가능금액");

// 		account->CurrencyCode = strData1;
// 		account->Deposit = _ttof(strData2);
// 		account->OpenDeposit = _ttof(strData11);
// 		account->OrdableAmt = _ttof(strData13);
// 		account->OpenPL = _ttof(strData10);
// 		account->TradePL = _ttof(strData8);
// 		account->Fee = _ttof(strData9);
// 
// 		account->Open_pos_mgn = _ttof(strData4);
// 		account->Ord_mgn = _ttof(strData5);
// 		account->Trst_mgn = _ttof(strData6);
// 		account->Mnt_mgn = _ttof(strData7);

	}

	OnTaskComplete(nRqID);
}

int HdClient::GetAbDeposit(SmTaskArg& arg)
{
	std::string account_no = arg.GetArg("account_no");
	std::string password = arg.GetArg("password");

	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	std::string reqString;
	std::string temp;
	reqString.append("1");
	temp = VtStringUtil::PadRight(SmGlobal::LoginUserID, ' ', 8);
	// 아이디 
	reqString.append(temp);

	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	reqString.append(temp);
	temp = VtStringUtil::PadRight(password, ' ', 8);
	reqString.append(temp);
	// 그룹명 - 공백
	reqString.append("                    ");
	// 통화코드
	reqString.append("USD");


	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(DefAbAccountProfitLoss, sInput, sInput.GetLength(), strNextKey);
	_RequestMap[nRqID] = arg;

	return nRqID;
}

void HdClient::OnGetAbDeposit(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	//VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAccount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "통화코드");
		CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁금총액");
		CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁금잔액");
		CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문증거금");
		CString strData7 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "위탁증거금");
		CString strData8 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "유지증거금");
		CString strData9 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래수수료");
		CString strData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");
		CString strData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산손익");
		CString strData12 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "추가증거금");
		CString strData13 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문가능금");


// 		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAccount.TrimRight());
// 		if (acnt)
// 		{
// 			acnt->OpenDeposit = _ttof(strData4.TrimRight());
// 			acnt->OrdableAmt = _ttof(strData13.TrimRight());
// 			acnt->CurrencyCode = strData3.Trim();
// 			acnt->OpenPL = _ttof(strData10.TrimRight());
// 			acnt->TradePL = _ttof(strData11.TrimRight());
// 			acnt->Fee = _ttof(strData9.TrimRight());
// 			acnt->Ord_mgn = _ttof(strData6.TrimRight());
// 			acnt->Trst_mgn = _ttof(strData7.TrimRight());
// 			acnt->Mnt_mgn = _ttof(strData8.TrimRight());
// 
// 		}
	}
	
	OnTaskComplete(nRqID);
}

int HdClient::GetAbAccountProfitLoss(SmTaskArg& arg)
{
	std::string account_no = arg.GetArg("account_no");
	std::string password = arg.GetArg("password");

	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	std::string reqString;
	std::string temp;
	reqString.append("1");
	temp = VtStringUtil::PadRight(SmGlobal::LoginUserID, ' ', 8);
	// 아이디 
	reqString.append(temp);

	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	reqString.append(temp);
	temp = VtStringUtil::PadRight(password, ' ', 8);
	reqString.append(temp);
	// 그룹명 - 공백
	reqString.append("                    ");
	// 통화코드
	reqString.append("USD");


	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(DefAbSymbolProfitLoss, sInput, sInput.GetLength(), strNextKey);
	_RequestMap[nRqID] = arg;

	return nRqID;
}

void HdClient::OnGetAbAccountProfitLoss(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	//VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	double fee = 0.0;
	double tradePL = 0.0;
	double totalPL = 0.0;

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++) {
		CString strAccount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		//VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAccount);

		CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "통화코드");
		CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
		CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산손익");
		CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산순손익");
		CString strData7 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "청산수수료");
		CString strData8 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");
		CString strData9 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "미결제수수료");
		CString strData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "미결제순손익");
		CString strData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");

// 		SmSymbol* sym = symMgr->FindHdSymbol((LPCTSTR)strData4.TrimRight());
// 		VtPosition* posi = acnt->FindAdd(sym->ShortCode);
// 		posi->TradePL = atof(strData5);
// 		fee += _ttof(strData7);
// 		acnt->TradePL = tradePL;
// 		acnt->Fee = fee;
// 		acnt->TotalPL = totalPL;
	}

	OnTaskComplete(nRqID);
}

int HdClient::GetAbOutStanding(SmTaskArg& arg)
{
	std::string account_no = arg.GetArg("account_no");
	std::string password = arg.GetArg("password");

	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string reqString;
	std::string temp;
	reqString.append("1");
	temp = VtStringUtil::PadRight(SmGlobal::LoginUserID, ' ', 8);
	// 아이디 
	reqString.append(temp);

	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	reqString.append(temp);
	temp = VtStringUtil::PadRight(password, ' ', 8);
	reqString.append(temp);
	// 그룹명 - 공백
	reqString.append("                    ");


	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(DefAbOutstanding, sInput, sInput.GetLength(), strNextKey);
	_RequestMap[nRqID] = arg;
	return nRqID;
}

void HdClient::OnGetAbOutStanding(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	SmRealtimeRegisterManager* realRegMgr = SmRealtimeRegisterManager::GetInstance();
	//VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAcnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strAcntName = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strSymbol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
		CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strPreOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "전일미결제수량");
		CString strOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일순 미결제수량");
		CString strAvgPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평균단가");
		CString strUnitPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평균단가(소수점반영)");
		CString strOpenPL = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");


// 		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAcnt.TrimRight());
// 		if (acnt) {
// 			VtPosition* posi = acnt->FindAdd((LPCTSTR)strSymbol.TrimRight());
// 			realRegMgr->RegisterProduct(posi->ShortCode, 0);
// 			if (strPosition.Compare(_T("1")) == 0) {
// 				posi->Position = VtPositionType::Buy;
// 				posi->OpenQty = _ttoi(strOpenQty);
// 				posi->PrevOpenQty = _ttoi(strPreOpenQty);
// 			}
// 			else if (strPosition.Compare(_T("2")) == 0) {
// 				posi->Position = VtPositionType::Sell;
// 				posi->OpenQty = -1 * _ttoi(strOpenQty);
// 				posi->PrevOpenQty = -1 * _ttoi(strPreOpenQty);
// 			}
// 			posi->AvgPrice = _ttof(strUnitPrice);
// 			SmSymbol* sym = symMgr->FindHdSymbol((LPCTSTR)strSymbol.TrimRight());
// 			if (sym && sym->Quote.intClose > 0) {
// 				posi->CurPrice = sym->Quote.intClose / std::pow(10, sym->Decimal);
// 				double curClose = sym->Quote.intClose / std::pow(10, sym->Decimal);
// 				posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu;
// 				acnt->TempOpenPL += posi->OpenProfitLoss;
// 				if (i == nRepeatCnt - 1) {
// 					//acnt->OpenPL = acnt->TempOpenPL;
// 					acnt->TempOpenPL = 0.0;
// 				}
// 			}
// 			else {
// 				acnt->TempOpenPL += _ttoi(strOpenPL);
// 				if (i == nRepeatCnt - 1) {
// 					//acnt->OpenPL = acnt->TempOpenPL;
// 					acnt->TempOpenPL = 0.0;
// 				}
// 			}
// 
// 			VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
// 			totalOrderMgr->AddPosition(0, acnt->AccountNo, posi->ShortCode, posi);
// 
// 			VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
// 			VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager(acnt->AccountNo);
// 			VtProductOrderManager* prdtOrderMgr = orderMgr->FindAddProductOrderManager((LPCTSTR)strSymbol.TrimRight());
// 			if (prdtOrderMgr)
// 				prdtOrderMgr->Init(true);
// 
// 		}

	}

	OnTaskComplete(nRqID);
}

int HdClient::GetAbAccepted(SmTaskArg& arg)
{
	std::string account_no = arg.GetArg("account_no");
	std::string password = arg.GetArg("password");

	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	std::string reqString;
	std::string temp;
	reqString.append("1");
	temp = VtStringUtil::PadRight(SmGlobal::LoginUserID, ' ', 8);
	// 아이디 
	reqString.append(temp);

	temp = VtStringUtil::PadRight(account_no, ' ', 6);
	reqString.append(temp);
	temp = VtStringUtil::PadRight(password, ' ', 8);
	reqString.append(temp);
	// 그룹명 - 공백
	reqString.append("                    ");



	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(DefAbAccepted, sInput, sInput.GetLength(), strNextKey);
	_RequestMap[nRqID] = arg;

	return nRqID;
}

void HdClient::OnGetAbAccepted(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

// 	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
// 
// 	VtOrder* order = nullptr;
// 
// 	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
// 	for (int i = 0; i < nRepeatCnt; i++)
// 	{
// 		CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
// 		CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문번호");
// 		CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
// 		CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문가격");
// 		CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문수량");
// 		CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
// 		CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "가격조건");
// 		CString strOriOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "원주문번호");
// 		CString strFirstOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "최초원주문번호");
// 
// 		strAcctNo.TrimRight();
// 		strOrdNo.TrimLeft('0');
// 		strOriOrderNo.TrimLeft('0');
// 		strFirstOrderNo.TrimLeft('0');
// 		strSeries.TrimRight();
// 
// 		VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);
// 
// 		order = orderMgr->FindOrder((LPCTSTR)(strOrdNo));
// 		if (!order) {
// 			order = new VtOrder();
// 
// 			std::string symCode = (LPCTSTR)strSeries;
// 			SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
// 			SmSymbol* sym = symMgr->FindHdSymbol(symCode);
// 
// 			std::string temp;
// 			strPrice.TrimLeft('0');
// 			if (sym) {
// 				temp = fmt::format("{:.{}f}", _ttof(strPrice), sym->Decimal);
// 			}
// 			else {
// 				OnTaskComplete(nRqID);
// 				return;
// 			}
// 
// 			strPrice = temp.c_str();
// 			strPrice.TrimLeft();
// 			strPrice.Remove('.');
// 
// 			order->AccountNo = (LPCTSTR)strAcctNo;
// 			order->shortCode = (LPCTSTR)strSeries;
// 			order->orderNo = (LPCTSTR)(strOrdNo);
// 			order->intOrderPrice = _ttoi(strPrice);
// 			order->amount = _ttoi(strAmount);
// 
// 			if (strPosition.Compare(_T("1")) == 0) {
// 				order->orderPosition = VtPositionType::Buy;
// 			}
// 			else if (strPosition.Compare(_T("2")) == 0) {
// 				order->orderPosition = VtPositionType::Sell;
// 			}
// 
// 			if (strPriceType.Compare(_T("1")) == 0) {
// 				order->priceType = VtPriceType::Price;
// 			}
// 			else if (strPosition.Compare(_T("2")) == 0) {
// 				order->priceType = VtPriceType::Market;
// 			}
// 
// 			order->orderType = VtOrderType::New;
// 
// 			order->firstOrderNo = (LPCTSTR)(strFirstOrderNo);
// 			order->oriOrderNo = (LPCTSTR)(strOriOrderNo);
// 
// 			orderMgr->OnOrderAcceptedHd(order);
// 		}
// 	}

	OnTaskComplete(nRqID);
}

int HdClient::GetAbChartData(SmTaskArg& arg)
{
	SmChartDataRequest req;
	req.symbolCode = arg.GetArg("symbol_code");
	std::string chart_type = arg.GetArg("chart_type");
	req.chartType = (SmChartType)std::stoi(chart_type);
	req.cycle = std::stoi(arg.GetArg("chart_cycle"));
	req.count = std::stoi(arg.GetArg("count"));
	req.next = std::stoi(arg.GetArg("next"));
	int nRqID = GetChartData(req);
	// 요청이 실패하면 바로 완료를 호출한다.
	if (nRqID < 0) {
		ViServerDataReceiver::GetInstance()->OnTaskComplete(arg.ArgId);
		return nRqID;
	}
	else {
		_RequestMap[nRqID] = arg;
		return nRqID;
	}
}

void HdClient::OnRcvdAbroadChartData(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	CString msg;

	auto it = _ChartDataReqMap.find(nRqID);
	if (it == _ChartDataReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}
	SmChartDataRequest req = it->second;
	//SmTimeSeriesCollector* tsCol = SmTimeSeriesCollector::GetInstance();
	SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
	std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
	int total_count = nRepeatCnt;
	int current_count = 1;
	std::vector<SmChartDataItem> chart_vec;
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = nRepeatCnt - 1; i >= 0; --i) {
		CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내일자");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내시간");
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결량");

		if (strDate.GetLength() == 0)
			continue;

		msg.Format(_T("OnRcvdAbroadChartData ::code = %s, index = %d, date = %s, t = %s, o = %s, h = %s, l = %s, c = %s, v = %s\n"), req.symbolCode.c_str(), i, strDate, strTime, strOpen, strHigh, strLow, strClose, strVol);
		TRACE(msg);

		SmChartDataItem data;
		data.symbolCode = req.symbolCode;
		data.chartType = req.chartType;
		data.cycle = req.cycle;
		data.date = strDate.Trim();
		data.time = strTime.Trim();
		data.h = _ttoi(strHigh);
		data.l = _ttoi(strLow);
		data.o = _ttoi(strOpen);
		data.c = _ttoi(strClose);
		data.v = _ttoi(strVol);

		chart_vec.push_back(data);
	}

	OnTaskComplete(nRqID);
}

void HdClient::OnRcvdAbroadChartData2(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString symbol_code = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	symbol_code = symbol_code.TrimRight();

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");

	CString msg;

	auto it = _ChartDataReqMap.find(nRqID);
	if (it == _ChartDataReqMap.end()) {
		OnTaskComplete(nRqID);
		return;
	}
	SmChartDataRequest req = it->second;
	SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
	std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
	int total_count = nRepeatCnt;
	int current_count = 1;
	std::vector<SmChartDataItem> chart_vec;
	// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
	// Received the chart data first.
	for (int i = 0; i < nRepeatCnt; ++i) {
		CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "국내일자");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "국내시간");
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
		CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "체결량");
		CString strTotalVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "누적거래량");


		if (strDate.GetLength() == 0)
			continue;

		msg.Format(_T("OnRcvdAbroadChartData :: index = %d, symbol_code = %s, date = %s, t = %s, o = %s, h = %s, l = %s, c = %s, v = %s\n"), i, symbol_code, strDate, strTime, strOpen, strHigh, strLow, strClose, strVol);
		TRACE(msg);


		SmChartDataItem data;
		data.symbolCode = req.symbolCode;
		data.chartType = req.chartType;
		data.cycle = req.cycle;
		data.date = strDate.Trim();
		data.time = strTime.Trim();
		data.h = _ttoi(strHigh);
		data.l = _ttoi(strLow);
		data.o = _ttoi(strOpen);
		data.c = _ttoi(strClose);
		data.v = _ttoi(strVol);

		chart_vec.push_back(data);
	}

	OnTaskComplete(nRqID);
}
// 
// void HdClient::PutAbOrder(HdOrderRequest& request)
// {
// 	if (!CheckPassword(request))
// 		return;
// 
// 	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
// 
// 	std::string orderString;
// 	std::string temp;
// 	// 계좌 번호
// 	temp = VtStringUtil::PadRight(request.AccountNo, ' ', 6);
// 	orderString.append(temp);
// 	// 비밀번호
// 	temp = VtStringUtil::PadRight(request.Password, ' ', 8);
// 	orderString.append(temp);
// 	// 종목 코드
// 	temp = VtStringUtil::PadRight(request.SymbolCode, ' ', 32);
// 	orderString.append(temp);
// 
// 	// 매매구분
// 	if (request.Position == VtPositionType::Buy)
// 		orderString.append(_T("1"));
// 	else if (request.Position == VtPositionType::Sell)
// 		orderString.append(_T("2"));
// 
// 	// 가격 조건
// 	if (request.PriceType == VtPriceType::Price)
// 		orderString.append(_T("1"));
// 	else if (request.PriceType == VtPriceType::Market)
// 		orderString.append(_T("2"));
// 	// 체결 조건
// 	if (request.FillCondition == VtFilledCondition::Fas)
// 		orderString.append(_T("1"));
// 	else if (request.FillCondition == VtFilledCondition::Fok)
// 		orderString.append(_T("2"));
// 	else if (request.FillCondition == VtFilledCondition::Fak)
// 		orderString.append(_T("3"));
// 	else if (request.FillCondition == VtFilledCondition::Day)
// 		orderString.append(_T("0"));
// 
// 	// 주문 가격
// 	if (request.PriceType == VtPriceType::Price)
// 		temp = VtStringUtil::PadRight(request.Price, ' ', 15);
// 	else if (request.PriceType == VtPriceType::Market)
// 		temp = VtStringUtil::PadRight(0, ' ', 15);
// 	orderString.append(temp);
// 
// 	// 주문 수량
// 	temp = VtStringUtil::PadRight(request.Amount, ' ', 10);
// 	orderString.append(temp);
// 	// 기타 설정
// 	temp = VtStringUtil::PadRight(1, ' ', 35);
// 	orderString.append(temp);
// 
// 	std::string userDefined;
// 	if (request.Type == 0)
// 		userDefined.append(_T("00000"));
// 	else if (request.Type == 1)
// 		userDefined.append(_T("11111"));
// 	else if (request.Type == 2)
// 		userDefined.append(_T("22222"));
// 	else
// 		return;
// 	userDefined.append(",");
// 	userDefined.append("fund_name");
// 	userDefined.append(",");
// 	userDefined.append(request.FundName);
// 	userDefined.append(",");
// 	userDefined.append("sub_account_no");
// 	userDefined.append(",");
// 	userDefined.append(request.SubAccountNo);
// 	userDefined.append(",");
// 	if (request.Type == 0)
// 		temp = VtStringUtil::PadRight(userDefined, '0', 60);
// 	else if (request.Type == 1)
// 		temp = VtStringUtil::PadRight(userDefined, '1', 60);
// 	else if (request.Type == 2)
// 		temp = VtStringUtil::PadRight(userDefined, '2', 60);
// 
// 	orderString.append(temp);
// 
// 	CString sTrCode = "g12003.AO0401%";
// 	CString sInput = orderString.c_str();
// 	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
// 	request.HtsOrderReqID = nRqID;
// 
// 	// 주문요청 번호와 함께 주문요청 맵에 넣어준다.
// 	_OrderReqMap[nRqID] = request;
// 
// 	//LOG_F(INFO,_T("PutOrder :: Req id = %d, order string = %s"), nRqID, orderString.c_str());
// 	//LOG_F(INFO, _T("신규주문요청 : 요청번호 = %d, 종목이름 = %s, 원주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %d, 요청 타입 = %d"),
// 	//	nRqID, request.SymbolCode.c_str(), request.OrderNo.c_str(), request.AccountNo.c_str(), request.SubAccountNo.c_str(),
// 	//	request.FundName.c_str(), request.Position == VtPositionType::Buy ? _T("매수") : _T("매도"), request.Amount, request.RequestType);
// }
// 
// void HdClient::ChangeAbOrder(HdOrderRequest& request)
// {
// 	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
// 
// 	if (!CheckPassword(request))
// 		return;
// 
// 	std::string orderString;
// 	std::string temp;
// 	// 계좌 번호
// 	temp = VtStringUtil::PadRight(request.AccountNo, ' ', 6);
// 	orderString.append(temp);
// 	// 비밀번호
// 	temp = VtStringUtil::PadRight(request.Password, ' ', 8);
// 	orderString.append(temp);
// 	// 종목 코드
// 	temp = VtStringUtil::PadRight(request.SymbolCode, ' ', 32);
// 	orderString.append(temp);
// 
// 	// 가격 조건
// 	if (request.PriceType == VtPriceType::Price)
// 		orderString.append(_T("1"));
// 	else if (request.PriceType == VtPriceType::Market)
// 		orderString.append(_T("2"));
// 	// 체결 조건
// 	if (request.FillCondition == VtFilledCondition::Fas)
// 		orderString.append(_T("1"));
// 	else if (request.FillCondition == VtFilledCondition::Fok)
// 		orderString.append(_T("2"));
// 	else if (request.FillCondition == VtFilledCondition::Fak)
// 		orderString.append(_T("3"));
// 	else if (request.FillCondition == VtFilledCondition::Day)
// 		orderString.append(_T("0"));
// 
// 	// 주문 가격
// 	if (request.PriceType == VtPriceType::Price)
// 		temp = VtStringUtil::PadRight(request.Price, ' ', 15);
// 	else if (request.PriceType == VtPriceType::Market)
// 		temp = VtStringUtil::PadRight(0, ' ', 15);
// 	orderString.append(temp);
// 
// 	// 정정 수량
// 	temp = VtStringUtil::PadRight(request.Amount, ' ', 10);
// 	orderString.append(temp);
// 	// 정정이나 취소시 원주문 번호
// 	temp = VtStringUtil::PadRight(request.OrderNo, ' ', 10);
// 	orderString.append(temp);
// 	// 기타설정
// 	temp = VtStringUtil::PadRight(1, ' ', 26);
// 	orderString.append(temp);
// 
// 	std::string userDefined;
// 	if (request.Type == 0)
// 		userDefined.append(_T("00000"));
// 	else if (request.Type == 1)
// 		userDefined.append(_T("11111"));
// 	else if (request.Type == 2)
// 		userDefined.append(_T("22222"));
// 	else
// 		return;
// 	userDefined.append(",");
// 	userDefined.append("fund_name");
// 	userDefined.append(",");
// 	userDefined.append(request.FundName);
// 	userDefined.append(",");
// 	userDefined.append("sub_account_no");
// 	userDefined.append(",");
// 	userDefined.append(request.SubAccountNo);
// 	userDefined.append(",");
// 	if (request.Type == 0)
// 		temp = VtStringUtil::PadRight(userDefined, '0', 60);
// 	else if (request.Type == 1)
// 		temp = VtStringUtil::PadRight(userDefined, '1', 60);
// 	else if (request.Type == 2)
// 		temp = VtStringUtil::PadRight(userDefined, '2', 60);
// 
// 	orderString.append(temp);
// 
// 	CString sTrCode = "g12003.AO0402%";
// 	CString sInput = orderString.c_str();
// 	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
// 	request.HtsOrderReqID = nRqID;
// 	// 주문요청 번호와 함께 주문요청 맵에 넣어준다.
// 	_OrderReqMap[nRqID] = request;
// 
// 	CString sInputEx = "1234561234    6AM16                           107155           1         16Ke0005641                         ";
// 
// 	//LOG_F(INFO,_T("PutOrder :: Req id = %d, order string = %s"), nRqID, orderString.c_str());
// 	//LOG_F(INFO, _T("신규주문요청 : 요청번호 = %d, 종목이름 = %s, 원주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %d, 요청 타입 = %d"),
// 	//	nRqID, request.SymbolCode.c_str(), request.OrderNo.c_str(), request.AccountNo.c_str(), request.SubAccountNo.c_str(),
// 	//	request.FundName.c_str(), request.Position == VtPositionType::Buy ? _T("매수") : _T("매도"), request.Amount, request.RequestType);
// }
// 
// void HdClient::CancelAbOrder(HdOrderRequest& request)
// {
// 	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
// 
// 	if (!CheckPassword(request))
// 		return;
// 
// 	std::string orderString;
// 	std::string temp;
// 	// 계좌 번호
// 	temp = VtStringUtil::PadRight(request.AccountNo, ' ', 6);
// 	orderString.append(temp);
// 	// 비밀번호
// 	temp = VtStringUtil::PadRight(request.Password, ' ', 8);
// 	orderString.append(temp);
// 	// 종목 코드
// 	temp = VtStringUtil::PadRight(request.SymbolCode, ' ', 32);
// 	orderString.append(temp);
// 
// 	// 가격 조건
// 	if (request.PriceType == VtPriceType::Price)
// 		orderString.append(_T("1"));
// 	else if (request.PriceType == VtPriceType::Market)
// 		orderString.append(_T("2"));
// 	// 체결 조건
// 	if (request.FillCondition == VtFilledCondition::Fas)
// 		orderString.append(_T("1"));
// 	else if (request.FillCondition == VtFilledCondition::Fok)
// 		orderString.append(_T("2"));
// 	else if (request.FillCondition == VtFilledCondition::Fak)
// 		orderString.append(_T("3"));
// 	else if (request.FillCondition == VtFilledCondition::Day)
// 		orderString.append(_T("0"));
// 
// 	// 주문 가격 15
// 	temp = "               ";
// 	orderString.append(temp);
// 
// 	// 정정 수량 10
// 	temp = "          ";
// 	orderString.append(temp);
// 	// 정정이나 취소시 원주문 번호
// 	temp = VtStringUtil::PadRight(request.OrderNo, ' ', 10);
// 	orderString.append(temp);
// 	// 기타설정 26
// 	temp = "                          ";
// 	orderString.append(temp);
// 
// 	std::string userDefined;
// 	if (request.Type == 0)
// 		userDefined.append(_T("00000"));
// 	else if (request.Type == 1)
// 		userDefined.append(_T("11111"));
// 	else if (request.Type == 2)
// 		userDefined.append(_T("22222"));
// 	else
// 		return;
// 	userDefined.append(",");
// 	userDefined.append("fund_name");
// 	userDefined.append(",");
// 	userDefined.append(request.FundName);
// 	userDefined.append(",");
// 	userDefined.append("sub_account_no");
// 	userDefined.append(",");
// 	userDefined.append(request.SubAccountNo);
// 	userDefined.append(",");
// 	if (request.Type == 0)
// 		temp = VtStringUtil::PadRight(userDefined, '0', 60);
// 	else if (request.Type == 1)
// 		temp = VtStringUtil::PadRight(userDefined, '1', 60);
// 	else if (request.Type == 2)
// 		temp = VtStringUtil::PadRight(userDefined, '2', 60);
// 
// 	orderString.append(temp);
// 
// 	CString sTrCode = "g12003.AO0403%";
// 	CString sInput = orderString.c_str();
// 	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
// 	request.HtsOrderReqID = nRqID;
// 	// 주문요청 번호와 함께 주문요청 맵에 넣어준다.
// 	_OrderReqMap[nRqID] = request;
// 
// 	//LOG_F(INFO,_T("PutOrder :: Req id = %d, order string = %s"), nRqID, orderString.c_str());
// 	//LOG_F(INFO, _T("신규주문요청 : 요청번호 = %d, 종목이름 = %s, 원주문 번호 = %d, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %d, 요청 타입 = %d"),
// 	//	nRqID, request.SymbolCode.c_str(), request.OrderNo.c_str(), request.AccountNo.c_str(), request.SubAccountNo.c_str(),
// 	//	request.FundName.c_str(), request.Position == VtPositionType::Buy ? _T("매수") : _T("매도"), request.Amount, request.RequestType);
// }

void HdClient::OnNewAbOrderHd(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	
	CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
	CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리구분");
	CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문수량");
	CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매매구분");
	CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가격구분");
	CString strCustom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "조작구분");
	/*
	//CString strMsg;
	//strMsg.Format("OnNewOrderHd 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]\n", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);
	//TRACE(strMsg);

	CString strMsg;
	//strMsg.Format("OnOrderAcceptedHd 계좌번호[%s]주문번호[%s][원주문번호[%s]\n", strAcctNo, strOrdNo, strOriOrderNo);
	//WriteLog(strMsg);
	strMsg.Format(_T("OnNewOrderHd>>%s\n"), strCustom);
	TRACE(strMsg);

	//LOG_F(INFO, _T("%s >>> 요청번호:%d, 계좌번호:%s, 주문번호:%s, 종목코드:%s, 주문가격:%s, 주문수량:%s, 매매구분:%s, 가격구분:%s"), __FUNCTION__, nRqID, strAcctNo, strOrdNo, strSeries, strPrice, strAmount, strPosition, strPriceType);

	strPrice = strPrice.TrimLeft('0');
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice;
	strPrice.Remove('.'); // 주문가격을 정수로 변환

	strAcctNo.TrimRight(); // 계좌 번호
	strOrdNo.TrimLeft('0'); // 주문 번호
	strOrdNo.TrimLeft(' '); // 주문 번호
	strSeries.TrimRight(); // 심볼 코드
	strPrice.TrimRight(); // 주문 가격
	strAmount.TrimRight(); // 주문 수량

	SmOrderInfo info;
	info.command = 1;
	info.nRqID = nRqID;
	info.event = VtOrderEvent::PutNew;
	info.acc_no = strAcctNo;
	info.symbol_code = strSeries;
	info.ord_no = strOrdNo;
	//info.ori_order_no = strOriOrderNo;
	//info.first_order_no = strFirstOrderNo;
	info.amount = strAmount;
	//info.remain = strRemain;
	//info.mody_cnt = strModyCnt;
	//info.fill_amount = strFilledCnt;
	//info.cancel_cnt = strCancelCnt;
	info.position = strPosition;
	info.price = strPrice;
	info.fprice = strOriOrderPrice;
	info.custom = strCustom;
	info.man = strMan;
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	totalOrderMgr->OnOrderNew(std::move(info));
	*/
}

void HdClient::OnModifyAbOrderHd(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
	CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리구분");
	CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문수량");
	CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매매구분");
	CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가격구분");
	CString strCustom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "조작구분");
	CString strOriOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "원주문번호");
	/*
	CString strMsg;
	//strMsg.Format("OnOrderAcceptedHd 계좌번호[%s]주문번호[%s][원주문번호[%s]\n", strAcctNo, strOrdNo, strOriOrderNo);
	//WriteLog(strMsg);
	strMsg.Format(_T("OnModifyOrderHd%s\n"), strCustom);
	TRACE(strMsg);

	//LOG_F(INFO, _T("%s >>> 요청번호:%d, 계좌번호:%s, 주문번호:%s, 원주문번호:%s, 종목코드:%s, 주문가격:%s, 주문수량:%s, 매매구분:%s, 가격구분:%s"), __FUNCTION__, nRqID, strAcctNo, strOrdNo, strOriOrdNo, strSeries, strPrice, strAmount, strPosition, strPriceType);

	//CString strMsg;
	//strMsg.Format("OnModifyOrderHd 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]\n", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	//TRACE(strMsg);

	strPrice = strPrice.TrimLeft('0'); // 주문 가격
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice; // 소수점 주문 가격 저장
	strPrice.Remove('.'); // 주문 가격 정수형으로 변환
	strAcctNo.TrimRight(); // 계좌 번호
	strOrdNo.TrimLeft('0'); // 주문 번호
	strSeries.TrimRight(); // 심볼 코드
	strPrice.TrimRight(); // 주문 가격 
	strAmount.TrimRight(); // 주문 수량
	strOriOrdNo.TrimRight(); // 원주문 번호

	SmOrderInfo info;
	info.command = 1;
	info.nRqID = nRqID;
	info.event = VtOrderEvent::Modified;
	info.acc_no = strAcctNo;
	info.symbol_code = strSeries;
	info.ord_no = strOrdNo;
	//info.ori_order_no = strOriOrderNo;
	//info.first_order_no = strFirstOrderNo;
	info.amount = strAmount;
	//info.remain = strRemain;
	//info.mody_cnt = strModyCnt;
	//info.fill_amount = strFilledCnt;
	//info.cancel_cnt = strCancelCnt;
	info.position = strPosition;
	info.price = strPrice;
	info.fprice = strOriOrderPrice;
	info.custom = strCustom;
	info.man = strMan;
	info.ori_order_no = strOriOrdNo;
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	totalOrderMgr->OnOrderModify(std::move(info));
	*/
}

void HdClient::OnCancelAbOrderHd(CString& sTrCode, LONG& nRqID)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
	CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리구분");
	CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문수량");
	CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매매구분");
	CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가격구분");
	CString strCustom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "사용자정의필드");
	CString strOriOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "원주문번호");

	/*
	CString strMsg;
	//strMsg.Format("OnOrderAcceptedHd 계좌번호[%s]주문번호[%s][원주문번호[%s]\n", strAcctNo, strOrdNo, strOriOrderNo);
	//WriteLog(strMsg);
	strMsg.Format(_T("OnCancelOrderHd%s\n"), strCustom);
	TRACE(strMsg);

	//LOG_F(INFO, _T("%s >>> 요청번호:%d, 계좌번호:%s, 주문번호:%s, 원주문번호:%s, 종목코드:%s, 주문가격:%s, 주문수량:%s, 매매구분:%s, 가격구분:%s"), __FUNCTION__, nRqID, strAcctNo, strOrdNo, strOriOrdNo, strSeries, strPrice, strAmount, strPosition, strPriceType);

	//CString strMsg;
	//strMsg.Format("OnCancelOrderHd 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]\n", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	//TRACE(strMsg);

	// 주문 가격
	strPrice = strPrice.TrimLeft('0');
	CString strOriOrderPrice;
	// 원주문가격 저장
	strOriOrderPrice = strPrice;

	// 주문 가겨을 정수로 변환
	strPrice.Remove('.');
	// 계좌 번호
	strAcctNo.TrimRight();
	// 주문 번호
	strOrdNo.TrimLeft('0');
	// 심볼 코드
	strSeries.TrimRight();
	// 주문 수량
	strAmount.TrimRight();
	// 원주문번호
	strOriOrdNo.TrimRight();

	SmOrderInfo info;
	info.command = 1;
	info.nRqID = nRqID;
	info.event = VtOrderEvent::Cancelled;
	info.acc_no = strAcctNo;
	info.symbol_code = strSeries;
	info.ord_no = strOrdNo;
	//info.ori_order_no = strOriOrderNo;
	//info.first_order_no = strFirstOrderNo;
	info.amount = strAmount;
	//info.remain = strRemain;
	//info.mody_cnt = strModyCnt;
	//info.fill_amount = strFilledCnt;
	//info.cancel_cnt = strCancelCnt;
	info.position = strPosition;
	info.price = strPrice;
	info.fprice = strOriOrderPrice;
	info.custom = strCustom;
	info.ori_order_no = strOriOrdNo;
	//info.man = strMan;
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	totalOrderMgr->OnOrderCancel(std::move(info));
	*/
}

void HdClient::OnAbOrderAcceptedHd(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	//CString strPriceType = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "가격구분");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문구분");
	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	CString strTraderTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문시간");

	//LOG_F(INFO, _T(" OnOrderAcceptedHd Custoem = %s"), strCustom);

	CString strMsg;
	//strMsg.Format("OnOrderAcceptedHd 계좌번호[%s]주문번호[%s][원주문번호[%s]\n", strAcctNo, strOrdNo, strOriOrderNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("OnOrderAcceptedHd>>%s\n"), strCustom);
	//TRACE(strMsg);
	strCustom.Trim();
	strAcctNo.TrimRight(); // 계좌 번호
	strOrdNo.TrimLeft('0'); // 주문 번호
	strSeries.TrimRight(); // 심볼 코드
	strPrice = strPrice.Trim(); // 주문 가격 트림
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice; // 원주문가격 저장

	// 주문 가격을 정수로 변환
	int count = strPrice.Remove('.');
	// 주문 가격 트림
	strPrice.TrimRight();
	// 주문 수량 트림
	strAmount.TrimRight();

// 	SmOrderInfo info;
// 	info.command = 1;
// 	info.event = VtOrderEvent::Accepted;
// 	info.acc_no = strAcctNo;
// 	info.symbol_code = strSeries;
// 	info.ord_no = strOrdNo;
// 	info.ori_order_no = strOriOrderNo;
// 	info.first_order_no = strFirstOrderNo;
// 	info.amount = strAmount;
// 	info.position = strPosition;
// 	info.price = strPrice;
// 	info.fprice = strOriOrderPrice;
// 	info.custom = strCustom;
// 	info.man = strMan;
// 	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
// 	totalOrderMgr->OnOrderAccepted(std::move(info));
}

void HdClient::OnAbOrderUnfilledHd(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");
	CString strCancelCnt = strAmount; //m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "취소수량");
	CString strModyCnt = strAmount; //m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "정정수량");
	CString strFilledCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "잔량");

	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	

	CString strMsg;
	//strMsg.Format("OnOrderAcceptedHd 계좌번호[%s]주문번호[%s][원주문번호[%s]\n", strAcctNo, strOrdNo, strOriOrderNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("OnOrderUnfilledHd>>%s\n"), strCustom);
	//TRACE(strMsg);

	strCustom.Trim();
	// 주문 가격
	strPrice = strPrice.Trim();
	CString strOriOrderPrice;
	// 원 주문 가격 저장
	strOriOrderPrice = strPrice;
	// 주문 가격을 정수로 변환
	int count = strPrice.Remove('.');
	// 계좌 번호 트림
	strAcctNo.TrimRight();
	// 주문 번호 트림
	strOrdNo.TrimLeft('0');
	// 원주문 번호 트림
	strOriOrderNo.TrimLeft('0');
	// 첫주문 번호 트림
	strFirstOrderNo.TrimLeft('0');
	// 심볼 코드 트림
	strSeries.TrimRight();
	// 주문 수량 트림
	strAmount.TrimRight();
	// 정정이나 취소시 처리할 수량 트림
	strRemain.TrimRight();
	// 정정이 이루어진 수량
	strModyCnt.TrimRight();
	// 체결된 수량
	strFilledCnt.TrimRight();
	// 취소된 수량
	strCancelCnt.TrimRight();

	//CString strMsg;
	strMsg.Format("OnOrderUnfilledHd 계좌번호[%s]주문번호[%s]\n", strAcctNo, strOrdNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	//TRACE(strMsg);
// 	SmOrderInfo info;
// 	info.command = 1;
// 	info.event = VtOrderEvent::Unfilled;
// 	info.acc_no = strAcctNo;
// 	info.symbol_code = strSeries;
// 	info.ord_no = strOrdNo;
// 	info.ori_order_no = strOriOrderNo;
// 	info.first_order_no = strFirstOrderNo;
// 	info.amount = strAmount;
// 	info.remain = strRemain;
// 	info.mody_cnt = strModyCnt;
// 	info.fill_amount = strFilledCnt;
// 	info.cancel_cnt = strCancelCnt;
// 	info.position = strPosition;
// 	info.price = strPrice;
// 	info.fprice = strOriOrderPrice;
// 	info.custom = strCustom;
// 	info.man = strMan;
// 	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
// 	totalOrderMgr->OnOrderUnfilled(std::move(info));
}

void HdClient::OnAbOrderFilledHd(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");

	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");


	CString strFillPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가격");
	CString strFillAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strFillTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");

	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");

	CString strFee = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "수수료");

	CString strMsg;
	//strMsg.Format("OnOrderAcceptedHd 계좌번호[%s]주문번호[%s][원주문번호[%s]\n", strAcctNo, strOrdNo, strOriOrderNo);
	//WriteLog(strMsg);
	strMsg.Format(_T("OnOrderFilledHd 수수료 = %s\n"), strFee);
	TRACE(strMsg);

	//LOG_F(INFO, _T(" OnOrderFilledHd Custoem = %s"), strCustom);

	strCustom.Trim();
	// 체결된 가격
	strFillPrice = strFillPrice.Trim();
	CString strOriFill = strFillPrice;
	// 체결된 가격을 정수로 변환
	int count = strFillPrice.Remove('.');
	// 계좌 번호 트림
	strAcctNo.TrimRight();
	// 주문 번호 트림
	strOrdNo.TrimLeft('0');
	// 심볼 코드
	strSeries.TrimRight();
	// 소수로 표시된 체결 가격
	strFillPrice.TrimRight();
	// 체결 수량
	strFillAmount.TrimLeft();
	// 체결된 시각
	strFillTime.TrimRight();

// 	SmOrderInfo info;
// 	info.command = 1;
// 	info.event = VtOrderEvent::Filled;
// 	info.acc_no = strAcctNo;
// 	info.symbol_code = strSeries;
// 	info.ord_no = strOrdNo;
// 	//info.ori_order_no = strOriOrderNo;
// 	//info.first_order_no = strFirstOrderNo;
// 	info.amount = strAmount;
// 	//info.remain = strRemain;
// 	//info.mody_cnt = strModyCnt;
// 	info.fill_amount = strFillAmount;
// 	//info.cancel_cnt = strCancelCnt;
// 	info.position = strPosition;
// 	info.price = strPrice;
// 	info.fee = strFee;
// 	//info.fprice = strOriOrderPrice;
// 	info.fill_time = strFillTime;
// 	info.fill_price = strFillPrice;
// 	info.ffill_price = strOriFill;
// 	info.fill_amount = strFillAmount;
// 	info.custom = strCustom;
// 	info.man = strMan;
// 	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
// 	totalOrderMgr->OnOrderFilled(std::move(info));
}



void HdClient::RegisterProduct(CString symCode)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	if (isdigit(symCode.GetAt(2))) {
		int nRealType = 0;
		int nResult = 0;
		CString strKey = symCode;
		TCHAR first = symCode.GetAt(0);
		CString prefix = symCode.Left(3);
		if (first == '1' || first == '4') {
			if (prefix.Compare(_T("167")) == 0 || prefix.Compare(_T("175")) == 0) {
				nRealType = 58;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 71;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
			else {
				nRealType = 51;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
				nRealType = 65;
				nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			}
		}
		else if (first == '2' || first == '3') {
			nRealType = 52;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			nRealType = 66;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
		else {
			nRealType = 82;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
	}
	else {
		std::string code = symCode;
		std::string key = VtStringUtil::PadRight(code, ' ', 32);
		int nRealType = 76; // 시세
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
		nRealType = 82; // 호가
		m_CommAgent.CommSetBroad(key.c_str(), nRealType);
	}
}

void HdClient::UnregisterProduct(CString symCode)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	int nRealType = 0;
	int nResult = 0;
	CString strKey = symCode;
	TCHAR first = symCode.GetAt(0);
	if (first == '1' || first == '4')
	{
		nRealType = 51;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		nRealType = 65;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
	}
	else if (first == '2' || first == '3')
	{
		nRealType = 52;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		nRealType = 66;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
	}
}

void HdClient::RegisterAccount(CString accountNo)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strUserId = SmGlobal::LoginUserID.c_str();
	CString strAcctNo = accountNo;
	int nResult = m_CommAgent.CommSetJumunChe(strUserId, strAcctNo);
}

void HdClient::UnregisterAccount(CString accountNo)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strUserId = SmGlobal::LoginUserID.c_str();
	CString strAcctNo = accountNo;
	int nResult = m_CommAgent.CommRemoveJumunChe(strUserId, strAcctNo);
}

void HdClient::OnAbRealtimeSise(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();

	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "기준체결시간");
	CString strCloseP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가");
	CString strOpen = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString strHigh = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString strLow = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");
	CString strAccAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "누적거래량");
	CString strPreDayCmp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");
	CString strUpRate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비등락율");
	CString strLocalDate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "국내일자");
	// 전일대비등락율
// 
// 	//m_edSeriesO.SetWindowText(strSeries);
// 	//m_edTimeO.SetWindowText(strTime);
// 	CString strFormatPrice = strCloseP;
// 	strFormatPrice = m_CommAgent.CommGetHWOrdPrice(strSeries, strCloseP, 0);
// 	//m_edClosePO.SetWindowText(strFormatPrice);
// 	//m_edVolumeO.SetWindowText(strVolume);
// 
// 	CString strType1, strType2, strType3, strType4;
// 	strCloseP = strCloseP;//"1241300";
// 	strType1 = m_CommAgent.CommGetHWOrdPrice(strSeries, strCloseP, 0);
// 	strType2 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType1, 1);
// 	strType3 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType2, 2);
// 	strType4 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType3, 0);
// 
// 	CString strMsg;
// 	strMsg.Format("Real 82 Recv[%s]->[%s]->[%s]->[%s]->[%s]", strCloseP, strType1, strType2, strType3, strType4);
// 
// 	//WriteLog(strMsg);
// 
// 	CString strBuff = m_CommAgent.CommGetDataDirect(strKey, nRealType, 0, 230, 0, "A");
// 	CString strSeries1(strBuff, 10);
// 	strMsg = strSeries1;
// 	//WriteLog(strMsg);
// 
// 	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
// 	SmSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
// 	if (!sym)
// 		return;
// 
// 	sym->Quote.Close = _ttoi(strCloseP);
// 	sym->Quote.Open = _ttoi(strOpen);
// 	sym->Quote.High = _ttoi(strHigh);
// 	sym->Quote.Low = _ttoi(strLow);
// 	sym->AccAmount(_ttoi(strAccAmount));





	SmQuoteData quote;

	quote.symbol_code = (LPCTSTR)strSeries.TrimRight();
	quote.command = 1;
	quote.time = (LPCTSTR)strTime.Trim();
	quote.volume = (LPCTSTR)strVolume.Trim();
	quote.up_down = (LPCTSTR)strUpdown.Trim();
	quote.sign = (LPCTSTR)strUpdown.Trim();
	quote.up_down_rate = (LPCTSTR)strUpRate.Trim();
	quote.to_preday = (LPCTSTR)strPreDayCmp.Trim();
	quote.close = (LPCTSTR)strCloseP;
	quote.open = (LPCTSTR)strOpen;
	quote.high = (LPCTSTR)strHigh;
	quote.low = (LPCTSTR)strLow;
	quote.acc_vol = (LPCTSTR)strAccAmount;
	quote.local_date = (LPCTSTR)strLocalDate;

	// 쓰레드 큐에 넣는다.
	SmRealtimeQuoteManager::GetInstance()->AddFutTask(std::move(quote));
}

void HdClient::OnAbRealtimeHoga(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();


	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");



	CString	strData076 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strData079 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strData080 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");



	CString	strData082 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strData085 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strData086 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");


	CString	strData088 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strData091 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strData092 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");


	CString	strData094 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strData097 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strData098 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");


	CString	strData100 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strData103 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strData104 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");

	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	CString	strData106 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strData107 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strData108 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	//CString	strData000 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");

// 	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
// 	SmSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
// 	if (!sym)
// 		return;
// 
// 
// 	sym->Hoga.Ary[0].SellPrice = _ttoi(strData076);
// 	sym->Hoga.Ary[0].BuyPrice = _ttoi(strData077);
// 	sym->Hoga.Ary[0].SellQty = _ttoi(strData078);
// 	sym->Hoga.Ary[0].BuyQty = _ttoi(strData079);
// 	sym->Hoga.Ary[0].SellCnt = _ttoi(strData080);
// 	sym->Hoga.Ary[0].BuyCnt = _ttoi(strData081);
// 
// 
// 
// 	sym->Hoga.Ary[1].SellPrice = _ttoi(strData082);
// 	sym->Hoga.Ary[1].BuyPrice = _ttoi(strData083);
// 	sym->Hoga.Ary[1].SellQty = _ttoi(strData084);
// 	sym->Hoga.Ary[1].BuyQty = _ttoi(strData085);
// 	sym->Hoga.Ary[1].SellCnt = _ttoi(strData086);
// 	sym->Hoga.Ary[1].BuyCnt = _ttoi(strData087);
// 
// 
// 	sym->Hoga.Ary[2].SellPrice = _ttoi(strData088);
// 	sym->Hoga.Ary[2].BuyPrice = _ttoi(strData089);
// 	sym->Hoga.Ary[2].SellQty = _ttoi(strData090);
// 	sym->Hoga.Ary[2].BuyQty = _ttoi(strData091);
// 	sym->Hoga.Ary[2].SellCnt = _ttoi(strData092);
// 	sym->Hoga.Ary[2].BuyCnt = _ttoi(strData093);
// 
// 
// 	sym->Hoga.Ary[3].SellPrice = _ttoi(strData094);
// 	sym->Hoga.Ary[3].BuyPrice = _ttoi(strData095);
// 	sym->Hoga.Ary[3].SellQty = _ttoi(strData096);
// 	sym->Hoga.Ary[3].BuyQty = _ttoi(strData097);
// 	sym->Hoga.Ary[3].SellCnt = _ttoi(strData098);
// 	sym->Hoga.Ary[3].BuyCnt = _ttoi(strData099);
// 
// 	sym->Hoga.Ary[4].SellPrice = _ttoi(strData100);
// 	sym->Hoga.Ary[4].BuyPrice = _ttoi(strData101);
// 	sym->Hoga.Ary[4].SellQty = _ttoi(strData102);
// 	sym->Hoga.Ary[4].BuyQty = _ttoi(strData103);
// 	sym->Hoga.Ary[4].SellCnt = _ttoi(strData104);
// 	sym->Hoga.Ary[4].BuyCnt = _ttoi(strData105);
// 
// 	sym->Hoga.TotSellQty = _ttoi(strData106);
// 	sym->Hoga.TotBuyQty = _ttoi(strData107);
// 	sym->Hoga.TotSellCnt = _ttoi(strData108);
// 	sym->Hoga.TotBuyCnt = _ttoi(strData109);

	

	SmHogaItem hoga_item;
	hoga_item.time = (LPCTSTR)strTime.Trim();
	hoga_item.symbol_code = (LPCTSTR)strSeries.Trim();

	hoga_item.sell_price1 = (LPCTSTR)strData076.Trim();
	hoga_item.buy_price1 = (LPCTSTR)strData077.Trim();
	hoga_item.sell_qty1 = (LPCTSTR)strData078.Trim();
	hoga_item.buy_qty1 = (LPCTSTR)strData079.Trim();
	hoga_item.sell_count1 = (LPCTSTR)strData080.Trim();
	hoga_item.buy_count1 = (LPCTSTR)strData081.Trim();

	hoga_item.sell_price2 = (LPCTSTR)strData082.Trim();
	hoga_item.buy_price2 = (LPCTSTR)strData083.Trim();
	hoga_item.sell_qty2 = (LPCTSTR)strData084.Trim();
	hoga_item.buy_qty2 = (LPCTSTR)strData085.Trim();
	hoga_item.sell_count2 = (LPCTSTR)strData086.Trim();
	hoga_item.buy_count2 = (LPCTSTR)strData087.Trim();

	hoga_item.sell_price3 = (LPCTSTR)strData088.Trim();
	hoga_item.buy_price3 = (LPCTSTR)strData089.Trim();
	hoga_item.sell_qty3 = (LPCTSTR)strData090.Trim();
	hoga_item.buy_qty3 = (LPCTSTR)strData091.Trim();
	hoga_item.sell_count3 = (LPCTSTR)strData092.Trim();
	hoga_item.buy_count3 = (LPCTSTR)strData093.Trim();

	hoga_item.sell_price4 = (LPCTSTR)strData094.Trim();
	hoga_item.buy_price4 = (LPCTSTR)strData095.Trim();
	hoga_item.sell_qty4 = (LPCTSTR)strData096.Trim();
	hoga_item.buy_qty4 = (LPCTSTR)strData097.Trim();
	hoga_item.sell_count4 = (LPCTSTR)strData098.Trim();
	hoga_item.buy_count4 = (LPCTSTR)strData099.Trim();

	hoga_item.sell_price5 = (LPCTSTR)strData100.Trim();
	hoga_item.buy_price5 = (LPCTSTR)strData101.Trim();
	hoga_item.sell_qty5 = (LPCTSTR)strData102.Trim();
	hoga_item.buy_qty5 = (LPCTSTR)strData103.Trim();
	hoga_item.sell_count5 = (LPCTSTR)strData104.Trim();
	hoga_item.buy_count5 = (LPCTSTR)strData105.Trim();

	hoga_item.tot_sell_qty = (LPCTSTR)strData106.Trim();
	hoga_item.tot_buy_qty = (LPCTSTR)strData107.Trim();
	hoga_item.tot_sell_count = (LPCTSTR)strData108.Trim();
	hoga_item.tot_buy_count = (LPCTSTR)strData109.Trim();


	
	CString msg;
	msg.Format(_T(" OnFutureHoga symbol_code = %s, time = %s\n"), strSeries, strTime);
	//TRACE(msg);
	SmRealtimeHogaManager::GetInstance()->AddFutTask(std::move(hoga_item));
}

void HdClient::OnFutureHoga(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");



	CString	strData076 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strData079 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strData080 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");



	CString	strData082 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strData085 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strData086 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");


	CString	strData088 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strData091 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strData092 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");


	CString	strData094 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strData097 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strData098 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");


	CString	strData100 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strData103 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strData104 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");

	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	CString	strData106 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strData107 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strData108 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	SmHogaItem hoga_item;
	hoga_item.time = (LPCTSTR)strTime.Trim();
	hoga_item.symbol_code = (LPCTSTR)strSeries.Trim();

	hoga_item.sell_price1 = (LPCTSTR)strData076.Trim();
	hoga_item.buy_price1 = (LPCTSTR)strData077.Trim();
	hoga_item.sell_qty1 = (LPCTSTR)strData078.Trim();
	hoga_item.buy_qty1 = (LPCTSTR)strData079.Trim();
	hoga_item.sell_count1 = (LPCTSTR)strData080.Trim();
	hoga_item.buy_count1 = (LPCTSTR)strData081.Trim();

	hoga_item.sell_price2 = (LPCTSTR)strData082.Trim();
	hoga_item.buy_price2 = (LPCTSTR)strData083.Trim();
	hoga_item.sell_qty2 = (LPCTSTR)strData084.Trim();
	hoga_item.buy_qty2 = (LPCTSTR)strData085.Trim();
	hoga_item.sell_count2 = (LPCTSTR)strData086.Trim();
	hoga_item.buy_count2 = (LPCTSTR)strData087.Trim();

	hoga_item.sell_price3 = (LPCTSTR)strData088.Trim();
	hoga_item.buy_price3 = (LPCTSTR)strData089.Trim();
	hoga_item.sell_qty3 = (LPCTSTR)strData090.Trim();
	hoga_item.buy_qty3 = (LPCTSTR)strData091.Trim();
	hoga_item.sell_count3 = (LPCTSTR)strData092.Trim();
	hoga_item.buy_count3 = (LPCTSTR)strData093.Trim();

	hoga_item.sell_price4 = (LPCTSTR)strData094.Trim();
	hoga_item.buy_price4 = (LPCTSTR)strData095.Trim();
	hoga_item.sell_qty4 = (LPCTSTR)strData096.Trim();
	hoga_item.buy_qty4 = (LPCTSTR)strData097.Trim();
	hoga_item.sell_count4 = (LPCTSTR)strData098.Trim();
	hoga_item.buy_count4 = (LPCTSTR)strData099.Trim();

	hoga_item.sell_price5 = (LPCTSTR)strData100.Trim();
	hoga_item.buy_price5 = (LPCTSTR)strData101.Trim();
	hoga_item.sell_qty5 = (LPCTSTR)strData102.Trim();
	hoga_item.buy_qty5 = (LPCTSTR)strData103.Trim();
	hoga_item.sell_count5 = (LPCTSTR)strData104.Trim();
	hoga_item.buy_count5 = (LPCTSTR)strData105.Trim();

	hoga_item.tot_sell_qty = (LPCTSTR)strData106.Trim();
	hoga_item.tot_buy_qty = (LPCTSTR)strData107.Trim();
	hoga_item.tot_sell_count = (LPCTSTR)strData108.Trim();
	hoga_item.tot_buy_count = (LPCTSTR)strData109.Trim();


	CString msg;
	msg.Format(_T(" OnFutureHoga symbol_code = %s, time = %s\n"), strSeries, strTime);
	//TRACE(msg);
	SmRealtimeHogaManager::GetInstance()->AddFutTask(std::move(hoga_item));
}

void HdClient::OnOptionHoga(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");

	CString	strData076 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strData079 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strData080 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");


	CString	strData082 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strData085 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strData086 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");

	CString	strData088 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strData091 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strData092 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");


	CString	strData094 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strData097 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strData098 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");


	CString	strData100 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strData103 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strData104 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");


	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");
	CString	strData106 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strData107 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strData108 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	SmHogaItem hoga_item;
	hoga_item.time = (LPCTSTR)strTime.Trim();
	hoga_item.symbol_code = (LPCTSTR)strSeries.Trim();

	hoga_item.sell_price1 = (LPCTSTR)strData076.Trim();
	hoga_item.buy_price1 = (LPCTSTR)strData077.Trim();
	hoga_item.sell_qty1 = (LPCTSTR)strData078.Trim();
	hoga_item.buy_qty1 = (LPCTSTR)strData079.Trim();
	hoga_item.sell_count1 = (LPCTSTR)strData080.Trim();
	hoga_item.buy_count1 = (LPCTSTR)strData081.Trim();

	hoga_item.sell_price2 = (LPCTSTR)strData082.Trim();
	hoga_item.buy_price2 = (LPCTSTR)strData083.Trim();
	hoga_item.sell_qty2 = (LPCTSTR)strData084.Trim();
	hoga_item.buy_qty2 = (LPCTSTR)strData085.Trim();
	hoga_item.sell_count2 = (LPCTSTR)strData086.Trim();
	hoga_item.buy_count2 = (LPCTSTR)strData087.Trim();

	hoga_item.sell_price3 = (LPCTSTR)strData088.Trim();
	hoga_item.buy_price3 = (LPCTSTR)strData089.Trim();
	hoga_item.sell_qty3 = (LPCTSTR)strData090.Trim();
	hoga_item.buy_qty3 = (LPCTSTR)strData091.Trim();
	hoga_item.sell_count3 = (LPCTSTR)strData092.Trim();
	hoga_item.buy_count3 = (LPCTSTR)strData093.Trim();

	hoga_item.sell_price4 = (LPCTSTR)strData094.Trim();
	hoga_item.buy_price4 = (LPCTSTR)strData095.Trim();
	hoga_item.sell_qty4 = (LPCTSTR)strData096.Trim();
	hoga_item.buy_qty4 = (LPCTSTR)strData097.Trim();
	hoga_item.sell_count4 = (LPCTSTR)strData098.Trim();
	hoga_item.buy_count4 = (LPCTSTR)strData099.Trim();

	hoga_item.sell_price5 = (LPCTSTR)strData100.Trim();
	hoga_item.buy_price5 = (LPCTSTR)strData101.Trim();
	hoga_item.sell_qty5 = (LPCTSTR)strData102.Trim();
	hoga_item.buy_qty5 = (LPCTSTR)strData103.Trim();
	hoga_item.sell_count5 = (LPCTSTR)strData104.Trim();
	hoga_item.buy_count5 = (LPCTSTR)strData105.Trim();

	hoga_item.tot_sell_qty = (LPCTSTR)strData106.Trim();
	hoga_item.tot_buy_qty = (LPCTSTR)strData107.Trim();
	hoga_item.tot_sell_count = (LPCTSTR)strData108.Trim();
	hoga_item.tot_buy_count = (LPCTSTR)strData109.Trim();

	CString msg;
	msg.Format(_T(" OnFutureHoga symbol_code = %s, time = %s\n"), strSeries, strTime);
	//TRACE(msg);

	SmRealtimeHogaManager::GetInstance()->AddOptTask(std::move(hoga_item));
}

void HdClient::OnProductHoga(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "kfutcode");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	CString	strData076 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho1");
	CString	strData077 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho1");
	CString	strData078 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem1");
	CString	strData079 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem1");
	CString	strData080 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt1");
	CString	strData081 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt1");

	CString	strData082 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho2");
	CString	strData083 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho2");
	CString	strData084 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem2");
	CString	strData085 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem2");
	CString	strData086 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt2");
	CString	strData087 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt2");

	CString	strData088 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho3");
	CString	strData089 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho3");
	CString	strData090 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem3");
	CString	strData091 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem3");
	CString	strData092 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt3");
	CString	strData093 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt3");

	CString	strData094 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho4");
	CString	strData095 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho4");
	CString	strData096 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem4");
	CString	strData097 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem4");
	CString	strData098 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt4");
	CString	strData099 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt4");

	CString	strData100 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho5");
	CString	strData101 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho5");
	CString	strData102 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem5");
	CString	strData103 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem5");
	CString	strData104 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt5");
	CString	strData105 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt5");

	CString	strData106 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totofferrem");
	CString	strData107 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totbidrem");
	CString	strData108 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totoffercnt");
	CString	strData109 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totbidcnt");

	SmHogaItem hoga_item;
	hoga_item.time = (LPCTSTR)strTime.Trim();
	hoga_item.symbol_code = (LPCTSTR)strSeries.Trim();

	hoga_item.sell_price1 = (LPCTSTR)strData076.Trim();
	hoga_item.buy_price1 = (LPCTSTR)strData077.Trim();
	hoga_item.sell_qty1 = (LPCTSTR)strData078.Trim();
	hoga_item.buy_qty1 = (LPCTSTR)strData079.Trim();
	hoga_item.sell_count1 = (LPCTSTR)strData080.Trim();
	hoga_item.buy_count1 = (LPCTSTR)strData081.Trim();

	hoga_item.sell_price2 = (LPCTSTR)strData082.Trim();
	hoga_item.buy_price2 = (LPCTSTR)strData083.Trim();
	hoga_item.sell_qty2 = (LPCTSTR)strData084.Trim();
	hoga_item.buy_qty2 = (LPCTSTR)strData085.Trim();
	hoga_item.sell_count2 = (LPCTSTR)strData086.Trim();
	hoga_item.buy_count2 = (LPCTSTR)strData087.Trim();

	hoga_item.sell_price3 = (LPCTSTR)strData088.Trim();
	hoga_item.buy_price3 = (LPCTSTR)strData089.Trim();
	hoga_item.sell_qty3 = (LPCTSTR)strData090.Trim();
	hoga_item.buy_qty3 = (LPCTSTR)strData091.Trim();
	hoga_item.sell_count3 = (LPCTSTR)strData092.Trim();
	hoga_item.buy_count3 = (LPCTSTR)strData093.Trim();

	hoga_item.sell_price4 = (LPCTSTR)strData094.Trim();
	hoga_item.buy_price4 = (LPCTSTR)strData095.Trim();
	hoga_item.sell_qty4 = (LPCTSTR)strData096.Trim();
	hoga_item.buy_qty4 = (LPCTSTR)strData097.Trim();
	hoga_item.sell_count4 = (LPCTSTR)strData098.Trim();
	hoga_item.buy_count4 = (LPCTSTR)strData099.Trim();

	hoga_item.sell_price5 = (LPCTSTR)strData100.Trim();
	hoga_item.buy_price5 = (LPCTSTR)strData101.Trim();
	hoga_item.sell_qty5 = (LPCTSTR)strData102.Trim();
	hoga_item.buy_qty5 = (LPCTSTR)strData103.Trim();
	hoga_item.sell_count5 = (LPCTSTR)strData104.Trim();
	hoga_item.buy_count5 = (LPCTSTR)strData105.Trim();

	hoga_item.tot_sell_qty = (LPCTSTR)strData106.Trim();
	hoga_item.tot_buy_qty = (LPCTSTR)strData107.Trim();
	hoga_item.tot_sell_count = (LPCTSTR)strData108.Trim();
	hoga_item.tot_buy_count = (LPCTSTR)strData109.Trim();

	CString msg;
	msg.Format(_T(" OnFutureHoga symbol_code = %s, time = %s\n"), strSeries, strTime);
	//TRACE(msg);	

	SmRealtimeHogaManager::GetInstance()->AddProTask(std::move(hoga_item));
}

void HdClient::OnRealFutureQuote(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");

	auto cur_time = VtStringUtil::GetCurrentDateTime();

	std::string sys_time = cur_time.second;


	//OutputDebugString(msg);


	CString	strData051 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString	strData053 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString	strData054 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");
	CString strCom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");
	CString strUpRate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "등락율");
	CString strAccVol = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "누적거래량");

	CString msg;
	msg.Format(_T(" OnRealFutureQuote code = %s, system_time = %s, \n 현재가 = %s\n"), strSeries, sys_time.c_str(), strData051);
	//TRACE(msg);


	SmQuoteData quote;

	quote.symbol_code = (LPCTSTR)strSeries.TrimRight();
	quote.command = 1;
	quote.time = (LPCTSTR)strTime.Trim();
	quote.volume = (LPCTSTR)strVolume.Trim();
	quote.up_down = (LPCTSTR)strUpdown.Trim();
	quote.sign = (LPCTSTR)strUpdown.Trim();
	quote.up_down_rate = (LPCTSTR)strUpRate.Trim();
	quote.to_preday = (LPCTSTR)strCom.Trim();
	quote.close = (LPCTSTR)strData051;
	quote.open = (LPCTSTR)strData052;
	quote.high = (LPCTSTR)strData053;
	quote.low = (LPCTSTR)strData054;
	quote.acc_vol = (LPCTSTR)strAccVol;

	// 쓰레드 큐에 넣는다.
	SmRealtimeQuoteManager::GetInstance()->AddFutTask(std::move(quote));
}

void HdClient::OnRealOptionQuote(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");

	CString strUpRate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "등락율");
	CString strCom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");

	CString	strData051 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString	strData053 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString	strData054 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");

	SmQuoteData quote;

	quote.symbol_code = (LPCTSTR)strSeries.TrimRight();
	quote.command = 1;
	quote.time = (LPCTSTR)strTime.Trim();
	quote.volume = (LPCTSTR)strVolume.Trim();
	quote.up_down = (LPCTSTR)strUpdown.Trim();
	quote.sign = (LPCTSTR)strUpdown.Trim();
	quote.up_down_rate = (LPCTSTR)strUpRate.Trim();
	quote.to_preday = (LPCTSTR)strCom.Trim();
	quote.close = (LPCTSTR)strData051;
	quote.open = (LPCTSTR)strData052;
	quote.high = (LPCTSTR)strData053;
	quote.low = (LPCTSTR)strData054;

	// 쓰레드 큐에 넣는다.
	SmRealtimeQuoteManager::GetInstance()->AddOptTask(std::move(quote));
}

void HdClient::OnRealProductQuote(CString& strKey, LONG& nRealType)
{
	CHDFCommAgent& m_CommAgent = _HdCtrl->GetHdAgent();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "cvolume");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "cgubun");

	CString	strData051 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "open");
	CString	strData053 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "high");
	CString	strData054 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "low");

	CString strUpRate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "등락율");
	CString strCom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");

	SmQuoteData quote;

	quote.symbol_code = (LPCTSTR)strSeries.TrimRight();
	quote.command = 1;
	quote.time = (LPCTSTR)strTime.Trim();
	quote.volume = (LPCTSTR)strVolume.Trim();
	quote.up_down = (LPCTSTR)strUpdown.Trim();
	quote.sign = (LPCTSTR)strUpdown.Trim();
	quote.up_down_rate = (LPCTSTR)strUpRate.Trim();
	quote.to_preday = (LPCTSTR)strCom.Trim();
	quote.close = (LPCTSTR)strData051;
	quote.open = (LPCTSTR)strData052;
	quote.high = (LPCTSTR)strData053;
	quote.low = (LPCTSTR)strData054;

	// 쓰레드 큐에 넣는다.
	SmRealtimeQuoteManager::GetInstance()->AddProTask(std::move(quote));
}

