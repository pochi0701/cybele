#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_listen.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <signal.h>
#ifdef linux
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <error.h>
#include <cerrno>
#include <pthread.h>
#else
#include <io.h>
#endif
#include "cbl.h"
#include "cbl_tools.h"
//int volatile child_count = 0;
void    thread_process(ACCESS_INFO& ac_in);
#ifdef linux
void* accessloop(void* arg);
#else
unsigned int __stdcall   accessloop(void* arg);
#endif
SOCKET	listen_socket;	//待ち受けソケット
/////////////////////////////////////////////////////////////////////////
// HTTPサーバ 待ち受け動作部
/////////////////////////////////////////////////////////////////////////
void	server_listen(void)
{
	int    ret;
	struct sockaddr_in    saddr = {};				// サーバソケットアドレス構造体
	//struct sockaddr_in  caddr;						// クライアントソケットアドレス構造体

	//socklen_t           caddr_len = sizeof(caddr);   // クライアントソケットアドレス構造体のサイズ
	int             sock_opt_val;
	//ACCESS_INFO*        ac_in;

	// =============================
	// listenソケット生成
	// =============================
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SERROR(listen_socket)) { // ソケット生成失敗チェック
		debug_log_output("socket() error.");
		perror("socket");
		return;
	}
	// ===============================
	// SO_REUSEADDRをソケットにセット
	// ===============================
	sock_opt_val = 1;
	setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&sock_opt_val, sizeof(sock_opt_val));
	// ===========================================
	// ソケットアドレス構造体に値をセット
	// ===========================================
	memset((char*)&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons((u_short)global_param.server_port);
	// =============================
	// bind 実行
	// =============================
	ret = bind(listen_socket, (struct sockaddr*)&saddr, sizeof(saddr));
	debug_log_output("binding...");
	if (ret < 0) { // bind 失敗チェック
		debug_log_output("bind() error. ret=%d\n", ret);
		perror("bind");
		return;
	}
	// =============================
	// listen実行
	// =============================
	ret = listen(listen_socket, LISTEN_BACKLOG);
	debug_log_output("listening...");
	if (ret < 0) {  // listen失敗チェック
		debug_log_output("listen() error. ret=%d\n", ret);
		perror("listen");
		return;
	}
	// =====================
	// BLOCKING MODE設定
	// =====================
	debug_log_output("THREAD MODE START");
	//TODO:worker数で設定出来るようにすること
#ifdef linux
	pthread_t hdl[MAXTHREAD];
	for (int i = 0; i < MAXTHREAD; i++) {
		pthread_create(&hdl[i], NULL, accessloop, (void*)&listen_socket);
	}
	for (int i = 0; i < MAXTHREAD; i++) {
		ret = pthread_join(hdl[i], NULL);
	}
#else

	HANDLE thread_handle[MAXTHREAD] = {};
	unsigned int  id[MAXTHREAD] = {};

	//handle1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) accessloop , (void*)&listen_socket, 0, &id1);
	//handle2 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) accessloop , (void*)&listen_socket, 0, &id2);
	//handle3 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) accessloop , (void*)&listen_socket, 0, &id3);
	for (auto i = 0; i < MAXTHREAD; i++) {
		thread_handle[i] = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, accessloop, (void*)&listen_socket, 0, &id[i]));
	}

	WaitForMultipleObjects(MAXTHREAD, thread_handle, TRUE, INFINITE);
	for (auto i = 0; i < MAXTHREAD; i++) {
		if (thread_handle[i]) {
			CloseHandle(thread_handle[i]);
		}
	}
