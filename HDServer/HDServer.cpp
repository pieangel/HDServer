
// HDServer.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "HDServer.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "HDServerDoc.h"
#include "HDServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHDServerApp

BEGIN_MESSAGE_MAP(CHDServerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CHDServerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CHDServerApp construction

CHDServerApp::CHDServerApp() noexcept
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("HDServer.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

int CHDServerApp::RegisterOCX(CString strFileName)
{
#ifdef LOGGING
	CommsDbgLog(0, "RegisterOCX() - start");
#endif
	int			iReturn = 1;
	CString		szErrorMsg;

	strFileName.Replace("'\'", "\\");
	// Initialize OLE.
	if (FAILED(OleInitialize(NULL))) {
		AfxMessageBox("DLLRegister OleInitialize ����");
		return 1;
	}

	SetErrorMode(SEM_FAILCRITICALERRORS);       // Make sure LoadLib fails.
												// Load the library.
	HINSTANCE hLib = LoadLibraryEx(strFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hLib == NULL) {
		szErrorMsg.Format("File Name=%s, GetLastError() NO = 0x%08lx\n", strFileName, GetLastError());
		AfxMessageBox(szErrorMsg);
		iReturn = 0;
		goto CleanupOle;
	}

	HRESULT(STDAPICALLTYPE * lpDllEntryPoint)(void);
	// Find the entry point.
	(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, "DllRegisterServer");
	if (lpDllEntryPoint == NULL) {
		// 		TCHAR szExt[_MAX_EXT];
		// 		_tsplitpath(strFileName, NULL, NULL, NULL, szExt);

		TCHAR drive[255];
		TCHAR szExt[255];
		TCHAR path[MAX_PATH];
		TCHAR filename[MAX_PATH];
		_tsplitpath_s((LPTSTR)(LPCTSTR)strFileName, drive, _countof(drive), path, _countof(path), filename, _countof(filename), szExt, _countof(szExt));

		if ((_stricmp(szExt, ".dll") != 0) && (_stricmp(szExt, ".ocx") != 0)) {
			szErrorMsg.Format("File Name=%s, GetProcAddress Fail\n", strFileName);
			AfxMessageBox(szErrorMsg);
		}

		iReturn = 0;
		goto CleanupLibrary;
	}

	// Call the entry point.
	if (FAILED((*lpDllEntryPoint)())) {
		szErrorMsg.Format("File Name=%s, lpDllEntryPoint Fail\n", strFileName);
		AfxMessageBox(szErrorMsg);
		iReturn = 0;
		goto CleanupLibrary;
	}
	return iReturn;

CleanupLibrary:
	FreeLibrary(hLib);

CleanupOle:
	OleUninitialize();

#ifdef LOGGING
	CommsDbgLog(0, "RegisterOCX() - end");
#endif

	return iReturn;
}

// The one and only CHDServerApp object

CHDServerApp theApp;


// CHDServerApp initialization

BOOL CHDServerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// íƮ OCX ���� ��� ó��
	TCHAR iniFileName[500] = { 0 };

	GetModuleFileName(NULL, iniFileName, MAX_PATH);
	CString path = iniFileName;
	CString fileName = path.Left(path.ReverseFind('\\') + 1);
	fileName = fileName += "HDFCommAgent.ocx";
	RegisterOCX(fileName);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_HDServerTYPE,
		RUNTIME_CLASS(CHDServerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CHDServerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CHDServerApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CHDServerApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CHDServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CHDServerApp message handlers



