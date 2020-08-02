// VtHdCtrl.cpp : implementation file
//

#include "pch.h"
#include "afxwin.h"
#include "SmHdCtrl.h"
#include "afxdialogex.h"
#include "../Log/loguru.hpp"
#include "../Util/VtStringUtil.h"
#include "SmHdClient.h"
#include "../resource.h"
#include <ctime>
#include "../Symbol/SmSymbol.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Database/SmTimeSeriesDBManager.h"
#include "../Chart/SmTimeSeriesCollector.h"
#include "../Json/json.hpp"
#include "../Symbol/SmQuoteDefine.h"
#include "../Symbol/SmHogaDefine.h"
#include "../Chart/SmChartDataManager.h"
#include "../Chart/SmChartData.h"
#include "../Service/SmTimeSeriesServiceManager.h"
#include "../Symbol/SmSymbolManager.h"
#include "../Ai/SmAIIndicatorManager.h"
using namespace nlohmann;
// VtHdCtrl dialog

IMPLEMENT_DYNAMIC(SmHdCtrl, CDialogEx)

SmHdCtrl::SmHdCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HD_CTRL, pParent)
{

}

SmHdCtrl::~SmHdCtrl()
{
	if (m_CommAgent.GetSafeHwnd()) {
		if (m_CommAgent.CommGetConnectState() == 1) {
			if (m_sUserId != "")
				m_CommAgent.CommLogout(m_sUserId);
		}

		m_CommAgent.CommTerminate(TRUE);
		m_CommAgent.DestroyWindow();
	}
}

void SmHdCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

int SmHdCtrl::Init()
{
	if (!m_CommAgent.GetSafeHwnd()) {
		m_CommAgent.Create("HDF CommAgent", WS_CHILD, CRect(0, 0, 0, 0), this, 2286);
	}
	else
		return -1000;

	if (m_CommAgent.GetSafeHwnd()) {
		int nRet = m_CommAgent.CommInit(1);

		if (nRet < 0) {
			//AfxMessageBox("통신프로그램 실행 오류");
			LOG_F(INFO, _T("통신프로그램 실행 오류"));
		}
		else {
			//AfxMessageBox("통신프로그램 실행 성공");
			LOG_F(INFO, _T("통신프로그램 실행 성공"));
		}

		return nRet;
	}

	return -1000;
}

int SmHdCtrl::LogIn(CString id, CString pwd, CString cert)
{
	m_sUserId = id;
	m_sUserPw = pwd;
	m_sAuthPw = cert;
	const char* pLoginSuccess[] = { "로그인 성공"	, "Login Successful" };
	const char* pLoginFail[] = { "로그인 실패"	, "Login Failure" };

	int nRet = m_CommAgent.CommLogin(m_sUserId, m_sUserPw, m_sAuthPw);
	if (nRet > 0) {
		//AfxMessageBox(pLoginSuccess[0]);

		//로긴후 반드시 호출...
		m_CommAgent.CommAccInfo();
		LOG_F(INFO, pLoginSuccess[0]);
	}
	else
	{
		CString strRet;
		strRet.Format("[%d]", nRet);
		//AfxMessageBox(pLoginFail[0] + strRet);
		LOG_F(INFO, _T("%s"), strRet);
	}

	return nRet;
}

void SmHdCtrl::LogIn()
{
	int nRet = m_CommAgent.CommLogin(m_sUserId, m_sUserPw, m_sAuthPw);
	if (nRet > 0)
	{
		//AfxMessageBox("로그인 성공");

		// 로긴후 반드시 호출...
		m_CommAgent.CommAccInfo();
		//m_CommAgent.CommReqMakeCod("all", 0);

		//GetAccountInfo();
	}
	else
	{
		CString strRet;
		strRet.Format("[%d]", nRet);
		AfxMessageBox("로그인 실패" + strRet);
	}
}

int SmHdCtrl::LogOut(CString id)
{
	return m_CommAgent.CommLogout(id);
}

void SmHdCtrl::LogOut()
{
	// 로그아웃한다.
	int nRet = m_CommAgent.CommLogout(m_sUserId);

	CString strRet;
	strRet.Format("[%d]", nRet);
	if (nRet < 0) {
		AfxMessageBox("로그아웃 실패" + strRet);
	}
	else {
		AfxMessageBox("로그아웃 성공" + strRet);
	}
}

