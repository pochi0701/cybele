#pragma once

#include "TrayWnd.h"

// CCybeleTrayWnd

class CCybeleTrayWnd : public CTrayWnd
{
private:
	HICON m_hIcon_main;
protected:
	void DoLButtonClick();
	void DoRButtonClick();
	void DoLButtonDoubleClick();

	DECLARE_DYNAMIC(CCybeleTrayWnd)

public:
	CCybeleTrayWnd();
	virtual ~CCybeleTrayWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAppExit();
	afx_msg void OnAppAbout();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	void    CblStart(void);
	void    CblStop(void);
	// exe起動時Cybeleスタートさせるかフラグ
	bool    start_flag;
	bool    cbl_run_flag;
	HANDLE  threadHandle;
	DWORD   id;
};


