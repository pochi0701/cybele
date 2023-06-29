#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_tools.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#include  <ctype.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <string.h>
#include  <limits.h>
#include  <errno.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <time.h>

#ifdef linux
#include  <fcntl.h>
#include  <unistd.h>
#include  <sys/time.h>
#include  <sys/socket.h>
#include  <sys/un.h>
#include  <netdb.h>
#include  <pthread.h>
#else
#include  <errno.h>
#include  <io.h>
#include  <process.h>
#endif
//#include  "cbl.h"
//#include  "const.h"
#include "cbl_tools.h"
#include "define.h"
static char debug_log_filename[FILENAME_MAX];	// デバッグログ出力ファイル名(フルパス)
static char debug_log_initialize_flag = (1);	// デバッグログ初期化フラグ
static void cut_before_n_length(char* sentence, unsigned int n);
static void cut_after_n_length(char* sentence, unsigned int n);

/// <summary>
/// CRLFCRLFのパターンを見つけて文字列化
/// </summary>
/// <param name="start">開始ポインタ</param>
/// <param name="end">終端ポインタ</param>
/// <returns>パターンのエンドポイント</returns>
char* seekCRLFCRLF(char* start, char* end)
{
	while (start <= end-4)
	{
		char* nptr = start;
		if (*start++ == '\r')
		{
			if (*start++ == '\n')
			{
				if (*start++ == '\r')
				{
					if (*start++ == '\n')
					{
						*nptr = 0;
						return start;
					}
				}
			}
		}
	}
	return NULL;
}

/// <summary>
/// 部分文字列の位置を示す  
/// </summary>
/// <param name="haystack">全データ</param>
/// <param name="haystacklen">全データ長</param>
/// <param name="needle">探索データ</param>
/// <param name="needlelen">探索データ長さ</param>
/// <returns></returns>
void* memmem(const void* haystack, size_t haystacklen, const void* needle, size_t needlelen)
{
	const char* begin = static_cast<const char*>(haystack);
	const char* last = begin + haystacklen - needlelen;

	if (needlelen == 0) {
		return (void*)begin;
	}

	for (; begin <= last; begin++) {
		if (*begin == *static_cast<const char*>(needle) && memcmp(begin, needle, needlelen) == 0) {
			return (void*)begin;
		}
	}

	return NULL;
}
/// <summary>
/// 最初に出て来たcut_charの前後を分割
/// </summary>
/// <param name="pattern">切り出し対象文字列</param>
/// <param name="split1">前の文字列&元の文字列</param>
/// <param name="split2">後の文字列</param>
/// <returns>成功:true/失敗:false</returns>
bool split(const char* pattern, wString& split1, wString& split2)
{
	int pos = split2.Pos(pattern);
	if (pos == wString::npos) {
		split1 = split2;
		split2.clear();
		return false;
	}
	else {
		split1 = split2.substr(0, pos);
		split2 = split2.substr(pos + 1);
		return true;
	}
}

/********************************************************************************/
// sentence文字列内のkey文字列をrep文字列で置換する。
/********************************************************************************/
void replace_character(char* sentence, const char* key, const char* rep)
{
	int klen = (int)strlen(key);
	int rlen = (int)strlen(rep);
	int slen = (int)strlen(sentence);
	int num;
	if (klen == 0 || slen == 0) {
		return;
	}
	auto p = strstr(sentence, key);
	if (klen == rlen) {
		while (p != NULL) {
			memcpy(static_cast<void*>(p), static_cast<const void*>(rep), rlen);
			p = strstr(p + rlen, key);
		}
		//前詰め置換そのままコピーすればいい
	}
	else if (klen > rlen) {
		num = klen - rlen;
		while (p != NULL) {
			auto q = p;
			while (1) {
				*q = *(q + num);
				if (*(q + num) == 0) {
					break;
				}
				q++;
			}
			memcpy(p, rep, rlen);
			p = strstr(p + rlen, key);
			//slen -= num;
		}
		//置換文字が長いので後詰めする
	}
	else {
		while (p != NULL) {
			num = rlen - klen;
			//pからrlen-klenだけのばす
			for (auto str = sentence + slen + num; str - num >= p; str--) {
				*str = *(str - num);
			}
			memcpy(p, rep, rlen);
			p = strstr(p + rlen, key);
			slen += num;
		}
	}
	//myfree(buf);
	return;
}
/// <summary>
/// entence文字列内の最初のkey文字列をrep文字列で置換する。
/// </summary>
/// <param name="sentence">対象文字列</param>
/// <param name="key">置換元文字</param>
/// <param name="rep">置換先文字</param>
void replace_character_first(char* sentence, const char* key, const char* rep)
{
	auto klen = (int)strlen(key);
	auto rlen = (int)strlen(rep);
	auto slen = (int)strlen(sentence);
	int num;
	if (klen == 0 || slen == 0) {
		return;
	}
	auto p = strstr(sentence, key);
	if (p == 0)
	{
		return;
	}
	if (klen == rlen) {
		memcpy(p, rep, rlen);
		//前詰め置換そのままコピーすればいい
	}
	else if (klen > rlen)
	{
		num = klen - rlen;
		strcpy(p, (p + num));
		memcpy(p, rep, rlen);
		//置換文字が長いので後詰めする
	}
	else
	{
		num = rlen - klen;
		//pからrlen-klenだけのばす
		for (auto str = sentence + slen + num; str - num >= p; str--) {
			*str = *(str - num);
		}
		memcpy(p, rep, rlen);
	}
	return;
}
//***************************************************************************
// sentence文字列より、cut_charから後ろを削除
//      見つからなければ何もしない。
// 入力  : char* sentence 入力文字列
//         char  cut_char 切り取り文字
// 戻り値: char* 切り取った後ろの文字列
//***************************************************************************
char* cut_after_character(char* sentence, char cut_char)
{
	char* symbol_p;
	// 削除対象キャラクターがあった場合、それから後ろを削除。
	symbol_p = strchr(sentence, cut_char);
	if (symbol_p != NULL) {
		*symbol_p++ = '\0';
	}
	return symbol_p;
}
//************************************************************************
// sentence文字列の、cut_charが最後に出てきた所から後ろをCUT
// もし、cut_charがsentence文字列に入っていなかった場合、文字列全部削除。
//************************************************************************
void 	cut_after_last_character(char* sentence, char cut_char)
{
	char* symbol_p;
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	// 削除対象キャラクターが最後に出てくる所を探す。
	symbol_p = strrchr(sentence, cut_char);
	if (symbol_p == NULL) {
		// 発見できなかった場合、文字列全部削除。
		*sentence = '\0';
		return;
	}
	*symbol_p = '\0';
}