void SmHdCtrl::RegisterProduct(std::string symCode)
{
	std::string key = VtStringUtil::PadRight(symCode, ' ', 32);
	int nRealType = 76;
	m_CommAgent.CommSetBroad(key.c_str(), nRealType);
	nRealType = 82;
	m_CommAgent.CommSetBroad(key.c_str(), nRealType);
}

void SmHdCtrl::UnregisterProduct(std::string symCode)
{
	int nRealType = 76;
	m_CommAgent.CommRemoveBroad(symCode.c_str(), nRealType);
	nRealType = 82;
	m_CommAgent.CommRemoveBroad(symCode.c_str(), nRealType);
}



void SmHdCtrl::GetChartData(SmChartDataRequest req)
{
	LOG_F(INFO, "GetChartData symbol Code: %s", req.symbolCode.c_str());
	LOG_F(INFO, "GetChartData : %s", req.GetDataKey().c_str());

	if (req.GetDataKey().length() < 8)
		return;
	if (std::isdigit(req.symbolCode.at(2))) {
		if (req.symbolCode.length() < 8)
			return;
		std::string prefix = req.symbolCode.substr(0, 3);
		GetChartDataForDomestic(req);
	}
	else {

		if (req.symbolCode.length() < 4)
			return;
		// 심볼 테이블에 있는 것만 요청을 한다.
		std::shared_ptr<SmSymbol> sym = SmSymbolManager::GetInstance()->FindSymbol(req.symbolCode);
		if (!sym)
			return;

		if (req.chartType == SmChartType::TICK)
			GetChartDataLongCycle(req);
		else if (req.chartType == SmChartType::MIN)
			GetChartDataShortCycle(req);
		else if (req.chartType == SmChartType::DAY)
			GetChartDataLongCycle(req);
		else if (req.chartType == SmChartType::WEEK)
			GetChartDataLongCycle(req);
		else if (req.chartType == SmChartType::MON)
			GetChartDataLongCycle(req);
		else
			GetChartDataShortCycle(req);
	}
}



void SmHdCtrl::GetChartDataShortCycle(SmChartDataRequest req)
{
	std::string temp;
	std::string reqString;
	// 종목 코드 32 자리
	temp = VtStringUtil::PadRight(req.symbolCode, ' ', 32);
	reqString.append(temp);

	std::string str = VtStringUtil::getCurentDate();
	CString msg;
	msg.Format("%s \n", str.c_str());
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

	CString sTrCode = "o51200";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	//int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), "");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";
	int nRqID = m_CommAgent.CommFIDRqData(sTrCode, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	
	//TRACE(sInput);
	_ChartDataReqMap[nRqID] = req;
}

void SmHdCtrl::GetChartDataLongCycle(SmChartDataRequest req)
{
	std::string temp;
	std::string reqString;
	// 최초 요청시 18자리 공백
	reqString.append("                  ");

	temp = VtStringUtil::PadRight(req.symbolCode, ' ', 32);
	reqString.append(temp);

	time_t rawtime;
	struct tm* timeinfo = nullptr;
	char buffer[80];

	time(&rawtime);
	localtime_s(timeinfo, &rawtime);

	/*
	struct tm base_date_local;
	char buf[100];

	localtime_s(&base_date_local, &base_time);//초 단위 값을 지역 시각(DateTime)을 구한다.
	*/

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


	CString sTrCode = "o44005";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	//int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), "");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";
	//CString strNextKey = m_CommAgent.CommGetNextKey(nRqID, "");
	//int nRqID = m_CommAgent.CommFIDRqData(sTrCode, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	//TRACE(sInput);
	_ChartDataReqMap[nRqID] = req;
}

void SmHdCtrl::GetChartDataForDomestic(SmChartDataRequest req)
{
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
	LOG_F(INFO, "GetChartDataDomestic %s", reqString.c_str());
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), "");
	_ChartDataReqMap[nRqID] = req;
}

