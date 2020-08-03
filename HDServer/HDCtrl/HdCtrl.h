#pragma once
#include "../pch.h"
#include "hdfcommagent.h"
#include <string>
// HdCtrl dialog
class HdClient;
class HdCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(HdCtrl)


	DECLARE_EVENTSINK_MAP()
	afx_msg void OnDataRecv(CString sTrCode, LONG nRqID);
	afx_msg void OnGetBroadData(CString strKey, LONG nRealType);
	afx_msg void OnGetMsg(CString strCode, CString strMsg);
	afx_msg void OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg);

public:
	HdCtrl(CWnd* pParent = nullptr);   // standard constructor
	virtual ~HdCtrl();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HD_CTRL };
#endif

	HdClient* Client() const { return _Client; }
	void Client(HdClient* val) { _Client = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CHDFCommAgent& GetHdAgent() {
		return m_CommAgent;
	}
	int Init();
	CString m_sUserId;
	int Login(std::string id, std::string pwd, std::string cert);
private:
	HdClient* _Client = nullptr;
	CHDFCommAgent m_CommAgent;
};
