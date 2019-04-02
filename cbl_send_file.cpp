#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_send_file.cpp
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
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <error.h>
#include <cerrno>
#include <sys/epoll.h>
#include <time.h>
#else
#include <errno.h>
#include <windows.h>
#include <io.h>
#include <process.h>
#include <signal.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
//#include <dirent.h>


#include "cbl.h"
#include "cbl_tools.h"
#include "cbl_String.h"
#include "define.h"

int  http_file_send(int accept_socket, char *filename, size_t content_length, size_t range_start_pos );
int  next_file(int *in_fd_p, JOINT_FILE_INFO_T *joint_file_info_p);
long FileSize(char* file_name);

//ヘッダ応答
size_t http_header_response(int accept_socket, HTTP_RECV_INFO* http_recv_info_p)
{
    size_t  content_length;
    int     send_header_data_len;
    wString send_http_header_buf;
    off_t   content_size = FileSize( (char*)http_recv_info_p->send_filename);

    // -------------------------------
    // ファイルサイズチェック
    // -------------------------------
    // end位置指定有り。
    if ( http_recv_info_p->range_end_pos > 0 )
    {
        content_length       = (http_recv_info_p->range_end_pos - http_recv_info_p->range_start_pos) + 1;
        send_header_data_len = send_http_header_buf.sprintf(
        "HTTP/1.0 206 Partial Content\r\n"
        "Accept-Ranges: bytes\r\n"
        HTTP_SERVER_NAME
        HTTP_CONNECTION
        HTTP_CONTENT_LENGTH
        "Content-Range: bytes %zu-%zu/%zu\r\n"
        HTTP_CONTENT_TYPE
        HTTP_END
        
        ,SERVER_NAME
        ,content_length
        ,http_recv_info_p->range_start_pos
        ,http_recv_info_p->range_end_pos
        ,content_size
        ,http_recv_info_p->mime_type
        );
    //start位置指定なし
    }else if ( http_recv_info_p->range_start_pos == 0 ){
        // 2004/07/22 Update end
        content_length = content_size;
        send_header_data_len = send_http_header_buf.sprintf(
        "HTTP/1.0 200 OK\r\n"
        "Accept-Ranges: bytes\r\n"
        HTTP_SERVER_NAME
        HTTP_CONNECTION
        HTTP_CONTENT_LENGTH
        HTTP_CONTENT_TYPE
        HTTP_END
        ,SERVER_NAME
        ,content_size
        ,http_recv_info_p->mime_type
        );
    // end位置指定無し。
    }else{
        // ファイルサイズチェック。
        content_length = content_size - http_recv_info_p->range_start_pos;
        send_header_data_len = send_http_header_buf.sprintf(
        "HTTP/1.0 206 Partial Content\r\n"
        "Accept-Ranges: bytes\r\n"
        HTTP_SERVER_NAME
        HTTP_CONNECTION
        HTTP_CONTENT_LENGTH
        "Content-Range: bytes %zu-%zu/%zu\r\n"
        HTTP_CONTENT_TYPE
        HTTP_END
        
        ,SERVER_NAME
        ,content_length
        ,http_recv_info_p->range_start_pos
        ,content_size-1
        ,content_size
        ,http_recv_info_p->mime_type
        );
    }
    send_header_data_len = send_http_header_buf.length();
    
    // --------------
    // ヘッダ返信
    // --------------
    //メモリ上にヘッダを作成して送出
    send(accept_socket, send_http_header_buf.c_str(), send_header_data_len, 0);
    debug_log_output("send_header_data_len = %d\n", send_header_data_len);
    debug_log_output("--------\n");
    debug_log_output("%s", send_http_header_buf.c_str());
    debug_log_output("--------\n");
    return content_length;
}