void SmHdCtrl::OnRcvdDomesticChartData(CString& sTrCode, LONG& nRqID)
{
	try {
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
		//influxdb_cpp::server_info si("127.0.0.1", 8086, "abroad_future", "angelpie", "orion1");
		//influxdb_cpp::server_info si("127.0.0.1", 8086, "test_x", "test", "test");
		CString msg;

		auto it = _ChartDataReqMap.find(nRqID);
		if (it == _ChartDataReqMap.end())
			return;
		SmChartDataRequest req = it->second;
		SmTimeSeriesCollector* tsCol = SmTimeSeriesCollector::GetInstance();
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		// 차트 데이터 목록에 추가한다.
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
		int total_count = nRepeatCnt;
		int current_count = 1;
		std::vector<SmChartDataItem> chart_vec;
		// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
		// Received the chart data first.
		for (int i = nRepeatCnt - 1; i >= 0; --i) {
			CString strDate = "";
			CString strTime = "";

			CString tempDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "날짜시간");

			if (chart_data->ChartType() == SmChartType::MIN)
				tempDate.Append(_T("00"));
			else
				tempDate.Append(_T("000000"));


			strTime = tempDate.Right(6);
			strDate = tempDate.Left(8);

			CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시가");
			CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "고가");
			CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "저가");
			CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
			CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "거래량");

			// 차트 데이터가 중간에 이상이 있으면 나머지는 전송을 하지 않는다.
			if (strDate.GetLength() == 0) {
				chart_data->Received(true);
				SmAIIndicatorManager::GetInstance()->OnReceivedChartData(chart_data);
				break;
			}

			//msg.Format(_T("OnRcvdAbroadChartData :: index = %d, date = %s, t = %s, o = %s, h = %s, l = %s, c = %s, v = %s\n"), i, strDate, strTime, strOpen, strHigh, strLow, strClose, strVol);
			//TRACE(msg);

			SmChartDataItem data;
			data.symbolCode = req.symbolCode;
			data.chartType = req.chartType;
			data.cycle = req.cycle;
			data.date = strDate.Trim();
			data.time = strTime.Trim();
			data.date_time = data.date + data.time;
			data.h = _ttoi(strHigh);
			data.l = _ttoi(strLow);
			data.o = _ttoi(strOpen);
			data.c = _ttoi(strClose);
			data.v = _ttoi(strVol);
			data.total_count = total_count;
			data.current_count = current_count;
			//chart_data->PushChartDataItemToBack(data);


			char buffer[4096];
			sprintf(buffer, "SendChartDataOnebyOne%s : %s\n", data.date.c_str(), data.time.c_str());
			OutputDebugString(buffer);
			//LOG_F(INFO, "OnReqResendCharrtDataOneByOne %s", data.GetDataKey().c_str());

			// 차트데이터에 추가한다.
			std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(data);
			chart_data->AddData(data);

			// 전체 갯수와 현재 갯수가 일치하면 차트 데이터를 받았다고 표시한다.
			if (total_count == current_count) {
				chart_data->Received(true);
				SmAIIndicatorManager::GetInstance()->OnReceivedChartData(chart_data);
			}
			current_count++;

			// 차트데이터는 바로 보낸다.
			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			tsMgr->SendChartData(req.session_id, data);
		}

		LOG_F(INFO, "OnRcvdDomesticChartData %s", req.GetDataKey().c_str());

		// 아직 처리되지 못한 데이터는 큐를 통해서 처리한다.
		RequestChartDataFromQ();

		// 차트 데이터 수신 요청 목록에서 제거한다.
		_ChartDataReqMap.erase(it);
	}
	catch (std::exception e)
	{
		std::string error = e.what();
		LOG_F(INFO, "%s", error);
	}
}

void SmHdCtrl::RequestChartDataFromQ()
{
	if (!_ChartDataReqQueue.empty()) {
		//Sleep(700);
		GetChartData(_ChartDataReqQueue.front());
		_ChartDataReqQueue.pop();
	}
}

void SmHdCtrl::DownloadMasterFiles(std::string param)
{
	m_CommAgent.CommReqMakeCod(param.c_str(), 0);
}

