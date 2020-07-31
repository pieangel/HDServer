
// HDServer.h : main header file for the HDServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CHDServerApp:
// See HDServer.cpp for the implementation of this class
//

class CHDServerApp : public CWinApp
{
public:
	CHDServerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CHDServerApp theApp;
