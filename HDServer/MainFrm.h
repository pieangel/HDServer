
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include <string>
#include <memory>

#define REQ_TIMER 100

class VtProgressDlg;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClientLogin();

	void ClearAllResources();
	// 로그인 후에 필요한 과정들을 수행한다.
	void StartProcess();
	void StartTimer(int milisecond);
	void StopTimer();
	void HideProgress();

private:
	bool _Init = false;

	void ReadConfig();
	std::string _FleWathPath;
	bool _EnableFileWatch = false;
	std::shared_ptr<VtProgressDlg> ProgressDlg = nullptr;
public:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


