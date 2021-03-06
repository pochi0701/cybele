﻿// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl.h
//		$Revision: 1.26 $
//		$Date: 2004/07/19 04:37:32 $
//
//	すべて自己責任でおながいしまつ。
// ==========================================================================
#ifndef	_CBL_H
#define	_CBL_H
#include <map>
#include "const.h"
#include "cbl_String.h"

// ==========================================================================
// MIMEリスト保存用構造体
// ==========================================================================
typedef struct {
    char   *mime_name;
    char   *file_extension;
    int	   stream_type;
    int	   menu_file_type;
} MIME_LIST_T;
// ==========================================================================
// HTTP Request情報保存用構造体
// ==========================================================================
typedef struct {
    char   recv_uri[QUERY_MAX];      // 受信したURI(decoded)
    //wString  recv_uri;               // 受信したURI(decoded)
    char   user_agent[256];          // 受信したUser-Agent
    char   recv_host[256];           // 受信したホスト名
    char   recv_range[256];          // 受信した Range
    off_t  range_start_pos;          // Rangeデータ 開始位置
    off_t  range_end_pos;            // Rangeデータ 終了位置
    char   content_length[32];	     // Content-Length
    char   content_type[128];	     // PUTのためのContent_type
    char   mime_type[128];           //
    char   send_filename[QUERY_MAX]; // フルパス
    char   action[128];              // ?action=  の内容
    char   request_uri[QUERY_MAX];   // 受信した生のURI
    char   cookie[2048];             // cookie;
    int    isGet;		     // GETなら1HEADなら2POSTなら3
} HTTP_RECV_INFO;
// ==========================================================================
// 全体パラメータ保存用構造体
// ==========================================================================
typedef struct {
    // -----------------
    // システム系
    // -----------------
    // デーモン化する/しない
    char        flag_daemon;
    // デバッグログ
    char        flag_debug_log_output;
    char	debug_log_filename[FILENAME_MAX];
    // 動作ユーザー名
    char        exec_user[32];
    char        exec_group[32];
    // -----------------
    // 自動検出系
    // -----------------
    // サーバホスト名
    char        server_name[32];
    char        flag_auto_detect;
    char        auto_detect_bind_ip_address[32];
    // --------------------
    // HTTP Server系
    // --------------------
    // HTTP Server Port
    int         server_port;
    // Document Root
    char        document_root[FILENAME_MAX];
    // Alias
    char        document_org[FILENAME_MAX];
    char        alias_key[MAX_COUNT_ALIAS][FILENAME_MAX];
    char        alias_rep[MAX_COUNT_ALIAS][FILENAME_MAX];
    // システムパスワード
    char        system_password[30];
    // ----------------------
    // 表示系
    // ----------------------
    // スキンを使用する／しない
    char        flag_use_skin;
    // スキン置き場
    char        skin_root[FILENAME_MAX];
    // スキン名
    char        skin_name[32];
    // ワーク作成場所
    char        work_root[FILENAME_MAX];
    // ----------------------
    // 拡張系
    // ----------------------
    // プロクシで User-Agent を上書きするならその文字列
    char	user_agent_proxy_override[128];
    // CGIスクリプトの実行を許可するかフラグ
    int         flag_execute_cgi;
    // プロクシを許可するかフラグ
    int         flag_allow_proxy;
} GLOBAL_PARAM_T;
// IPアドレスアクセスコントロール用
typedef struct {
    int				flag;			// 値が入ってるか
    char 	address[4];		// アドレス
    char 	netmask[4];		// ネットマスク
} ACCESS_CHECK_LIST_T;
#define		JOINT_MAX	(255)
// ********************************
// JOINTする個々のファイル情報
// ********************************
typedef struct {
    char   name[FILENAME_MAX];
    off_t   size;
} _FILE_INFO_T;
// ****************************************
// JOINTファイル情報 (VOB解析情報)
// ****************************************
typedef struct {
    unsigned int		file_num;		// 全ファイル数
    size_t			total_size;		// 全ファイル総byte数
    _FILE_INFO_T		file[JOINT_MAX];	// JOINTファイル情報
    unsigned int	current_file_num;			// とりあえずVOB専用
} JOINT_FILE_INFO_T;


typedef struct {
    SOCKET  accept_socket;          // SOCKET
    char    *access_host;           // アクセスしてきたIP
    struct  sockaddr_in  caddr;
} ACCESS_INFO;
// 2004/08/02 Add test
// 2004/08/11 Add test
typedef struct {
    char           access_ip[32];               // アクセスしてきたIP
    char           *user_name;                  // ユーザー名
    char           *user_pass;                  // ユーザーパスワード
    int            login_flag;                  // ログインフラグ
    time_t         login_time;                  // ログイン時刻
    int            file_send_flag;              // ファイル転送中フラグ(ファイル転送中はタイムアウトさせないため)
    char           recv_uri_log[FILENAME_MAX];  // 受信したURI(decoded)のログ
    char           recv_uri_last[FILENAME_MAX]; // 受信したURI(decoded)のログ
} ACCESS_USER_INFO;
typedef struct {
    ACCESS_USER_INFO    user_info[MULTI_ACCESS_MAX];
    int                 list_num;
} ACCESS_USER_INFO_LIST;
// ======================
// extern いろいろ
// ======================
// ------------------
// 各種リスト
// ------------------
extern GLOBAL_PARAM_T           global_param;
extern MIME_LIST_T              mime_list[];
extern ACCESS_CHECK_LIST_T      access_allow_list[ACCESS_ALLOW_LIST_MAX];     // アクセス許可リスト
extern ACCESS_USER_INFO_LIST    user_info_list;
// ------------------
// グローバル関数
// ------------------
// cbl 初期化
extern void global_param_init(void);
extern int  Initialize(void);
extern int  Finalize(void);

// config_file(cbl.conf) 読み込み部
extern void config_file_read(void);
extern void config_sanity_check(void);
// バッチ処理
#ifdef linux
extern void*    batch(void* ptr);
#else
extern unsigned int __stdcall batch(void* ptr);
#endif
// HTTP 待ち受け部
extern void server_listen(void);
// HTTP処理部
extern void server_http_process(int accept_socket , char* access_host , char* client_addr_str );
// バッファリングしながら in_fd から out_fd へ データを転送
extern int copy_descriptors(int in_fd, int out_fd, off_t content_length, JOINT_FILE_INFO_T *joint_file_info_p,char* name,off_t range_start_pos);

// Proxy解析＆返信
extern int http_proxy_response(int accept_socket, HTTP_RECV_INFO *http_recv_info_p);
// CGI解析＆返信
extern int http_cgi_response(int accept_socket, HTTP_RECV_INFO *http_recv_info_p );
// ファイル実体返信
extern int http_file_response(int accept_socket, HTTP_RECV_INFO *http_recv_info_p );
// MIME
extern void check_file_extension_to_mime_type(const char *file_extension, char *mime_type, int mime_type_size );
// 日本語文字コード変換(NKFラッパー）
extern void convert_language_code(const char *in, char *out, size_t len, int in_flag, int out_flag);
int copy_body(int in_fd, int _out_fd, size_t content_length);
#define WEB
#endif
