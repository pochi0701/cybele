// Cybele.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "Cybele.h"
#include "CybeleTrayWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCybeleApp

BEGIN_MESSAGE_MAP(CCybeleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCybeleApp コンストラクション

CCybeleApp::CCybeleApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CCybeleApp オブジェクトです。

CCybeleApp theApp;

// Mutexを生成するときに使用する名前
// 今回はアプリのexe名にします。
#define EXE_NAME   _("Cybele.exe")

// Mutexのハンドル
// グローバル変数とします。
HANDLE m_hMutex = NULL;


// CCybeleApp 初期化

BOOL CCybeleApp::InitInstance()
{

	//フルアクセス(「別のユーザーとして実行」に対応)
	SECURITY_DESCRIPTOR sd;

	if (0 == ::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
		// エラー
		return FALSE;
	}

	if (0 == ::SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE)) {
		// エラー
		return FALSE;
	}

	SECURITY_ATTRIBUTES secAttribute;
	secAttribute.nLength = sizeof(secAttribute);
	secAttribute.lpSecurityDescriptor = &sd;
	secAttribute.bInheritHandle = TRUE;

	m_hMutex = ::CreateMutex(&secAttribute, FALSE, _T("abcdefg"));

	if (m_hMutex == NULL) {
		// ミューテックスの取得に失敗
		return FALSE;
	}

	// 起動していたらすぐに終了させる
	if (::GetLastError() == ERROR_ALREADY_EXISTS) {
		::CloseHandle(m_hMutex);
		return FALSE;  // FALSEを返すと終了する。
	}

	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("Cybele Application Server. Birdland.Co.Ltd All rights reserved."));

	CCybeleTrayWnd* pWnd = new CCybeleTrayWnd();
	pWnd->Create();
	m_pMainWnd = pWnd;

	return TRUE;

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
//	return FALSE;
}

int CCybeleApp::ExitInstance() {
	// Mutexの所有権を解放する。
	::ReleaseMutex(m_hMutex);
	::CloseHandle(m_hMutex);

	return CWinApp::ExitInstance();
}
