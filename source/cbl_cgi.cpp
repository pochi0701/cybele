﻿#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_cgi.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include <stdio.h>
#else
#include <io.h>
#include <process.h>
//#include <dir.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include "cbl.h"
#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
#include "define.h"

wString escape(const wString& str);
int split(const char* cut_char, wString& split1, wString& split2);
//////////////////////////////////////////////////////////////////////////
int HTTP_RECV_INFO::http_cgi_response(SOCKET accept_socket)
{
	char* query_string;
	char script_filename[1024];
	char* script_exec_name;
	char cwd[FILENAME_MAX];
	char ext[4];
	//WINDOWSでドライブから始まる場合
	if (send_filename[1] != ':') {
		debug_log_output("WARNING: send_filename[1] != ':', send_filename = '%s'", send_filename);
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			debug_log_output("getcwd() failed. err = %s", strerror(errno));
			return -1;
		}
		strncpy(script_filename, send_filename, sizeof(send_filename));
		//2004/07/13 Update end
		path_sanitize(script_filename, sizeof(script_filename));
	}
	else {
		strncpy(script_filename, send_filename, sizeof(script_filename));
	}
	query_string = strchr(request_uri, '?');
	if (query_string == NULL) {
		query_string = (char*)"";
	}
	else {
		*query_string++ = '\0';
	}
	script_exec_name = strrchr(request_uri, '/');
	if (script_exec_name == NULL) {
		script_exec_name = request_uri;
	}
	else {
		script_exec_name++;
	}
	if (script_exec_name == NULL) {
		debug_log_output("script_exec_name and script_name == NULL");
		return -1;
	}
	//拡張子の取得
	filename_to_extension(script_exec_name, ext, 4);
	if (strcasecmp(ext, "jss") == 0) {
		//スレッドIDについて出力先を保存
		jss(accept_socket, script_filename, query_string);
		sClose(accept_socket);
		return 0;
	}
	else {
		//ヘッダくらいは返したほうが良いが
		sClose(accept_socket);
		return 0;
	}
}

