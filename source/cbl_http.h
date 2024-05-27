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

/// <summary>
/// 0:ファイル実体
/// 1:ディレクトリ
/// 2:一時ファイル
/// 3:PLWファイル
/// 5:VOBファイル
/// 6:CGIファイル
/// </summary>
enum class FILETYPES {
    _OPENDIR = -2,
    _NOTFOUND = -1,
    _FILE = 0,
    _DIR = 1,
    _CGI = 3,
};
enum class QUERY_METHOD
{
    NONE = -1,
    GET = 1,
    HEAD = 2,
    POST = 3,
    PUT = 4,
    DEL = 5,
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
    char   action[256];              // ?action=  の内容
    char   request_uri[QUERY_MAX];   // 受信した生のURI
    char   boundary[128];            // multipart/fomr-dataの時のboundary
    QUERY_METHOD    method;		     // GETなら1HEADなら2POSTなら3

	HTTP_RECV_INFO()
	{
		*recv_range = 0;
		*content_length = 0;
		*content_type = 0;
		*cookie = 0;
		*recv_uri = 0;
		*user_agent = 0;
		*recv_host = 0;
		range_start_pos = 0;
		range_end_pos = 0;
		*mime_type = 0;
		*send_filename = 0;
		*action = 0;
		*request_uri = 0;
		*boundary = 0;
		method = QUERY_METHOD::NONE;
	};
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