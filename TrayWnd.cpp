// TrayWnd.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "Cybele.h"
#include "TrayWnd.h"

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
	return CreateEx(	WS_EX_TOOLWINDOW,
						AfxRegisterWndClass(0),
						_T("TrayWnd"),
						WS_OVERLAPPED,
						0, 0, 0, 0,
						NULL, NULL );
}

BOOL CTrayWnd::NotifyIcon( DWORD dwMessage, HICON hIcon, LPCTSTR pszTip /*= NULL*/ )
{
	ASSERT( NIM_ADD    == dwMessage 
		 || NIM_DELETE == dwMessage 
		 || NIM_MODIFY == dwMessage );
	
	NOTIFYICONDATA nid;
	ZeroMemory( &nid, sizeof(NOTIFYICONDATA) );
	nid.cbSize				= sizeof(NOTIFYICONDATA);
	nid.hWnd				= GetSafeHwnd();
	nid.uID					= ID_ICON;	// ���̊֐��́A�A�C�R����1�����T�|�[�g����B
										// �^�X�N�g���C�̃A�C�R��ID
										// 1�̃v���Z�X�ŕ����̃A�C�R�����^�X�N�g���C�ɕ\������ꍇ�ɂ́A���ꂼ��قȂ�ID���w�肷��B
										// ����ID�̓v���Z�X���ƂɓƗ����Ă���̂ŁA���̃v���Z�X������ID���w�肵�Ă����Ƃ��Ă��A����炪�Փ˂��邱�Ƃ͂Ȃ��B
	nid.uCallbackMessage	= WM_USER_TRAYNOTIFYICON;
	nid.uFlags				= NIF_MESSAGE;
	
	if( NULL			!= hIcon
	 && m_hIcon_current != hIcon )
	{	// �A�C�R���ύX
		nid.uFlags			|= NIF_ICON;
		nid.hIcon			= hIcon;
		m_hIcon_current		= hIcon;
	}
	
	if( NULL != pszTip )
	{	// �c�[���`�b�v�\��
		nid.uFlags |= NIF_TIP;
		_tcsncpy_s( nid.szTip, sizeof(nid.szTip), pszTip, _TRUNCATE );
	}
	
	return Shell_NotifyIcon( dwMessage, &nid );
}

BOOL CTrayWnd::NotifyIcon( DWORD dwMessage, HICON hIcon, UINT nStringResource )
{
	// �\�����b�Z�[�W���A�X�g�����O���\�[�X�̃��\�[�X�h�c�Ŏw�肷��֐�
	CString msg;
	VERIFY( msg.LoadString(nStringResource) );
	return NotifyIcon( dwMessage, hIcon, msg );
}

BEGIN_MESSAGE_MAP(CTrayWnd, CWnd)
	ON_MESSAGE(WM_USER_TRAYNOTIFYICON, OnTrayNotify)
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CTrayWnd ���b�Z�[�W �n���h��



void CTrayWnd::PostNcDestroy()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B

	CWnd::PostNcDestroy();

	delete this;
}

LRESULT CTrayWnd::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	WPARAM uiIconID   = wParam;
	LPARAM uiMouseMsg = lParam;
	
	if( ID_ICON != uiIconID )
	{
		return 0;
	}
	
	// �����Ŗ͑����b�Z�[�W���|�X�g���闝�R�F
	// ���̒ʒm�֐�����́A�ł��邾�����������o���K�v������B
	// ���Ƃ��΁AOnLButtonDown��OLE�R���g���[�����쐬���悤�Ƃ���ƁA
	// �Í��̂悤�ȃG���[�R�[�hRPC_E_CANTCALLOUT_ININPUTSYNCCALL��
	// MFC����󂯎�邱�ƂɂȂ邾�낤�B
	// WinError.h�ɂ��Ƃ��̃G���[�̈Ӗ��́u�A�v���P�[�V����������
	// �����Ăяo�����f�B�X�p�b�`���Ă���̂ŁA���M�Ăяo���͂ł��Ȃ��B�v
	
	switch( uiMouseMsg )
	{
	case WM_LBUTTONDOWN:
		m_bFireDoubleClick = FALSE;
		SetTimer( ID_CLICKTIMER, GetDoubleClickTime(), NULL );
		break;
		
	case WM_LBUTTONUP:
		if( m_bFireDoubleClick )
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
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if( ID_CLICKTIMER == nIDEvent )
	{
		 KillTimer(nIDEvent);
		 PostMessage( WM_LBUTTONUP );
	}

//	CWnd::OnTimer(nIDEvent);
}

void CTrayWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	DoLButtonClick();

//	CWnd::OnLButtonUp(nFlags, point);
}

void CTrayWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	DoRButtonClick();

//	CWnd::OnRButtonUp(nFlags, point);
}

void CTrayWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	DoLButtonDoubleClick();

//	CWnd::OnLButtonDblClk(nFlags, point);
}
