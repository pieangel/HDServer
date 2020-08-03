// HdCtrl.cpp : implementation file
//

#include "pch.h"
#include "../resource.h"
//#include "../HdTrader.h"
#include "HdCtrl.h"
#include "HDDefine.h"
#include "HdClient.h"
#include "afxdialogex.h"


BEGIN_MESSAGE_MAP(HdCtrl, CDialogEx)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(HdCtrl, CDialogEx)
	ON_EVENT(HdCtrl, (UINT)-1, 3, OnDataRecv, VTS_BSTR VTS_I4)
	ON_EVENT(HdCtrl, (UINT)-1, 4, OnGetBroadData, VTS_BSTR VTS_I4)
	ON_EVENT(HdCtrl, (UINT)-1, 5, OnGetMsg, VTS_BSTR VTS_BSTR)
	ON_EVENT(HdCtrl, (UINT)-1, 6, OnGetMsgWithRqId, VTS_I4 VTS_BSTR VTS_BSTR)

END_EVENTSINK_MAP()

void HdCtrl::OnDataRecv(CString sTrCode, LONG nRqID)
{
	if (!_Client)
		return;
	_Client->OnDataRecv(sTrCode, nRqID);
}

void HdCtrl::OnGetBroadData(CString strKey, LONG nRealType)
{
	if (!_Client)
		return;
	_Client->OnGetBroadData(strKey, nRealType);
}

void HdCtrl::OnGetMsg(CString strCode, CString strMsg)
{
	if (!_Client)
		return;
	_Client->OnGetMsg(strCode, strMsg);
}

void HdCtrl::OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg)
{
	if (!_Client)
		return;
	_Client->OnGetMsgWithRqId(nRqId, strCode, strMsg);
}

// HdCtrl dialog

IMPLEMENT_DYNAMIC(HdCtrl, CDialogEx)

HdCtrl::HdCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HD_CTRL, pParent)
{

}

HdCtrl::~HdCtrl()
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

void HdCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


int HdCtrl::Init()
{
	if (!m_CommAgent.GetSafeHwnd()) {
		m_CommAgent.Create("HDF CommAgent", WS_CHILD, CRect(0, 0, 0, 0), this, 2286);
	}
	else
		return -1000;

	if (m_CommAgent.GetSafeHwnd()) {
		int nRet = m_CommAgent.CommInit(1);

		if (nRet < 0) {
			AfxMessageBox("통신프로그램 실행 오류");
		}
		//else {
		//	AfxMessageBox("통신프로그램 실행 성공");
		//}

		return nRet;
	}

	return -1000;
}

int HdCtrl::Login(std::string id, std::string pwd, std::string cert)
{
	m_sUserId = id.c_str();

	const char* pLoginSuccess[] = { "로그인 성공"	, "Login Successful" };
	const char* pLoginFail[] = { "로그인 실패"	, "Login Failure" };

	int nRet = m_CommAgent.CommLogin(id.c_str(), pwd.c_str(), cert.c_str());
	if (nRet > 0) {
		//AfxMessageBox(pLoginSuccess[0]);

		//로긴후 반드시 호출...
		// 계좌 정보를 가져온다.
		m_CommAgent.CommAccInfo();
		//LOG_F(INFO, pLoginSuccess[0]);
	}
	else
	{
		CString strRet;
		strRet.Format("[%d]", nRet);
		AfxMessageBox(pLoginFail[0] + strRet);
		//LOG_F(INFO, _T("%s"), strRet);
	}

	return nRet;
}



// HdCtrl message handlers