void SmHdCtrl::GetSiseData(std::string symCode)
{
	CString sFidCode = "o51000";
	std::string sym_code = VtStringUtil::PadRight(symCode, ' ', 32);
	CString sInput = sym_code.c_str();
	CString sReqFidInput = "000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037";
	CString strNextKey = "";
	int nRqID = m_CommAgent.CommFIDRqData(sFidCode, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	_SiseDataReqMap[nRqID] = symCode;
}

void SmHdCtrl::GetHogaData(std::string symCode)
{
	CString sFidCode = "o51010";
	std::string sym_code = VtStringUtil::PadRight(symCode, ' ', 32);
	CString sInput = sym_code.c_str();
	CString sReqFidInput = "000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046";
	CString strNextKey = "";
	int nRqID = m_CommAgent.CommFIDRqData(sFidCode, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	_SiseDataReqMap[nRqID] = symCode;
}

void SmHdCtrl::RequestChartData(SmChartDataRequest req)
{
	std::lock_guard<std::mutex> lock(_mutex);

	LOG_F(INFO, "RequestChartData %s", req.GetDataKey().c_str());
	_ChartDataReqQueue.push(req);

// 	if (_ChartDataReqQueue.size() > 2) {
// 		while (!_ChartDataReqQueue.empty()) _ChartDataReqQueue.pop();
// 		return;
// 	}
// 	else 
//	if (_ChartDataReqQueue.size() > 1)
//		return;

	GetChartData(_ChartDataReqQueue.front());
}

void SmHdCtrl::OnRcvdAbroadHoga(CString& strKey, LONG& nRealType)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	CString strSymCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol((LPCTSTR)strSymCode.Trim());
	if (!sym)
		return;
	CString strHogaTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	HdHoga hoga;
	hoga.Items[0].strBuyHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	hoga.Items[0].strSellHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	hoga.Items[0].strBuyHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	hoga.Items[0].strSellHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	hoga.Items[0].strBuyHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");
	hoga.Items[0].strSellHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");

	hoga.Items[1].strBuyHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	hoga.Items[1].strSellHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	hoga.Items[1].strBuyHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	hoga.Items[1].strSellHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	hoga.Items[1].strBuyHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");
	hoga.Items[1].strSellHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");

	hoga.Items[2].strBuyHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	hoga.Items[2].strSellHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	hoga.Items[2].strBuyHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	hoga.Items[2].strSellHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	hoga.Items[2].strBuyHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");
	hoga.Items[2].strSellHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");

	hoga.Items[3].strBuyHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	hoga.Items[3].strSellHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	hoga.Items[3].strBuyHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	hoga.Items[3].strSellHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	hoga.Items[3].strBuyHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");
	hoga.Items[3].strSellHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");

	hoga.Items[4].strBuyHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	hoga.Items[4].strSellHoga = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	hoga.Items[4].strBuyHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	hoga.Items[4].strSellHogaQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	hoga.Items[4].strBuyHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");
	hoga.Items[4].strSellHogaCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");

	CString strTotBuyQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString strTotSellQty = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString strTotBuyCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");
	CString strTotSellCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString strDomDate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "국내일자");
	CString strDomTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "국내시간");
	SmHoga hoga_data;
	for (int i = 0; i < 5; i++) {
		hoga_data.Ary[i].BuyPrice = sym->Hoga.Ary[i].BuyPrice = _ttoi(hoga.Items[i].strBuyHoga);
		hoga_data.Ary[i].BuyCnt = sym->Hoga.Ary[i].BuyCnt = _ttoi(hoga.Items[i].strBuyHogaCnt);
		hoga_data.Ary[i].BuyQty = sym->Hoga.Ary[i].BuyQty = _ttoi(hoga.Items[i].strBuyHogaQty);
		hoga_data.Ary[i].SellPrice = sym->Hoga.Ary[i].SellPrice = _ttoi(hoga.Items[i].strSellHoga);
		hoga_data.Ary[i].SellCnt = sym->Hoga.Ary[i].SellCnt = _ttoi(hoga.Items[i].strSellHogaCnt);
		hoga_data.Ary[i].SellQty = sym->Hoga.Ary[i].SellQty = _ttoi(hoga.Items[i].strSellHogaQty);
	}

	hoga_data.DomesticDate = sym->Hoga.DomesticDate = strDomDate;
	hoga_data.DomesticTime =  sym->Hoga.DomesticTime = strDomTime;
	hoga_data.Time = sym->Hoga.Time = strHogaTime;
	hoga_data.TotBuyCnt = sym->Hoga.TotBuyCnt = _ttoi(strTotBuyCnt);
	hoga_data.TotBuyQty = sym->Hoga.TotBuyQty = _ttoi(strTotBuyQty);
	hoga_data.TotSellCnt = sym->Hoga.TotSellCnt = _ttoi(strTotSellCnt);
	hoga_data.TotSellQty = sym->Hoga.TotSellQty = _ttoi(strTotSellQty);

	//TRACE(sym->Hoga.Time.c_str());

	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	//dbMgr->SaveHogaItem(std::move(hoga_data));

	CString msg;
	msg.Format(_T("hoga :: time = %s, tot_buy_cnt = %d\n"), sym->Hoga.SymbolCode.c_str(), sym->Hoga.TotBuyCnt);
	//TRACE(msg);
}

