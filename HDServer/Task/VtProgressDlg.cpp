// VtProgressDlg.cpp : implementation file
//

#include "pch.h"
#include "VtProgressDlg.h"
#include "afxdialogex.h"
#include "../resource.h"

//#include "HdScheduler.h"
//#include "../MainFrm.h"
#include "../Symbol/SmSymbolReader.h"
//#include "../Task/SmTaskManager.h"
// VtProgressDlg dialog





IMPLEMENT_DYNAMIC(VtProgressDlg, CDialogEx)

VtProgressDlg::VtProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROGRESS, pParent)
{

}

VtProgressDlg::~VtProgressDlg()
{
}

void VtProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATE, _StaticState);
	DDX_Control(pDX, IDC_PROGRESS_CTRL, _PrgCtrl);
	DDX_Control(pDX, IDC_STATIC_TASK, _StaticTask);
}


BEGIN_MESSAGE_MAP(VtProgressDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// VtProgressDlg message handlers


BOOL VtProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	VERIFY(_BrushBackNor.CreateSolidBrush(RGB(3, 30, 82)));

	_StaticState.SetTextColor(RGB(255, 255, 255));
	_StaticState.SetColor(RGB(3, 30, 82));
	_StaticState.SetGradientColor(RGB(3, 30, 82));
	_StaticState.SetTextAlign(1);
	_StaticState.SetWindowText("작업중입니다.");

	_StaticTask.SetTextColor(RGB(255, 255, 255));
	_StaticTask.SetColor(RGB(3, 30, 82));
	_StaticTask.SetGradientColor(RGB(3, 30, 82));
	_StaticTask.SetTextAlign(1);



	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtProgressDlg::CloseDialog()
{

	EndDialog(1);
}

void VtProgressDlg::SetPrgressPos(int pos)
{
	_PrgCtrl.SetPos(pos);
}

void VtProgressDlg::SetTaskInfo(HdTaskInfo& taskInfo)
{
	if (taskInfo.argVec.size() == 0)
		return;
	_StaticTask.SetWindowText(taskInfo.TaskName.c_str());
	size_t index = taskInfo.TotalTaskCount - taskInfo.RemainTaskCount;
	if (index < taskInfo.argVec.size()) {
		std::pair<std::string, HdTaskArg> arg = taskInfo.argVec[index];
		_StaticState.SetWindowText(std::get<0>(arg).c_str());
	}
	double top = (double)taskInfo.TotalTaskCount - (double)taskInfo.RemainTaskCount;
	double entire = top / taskInfo.TotalTaskCount;
	int percent = static_cast<int>(entire * 100);
	SetPrgressPos(percent);
}


void VtProgressDlg::SetTaskInfo(std::string msg)
{
	_StaticState.SetWindowText(msg.c_str());
}

void VtProgressDlg::SetTaskInfo2(HdTaskInfo& taskInfo)
{
	_StaticTask.SetWindowText(taskInfo.TaskName.c_str());
	if (taskInfo.argMap.size() > 0) {
		std::shared_ptr<HdTaskArg> arg = taskInfo.argMap.begin()->second;
		_StaticState.SetWindowText(arg->info_text.c_str());
	}
	double top = (double)taskInfo.TotalTaskCount - (double)taskInfo.RemainTaskCount;
	double entire = top / taskInfo.TotalTaskCount;
	int percent = static_cast<int>(entire * 100);
	SetPrgressPos(percent);
}

void VtProgressDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	//delete this;
	CDialogEx::PostNcDestroy();
}


void VtProgressDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	DestroyWindow();
	CDialogEx::OnCancel();
}


HBRUSH VtProgressDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hBrush = NULL;
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STATE || 
		pWnd->GetDlgCtrlID() == IDC_STATIC_TASK)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(3, 30, 82));
	}
	_StaticState.SetTextAlign(1);
	_StaticTask.SetTextAlign(1);
	hBrush = (HBRUSH)_BrushBackNor;
	return hBrush;
}

void VtProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
}

void VtProgressDlg::RefreshProgress()
{
	_StaticTask.SetWindowText(_TaskTitle.c_str());
	_StaticState.SetWindowText(_TaskDetail.c_str());
	double top = (double)_TotalCount - (double)_RemainCount;
	double entire = top / (double)_TotalCount;
	int percent = static_cast<int>(entire * 100);
	SetPrgressPos(percent);
}