#endif
	sClose(listen_socket);
	return;
}
/////////////////////////////////////////////////////////////////////////
// 複数アクセス対応
/////////////////////////////////////////////////////////////////////////
#ifdef linux
void* accessloop(void* arg)
#else
unsigned int __stdcall accessloop(void* arg)
#endif
{
	int                lis_soc = *static_cast<int*>(arg);
	struct sockaddr_in caddr = {};					// クライアントソケットアドレス構造体
	socklen_t          caddr_len = sizeof(caddr);   // クライアントソケットアドレス構造体のサイズ
	char               access_host[256] = {};


	// =====================
	// メインループ
	// =====================
	ready_flag += 1;
	while (loop_flag) {
		// ====================
		// Accept待ち.
		// ====================

		debug_log_output("Waiting for a new client...");
		// 接続Socket
		auto accept_socket = accept(lis_soc, (struct sockaddr*)&caddr, &caddr_len);
		if (SERROR(accept_socket)) // accept失敗チェック
		{
			debug_log_output("accept() error. ret=%d\n", accept_socket);
			continue;           // 最初に戻る。
		}
		// 停止時Unit1.cpp で listen_socketをclosesocketし、loop_flagに0を入れています。
		if (!loop_flag) {
			debug_log_output("%s(%d) accept_socet\n", __FILE__, __LINE__);
			sClose(accept_socket);       // Socketクローズ
			break;
		}
		ACCESS_INFO ac_in = {};
		ac_in.accept_socket = (unsigned int)accept_socket;
		ac_in.access_host = access_host;
		ac_in.caddr = caddr;
		thread_process(ac_in);
	}
	//_endthreadex(0);
	return NULL;
}
/////////////////////////////////////////////////////////////////////////
// 複数アクセス対応
/////////////////////////////////////////////////////////////////////////
void thread_process(ACCESS_INFO& ac_in)
{
	int        access_check_ok;
	char       client_addr_str[32] = {};
	char       client_address[4] = {};
	char       masked_client_address[4] = {};
	char       access_host[256] = {};
	//ローカルに保存して縁を切る
	SOCKET accept_socket = (unsigned int)ac_in.accept_socket;
	struct sockaddr_in  caddr = ac_in.caddr;         // クライアントソケットアドレス構造体
	strcpy(access_host, ac_in.access_host);
	debug_log_output("\n\n=============================================================\n");
	debug_log_output("Socket Accept!!(accept_socket=%d)\n", accept_socket);
	//child_count ++;
	rand();
	// caddr 情報表示
	debug_log_output("client addr = %s\n", inet_ntoa(caddr.sin_addr));
	debug_log_output("client port = %d\n", ntohs(caddr.sin_port));
	// ==============================
	// アクセスチェック
	// ==============================
	access_check_ok = FALSE;
	// クライアントアドレス
	strncpy(static_cast<char*>(client_addr_str), inet_ntoa(caddr.sin_addr), sizeof(client_addr_str));
	// -------------------------------------------------------------------------
	// Access Allowチェック
	//  リストが空か、クライアントアドレスが、リストに一致したらＯＫとする。
	// -------------------------------------------------------------------------
	if (access_allow_list[0].flag == FALSE) { // アクセスリストが空。
		// チェックＯＫとする。
		debug_log_output("No Access Allow List. No Check.\n");
		access_check_ok = TRUE;
	}
	else {
		debug_log_output("Access Check.\n");
		// クライアントアドレス
		strncpy(client_addr_str, inet_ntoa(caddr.sin_addr), sizeof(client_addr_str) - 1);
		// client_addr_strをchar[4]に変換
		strncat(client_addr_str, ".", sizeof(client_addr_str) - 1);
		for (auto i = 0; i < 4; i++) {
			char       work1[32];
			char       work2[32];
			sentence_split(client_addr_str, '.', work1, work2);
			client_address[i] = (unsigned char)atoi(work1);
			strncpy(client_addr_str, work2, sizeof(client_addr_str));
		}
		// リストの存在する数だけループ
		for (auto i = 0; i < ACCESS_ALLOW_LIST_MAX; i++) {
			if (access_allow_list[i].flag == FALSE) { // リスト終了
				break;
			}
			// masked_client_address 生成
			masked_client_address[0] = client_address[0] & access_allow_list[i].netmask[0];
			masked_client_address[1] = client_address[1] & access_allow_list[i].netmask[1];
			masked_client_address[2] = client_address[2] & access_allow_list[i].netmask[2];
			masked_client_address[3] = client_address[3] & access_allow_list[i].netmask[3];
			// 比較実行
			if ((masked_client_address[0] == access_allow_list[i].address[0]) &&
				(masked_client_address[1] == access_allow_list[i].address[1]) &&
				(masked_client_address[2] == access_allow_list[i].address[2]) &&
				(masked_client_address[3] == access_allow_list[i].address[3]))
			{
				debug_log_output("[%d.%d.%d.%d] == [%d.%d.%d.%d] accord!!",
					masked_client_address[0], masked_client_address[1], masked_client_address[2], masked_client_address[3],
					access_allow_list[i].address[0], access_allow_list[i].address[1], access_allow_list[i].address[2], access_allow_list[i].address[3]);
				access_check_ok = TRUE;
				break;
			}
			else {
				debug_log_output("[%d.%d.%d.%d] == [%d.%d.%d.%d] discord!!",
					masked_client_address[0], masked_client_address[1], masked_client_address[2], masked_client_address[3],
					access_allow_list[i].address[0], access_allow_list[i].address[1], access_allow_list[i].address[2], access_allow_list[i].address[3]);
			}
		}
	}
	if (access_check_ok == FALSE) {
		debug_log_output("Access Denied.\n");
		debug_log_output("%s(%d) accept_socet\n", __FILE__, __LINE__);
		sClose(accept_socket);     // Socketクローズ
	}
	else {
		// HTTP鯖として、仕事実行
		server_http_process(accept_socket, access_host, client_addr_str);
		debug_log_output("HTTP process end.\n");
		debug_log_output("=============================================================\n");
		//sClose(accept_socket);
	}
}
