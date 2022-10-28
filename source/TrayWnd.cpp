// TrayWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Cybele.h"
#include "TrayWnd.h"
#include "define.h"

#define ID_ICON					(100)
#define WM_USER_TRAYNOTIFYICON	(WM_APP+100)
#define ID_CLICKTIMER			(4)

// CTrayWnd

IMPLEMENT_DYNAMIC(CTrayWnd, CWnd)

CTrayWnd::CTrayWnd() :
	m_hIcon_current(NULL),
	m_bFireDoubleClick(false)
{

}

CTrayWnd::~CTrayWnd()
{
}

BOOL CTrayWnd::Create()
{
	return CreateEx(WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(0),
		_T("TrayWnd"),
		WS_OVERLAPPED,
		0, 0, 0, 0,
		NULL, NULL);
}

BOOL CTrayWnd::NotifyIcon(DWORD dwMessage, HICON hIcon, LPCTSTR pszTip /*= NULL*/)
{
	ASSERT(NIM_ADD == dwMessage
		|| NIM_DELETE == dwMessage
		|| NIM_MODIFY == dwMessage);

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = GetSafeHwnd();
	nid.uID = ID_ICON;	// この関数は、アイコンを1つだけサポートする。
						// タスクトレイのアイコンID
						// 1つのプロセスで複数のアイコンをタスクトレイに表示する場合には、それぞれ異なるIDを指定する。
						// このIDはプロセスごとに独立しているので、他のプロセスが同じIDを指定していたとしても、それらが衝突することはない。
	nid.uCallbackMessage = WM_USER_TRAYNOTIFYICON;
	nid.uFlags = NIF_MESSAGE;

	if (NULL != hIcon
		&& m_hIcon_current != hIcon)
	{	// アイコン変更
		nid.uFlags |= NIF_ICON;
		nid.hIcon = hIcon;
		m_hIcon_current = hIcon;
	}

	if (NULL != pszTip)
	{	// ツールチップ表示
		nid.uFlags |= NIF_TIP;
		_tcsncpy_s(nid.szTip, sizeof(nid.szTip), pszTip, _TRUNCATE);
	}

	return Shell_NotifyIcon(dwMessage, &nid);
}

BOOL CTrayWnd::NotifyIcon(DWORD dwMessage, HICON hIcon, UINT nStringResource)
{
	// 表示メッセージを、ストリングリソースのリソースＩＤで指定する関数
	CString msg;
	VERIFY(msg.LoadString(nStringResource));
	return NotifyIcon(dwMessage, hIcon, msg);
}

BEGIN_MESSAGE_MAP(CTrayWnd, CWnd)
	ON_MESSAGE(WM_USER_TRAYNOTIFYICON, OnTrayNotify)
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CTrayWnd メッセージ ハンドラ



void CTrayWnd::PostNcDestroy()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

	CWnd::PostNcDestroy();

	delete this;
}

LRESULT CTrayWnd::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	WPARAM uiIconID = wParam;
	LPARAM uiMouseMsg = lParam;

	if (ID_ICON != uiIconID)
	{
		return 0;
	}

	// ここで模造メッセージをポストする理由：
	// この通知関数からは、できるだけ早く抜け出す必要がある。
	// たとえば、OnLButtonDownでOLEコントロールを作成しようとすると、
	// 暗号のようなエラーコードRPC_E_CANTCALLOUT_ININPUTSYNCCALLを
	// MFCから受け取ることになるだろう。
	// WinError.hによるとこのエラーの意味は「アプリケーションが入力
	// 同期呼び出しをディスパッチしているので、発信呼び出しはできない。」

	switch (uiMouseMsg)
	{
	case WM_LBUTTONDOWN:
		m_bFireDoubleClick = FALSE;
		SetTimer(ID_CLICKTIMER, GetDoubleClickTime(), NULL);
		break;

	case WM_LBUTTONUP:
		if (m_bFireDoubleClick)
		{
			PostMessage(WM_LBUTTONDBLCLK);
		}
		break;

	case WM_LBUTTONDBLCLK:
		m_bFireDoubleClick = TRUE;
		KillTimer(ID_CLICKTIMER);
		break;

	case WM_RBUTTONUP:
		PostMessage(WM_RBUTTONUP);
		break;
	}

	return 0;
}

void CTrayWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if (ID_CLICKTIMER == nIDEvent)
	{
		KillTimer(nIDEvent);
		PostMessage(WM_LBUTTONUP);
	}

	//	CWnd::OnTimer(nIDEvent);
}

void CTrayWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	IGNORE_PARAMETER(nFlags);
	IGNORE_PARAMETER(point);
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	DoLButtonClick();

	//	CWnd::OnLButtonUp(nFlags, point);
}

void CTrayWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	IGNORE_PARAMETER(nFlags);
	IGNORE_PARAMETER(point);
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	DoRButtonClick();

	//	CWnd::OnRButtonUp(nFlags, point);
}

void CTrayWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	IGNORE_PARAMETER(nFlags);
	IGNORE_PARAMETER(point);
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	DoLButtonDoubleClick();

	//	CWnd::OnLButtonDblClk(nFlags, point);
}