void SmHdCtrl::OnRcvdAbroadSise(CString& strKey, LONG& nRealType)
{
	CString strSymCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strPrev = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "직전대비구분");
	CString strSignToPreDay = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비구분");
	CString strToPreDay = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");
	CString strRatioToPreDay = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비등락율");
	CString strClose = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가");
	CString strOpen = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString strHigh = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString strLow = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strSign = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");

	SmQuote quoteItem;
	quoteItem.SymbolCode = strSymCode.Trim();
	quoteItem.OriginTime = strTime;
	quoteItem.SignToPreDay = strSignToPreDay.Trim();
	quoteItem.GapFromPreDay = _ttoi(strToPreDay);
	quoteItem.RatioToPreday = strRatioToPreDay.Trim();
	quoteItem.Close = _ttoi(strClose);
	quoteItem.Open = _ttoi(strOpen);
	quoteItem.High = _ttoi(strHigh);
	quoteItem.Low = _ttoi(strLow);
	quoteItem.Volume= _ttoi(strVolume);
	quoteItem.Sign = strSign.Trim();


	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	//dbMgr->SaveQuoteItem(std::move(quoteItem));
	//dbMgr->SaveCurrentQuoteItem(std::move(quoteItem));


	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol((LPCTSTR)strSymCode.Trim());
	if (!sym)
		return;
	sym->Quote.Close = _ttoi(strClose);
	sym->Quote.Open = _ttoi(strOpen);
	sym->Quote.High = _ttoi(strHigh);
	sym->Quote.Low = _ttoi(strLow);
	sym->Quote.OriginTime = strTime;
	sym->Quote.GapFromPreDay = _ttoi(strToPreDay);
	sym->Quote.RatioToPreday = strRatioToPreDay.Trim();
	sym->Quote.SignToPreDay = strSignToPreDay;

	CString msg;
	msg.Format(_T("symbol = %s, time = %s, h=%s, l=%s, o=%s, c=%s, v=%s, ratio = %s\n"), strSymCode, strTime, strHigh, strLow, strOpen, strClose, strVolume, strRatioToPreDay);
	//TRACE(msg);
}

void SmHdCtrl::OnRcvdAbroadSiseByReq(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strData1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "한글종목명");
		CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "현재가");
		CString msg;
		msg.Format(_T("code = %s, name = %s, close = %s\n"), strData1, strData2, strData3);
		TRACE(msg);

		CString strSymCode = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "종목코드");
		CString strTime = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "국내시간");
		CString strPrev = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "직전대비구분");
		CString strSignToPreDay = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "전일대비구분");
		CString strToPreDay = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "전일대비");
		CString strRatioToPreDay = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "전일대비등락율");
		CString strClose = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "현재가");
		CString strOpen = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, nRqID, "OutRec1", 0, "저가");

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol((LPCTSTR)strSymCode.Trim());
		if (!sym)
			continue;
		sym->Quote.Close = _ttoi(strClose);
		sym->Quote.Open = _ttoi(strOpen);
		sym->Quote.High = _ttoi(strHigh);
		sym->Quote.Low = _ttoi(strLow);
		sym->Quote.OriginTime = strTime;
		sym->Quote.GapFromPreDay = _ttoi(strToPreDay);
		sym->Quote.RatioToPreday = strRatioToPreDay.Trim();
		sym->Quote.SignToPreDay = strSignToPreDay;

		SmQuote quoteItem;
		quoteItem.SymbolCode = strSymCode.Trim();
		quoteItem.OriginTime = strTime;
		quoteItem.SignToPreDay = strSignToPreDay.Trim();
		quoteItem.GapFromPreDay = _ttoi(strToPreDay);
		quoteItem.RatioToPreday = strRatioToPreDay.Trim();
		quoteItem.Close = _ttoi(strClose);
		quoteItem.Open = _ttoi(strOpen);
		quoteItem.High = _ttoi(strHigh);
		quoteItem.Low = _ttoi(strLow);
		quoteItem.Volume = 0;
		quoteItem.Sign = "";

		//CString msg;
		msg.Format(_T("symbol = %s, time = %s, h=%s, l=%s, o=%s, c=%s, ratio = %s\n"), strSymCode, strTime, strHigh, strLow, strOpen, strClose, strRatioToPreDay);
		TRACE(msg);


		SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
		dbMgr->SaveCurrentQuoteItem(std::move(quoteItem));
	}
}

