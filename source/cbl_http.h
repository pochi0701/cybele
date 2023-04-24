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
#ifndef	_CBL_HTTP
#define	_CBL_HTTP
#include "const.h"
#include "cbl_String.h"
// ==========================================================================
// HTTP Request分類
// ==========================================================================
enum class FILETYPES {
    _OPENDIR = -2,
    _NOTFOUND = -1,
    _FILE = 0,
    _DIR = 1,
    _TMP = 2,
    _PLW = 3,
    _VOB = 5,
    _CGI = 6
};
enum class QUERY_METHOD
{
    NONE = -1,
    GET = 1,
    HEAD = 2,
    POST = 3,
    PUT = 4,
};
// ==========================================================================
// HTTP Request情報保存用構造体
// ==========================================================================
class HTTP_RECV_INFO {
private:
    char   recv_range[256];          // 受信した Range
    char   content_length[32];	     // Content-Length
    char   content_type[128];	     // PUTのためのContent_type
    char   cookie[2048];             // cookie;
public:
    char   recv_uri[QUERY_MAX];      // 受信したURI(decoded)
    char   user_agent[256];          // 受信したUser-Agent
    char   recv_host[256];           // 受信したホスト名
    off_t  range_start_pos;          // Rangeデータ 開始位置
    off_t  range_end_pos;            // Rangeデータ 終了位置
    char   mime_type[128];           //
    char   send_filename[QUERY_MAX]; // フルパス
    char   action[128];              // ?action=  の内容
    char   request_uri[QUERY_MAX];   // 受信した生のURI
    QUERY_METHOD    isGet;		     // GETなら1HEADなら2POSTなら3

    // JavaScript実行
    void jss(SOCKET accpet_socket, char* script_filename, char* query_string);
    // CGI解析＆返信
    int http_cgi_response(SOCKET accept_socket);
    // HTTP HEADER受信
    int http_header_receive(SOCKET accept_socket);
    // HTTP ファイルタイプチェック
    FILETYPES http_file_check(void);
    // default index
    FILETYPES http_index(void);
    // 301 file moved
    int http_redirect_response(SOCKET accept_socket, char* location);
    // 404 not found
    int http_not_found_response(SOCKET accept_socket);
    // ファイル実体返信
    int http_file_response(SOCKET accept_socket);
    //ヘッダ応答
    size_t http_header_response(SOCKET accept_socket);
    // Proxy解析＆返信
    int http_proxy_response(SOCKET accept_socket);
    // Header送信
    SOCKET send_header(void);
    //クリア
    void clear(void)
    {
        memset(this, 0, sizeof(HTTP_RECV_INFO));
        //memset(&http_recv_info, 0, sizeof(http_recv_info));
    }
};
#endif