// **************************************************************************
// ファイル実体の返信。
// ヘッダ生成＆送信準備
// **************************************************************************
int http_file_response(int accept_socket, HTTP_RECV_INFO *http_recv_info_p)
{
    // ---------------
    // 作業用変数初期化
    // ---------------
    size_t      content_length;
    // --------------
    // OK ヘッダ生成
    // --------------
    content_length = http_header_response(accept_socket, http_recv_info_p);
    // --------------
    // 実体返信
    // --------------
    http_file_send(accept_socket, http_recv_info_p->send_filename,content_length,http_recv_info_p->range_start_pos );
    return 0;
}
// **************************************************************************
// ファイルの実体の送信実行部
// **************************************************************************
int http_file_send(int accept_socket, char *filename, size_t content_length, size_t range_start_pos )
{
    int		in_fd;
    long	seek_ret;
    
    // ---------------------
    // ファイルオープン
    // ---------------------
    in_fd = myopen(filename, O_RDONLY | O_BINARY,S_IREAD);
    if ( in_fd < 0 ) {
        debug_log_output("open() error.");
        return ( -1 );
    }
    
    
    // ------------------------------------------
    // range_start_posへファイルシーク
    // ------------------------------------------

    if( range_start_pos ){
        seek_ret = lseek(in_fd, range_start_pos, SEEK_SET);
        if ( seek_ret < 0 )     {// lseek エラーチェック
            debug_log_output("lseek() error.");
        close(in_fd);
            return ( -1 );
        }
    }

    // ================
    // 実体転送開始
    // ================
    if( copy_descriptors(in_fd,accept_socket,content_length,NULL,NULL,0)<0 ) {
        return ( -1 );
    }
    // 正常終了
    return 0;
}
/////////////////////////////////////////////////////////////////////////////
// データコピー登録
int copy_descriptors(int in_fd, int out_fd, off_t content_length, JOINT_FILE_INFO_T *joint_file_info_p,char* name,off_t range_start_pos)
{
    IGNORE_PARAMETER(joint_file_info_p);
    IGNORE_PARAMETER(name);
    IGNORE_PARAMETER(range_start_pos);
    return copy_body(in_fd,out_fd,content_length);
}
//ノンブロッキングモード対応のコピー
// -1:ERROR  0:END
int copy_body(int in_fd, int out_fd, size_t content_length )
{
    int                    read_length;
    int                    write_length;
    int                    target_read_size;
    unsigned char*         send_buf_p       = (unsigned char*)new char[SEND_BUFFER_SIZE];
    int                    current_read_size;
    size_t                 total_read_size  =0;
    size_t                 total_write_size =0;
    // ================
    // 実体転送開始
    // ================
    while ( 1 )
    {
        // 目標readサイズ計算 content_length==0も考慮
        if ( (content_length - total_write_size) > SEND_BUFFER_SIZE || content_length == 0) {
            target_read_size = SEND_BUFFER_SIZE;
        }else{
            target_read_size = (size_t)(content_length - total_write_size);
        }
        
        // ファイルからデータを読み込む。必ず読める前提
        read_length = read(in_fd, send_buf_p, target_read_size);
        //read end
        if ( read_length == 0 )
        {
            //読み終わった。contents_length変えるべき
            debug_log_output("rw end %d %d", in_fd, out_fd );
            //debug_log_output("%s(%d) in_fd",__FILE__,__LINE__);
            close(in_fd);
            //debug_log_output("%s(%d) out_fd",__FILE__,__LINE__);
            sClose(out_fd);
            delete[] send_buf_p;
            send_buf_p=0;
            return 0;
        //read error
        }else if ( read_length < 0 ){
            //debug_log_output("%s(%d) in_fd",__FILE__,__LINE__);
            close(in_fd);
            //debug_log_output("%s(%d) out_fd",__FILE__,__LINE__);
            sClose(out_fd);
            delete[] send_buf_p;
            debug_log_output("read error error=%s\n", strerror(errno));
            return ( -1 );
        //読み込み正常終了
        }else{
            debug_log_output("Normal read %d", read_length );
            total_read_size += read_length;
            current_read_size = read_length;
        }
        // SOCKET にデータを送信
        write_length = send(out_fd, (char*)send_buf_p, current_read_size,0);
        //write error
        if ( write_length < 0) {
            debug_log_output("send() error.%d %s\n", errno,strerror(errno));
            delete[] send_buf_p;       // Memory Free.
            debug_log_output("%s(%d) in_fd",__FILE__,__LINE__);
            close(in_fd);   // File Close
            debug_log_output("%s(%d) out_fd",__FILE__,__LINE__);
            sClose(out_fd);
            return ( -1 );
        }
        //書き込み更新
        total_write_size += write_length;
        if ( content_length != 0 )
        {
            debug_log_output("Streaming..  %ld / %ld ( %ld.%ld%% )\n",
            total_write_size, content_length,
            total_write_size * 100 / content_length,
            (total_write_size * 1000 / content_length ) % 10 );
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
int next_file(int *in_fd_p, JOINT_FILE_INFO_T *joint_file_info_p)
{
    if (in_fd_p && joint_file_info_p){
        // 読み終わったファイルをCLOSE()
        close(*in_fd_p);
        // 次のファイルがあるか?
        joint_file_info_p->current_file_num++;
        if ( joint_file_info_p->current_file_num >= joint_file_info_p->file_num ){
            return 1;           // これで終了
        }
        // 次のファイルをOPEN()
        *in_fd_p = open(joint_file_info_p->file[joint_file_info_p->current_file_num].name, O_BINARY);
        if ( *in_fd_p < 0 ){
            return ( -1 );
        }
        return 0;               // 次のファイルの準備完了
    } else {
        // パラメータがNULLの場合には1ファイルのみの処理とする
        return 1;               // これで終了
    }
}
long FileSize(char* file_name)
{
    long flen;
    int  handle  = myopen(file_name,O_RDONLY | O_BINARY , S_IREAD );
    if( handle<0 ){
        return -1;
    }
    flen = lseek(handle,0,SEEK_END);
    close(handle);
    return flen;
}

