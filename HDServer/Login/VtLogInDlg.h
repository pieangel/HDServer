#pragma once
#include "../Util/GradientStatic.h"
#include "../Util/ShadeButtonST.h"
#include "../Util/BtnST.h"
#include "afxwin.h"

// VtLogInDlg dialog

class VtLogInDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtLogInDlg)

public:
	VtLogInDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~VtLogInDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	
public:
//	CEdit _EditId;
	CEdit _EditID;
	CEdit _EditCert;
	CEdit _EditPwd;
	CButton	m_ctrlSaveID;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckSave();
	bool Save = true;
	CString id, pwd, cert;
	CString _ServerIP, _ServerPort;
	int _ConnectTimeout = -1;
	int _SendPacketSize = -1;
	virtual BOOL OnInitDialog();
private:
	CBrush _BrushBackNor;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CGradientStatic _StaticCert;
	CGradientStatic _StaticID;
	CGradientStatic _StaticPwd;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButtonST _BtnCancel;
	CButtonST _BtnSignIn;
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnCan();
	CButtonST _BtnCan;
	afx_msg void OnBnClickedCheckSimul();
	afx_msg void OnClose();

	afx_msg LRESULT OnLogin(WPARAM wParam, LPARAM lParam);

private:
	// 로그인이 성공하자 마자 계좌를 읽어 온다.
	void ReadAccount();
	BOOL Login();
	BOOL	ConnectServer();

	CString GetProfileFileName();

	void	LoadProfile();

	void	SaveProfile();

	void    SetServerIP(bool simul = true);

protected:
	DECLARE_MESSAGE_MAP()
};
