// VtLogInDlg.cpp : implementation file
//

#include "pch.h"
//#include "../HdTrader.h"
#include "VtLogInDlg.h"
#include "afxdialogex.h"
#include "../resource.h"
//#include "VtHdClient.h"
#include <string>
#include "VtLoginManager.h"
#include "../MainFrm.h"
//#include "../EbCtrl/EbClient.h"
#include "../Util/cryptor.hpp"
#include <memory>
#include "../Global/SmGlobal.h"
#include "../Archieve/VtSaveManager.h"
#include "../HdCtrl/HdClient.h"

//extern TApplicationFont g_Font;
// VtLogInDlg dialog

IMPLEMENT_DYNAMIC(VtLogInDlg, CDialogEx)

VtLogInDlg::VtLogInDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_MAIN, pParent)
{

}

VtLogInDlg::~VtLogInDlg()
{
}

void VtLogInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT_CERT, _EditId);
	DDX_Control(pDX, IDC_EDIT_ID, _EditID);
	DDX_Control(pDX, IDC_EDIT_CERT, _EditCert);
	DDX_Control(pDX, IDC_EDIT_PWD, _EditPwd);
	DDX_Control(pDX, IDC_STATIC_CERT, _StaticCert);
	DDX_Control(pDX, IDC_STATIC_ID, _StaticID);
	DDX_Control(pDX, IDC_STATIC_PWD, _StaticPwd);
	DDX_Control(pDX, IDC_BTN_LOGIN, _BtnSignIn);
	DDX_Control(pDX, IDC_BTN_CAN, _BtnCan);
	DDX_Control(pDX, IDC_CHECK_SAVE, m_ctrlSaveID);
}


BEGIN_MESSAGE_MAP(VtLogInDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_SAVE, &VtLogInDlg::OnBnClickedCheckSave)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_LOGIN, &VtLogInDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_CAN, &VtLogInDlg::OnBnClickedBtnCan)
	ON_BN_CLICKED(IDC_CHECK_SIMUL, &VtLogInDlg::OnBnClickedCheckSimul)
	ON_WM_CLOSE()
	//ON_MESSAGE(WM_USER + XM_LOGIN, &VtLogInDlg::OnLogin)
END_MESSAGE_MAP()


// VtLogInDlg message handlers


void VtLogInDlg::OnBnClickedOk()
{
	_EditID.GetWindowText(id);
	_EditPwd.GetWindowText(pwd);
	_EditCert.GetWindowText(cert);
	
	CDialogEx::OnOK();
}


void VtLogInDlg::OnBnClickedCheckSave()
{
	Save = !Save;
}


BOOL VtLogInDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	// 설정파일을 읽어 컨트롤들을 초기화
	LoadProfile();

	VERIFY(_BrushBackNor.CreateSolidBrush(RGB(3, 30, 82)));

	_BtnCan.BorderShadow(RGB(34, 133, 212));
	_BtnCan.BorderLight(RGB(34, 133, 212));
	_BtnCan.DrawBorder(TRUE, TRUE);
	_BtnCan.SetAlign(ST_ALIGN_VERT | ST_ALIGN_HORIZ);
	_BtnCan.SetColor(BTNST_COLOR_BK_IN, RGB(3, 30, 82), true);
	_BtnCan.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
	_BtnCan.SetColor(BTNST_COLOR_BK_OUT, RGB(3, 30, 82), true);
	_BtnCan.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);

	_BtnSignIn.BorderShadow(RGB(34, 133, 212));
	_BtnSignIn.BorderLight(RGB(34, 133, 212));
	_BtnSignIn.DrawBorder(TRUE, TRUE);
	_BtnSignIn.SetAlign(ST_ALIGN_VERT | ST_ALIGN_HORIZ);
	_BtnSignIn.SetColor(BTNST_COLOR_BK_IN, RGB(3, 30, 82), true);
	_BtnSignIn.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
	_BtnSignIn.SetColor(BTNST_COLOR_BK_OUT, RGB(3, 30, 82), true);
	_BtnSignIn.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);

	_StaticID.SetTextColor(RGB(255, 255, 255));
	_StaticID.SetColor(RGB(3, 30, 82));
	_StaticID.SetGradientColor(RGB(3, 30, 82));

	_StaticPwd.SetTextColor(RGB(255, 255, 255));
	_StaticPwd.SetColor(RGB(3, 30, 82));
	_StaticPwd.SetGradientColor(RGB(3, 30, 82));

	_StaticCert.SetTextColor(RGB(255, 255, 255));
	_StaticCert.SetColor(RGB(3, 30, 82));
	_StaticCert.SetGradientColor(RGB(3, 30, 82));


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL VtLogInDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	pDC->FillRect(&rcClient, &_BrushBackNor);
	return TRUE;
}


