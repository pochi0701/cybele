#pragma once


// CTrayWnd

class CTrayWnd : public CWnd
{
private:
	HICON m_hIcon_current;
	BOOL m_bFireDoubleClick;
protected:
	virtual void DoLButtonClick(){}
	virtual void DoRButtonClick(){}
	virtual void DoLButtonDoubleClick(){}
public:
	BOOL Create();
	BOOL NotifyIcon( DWORD dwMessage, HICON hIcon, LPCTSTR pszTip = NULL );
	BOOL NotifyIcon( DWORD dwMessage, HICON hIcon, UINT nStringResource );

	DECLARE_DYNAMIC(CTrayWnd)

public:
	CTrayWnd();
	virtual ~CTrayWnd();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


