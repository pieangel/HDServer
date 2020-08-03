#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "HdTaskArg.h"
#include "../Util/GradientStatic.h"
#include "../Util/XColorStatic.h"
#include <map>
#include <memory>
#include "../MainFrm.h"

// VtProgressDlg dialog
class CMainFrame;
class VtProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtProgressDlg)

public:
	VtProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtProgressDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS };
#endif

	int TotalCount() const { return _TotalCount; }
	void TotalCount(int val) { _TotalCount = val; }
	int RemainCount() const { return _RemainCount; }
	void RemainCount(int val) { _RemainCount = val; }
	std::string TaskTitle() const { return _TaskTitle; }
	void TaskTitle(std::string val) { _TaskTitle = val; }
	std::string TaskDetail() const { return _TaskDetail; }
	void TaskDetail(std::string val) { _TaskDetail = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBrush _BrushBackNor;
	virtual BOOL OnInitDialog();
	CGradientStatic _StaticState;
	CGradientStatic _StaticTask;
	void CloseDialog();
	CProgressCtrl _PrgCtrl;
	void SetPrgressPos(int pos);
	void SetTaskInfo(HdTaskInfo& taskInfo);
	void SetTaskInfo2(HdTaskInfo& taskInfo);
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void SetTaskInfo(std::string msg);
	CMainFrame* MainFrm = nullptr;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void RefreshProgress();
private:
	int _TotalCount = 0;
	int _RemainCount = 0;
	std::string _TaskTitle;
	std::string _TaskDetail;
};