/// <summary>
/// sentenceの、後ろ n byteを残して削除。
/// </summary>
/// <param name="sentence">対象文字列</param>
/// <param name="n">バイト数</param>
void    cut_before_n_length(char* sentence, unsigned int n)
{
#if 0
	unsigned int len;
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	len = strlen(sentence);
	// sentence が、nよりも同じか短いならばreturn
	if (len <= n) {
		return;
	}
	strncpy(sentence, (sentence + len - n), n + 1);
#else
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	auto sentence_len = (unsigned int)strlen(sentence);
	// sentence が、nよりも同じか短いならばreturn
	if (sentence_len <= n) {
		return;
	}
	// テンポラリエリアmalloc.
	auto malloc_p = static_cast<char*>(malloc(sentence_len + 10));
	if (malloc_p == NULL) {
		return;
	}
	auto work_p = sentence;
	work_p += sentence_len;
	work_p -= n;
	strncpy(malloc_p, work_p, sentence_len + 10);
	strncpy(sentence, malloc_p, sentence_len);
	free(malloc_p);
	return;
#endif
}
/// <summary>
/// sentenceの、後ろ n byteを削除
/// 全長がn byteに満たなかったら、文字列全部削除
/// </summary>
/// <param name="sentence"></param>
/// <param name="n"></param>
void    cut_after_n_length(char* sentence, unsigned int n)
{
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	auto sentence_len = (unsigned int)strlen(sentence);
	// sentence が、nよりも同じか短いならば、全部削除してreturn;
	if (sentence_len <= n) {
		*sentence = 0;
		//strncpy(sentence, "", sentence_len);
		return;
	}
	// 後ろ n byteを削除
	auto work_p = sentence;
	work_p += sentence_len;
	work_p -= n;
	*work_p = '\0';
	return;
}
/// <summary>
/// sentence文字列の、cut_charを抜く。
/// </summary>
/// <param name="sentence">対象文字列</param>
/// <param name="cut_char">カットする文字</param>
void    cut_character(char* sentence, char cut_char)
{
	char* work1;
	char* work2;
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	work1 = sentence;
	work2 = sentence;
	// 処理ループ。
	while (*work1) {
		// 削除対象のキャラクターがいたら、それを飛ばす。
		if (*work1 == cut_char) {
			work1++;
		}
		else {
			*work2++ = *work1++;
		}
	}
	// '\0' をコピー。
	*work2 = '\0';
	return;
}
/// <summary>
/// 文字列の左端文字削除
/// </summary>
/// <param name="sentence">対象文字列</param>
/// <param name="cut_char">削除文字（省略値=' '）</param>
void ltrim(char* sentence, char cut_char)
{
	char* p = sentence;
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	// 削除対象キャラクターがあるかぎり進める。
	while ((*p == cut_char) && *p) {
		p++;
	}
	// sentence書き換え
	if (p != sentence) {
		while (*p) {
			*sentence++ = *p++;
		}
		*sentence = 0;
		//string corupped bug.
		//strcpy((char*)sentence, (char*)p);
	}
	return;
}

