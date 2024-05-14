#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_main.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#define MAINVAR
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <signal.h>
#include  <sys/types.h>

#ifdef linux
#include  <unistd.h>
#include  <pwd.h>
#include  <grp.h>
#include  <sys/wait.h>
#include  <regex.h>
#include  <errno.h>
#include  <cerrno>
#else
#include  <windows.h>
//#include  <regexp.h>
//#include  "unit1.h"
#endif
#include  "cbl.h"
#include  "cbl_tools.h"
#include  "cbl_String.h"
//#include  "const.h"
#include  "cbl_db.h"
static char Application[256];
static void print_help(void);
#ifdef linux
static void daemon_init(void);
static void set_user_id(char* user, char* group);
static void setup_SIGCHLD(void);
static void catch_SIGCHLD(int signo);
#endif
// ********************************
// カレントディレクトリ名
// ********************************
wString    current_dir;
// ********************************
// カレントディレクトリ名
// ********************************
//extern DBCatalog* catalog;
// ********************************
// 実行可能フラグ
// ********************************
//int ready_flag = 0;
// ********************************
// SESSION用MAP
// ********************************
std::map<wString, wString>* session;



// **************************************************************************
// * Main Program
// **************************************************************************
#ifdef linux
int main(int argc, char* argv[])
#else
int Cybelemain(void* arg)
#endif
{
	char flag_daemon = FALSE;
	/* スレッド用パラメータ */
#ifdef linux
        strcpy(Application, argv[0]);
        if( Application[0] == '.' ){
	        std::ignore = getcwd(Application, sizeof(Application));
        }else{
            cut_after_last_character(Application,'/');
        }
#else
	IGNORE_PARAMETER(arg);
	HANDLE handle = NULL;
	DWORD dwExCode;
	unsigned int id = 0;
#endif
	// =============================================
	// 各種初期化
	// =============================================
	Initialize();

	global_param.global_param_init();
	//wString aaa = wString::jpeg_size("c:\\0001.jpg");
	//wString str = "abcdefg";
	//int res1 = dregex::match("あじのもとのあみのさん", "/あ.*ん/");
	//wString ret = "";
	//vector<wString> pat;
	//vector<wString> rep;
	//pat.push_back("/\\[\\[([^\\[]*)\\]\\]/");
	//rep.push_back("\"+webpagelink( \"$1\" )+\"");
	//pat.push_back("/{{([^{]*)}}/");
	//pat.push_back("/\\{\\{([^{]*)\\}\\}/");
	//rep.push_back("\"+image( \"$1\" )+\"");
	//int res2 = dregex::replace(&ret, "{{test.jpg|aaaa}}",pat,rep);
	// =============================================
	// オプションチェック
	// =============================================
#ifdef linux

	for (auto i = 1; i < argc; i++) {
		// ----------------------------------------------------
		// -h, --help, -v, --version:  ヘルプメッセージ
		// ----------------------------------------------------
		if ((strcmp(argv[i], "-h") == 0) ||
			(strcmp(argv[i], "--help") == 0) ||
			(strcmp(argv[i], "-v") == 0) ||
			(strcmp(argv[i], "--version") == 0))
		{
			print_help();
			printf("abort by main\n");
			abort();
			return 0;
		}
		if (strcmp(argv[i], "-d") == 0)
		{
			flag_daemon = TRUE;
		}
	}
#endif
	// =============================================
	// コンフィグファイル読む(cbl.conf)
	// =============================================
	global_param.config_file_read();


	// デーモンモードについては、コマンドラインパラメータを優先
	if (flag_daemon)
	{
		global_param.flag_daemon = TRUE;
	}
#ifdef linux
	// ======================
	// = SetUID 実行
	// ======================
	set_user_id(global_param.exec_user, global_param.exec_group);
#endif
	// =======================
	// Debug Log 出力開始
	// =======================
	if (global_param.flag_debug_log_output == TRUE) {
#ifdef linux
		printf("debug log output start..\n");
#endif
		debug_log_output("\n%s boot up.", SERVER_NAME);
		debug_log_output("debug log output start..\n");
	}
	global_param.config_sanity_check();
	// =================
	// daemon化する。
	// =================
#ifdef linux
	if (global_param.flag_daemon == TRUE) {
		printf("Daemoning....\n");
		daemon_init();
	}
	else {
		signal(SIGPIPE, SIG_IGN);
		signal(SIGCHLD, SIG_IGN);
	}
	// ==========================================
	// 子プロセスシグナル受信準備。forkに必要
	// ==========================================
	setup_SIGCHLD();
#endif
	// ==================================
	// Server自動検出を使うばあい、
	// Server Detect部をForkして実行
	// ==================================
	//if (global_param.flag_auto_detect == TRUE) {
    // 以下子プロセス部
#ifdef linux
	int pid = fork();
	if (pid < 0) // fork失敗チェック
	{
		perror("fork");
		exit(1);
	}
	if (pid == 0)
	{
		// 以下子プロセス部
		batch(NULL);
		exit(0);
	}
#else
	//かえって遅い気がします。
	//*HANDLE handle = _beginthreadex(NULL,0,functionptr,(void*)param,0,NULL);
	//handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) batch , NULL, NULL , &id);
	handle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, batch, NULL, 0, &id));
	//標準以下
	//SetThreadPriority(handle,THREAD_PRIORITY_BELOW_NORMAL);
#endif
	//}

	// =======================
	// HTTP Server仕事開始
	// =======================
	server_listen();
