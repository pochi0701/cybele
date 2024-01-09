// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl.h
//		$Revision: 1.26 $
//		$Date: 2004/07/19 04:37:32 $
//
// ==========================================================================
#ifndef	_CBL_H
#define	_CBL_H
#include <map>
#include "const.h"
#include "cbl_http.h"
#include "cbl_String.h"
#include "TinyJS.h"

// ==========================================================================
// MIMEリスト保存用構造体
// ==========================================================================

/// <summary>
/// MIME_LIST処理
/// </summary>
class  MIME_LIST_T {
public:
	/// <summary>MIME型</summary>
	const char* mime_type;
	/// <summary>対応する拡張子</summary>
	const char* file_extension;
	/// <summary>ストリームタイプ</summary>
	STREAM_TYPE	stream_type;
	/// <summary>返却用ファイルタイプ</summary>
	MIME_TYPE menu_file_type;

	static void check_file_extension_to_mime_type(const char* file_extension, char* mime_type, int mime_type_size);
};

// ==========================================================================
// 全体パラメータ保存用構造体
// ==========================================================================
class GLOBAL_PARAM_T {
public:
	/// ---------------------------------------------------------------------
	/// <summary>コンストラクタ</summary>
	GLOBAL_PARAM_T()
	{
		flag_daemon = 0;
		flag_debug_log_output = 0;
		*debug_log_filename = 0;
		*exec_user = 0;
		*exec_group = 0;
		*server_name = 0;
		flag_auto_detect = 0;
		server_port = 0;
		*system_password = 0;
		*document_root = 0;
		*alias_key[0] = 0;
		*alias_rep[0] = 0;
		flag_use_skin = 0;
		*skin_root = 0;
		*skin_name = 0;
		*work_root = 0;
		*user_agent_proxy_override = 0;
		flag_execute_cgi = 0;
		flag_allow_proxy = 0;
		*global_ip = 0;
	}
	/// <summary>初期化</summary>
	void global_param_init(void);
	/// <summary>コンフィグファイルの読み込み</summary>
	void config_file_read(void);
	/// <summary>コンフィグファイル整理</summary>
	/// <param name=""></param>
	void config_sanity_check(void);
	/// ---------------------------------------------------------------------
	/// システム系
	/// <summary>デーモン化する/しない</summary>
	char flag_daemon;
	/// <summary>デバッグログ</summary>
	char flag_debug_log_output;
	char debug_log_filename[FILENAME_MAX];
	/// <summary>動作ユーザー名</summary>
	char exec_user[32];
	char exec_group[32];
	/// ---------------------------------------------------------------------
	/// 自動検出系
	/// <summary>サーバホスト名</summary>
	char server_name[32];
	char flag_auto_detect;
	/// ---------------------------------------------------------------------
	/// HTTP Server系
	/// <summary>HTTP Server Port</summary>
	int server_port;
	/// <summary>システムパスワード</summary>
	char system_password[30];
	/// <summary>Document Root</summary>
	char  document_root[FILENAME_MAX];
	/// <summary>ファイルパスエイリアス</summary>
	char alias_key[MAX_COUNT_ALIAS][FILENAME_MAX];
	char alias_rep[MAX_COUNT_ALIAS][FILENAME_MAX];
	/// ---------------------------------------------------------------------
	/// 表示系
	/// <summary>スキンを使用する／しない</summary>
	char flag_use_skin;
	/// <summary>スキン置き場</summary>
	char skin_root[FILENAME_MAX];
	/// <summary>スキン名</summary>
	char skin_name[32];
	/// <summary>ワーク作成場所</summary>
	char work_root[FILENAME_MAX];
	/// ---------------------------------------------------------------------
	/// 拡張系
	/// <summary>プロクシで User-Agent を上書きするならその文字列</summary>
	char user_agent_proxy_override[128];
	/// <summary>CGIスクリプトの実行を許可するかフラグ</summary>
	int flag_execute_cgi;
	/// <summary>プロクシを許可するかフラグ</summary>
	int flag_allow_proxy;
	/// <summary>Global IP</summary>
	char global_ip[16];