/// <summary>
/// 文字列の右端文字削除
/// </summary>
/// <param name="sentence">対象文字列</param>
/// <param name="cut_char">削除文字（省略値=' '）</param>
void rtrim(char* sentence, char cut_char)
{
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	auto length = (int)strlen(sentence);  // 文字列長Get
	auto source_p = sentence;
	source_p += length;         // ワークポインタを文字列の最後にセット。
	for (auto i = 0; i < length; i++) {// 文字列の数だけ繰り返し。
		source_p--;                     // 一文字ずつ前へ。
		if (*source_p == cut_char) {// 削除キャラ ヒットした場合削除
			*source_p = '\0';
		}
		else {// 違うキャラが出てきたところで終了。
			break;
		}
	}
	return;
}
/********************************************************************************/
// sentence文字列内のunique_charが連続しているところを、unique_char1文字だけにする。
/********************************************************************************/
void duplex_character_to_unique(char* sentence, char unique_char)
{
	char* p1;
	char* p2;
	int                 unique_char_count = 0;
	if (sentence == NULL || *sentence == 0) {
		return;
	}
	p1 = sentence;
	p2 = sentence;
	// sensense文字列から、unique_char以外をワークへコピー。
	while (*p1) {
		// unique_char発見
		if (*p1 == unique_char) {
			// 最初の一つならコピー。それ以外ならスキップ。
			if (unique_char_count == 0) {
				*p2++ = *p1++;
			}
			else {
				p1++;
			}
			unique_char_count++;
			// unique_char 以外ならコピー。
		}
		else {
			unique_char_count = 0;
			*p2++ = *p1++;
		}
	}
	*p2 = '\0';
	return;
}
//*********************************************************
// sentence文字列より、最初に出て来たcut_charの前後を分割。
//
//      sentence        (IN) 分割対象の文字列を与える。
//      cut_char        (IN) 分割対象の文字を入れる。
//      split1          (OUT)カットされた前の部分が入る。sentenceと同等のサイズが望ましい。
//      split2          (OUT)カットされた後ろの部分が入る。sentenceと同等のサイズが望ましい。
//
//
// return
//              0:                      正常終了。
//              それ以外：      エラー。分割失敗などなど。
//*********************************************************
int sentence_split(char* sentence, char cut_char, char* split1, char* split2)
{
	char* p = sentence;
	char* pos;
	// エラーチェック。
	if (sentence == NULL || *sentence == 0 || split1 == NULL || split2 == NULL) {
		return 1;       // 引数にNULLまじり。
	}
	pos = strchr(sentence, cut_char);
	// 分割文字発見できず。
	if (pos == NULL) {
		return 1;
	}
	//比較しながら複写
	while (*p) {
		//分割文字より前半
		if (p < pos) {
			*split1++ = *p++;
			//分割文字より後半
		}
		else if (p > pos) {
			*split2++ = *p++;
			//分割文字位置
		}
		else {
			p++;
		}
	}
	//糸止め
	*split1 = 0;
	*split2 = 0;
	return 0; // 正常終了。
}
//******************************************************************
// filenameから、拡張子を取り出す('.'も消す）
// '.'が存在しなかった場合、拡張子が長すぎた場合は、""が入る。
//******************************************************************
void filename_to_extension(char* filename, char* extension_buf, unsigned int extension_buf_size)
{
	// 拡張子の存在チェック。
	auto p = strrchr(filename, '.');
	if ((p == NULL) || (strlen(p) > extension_buf_size)) {
		*extension_buf = 0;
		//strncpy(extension_buf, "", extension_buf_size );
		return;
	}
	// 拡張子を切り出し。
	p++;
	strncpy(extension_buf, p, extension_buf_size);
	return;
}
// **************************************************************************
// text_buf から、CR/LF か'\0'が現れるまでを切り出して、line_bufにcopy。
// (CR/LFはcopyされない)
// 次の行の頭のポインタをreturn。
// Errorか'\0'が現れたらNULLが戻る。
// **************************************************************************
//char* buffer_distill_line(char* text_buf_p, char* line_buf_p, unsigned int line_buf_size)
//{
//	char* p;
//	unsigned int                counter = 0;
//	p = text_buf_p;
//	// ------------------
//	// CR/LF '\0'を探す
//	// ------------------
//	while (1) {
//		if (*p == '\r') { // CR
//			p++;
//			continue;
//		}
//		if (*p == '\n') { // LF
//			p++;
//			break;
//		}
//		if (*p == '\0') {
//			break;
//		}
//		p++;
//		counter++;
//	}
//	// --------------------------------------------------
//	// 数えた文字数だけ、line_buf_p に文字列をコピー
//	// --------------------------------------------------
//	memset(line_buf_p, '\0', line_buf_size);
//	if (counter >= line_buf_size) {
//		counter = (line_buf_size - 1);
//	}
//	strncpy(line_buf_p, text_buf_p, counter);
//	if (*p == '\0') {
//		return NULL;            // バッファの最後
//	}
//	else {
//		return p;               // バッファの途中
//	}
//}
// **************************************************************************
//  URIエンコードを行います.
//  機能 : URIデコードを行う
//  書式 : int uri_encode
//  (char* dst,size_t dst_len,const char* src,int src_len);
//  引数 : dst 変換した文字の書き出し先.
//                 dst_len 変換した文字の書き出し先の最大長.
//                 src 変換元の文字.
//                 src_len 変換元の文字の長さ.
//  返値 : エンコードした文字の数(そのままも含む)
// **************************************************************************
//int uri_encode(char* dst, unsigned int dst_len, const char* src, unsigned int src_len)
//{
//	unsigned int idx_src;
//	unsigned int idx_dst;
//	int cnt;
//	// 引数チェック
//	if ((dst == NULL) || (dst_len < 1) || (src == NULL) || (src_len < 1)) {
//		return 0;
//	}
//	cnt = 0;
//	for (idx_src = idx_dst = 0; (idx_src < src_len) && (idx_dst < dst_len) && (src[idx_src] != '\0'); idx_src++) {
//		/* ' '(space) はちと特別扱いにしないとまずい */
//		if (src[idx_src] == ' ') {
//			//dst[idx_dst++] = '+';
//			dst[idx_dst++] = '%';
//			dst[idx_dst++] = '2';
//			dst[idx_dst++] = '0';
//		}
//		/* エンコードしない文字全員集合 */
//		else if (strchr("!$()*,-./:;?@[]^_`{}~", src[idx_src]) != NULL) {
//			dst[idx_dst] = src[idx_src];
//			idx_dst += 1;
//		}
//		/* アルファベットと数字はエンコードせずそのまま */
//		else if (isalnum(src[idx_src])) {
//			dst[idx_dst] = src[idx_src];
//			idx_dst += 1;
//		}
//		/* \マークはエンコード */
//		else if (strchr(DELIMITER, src[idx_src]) != NULL) {
//			dst[idx_dst++] = '%';
//			dst[idx_dst++] = '5';
//			dst[idx_dst++] = 'C';
//		}
//		/* それ以外はすべてエンコード */
//		else {
//			if ((idx_dst + 3) > dst_len)
//				break;
//			idx_dst += sprintf(&dst[idx_dst], "%%%2X", (unsigned char)(src[idx_src]));
//		}
//		cnt++;
//		if ((idx_dst + 1) < dst_len) {
//			dst[idx_dst] = '\0';
//		}
//	}
//	return cnt;
//	// 2004/10/01 Update end
//}
// **************************************************************************
// URIデコードを行います.
//  機能 : URIデコードを行う
//  引数 : dst 変換した文字の書き出し先.
//                dst_len 変換した文字の書き出し先の最大長.
//                src 変換元の文字.
//                src_len 変換元の文字の長さ.
// 返値 : デコードした文字の数(そのままも含む)
// **************************************************************************
int uri_decode(char* dst, unsigned int dst_len, const char* src, unsigned int src_len)
{
	unsigned int    idx_src;
	unsigned int    idx_dst;
	int             cnt;
	char            work[3] = { 0 };
	//const char    *ptr_stop;
	char* strtol_end_ptr;
	int             code;
	// 引数チェック
	if ((dst == NULL) || (dst_len < 1) || (src == NULL) || (src_len < 1)) {
		return 0;
	}
	cnt = 0;
	// =================
	// メインループ
	// =================
	for (idx_src = idx_dst = 0; (idx_src < src_len) && (idx_dst < dst_len) && (src[idx_src] != '\0'); idx_dst++, cnt++) {
		if (src[idx_src] == '%') {
			if (idx_src + 2 > src_len) {
				break;
			}
			work[0] = src[idx_src + 1];
			work[1] = src[idx_src + 2];
			work[2] = '\0';
			code = strtol(work, &strtol_end_ptr, 16);
			//ptr_stop = &src[idx_src + (strtol_end_ptr - work) + 1];
			if (code == LONG_MIN || code == LONG_MAX) {
				break;
			}
			if (strtol_end_ptr != NULL) {
				if (*strtol_end_ptr != '\0') {
					break;
				}
			}
			dst[idx_dst] = (char)code;
			idx_src += 3;
		}
		else if (src[idx_src] == '+') {
			dst[idx_dst] = ' ';
			idx_src += 1;
			//ptr_stop++;
		}
		else {
			dst[idx_dst] = src[idx_src];
			idx_src += 1;
			//ptr_stop++;
		}
		if (idx_dst + 1 < dst_len) {
			dst[idx_dst + 1] = '\0';
		}
	}
	return cnt;
}

#ifndef linux
/// <summary>
/// VC用gettimeofdayの定義がない場合のソース
/// </summary>
/// <param name="tp"></param>
/// <param name="tzp">タイムゾーン。null推奨</param>
/// <returns>0:互換性のため</returns>
int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
	IGNORE_PARAMETER(tzp);
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970 
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}
#endif


