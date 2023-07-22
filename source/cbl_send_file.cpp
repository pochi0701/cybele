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

int  http_file_send(SOCKET accept_socket, char* filename, unsigned int content_length, unsigned int range_start_pos);
//int  next_file(int *in_fd_p, JOINT_FILE_INFO_T *joint_file_info_p);
long FileSize(char* file_name);
//static  int warifu = 0;
//static  unsigned char warifu_p[5000];

/// <summary>
/// ヘッダ応答
/// </summary>
/// <param name="accept_socket">ソケット</param>
/// <returns>ソケットに返却したデータ長</returns>
size_t HTTP_RECV_INFO::http_header_response(SOCKET accept_socket)
{
	size_t  t_content_length;
	int     send_header_data_len;
	wString send_http_header_buf;
	off_t   content_size = FileSize(static_cast<char*>(send_filename));

	// -------------------------------
	// ファイルサイズチェック
	// -------------------------------
	// end位置指定有り。
	if (range_end_pos > 0)
	{
		t_content_length = (range_end_pos - range_start_pos) + 1;
		send_header_data_len = send_http_header_buf.sprintf(
			"HTTP/1.0 206 Partial Content\r\n"
			"Accept-Ranges: bytes\r\n"
			HTTP_SERVER_NAME
			HTTP_CONNECTION
			HTTP_CONTENT_LENGTH
			"Content-Range: bytes %zu-%zu/%zu\r\n"
			HTTP_CONTENT_TYPE
			HTTP_END

			, SERVER_NAME
			, t_content_length
			, range_start_pos
			, range_end_pos
			, content_size
			, mime_type
		);
		//start位置指定なし
	}
	else if (range_start_pos == 0) {
		// 2004/07/22 Update end
		t_content_length = content_size;
		send_header_data_len = send_http_header_buf.sprintf(
			"HTTP/1.0 200 OK\r\n"
			"Accept-Ranges: bytes\r\n"
			HTTP_SERVER_NAME
			HTTP_CONNECTION
			HTTP_CONTENT_LENGTH
			HTTP_CONTENT_TYPE
			HTTP_END
			, SERVER_NAME
			, content_size
			, mime_type
		);
		// end位置指定無し。
	}
	else {
		// ファイルサイズチェック。
		t_content_length = content_size - range_start_pos;
		send_header_data_len = send_http_header_buf.sprintf(
			"HTTP/1.0 206 Partial Content\r\n"
			"Accept-Ranges: bytes\r\n"
			HTTP_SERVER_NAME
			HTTP_CONNECTION
			HTTP_CONTENT_LENGTH
			"Content-Range: bytes %zu-%zu/%zu\r\n"
			HTTP_CONTENT_TYPE
			HTTP_END

			, SERVER_NAME
			, t_content_length
			, range_start_pos
			, content_size - 1
			, content_size
			, mime_type
		);
	}
	// todo:なくして大丈夫？
	//send_header_data_len = send_http_header_buf.length();

	// --------------
	// ヘッダ返信
	// --------------
	//メモリ上にヘッダを作成して送出
	send(accept_socket, send_http_header_buf.c_str(), send_header_data_len, 0);
	debug_log_output("send_header_data_len = %d\n", send_header_data_len);
	debug_log_output("--------\n");
	debug_log_output("%s", send_http_header_buf.c_str());
	debug_log_output("--------\n");
	return t_content_length;
}

/// <summary>
/// ファイル実体の返信。
/// ヘッダ生成＆送信準備
/// </summary>
/// <param name="accept_socket">接続したソケット</param>
/// <returns>0</returns>
int HTTP_RECV_INFO::http_file_response(SOCKET accept_socket)
{
	// ---------------
	// 作業用変数初期化
	// ---------------
	// --------------
	// OK ヘッダ生成
	// --------------
	auto t_content_length = (unsigned int)http_header_response(accept_socket);
	// --------------
	// 実体返信
	// --------------
	http_file_send(accept_socket, send_filename, t_content_length, range_start_pos);
	return 0;
}

