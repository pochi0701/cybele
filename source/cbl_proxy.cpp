#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_proxy.cpp
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
//#include <unistd.h>
#include <sys/types.h>
#ifdef linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <arpa/inet.h>
#else
#include <io.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
//#include <dirent.h>
#include "cbl.h"
#include "cbl_tools.h"
#include "cbl_String.h"
#include "define.h"
//#include "const.h"
//#define MAX_LINE 100           /* 記憶する、HTTPヘッダの最大行数 */
//#define LINE_BUF_SIZE (4096)     /* 行バッファ */
const char* HTTP_RECV_CONTENT_LENGTH = "Content-Length: ";
extern int line_receive (int accept_socket, char* line_buf_p, int line_max);
//extern int line_receive(const int accept_socket, wString& line_buf_p);
// バッファリングしながら in_fd から out_fd へ データを転送
int copy_all (SOCKET in_fd, SOCKET out_fd);
// URIからヘッダを生成して送信
int getHeader (SOCKET sock, wString& lines, int& content_is_html, int& t_content_length);
int cirtUserAgent (char* ip, char* agents);
char agent[64] = {};

#ifndef linux
// **************************************************************************
char* strcasestr (const char* p1, const char* p2)
{
	size_t len;
	len = strlen (p2);
	if (len == 0) return (char*)p1;

	while (*p1) {
		if (!strncasecmp (p1, p2, len)) {
			return (char*)p1;
		}
		p1++;
	}
	return NULL;
}
#endif
//プロクシーからデータ返す
int HTTP_RECV_INFO::http_proxy_response (SOCKET accept_socket)
{
	//許されてない場合終了
	if (!global_param.flag_allow_proxy) {
		return -1;
	}
	//user agent作成
	if (*agent == 0) {
		cirtUserAgent (global_param.global_ip, agent);
	}
	//URIを送出
	wString send_http_header_buf;
	SOCKET sock = send_header ();
	if (SERROR (sock)) {
		return -1;
	}
	//ヘッダ読み込み
	//char line_buf[MAX_LINE][LINE_BUF_SIZE];
	wString lines;
	int  content_is_html = 0;
	int  t_content_length = 0;

	int  line = getHeader (sock, lines, content_is_html, t_content_length);
	if (line < 0) {
		sClose (sock);
		return -1;
	}
	//for( int i = 0 ; i < line ; i++ ){
	//    sprintf( line_buf[i],"%s\r\n",lines.GetListString(i).c_str());
	//}
	// 以下のパターン変更
	// <a href="http://some_host/some_url"
	// <img src="http://some_host/some_url"
	// <script src="http://some_host/some_url"
	// <link rel="http://some_host/some_url"
	// <form action="http://some_host/some_url"
	if (content_is_html) {

		wString wb;
		//char work_buf[LINE_BUF_SIZE + 10];
		//char work[LINE_BUF_SIZE + 10];
		//ヘッダ送信
		for (int i = 0; i < line; i++) {
			//Content-Length:送らない
			wString ln = lines.get_list_string (i) + "\r\n";
			//if (!strncasecmp(line_buf[i], HTTP_RECV_CONTENT_LENGTH, strlen(HTTP_RECV_CONTENT_LENGTH))){
			if (!strncasecmp (ln.c_str (), HTTP_RECV_CONTENT_LENGTH, strlen (HTTP_RECV_CONTENT_LENGTH))) {
				continue;
			}
			send (accept_socket, ln.c_str (), ln.length (), 0);
		}
		//debug_log_output("sent header");
		//write(accept_socket, "debug:--\n", strlen("debug:--\n"));
		while (1) {
			//１行読み込み
			//memset(work_buf, 0, LINE_BUF_SIZE);
			int len = wb.line_receive (sock);
			//strcpy(work_buf,wb.c_str());
			//int len = line_receive(sock, work_buf, LINE_BUF_SIZE);
			//EOFの場合は-1,空行の場合は0
			if (len < 0) {
				debug_log_output ("proxy exit %s(%d)=%d", __FILE__, __LINE__, len);
				break;
			}

			//debug_log_output("recv html: '%s' len = %d", wb.c_str(), len);
			wb += "\r\n";
			//strcpy( work_buf+len,"\r\n");
			/*
			* HTML中に href="..." があったら、プロクシを経由するように
			* たとえば <a href="/-.-http://www.yahoo.co.jp/"> のように変換する
			*
			* 問題点:
			*   タグの途中に改行があると失敗するだろう.
			*   面倒なのでたいした置換はしていない
			*/
			auto ptr = wb.c_str ();
			char* new_ptr;
			while (1) {
				ptr = strcasestr (ptr, " href");
				if (ptr == NULL) break;
				//"<a"がある
				new_ptr = strstr (ptr, "\"http://");
				if (new_ptr == NULL) {
					new_ptr = strstr (ptr, "'http://");
					if (new_ptr == NULL) {
						break;
					}
				}
				//"http://"がある
				int pos = (int)(new_ptr - wb.c_str () + 1);
				wb = wb.insert (pos, "/-.-");
				ptr++;
				//strncpy( work, work_buf,ptr-work_buf);
				//strcpy( work+(ptr-work_buf), "/-.-" );
				//strcat( work, ptr );
				//strcpy( work_buf, work );
			}
			/*
			* HTML中に SRC="..." があったら、プロクシを経由するように変換する
			*
			* 問題点:
			*   タグの途中に改行があると失敗するだろう.
			*/
			ptr = wb.c_str ();
			while (1) {
				ptr = strcasestr (ptr, " src");
				if (ptr == NULL) break;
				//"src"がある
				new_ptr = strstr (ptr, "\"http://");
				if (new_ptr == NULL) {
					new_ptr = strstr (ptr, "'http://");
					if (new_ptr == NULL) {
						break;
					}
				}
				//ptr = strstr( ptr, "\"http://");
				//if( ptr == NULL ) break;
				//"http://"がある
				int pos = (int)(new_ptr - wb.c_str () + 1);
				wb = wb.insert (pos, "/-.-");
				ptr++;
				//strncpy( work, work_buf,ptr-work_buf);
				//strcpy( work+(ptr-work_buf), "/-.-" );
				//strcat( work, ptr );
				//strcpy( work_buf, work );
			}
			/*
			* HTML中に action="..." があったら、プロクシを経由するように変換する
			*
			* 問題点:
			*   タグの途中に改行があると失敗するだろう.
			*/
			ptr = wb.c_str ();
			while (1) {
				ptr = strcasestr (ptr, " action=");
				if (ptr == NULL) break;
				//"action="がある
				new_ptr = strstr (ptr, "\"http://");
				if (new_ptr == NULL) {
					new_ptr = strstr (ptr, "'http://");
					if (new_ptr == NULL) {
						break;
					}
				}
				//"http://"がある
				int pos = (int)(new_ptr - wb.c_str () + 1);
				wb = wb.insert (pos, "/-.-");
				ptr++;
				//strncpy( work, work_buf,ptr-work_buf);
				//strcpy( work+(ptr-work_buf), "/-.-" );
				//strcat( work, ptr );
				//strcpy( work_buf, work );
			}
			//send( accept_socket , work_buf , strlen( work_buf ) , 0 );
			//debug_log_output("sent html: %s", work_buf);
			send (accept_socket, wb.c_str (), wb.length (), 0);
			debug_log_output ("sent html: %s", wb.c_str ());
		}
		//画像等
	}
	else {
		//for (int i=0; i<line; i++) {
		//    send( accept_socket , line_buf[i] , strlen( line_buf[i] ) , 0 );
		//}
		send (accept_socket, lines.c_str (), lines.length (), 0);
		copy_all (sock, accept_socket);
	}
	sClose (sock);
	return 0;
}
//URIから相手先へ接続しヘッダ送信
SOCKET HTTP_RECV_INFO::send_header ()
{
	wString send_http_header_buf;
	wString p_uri_string;
	wString p_target_host_name;      //ホスト名
	wString base_url;                //ホスト名抜いたURL
	wString p_auth;                  //認証文字列
	int port = 80;                   //ポート(省略値:80)
	int num;
	p_uri_string = request_uri;

	//Proxyパターンである場合
	//  /-.-http://user:password@www.make-it.co.jp:8080/index.php?query=1
	if (p_uri_string.find ("/-.-http://") == 0) {
		p_uri_string = p_uri_string.substr (11);
		//通常の場合(他で流用するため)
	}
	else if (p_uri_string.find ("http://") == 0) {
		p_uri_string = p_uri_string.substr (7);
	}
	else {
		return INVALID_SOCKET;
	}
	//user:password@www.make-it.co.jp:8080/index.php?query=1

	//->index.php?query=1
	int poss = p_uri_string.find ('/');
	if (poss != wString::npos) {
		base_url = p_uri_string.substr (poss);
		p_target_host_name = p_uri_string.substr (0, poss);
	}
	else {
		base_url = "/";
		p_target_host_name = p_uri_string;
	}
	//->
	//認証部分切る
	num = p_target_host_name.find ('@');
	if (num >= 0) {
		// there is a user name.
		p_auth = p_target_host_name.substr (0, num);
		p_target_host_name = p_target_host_name.substr (num + 1);
	}
	//ポート取る
	num = p_target_host_name.find (':');
	if (num >= 0) {
		port = atoi (p_target_host_name.substr (num + 1).c_str ());
		p_target_host_name = p_target_host_name.substr (0, num);
	}
	debug_log_output ("header:target_host_name: %s", static_cast<char*>(p_target_host_name.c_str ()));
	debug_log_output ("header:base_url: %s", base_url.c_str ());
	debug_log_output ("header:port: %d", port);
	debug_log_output ("header:authenticate: %s", p_auth.length () ? p_auth.c_str () : "NULL");

	//出力バッファ作成
	//出力バッファ 1:GET 2:HEAD
	const char* GETHEAD[4] = { "","GET","HEAD","" };
	//GET/HEAD
	send_http_header_buf.sprintf ("%s %s HTTP/1.0\r\n", GETHEAD[(int)method], base_url.c_str ());
	send_http_header_buf.cat_sprintf ("Host: %s\r\n", p_target_host_name.c_str ());
	//User Agent設定
	if (global_param.user_agent_proxy_override[0]) {
		send_http_header_buf.cat_sprintf ("User-agent: %s%s\r\n", global_param.user_agent_proxy_override, MACADDR);
	}
	else {
		// UserAgent計算
		if (user_agent[0]) {
			if (*agent) {
				send_http_header_buf.cat_sprintf ("User-agent: " USERAGENT ";%s%s%s\r\n", user_agent, agent, MACADDR);
			}
			else {
				send_http_header_buf.cat_sprintf ("User-agent: " USERAGENT ";%s%s\r\n", user_agent, MACADDR);
			}
		}
		else {
			send_http_header_buf.cat_sprintf ("User-agent: " USERAGENT ";%s%s\r\n", SERVER_NAME, MACADDR);
		}
	}
	//
	send_http_header_buf.cat_sprintf ("Accept: */*\r\n");
	send_http_header_buf.cat_sprintf ("Connection: close\r\n");
	//TODO ACCEPT BYTE必要
	//RANGEあり
	if (range_start_pos) {
		send_http_header_buf.cat_sprintf ("Range: bytes=");
		send_http_header_buf.cat_sprintf ("%jd-", range_start_pos);
		//ENDあり
		if (range_end_pos) {
			send_http_header_buf.cat_sprintf ("%jd", range_end_pos);
		}
		send_http_header_buf.cat_sprintf ("\r\n");
	}
	//認証
	if (p_auth.length () != 0) {
		send_http_header_buf.cat_sprintf ("Authorization: Basic %s\r\n", static_cast<char*>(p_auth.base64 ().c_str ()));
	}
	send_http_header_buf.cat_sprintf ("\r\n");



	SOCKET sock = wString::sock_connect (p_target_host_name, port);
	//Socket生成エラー
	if (SERROR (sock)) {
		debug_log_output ("error: %s create sock: %d", strerror (errno), sock);
		return INVALID_SOCKET;//-1;
	}
	//ヘッダ送信
	send (sock, send_http_header_buf.c_str (), send_http_header_buf.length (), 0);
	debug_log_output ("================= send to proxy\n");
	debug_log_output ("%s", send_http_header_buf.c_str ());
	debug_log_output ("=================\n");
	return sock;
}
int getHeader (SOCKET sock, wString& lines, int& content_is_html, int& t_content_length)
{
	const char* HTTP_RECV_CONTENT_TYPE = "Content-type: text/html";
	const char* HTTP_RECV_LOCATION = "Location: ";
	const char* HTTP_RECV_LOCATION2 = "Content-Location: ";
	int line;
	wString    wb;
	int num;
	lines.clear ();
	//TODO ここが４０４や４００の可能性あり
//    for (line = 0; line < MAX_LINE; line ++) {
	for (line = 0;; line++) {
		int len = wb.line_receive (sock);
		if (len < 0) {
			return -1;
		}
		else if (len > 0 && line == 0) {
			char* sptr = strstr (wb.c_str (), " ");
			if (sptr == NULL) {
				return -1;
			}
			// 空白の次の文字列
			num = atoi (&sptr[1]);
			switch (num) {
			case 200:
				break;
			case 301:
				//TODO::ヘッダ読んでなんかするように
				wb.line_receive (sock);
				break;
			case 302:
				break;
			case 404:
				return -1;
			default:
				return -1;
			}
		}
		debug_log_output ("recv html: '%s' len = %d", wb.c_str (), len);
		//CONTENT TYPEあればHTML/JS/CSS等
		if (!strncasecmp (wb.c_str (), HTTP_RECV_CONTENT_TYPE, strlen (HTTP_RECV_CONTENT_TYPE))) {
			content_is_html = 1;
			//content_length取得
		}
		else if (!strncasecmp (wb.c_str (), HTTP_RECV_CONTENT_LENGTH, strlen (HTTP_RECV_CONTENT_LENGTH))) {
			t_content_length = strtoull (wb.c_str () + strlen (HTTP_RECV_CONTENT_LENGTH), NULL, 0);
			//location 取得
		}
		else if (!strncasecmp (wb.c_str (), HTTP_RECV_LOCATION, strlen (HTTP_RECV_LOCATION))) {
			wb.sprintf ("Location: /-.-%s", wb.c_str () + strlen (HTTP_RECV_LOCATION));
			//location2取得
		}
		else if (!strncasecmp (wb.c_str (), HTTP_RECV_LOCATION2, strlen (HTTP_RECV_LOCATION2))) {
			wb.sprintf ("Content-Location: /-.-%s", wb.c_str () + strlen (HTTP_RECV_LOCATION2));
		}
		//AnsiStringにして送出
		lines.Add (wb);
		if (len <= 2) {
			line++;
			break;
		}
	}
	return line;
}