void SmHdCtrl::OnRcvdAbroadHogaByReq(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		CString strSymCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol((LPCTSTR)strSymCode.Trim());
		if (!sym)
			continue;
		CString strHogaTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "호가시간");

		HdHoga hoga;
		hoga.strSymbolCode = strSymCode;
		hoga.Items[0].strBuyHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가1");
		hoga.Items[0].strSellHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가1");
		hoga.Items[0].strBuyHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가수량1");
		hoga.Items[0].strSellHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가수량1");
		hoga.Items[0].strBuyHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가건수1");
		hoga.Items[0].strSellHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가건수1");

		hoga.Items[1].strBuyHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가2");
		hoga.Items[1].strSellHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가2");
		hoga.Items[1].strBuyHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가수량2");
		hoga.Items[1].strSellHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가수량2");
		hoga.Items[1].strBuyHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가건수2");
		hoga.Items[1].strSellHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가건수2");

		hoga.Items[2].strBuyHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가3");
		hoga.Items[2].strSellHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가3");
		hoga.Items[2].strBuyHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가수량3");
		hoga.Items[2].strSellHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가수량3");
		hoga.Items[2].strBuyHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가건수3");
		hoga.Items[2].strSellHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가건수3");

		hoga.Items[3].strBuyHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가4");
		hoga.Items[3].strSellHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가4");
		hoga.Items[3].strBuyHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가수량4");
		hoga.Items[3].strSellHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가수량4");
		hoga.Items[3].strBuyHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가건수4");
		hoga.Items[3].strSellHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가건수4");

		hoga.Items[4].strBuyHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가5");
		hoga.Items[4].strSellHoga = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가5");
		hoga.Items[4].strBuyHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가수량5");
		hoga.Items[4].strSellHogaQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가수량5");
		hoga.Items[4].strBuyHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수호가건수5");
		hoga.Items[4].strSellHogaCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도호가건수5");

		CString strTotBuyQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "총매수호가잔량");
		CString strTotSellQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "총매도호가잔량");
		CString strTotBuyCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "총매수호가건수");
		CString strTotSellCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "총매도호가건수");
		
		SmHoga hoga_data;
		for (int i = 0; i < 5; i++) {
			hoga_data.Ary[i].BuyPrice = sym->Hoga.Ary[i].BuyPrice = _ttoi(hoga.Items[i].strBuyHoga);
			hoga_data.Ary[i].BuyCnt = sym->Hoga.Ary[i].BuyCnt = _ttoi(hoga.Items[i].strBuyHogaCnt);
			hoga_data.Ary[i].BuyQty = sym->Hoga.Ary[i].BuyQty = _ttoi(hoga.Items[i].strBuyHogaQty);
			hoga_data.Ary[i].SellPrice = sym->Hoga.Ary[i].SellPrice = _ttoi(hoga.Items[i].strSellHoga);
			hoga_data.Ary[i].SellCnt = sym->Hoga.Ary[i].SellCnt = _ttoi(hoga.Items[i].strSellHogaCnt);
			hoga_data.Ary[i].SellQty = sym->Hoga.Ary[i].SellQty = _ttoi(hoga.Items[i].strSellHogaQty);
		}

		hoga_data.Time = sym->Hoga.Time = strHogaTime;
		hoga_data.TotBuyCnt = sym->Hoga.TotBuyCnt = _ttoi(strTotBuyCnt);
		hoga_data.TotBuyQty = sym->Hoga.TotBuyQty = _ttoi(strTotBuyQty);
		hoga_data.TotSellCnt = sym->Hoga.TotSellCnt = _ttoi(strTotSellCnt);
		hoga_data.TotSellQty = sym->Hoga.TotSellQty = _ttoi(strTotSellQty);

		SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
		dbMgr->SaveHogaItem(std::move(hoga_data));

		CString msg;
		msg.Format(_T("hoga :: time = %s, tot_buy_cnt = %d\n"), sym->Hoga.SymbolCode.c_str(), sym->Hoga.TotBuyCnt);
		//TRACE(msg);
	}
}