/// <summary>
/// ファイルの実体の送信実行部
/// </summary>
/// <param name="accept_socket">送受信ソケット</param>
/// <param name="filename">送信フルファイル名</param>
/// <param name="content_length">コンテンツ長</param>
/// <param name="range_start_pos">開始位置</param>
/// <returns></returns>
int http_file_send(SOCKET accept_socket, char* filename, unsigned int content_length, unsigned int range_start_pos)
{
	// ---------------------
	// ファイルオープン
	// ---------------------
	auto in_fd = myopen(filename, O_RDONLY | O_BINARY, S_IREAD);
	if (in_fd < 0) {
		debug_log_output("open() error.");
		return (-1);
	}
	//warifu = 0;
	//ファイル名置換開始
	//
	//ファイル名存在チェック
	//存在するなら割符ＯＮ
	//if (strstr(filename, "emu.mp4")) {
	//    warifu = 1;
	//    replace_character(filename, "emu.mp4", "warifu.mp4");
	//    //バッファリード
	//    // ファイルからデータを読み込む。必ず読める前提
	//    int in_fd2 = myopen(filename, O_RDONLY | O_BINARY, S_IREAD);
	//    long read_length2 = read(in_fd2, warifu_p, 5000);
	//    close(in_fd2);
	//}
	// ------------------------------------------
	// range_start_posへファイルシーク
	// ------------------------------------------

	if (range_start_pos) {
		auto seek_ret = lseek(in_fd, range_start_pos, SEEK_SET);
		if (seek_ret < 0) {// lseek エラーチェック
			debug_log_output("lseek() error.");
			close(in_fd);
			return (-1);
		}
	}

	// ================
	// 実体転送開始
	// ================
	if (copy_descriptors(in_fd, (int)accept_socket, content_length, NULL, NULL, 0) < 0) {
		return (-1);
	}
	// 正常終了
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// 


/// <summary>
/// データコピー登録
/// </summary>
/// <param name="in_fd">入力ファイルディスクリプタ</param>
/// <param name="out_fd">出力ファイルディスクリプタ</param>
/// <param name="content_length">コンテンツ長</param>
/// <param name="joint_file_info_p">DVD結合の場合</param>
/// <param name="name">名称</param>
/// <param name="range_start_pos">コンテンツ開始位置</param>
/// <returns>-1:ERROR  0:END</returns>
int copy_descriptors(int in_fd, int out_fd, unsigned int content_length, JOINT_FILE_INFO_T* joint_file_info_p, char* name, unsigned int range_start_pos)
{
	IGNORE_PARAMETER(joint_file_info_p);
	IGNORE_PARAMETER(name);
	IGNORE_PARAMETER(range_start_pos);
	return copy_body(in_fd, out_fd, content_length, range_start_pos);
}

/// <summary>
/// ノンブロッキングモード対応のコピー
/// </summary>
/// <param name="in_fd">入力ファイルディスクリプタ</param>
/// <param name="out_fd">出力ファイルディスクリプタ</param>
/// <param name="content_length">コンテンツ長</param>
/// <param name="range_start_pos">コンテンツ開始位置</param>
/// <returns> -1:ERROR  0:END</returns>
int copy_body(int in_fd, int out_fd, unsigned int content_length, unsigned int range_start_pos)
{
	int             target_read_size;
	unsigned char*	send_buf_p = reinterpret_cast<unsigned char*>(new char[SEND_BUFFER_SIZE]);
	int             current_read_size;
	unsigned int    total_read_size = 0;
	unsigned int    total_write_size = 0;
	// ================
	// 実体転送開始
	// ================
	while (1)
	{
		// 目標readサイズ計算 content_length==0も考慮
		if ((content_length - total_write_size) > SEND_BUFFER_SIZE || content_length == 0) {
			target_read_size = SEND_BUFFER_SIZE;
		}
		else {
			target_read_size = (content_length - total_write_size);
		}

		// ファイルからデータを読み込む。必ず読める前提
		auto read_length = read(in_fd, send_buf_p, target_read_size);
		//read end
		if (read_length == 0)
		{
			//読み終わった。contents_length変えるべき
			debug_log_output("rw end %d %d", in_fd, out_fd);
			//debug_log_output("%s(%d) in_fd",__FILE__,__LINE__);
			close(in_fd);
			//debug_log_output("%s(%d) out_fd",__FILE__,__LINE__);
			sClose(out_fd);
			delete[] send_buf_p;
			send_buf_p = 0;
			return 0;
			//read error
		}
		else if (read_length < 0) {
			//debug_log_output("%s(%d) in_fd",__FILE__,__LINE__);
			close(in_fd);
			//debug_log_output("%s(%d) out_fd",__FILE__,__LINE__);
			sClose(out_fd);
			delete[] send_buf_p;
			debug_log_output("read error error=%s\n", strerror(errno));
			return (-1);
			//読み込み正常終了
		}
		else {
			debug_log_output("Normal read %d", read_length);
			//読み込んだデータに割符復元処理を行う
			//処理開始 total_read_size+range_start_pos
			//if (warifu == 1) {
			//    long posstt = range_start_pos + total_read_size;
			//    long posend = range_start_pos + total_read_size + read_length;
			//    long iposstt = (posstt +4999)/ 5000;
			//    long iposend = posend / 5000;
			//    for (long i = iposstt; i <= iposend; i++) {
			//        //if (send_buf_p[i * 5000 - range_start_pos - total_read_size] != warifu_p[i]) {
			//        //    int aa = 1;
			//        //}
			//        send_buf_p[i * 5000 - range_start_pos - total_read_size] = warifu_p[i];
			//    }
			//    //A～Bに5000刻みがあればsend_buf_p[ptra] = warifu[ptrb]
			//    //処理終了 total_read_size+range_start_pos + read_length
			//}
			total_read_size += read_length;
			current_read_size = read_length;
		}
		// SOCKET にデータを送信
		auto write_length = send(out_fd, reinterpret_cast<char*>(send_buf_p), current_read_size, 0);
		//write error
		if (write_length < 0) {
			debug_log_output("send() error.%d %s\n", errno, strerror(errno));
			delete[] send_buf_p;       // Memory Free.
			debug_log_output("%s(%d) in_fd", __FILE__, __LINE__);
			close(in_fd);   // File Close
			debug_log_output("%s(%d) out_fd", __FILE__, __LINE__);
			sClose(out_fd);
			return (-1);
		}
		//書き込み更新
		total_write_size += write_length;
		if (content_length != 0)
		{
			debug_log_output("Streaming..  %ld / %ld ( %ld.%ld%% )\n",
				total_write_size, content_length,
				total_write_size * 100 / content_length,
				(total_write_size * 1000 / content_length) % 10);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
//int next_file(int *in_fd_p, JOINT_FILE_INFO_T *joint_file_info_p)
//{
//    if (in_fd_p && joint_file_info_p){
//        // 読み終わったファイルをCLOSE()
//        close(*in_fd_p);
//        // 次のファイルがあるか?
//        joint_file_info_p->current_file_num++;
//        if ( joint_file_info_p->current_file_num >= joint_file_info_p->file_num ){
//            return 1;           // これで終了
//        }
//        // 次のファイルをOPEN()
//        *in_fd_p = open(joint_file_info_p->file[joint_file_info_p->current_file_num].name, O_BINARY);
//        if ( *in_fd_p < 0 ){
//            return ( -1 );
//        }
//        return 0;               // 次のファイルの準備完了
//    } else {
//        // パラメータがNULLの場合には1ファイルのみの処理とする
//        return 1;               // これで終了
//    }
//}

/// <summary>
/// ファイルサイズ取得
/// </summary>
/// <param name="file_name">対象フルファイル名</param>
/// <returns>ファイルサイズ。エラー時-1</returns>
long FileSize(char* file_name)
{
	long flen;
	int  handle = myopen(file_name, O_RDONLY | O_BINARY, S_IREAD);
	if (handle < 0) {
		return -1;
	}
	flen = lseek(handle, 0, SEEK_END);
	close(handle);
	return flen;
}