////////////////////////////////////////////////////////////////////////////////
//ＳＯＣＫＥＴ間コピー
// copy_all(int in SOCKET, int out SOCKET )
// return  0:normal end
//        -1:error end
#define BUFFERSIZE 4096
int copy_all (SOCKET in_fd, SOCKET out_fd)
{
	char buf[BUFFERSIZE];
	// ================
	// 実体転送開始
	// ================
	while (1) {
		// ファイルからデータを読み込む。必ず読める前提
		auto rlen = recv (in_fd, buf, BUFFERSIZE, 0);
		//read end
		if (rlen == 0) {
			debug_log_output ("copy end");
			return 0;
			//read error
		}
		else if (SERROR (rlen)) {
			debug_log_output ("read error error=%s\n", strerror (errno));
			return (-1);
		}
		//読み込み正常終了
		// SOCKET にデータを送信
		auto wlen = send (out_fd, buf, rlen, 0);

		//write error
		if (rlen != wlen) {
			debug_log_output ("send() error.%d %s\n", errno, strerror (errno));
			return (-1);
		}
		//書き込み更新
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int cirtUserAgent (char* ip, char* agents)
{
	unsigned long iplong = htonl (inet_addr (ip));
	// 定数
	unsigned long salt = 0x43253215;
	int sat = 18;//salt2
	int sam = 77;//salt3
	// IP取得 XOR
	iplong ^= salt;
	// bit取得
	int n = rand () % 32;
	int b = (iplong & (1 << n)) ? 1 : 0;
	// 種生成
	unsigned int r = (rand () % 0xfffe) + 1;
	unsigned int x = (r * r) % sam;
	unsigned int y = ((unsigned int)(pow (sat, b) * r)) % sam;
	//n番目,X,Y
	sprintf (agents, "CB[%d.%u.%u]", n, x, y);
	return 1;
}