#ifndef linux
	if (handle != 0) {
		//server_detect待ち
		if (WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0) {
			//強制的に終わらす
			TerminateThread(handle, id);
			while (1) {
				GetExitCodeThread(handle, &dwExCode);
				if (dwExCode != STILL_ACTIVE) {
					break;
				}
			}
			return -1;
		}
		CloseHandle(handle);
	}
	Finalize();
	//ExitThread(TRUE);
#else
	Finalize();
#endif
	return 0;
}
// **************************************************************************
//
// **************************************************************************

//アクセスできたら０、できなかったら-1
int Initialize(void)
{
	//wString::wStringInit();
	wString     curdir;
#ifdef linux
	wString tmp(Application);
	curdir = tmp.trim();
#else
	WSADATA wsa;
	WORD version = MAKEWORD(2, 0);
	WSAStartup(version, &wsa);

	// arg0 相当のファイル名取得
	int ret = ::GetModuleFileName(NULL, Application, sizeof(Application));
	if (ret == 0)
	{
		perror("Module Name Not Found.");
	}

	//char Application[256];
	wString apName = wString::linux_file_name(Application);
	curdir = wString::extract_file_dir(apName).trim().nkfcnv("Sw");
#endif
	//curdir.trim();
	current_dir = curdir;
	//ショートカットから呼ばれたときのパッチ。だましですよー
#ifdef linux
	std::ignore = chdir(current_dir.c_str());
#else
	SetCurrentDirectory(current_dir.nkfcnv("Ws").c_str());
#endif
	/* TODO : work_rootに従って削除すること */
	if (!wString::directory_exists(curdir + DELIMITER"work")) {
		wString::create_dir(current_dir + DELIMITER"work");
	}
	else {
		//workの中のファイルを消す
		wString list;
		list = wString::enum_folder(current_dir + DELIMITER"work");
		int count = list.lines();
		for (auto i = 0; i < count; i++) {
			wString::delete_file(list.get_list_string(i));
		}
	}
	//セッション用MAP作成
	session = new map<wString, wString>;
	//DB初期化
	catalog = new DBCatalog();

	//memset(&global_param, 0, sizeof(global_param));
	// 構造体まとめて初期化
	memset(access_allow_list, 0, sizeof(access_allow_list));
	return 0;
}
// **************************************************************************
// アクセスできたら０、できなかったら-1
// **************************************************************************
int Finalize(void)
{
	delete catalog;
	//map<wString, wString>::iterator it;
	//TODO:動作不明
	for (auto it = session->begin(); it != session->end(); it++) {
		wString f = it->first;
		wString s = it->second;
	}
	delete session;

#ifndef linux
	//WINSOCK DLL停止
	WSACleanup();
#endif
	//wString::wStringEnd();
	return 0;
}
// **************************************************************************
// Helpメッセージ出力
// **************************************************************************
static void print_help(void)
{
#ifdef linux
	printf("%s -- %s\n\n", SERVER_NAME, SERVER_DETAIL);
	printf("Usase: cybele [options]\n");
	printf("Options:\n");
	printf(" -h, --help\tprint this message.\n");
	printf("\n");
#endif
}
#ifdef linux
// **************************************************************************
// デーモン化する。
// **************************************************************************
static void daemon_init(void)
{
	int       pid;
	// 標準入出力／標準エラー出力を閉じる
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	pid = fork();
	if (pid != 0) {
		exit(0);
	}
	setsid();
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	pid = fork();
	if (pid != 0) {
		exit(0);
	}
	return;
}
// **************************************************************************
// 指定されたUID/GIDに変更する。root起動されたときのみ有効
// **************************************************************************
static void set_user_id(char* user, char* group)
{
	struct passwd* user_passwd;
	struct group* user_group;
	// rootかチェック
	if (getuid() != 0) {
		return;
	}
	// userが指定されているかチェック
	if (strlen(user) <= 0) {
		return;
	}
	// userIDをGet.
	user_passwd = getpwnam((const char*)user);
	if (user_passwd == NULL)
	{
		return;
	}
	// groupはオプション。指定があれば設定する。
	if (strlen(group) > 0)
	{
		user_group = getgrnam((const char*)group);
		if (user_group == NULL) {
			return;
		}
		// setgid実行
		std::ignore = setgid(user_group->gr_gid);
	}
	// rootで両方の情報をとっておき、かつsetgidを先にしないと失敗する
	// setuid実行
	std::ignore = setuid(user_passwd->pw_uid);
	return;
}
// **************************************************************************
// 子プロセスが終了したときにシグナルを受け取る設定
// **************************************************************************
static void setup_SIGCHLD(void)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	// SIGCHLD発生時にcatch_SIGCHLD()を実行
	act.sa_handler = catch_SIGCHLD;
	// catch_SIGCHLD()中の追加シグナルマスクなし
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NOCLDSTOP | SA_RESTART;
	sigaction(SIGCHLD, &act, NULL);
	return;
}
// **************************************************************************
// シグナル駆動部。SIGCHLDを受け取ると呼ばれる。
// **************************************************************************
static void catch_SIGCHLD(int signo)
{
	pid_t child_pid = 0;
	int child_ret;
	debug_log_output("catch SIGCHLD!!(signo=%d)\n", signo);
	// すべての終了している子プロセスに対してwaitpid()を呼ぶ
	while (1)
	{
		child_pid = waitpid(-1, &child_ret, WNOHANG);
		if (child_pid <= 0)	// 全部waitpidが終わると、-1が戻る
		{
			break;
		}
		//debug_log_output("catch_SIGCHILD waitpid()=%d, child_count = %d\n"
		//, child_pid, child_count);
	}
	debug_log_output("catch SIGCHLD end.\n");
	return;
}
#endif
