﻿#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_db.cpp
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
#ifdef linux
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <error.h>
#include <cerrno>
#else
#include <io.h>
#endif
#include <sys/stat.h>
//#include <dirent.h>
#include "cbl.h"
#include "cbl_tools.h"
#include "cbl_String.h"
#include "define.h"
#include "TinyJS.h"
/// <summary>
/// サーバ HTTP処理部
/// </summary>
/// <param name="accept_socket">アクセプトソケット</param>
/// <param name="access_host">ホストIPアドレス</param>
/// <param name="client_addr_str">未使用</param>
void server_http_process (SOCKET accept_socket, char* access_host, char* client_addr_str)
{
	IGNORE_PARAMETER (client_addr_str);
	FILETYPES		result;
	HTTP_RECV_INFO	http_recv_info;	//HTTP受信情報保存構造体
	int             ret;
	time_t nowtime;         //現在時刻
	time (&nowtime);
	//    user_info_list.list_num = 0;
	// 2004/08/13 Add end
	http_recv_info.clear ();
	// ----------------------------------------
	// HTTP リクエストヘッダ受信
	// ----------------------------------------
	//debug_log_output("HTTP Header receive start!\n");
	ret = http_recv_info.http_header_receive (accept_socket);
	if (ret != 0) { // エラーチェック
		// エラーメッセージ
		debug_log_output ("%s(%d):http_header_receive() Error. result=%d\n", __FILE__, __LINE__, ret);
		sClose (accept_socket);
		return;
	}
	// ==========================
	// = User-Agent チェック
	// ==========================
#if 0
	if (strlen (allow_user_agent[0].user_agent) == 0) { // User-Agnet指示が無し。
		debug_log_output ("user_agent: allow_user_agent No List. All Allow.");
		flag_allow_user_agent_check = 1; // OK
	}
	else {
		// User-Agent チェック実行
		//TODO iはかきつぶしちゃいけないよ。
		for (int i = 0; i < ALLOW_USER_AGENT_LIST_MAX; i++) {
			if (strlen (allow_user_agent[i].user_agent) == 0) {
				break;
			}
			// 一致チェック
			debug_log_output ("user_agent: Check[%d] '%s' in '%s'", i, allow_user_agent[i].user_agent, http_recv_info.user_agent);
			if (strstr (http_recv_info.user_agent, allow_user_agent[i].user_agent) != NULL) {
				debug_log_output ("user_agent: '%s' OK.", allow_user_agent[i].user_agent);
				flag_allow_user_agent_check = 1; // 一致。OK
				break;
			}
		}
	}
	// User-Agentチェック NGならば、ソケットクローズ。終了。
	if (flag_allow_user_agent_check == 0) {
		debug_log_output ("allow_user_agent check. Deny. Socket close.");
		// ソケットクローズ
		//close(accept_socket);
		// 2004/07/21 Delete end
		// 2004/07/20 Update start
		//		return;
		// User-Agent NGでも最近アクセスできたIPと同一ならOK
		// PCからの動画再生の場合、User-Agentがプレイヤー名（Windows-Media-Playerなど）に
		// なってしまうので、最近アクセスできたIPを保持しておき、そのIPならばUser-Agentが
		// cbl.confに記述されていないものでもアクセスを認める
		if ((strlen (access_host) == 0) ||
			(strstr (http_recv_info.recv_host, access_host) == NULL)) {
			debug_log_output ("%s(%d) accept_socet\n", __FILE__, __LINE__);
			sClose (accept_socket);
			return;
		}
		// 2004/07/20 Update end
	}
#endif

	if (path_sanitize (http_recv_info.recv_uri, sizeof (http_recv_info.recv_uri)) == NULL) {
		// BAD REQUEST!
		http_recv_info.http_not_found_response (accept_socket);
		debug_log_output ("%s(%d) BAD REQUEST. Path sanitize %s\n", __FILE__, __LINE__,http_recv_info.recv_uri);
		sClose(accept_socket);
		return;
	}
	//PROXY判定
	if (!strncmp (http_recv_info.recv_uri, "/-.-", 4)) {
		// proxy
		if (http_recv_info.http_proxy_response (accept_socket) < 0) {
			http_recv_info.http_not_found_response (accept_socket);
		}
		// ソケットクローズ
		//debug_log_output("%s(%d) accept_socet\n", __FILE__, __LINE__);
		sClose (accept_socket);
		return;
	}

	//debug_log_output("sanitized recv_uri: %s", http_recv_info.recv_uri);

	// ============================
	// ファイルチェック
	//  種類に応じて分岐
	// ============================
	result = http_recv_info.http_file_check ();
	// ディレクトリだが終端が '/' ではない
	if (result == FILETYPES::_OPENDIR) { 
		char buffer[FILENAME_MAX];
		sprintf (buffer, "%s/", http_recv_info.recv_uri);
		http_recv_info.http_redirect_response (accept_socket, buffer);
	}
	// file?action=/skin_root/skin_name/action.jss
	else if (*http_recv_info.action) {
		// ----------------------------------------
		// jss実行
		// ----------------------------------------
		debug_log_output ("%s start!\n", http_recv_info.action);
		// send_filename = skin_root/skin_name/action 
		wString skin_filename;
		skin_filename += global_param.skin_root;
		skin_filename += global_param.skin_name;
		if (!skin_filename.ends_with (DELIMITER)) {
			skin_filename += DELIMITER;
		}
		skin_filename += http_recv_info.action;
		strcpy (http_recv_info.send_filename, skin_filename.c_str ());

		for (unsigned int i = 0; i < strlen (http_recv_info.request_uri); i++) {
			if (http_recv_info.request_uri[i] == '?') {
				http_recv_info.request_uri[i] = 0;
				break;
			}
		}
		skin_filename.sprintf ("%s?url=%s", http_recv_info.action, http_recv_info.request_uri);
		strcpy (http_recv_info.request_uri, skin_filename.c_str ());
		http_recv_info.http_cgi_response (accept_socket);
		//debug_log_output ("%s end!\n", http_recv_info.action);
		debug_log_output ("HTTP Process action done. From %s:%s\n", access_host, http_recv_info.recv_uri);
		return;
	}
	// ファイルが見つからない
	else if (result < FILETYPES::_FILE) { 
		http_recv_info.http_not_found_response (accept_socket);
		return;
	}
	// ファイル実体ならば、実体転送。
	else if (result == FILETYPES::_FILE) { 
		// ----------------------------------------
		// ファイルの実体
		// HTTPリクエストヘッダに従ってデータを返信。
		// ----------------------------------------
		http_recv_info.http_file_response (accept_socket);
	}
	else if (result == FILETYPES::_CGI) {
		// ---------------------------------------------
		// cgiファイル
		// cgiを実行して結果を返信
		// ---------------------------------------------
		//debug_log_output("HTTP CGI response start!\n");
		http_recv_info.http_cgi_response (accept_socket);
		debug_log_output ("HTTP Process CGI done. From %s:%s\n", access_host, http_recv_info.recv_uri);

		//debug_log_output("HTTP CGI response end!\n");
		return;
	}
	else {
		// ----------------------------------------
		// ディレクトリ 内にindex.html or index.htm or index.cgi or index.mdがあったら、そちらを表示する。
		// ----------------------------------------
		//request_uriの最後が?なら強制的にmenu.jssを起動。それ以外は通常のファイル処理
		//request_uriの設定
		//?以降のパラメータがあるなら付け替える。なければsend_filenameをつける
		wString tmp (http_recv_info.request_uri);
		int pos = tmp.Pos ("?");
		if (pos >= 0) {
			strcpy (http_recv_info.recv_uri, "/menu.jss");
			tmp = tmp.substr (pos + 1, tmp.length () - pos - 1);
			if (tmp.length ()) {
				sprintf (http_recv_info.request_uri, "/menu.jss?%s", tmp.c_str ());
			}
			else {
				sprintf (http_recv_info.request_uri, "/menu.jss?root=%s", http_recv_info.send_filename);
			}
			//send file nameの設定
			sprintf (http_recv_info.send_filename, "%s%smenu.jss", global_param.skin_root, global_param.skin_name);
			http_recv_info.http_cgi_response (accept_socket);
			debug_log_output ("File menu convert done.\n");
			return;
		}
		// 通常の処理
		else {
			result = http_recv_info.http_index ();
			if (result == FILETYPES::_FILE) {
				// ----------------------------------------
				// ファイルの実体
				// HTTPリクエストヘッダに従ってデータを返信。
				// ----------------------------------------
				//debug_log_output ("HTTP response start!\n");
				http_recv_info.http_file_response (accept_socket);
				//debug_log_output ("HTTP response end!\n");
			}
			else if (result == FILETYPES::_CGI) {
				//debug_log_output ("HTTP CGI response start! %d\n", accept_socket);
				http_recv_info.http_cgi_response (accept_socket);
				//debug_log_output ("HTTP CGI response end!\n");
				debug_log_output ("HTTP Process CGI done. From %s:%s\n", access_host, http_recv_info.recv_uri);
				return;
			}
			else {
				debug_log_output ("%s(%d):BAD REQUEST!", __FILE__, __LINE__);

				http_recv_info.http_not_found_response (accept_socket);
				return;
			}
		}
	}
	if ((strlen (access_host) == 0) || (strstr (http_recv_info.recv_host, access_host) == NULL)) {
		//アクセスしてきたIPを保持しておく
		strncpy (access_host, http_recv_info.recv_host, sizeof (http_recv_info.recv_host) - 1);
		cut_after_character (access_host, ':');
	}
	//sClose(accept_socket);
	debug_log_output ("HTTP Process done. From %s:%s\n", access_host, http_recv_info.recv_uri);
	sClose(accept_socket);
	return;
}
/////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 各種indexファイルのアクセス
/// html,htm,php,jssの順
/// </summary>
/// <param name=""></param>
/// <returns></returns>
FILETYPES HTTP_RECV_INFO::http_index (void)
{
	char       file_extension[16];

	wString read_filename;
	//Path Normalize.
	wString document_path = send_filename;
	if (!document_path.ends_with (DELIMITER)) {
		document_path += DELIMITER;
	}
	// ----------------------------------------------
	// document_root/index.* のフルパス生成
	// ----------------------------------------------
	read_filename.sprintf ("%sindex.html", document_path.c_str ());
	if (wString::file_exists(read_filename)) {
		strcat (request_uri, "index.html");
		strcpy (send_filename, read_filename.c_str ());
		// ファイルの拡張子より、Content-type を決定
		filename_to_extension (send_filename, file_extension, sizeof (file_extension));
		//debug_log_output ("send_filename='%s', file_extension='%s'\n", send_filename, file_extension);
		// 拡張子から、mime_typeを導く。
		MIME_LIST_T::check_file_extension_to_mime_type (file_extension, mime_type, sizeof (mime_type));
		return FILETYPES::_FILE;
	}
	read_filename.sprintf ("%sindex.htm", document_path.c_str ());
	if (wString::file_exists(read_filename)) {
		strcat (request_uri, "index.htm");
		strcpy (send_filename, read_filename.c_str ());
		// ファイルの拡張子より、Content-type を決定
		filename_to_extension (send_filename, file_extension, sizeof (file_extension));
		//debug_log_output ("send_filename='%s', file_extension='%s'\n", send_filename, file_extension);
		// 拡張子から、mime_typeを導く。
		MIME_LIST_T::check_file_extension_to_mime_type (file_extension, mime_type, sizeof (mime_type));
		return FILETYPES::_FILE;
	}
	read_filename.sprintf ("%sindex.php", document_path.c_str ());
	if (wString::file_exists(read_filename)) {
		strcat (request_uri, "index.php");
		strcpy (send_filename, read_filename.c_str ());
		// ファイルの拡張子より、Content-type を決定
		filename_to_extension (send_filename, file_extension, sizeof (file_extension));
		//debug_log_output ("send_filename='%s', file_extension='%s'\n", send_filename, file_extension);
		// 拡張子から、mime_typeを導く。
		MIME_LIST_T::check_file_extension_to_mime_type (file_extension, mime_type, sizeof (mime_type));
		return FILETYPES::_CGI;
	}
	read_filename.sprintf ("%sindex.jss", document_path.c_str ());
	if (wString::file_exists(read_filename)) {
		strcat (request_uri, "index.jss");
		strcpy (send_filename, read_filename.c_str ());
		// ファイルの拡張子より、Content-type を決定
		filename_to_extension (send_filename, file_extension, sizeof (file_extension));
		//debug_log_output ("send_filename='%s', file_extension='%s'\n", send_filename, file_extension);
		// 拡張子から、mime_typeを導く。
		MIME_LIST_T::check_file_extension_to_mime_type (file_extension, mime_type, sizeof (mime_type));
		return FILETYPES::_CGI;
	}
	return FILETYPES::_DIR;
}
/// <summary>
/// HTTPヘッダを受信して解析する。
/// GET,HEAD,POST
/// URIとuser_agent、Range、Hostを解析。
/// URIは、URIデコード
/// </summary>
/// <param name="accept_socket">入力ソケット</param>
/// <returns>0:正常終了,0以外:エラー</returns>
int HTTP_RECV_INFO::http_header_receive (SOCKET accept_socket)
{
	int result = 0;
	wString     wb1;
	wString     wb2;
	wString 	split1;
	wString 	split2;
	wString     line;
	int		i;
	// ================================
	// １行づつ HTTPヘッダを受信
	// ================================
	for (i = 0;; i++) {
		// 1行受信 実行。
		auto recv_len = line.line_receive (accept_socket);
		// 受信した内容をチェック。
		if (recv_len == 0) { // 空行検知。ヘッダ受信終了。
			break;
		}
		else if (recv_len < 0) { // 受信失敗
			return (-1);
		}
		// debug. 受信したヘッダ表示
		//debug_log_output("'%s'(%d byte)\n", line.c_str(), recv_len);
		// --------------------------
		// GETメッセージチェック
		// --------------------------
		if (i == 0) { // １行目のみチェック
			//debug_log_output("%d:URI Check start.'%s'\n", accept_socket, line.c_str());
			// GETある？
			if (line.find ("GET") != wString::npos) {
				method = QUERY_METHOD::GET;
			}
			else if (line.find ("HEAD") != wString::npos) {
				method = QUERY_METHOD::HEAD;
			}
			else if (line.find ("POST") != wString::npos) {
				method = QUERY_METHOD::POST;
			}
			else if (line.find ("PUT") != wString::npos) {
				method = QUERY_METHOD::PUT;
			}
			else if (line.find ("DELETE") != wString::npos) {
				method = QUERY_METHOD::DEL;
			}
			else {
				debug_log_output ("'GET' not found. error.%d", accept_socket);
				return (-1);
			}
			// 最初のスペースまでを削除。
			if (!line.cut_before_character (' ')) {
				// データ異常
				return -1;
			}
			// 次にスペースが出てきたところの後ろまでを削除。
			if (!line.cut_after_character (' ')) {
				// データ異常
				return -1;
			}
			//strcpy( line_buf,line.c_str());
			// ===========================
			// GETオプション部解析
			// ===========================
			// REQUEST_URI用・Proxy用に値を保存
			strncpy (request_uri, line.c_str (), sizeof (request_uri) - 1);
			// '?'が存在するかチェック。
			if (line.find ('?') != wString::npos) {
				split2 = line;
				// '?'より前をカット
				split2.cut_before_character ('?');
				while (split2.length ()) {
					// 最初に登場する'&'で分割
					split ("&", split1, split2);
					// -------------------------------------
					// GETした内容 解析開始
					// 超安直。いいのかこんな比較で。
					// -------------------------------------
					// URIデコード
					if (split1.length ()) {
						wb2 = split1.uri_decode ();
						// "action="あるか調査。
						if (strncasecmp (wb2.c_str (), "action=", strlen ("action=")) == 0) {
							// = より前を削除
							wb2.cut_before_character ('=');
							// 構造体に値を保存。
							strncpy (action, wb2.c_str (), sizeof (action) - 1);
							continue;
						}
					}
				}
			}
			//debug_log_output("action = '%s'", action);
			// URIデコード
			line.cut_after_character ('?');
			line = line.uri_decode ();
			//debug_log_output("URI(decoded):'%s'\n", line.c_str());
			// 構造体に保存
			strncpy (recv_uri, line.c_str (), sizeof (recv_uri) - 1);
			//httpから始まってる場合には、http://以降の最初の'/'の前でカット
			if (strncmp (recv_uri, "http://", 7) == 0) {
				const char* ptr = strstr (recv_uri + 7, "/");
				if (ptr) {
					strcpy (recv_uri, ptr);
				}
			}
			continue;
		}
		// User-agent切り出し
		if (strncasecmp (line.c_str (), HTTP_USER_AGENT, strlen (HTTP_USER_AGENT)) == 0) {
			//debug_log_output("User-agent: Detect.\n");
			// ':'より前を切る
			line.cut_before_character (':');
			line = line.ltrim ();
			// 構造体に保存
			strncpy (user_agent, line.c_str (), sizeof (user_agent) - 1);
			continue;
		}
		// Rangeあるかチェック
		if (strncasecmp (line.c_str (), HTTP_RANGE, strlen (HTTP_RANGE)) == 0) {
			//debug_log_output("%s Detect.\n", HTTP_RANGE);
			// ':' より前を切る。
			line.cut_before_character (':');
			line = line.ltrim ();
			// recv_range にRangeの中身保存
			strncpy (recv_range, line.c_str (), sizeof (recv_range) - 1);
			// '=' より前を切る
			line.cut_before_character ('=');
			// '-'で前後に分割。
			wb2 = line;
			split ("-", wb1, wb2);
			// 値を文字列→数値変換
			range_start_pos = strtoull (wb1.c_str (), NULL, 10);
			if (wb2.length () > 0) {
				range_end_pos = strtoull (wb2.c_str (), NULL, 10);
			}
			debug_log_output ("range_start_pos=%d\n", range_start_pos);
			debug_log_output ("range_end_pos=%d\n", range_end_pos);
			continue;
		}
		// Hostあるかチェック
		if (strncasecmp (line.c_str (), HTTP_HOST, strlen (HTTP_HOST)) == 0) {
			// ':' より前を切る。
			line.cut_before_character (':');
			line = line.ltrim ();
			strncpy (recv_host, line.c_str (), sizeof (recv_host) - 1);
			//debug_log_output("%s Detect. %s '%s'", HTTP_HOST, HTTP_HOST, recv_host);
			continue;
		}
		// cookieあるかチェック
		if (strncasecmp (line.c_str (), HTTP_COOKIE, strlen (HTTP_COOKIE)) == 0) {
			// ':' より前を切る。
			line.cut_before_character (':');
			line.ltrim ();
			strncpy (cookie, line.c_str (), sizeof (cookie) - 1);
			//debug_log_output("%s Detect. %s '%s'", HTTP_COOKIE, HTTP_COOKIE, cookie);
			continue;
		}
		// Content-Lengthあるかチェック
		if (strncasecmp (line.c_str (), HTTP_CONTENT_LENGTH1, strlen (HTTP_CONTENT_LENGTH1)) == 0) {
			// ':' より前を切る。
			line.cut_before_character (':');
			line = line.ltrim ();
			strncpy (content_length, line.c_str (), sizeof (content_length) - 1);
			//debug_log_output("%s Detect. %s '%s'", HTTP_CONTENT_LENGTH1, HTTP_CONTENT_LENGTH1, content_length);
			continue;
		}
		// Content-TYPEあるかチェック
		if (strncasecmp (line.c_str (), HTTP_CONTENT_TYPE1, strlen (HTTP_CONTENT_TYPE1)) == 0) {
			// ':' より前を切る。
			line.cut_before_character (':');
			line = line.ltrim ();
			// multipart
			if (strncasecmp (line.c_str (), "multipart/form-data", strlen ("multipart/form-data")) == 0) {
				wString bnd = line;
				bnd.cut_before_character ('=');
				strncpy (boundary, bnd.c_str (), sizeof (boundary) - 1);
				strncpy (content_type, "multipart/form-data", sizeof ("multipart/form-data") - 1);
				//debug_log_output("%s Detect. %s '%s'", "multipart/form-data", "multipart/form-data", boundary);
			}
			else {
				strncpy (content_type, line.c_str (), sizeof (content_type) - 1);
				//debug_log_output("%s Detect. %s '%s'", HTTP_CONTENT_TYPE1, HTTP_CONTENT_TYPE1, content_type);
			}
			continue;
		}
	}
	return result;
}
/// <summary>
/// リクエストされたURIのファイルをチェック
/// aliasでの置き換え、documet_rootチェック、なければskin置き場チェックを行う。
///          -2:_OPENDIR
///          -1:_NOTFOUND
/// 		 0:実体
/// 		 1:ディレクトリ
/// 		 2:CGIファイル
/// </summary>
/// <returns>チェックしたファイルタイプ</returns>
FILETYPES HTTP_RECV_INFO::http_file_check (void)
{
	char 	work_buf[1024];
	char 	work_data[FILENAME_MAX] = {};
	char	file_extension[16];
	//debug_log_output ("http_file_check() start.");
	// -------------------------
	// ファイルチェック
	// -------------------------
	// 要求パスのフルパス生成。
	strncpy (send_filename, global_param.document_root, sizeof (send_filename) - 1);
	strncpy (work_data, recv_uri, sizeof (work_data) - 1);

	// recv_uri保護のための変更
	rtrim (work_data, '/');
	replace_character (work_data, "/", DELIMITER);
	strncat (send_filename, work_data, sizeof (send_filename) - 1);
	//uri_decode
	uri_decode (work_buf, sizeof (work_buf), send_filename, sizeof (send_filename));
	strncpy (send_filename, work_buf, sizeof (send_filename) - 1);


	//(aliasを加味して）
	for (int i = 0; i < MAX_COUNT_ALIAS; i++) {
		if (*global_param.alias_key[i] && strstr (send_filename, global_param.alias_key[i])) {
			//aliasで置換する
			//snprintf(work_buf, sizeof(work_buf), "%s%s", global_param.document_org, global_param.alias_key[i]);
			//replace_character(send_filename, work_buf, global_param.alias_rep[i]);
			replace_character (send_filename, global_param.alias_key[i], global_param.alias_rep[i]);
		}
	}

	//debug_log_output ("send_filename='%s'\n", send_filename);
	// ------------------------------------------------------------
	// ファイルあるかチェック。
	// ------------------------------------------------------------
	if (wString::file_exists (send_filename)) {    // パスが示すファイルが存在する
		// ファイル実体と検知
		//debug_log_output ("'%s' is File!!", send_filename);

		// -------------------------------------------
		// ファイルの拡張子より、Content-type を決定
		// -------------------------------------------
		filename_to_extension (send_filename, file_extension, sizeof (file_extension));
		//debug_log_output ("send_filename='%s', file_extension='%s'\n", send_filename, file_extension);
		// 拡張子から、mime_typeを導く。
		MIME_LIST_T::check_file_extension_to_mime_type (file_extension, mime_type, sizeof (mime_type));
		// 実体ファイルで分岐
		if (strcasecmp (file_extension, "cgi") == 0 ||
			strcasecmp (file_extension, "jss") == 0 ||
			strcasecmp (file_extension, "php") == 0 ||
			strcasecmp (file_extension, "exe") == 0) {
			// CGIの実行が不許可なら、Not Found.
			return (global_param.flag_execute_cgi ? FILETYPES::_CGI : FILETYPES::_NOTFOUND);  // cgiファイル
		}
		else {
			return (FILETYPES::_FILE);	// File実体
		}
	}
	else if (wString::directory_exists (send_filename)) {    // パスが示すディレクトリが存在する
		//		int len;
		//		len = strlen(recv_uri);
		//		if (len > 0 && recv_uri[len - 1] != '/') {
		//			// '/' で終端していないディレクトリ要求の場合...
		//			return ( -2 );
		//		}
		// ディレクトリと検知
		 //debug_log_output("'%s' is Dir!!", send_filename);
		return (FILETYPES::_DIR);	// ディレクトリ
		//ファイルリストチェック
	}
	else {
		//		debug_log_output("stat() error\n", result);
		// ----------------------------------------------------------------------------
		// もし、実体が存在しなかったら、skin置き場に変えてもう一度チェック
		// Skin置き場は実体ファイルのみ認める。
		// ----------------------------------------------------------------------------
		//debug_log_output("DocumentRoot not found. SkinDir Check.");
		//debug_log_output("global_param.skin_root='%s'", global_param.skin_root);
		//debug_log_output("global_param.skin_name='%s'", global_param.skin_name);
		// 2004/10/13 Add start
		strncpy (work_data, recv_uri, sizeof (work_data) - 1);
		if (work_data[0] == '/') {
			char work[2048];
			strcpy (work, work_data + 1);
			strcpy (work_data, work);
		}
		//replace_character(work_data,"/","\\");
		//cut_before_last_character(work_data, '/');
		// 2004/10/13 Add end
		// skin置き場にあるモノとして、フルパス生成。
		strncpy (send_filename, global_param.skin_root, sizeof (send_filename) - 1);
		strncat (send_filename, global_param.skin_name, sizeof (send_filename) - 1);
		strncat (send_filename, work_data, sizeof (send_filename) - 1);
		// '/' が重なってるところの重複を排除。
		//		duplex_character_to_unique(send_filename, '/');
		// 2004/10/13 Delete start
		//		duplex_character_to_unique(send_filename, DELIMITER[0]);
		// 2004/10/13 Delete end
		//debug_log_output ("SkinDir:send_filename='%s'\n", send_filename);
		// ------------------------------------------------------------
		// Skin置き場にファイルあるかチェック。
		// ------------------------------------------------------------
		// 2004/07/0? Delete start
		//		result = stat(send_filename, &send_filestat);
		//		debug_log_output("stat: result=%d, st_mode=%04X, S_ISREG=%d\n", result, send_filestat.st_mode, S_ISREG(send_filestat.st_mode));
		// 2004/07/0? Delete end
		// ファイル実体と検知。
		//		if ( ( result == 0 ) && (S_ISREG(send_filestat.st_mode) == 1 ) )
		if (wString::file_exists (send_filename)) {       // パスが示すファイルが存在する
			// ファイル実体と検知
			//debug_log_output("'%s' is File!!", send_filename);
			// -------------------------------------------
			// ファイルの拡張子より、Content-type を決定
			// -------------------------------------------
			filename_to_extension (send_filename, file_extension, sizeof (file_extension));
			//debug_log_output("send_filename='%s', file_extension='%s'\n", send_filename, file_extension);
			MIME_LIST_T::check_file_extension_to_mime_type (file_extension, mime_type, sizeof (mime_type));
			if (strcasecmp (file_extension, "cgi") == 0 ||
				strcasecmp (file_extension, "jss") == 0 ||
				strcasecmp (file_extension, "exe") == 0) {
				// CGIの実行が不許可なら、Not Found.
				return (global_param.flag_execute_cgi ? FILETYPES::_CGI : FILETYPES::_NOTFOUND);	// cgiファイル
			}
			else {
				return (FILETYPES::_FILE);	// File実体
			}
		}
		else {
			// -------------------------------------
			// File Not Found.
			// やっぱり、404にしよう。
			// -------------------------------------
			return (FILETYPES::_NOTFOUND);
		}
	}
}
#if 0
// **************************************************************************
// accept_socketから、１行(CRLFか、LF単独が現れるまで)受信
// CRLFは削除する。
// 受信したサイズをreturnする。
// CRLFだけの行は0を返す
// EOFは-1を返す
// **************************************************************************
int line_receive (int accept_socket, char* line_buf_p, int line_max)
{
	char 	byte_buf;
	int 	line_len = 0;
	int		recv_len;
	int		received = 0;
	// １行受信実行
	while (1) {
		recv_len = recv (accept_socket, &byte_buf, 1, 0);
		if (recv_len == 0) {
			if (received) {
				return line_len;
			}
			else {
				return -1;
			}
		}
		if (recv_len < 0) { // 受信失敗チェック
#ifdef linux
			debug_log_output ("header read error cnt = %d error=%s\n", recv_len, strerror (errno));
#else
			debug_log_output ("header read error cnt = %d socket=%d error=%d\n", recv_len, accept_socket, WSAGetLastError ());
#endif
			return (-1);
		}
		received = 1;
		// CR/LFチェック
		if (byte_buf == '\r') {
			continue;
		}
		else if (byte_buf == '\n') {
			*line_buf_p = 0;
			break;
		}
		// バッファにセット
		*line_buf_p++ = byte_buf;
		// 受信バッファサイズチェック
		if (++line_len >= line_max) {
			// バッファオーバーフロー検知
			debug_log_output ("line_buf over flow.");
			return (-1);
		}
	}
	return line_len;
}
#endif
/// <summary>
/// 301(redirect)コードを送出
/// </summary>
/// <param name="accept_socket">SOCKET</param>
/// <param name="location">リダイレクト先</param>
/// <returns>0</returns>
int HTTP_RECV_INFO::http_redirect_response (SOCKET accept_socket, char* location)
{
	wString buffer;
	buffer.sprintf ("HTTP/1.1 301 Found\r\n" "Location: %s\r\n" "\r\n", location);
	send (accept_socket, buffer.c_str (), buffer.length (), 0);
	debug_log_output ("Redirect to %s", location);
	sClose (accept_socket);
	return 0;
}
/// <summary>
/// 404(Not Found)コードを送出
/// </summary>
/// <param name="accept_socket">SOCKER</param>
/// <returns>0</returns>
int HTTP_RECV_INFO::http_not_found_response (SOCKET accept_socket)
{
	wString buffer;
	buffer.sprintf (HTTP_NOT_FOUND1 HTTP_SERVER_NAME HTTP_CONTENT_TYPE HTTP_CONTENT_LENGTH HTTP_END "%s"
					, SERVER_NAME
					, "text/html"
					, (size_t)9
					, "Not Found");
	send (accept_socket, buffer.c_str (), buffer.length (), 0);
	debug_log_output ("Not Found %s", request_uri);
	sClose (accept_socket);
	return 0;
}
