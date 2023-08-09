// CybeleTrayWnd.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Cybele.h"
#include "CybeleTrayWnd.h"

#include "AboutDlg.h"
#include "cbl_String.h"
#include "cbl.h"
#include "const.h"



// CCybeleTrayWnd
extern int Cybelemain(void* argv);

IMPLEMENT_DYNAMIC(CCybeleTrayWnd, CTrayWnd)
#if 0
typedef struct tagNCB_RESULT
{
	ADAPTER_STATUS	adapt;
	NAME_BUFFER		szNameBuff[32];
} NCB_RESULT;
//---------------------------------------------------------------------------
bool CCybeleTrayWnd::GetMacAddr(char* MAC_ADDR)
{
	GUID	guid;
	BYTE	bMacAddr[6];
	UCHAR	bResult;



	/* GUIDを取得する */
	CoCreateGuid(&guid);
	/* MACアドレスを取得する */
	bResult = GetMacAddress((char*)bMacAddr);
	if (bResult != NRC_GOODRET) {
		//                  001122334455
		sprintf(MAC_ADDR, "FFFFFFFFFFFF");
		return false;
	}

	/* 表示 */

	sprintf(MAC_ADDR, "%02X%02X%02X%02X%02X%02X",
		bMacAddr[0], bMacAddr[1], bMacAddr[2],
		bMacAddr[3], bMacAddr[4], bMacAddr[5]);

	return true;
}
#endif
//---------------------------------------------------------------------------
void CCybeleTrayWnd::CblStart(void)
{
	threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Cybelemain, NULL, 0, &id);
	cbl_run_flag = true;   // Cybele起動フラグON
}
//---------------------------------------------------------------------------
void CCybeleTrayWnd::CblStop(void)
{
	DWORD dwExCode;
	DWORD dwParam;
	//detect_finalize();
	GetExitCodeThread(threadHandle, &dwParam);
	if (dwParam == STILL_ACTIVE) {
		loop_flag = 0;
		closesocket(listen_socket);
		if (WaitForSingleObject(threadHandle, INFINITE) != WAIT_OBJECT_0) {
			TerminateThread(threadHandle, id);
			while (1) {
				GetExitCodeThread(threadHandle, &dwExCode);
				if (dwExCode != STILL_ACTIVE) {
					break;
				}
			}
		}
	}
	CloseHandle(threadHandle);
	threadHandle = (HANDLE)NULL;
	cbl_run_flag = false;   //Cybele起動フラグOFF
}
CCybeleTrayWnd::CCybeleTrayWnd() :
	m_hIcon_main(NULL)
{
	//threadHandle = NULL;
	OleInitialize(NULL);
	//MacAddrの設定
	//GetMacAddr(MAC_ADDR);
	//wString::wStringInit();
	//サーバ起動
	CblStart();
	//ブラウザ起動
	system("start http://127.0.0.1:8000/");
	//ShellExecute(NULL, "open", "http://127.0.0.1:8000/", NULL, NULL, SW_SHOW);
#if 0
	//最初がいいからここでＰＲＯＸＹ設定すっかな。
	//ついでにFireWall登録しちゃうわｗ
	TRegistry* reg = new TRegistry;
	char buffer[1024] = {};
	char tmp[1024] = {};
	int* pnum;
	AnsiString str;
	int pos;
	int len;
	AnsiString key;
	//ウイルスと大差ないので取る
	////ファイヤーウォールの登録
	//AnsiString key="SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile\\AuthorizedApplications\\List";
	//reg->RootKey = HKEY_LOCAL_MACHINE;
	//if( reg->KeyExists(key) ){
	//    reg->OpenKey(key,false);
	//    str = reg->ReadString(Application->ExeName);
	//    if( str != Application->ExeName+":*:Enabled:Cybele	" ){
	//        reg->WriteString(Application->ExeName,Application->ExeName+":*:Enabled:Cybele");
	//    }
	//    reg->CloseKey();
	//}



	key = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Connections";
	reg->RootKey = HKEY_LOCAL_MACHINE;
	if (reg->KeyExists(key)) {
		reg->OpenKey(key, false);
		int len = reg->ReadBinaryData("WinHttpSettings", buffer, sizeof(buffer));
		//文字列長さの位置
		pos = 8 + sizeof(int);
		if (len < pos)exit(2);
		//文字列長さ
		pnum = (int*)&buffer[pos];
		pos += sizeof(int);
		//*pnum == 0ならばプロキシー設定がないとする。
		if (*pnum) {
			//strにプロキシー設定を取得
			memcpy(tmp, buffer + pos, *pnum);
			str = tmp;
			if (str.Pos(";") > 0) {
				//ftp=hogehoge.co.jp:80;gopher=hogehoge.co.jp:80;http=hogehoge.co.jp:80;https=hogehoge.co.jp:80
				TStringList* work = new TStringList;
				work->Delimiter = ';';
				work->DelimitedText = str;
				for (int i = 0; i < work->Count; i++) {
					str = work->Strings[i];
					if (str.Pos("http=") > 0) {
						len = str.Pos(":");
						strcpy(Proxy, str.SubString(6, len - 6).c_str());
						ProxyPort = atoi(str.SubString(len + 1, str.Length() - len).c_str());
						IsProxy = 1;
						break;
					}
				}
				delete work;
			}
			else {
				//
				len = str.Pos(":");
				strcpy(Proxy, str.SubString(1, len - 1).c_str());
				ProxyPort = atoi(str.SubString(len + 1, str.Length() - len).c_str());
				IsProxy = 1;
			}
			//
			//extern char Proxy[256];
			//extern int ProxyPort;

			//あとlocal設定あるけど捨てる
			//int numn = strlen(tmp);
			//pos += *ptr;
			//ptr = (int*)&buffer[pos];
			//ppr = buffer+pos;
		}
		else {
			IsProxy = 0;
		}
		reg->CloseKey();
	}
	else {
		//カレントユーザでもしらべる。
		key = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
		reg->RootKey = HKEY_CURRENT_USER;
		if (reg->KeyExists(key)) {
			reg->OpenKey(key, false);
			int num = reg->ReadInteger("ProxyEnable");
			if (num == 0) {
				IsProxy = 0;
				reg->CloseKey();
				return;
			}
			str = reg->ReadString("ProxyServer");
			if (str.Pos(":") > 0) {
				//
				len = str.Pos(":");
				strcpy(Proxy, str.SubString(1, len - 1).c_str());
				ProxyPort = atoi(str.SubString(len + 1, str.Length() - len).c_str());
				IsProxy = 1;
			}
			else {
				IsProxy = 0;
			}
			reg->CloseKey();

		}
		else {
			IsProxy = 0;
		}

	}
	delete reg;

#endif
}

