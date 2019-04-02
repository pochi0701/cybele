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


// CCybeleApp 初期化

BOOL CCybeleApp::InitInstance()
{
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