	/// <summary>
	/// 内部JavaScript
	/// </summary>
	static CTinyJS* sjss;
	/// ---------------------------------------------------------------------
private:
	/// <summary>
	/// １行読み込む
	/// 読み込んだ文字数がreturnされる。
	/// 最後まで読んだら、-1が戻る。
	/// </summary>
	/// <param name="fd">読み込みファイルディスクリプタ</param>
	/// <param name="line_buf">読み込みバッファ</param>
	/// <param name="line_buf_size">読み込みバッファサイズ</param>
	/// <returns></returns>
	static int config_file_read_line(int fd, char* line_buf, int line_buf_size);
	/// <summary>
	/// 文字列の真偽値valueを読み取ってflagに設定
	/// </summary>
	/// <param name="value">設定する文字列の真偽値</param>
	/// <param name="flag">設定される変数</param>
	void readTF(char* value, char& flag);
	/// <summary>
	/// 文字列の真偽値valueを読み取ってflagに設定
	/// </summary>
	/// <param name="value">設定する文字列の真偽値</param>
	/// <param name="flag">設定される変数</param>
	void readTF(char* value, int& flag);
	/// <summary>
	/// コンフィグファイルオープン
	/// </summary>
	/// <returns></returns>
	static int config_file_open(void);
	/// <summary>
	/// 行整理
	/// </summary>
	/// <param name="line_buf">整理する行</param>
	static void line_buffer_sanitize(char* line_buf);
};
// IPアドレスアクセスコントロール用
typedef struct {
	int		flag;			// 値が入ってるか
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
	unsigned int		file_num;					// 全ファイル数
	size_t			total_size;						// 全ファイル総byte数
	_FILE_INFO_T		file[JOINT_MAX];			// JOINTファイル情報
	unsigned int	current_file_num;				// とりあえずVOB専用
} JOINT_FILE_INFO_T;


typedef struct {
	SOCKET  accept_socket;							// SOCKET
	char* access_host;								// アクセスしてきたIP
	struct  sockaddr_in  caddr;
} ACCESS_INFO;
// 2004/08/02 Add test
// 2004/08/11 Add test
//typedef struct {
//	char			access_ip[32];					// アクセスしてきたIP
//	char*			user_name;						// ユーザー名
//	char*			user_pass;						// ユーザーパスワード
//	int				login_flag;						// ログインフラグ
//	time_t			login_time;						// ログイン時刻
//	int				file_send_flag;					// ファイル転送中フラグ(ファイル転送中はタイムアウトさせないため)
//	char			recv_uri_log[FILENAME_MAX];		// 受信したURI(decoded)のログ
//	char			recv_uri_last[FILENAME_MAX];	// 受信したURI(decoded)のログ
//} ACCESS_USER_INFO;
//typedef struct {
//	ACCESS_USER_INFO    user_info[MULTI_ACCESS_MAX];
//	int                 list_num;
//} ACCESS_USER_INFO_LIST;

/// <summary>
/// multipart
/// </summary>
class multipart
{
public:
	/// <summary>コンストラクタ</summary>
	multipart(void)
	{
		content = NULL;
		length = 0;
		name[0] = 0;
		fileName[0] = 0;
	}
	void* content;
	int length;
	char name[256];
	char fileName[256];
};


// ======================
// extern いろいろ
// ======================
// ------------------
// 各種リスト
// ------------------
extern GLOBAL_PARAM_T           global_param;
extern MIME_LIST_T              mime_list[];
extern ACCESS_CHECK_LIST_T      access_allow_list[ACCESS_ALLOW_LIST_MAX];     // アクセス許可リスト
//extern ACCESS_USER_INFO_LIST    user_info_list;
// ------------------
// グローバル関数
// ------------------
// cbl 初期化
extern int  Initialize(void);
extern int  Finalize(void);

// バッチ処理
#ifdef linux
extern void* batch(void* ptr);
#else
extern unsigned int __stdcall batch(void* ptr);
#endif
// HTTP 待ち受け部
extern void server_listen(void);
// HTTP処理部
extern void server_http_process(SOCKET accept_socket, char* access_host, char* client_addr_str);
// バッファリングしながら in_fd から out_fd へ データを転送
extern int copy_descriptors(int in_fd, int out_fd, unsigned int content_length, JOINT_FILE_INFO_T* joint_file_info_p, char* name, unsigned int range_start_pos);
// 日本語文字コード変換(NKFラッパー）
extern void convert_language_code(const char* in, char* out, size_t len, int in_flag, int out_flag);
int copy_body(int in_fd, int _out_fd, unsigned int content_length, unsigned int range_start_pos);
#define WEB
#endif