HBRUSH VtLogInDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_CHECK_SAVE)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		hbr = (HBRUSH)_BrushBackNor;
	}
	if (pWnd->GetDlgCtrlID() == IDC_CHECK_SIMUL)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		hbr = (HBRUSH)_BrushBackNor;
	}
	if (nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)_BrushBackNor;
	}
	return hbr;
}


void VtLogInDlg::OnBnClickedBtnLogin()
{	
	EnableWindow(FALSE);

	HdClient* client = HdClient::GetInstance();

	// 서버접속
	if (client->Init() < 0)
	{
		EnableWindow(TRUE);
		return;
	}

	// 로그인
	Login();
}


void VtLogInDlg::OnBnClickedBtnCancel()
{
	// 로그인 종료
	CDialogEx::EndDialog(IDCANCEL);

}


void VtLogInDlg::OnBnClickedBtnCan()
{
	CDialogEx::EndDialog(IDCANCEL);
}


void VtLogInDlg::OnBnClickedCheckSimul()
{
	bool simul = false;
	if (((CButton*)GetDlgItem(IDC_CHECK_SIMUL))->GetCheck() == BST_CHECKED) {
		simul = true;
	}
	else {
		simul = false;
	}
	SetServerIP(simul);
}


void VtLogInDlg::OnClose()
{
	SaveProfile();

	CDialogEx::OnClose();
}

LRESULT VtLogInDlg::OnLogin(WPARAM wParam, LPARAM lParam)
{
	LPCSTR pszCode = (LPCSTR)wParam;
	LPCSTR pszMsg = (LPCSTR)lParam;

	CString strMsg;
	strMsg.Format("[%s] %s", pszCode, pszMsg);

	EnableWindow(TRUE);

// 	if (atoi(pszCode) == 0)
// 	{
// 		// 로그인 성공
// 		//MessageBox(strMsg, "로그인 성공", MB_ICONINFORMATION);
// 		VtSaveManager* saveMgr = VtSaveManager::GetInstance();
// 
// 		// 로그인 사용자를 위한 디렉토리를 만든다.
// 		VtGlobal::LoginUserID = id;
// 		saveMgr->CreateUserDirectory();
// 
// 		// 상태 저장
// 		SaveProfile();
// 
// 		// 로그인 종료
// 		CDialog::EndDialog(IDOK);
// 
// 		CViDataCollectorDlg* mf = (CViDataCollectorDlg*)AfxGetMainWnd();
// 		mf->StartProcess();
// 	}
// 	else
// 	{
// 		// 로그인 실패
// 		MessageBox(strMsg, "로그인 실패", MB_ICONSTOP);
// 	}

	return 0L;
}

void VtLogInDlg::ReadAccount()
{
	
}

BOOL VtLogInDlg::Login()
{
	//-----------------------------------------------------------------------------
	// 아이디
	GetDlgItemText(IDC_EDIT_ID, id);

	//-----------------------------------------------------------------------------
	// 비밀번호
	GetDlgItemText(IDC_EDIT_PWD, pwd);

	//-----------------------------------------------------------------------------
	// 공인인증 비밀번호
	GetDlgItemText(IDC_EDIT_CERT, cert);

	int result = HdClient::GetInstance()->Login((LPCTSTR)id, (LPCTSTR)pwd, (LPCTSTR)cert);
	//-----------------------------------------------------------------------------
	// 로그인 에러 발생,
	//		이것은 로그인 사전단계에서 발생한 에러이며 로그인 과정에서 발생한 에러는
	//		메시지로 알려준다.
	if (result < 0)
	{
		EnableWindow(TRUE);
		return FALSE;
	}

	// 현재 설정을 저장한다.
	SaveProfile();

	// 로그인 성공
	//MessageBox(strMsg, "로그인 성공", MB_ICONINFORMATION);
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();

	// 로그인 사용자를 위한 디렉토리를 만든다.
	SmGlobal::LoginUserID = id;
	saveMgr->CreateUserDirectory();

	// 상태 저장
	SaveProfile();

	// 로그인 종료
	CDialog::EndDialog(IDOK);

	// 해외 종목 파일을 다운로드 받는다.
	HdClient::GetInstance()->DownloadAbroadSymbolFiles();

	CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();
	// 서버에서 정보를 가져오기 시작한다.
	//mf->StartProcess();

	return TRUE;
}

BOOL VtLogInDlg::ConnectServer()
{
	
	return TRUE;
}

