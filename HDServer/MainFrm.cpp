
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "HDServer.h"

#include "MainFrm.h"

#include "Login/VtLogInDlg.h"
#include "Archieve/VtSaveManager.h"
#include "Login/VtLoginManager.h"
#include "Config/SmConfigManager.h"
#include "Global/SmGlobal.h"
#include "Market/SmMarketManager.h"
#include "Symbol/SmSymbolReader.h"
#include "Task/VtProgressDlg.h"
#include "Symbol/SmSymbolManager.h"
#include "Symbol/SmRealtimeRegisterManager.h"
#include "Task/SmCallbackManager.h"
#include "Util/SmUtil.h"
#include "HdCtrl/HdClient.h"
#include "Symbol/SmSymbolReader.h"
#include "Task/ViServerDataReceiver.h"
#include "Chart/SmChartDataManager.h"
#include "Task/SmRealtimeHogaManager.h"
#include "Task/SmRealtimeQuoteManager.h"
#include "Symbol/SmSymbolManager.h"
#include "Database/SmMongoDBManager.h"
#include "Server/SmSessionManager.h"
#include "Config/SmConfigManager.h"
#include "Service/SmProtocolManager.h"
#include "Database/SmTimeSeriesDBManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_CLIENT_LOGIN, &CMainFrame::OnClientLogin)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers



void CMainFrame::OnClientLogin()
{
	VtLogInDlg loginDlg(this);
	if (loginDlg.DoModal() == IDCANCEL)
	{
		return;
	}
}

void CMainFrame::ClearAllResources()
{
	// 증권사 이벤트를 막는다.
	HdClient::GetInstance()->Enable(false);
	// 시세 수집과 호가 수집을 멈춘다.
	SmRealtimeHogaManager::GetInstance()->Enable(false);
	SmRealtimeQuoteManager::GetInstance()->Enable(false);

	SmMongoDBManager::DestroyInstance();
	SmSymbolManager::DestroyInstance();
	SmRealtimeHogaManager::DestroyInstance();
	SmRealtimeQuoteManager::DestroyInstance();
	SmChartDataManager::DestroyInstance();
	ViServerDataReceiver::DestroyInstance();
	HdClient::DestroyInstance();
	SmCallbackManager::DestroyInstance();
	SmRealtimeRegisterManager::DestroyInstance();

	VtSaveManager::DestroyInstance();
	VtLoginManager::DestroyInstance();
	SmSymbolReader::DestroyInstance();
	SmMarketManager::DestroyInstance();
	SmConfigManager::DestroyInstance();
	SmProtocolManager::DestroyInstance();
	SmTimeSeriesDBManager::DestroyInstance();
}

void CMainFrame::StartProcess()
{
	SmSymbolReader* symbol_reader = SmSymbolReader::GetInstance();
	symbol_reader->ReadMarketFile();
	symbol_reader->ReadPmFile();
	symbol_reader->ReadJmFile();

	// 서버 데이터 가져오기를 표시하는 대화 상자를 생성한다.
	ProgressDlg = std::make_shared<VtProgressDlg>();
	ProgressDlg->Create(IDD_PROGRESS, this);
	ProgressDlg->MainFrm = this;
	ProgressDlg->ShowWindow(SW_SHOW);
	ProgressDlg->SetForegroundWindow();
	ProgressDlg->BringWindowToTop();

	// 파일에서 국내 선물/옵션 상품 정보를 읽어 온다.
	//client->LoadProductInfo();


	ViServerDataReceiver* rcvr = ViServerDataReceiver::GetInstance();
	// 진행상황 표시 대화상자를 할당해 준다.
	rcvr->ProgressDlg(ProgressDlg);
	// 심볼 코드를 가져오기 시작한다.
	rcvr->StartGetDmSymbolCode();
}

void CMainFrame::StartTimer(int milisecond)
{
	SetTimer(REQ_TIMER, milisecond, NULL);
}

void CMainFrame::StopTimer()
{
	KillTimer(REQ_TIMER);
}

void CMainFrame::HideProgress()
{
	if (ProgressDlg) {
		ProgressDlg->ShowWindow(SW_HIDE);
		ProgressDlg->DestroyWindow();
	}
}

void CMainFrame::ReadConfig()
{

}


void CMainFrame::OnClose()
{
	ClearAllResources();

	CMDIFrameWnd::OnClose();
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	ViServerDataReceiver* datRcvr = ViServerDataReceiver::GetInstance();
	datRcvr->ExecNext();

	CMDIFrameWnd::OnTimer(nIDEvent);
}
