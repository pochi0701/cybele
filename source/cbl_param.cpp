#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_param.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef linux
#include <unistd.h>
#else
#include <io.h>
//#include <dir.h>
#endif
#include "cbl.h"
#include "cbl_tools.h"
#include "cbl_String.h"
#include "define.h"
extern wString current_dir;
static int config_file_open(void);
static int config_file_read_line(int fd, char* line_buf, int line_buf_size);
static void line_buffer_sanitize(char* line_buf);
// ********************************************
// MIME リスト
// とりあえず知ってる限り書いておく。
// ********************************************
MIME_LIST_T     mime_list[] = {
	//  {mime_name           ,file_extension  ,stream_type     ,   menu_file_type  },
	{(char*)"text/plain"      ,(char*)"txt"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"text/html"       ,(char*)"htm"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"text/html"       ,(char*)"html"   ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"text/css"        ,(char*)"css"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"text/html"       ,(char*)"exe"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"text/html"       ,(char*)"jss"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"text/html"       ,(char*)"php"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"text/html"       ,(char*)"pl"     ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   },
	{(char*)"image/gif"       ,(char*)"gif"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_IMAGE      },
	{(char*)"image/jpeg"      ,(char*)"jpeg"   ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_IMAGE      },
	{(char*)"image/jpeg"      ,(char*)"jpg"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_IMAGE      },
	{(char*)"image/png"       ,(char*)"png"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_IMAGE      },
	{(char*)"image/x-icon"    ,(char*)"ico"    ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_IMAGE      },
	{(char*)"video/mpeg"      ,(char*)"mpeg"   ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/mpeg"      ,(char*)"mpg"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/mpeg"      ,(char*)"m2p"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/mpeg"      ,(char*)"hnl"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/x-msvideo" ,(char*)"avi"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/mpeg"      ,(char*)"vob"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/mpeg"      ,(char*)"vro"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },  /* add for DVD-RAM */
	{(char*)"video/quicktime" ,(char*)"mov"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/x-ms-wmv"  ,(char*)"wmv"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/x-ms-wmx"  ,(char*)"asf"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"audio/mpeg"      ,(char*)"mp3"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MUSIC      },
	{(char*)"audio/x-ogg"     ,(char*)"ogg"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MUSIC      },
	{(char*)"video/mp4"       ,(char*)"mp4"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/divx"      ,(char*)"divx"   ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"video/flv"       ,(char*)"flv"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MOVIE      },
	{(char*)"audio/x-ms-wma"  ,(char*)"wma"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MUSIC      },
	{(char*)"audio/x-wav"     ,(char*)"wav"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MUSIC      },
	{(char*)"audio/ac3"       ,(char*)"ac3"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MUSIC      },
	{(char*)"audio/x-m4a"     ,(char*)"m4a"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MUSIC      },
	{(char*)"text/plain"      ,(char*)"plw"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_PLAYLIST   }, // Play List for Cybele.
	{(char*)"text/plain"      ,(char*)"upl"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_PLAYLIST   }, // Uzu Play List拡張子でもOK. ファイル自身の互換は無し。
	{(char*)"text/plain"      ,(char*)"m3u"    ,STREAM_TYPE::TYPE_STREAM     ,   MIME_TYPE::TYPE_MUSICLIST  }, // m3u でもOK?
	{(char*)"text/javascript" ,(char*)"js"     ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   }, //JavaScript
	{(char*)"application/wasm",(char*)"wasm"   ,STREAM_TYPE::TYPE_NO_STREAM  ,   MIME_TYPE::TYPE_DOCUMENT   }, //JavaScript
	{NULL, NULL, STREAM_TYPE::NOT_DEFINED, MIME_TYPE::NOT_MIME }
};
// ********************************************
// 全体パラメータ構造体の実体
// ********************************************
GLOBAL_PARAM_T  global_param;
// IPアクセス許可リスト
ACCESS_CHECK_LIST_T     access_allow_list[ACCESS_ALLOW_LIST_MAX];
//終了フラグ
int loop_flag = 1;
// ********************************************
// 全体パラメータ構造体の初期化。
// デフォルト値をセット
// ********************************************
void global_param_init(void)
{
	//memset(&user_info_list, 0, sizeof(user_info_list));
	//user_info_list.list_num = 0;
	// 構造体まとめて初期化
	memset(&global_param, 0, sizeof(global_param));
	memset(access_allow_list, 0, sizeof(access_allow_list));
	//memset(allow_user_agent,      0, sizeof(allow_user_agent));
	// デーモン化フラグ
	global_param.flag_daemon = DEFAULT_FLAG_DAEMON;
	// 自動検出
	global_param.flag_auto_detect = DEFAULT_FLAG_AUTO_DETECT;
	// デフォルトServer名。gethostname()する。
	gethostname(global_param.server_name, sizeof(global_param.server_name));
	// デフォルトHTTP 待ち受けPort.
	global_param.server_port = DEFAULT_SERVER_PORT;
	// Document Root
	strncpy(global_param.document_root, DEFAULT_DOCUMENT_ROOT, sizeof(global_param.document_root)-1);
	// DebugLog
	global_param.flag_debug_log_output = DEFAULT_FLAG_DEBUG_LOG_OUTPUT;
#ifndef linux
	//strncpy(global_param.document_org, DEFAULT_DOCUMENT_ROOT, sizeof(global_param.document_org));
	////strncpy(global_param.debug_log_filename, DEFAULT_DEBUG_LOG_FILENAME, sizeof(global_param.debug_log_filename)-1);
	////debuglogを完全パスに
	char cwd[FILENAME_MAX];
	_getcwd(cwd, sizeof(cwd));
	sprintf(global_param.debug_log_filename, "%s%s%s", cwd, DELIMITER, DEFAULT_DEBUG_LOG_FILENAME);
#else
	strncpy(global_param.debug_log_filename, DEFAULT_DEBUG_LOG_FILENAME, sizeof(global_param.debug_log_filename)-1);
#endif    
	// スキン情報使用フラグ
	global_param.flag_use_skin = DEFAULT_FLAG_USE_SKIN;
	// スキン置き場
	strncpy(global_param.skin_root, DEFAULT_SKINDATA_ROOT, sizeof(global_param.skin_root)-1);
	// スキン名
	strncpy(global_param.skin_name, DEFAULT_SKINDATA_NAME, sizeof(global_param.skin_name)-1);
	// ワーク
	strncpy(global_param.work_root, DEFAULT_WORKROOT_NAME, sizeof(global_param.work_root)-1);
	// CGIスクリプトの実行を許可するかフラグ
	global_param.flag_execute_cgi = DEFAULT_FLAG_EXECUTE_CGI;
	// プロクシを許可するかフラグ
	global_param.flag_allow_proxy = DEFAULT_FLAG_ALLOW_PROXY;
	// cbl.conf 変更対応 allow_user_agent
	//strncpy(allow_user_agent[0].user_agent, DEFAULT_ALLOW_USER_AGENT, sizeof(allow_user_agent[0].user_agent) );
	// cbl.conf 変更対応 allow_user_agent
	strncpy(global_param.system_password, "", sizeof(global_param.system_password)-1);
	return;
}
// ****************************************************
// True,Falseの設定
// ****************************************************
void readTF(char* value, char& flag) {
	if (strcasecmp(value, "true") == 0) {
		flag = TRUE;
	}
	else if (strcasecmp(value, "false") == 0) {
		flag = FALSE;
	}
}
void readTF(char* value, int& flag) {
	if (strcasecmp(value, "true") == 0) {
		flag = TRUE;
	}
	else if (strcasecmp(value, "false") == 0) {
		flag = FALSE;
	}
}
// ****************************************************
// cbl.conf 読む
// ****************************************************
void config_file_read(void)
{
	int    fd;
	char   line_buf[1024 * 4];
	int    ret;
	int    count_alias = 0;
	char   key[1024];
	char   value[1023];
	char   work1[1024];
	char   work2[1024];
#ifdef linux
	int    count_access_allow = 0;
	char   work3[1024];
	char   work4[1023];
#endif
	// =======================
	// ConfigファイルOPEN
	// =======================
	fd = config_file_open();
	if (fd < 0) {
		return;
	}
	// =====================
	// 内容読み込み
	// =====================
	while (1) {
		// １行読む。
		ret = config_file_read_line(fd, line_buf, sizeof(line_buf));
		if (ret < 0) {
#ifdef linux
			printf("EOF Detect.\n");
#endif
			break;
		}
		// 読んだ行を整理する。
		line_buffer_sanitize(line_buf);
		if (strlen(line_buf) > 0) // 値が入ってた。
		{
			// ' 'で、前後に分ける
			sentence_split(line_buf, ' ', key, value);
#ifdef linux
			printf("key='%s', value='%s'\n", key, value);
#endif
			// ---------------------
			// 値の読みとり実行。
			// ---------------------
			if ((strlen(key) > 0) && (strlen(value) > 0))
			{

				// flag_auto_detect
				if (strcasecmp("flag_auto_detect", key) == 0) {
					readTF(value, global_param.flag_auto_detect);
				}
#ifdef linux
				// flag_daemon
				else if (strcasecmp("flag_daemon", key) == 0) {
					readTF(value, global_param.flag_daemon);
				}
				// flag_debug_log_output
				else if (strcasecmp("flag_debug_log_output", key) == 0) {
					readTF(value, global_param.flag_debug_log_output);
				}
				// debug_log_filename
				else if (strcasecmp("debug_log_filename", key) == 0) {
					strncpy(global_param.debug_log_filename, value, sizeof(global_param.debug_log_filename)-1);
				}
				// exec_user
				else if (strcasecmp("exec_user", key) == 0) {
					strncpy(global_param.exec_user, value, sizeof(global_param.exec_user)-1);
				}
				// exec_group
				else if (strcasecmp("exec_group", key) == 0) {
					strncpy(global_param.exec_group, value, sizeof(global_param.exec_group)-1);
				}
				// auto_detect_bind_ip_address
				else if (strcasecmp("auto_detect_bind_ip_address", key) == 0) {
					strncpy(global_param.auto_detect_bind_ip_address, value, sizeof(global_param.auto_detect_bind_ip_address)-1);
				}
				// server_name
				else if (strcasecmp("server_name", key) == 0) {
					strncpy(global_param.server_name, value, sizeof(global_param.server_name)-1);
				}
#endif
				// server_port
				else if (strcasecmp("server_port", key) == 0) {
					global_param.server_port = atoi(value);
				}
				// document_root
				else if (strcasecmp("document_root", key) == 0) {
					//ALIASのために保存
					//strncpy(global_param.document_org, value, sizeof(global_param.document_org));
#ifdef linux
					strncpy(global_param.document_root, value, sizeof(global_param.document_root)-1);
					//printf("%s\n", value );
#else
					// ":"が含まれていなければ、相対パスとみなす
					if (strchr(value, ':') == NULL) {
						//現在のディレクトリを取ってはいけない。現在のディレクトリは不定
						snprintf(global_param.document_root, sizeof(global_param.document_root), "%s%s", current_dir.c_str(), value);
					}
					else {
						strncpy(global_param.document_root, value, sizeof(global_param.document_root)-1);
					}
#endif
				}
				// alias
				else if (strcasecmp("alias", key) == 0 && count_alias < MAX_COUNT_ALIAS) {
					// valueを' 'で分割
					sentence_split(value, ' ', work1, work2);
					strncpy(global_param.alias_key[count_alias], work1, sizeof(global_param.alias_key[0])-1);
					strncpy(global_param.alias_rep[count_alias], work2, sizeof(global_param.alias_rep[0])-1);
					count_alias++;
				}
				// flag_use_skin
				else if (strcasecmp("flag_use_skin", key) == 0) {
					readTF(value, global_param.flag_use_skin);
				}
				// skin_root
				else if (strcasecmp("skin_root", key) == 0) {
#ifdef linux
					strncpy(global_param.skin_root, value, sizeof(global_param.skin_root)-1);
#else
					// ":"が含まれていなければ、相対パスとみなす
					if (strchr(value, ':') == NULL) {
						//現在のディレクトリを取ってはいけない
						snprintf(global_param.skin_root, sizeof(global_param.skin_root), "%s%s", current_dir.c_str(), value);
					}
					else {
						strncpy(global_param.skin_root, value, sizeof(global_param.skin_root)-1);
					}
#endif

					// 最後がDelimiter[0]じゃなかったら、Delimiter[0]を追加
					if (global_param.skin_root[strlen(global_param.skin_root) - 1] != DELIMITER[0]) {
						strncat(global_param.skin_root, DELIMITER, sizeof(global_param.skin_root)-1);
					}
				}
				// skin_name
				else if (strcasecmp("skin_name", key) == 0) {
					strncpy(global_param.skin_name, value, sizeof(global_param.skin_name)-1);
					// 最後が'/'じゃなかったら、'/'を追加
					if (global_param.skin_name[strlen(global_param.skin_name) - 1] != DELIMITER[0]) {
						strncat(global_param.skin_name, DELIMITER, sizeof(global_param.skin_name)-1);
						//printf("global_param.skin_name_B '%s' open.\n", global_param.skin_name);
					}
				}
				// access_allow
				else if (strcasecmp("system_password", key) == 0) {
					strncpy(global_param.system_password, value, sizeof(global_param.system_password)-1);
				}

#ifndef linux
				// work_root
				else if (strcasecmp("work_root", key) == 0) {
					strncpy(global_param.work_root, value, sizeof(global_param.work_root)-1);
					//                                        printf("global_param.skin_name '%s' open.\n", global_param.skin_name);
					// 最後が'/'じゃなかったら、'/'を追加
					if (global_param.work_root[strlen(global_param.work_root) - 1] != DELIMITER[0]) {
						strncat(global_param.work_root, DELIMITER, strlen(DELIMITER));
						// printf("global_param.skin_name_B '%s' open.\n", global_param.skin_name);
					}
				}
#else
				// user_agent_proxy_override
				else if (strcasecmp("user_agent_proxy_override", key) == 0) {
					strncpy(global_param.user_agent_proxy_override, value, sizeof(global_param.user_agent_proxy_override)-1);
				}
				// flag_execute_cgi
				else if (strcasecmp("flag_execute_cgi", key) == 0) {
					readTF(value, global_param.flag_execute_cgi);
				}
				// flag_allow_proxy
				else if (strcasecmp("flag_allow_proxy", key) == 0) {
					readTF(value, global_param.flag_allow_proxy);
				}
				// access_allow
				else if (strcasecmp("access_allow", key) == 0) {
					if (count_access_allow < ACCESS_ALLOW_LIST_MAX) {
						// valueを'/'で分割
						sentence_split(value, '/', work1, work2);
						access_allow_list[count_access_allow].flag = TRUE;
						// adddress文字列を、'.'で分割し、それぞれをatoi()
						strncat(work1, ".", sizeof(work1)-1); // 分割処理のため、最後に"."を足しておく
						for (int i = 0; i < 4; i++) {
							sentence_split(work1, '.', work3, work4);
							access_allow_list[count_access_allow].address[i] = (char)atoi(work3);
							strncpy(work1, work4, sizeof(work1)-1);
						}
						// netmask文字列を、'.'で分割し、それぞれをatoi()
						strncat(work2, ".", sizeof(work2)-1); // 分割処理のため、最後に"."を足しておく
						for (int i = 0; i < 4; i++) {
							sentence_split(work2, '.', work3, work4);
							access_allow_list[count_access_allow].netmask[i] = (char)atoi(work3);
							strncpy(work2, work4, sizeof(work1)-1);
						}
						printf("[%d] address=[%d.%d.%d.%d/%d.%d.%d.%d]\n", count_access_allow,
							access_allow_list[count_access_allow].address[0],
							access_allow_list[count_access_allow].address[1],
							access_allow_list[count_access_allow].address[2],
							access_allow_list[count_access_allow].address[3],
							access_allow_list[count_access_allow].netmask[0],
							access_allow_list[count_access_allow].netmask[1],
							access_allow_list[count_access_allow].netmask[2],
							access_allow_list[count_access_allow].netmask[3]);
						// addressをnetmaskで and 演算しちゃう。
						for (int i = 0; i < 4; i++) {
							access_allow_list[count_access_allow].address[i] &= access_allow_list[count_access_allow].netmask[i];
						}
						count_access_allow++;
					}
				}
#endif
			}
		}
	}
	close(fd);
	wString tmp = wString::HTTPGet("http://neon.cx/cybele/ip.php");
	// アクセスできた時
	if (tmp.Length() > 0)
	{
		if (tmp[tmp.length() - 1] == '\n')
		{
			tmp = tmp.SubString(0, tmp.length() - 1);
		}
		strcpy(global_param.global_ip, tmp.c_str());
	}
	// アクセス不可の時
	else
	{
		strcpy(global_param.global_ip, "27.120.109.45");
	}
	return;
}
// *****************************************************
// cbl.conf から１行読み込む
// 読み込んだ文字数がreturnされる。
// 最後まで読んだら、-1が戻る。
// *****************************************************
static int config_file_read_line(int fd, char* line_buf, int line_buf_size)
{
	int  read_len;
	int  total_read_len;
	char read_char;
	char* p;
	p = line_buf;
	total_read_len = 0;
	while (1) {
		// 一文字read.
		read_len = read(fd, &read_char, 1);
		if (read_len <= 0) { // EOF検知
			// 2005/04/19 Maida EDIT START
			// ここでreturnを行うと最後の行が改行されていない場合読み込まれない。
			// そのためbreakを返す。
			return (-1);
		}
		else if (read_char == '\r') {
			continue;
		}
		else if (read_char == '\n') {
			break;
		}
		*p = read_char;
		p++;
		total_read_len++;
		if (total_read_len >= line_buf_size) {
			break;
		}
	}
	*p = '\0';
	return total_read_len;
}
// ****************************************************
// cbl_conf を開く
// 開けなかったら -1
// ****************************************************
static int config_file_open(void)
{
	int         fd;
#ifdef linux
	fd = myopen(DEFAULT_CONF_FILENAME1, O_BINARY | O_RDONLY);
	if (fd >= 0) {
		printf("config '%s' open.\n", DEFAULT_CONF_FILENAME1);
		return (fd);
	}
	fd = myopen(DEFAULT_CONF_FILENAME2, O_BINARY | O_RDONLY);
	if (fd >= 0) {
		printf("config '%s' open.\n", DEFAULT_CONF_FILENAME2);
		return (fd);
	}
	fd = myopen(DEFAULT_CONF_FILENAME3, O_BINARY | O_RDONLY);
	if (fd >= 0) {
		printf("config '%s' open.\n", DEFAULT_CONF_FILENAME3);
		return (fd);
	}
#else
	char work[FILENAME_MAX];
	snprintf(work, FILENAME_MAX, "%s%s%s", current_dir.c_str(), DELIMITER, DEFAULT_CONF_FILENAME1);
	fd = myopen(work, O_BINARY | O_RDONLY);
	if (fd >= 0) {
		//              printf("config '%s' open.\n", DEFAULT_CONF_FILENAME1);
		return (fd);
	}
#endif
	return (-1);
}

/// <summary>
/// 読み込み行の整理
/// </summary>
/// <param name="line_buf">読み込みバッファ</param>
static void line_buffer_sanitize(char* line_buf)
{
	// '#'より後ろを削除。
	cut_after_character(line_buf, '#');
	// '\t'を' 'に置換
	replace_character(line_buf, "\t", " ");
	// ' 'が重なっているところを削除
	duplex_character_to_unique(line_buf, ' ');
	// 頭に' 'がいたら削除。
	ltrim(line_buf);
	// 最後に ' 'がいたら削除。
	rtrim(line_buf);
	return;
}
//========================================================
// 拡張子を渡すと、Content-type と、file_typeを返す。
//========================================================
void check_file_extension_to_mime_type(const char* file_extension, char* mime_type, int mime_type_size)
{
	int     i;
	strncpy(mime_type, DEFAULT_MIME_TYPE, mime_type_size-1);
	debug_log_output("file_extension='%s'\n", file_extension);
	// -------------------------------------------
	// ファイルの拡張子比較。Content-type を決定
	// -------------------------------------------
	for (i = 0;; i++) {
		if (mime_list[i].mime_name == NULL)
			break;
		if (strcasecmp(mime_list[i].file_extension, file_extension) == 0) {
			strncpy(mime_type, mime_list[i].mime_name, mime_type_size-1);
			break;
		}
	}
	debug_log_output("mime_type='%s'\n", mime_type);
	return;
}
void config_sanity_check()
{
	struct stat sb;
#ifdef linux
	char cwd[FILENAME_MAX];
	char buf[FILENAME_MAX];
	if (global_param.document_root[0] != '/') {
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			debug_log_output("document_root: getcwd(): %s", strerror(errno));
			exit(-1);
		}
		snprintf(buf, sizeof(buf), "%s/%s", cwd, global_param.document_root);
		strncpy(global_param.document_root, buf, sizeof(global_param.document_root)-1);
		debug_log_output("concatenated document_root: '%s'", global_param.document_root);
	}
#endif
	if (path_sanitize(global_param.document_root, sizeof(global_param.document_root)) == NULL) {
		debug_log_output("WARNING! weird path has been specified.");
		debug_log_output("falling back to the default document root.");
		strncpy(global_param.document_root, DEFAULT_DOCUMENT_ROOT
			, sizeof(global_param.document_root)-1);
	}
	if (stat(global_param.document_root, &sb) != 0) {
		debug_log_output("document_root: %s: %s", global_param.document_root, strerror(errno));
		exit(-1);
	}
	//TODO:statを機種固有ライブラリに移行
	//if (!S_ISDIR(sb.st_mode)) {
	//    debug_log_output("document_root: %s: is not a directory.", global_param.document_root);
	//    exit(-1);
	//}
	debug_log_output("document_root: '%s'", global_param.document_root);
}