void SmHdCtrl::OnRcvdAbroadChartData(CString& sTrCode, LONG& nRqID)
{
	try {
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
		//influxdb_cpp::server_info si("127.0.0.1", 8086, "abroad_future", "angelpie", "orion1");
		//influxdb_cpp::server_info si("127.0.0.1", 8086, "test_x", "test", "test");
		CString msg;

		auto it = _ChartDataReqMap.find(nRqID);
		if (it == _ChartDataReqMap.end())
			return;
		SmChartDataRequest req = it->second;
		SmTimeSeriesCollector* tsCol = SmTimeSeriesCollector::GetInstance();
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
		// 가장 최근것이 가장 먼저 온다. 따라서 가장 과거의 데이터를 먼저 가져온다.
		// Received the chart data first.
		int total_count = nRepeatCnt;
		int current_count = 1;
		for (int i = nRepeatCnt - 1; i >= 0; --i) {
			CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내일자");
			CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내시간");
			CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "시가");
			CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "고가");
			CString strLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "저가");
			CString strClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종가");
			CString strVol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결량");

			// 차트 데이터가 중간에 이상이 있으면 나머지는 전송을 하지 않는다.
			if (strDate.GetLength() == 0) {
				chart_data->Received(true);
				SmAIIndicatorManager::GetInstance()->OnReceivedChartData(chart_data);
				break;
			}

			//msg.Format(_T("OnRcvdAbroadChartData :: index = %d, date = %s, t = %s, o = %s, h = %s, l = %s, c = %s, v = %s\n"), i, strDate, strTime, strOpen, strHigh, strLow, strClose, strVol);
			//TRACE(msg);

			SmChartDataItem data;
			data.symbolCode = req.symbolCode;
			data.chartType = req.chartType;
			data.cycle = req.cycle;
			data.date = strDate.Trim();
			data.time = strTime.Trim();
			data.date_time = data.date + data.time;
			data.h = _ttoi(strHigh);
			data.l = _ttoi(strLow);
			data.o = _ttoi(strOpen);
			data.c = _ttoi(strClose);
			data.v = _ttoi(strVol);
			data.total_count = total_count;
			data.current_count = current_count;
			//chart_data->PushChartDataItemToBack(data);


			char buffer[4096];
			sprintf(buffer, "SendChartDataOnebyOne%s : %s\n", data.date.c_str(), data.time.c_str());
			OutputDebugString(buffer);
			LOG_F(INFO, "OnReqResendCharrtDataOneByOne %s", data.GetDataKey().c_str());

			// 차트데이터에 추가한다.
			std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(data);
			chart_data->AddData(data);

			// 전체 갯수와 현재 갯수가 일치하면 차트 데이터를 받았다고 표시한다.
			if (total_count == current_count) {
				chart_data->Received(true);
				SmAIIndicatorManager::GetInstance()->OnReceivedChartData(chart_data);
			}
			current_count++;

			// 차트데이터는 바로 보낸다.
			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			tsMgr->SendChartData(req.session_id, data);
		}

		// 차트 데이터 수신 요청 목록에서 제거한다.
		_ChartDataReqMap.erase(it);

		// 아직 처리되지 못한 데이터는 큐를 통해서 처리한다.
		RequestChartDataFromQ();
	}
	catch (std::exception exception) {
		std::string err = exception.what();
	}
}