/// <summary>
/// "YYYY/MM/DD HH:MM:SS.SSS" 形式の現在の日時の文字列を生成する。
/// </summary>
/// <param name="sentence"></param>
void make_datetime_string(char* sentence)
{
	*sentence = 0;
	struct tm now;				// 年/月/日/時/分/秒などのメンバを持つ構造体
	struct timeval total_usec;	// 経過秒取得用変数
	time_t t;					// 変換用

	// 現在時刻(経過秒)取得
	if (gettimeofday(&total_usec, NULL) == -1) {
		debug_log_output("gettimeofday ERRNO=%d", errno);
		return;
	}

	/* 経過秒変換 */

	t = total_usec.tv_sec;
	localtime_r(&t, &now); // 経過秒を日本時刻に変換

	sprintf(sentence, "%04d/%02d/%02d %02d:%02d:%02d.%03d",
		now.tm_year + 1900,				// 年
		now.tm_mon + 1,					// 月
		now.tm_mday,					// 日
		now.tm_hour,					// 時刻
		now.tm_min,						// 分
		now.tm_sec,						// 秒
		(int)total_usec.tv_usec/1000);	// ミリ秒
	return;

}
//*******************************************************************
// デバッグ出力初期化(ファイル名セット)関数
// この関数を最初に呼ぶまでは、デバッグログは一切出力されない。
//*******************************************************************
void debug_log_initialize(const char* set_debug_log_filename)
{
	// 引数チェック
	if (set_debug_log_filename == NULL) {
		return;
	}
	if (strlen(set_debug_log_filename) == 0) {
		return;
	}
	// デバッグログファイル名をセット。
	strncpy(debug_log_filename, set_debug_log_filename, sizeof(debug_log_filename));
	// デバッグログ 初期化完了フラグを0に。
	debug_log_initialize_flag = 0;
	return;
}
//*************************************************
// デバッグ出力用関数。
// printf() と同じフォーマットにて使用する。
//*************************************************
void debug_log_output(const char* fmt, ...)
{
#ifdef _DEBUG
	static int        fd = -1;
	char       buf[1024 * 5 + 1] = { 0 };
	char       work_buf[1024 * 4 + 1] = { 0 };
	char       date_and_time[80];
	char       replace_date_and_time[256] = { 0 };
#ifdef linux
	struct timeval tv;
#endif
	va_list     arg;
	// =========================================
	// デバッグログ 初期化フラグをチェック
	// =========================================
	if (debug_log_initialize_flag != 0) {
		return;
    }
	// =========================================
	// Debug出力文字列生成。
	// 行頭に、date_and_time を挿入しておく
	// =========================================
	//memset(buf, '\0', sizeof(buf));
	//memset(work_buf, '\0', sizeof(work_buf));
	// 引数で与えられた文字列を展開。
	va_start(arg, fmt);
	vsnprintf(work_buf, sizeof(work_buf), fmt, arg);
	va_end(arg);
	// work_bufの一番最後に'\n'がついていたら削除。
	auto len = (int)strlen(work_buf);
	if (len > 0 && work_buf[len - 1] == '\n') {
		work_buf[len - 1] = '\0';
	}
	// 挿入用文字列生成( "\ndate_and_time" になる)
	make_datetime_string(date_and_time);
#ifdef linux
	snprintf(replace_date_and_time, sizeof(replace_date_and_time), "\n%s[%d] ", date_and_time, getpid());
#else
	snprintf(replace_date_and_time, sizeof(replace_date_and_time), "\n%s[%ld] ", date_and_time, GetCurrentThreadId());
#endif

	// 出力文字列生成開始。
#ifdef linux
	snprintf((char*)buf, sizeof(buf), "%s[%d] %s", date_and_time, getpid(), work_buf);
#else
	snprintf(buf, sizeof(buf), "%s[%d] %s", date_and_time, _getpid(), work_buf);
#endif
	replace_character(buf, "\n", replace_date_and_time); // \nの前にdate_and_timeを挿入
	// 一番最後に'\n'をつける。
	strncat(buf, "\n", sizeof(buf) - strlen(buf));
	// =====================
	// ログファイル出力
	// =====================
	if(fd < 0 ) {
		fd = myopen(debug_log_filename, O_CREAT | O_APPEND | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE);
		if (fd < 0) {
			return;
		}
	}
	// 出力
	write(fd, buf, (unsigned int)strlen(buf));
	// メッセージ実体を出力
	//close(fd);
	// ファイルクローズ
	//fd = -1;
	return;
	//DEBUGが定義されてない場合ログ出力しない
#else
	IGNORE_PARAMETER(fmt);
#endif
}
//---------------------------------------------------------------------------
char* path_sanitize(char* orig_dir, size_t dir_size)
{
	IGNORE_PARAMETER(dir_size);
#ifdef linux
	char* p;
	char* q;
	char* dir;
	char* buf;
	size_t malloc_len;
	if (orig_dir == NULL) return NULL;
	malloc_len = strlen(orig_dir) * 2;
	buf = (char*)malloc(malloc_len);
	buf[0] = '\0';
	p = buf;
	dir = q = orig_dir;
	while (q != NULL) {
		dir = q;
		while (*dir == '/') dir++;
		q = strchr(dir, '/');
		if (q != NULL) {
			*q++ = '\0';
		}
		if (!strcmp(dir, "..")) {
			p = strrchr(buf, '/');
			if (p == NULL) {
				free(buf);
				dir[0] = '\0';
				return NULL; //  not allowed.
			}
			*p = '\0';
		}
		else if (strcmp(dir, ".")) {
			p += snprintf(p, malloc_len - (p - buf), "/%s", dir);
		}
	}
	if (buf[0] == '\0') {
		strncpy(orig_dir, "/", dir_size);
	}
	else {
		strncpy(orig_dir, buf, dir_size);
	}
	free(buf);
#endif
	return orig_dir;
}
static int myMkdir(wString FileName);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//階層フォルダを作成する
//引数フォルダ名(最後が\で終わる）またはフルパス名（ファイル名含む）
int myMkdir(wString FileName)
{
	while (FileName.length() > 0 && FileName[FileName.length() - 1] != '/') {
		FileName = FileName.SubString(0, FileName.length() - 1);
	}
	if (FileName.length() > 0) {
		FileName = FileName.SubString(0, FileName.length() - 1);
	}
	else {
		FileName = "/";
	}
	if (wString::DirectoryExists(FileName)) {
		return true;
	}
	else {
		//１つ上のフォルダを作って
		if (FileName.Pos("/") != (int)wString::npos &&
			myMkdir(FileName.SetLength(FileName.Length() - 1)) == (int)true) {
			//自分のフォルダ作成
			wString::CreateDir(FileName);
		}
		else {
			return false;
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
#define HTTP_BUF_SIZE (1024*10)
/// <summary>
/// HTTPDownload
/// </summary>
/// <param name="src">読み取り元URL</param>
/// <param name="dst">保存先ファイル名</param>
/// <param name="offset">０なら全取得,サイズ指定ならサイズに満たない場合、超えた場合全取得</param>
/// <returns>1:成功 2:サイズ同じ　false:失敗</returns>
int HTTPDownload(char* src, char* dst, off_t offset)
{
	time_t         rbgn_time = time(NULL) + NO_RESPONSE_TIMEOUT;

	char* buf;                           //バッファ
	int         recv_len;                       //読み取り長さ
	int         content_length = 0;
	int         len;
	char* work1;
	char* work2;
	int         fd = -1;                          //ファイルディスクリプタ
	char        host[256] = { 0 };
	char        server[256];
	SOCKET      server_socket;                  //サーバーソケット
	int         status = true;
	int         server_port = HTTP_SERVER_PORT;
	//出力ファイルの設定
	// ================
	// 実体転送開始
	// ================

	//準備
	//領域の取得
	buf = mycalloc(HTTP_BUF_SIZE, 1);
	//ホスト名の設定
	strncpy(host, src, sizeof(host));
	//先頭のHTTP://を抜く
	work2 = strstr(host, "://") + 3;
	if (work2 != NULL) {
		strcpy(host, work2);
	}
	//次の'/'からが本体
	work1 = strstr(host, "/");
	strcpy(server, work1);
	//'/'の所で切る
	*work1 = 0;
	//ポートがあれば取得
	work1 = strstr(host, ":");
	if (work1) {
		server_port = atoi(work1 + 1);
		*work1 = 0;
	}

	//strcpy( host, work2 );
	//ソケット作成と接続
	server_socket = wString::sock_connect(host, server_port);
	if (!SERROR(server_socket)) {
		//元ファイルがあった場合
		if (offset != 0) {
			//コネクションクローズしない
			sprintf(buf, "HEAD %s HTTP/1.0\r\n"
				"Accept: */*\r\n"
				"User-Agent: %s\r\n"
				"Host: %s\r\n"
				"Connection: close\r\n\r\n",
				wString(server).uri_encode().c_str(),
				USERAGENT,
				host);
			//サーバに繋がってheadをとった
			if (send(server_socket, buf, (int)strlen(buf), 0) != SOCKET_ERROR) {
				//初回分からヘッダを削除
				recv_len = recv(server_socket, buf, HTTP_BUF_SIZE, 0);
				int num = atoi(strchr(buf, ' ') + 1);
				if (200 <= num && num < 300) {
					//len = 0;
					buf[recv_len] = 0;
					//\r\n\r\nを探す
					work1 = strstr(buf, "Content-Length:");
					if (work1) {
						work1 += 16;
						content_length = atoi(work1);
						//PHPとかで中身がない。
					}
					else {
						sClose(server_socket);
						myfree(buf);
						return false;
					}
				}
				else if (num == 302) {
					//Location:----\r\n
					work1 = strstr(buf, "Location:") + 10;
					if (work1) {
						work2 = work1;
						while (work2[0] != '\r' || work2[1] != '\n') {
							work2++;
						}
						*work2 = 0;
						sClose(server_socket);
						status = HTTPDownload(work1, dst, offset);
						myfree(buf);
						return status;
					}
					sClose(server_socket);
					myfree(buf);
					return false;
				}
				//サーバから返答なし
			}
			else {
				sClose(server_socket);
				myfree(buf);
				return false;
			}
			sClose(server_socket);
			server_socket = wString::sock_connect(host, server_port);
			if (SERROR(server_socket)) {

				myfree(buf);
				return false;
			}
			//HTTP1.0 GET発行 レンジ付き
			if (offset < content_length) {   //range発行
				sprintf(buf, "GET %s HTTP/1.0\r\n"
					"Accept: */*\r\n"
#ifdef linux
					"User-Agent: %s\r\nHost: %s\r\nRange: bytes=%ld-\r\nConnection: close\r\n\r\n",
#elif raspberry
					"User-Agent: %s\r\nHost: %s\r\nRange: bytes=%zu-\r\nConnection: close\r\n\r\n",
#else
					"User-Agent: %s\r\nHost: %s\r\nRange: bytes=%ld-\r\nConnection: close\r\n\r\n",
#endif
					wString(server).uri_encode().c_str(),
					//"Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)",
					USERAGENT,
					host,
					offset);
				//                               GetAuthorization(void),
				myMkdir(wString(dst));
				fd = open(dst, O_WRONLY | O_APPEND | O_BINARY, 0777);
				//HTTP1.0 GET発行 ファイルが変なので全部取得
			}
			else if (offset > content_length) {
				sprintf(buf, "GET %s HTTP/1.0\r\n"
					"Accept: */*\r\n"
					"User-Agent: %s\r\nHost: %s\r\nConnection: close\r\n\r\n",
					wString(server).uri_encode().c_str(),
					USERAGENT,
					host);
				//                               GetAuthorization(void),
				myMkdir(wString(dst));
				fd = open(dst, O_WRONLY | O_TRUNC | O_BINARY, 0777);
				//取得済み
			}
			else {
				sClose(server_socket);
				myfree(buf);
				return 2;
			}


			//ファイルはありません。
		}
		else {
			sprintf(buf, "GET %s HTTP/1.0\r\n"
				"Accept: */*\r\n"
				"User-Agent: %s\r\nHost: %s\r\nConnection: close\r\n\r\n",
				wString(server).uri_encode().c_str(),
				USERAGENT,
				host);
			myMkdir(wString(dst));
			fd = open(dst, O_WRONLY | O_CREAT | O_BINARY, 0777);
		}
		//ファイルがないならエラー
		if (fd < 0) {
			sClose(server_socket);
			myfree(buf);
			debug_log_output("open() error.");
			return (false);
		}
		//サーバに繋がった
		if (send(server_socket, buf, (int)strlen(buf), 0) != SOCKET_ERROR) {
			//初回分からヘッダを削除
			recv_len = recv(server_socket, buf, HTTP_BUF_SIZE, 0);
			int num = atoi(strchr(buf, ' ') + 1);
			if (num == 200 || num == 206) {
				len = 0;
				//コンテンツ長さ
				content_length = atoi(strstr(buf, "Content-Length:") + 16);
				//\r\n\r\nを探す
				work1 = strstr(buf, HTTP_DELIMITER) + 4;//sizeof( HTTP_DELIMITER );//実体の先頭
				recv_len -= (int)(work1 - buf);
				memcpy(buf, work1, recv_len);           //移動
				write(fd, buf, recv_len);                //書き込めないことはないと
				len += recv_len;
				rbgn_time = time(NULL) + NO_RESPONSE_TIMEOUT;
				while (loop_flag) {
					recv_len = recv(server_socket, buf, HTTP_BUF_SIZE, 0);
					if (recv_len < 0) {
						break;
					}
					else if (recv_len > 0) {
						write(fd, buf, recv_len);            //書き込めないことはないと
						//新興
						//ここを更新しないと１０秒で書き込みが終わる。
						rbgn_time = time(NULL) + NO_RESPONSE_TIMEOUT;
						//buf += recv_len;
						len += recv_len;
						//指定時刻書き込めなかったら落ちる
					}
					else if (len == content_length) {
						status = 1;
						break;
					}
					else if (time(NULL) > rbgn_time) {
						status = false;
						break;
					}
				}
			}
			else if (num == 302) {
				work1 = strstr(buf, "Location:") + 10;
				if (work1) {
					work2 = work1;
					while (work2[0] != '\r' || work2[1] != '\n') {
						work2++;
					}
					*work2 = 0;
					sClose(server_socket);
					close(fd);
					status = HTTPDownload(work1, dst, offset);
					myfree(buf);
					return status;
				}
			}
			else {
				close(fd);
				fd = -1;
				unlink(dst);
				status = false;
			}
		}
		sClose(server_socket);
	}
	// スレッド終了
	myfree(buf);
	if (fd != -1) {
		close(fd);
	}
	//ExitThread(TRUE);
	return status;
}
wString GetAuthorization(wString& AuthorizedString)
{
	wString auth;
	if (AuthorizedString.length()) {
		auth.sprintf("Authorization: Basic %s", AuthorizedString.c_str());
	}
	return auth;
}

#ifdef linux
//---------------------------------------------------------------------------
int send(int fd, const char* buffer, unsigned int length, int mode)
{
	return write(fd, buffer, length);
}
//---------------------------------------------------------------------------
int recv(int fd, char* buffer, unsigned int length, int mode)
{
	return read(fd, buffer, length);
}
//---------------------------------------------------------------------------
int getTargetFile(const char* LinkFile, char* TargetFile)
{
	return FALSE;
}
//---------------------------------------------------------------------------
void Sleep(unsigned int milliseconds)
{
	unsigned long sec = milliseconds / 1000;
	milliseconds %= 1000;
	//linuxでは秒タイマー
	if (sec) {
		sleep(sec);
	}
	//linuxではμ秒タイマー
	if (milliseconds) {
		usleep(milliseconds * 1000);
	}
}
#endif
char* mymalloc(size_t size)
{
	return(new char[size]);
}
char* mycalloc(size_t size1, int num)
{
	char* tmp = new char[size1 * num];
	memset(tmp, 0, size1 * num);
	return tmp;
}
void myfree(char* ptr)
{
	delete[] ptr;
}
// ソケットを作成し、相手に接続するラッパ. 失敗 = -1
//---------------------------------------------------------------------------
SOCKET sock_connect(char* host, int port)
{
	SOCKET sock;
	struct sockaddr_in sockadd = { 0 };     //ＳＯＣＫＥＴ構造体
	struct hostent* hent;
	debug_log_output("sock_connect: %s:%d", host, port);
	//ＳＯＣＫＥＴ作成
	if (SERROR(sock = socket(PF_INET, SOCK_STREAM, 0))) {
		debug_log_output("sock_connect_error:");
		return INVALID_SOCKET;
	}
	debug_log_output("sock: %d", sock);
	if (NULL == (hent = gethostbyname(host))) {
		sClose(sock);
		return INVALID_SOCKET;
	}
	debug_log_output("hent: %p", hent);
	//ソケット構造体へアドレス設定
	memcpy(&sockadd.sin_addr, hent->h_addr, hent->h_length);
	//ソケット構造体へポート設定
	sockadd.sin_port = htons((u_short)port);
	//ＩＰＶ４アドレスファミリを設定
	sockadd.sin_family = AF_INET;
	//接続
	if (SERROR(connect(sock, (struct sockaddr*)&sockadd, sizeof(sockadd)))) {
		debug_log_output("connect: error Content=%s\n", host);
		sClose(sock);
		return INVALID_SOCKET;
	}
	debug_log_output("Sock Connected\n");
	//set_nonblocking_mode(sock, 0);    /* blocking */
	return sock;
}
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//int open(unsigned char *pathname, int flags)
//{
//    return open((char*)pathname,flags);
//}
//int open(unsigned char *pathname, int flags, mode_t mode)
//{
//    return open((char*)pathname,flags,mode);
//}
#if 1
/********************************************************************************/
// 日本語文字コード変換。
// (libnkfのラッパー関数)
//
//      サポートされている形式は以下の通り。
//              in_flag:        CODE_AUTO, CODE_SJIS, CODE_EUC, CODE_UTF8, CODE_UTF16
//              out_flag:       CODE_SJIS, CODE_EUC
/********************************************************************************/
void convert_language_code(const char* in, char* out, size_t len, int in_flag, int out_flag)
{
	char       nkf_option[8] = { 0 };
	memset(nkf_option, '\0', sizeof(nkf_option));
	//=====================================================================
	// in_flag, out_flagをみて、libnkfへのオプションを組み立てる。
	//=====================================================================
	switch (in_flag)
	{
	case CODE_SJIS:
		strncpy(nkf_option, "S", sizeof(nkf_option));
		break;
	case CODE_EUC:
		strncpy(nkf_option, "E", sizeof(nkf_option));
		break;
	case CODE_UTF8:
		strncpy(nkf_option, "W", sizeof(nkf_option));
		break;
	case CODE_UTF16:
		strncpy(nkf_option, "W16", sizeof(nkf_option));
		break;
	case CODE_AUTO:
	default:
		strncpy(nkf_option, "", sizeof(nkf_option));
		break;
	}
	switch (out_flag)
	{
	case CODE_EUC:
		strncat(nkf_option, "e", sizeof(nkf_option) - strlen(nkf_option));
		break;
	case CODE_SJIS:
		strncat(nkf_option, "s", sizeof(nkf_option) - strlen(nkf_option));
		break;
	case CODE_UTF8:
	default:
		strncat(nkf_option, "w", sizeof(nkf_option) - strlen(nkf_option));
		break;
	}
	//=================================================
	// libnkf 実行
	//=================================================
	*out = 0;
	nkf(static_cast<const char*>(in), out, len, static_cast<const char*>(nkf_option));
	return;
}
#endif
#if 0
//ファイルディスクリプタブロック設定
//1:ノンブロッキング 0:ブロッキング
void set_nonblocking_mode(int fd, int flag)
{
	int res, nonb = 0;
	nonb |= O_NONBLOCK;
	if ((res = fcntl(fd, F_GETFL, 0)) == -1) {
		debug_log_output("fcntl(fd, F_GETFL) failed");
	}
	if (flag) {
		res |= O_NONBLOCK;
	}
	else {
		res &= ~O_NONBLOCK;
	}
	if (fcntl(fd, F_SETFL, res) == -1) {
		debug_log_output("fcntl(fd, F_SETFL, nonb) failed");
	}
}
#endif
//linux/windows共用オープン
//追加: O_CREAT | O_APPEND | O_WRONLY(またはO_RDWR) | (O_BINARY) , S_IREAD | S_IWRITE
//新規: O_CREAT | O_TRUNC  | O_WRONLY(またはO_RDWR) | (O_BINARY) , S_IREAD | S_IWRITE
//読込: O_RDONLY                                     | (O_BINARY)
int myopen(const char* filename, int amode, int option)
{
	//毎回オープンしないようにする
	//static int hd = -1;
#ifdef linux
	if (option != 0) {
		return open(filename, amode, option);
	}
	else {
		return open(filename, amode);
	}
#else
	char work[1024];
	strcpy(work, filename);
	int ptr = 0;
	while (work[ptr]) {
		if (work[ptr] == '/') {
			work[ptr] = '\\';
}
		ptr++;
	}
	if (option != 0) {
		//if (hd < 0) {
		return open(work, amode, option);
		//}
	}
	else {
		//if (hd < 0) {
		return open(work, amode);
		//}
	}
	//return hd;
#endif
}
//汎用ソケットクローズ
int sClose(SOCKET socket)
{
	int ret;
#ifdef linux
	ret = shutdown(socket, SD_BOTH);
	if (ret != 0) debug_log_output("shutdown error=%s,%d,%d", strerror(errno), errno, socket);
	ret = close(socket);
	if (ret != 0) debug_log_output("close error=%s,%d,%d", strerror(errno), errno, socket);
#else
	ret = shutdown(socket, SD_BOTH);
	if (ret != 0) {
		debug_log_output("shutdown error=%s,%d,%d", strerror(errno), errno, socket);
	}
	ret = closesocket(socket);
	if (ret != 0) {
		debug_log_output("close error=%s,%d,%d", strerror(errno), errno, socket);
}
#endif
	return ret;
}
//#ifdef linux
//void ExitThread(DWORD dwExitCode)
//{
//    return;
//}
//#endif
// **************************************************************************
// fdから、１行(CRLFか、LF単独が現れるまで)受信
// CRLFは削除する。
// 受信したサイズをreturnする。
// **************************************************************************
int readLine(int fd, char* line_buf_p, int line_max)
{
	char byte_buf;
	int  line_len = 0;
	int	 recv_len;
	// １行受信実行
	while (1) {
		recv_len = read(fd, &byte_buf, 1);
		if (recv_len != 1) { // 受信失敗チェック
			return (-1);
		}
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
			return (-1);
		}
	}
	return line_len;
}

/////////////////////////////////////////////////////////////////////////////
//mp3
// MP3 ID3v1 タグ情報
wString mp3::mp3_id3_tag(const char* filename)
{
	wString tmp;
	if (mp3_id3_tag_read(filename) == 0) {
		tmp = "{";
		tmp.cat_sprintf("\"title\":\"%s\"", mp3_id3v1_title);
		tmp.cat_sprintf(",\"album\":\"%s\"", mp3_id3v1_album);
		tmp.cat_sprintf(",\"artist\":\"%s\"", mp3_id3v1_artist);
		tmp.cat_sprintf(",\"year\":\"%s\"", mp3_id3v1_year);
		tmp.cat_sprintf(",\"comment\":\"%s\"}", mp3_id3v1_comment);
	}
	else {
		tmp = "{\"title\":\"\",\"album\":\"\",\"artist\":\"\",\"year\":\"\",\"comment\":\"\"}";
	}
	return tmp;
}
/////////////////////////////////////////////////////////////////////////////
wString mp3::mp3_id3_tag(const wString& filename)
{
	return mp3_id3_tag(filename.c_str());
}
/////////////////////////////////////////////////////////////////////////////
// mp3ファイル解析
int  mp3::mp3_id3_tag_read(const char* mp3_filename)
{

	if (mp3_id3v1_tag_read(mp3_filename) != 0) {
		if (mp3_id3v2_tag_read(mp3_filename) != 0) {
			return -1;
		}
	}
	else {
		return 0;
	}
	if (mp3_id3v1_flag > 0) {
		if (mp3_id3v1_title[0] == '\0') {
			mp3_id3v1_flag = 0;
		}
		else {
			return 0;
		}
	}
	return -1;
}
/////////////////////////////////////////////////////////////////////////////
// MP3ファイルから、ID3v1形式のタグデータを得る
//
int  mp3::mp3_id3v1_tag_read(const char* mp3_filename)
{
	int fd;
	unsigned char       buf[128];
	off_t               length;
	memset(buf, '\0', sizeof(buf));
	fd = open(mp3_filename, O_RDONLY);
	if (fd < 0)
	{
		return -1;
	}
	// 最後から128byteへSEEK
	length = lseek(fd, -128, SEEK_END);
	if (length < 0) {
		return -1;
	}
	// ------------------
	// "TAG"文字列確認
	// ------------------
	// 3byteをread.
	if (read(fd, buf, 3) > 0) {
		//debug_log_output("buf='%s'", buf);
		// "TAG" 文字列チェック
		if (memcmp(static_cast<const void*>(buf), "TAG", 3) != 0)
		{
			//debug_log_output("NO ID3 Tag.");
			close(fd);
			return -1;              // MP3 タグ無し。
		}
		// ------------------------------------------------------------
		// Tag情報read
		//
		//  文字列最後に0xFFと' 'が付いていたら削除。
		//  client文字コードに変換。
		// ------------------------------------------------------------
		// 曲名
		memset(static_cast<void*>(buf), '\0', sizeof(buf));
		if (read(fd, buf, 30) > 0) {
			wString::Rtrimch(reinterpret_cast<char*>(buf), 0xFF);
			wString::Rtrimch(reinterpret_cast<char*>(buf), ' ');
			convert_language_code(reinterpret_cast<const char*>(buf), reinterpret_cast<char*>(mp3_id3v1_title), sizeof(mp3_id3v1_title), CODE_AUTO, CODE_UTF8);

			// アーティスト
			memset(static_cast<void*>(buf), '\0', sizeof(buf));
			if (read(fd, buf, 30) > 0) {
				wString::Rtrimch(reinterpret_cast<char*>(buf), 0xFF);
				wString::Rtrimch(reinterpret_cast<char*>(buf), ' ');
				convert_language_code(reinterpret_cast<const char*>(buf), reinterpret_cast<char*>(mp3_id3v1_artist), sizeof(mp3_id3v1_artist), CODE_AUTO, CODE_UTF8);

				// アルバム名
				memset(static_cast<void*>(buf), '\0', sizeof(buf));
				if (read(fd, buf, 30) > 0) {
					wString::Rtrimch(reinterpret_cast<char*>(buf), 0xFF);
					wString::Rtrimch(reinterpret_cast<char*>(buf), ' ');
					convert_language_code(reinterpret_cast<const char*>(buf), reinterpret_cast<char*>(mp3_id3v1_album), sizeof(mp3_id3v1_album), CODE_AUTO, CODE_UTF8);

					// 制作年度
					memset(static_cast<void*>(buf), '\0', sizeof(buf));
					if (read(fd, buf, 4) > 0) {
						wString::Rtrimch(reinterpret_cast<char*>(buf), 0xFF);
						wString::Rtrimch(reinterpret_cast<char*>(buf), ' ');
						convert_language_code(reinterpret_cast<const char*>(buf), reinterpret_cast<char*>(mp3_id3v1_year), sizeof(mp3_id3v1_year), CODE_AUTO, CODE_UTF8);
						// コメント
						memset(static_cast<void*>(buf), '\0', sizeof(buf));
						if (read(fd, buf, 28) > 0) {
							wString::Rtrimch(reinterpret_cast<char*>(buf), 0xFF);
							wString::Rtrimch(reinterpret_cast<char*>(buf), ' ');
							convert_language_code(reinterpret_cast<const char*>(buf), reinterpret_cast<char*>(mp3_id3v1_comment), sizeof(mp3_id3v1_comment), CODE_AUTO, CODE_UTF8);
							// ---------------------
							// 存在フラグ
							// ---------------------
							mp3_id3v1_flag = 1;
						}
					}
				}
			}
		}
	}
	close(fd);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
unsigned int mp3::id3v2_len(unsigned char* buf)
{
	return buf[0] * 0x200000 + buf[1] * 0x4000 + buf[2] * 0x80 + buf[3];
}
/////////////////////////////////////////////////////////////////////////////
// MP3ファイルから、ID3v2形式のタグデータを得る
// 0: 成功  -1: 失敗(タグなし)
int  mp3::mp3_id3v2_tag_read(const char* mp3_filename)
{
	int fd;
	unsigned char  buf[1024] = { 0 };
	unsigned char  buf2[1024] = { 0 };
	unsigned char* frame;
	unsigned int   len;
	struct _copy_list {
		unsigned char id[5];
		unsigned char* container;
		size_t maxlen;
	} copy_list[] = {
		{ "TIT2", mp3_id3v1_title
		, sizeof(mp3_id3v1_title) },
		{ "TPE1", mp3_id3v1_artist
		, sizeof(mp3_id3v1_artist) },
		{ "TALB", mp3_id3v1_album
		, sizeof(mp3_id3v1_album) },
		{ "TCOP", mp3_id3v1_year
		, sizeof(mp3_id3v1_year) },
		{ "TYER", mp3_id3v1_year
		, sizeof(mp3_id3v1_year) },
		{ "COMM", mp3_id3v1_comment
		, sizeof(mp3_id3v1_comment) },
	};
	int list_count = sizeof(copy_list) / sizeof(struct _copy_list);
	int i;
	int flag_extension = 0;
	memset(buf, '\0', sizeof(buf));
	fd = open(mp3_filename, O_RDONLY);
	if (fd < 0)
	{
		return -1;
	}
	// ------------------
	// "ID3"文字列確認
	// ------------------
	// 10byteをread.
	if (read(fd, buf, 10) != 10)
	{
		// read failed.
		return -1;
	}
	// debug_log_output("buf='%s'", buf);
	// "ID3" 文字列チェック
	if (strncmp(reinterpret_cast<char*>(buf), "ID3", 3) != 0)
	{
		/*
		*  ファイルの後ろにくっついてる ID3v2 タグとか
		*  ファイルの途中にあるのとか 面倒だから 読まないよ。
		*/
		//debug_log_output("NO ID3v2 Tag.");
		close(fd);
		return -1;              // v2 タグ無し。
	}
	//debug_log_output("ID3 v2.%d.%d Tag found", buf[3], buf[4]);
	//debug_log_output("ID3 flag: %02X", buf[5]);
	if (buf[5] & 0x40) {
		//debug_log_output("ID3 flag: an extended header exist.");
		flag_extension = 1;
	}
	len = id3v2_len(buf + 6);
	if (flag_extension) {
		int exlen;
		if (read(fd, buf, 6) != 6) {
			close(fd);
			return -1;
		}
		exlen = id3v2_len(buf);
		//debug_log_output("ID3 ext. flag: len = %d", exlen);
		if (exlen < 6) {
			//debug_log_output("invalid ID3 ext. header.");
			close(fd);
			return -1;
		}
		else if (exlen > 6) {
			//debug_log_output("large ID3 ext. header.");
			lseek(fd, exlen - 6, SEEK_CUR);
		}
		len -= exlen;
	}
	// ------------------------------------------------------------
	// Tag情報read
	//
	//  client文字コードに変換。
	// ------------------------------------------------------------
	while (len > 0) {
		int frame_len;
		/* フレームヘッダを 読み込む */
		if (read(fd, buf, 10) != 10) {
			close(fd);
			return -1;
		}
		/* フレームの長さを算出 */
		frame_len = id3v2_len(buf + 4);
		/* フレーム最後まで たどりついた */
		unsigned long* ppp = reinterpret_cast<unsigned long*>(buf);
		if (frame_len == 0 || *ppp == 0) {
			break;
		}
		for (i = 0; i < list_count; i++) {
			if (!strncmp(reinterpret_cast<char*>(buf), reinterpret_cast<char*>(copy_list[i].id), 4)) break;
		}
		if (i < list_count) {
			// 解釈するタグ 発見
			// 存在フラグ
			mp3_id3v1_flag = 1;
			frame = static_cast<unsigned char*>(malloc(frame_len + 1));
			if (frame == NULL) {
				close(fd);
				return -1;
			}
			memset(frame, '\0', frame_len + 1);
			if (read(fd, frame, frame_len) != frame_len) {
				//debug_log_output("ID3v2 Tag[%s] read failed", copy_list[i].id);
				free(frame);
				close(fd);
				return -1;
			}
			//debug_log_output("ID3v2 Tag[%s] found. '%s'", copy_list[i].id, frame + 1);
			wString::Rtrimch(reinterpret_cast<char*>(frame + 1), ' ');
			//convert_language_code( frame+1,strlen(frame+1),CODE_AUTO, CODE_UTF8);
			strncpy(reinterpret_cast<char*>(buf2), reinterpret_cast<char*>(frame + 1), copy_list[i].maxlen);
			//strncpy( (char*)copy_list[i].container, (char*)(frame+1), copy_list[i].maxlen );
			convert_language_code(reinterpret_cast<const char*>(buf2), reinterpret_cast<char*>(copy_list[i].container), copy_list[i].maxlen, CODE_AUTO, CODE_UTF8);
			//convert_language_code( copy_list[i].container,copy_list[i].maxlen,CODE_AUTO, CODE_UTF8);
			free(frame);
		}
		else
		{
			/* マッチしなかった */
			buf[4] = '\0';
			//debug_log_output("ID3v2 Tag[%s] skip", buf);
			lseek(fd, frame_len, SEEK_CUR);
		}
		len -= (frame_len + 10); /* フレーム本体 + フレームヘッダ */
	}
	close(fd);
	return mp3_id3v1_flag ? 0 : -1;
}
unsigned char   mp3::mp3_id3v1_flag;         // MP3 タグ 存在フラグ
unsigned char   mp3::mp3_id3v1_title[128];   // MP3 曲名
unsigned char   mp3::mp3_id3v1_album[128];   // MP3 アルバム名
unsigned char   mp3::mp3_id3v1_artist[128];  // MP3 アーティスト
unsigned char   mp3::mp3_id3v1_year[128];    // MP3 制作年度
unsigned char   mp3::mp3_id3v1_comment[128]; // MP3 コメント