CString VtLogInDlg::GetProfileFileName()
{
	CString strPath;
	int nLen = (int)GetModuleFileName(AfxGetInstanceHandle(), strPath.GetBuffer(256), 256);
	strPath.ReleaseBuffer(nLen);
	int nFind = strPath.ReverseFind('\\');
	if (nFind >= 0) strPath = strPath.Left(nFind);

	return strPath + "\\SMTrader.ini";
}

void VtLogInDlg::LoadProfile()
{
	CString strFileName = GetProfileFileName();
	TCHAR	szData[256];

	//-----------------------------------------------------------------------------
	// 아이디 저장
	GetPrivateProfileString("LOGIN", "SAVE_ID", "0", szData, sizeof(szData), strFileName);
	if (atoi(szData) == 0)
	{
		m_ctrlSaveID.SetCheck(0);
	}
	else
	{
		m_ctrlSaveID.SetCheck(1);

		//-----------------------------------------------------------------------------
		// 아이디
		GetPrivateProfileString("LOGIN", "ID", "", szData, sizeof(szData), strFileName);
		std::string temp;
		temp = szData;
		id = cryptor::decrypt(temp).c_str();
		SetDlgItemText(IDC_EDIT_ID, id);

		GetPrivateProfileString("LOGIN", "PWD", "", szData, sizeof(szData), strFileName);
		temp = szData;
		pwd = cryptor::decrypt(temp).c_str();
		SetDlgItemText(IDC_EDIT_PWD, pwd);

		GetPrivateProfileString("LOGIN", "CERT", "", szData, sizeof(szData), strFileName);
		temp = szData;
		cert = cryptor::decrypt(temp).c_str();
		SetDlgItemText(IDC_EDIT_CERT, cert);
	}

	//-----------------------------------------------------------------------------
	// 공인인증 에러시 에러 Dialog 표시
	//GetPrivateProfileString("LOGIN", "SHOW_CERT_ERR_DLG", "1", szData, sizeof(szData), strFileName);
	//m_ctrlShowCertErrDlg.SetCheck(atoi(szData) == 0 ? 0 : 1);

	//-----------------------------------------------------------------------------
	// 실서버/모의서버
	GetPrivateProfileString("COMM", "SERVER_TYPE", "", szData, sizeof(szData), strFileName);
	BOOL bSimul = _stricmp(szData, "SIMUL") == 0;
	//CheckRadioButton(IDC_RADIO_REAL, IDC_RADIO_SIMUL, bSimul ? IDC_RADIO_SIMUL : IDC_RADIO_REAL);
	CheckDlgButton(IDC_CHECK_SIMUL, bSimul ? BST_CHECKED : BST_UNCHECKED);

	

	//SetServerAddrType(bSimul == FALSE);

	//-----------------------------------------------------------------------------
	// 서버 주소 TYPE
// 	GetPrivateProfileString("COMM", "SERVER_ADDR_TYPE", "", szData, sizeof(szData), strFileName);
// 	SetServerAddrTypeProfile(szData);
// 	GetPrivateProfileString("COMM", "SERVER_IP", "", szData, sizeof(szData), strFileName);
// 	CString strData; m_ctrlServerAddrType.GetWindowText(strData);
// 	SetServerIP(strData, szData);

	SetServerIP(bSimul);

	//-----------------------------------------------------------------------------
	// 서버 포트
	GetPrivateProfileString("COMM", "SERVER_PORT", "", szData, sizeof(szData), strFileName);
	//SetPortProfile(atoi(szData));
	_ServerPort = szData;

	//-----------------------------------------------------------------------------
	// 최대전송크기
	//GetPrivateProfileString("COMM", "USE_SEND_MAX_PACKET_SIZE", "", szData, sizeof(szData), strFileName);
// 	if (atoi(szData) == 0)
// 	{
// 		m_ctrlSendPacketSize.SetCheck(0);
// 		GetDlgItem(IDC_COMBO_SEND_PACKET_SIZE)->EnableWindow(FALSE);
// 		SetDlgItemInt(IDC_COMBO_SEND_PACKET_SIZE, 512);
// 	}
// 	else
// 	{
// 		m_ctrlSendPacketSize.SetCheck(1);
// 		GetDlgItem(IDC_COMBO_SEND_PACKET_SIZE)->EnableWindow(TRUE);
// 
// 		GetPrivateProfileString("COMM", "SEND_MAX_PACKET_SIZE", "", szData, sizeof(szData), strFileName);
// 		SetDlgItemInt(IDC_COMBO_SEND_PACKET_SIZE, atoi(szData));
// 	}

	//-----------------------------------------------------------------------------
	// 서버접속시간
	//GetPrivateProfileString("COMM", "USE_CONNECT_TIMEOUT", "", szData, sizeof(szData), strFileName);
// 	if (atoi(szData) == 0)
// 	{
// 		m_ctrlConnectTimeout.SetCheck(0);
// 		GetDlgItem(IDC_EDIT_CONNECT_TIMEOUT)->EnableWindow(FALSE);
// 		SetDlgItemInt(IDC_EDIT_CONNECT_TIMEOUT, 30);
// 	}
// 	else
// 	{
// 		m_ctrlConnectTimeout.SetCheck(1);
// 		GetDlgItem(IDC_EDIT_CONNECT_TIMEOUT)->EnableWindow(TRUE);
// 
// 		GetPrivateProfileString("COMM", "CONNECT_TIMEOUT", "", szData, sizeof(szData), strFileName);
// 		SetDlgItemInt(IDC_EDIT_CONNECT_TIMEOUT, atoi(szData));
// 	}
}