void SmHdCtrl::OnRcvdAbroadChartData2(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
	//influxdb_cpp::server_info si("127.0.0.1", 8086, "abroad_future", "angelpie", "orion1");
	//influxdb_cpp::server_info si("127.0.0.1", 8086, "test_x", "test", "test");
	CString msg;

	auto it = _ChartDataReqMap.find(nRqID);
	if (it == _ChartDataReqMap.end())
		return;
	SmChartDataRequest req = it->second;
	SmTimeSeriesCollector* tsCol = SmTimeSeriesCollector::GetInstance();
	SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
	std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(req);
	int total_count = nRepeatCnt;
	int current_count = 1;
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

		
		// 차트 데이터가 중간에 이상이 있으면 나머지는 전송을 하지 않는다.
		if (strDate.GetLength() == 0) {
			chart_data->Received(true);
			SmAIIndicatorManager::GetInstance()->OnReceivedChartData(chart_data);
			break;
		}

		//msg.Format(_T("OnRcvdAbroadChartData :: index = %d, date = %s, t = %s, o = %s, h = %s, l = %s, c = %s, v = %s\n"), i, strDate, strTime, strOpen, strHigh, strLow, strClose, strVol);
		//TRACE(msg);

		SmChartDataItem data;
		data.symbolCode = req.symbolCode;
		data.chartType = req.chartType;
		data.cycle = req.cycle;
		data.date = strDate.Trim();
		data.time = strTime.Trim();
		data.date_time = data.date + data.time;
		data.h = _ttoi(strHigh);
		data.l = _ttoi(strLow);
		data.o = _ttoi(strOpen);
		data.c = _ttoi(strClose);
		data.v = _ttoi(strVol);
		data.total_count = total_count;
		data.current_count = current_count;
		// 차트데이터에 데이터를 쌓는다.
		//chart_data->PushChartDataItemToBack(data);


		char buffer[4096];
		sprintf(buffer, "SendChartDataOnebyOne%s : %s\n", data.date.c_str(), data.time.c_str());
		OutputDebugString(buffer);
		LOG_F(INFO, "OnReqResendCharrtDataOneByOne %s", data.GetDataKey().c_str());

		// 차트데이터에 추가한다.
		std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(data);
		chart_data->AddData(data);

		// 전체 갯수와 현재 갯수가 일치하면 차트 데이터를 받았다고 표시한다.
		if (total_count == current_count) {
			chart_data->Received(true);
			SmAIIndicatorManager::GetInstance()->OnReceivedChartData(chart_data);
		}
		current_count++;

		// 차트데이터는 바로 보낸다.
		SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
		tsMgr->SendChartData(req.session_id, data);
	}

	// 차트 데이터 수신 요청 목록에서 제거한다.
	_ChartDataReqMap.erase(it);
	// 아직 처리되지 못한 데이터는 큐를 통해서 처리한다.
	RequestChartDataFromQ();
}


BEGIN_MESSAGE_MAP(SmHdCtrl, CDialogEx)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SmHdCtrl, CDialogEx)
	ON_EVENT(SmHdCtrl, (UINT)-1, 3, OnDataRecv, VTS_BSTR VTS_I4)
	ON_EVENT(SmHdCtrl, (UINT)-1, 4, OnGetBroadData, VTS_BSTR VTS_I4)
	ON_EVENT(SmHdCtrl, (UINT)-1, 5, OnGetMsg, VTS_BSTR VTS_BSTR)
	ON_EVENT(SmHdCtrl, (UINT)-1, 6, OnGetMsgWithRqId, VTS_I4 VTS_BSTR VTS_BSTR)

END_EVENTSINK_MAP()

// VtHdCtrl message handlers

void SmHdCtrl::OnDataRecv(CString sTrCode, LONG nRqID)
{
	if (sTrCode == DefAbChartData) {
		OnRcvdAbroadChartData(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbSiseData) {
		OnRcvdAbroadSiseByReq(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbHogaData) {
		OnRcvdAbroadHogaByReq(sTrCode, nRqID);
	}
	else if (sTrCode == DefAbsChartData2) {
		OnRcvdAbroadChartData2(sTrCode, nRqID);
	}
	else if (sTrCode == DefChartData) {
		OnRcvdDomesticChartData(sTrCode, nRqID);
	}
}

void SmHdCtrl::OnGetBroadData(CString strKey, LONG nRealType)
{
	if (!_Client)
		return;
	switch (nRealType)
	{
	case 76: // hoga
		OnRcvdAbroadHoga(strKey, nRealType);
		break;
	case 82: // sise
		OnRcvdAbroadSise(strKey, nRealType);
		break;
	default:
		break;
	}
}

void SmHdCtrl::OnGetMsg(CString strCode, CString strMsg)
{
	try {
		LOG_F(INFO, "OnGetMsg code = %s, msg = %s", strCode, strMsg);
	}
	catch (std::exception e) {
		std::string msg = e.what();
		LOG_F(INFO, "OnGetMsg Error code = %s, msg = %s", strCode, msg.c_str());
	}
}

void SmHdCtrl::OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg)
{
	try {
		CString msg;
		msg.Format(_T("req_id = %d, hd_server_code = %s, hd_server_msg = %s\n"), nRqId, strCode, strMsg);
		//TRACE(msg);
		LOG_F(INFO, "OnGetMsgWithRqId nRqId = %d, code = %s, msg = %s", nRqId, strCode, strMsg);
	}
	catch (std::exception e) {
		std::string msg = e.what();
		LOG_F(INFO, "OnGetMsgWithRqId nRqId = %d, code = %s, msg = %s", nRqId, strCode, msg.c_str());
	}
}