CCybeleTrayWnd::~CCybeleTrayWnd()
{
}

void CCybeleTrayWnd::DoLButtonClick()
{
	POINT pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.LoadMenu(IDM_TRAY_L);

	CMenu* pPopup = menu.GetSubMenu(0);

	// SetForgroundWindowとPostMessageが必要な理由は、
	// Knowledge Base (Q135788)参照のこと
	SetForegroundWindow();
	pPopup->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	PostMessage(WM_NULL);
}

void CCybeleTrayWnd::DoRButtonClick()
{
	POINT pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.LoadMenu(IDM_TRAY_R);

	CMenu* pPopup = menu.GetSubMenu(0);

	// SetForgroundWindowとPostMessageが必要な理由は、
	// Knowledge Base (Q135788)参照のこと
	SetForegroundWindow();
	pPopup->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	PostMessage(WM_NULL);
}

void CCybeleTrayWnd::DoLButtonDoubleClick()
{
	SendMessage(WM_COMMAND, ID_APP_ABOUT);
}

BEGIN_MESSAGE_MAP(CCybeleTrayWnd, CTrayWnd)
	ON_COMMAND(ID_APP_EXIT, &CCybeleTrayWnd::OnAppExit)
	ON_COMMAND(ID_APP_ABOUT, &CCybeleTrayWnd::OnAppAbout)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CCybeleTrayWnd メッセージ ハンドラ

void CCybeleTrayWnd::OnAppExit()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	PostMessage(WM_CLOSE);
}

void CCybeleTrayWnd::OnAppAbout()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CAboutDlg dlg;
	dlg.DoModal();
}

int CCybeleTrayWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTrayWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ここに特定な作成コードを追加してください。
	m_hIcon_main = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	NotifyIcon(NIM_ADD, m_hIcon_main, _T("Cybele"));

	return 0;
}

void CCybeleTrayWnd::OnDestroy()
{
	CTrayWnd::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
	NotifyIcon(NIM_DELETE, NULL);
}