/// <summary>
/// JavaScript実行
/// </summary>
/// <param name="accept_socket">結合ソケット</param>
/// <param name="script_filename">対象フルファイル名</param>
/// <param name="query_string">クエリー文字列</param>
void HTTP_RECV_INFO::jss(SOCKET accept_socket, char* script_filename, char* query_string)
{
	//実行
	char server_port[100] = { 0 };
	char remote_port[100] = { 0 };
	struct sockaddr_in saddr;
	socklen_t socklen;
	char next_cwd[FILENAME_MAX] = { 0 };

	//指定フォルダにcd
	strncpy(next_cwd, script_filename, sizeof(next_cwd));
	cut_after_last_character(next_cwd, '/');
	struct stat results;
	if (stat(script_filename, &results) != 0)
	{
		debug_log_output("Cannot stat file! '%s'\n", script_filename);
		return;
	}
	//指定フォルダにCD
	if (chdir(next_cwd) != 0) {
		debug_log_output("chdir failed. err = %s", strerror(errno));
	}
	int size = results.st_size;
	//追加: O_CREATE | O_APPEND | O_WRONLY(またはO_RDWR) | (O_BINARY) , S_IREAD | S_IWRITE
	//新規: O_CREATE | O_TRUNC  | O_WRONLY(またはO_RDWR) | (O_BINARY) , S_IREAD | S_IWRITE
	//読込: O_RDONLY                                     | (O_BINARY) 

	int fd = myopen(script_filename, O_RDONLY | O_BINARY);
	/* if we open as text, the number of bytes read may be > the size we read */
	if (fd < 0) {
		debug_log_output("Unable to open file '%s'", script_filename);
		return;
	}

	char* buffer = new char[size + 1];
	//long actualRead = fread (buffer+2, 1, size, file);
	int actualRead = read(fd, buffer, size);
	if (actualRead < 0) {
		delete[] buffer;
		close(fd);
		debug_log_output("ServerSide JavaScript Error end");
		return;
	}
	//todo:失敗した場合のactualReadの値
	buffer[actualRead] = 0;
	buffer[size] = 0;
	close(fd);

	CTinyJS  javaScriptThread(accept_socket);
	registerFunctions(&javaScriptThread);
	registerMathFunctions(&javaScriptThread);
	//insert Environment to js
	wString script1;
	wString script2;
	wString script3;
	wString script4;
	javaScriptThread.root->addChild("_SERVER", new CScriptVar(TINYJS_BLANK_DATA, (int)SCRIPTVAR_FLAGS::SCRIPTVAR_OBJECT));
	javaScriptThread.root->addChild("_GET", new CScriptVar(TINYJS_BLANK_DATA, (int)SCRIPTVAR_FLAGS::SCRIPTVAR_OBJECT));
	javaScriptThread.root->addChild("_COOKIE", new CScriptVar(TINYJS_BLANK_DATA, (int)SCRIPTVAR_FLAGS::SCRIPTVAR_OBJECT));
	javaScriptThread.root->addChild("_POST", new CScriptVar(TINYJS_BLANK_DATA, (int)SCRIPTVAR_FLAGS::SCRIPTVAR_OBJECT));

	try
	{
		script1.clear();
		//環境変数をJSに展開
		//TODO:まとめて一回で評価する方がよさげ
		//fastcgi_param  DOCUMENT_URI       $document_uri;
		// "multipart/form-data; boundary=---------------------------382462320637558520782293981033"
		if (content_type)   script1.cat_sprintf("var _SERVER.CONTENT_TYPE=\"%s\";", escape(content_type).c_str());
		if (content_length) script1.cat_sprintf("var _SERVER.CONTENT_LENGTH=\"%s\";", escape(content_length).c_str());
		if (recv_host)      script1.cat_sprintf("var _SERVER.HTTP_HOST=\"%s\";", escape(recv_host).c_str());
		if (user_agent)     script1.cat_sprintf("var _SERVER.HTTP_USER_AGENT=\"%s\";", escape(user_agent).c_str());
		//SERVER SIGNATURE
		script1.cat_sprintf("var _SERVER.PATH=\"%s\";", escape(DEFAULT_PATH).c_str());
		script1.cat_sprintf("var _SERVER.SERVER_SOFTWARE=\"%s\";", escape(SERVER_NAME).c_str());
		script1.cat_sprintf("var _SERVER.SERVER_NAME=\"%s\";", escape(global_param.server_name).c_str());
		//SERVER PORT
		socklen = sizeof(saddr);
		getsockname(accept_socket, (struct sockaddr*)&saddr, &socklen);
		script1.cat_sprintf("var _SERVER.SERVER_ADDR=\"%s\";", escape(inet_ntoa(saddr.sin_addr)).c_str());
		snprintf(server_port, sizeof(server_port), "%u", ntohs(saddr.sin_port));
		script1.cat_sprintf("var _SERVER.SERVER_PORT=\"%s\";", escape(server_port).c_str());
		script1.cat_sprintf("var _SERVER.DOCUMENT_ROOT=\"%s\";", escape(global_param.document_root).c_str());
		//REMOTE ADDR
		socklen = sizeof(saddr);
		getpeername(accept_socket, (struct sockaddr*)&saddr, &socklen);
		script1.cat_sprintf("var _SERVER.REMOTE_ADDR=\"%s\";", escape(inet_ntoa(saddr.sin_addr)).c_str());
		//REMOTE PORT
		snprintf(remote_port, sizeof(remote_port), "%u", ntohs(saddr.sin_port));
		script1.cat_sprintf("var _SERVER.REMOTE_PORT=\"%s\";", escape(remote_port).c_str());
		script1.cat_sprintf("var _SERVER.SCRIPT_FILENAME=\"%s\";", escape(script_filename).c_str());
		script1.cat_sprintf("var _SERVER.GATEWAY_INTERFACE=\"%s\";", "CGI/1.1");
		script1.cat_sprintf("var _SERVER.SERVER_PROTOCOL=\"%s\";", "HTTP/1.0");
		char GETPOST[4][5] = { "","GET","HEAD","POST" };
		script1.cat_sprintf("var _SERVER.REQUEST_METHOD=\"%s\";", GETPOST[isGet]);

		script1.cat_sprintf("var _SERVER.QUERY_STRING=\"%s\";", escape(query_string).c_str());
		script1.cat_sprintf("var _SERVER.REQUEST_URI=\"%s\";", escape(request_uri).c_str());
		script1.cat_sprintf("var _SERVER.SCRIPT_NAME=\"%s\";", escape(request_uri).c_str());
		javaScriptThread.execute(script1);


		//GETの展開 query stringをgetに展開
		script3 = query_string;
		script4.clear();
		while (script3.length()) {
			split("&", script1, script3);
			split("=", script2, script1);
			script4 += "var _GET." + script2 + (char*)"=\"" + escape(script1.uri_decode()) + (char*)"\";";
		}
		if (script4.Length()) {
			javaScriptThread.execute(script4);
		}


		//COOKIEの展開 query stringをgetに展開
		script3 = cookie;
		script4.clear();
		while (script3.length()) {
			split(";", script1, script3);
			script1 = script1.LTrim();
			split("=", script2, script1);
			script4 += "var _COOKIE." + script2 + (char*)"=\"" + escape(script1.uri_decode()) + (char*)"\";";
			//SESSIONIDの設定
			if (script2 == "sid") {
				script4 += (char*)"var JSSESSID=\"" + escape(script1.uri_decode()) + (char*)"\";";
			}
		}
		if (script4.Length()) {
			javaScriptThread.execute(script4);
		}

		//POSTの展開
		if (isGet == 3) {
			char buf[10240];
			int num;
			int contentsize = atoi(content_length);
			int readsize;
			script3.clear();
			//指定されたサイズまで読む
			while (contentsize) {
				if (contentsize < 1024) {
					readsize = contentsize;
				}
				else {
					readsize = 1024;
				}
				//num = read( accept_socket, buf, readsize);
				num = recv(accept_socket, buf, readsize, 0);
				////wString* tmp;
				if (num <= 0) {
					break;
				}
				else {
					contentsize -= num;
				}
				buf[num] = 0;
				script3 += buf;
			}
			script4.clear();
			while (script3.length()) {
				split("&", script1, script3);
				split("=", script2, script1);
				script4 += (char*)"var _POST." + script2 + (char*)"=\"" + escape(script1.uri_decode()) + (char*)"\";";
			}
			if (script4.Length()) {
				javaScriptThread.execute(script4, 1);
			}
		}
		javaScriptThread.execute(buffer, 0);
	}
	catch (CScriptException* e)
	{
		debug_log_output("SCRIPT ERROR: %s\n", e->text.c_str());
		wString tmp;
		tmp.sprintf("SCRIPT ERROR: %s\n", e->text.c_str());
		send(accept_socket, tmp.c_str(), tmp.Length(), 0);
		delete[] buffer;
		return;
	}
	delete[] buffer;
	debug_log_output("ServerSide JavaScript end");
	return;
}
//文字列のエスケープ
wString escape(const wString& str)
{

	wString dst;
	unsigned int len = str.Length();
	// 引数チェック
	for (auto is = 0U; is < len; is++) {
		if (str[is] == '\"') {
			dst += "\\\"";
		}
		else if (str[is] == '\\') {
			dst += "\\\\";
		}
		else {
			dst += str[is];
		}
	}
	return dst;
}