void VtLogInDlg::SaveProfile()
{
	CString strFileName = GetProfileFileName();
	CString strData;

	//-----------------------------------------------------------------------------
	// 아이디 저장
	if (m_ctrlSaveID.GetCheck() == 0)
	{
		WritePrivateProfileString("LOGIN", "SAVE_ID", "0", strFileName);
		WritePrivateProfileString("LOGIN", "ID", "", strFileName);
		WritePrivateProfileString("LOGIN", "PWD", "", strFileName);
		WritePrivateProfileString("LOGIN", "CERT", "", strFileName);
	}
	else
	{
		//GetDlgItemText(IDC_EDIT_ID, strData);

		auto enc_id = cryptor::encrypt(std::move((LPCTSTR)id));
		auto enc_pwd = cryptor::encrypt(std::move((LPCTSTR)pwd));
		auto enc_cert = cryptor::encrypt(std::move((LPCTSTR)cert));

		WritePrivateProfileString("LOGIN", "SAVE_ID", "1", strFileName);
		WritePrivateProfileString("LOGIN", "ID", enc_id.c_str(), strFileName);
		WritePrivateProfileString("LOGIN", "PWD", enc_pwd.c_str(), strFileName);
		WritePrivateProfileString("LOGIN", "CERT", enc_cert.c_str(), strFileName);
	}

	//-----------------------------------------------------------------------------
	// 공인인증 에러시 에러 Dialog 표시
	strData = "0"; // m_ctrlShowCertErrDlg.GetCheck() == 0 ? "0" : "1";
	WritePrivateProfileString("LOGIN", "SHOW_CERT_ERR_DLG", strData, strFileName);

	//-----------------------------------------------------------------------------
	// 실서버/모의서버
	strData = ((CButton*)GetDlgItem(IDC_CHECK_SIMUL))->GetCheck() == BST_UNCHECKED ? "REAL" : "SIMUL";
	WritePrivateProfileString("COMM", "SERVER_TYPE", strData, strFileName);

	//-----------------------------------------------------------------------------
	// 서버 주소 TYPE
	//GetDlgItemText(IDC_COMBO_SERVER_ADDR_TYPE, strData);
	//WritePrivateProfileString("COMM", "SERVER_ADDR_TYPE", strData, strFileName);

	//-----------------------------------------------------------------------------
	// 서버 주소
	//GetDlgItemText(IDC_EDIT_SERVER_IP, strData);
	WritePrivateProfileString("COMM", "SERVER_IP", _ServerIP, strFileName);

	//-----------------------------------------------------------------------------
	// 서버 포트
	//GetDlgItemText(IDC_COMBO_SERVER_PORT, strData);
	WritePrivateProfileString("COMM", "SERVER_PORT", _ServerPort, strFileName);

	//-----------------------------------------------------------------------------
	// 최대전송크기
// 	strData = m_ctrlSendPacketSize.GetCheck() == 0 ? "0" : "1";
// 	WritePrivateProfileString("COMM", "USE_SEND_MAX_PACKET_SIZE", strData, strFileName);
// 	GetDlgItemText(IDC_COMBO_SEND_PACKET_SIZE, strData);
// 	WritePrivateProfileString("COMM", "SEND_MAX_PACKET_SIZE", strData, strFileName);
// 	OnCheckSendPacketSize();

	//-----------------------------------------------------------------------------
	// 서버접속시간
// 	strData = m_ctrlConnectTimeout.GetCheck() == 0 ? "0" : "1";
// 	WritePrivateProfileString("COMM", "USE_CONNECT_TIMEOUT", strData, strFileName);
// 	GetDlgItemText(IDC_EDIT_CONNECT_TIMEOUT, strData);
// 	WritePrivateProfileString("COMM", "CONNECT_TIMEOUT", strData, strFileName);
// 	OnCheckConnectTimeout();
}

void VtLogInDlg::SetServerIP(bool simul /*= true*/)
{
	
}
