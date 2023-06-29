﻿#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_String.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>
#ifdef linux
#include <dirent.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <process.h>
#include "dirent.h"
//#include <dir.h>
#include <direct.h>
#include <io.h>
#endif
#include "cbl_String.h"
#include "cbl.h"
#include "cbl_tools.h"
#include <time.h>
#include "define.h"
#ifndef strrstr
///---------------------------------------------------------------------------
/// <summary>
/// strrstr()
///   文字列 から 文字列 を検索する。
///   最後 に現れた文字列の先頭を指す ポインタ を返す。
///   文字列が見つからない場合は null を返す。
///   pattern が 空文字列 の場合は常に 文字列終端 を返す。
///   文字列終端文字 '\0' は検索対象とならない。
///   半角英字 の 大文字と小文字 を区別する。
/// </summary>
/// <param name="string">検索対象となる文字列</param>
/// <param name="pattern">文字列から検索する文字列</param>
/// <returns></returns>
char* // 文字列へのポインタ
strrstr(const char* string, const char* pattern)
{
	// 文字列終端に達するまで検索を繰り返す。
	const char* last = NULL;
	for (const char* p = string; ; p++) {
		if (0 == *p) {
			return (char*)last;
		}
		p = strstr(p, pattern);
		if (p == NULL) {
			break;
		}
		last = p;
	}
	return (char*)last;
}//strrstr
#endif

/// <summary>検索失敗時の値</summary>
const int wString::npos = (int)(-1);
///---------------------------------------------------------------------------
/// <summary>
/// 文字列クラス　ディフォールトコンストラクタ
/// </summary>
wString::wString(void)
{
	//初期化
	len = 0;
	total = 1;
	String = static_cast<char*>(new char[1]);
	*String = 0;
}
///---------------------------------------------------------------------------
/// <summary>
/// 文字列クラス文字数指定（内容は０）
/// </summary>
/// <param name="mylen">初期文字数指定</param>
wString::wString(int mylen)
{
	//初期化
	if (mylen < 0) mylen = 0;
	len = 0;
	//末尾０の分
	total = mylen + 1;
	String = static_cast<char*>(new char[mylen + 1]);
	*String = 0;
}
///---------------------------------------------------------------------------
/// <summary>
/// 文字列クラス文字列コンストラクタ
/// </summary>
/// <param name="str">元となる文字列</param>
wString::wString(const char* str)
{
	//初期化
	len = static_cast<unsigned int>(strlen(str));
	if (len) {
		total = len + 1;
		String = static_cast<char*>(new char[total]);
		strcpy(String, str);
	}
	else {
		total = 1;
		String = static_cast<char*>(new char[1]);
		String[0] = 0;
	}
}
///---------------------------------------------------------------------------
/// <summary>
/// コピーコンストラクタ
/// </summary>
/// <param name="str">コピーする文字列</param>
wString::wString(const wString& str)
{
	//初期化
	len = str.len;
	if (str.len) {
		total = str.total;
		String = new char[str.total];
		memcpy(String, str.String, str.total);
		//*String = 0;
	}
	else {
		total = 1;
		String = static_cast<char*>(new char[1]);
		String[0] = 0;
	}
}
///---------------------------------------------------------------------------
/// <summary>
/// デストラクタ
/// </summary>
wString::~wString()
{
	delete[] String;
}
///---------------------------------------------------------------------------
/// <summary>
/// プラスメソッドa+b
/// </summary>
/// <param name="str">プラスするwString</param>
/// <returns>プラス結果</returns>
wString wString::operator+(const wString& str) const
{
	wString temp(*this);
	temp += str;
	return temp;
}
///---------------------------------------------------------------------------
/// <summary>
/// プラスメソッドa+b
/// </summary>
/// <param name="str">プラスする文字列</param>
/// <returns>プラス結果</returns>
wString wString::operator+(const char* str) const
{
	wString temp(*this);
	temp += str;
	return temp;
}
///---------------------------------------------------------------------------
/// <summary>
/// プラスメソッド+(a,b)
/// </summary>
/// <param name="str1">元文字列</param>
/// <param name="str2">プラスするwString</param>
/// <returns>プラス結果</returns>
wString operator+(const char* str1, const wString str2)
{
	wString temp(str1);
	temp += str2;
	return temp;
}
///---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/// <summary>
/// プラスメソッドa+=b
/// </summary>
/// <param name="str">プラスするwString</param>
void wString::operator+=(const wString& str)
{
	unsigned int newLen = len + str.len;
	resize(newLen);
	memcpy(String + len, str.String, str.len);
	String[newLen] = 0;
	len = newLen;
	return;
}
///---------------------------------------------------------------------------

//---------------------------------------------------------------------------


/// <summary>
/// プラスメソッドa+=b
/// </summary>
/// <param name="str">プラスする文字列</param>
void wString::operator+=(const char* str)
{
	auto slen = static_cast<unsigned int>(strlen(str));
	auto newLen = slen + len;
	resize(newLen);
	strcpy(String + len, str);
	len = newLen;
	return;
}
///---------------------------------------------------------------------------
/// <summary>
/// バイナリ文字列の設定a.setBinary(mem,len)
/// </summary>
/// <param name="mem">設定バイナリ</param>
/// <param name="addLen">設定長さ</param>
void wString::setBinary(const void* mem, int binaryLength)
{
	auto newLen = binaryLength + len;
	resize(newLen);
	memcpy(static_cast<void*>(String + len), mem, binaryLength);
	len = newLen;
	return;
}
///---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// <summary>
/// 先頭文字列の比較（バイナリ非対応）
/// </summary>
/// <param name="needle">検索する文字列</param>
/// <returns>先頭と一致すればtrue</returns>
bool wString::startsWith(const char* needle)
{
	return strstr(String, needle) == String;
}
//---------------------------------------------------------------------------
/// <summary>
/// 末尾文字列の比較（バイナリ非対応）
/// </summary>
/// <param name="needle">検索する文字列</param>
/// <returns>先頭と一致すればtrue</returns>
bool wString::endsWith(const char* needle)
{
	int clen = strlen(needle);
	if (clen == 0 || clen > len) {
		return false;
	}
	return strcmp(String + len - clen, needle) == 0;
}
///---------------------------------------------------------------------------
/// <summary>
/// 文字列に1バイト文字加算
/// </summary>
/// <param name="ch">加算する1バイト文字</param>
void wString::operator+=(const char ch)
{
	int tmpl = ((len >> 4) + 1) << 4;
	resize(tmpl);
	String[len++] = ch;
	String[len] = 0;
	return;
}
///---------------------------------------------------------------------------
/// <summary>
/// wStringとwStringの比較
/// </summary>
/// <param name="str">比較する文字列</param>
/// <returns>ture:同一、false:異なる場合</returns>
bool wString::operator==(const wString& str) const
{
	if (len != str.len) {
		return false;
	}
	return(strncmp(String, str.String, len) == 0);
}
///---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool wString::operator==(const char* str) const
{
	auto mylen = static_cast<unsigned int>(strlen(str));
	if (len != mylen) {
		return false;
	}
	return(strncmp(String, str, len) == 0);
}
///---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool wString::operator!=(const wString& str) const
{
	if (len != str.len) {
		return true;
	}
	return(strncmp(String, str.String, len) != 0);
}
//---------------------------------------------------------------------------
bool wString::operator!=(const char* str) const
{
	if (len != strlen(str)) {
		return true;
	}
	return(strncmp(String, str, len) != 0);
}
//---------------------------------------------------------------------------
bool wString::operator>=(const wString& str) const
{
	unsigned int maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) >= 0);
}
//---------------------------------------------------------------------------
bool wString::operator>=(const char* str) const
{
	auto mylen = static_cast<unsigned int>(strlen(str));
	auto maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) >= 0);
}
//---------------------------------------------------------------------------
bool wString::operator<=(const wString& str) const
{
	unsigned int maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) <= 0);
}
//---------------------------------------------------------------------------
bool wString::operator<=(const char* str) const
{
	auto mylen = static_cast<unsigned int>(strlen(str));
	auto maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) <= 0);
}
//---------------------------------------------------------------------------
bool wString::operator>(const wString& str) const
{
	auto maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) > 0);
}
//---------------------------------------------------------------------------
bool wString::operator>(const char* str) const
{
	auto mylen = static_cast<unsigned int>(strlen(str));
	auto maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) > 0);
}
//---------------------------------------------------------------------------
bool wString::operator<(const wString& str) const
{
	auto maxlen = (len > str.len) ? len : str.len;
	return(strncmp(String, str.String, maxlen) < 0);
}
//---------------------------------------------------------------------------
bool wString::operator<(const char* str) const
{
	auto mylen = static_cast<unsigned int>(strlen(str));
	auto maxlen = (len > mylen) ? len : mylen;
	return(strncmp(String, str, maxlen) < 0);
}
//---------------------------------------------------------------------------
void wString::operator=(const wString& str)
{
	resize(str.total);
	memcpy(String, str.String, str.total);
	len = str.len;
	return;
}
//---------------------------------------------------------------------------
void wString::operator=(const char* str)
{
	auto newLen = static_cast<unsigned int>(strlen(str));
	resize(newLen);
	strcpy(String, str);
	len = newLen;
	return;
}
//---------------------------------------------------------------------------
void wString::operator=(const int num)
{
	this->sprintf("%d", num);
	return;
}
//---------------------------------------------------------------------------
void wString::operator=(const double num)
{
	this->sprintf("%f", num);
	return;
}
//---------------------------------------------------------------------------
char wString::operator[](unsigned int index) const
{
	if (index < len) {
		return String[index];
	}
	else {
		perror("out bound");
		return -1;
	}
}
//---------------------------------------------------------------------------
char wString::at(unsigned int index) const
{
	if (index < len) {
		return String[index];
	}
	else {
		perror("out bound");
		return -1;
	}
}
//---------------------------------------------------------------------------
wString& wString::SetLength(const unsigned int num)
{
	resize(num);
	return *this;
}
//---------------------------------------------------------------------------
// 比較
//---------------------------------------------------------------------------
int wString::compare(const wString& str) const
{
	return strcmp(String, str.String);
}
//---------------------------------------------------------------------------
// 比較
//---------------------------------------------------------------------------
int wString::compare(const char* str) const
{
	return strcmp(String, str);
}
//---------------------------------------------------------------------------
// クリア
//---------------------------------------------------------------------------
void  wString::clear(void)
{
	len = 0;
	if (*String) {
		*String = 0;
	}
}
//---------------------------------------------------------------------------
// 部分文字列
//---------------------------------------------------------------------------
wString  wString::SubString(int start, int mylen) const
{
	if (start + mylen > (int)len) mylen = (int)len - start;
	wString temp(mylen);
	if (mylen > 0) {
		memcpy(temp.String, String + start, mylen);
		temp.String[mylen] = 0;
		//長さ不定。数えなおす
		temp.len = mylen;
	}
	return temp;
}
//---------------------------------------------------------------------------
// substr
//---------------------------------------------------------------------------
wString wString::substr(int start, int mylen) const
{
	if (mylen < 0) mylen = len;
	if (start + mylen > (int)len) mylen = (int)len - start;
	wString temp(mylen);
	if (mylen > 0) {
		memcpy(temp.String, String + start, mylen);
		temp.String[mylen] = 0;
		//長さ不定。数えなおす
		temp.len = mylen;
	}
	return temp;
}

//---------------------------------------------------------------------------
// 位置
//---------------------------------------------------------------------------
int wString::find(const wString& str, int index) const
{
	char* ptr = strstr(String + index, str.String);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)((ptr - String));
	}
}
//---------------------------------------------------------------------------
// 位置
//---------------------------------------------------------------------------
int wString::find(const char* str, int index) const
{
	char* ptr = strstr(String + index, str);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// 位置
//---------------------------------------------------------------------------
int wString::find(char ch, int index) const
{
	char* ptr = strchr(String + index, ch);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// 行末からの検索
//---------------------------------------------------------------------------
int wString::rfind(const wString& str, int index) const
{
	char* ptr = strrstr(String + index, str.String);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// 行末からの検索
// str:文字列
// index:開始位置(省略可能)
//---------------------------------------------------------------------------
int wString::rfind(const char* str, int index) const
{
	char* ptr = strrstr(String + index, str);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// 行末からの検索
// ch:char
// index:開始位置(省略可能)
//---------------------------------------------------------------------------
int wString::rfind(char ch, int index) const
{
	char* ptr = strrchr(String + index, ch);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)(ptr - String);
	}
}
//---------------------------------------------------------------------------
// 位置
//---------------------------------------------------------------------------
//int wString::Pos(const char* pattern)
//{
//    char* ptr = strstr(String,pattern);
//    if( ptr == NULL ){
//        return npos;
//    }else{
//        return (int)(ptr-String);
//    }
//}
//---------------------------------------------------------------------------
// 位置
//---------------------------------------------------------------------------

/// <summary>
/// 文字列中の位置
/// </summary>
/// <param name="pattern">存在を検査する文字列</param>
/// <param name="pos">検査開始位置</param>
/// <returns>見つからない場合wString::npos,存在する場合、先頭を０とした位置情報</returns>
int wString::Pos(const char* pattern, int pos) const
{
	auto ptr = strstr(String + pos, pattern);
	if (ptr == NULL) {
		return npos;
	}
	else {
		return (int)(ptr - String);
	}
}

//---------------------------------------------------------------------------
// 位置
//---------------------------------------------------------------------------
int wString::Pos(const wString& pattern, int pos) const
{
	return Pos(pattern.String, pos);
}
//---------------------------------------------------------------------------
//　Size
//---------------------------------------------------------------------------

/// <summary>
/// 文字長
/// </summary>
/// <returns>文字列の長さ</returns>
unsigned int wString::size(void) const
{
	return len;
}

/// <summary>
/// 文字長
/// </summary>
/// <param name=""></param>
/// <returns>文字列の長さ</returns>
unsigned int wString::length(void) const
{
	return len;
}

/// <summary>
/// ファイル読み込み
/// </summary>
/// <param name="str">読み込むファイルのフルファイル名</param>
int wString::LoadFromFile(const wString& str)
{
	return LoadFromFile(str.String);
}
//---------------------------------------------------------------------------
// ファイル読み込み
//---------------------------------------------------------------------------

/// <summary>
/// ファイル読み込み
/// </summary>
/// <param name="FileName">ファイル名（フルパス）またはURL</param>
/// <returns>０</returns>
int wString::LoadFromFile(const char* FileName)
{
	long flen;
	int  handle;
	if (strncmp(FileName, "http://", 7) == 0) {
		wString tmp;
		tmp = wString::HTTPGet(const_cast<char*>(FileName));
		*this = tmp;
	}
	else {
#ifdef linux
		handle = open(FileName, O_RDONLY | S_IREAD);
#else
		handle = myopen(FileName, O_RDONLY | O_BINARY, S_IREAD);
#endif
		if (handle < 0) {
			return -1;
		}
		flen = lseek(handle, 0, SEEK_END);
		lseek(handle, 0, SEEK_SET);
		SetLength(flen + 1);
		len = read(handle, String, flen);
		close(handle);
		String[len] = 0;
		//\0がある場合を考えればstrlenとってはいけない
		//len = strlen(String);
	}
	return 0;
}

/// <summary>
/// CSVファイル読み込み
/// </summary>
/// <param name="str">CSVファイル名</param>
void wString::LoadFromCSV(const wString& str)
{
	LoadFromCSV(str.String);
}

int isNumber(char* str)
{
	for (auto i = (int)strlen(str) - 1; i >= 0; i--) {
		auto ch = (unsigned char)str[i];
		if ((!isdigit(ch)) && ch != '.') {
			return 0;
		}
	}
	return 1;
}

//---------------------------------------------------------------------------
// ファイル読み込み
//---------------------------------------------------------------------------
void wString::LoadFromCSV(const char* FileName)
{
	int  fd;
	char s[1024] = { 0 };
	char t[1024] = { 0 };
	int first = 1;
	fd = myopen(FileName, O_RDONLY | O_BINARY, S_IREAD);
	if (fd < 0) {
		printf("%sファイルが開けません\n", FileName);
		return;
	}

	*this = "[";
	//１行目はタイトル
	while (true) {
		auto ret = readLine(fd, s, sizeof(s));
		if (ret < 0) break;
		//分解する
		char* p = strtok(s, ",");
		int ptr = 0;
		if (p) {
			if (isNumber(p)) {
				ptr += ::sprintf(t + ptr, "%s", p);
			}
			else {
				ptr += ::sprintf(t + ptr, "\"%s\"", p);
			}
		}
		while ((p = strtok(NULL, ",")) != 0) {
			if (isNumber(p)) {
				ptr += ::sprintf(t + ptr, ",%s", p);
			}
			else {
				ptr += ::sprintf(t + ptr, ",\"%s\"", p);
			}
		}
		if (first) {
			first = 0;
		}
		else {
			*this += ",";
		}
		*this += wString("[") + t + "]";
	}
	*this += "]";
	close(fd);
	return;
}
//---------------------------------------------------------------------------
// ファイル書き込み
//---------------------------------------------------------------------------
int wString::SaveToFile(const wString& str)
{
	return SaveToFile(str.String);
}
//---------------------------------------------------------------------------
// ファイル書き込み
//---------------------------------------------------------------------------
int wString::SaveToFile(const char* FileName)
{
#ifdef linux
	int handle = myopen(FileName, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#else
	int handle = myopen(FileName, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, S_IREAD | S_IWRITE);
#endif
	if (handle < 0) {
		return handle;
	}
	write(handle, String, len);
	close(handle);
	return 0;
}
//---------------------------------------------------------------------------
// 大文字化
//---------------------------------------------------------------------------
wString wString::ToUpper(void)
{
	wString temp(*this);
	for (auto i = 0U; i < Length(); i++)
	{
		temp.String[i] = (char)toupper(temp.String[i]);
	}
	return temp;
}
//---------------------------------------------------------------------------
// 大文字化
//---------------------------------------------------------------------------
wString wString::ToLower(void)
{
	wString temp(*this);
	for (auto i = 0U; i < Length(); i++)
	{
		temp.String[i] = (char)tolower(temp.String[i]);
	}
	return temp;
}
//---------------------------------------------------------------------------
// トリム
//---------------------------------------------------------------------------
wString wString::Trim(void)
{
	wString temp(*this);
	if (temp.len) {
		//先頭の空白等を抜く
		while (temp.len && *temp.String <= ' ') {
#ifdef linux
			char* src = temp.String;
			char* dst = src + 1;
			while (*src) {
				*src++ = *dst++;
			}
#else
			strcpy(temp.String, temp.String + 1);
#endif
			temp.len--;
		}
		//末尾の空白等を抜く
		while (temp.len && temp.String[temp.len - 1] <= ' ') {
			temp.String[--temp.len] = 0;
		}
	}
	return temp;
}
//---------------------------------------------------------------------------
// トリム
//---------------------------------------------------------------------------
wString wString::RTrim(void)
{
	wString temp(*this);
	if (temp.len) {
		//末尾の空白等を抜く
		while (temp.len && temp.String[temp.len - 1] <= ' ') {
			temp.String[--temp.len] = 0;
		}
	}
	return temp;
}
//---------------------------------------------------------------------------
// sentence文字列の行末に、cut_charがあったとき、削除
//---------------------------------------------------------------------------
void wString::Rtrimch(char* sentence, unsigned char cut_char)
{
	if (sentence == NULL || *sentence == 0) return;
	char* source_p;
	auto length = (int)strlen(sentence);        // 文字列長Get
	source_p = sentence;
	source_p += length;                 // ワークポインタを文字列の最後にセット。
	for (auto i = 0; i < length; i++) {       // 文字列の数だけ繰り返し。
		source_p--;                     // 一文字ずつ前へ。
		if (*source_p == cut_char) {     // 削除キャラ ヒットした場合削除
			*source_p = '\0';
		}
		else {                          // 違うキャラが出てきたところで終了。
			break;
		}
	}
	return;
}
//---------------------------------------------------------------------------
// トリム
//---------------------------------------------------------------------------
#if 1
wString wString::LTrim(void)
{
	wString temp(*this);
	if (temp.len) {
		//先頭の空白等を抜く
		while (temp.len && *temp.String <= ' ') {
			char* src = temp.String;
			char* dst = src + 1;
			while (*src) {
				*src++ = *dst++;
			}
			temp.len--;
		}
	}
	return temp;
}
#else
void wString::LTrim(void)
{
	if (len) {
		//先頭の空白等を抜く
		while (len && *String <= ' ') {
			char* src = String;
			char* dst = src + 1;
			while (*src) {
				*src++ = *dst++;
			}
			len--;
		}
	}
}
#endif
#if 1
//--------------------------------------------------------------------
wString wString::FileStats(const char* str, int mode)
{
	struct stat      stat_buf;
	wString buf;
	if (stat(str, &stat_buf) == 0 && mode == 0) {
		/* ファイル情報を表示 */

#ifdef linux
		buf.sprintf("{\"permission\":\"%o\",\"size\":%d,\"date\":\"%s\"}", stat_buf.st_mode, stat_buf.st_size, ctime(&stat_buf.st_mtime));
#else
		buf.sprintf("{\"permission\":\"%o\",\"size\":%d,\"date\":\"%s\"}", stat_buf.st_mode, stat_buf.st_size, ctimew(&stat_buf.st_mtime).c_str());
#endif
		//printf("デバイスID : %d\n",stat_buf.st_dev);
		//printf("inode番号 : %d\n",stat_buf.st_ino);
		//printf("アクセス保護 : %o\n",stat_buf.st_mode );
		//printf("ハードリンクの数 : %d\n",stat_buf.st_nlink);
		//printf("所有者のユーザID : %d\n",stat_buf.st_uid);
		//printf("所有者のグループID : %d\n",stat_buf.st_gid);
		//printf("デバイスID（特殊ファイルの場合） : %d\n",stat_buf.st_rdev);
		//printf("容量（バイト単位） : %d\n",stat_buf.st_size);
		//printf("ファイルシステムのブロックサイズ : %d\n",stat_buf.st_blksize);
		//printf("割り当てられたブロック数 : %d\n",stat_buf.st_blocks);
		//printf("最終アクセス時刻 : %s",ctime(&stat_buf.st_atime));
		//printf("最終修正時刻 : %s",ctime(&stat_buf.st_mtime));
		//printf("最終状態変更時刻 : %s",ctime(&stat_buf.st_ctime));
	}
	else {
		//date
		if (mode == 1) {
			//char s[128] = {0};
			//time_t timer;
			//struct tm *timeptr;
			//timer = time(NULL);
			//timeptr = localtime(&stat_buf.st_mtime);
			//strftime(s, 128, "%Y/%m/%d %H:%M:%S", timeptr);
			buf.sprintf("%d", stat_buf.st_mtime);
		}
	}
	return buf;
}
//---------------------------------------------------------------------------
wString wString::FileStats(const wString& str, int mode)
{
	return FileStats(str.String, mode);
}
#endif
//---------------------------------------------------------------------------
int wString::FileExists(const char* str)
{
	int  flag = 0;
#ifdef linux
	struct stat send_filestat;
	int  result = stat(str, &send_filestat);
	if ((result == 0) && (S_ISREG(send_filestat.st_mode) == 1)) {
		flag = 1;
	}
#else
	WIN32_FIND_DATA fd; //検索データ
	HANDLE result = ::FindFirstFile(str, &fd); //最初の検索で使用する関数
	if (result != INVALID_HANDLE_VALUE) {
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			flag = 1;
		}
		FindClose(result); //ハンドルを閉じる
	}
#endif
	return flag;
}
//---------------------------------------------------------------------------
int wString::FileExists(const wString& str)
{
	return FileExists(str.String);
}
//---------------------------------------------------------------------------
int wString::FileExists(void)
{
	return FileExists(String);
}
//---------------------------------------------------------------------------
//パス部分を抽出
wString wString::ExtractFileDir(wString& str)
{
	//todo SJIS/EUC対応するように
	wString temp(str);
	int ptr = temp.LastDelimiter(DELIMITER);
	temp.len = ptr;
	temp.String[ptr] = 0;
	return temp;
}
//---------------------------------------------------------------------------
int wString::CreateDir(const wString& str)
{
	int flag = 0;
#ifdef linux
	//0x777ではちゃんとフォルダできない
	flag = (mkdir(str.String, 0777) != -1);
#else
	char work[2048];
	strcpy(work, str.c_str());
	WindowsFileName(work);
	if (!DirectoryExists(work)) {
		flag = (mkdir(work) != -1);
	}
#endif
	return flag;
}
//---------------------------------------------------------------------------
//　TStringList対策
//  行数を返す
//---------------------------------------------------------------------------
void wString::ResetLength(unsigned int num)
{
	assert(total > (unsigned int)num);
	String[num] = 0;
	len = num;
}
//---------------------------------------------------------------------------
char* wString::c_str(void) const
{
	return String;
}
//---------------------------------------------------------------------------
unsigned int wString::Length(void) const
{
	return len;
}
//---------------------------------------------------------------------------
unsigned int wString::Total(void) const
{
	return total;
}
//---------------------------------------------------------------------------
int wString::LastDelimiter(const char* delim) const
{
	auto pos = -1;
	auto dlen = (int)strlen(delim);
	for (auto i = (int)len - dlen; i > 0; i--) {
		if (strncmp(String + i, delim, dlen) == 0) {
			pos = i;
			break;
		}
	}
	return pos;
}
//---------------------------------------------------------------------------
bool wString::RenameFile(const wString& src, const wString& dst)
{
	if (rename(src.c_str(), dst.c_str()) >= 0) {
		return true;
	}
	else {
		return false;
	}
}
//---------------------------------------------------------------------------
unsigned long wString::FileSizeByName(char* str)
{
	unsigned long pos;
	int handle;
#ifdef linux
	handle = open(str, 0);
#else
	handle = open(str, O_BINARY);
#endif
	pos = lseek(handle, 0, SEEK_END);
	close(handle);
	return pos;
}
//---------------------------------------------------------------------------
unsigned long wString::FileSizeByName(wString& str)
{
	return FileSizeByName(str.String);
}
//---------------------------------------------------------------------------
wString wString::ExtractFileName(const char* str, const char* delim)
{
	wString tmp(str);
	return ExtractFileName(tmp, delim);
}
//---------------------------------------------------------------------------
wString wString::ExtractFileName(const wString& str, const char* delim)
{
	int pos = str.LastDelimiter(delim);
	return str.substr(pos + 1);
}
//---------------------------------------------------------------------------
wString wString::ExtractFileExt(const wString& str)
{
	int pos = str.LastDelimiter(".");
	return str.substr(pos + 1);
}

//---------------------------------------------------------------------------
wString wString::ChangeFileExt(wString& str, const char* ext)
{
	int pos = str.LastDelimiter(".");
	return str.substr(0, pos + 1) + ext;
}
//---------------------------------------------------------------------------
int wString::DeleteFile(const wString& str)
{
	int flag = 0;
#ifdef linux
	flag = (unlink(str.String) == 0);
#else
	char work[2048];
	strcpy(work, str.c_str());
	WindowsFileName(work);
	if (FileExists(work)) {
		if (unlink(work) == 0) {
			flag = 1;
		}
	}
#endif
	return flag;
}
//---------------------------------------------------------------------------
int wString::DirectoryExists(const char* str)
{
	int flag = 0;
#ifdef linux
	struct stat send_filestat;
	int  result = stat(str, &send_filestat);
	if ((result == 0) && (S_ISDIR(send_filestat.st_mode) == 1)) {
		flag = 1;
	}
#else

	char tmp[1024];
	strcpy(tmp, str);
	// フォルダの末尾は'/'以外
	if (tmp[strlen(tmp) - 1] == '/')
	{
		tmp[strlen(tmp) - 1] = 0;
	}
	WIN32_FIND_DATA fd; //検索データ
	HANDLE result = ::FindFirstFile(tmp, &fd); //最初の検索で使用する関数
	if (result != INVALID_HANDLE_VALUE) {
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
			flag = 1;
		}
		FindClose(result); //ハンドルを閉じる
	}
#endif
	return flag;
}
//---------------------------------------------------------------------------
int wString::DirectoryExists(const wString& str)
{
	return DirectoryExists(str.String);
}
/********************************************************************************/
// sentence文字列内のkey文字列をrep文字列で置換する。
// sentence:元文字列
// slen:元文字列の長さ
// p:置換前の位置
// klen:置換前の長さ
// rep:置換後文字列
/********************************************************************************/
void wString::replace_character_len(const char* sentence, int slen, const char* p, int klen, const char* rep)
{
	auto rlen = (int)strlen(const_cast<char*>(rep));
	int num;
	if (klen == rlen) {
		memcpy(const_cast<char*>(p), rep, rlen);
		//前詰め置換そのままコピーすればいい
	}
	else if (klen > rlen) {
		num = klen - rlen;
		strcpy(const_cast<char*>(p), p + num);
		memcpy(const_cast<char*>(p), rep, rlen);
		//置換文字が長いので後詰めする
	}
	else {
		num = rlen - klen;
		//pからrlen-klenだけのばす
		for (auto str = const_cast<char*>(sentence + slen + num); str > p + num; str--) {
			*str = *(str - num);
		}
		memcpy(const_cast<char*>(p), rep, rlen);
	}
	return;
}
//---------------------------------------------------------------------------
// フォルダのファイルを数える
// 引数　wString Path:
// 戻値　ファイルリスト。ほっておいていいが、コピーしてほしい
wString wString::EnumFolderjson(const wString& Path)
{
#ifdef linux
	struct dirent** namelist;
	int n;
	wString temp;
	wString Path2;
	Path2 = Path;
	int first = 1;
	//Directoryオープン
	n = scandir(Path.String, &namelist, NULL, alphasort);
	if (n >= 0) {
		temp = "[";
		for (int i = 0; i < n; i++) {
			if (first) {
				first = 0;
			}
			else {
				temp += ",";
			}
			temp += "\"" + Path2 + DELIMITER + namelist[i]->d_name + "\"";
			free(namelist[i]);
		}
		temp += "]";
		free(namelist);
	}
	else {
		perror("ディレクトリのオープンエラー");
		exit(1);
	}
	return temp;
#else
	DIR* dir;
	wString              temp;
	wString              Path2;

	Path2 = Path;
	//Directoryオープン
	if ((dir = opendir(Path.String)) != NULL) {
		struct dirent* ent;
		std::vector<wString> list;
		//ファイルリスト
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") != 0 &&
				strcmp(ent->d_name, "..") != 0) {
				list.push_back("\"" + Path2 + DELIMITER + ent->d_name + "\"");
			}
		}
		closedir(dir);

		//sort
		if (list.size() > 0) {
			for (unsigned int ii = 0; ii < list.size() - 1; ii++) {
				for (unsigned int jj = ii + 1; jj < list.size(); jj++) {
					if (strcmp(list[ii].c_str(), list[jj].c_str()) > 0) {
						std::swap(list[ii], list[jj]);
					}
				}
			}
		}        temp = "[";
		for (unsigned int i = 0; i < list.size(); i++) {
			if (i) temp += ",";
			temp += list[i];
		}
		temp += "]";
	}
	else {
		perror("ディレクトリのオープンエラー");
		exit(1);
	}
	return temp;
#endif
}
wString wString::dump() const
{
	wString tmp;
	void* ptr = String;
	int vlen = len;
	unsigned char* out = static_cast<unsigned char*>(ptr);
	for (int start = 0; start < vlen; start += 16)
	{
		tmp.cat_sprintf("%04d:", start);
		for (int line = start; line < start + 16 && line < vlen; line++)
		{
			tmp.cat_sprintf("%02X ", out[line]);
		}
		for (int line = start; line < start + 16 && line < vlen; line++)
		{
			if (out[line] >= ' ' && out[line] < 0x7f)
			{
				tmp.cat_sprintf("%c", out[line]);
			}
			else
			{
				tmp.cat_sprintf(".");
			}
		}
		tmp.cat_sprintf("\r\n");
	}
	return tmp;
}

//---------------------------------------------------------------------------
// フォルダのファイルを数える
// 引数　wString Path:
// 戻値　ファイルリスト。ほっておいていいが、コピーしてほしい
wString wString::EnumFolder(const wString& Path)
{
#ifdef linux
	struct dirent** namelist;
	int n;
	wString temp;
	wString Path2;
	Path2 = Path;
	int first = 1;

	n = scandir(Path.String, &namelist, NULL, alphasort);
	if (n < 0) {
		temp = "";
	}
	else
	{
		temp = "[";
		for (int i = 0; i < n; i++) {
			if (first) {
				first = 0;
			}
			else {
				temp += ",";
			}
			temp += "\"" + Path2 + DELIMITER + namelist[i]->d_name + "\"";
			free(namelist[i]);
		}
		temp += "]";
		free(namelist);
	}
#else
	DIR* dir;
	struct dirent* ent;
	wString              temp;
	wString              Path2;
	std::vector<wString> list;
	Path2 = Path;
	//Directoryオープン
	if ((dir = opendir(Path.String)) != NULL) {
		//ファイルリスト
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") != 0 &&
				strcmp(ent->d_name, "..") != 0) {
				list.push_back(Path2 + DELIMITER + ent->d_name);
			}
		}
		closedir(dir);
		if (list.size() > 0) {
			for (unsigned int ii = 0; ii < list.size() - 1; ii++) {
				for (unsigned int jj = ii + 1; jj < list.size(); jj++) {
					if (strcmp(list[ii].c_str(), list[jj].c_str()) > 0) {
						std::swap(list[ii], list[jj]);
					}
				}
			}
		}
		for (unsigned int i = 0; i < list.size(); i++) {
			temp.Add(list[i]);
		}
	}
	else {
		perror("ディレクトリのオープンエラー");
		exit(1);
	}
#endif
	return temp;
}
#ifndef va_copy
int wString::vtsprintf(const char* fmt, va_list arg) {
	int len = 0;
	int size = 0;
	int zeroflag, width;


	while (*fmt) {
		if (*fmt == '%') {        /* % に関する処理 */
			zeroflag = width = 0;
			fmt++;
			if (*fmt == '0') {
				fmt++;
				zeroflag = 1;
			}
			while ((*fmt >= '0') && (*fmt <= '9')) {
				width *= 10;
				width += *(fmt++) - '0';
			}

			/* printf ("zerof = %d,width = %d\n",zeroflag,width); */

			//lluもluもuも同じ
			while (*fmt == 'l' || *fmt == 'z') {
				*fmt++;
			}

			switch (*fmt) {
			case 'd':        /* 10進数 */
				size = tsprintf_decimal(va_arg(arg, signed long), zeroflag, width);
				break;
			case 'u':        /* 10進数 */
				size = tsprintf_decimalu(va_arg(arg, unsigned long), zeroflag, width);
				break;
			case 'o':        /* 8進数 */
				size = tsprintf_octadecimal(va_arg(arg, unsigned long), zeroflag, width);
				break;
			case 'x':        /* 16進数 0-f */
				size = tsprintf_hexadecimal(va_arg(arg, unsigned long), 0, zeroflag, width);
				break;
			case 'X':        /* 16進数 0-F */
				size = tsprintf_hexadecimal(va_arg(arg, unsigned long), 1, zeroflag, width);
				break;
			case 'c':        /* キャラクター */
				size = tsprintf_char(va_arg(arg, int));
				break;
			case 's':        /* ASCIIZ文字列 */
				size = tsprintf_string(va_arg(arg, char*));
				break;
			default:        /* コントロールコード以外の文字 */
				/* %%(%に対応)はここで対応される */
				len++;
				*this += *fmt;
				break;
			}
			len += size;
			fmt++;
		}
		else {
			*this += *(fmt++);
			len++;
		}
	}
	va_end(arg);
	return (len);
}


/*
数値 => 10進文字列変換
*/
int wString::tsprintf_decimal(signed long val, int zerof, int width)
{
	//末尾０を保証
	char  tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int   len = 0;
	int   minus = 0;

	if (!val) {        /* 指定値が0の場合 */
		*(ptmp--) = '0';
		len++;
	}
	else {
		/* マイナスの値の場合には2の補数を取る */
		if (val < 0) {
			val = ~val;
			val++;
			minus = 1;
		}
		while (val && len < 19) {
			/* バッファアンダーフロー対策 */
			//if (len >= 19){
			//    break;
			//}

			*ptmp = (char)((val % 10) + '0');
			val /= 10;
			ptmp--;
			len++;
		}

	}

	/* 符号、桁合わせに関する処理 */
	if (zerof) {
		if (minus) {
			width--;
		}
		while (len < width) {
			*(ptmp--) = '0';
			len++;
		}
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
	}
	else {
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
		while (len < width) {
			*(ptmp--) = ' ';
			len++;
		}
	}
	*this += (ptmp + 1);
	return (len);
}
/*
数値 => 10進文字列変換
*/
int wString::tsprintf_decimalu(unsigned long val, int zerof, int width)
{
	char tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int len = 0;
	int minus = 0;

	if (!val) {        /* 指定値が0の場合 */
		*(ptmp--) = '0';
		len++;
	}
	else {
		while (val) {
			/* バッファアンダーフロー対策 */
			if (len >= 19) {
				break;
			}

			*ptmp = (char)((val % 10) + '0');
			val /= 10;
			ptmp--;
			len++;
		}
	}

	/* 符号、桁合わせに関する処理 */
	if (zerof) {
		if (minus) {
			width--;
		}
		while (len < width) {
			*(ptmp--) = '0';
			len++;
		}
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
	}
	else {
		while (len < width) {
			*(ptmp--) = ' ';
			len++;
		}
	}

	*this += (ptmp + 1);
	return (len);
}
/*
数値 => 8進文字列変換
*/
int wString::tsprintf_octadecimal(unsigned long val, int zerof, int width)
{
	char tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int len = 0;
	int minus = 0;

	if (!val) {        /* 指定値が0の場合 */
		*(ptmp--) = '0';
		len++;
	}
	else {
		while (val) {
			/* バッファアンダーフロー対策 */
			if (len >= 19) {
				break;
			}

			*ptmp = (char)((val % 8) + '0');
			val /= 8;
			ptmp--;
			len++;
		}
	}

	/* 符号、桁合わせに関する処理 */
	if (zerof) {
		if (minus) {
			width--;
		}
		while (len < width) {
			*(ptmp--) = '0';
			len++;
		}
		if (minus) {
			*(ptmp--) = '-';
			len++;
		}
	}
	else {
		while (len < width) {
			*(ptmp--) = ' ';
			len++;
		}
	}

	*this += (ptmp + 1);
	return (len);
}
/*
数値 => 16進文字列変換
*/
int wString::tsprintf_hexadecimal(unsigned long val, int capital, int zerof, int width)
{
	char tmp[22] = { 0 };
	char* ptmp = tmp + 20;
	int len = 0;
	char str_a;

	/* A～Fを大文字にするか小文字にするか切り替える */
	if (capital) {
		str_a = 'A';
	}
	else {
		str_a = 'a';
	}

	if (!val) {        /* 指定値が0の場合 */
		*(ptmp--) = '0';
		len++;
	}
	else {
		while (val) {
			/* バッファアンダーフロー対策 */
			if (len >= 18) {
				break;
			}

			*ptmp = (char)(val % 16);
			if (*ptmp > 9) {
				*ptmp += (char)(str_a - 10);
			}
			else {
				*ptmp += '0';
			}

			val >>= 4;        /* 16で割る */
			ptmp--;
			len++;
		}
	}
	while (len < width) {
		*(ptmp--) = zerof ? '0' : ' ';
		len++;
	}

	*this += (ptmp + 1);
	return(len);
}

/*
数値 => 1文字キャラクタ変換
*/
int wString::tsprintf_char(int ch)
{
	*this += (char)ch;
	return(1);
}

/*
数値 => ASCIIZ文字列変換
*/
int wString::tsprintf_string(char* str)
{
	*this += str;
	return(strlen(str));
}
#endif
//---------------------------------------------------------------------------
//wString 可変引数
int wString::sprintf(const char* format, ...)
{
#ifndef va_copy
	String[0] = 0;
	len = 0;
	va_list ap;
	va_start(ap, format);
	vtsprintf(format, ap);
	va_end(ap);
	return len;
#else
	int stat;
	//可変引数を２つ作る
	va_list ap1, ap2;
	va_start(ap1, format);
	va_copy(ap2, ap1);
	//最初はダミーで文字列長をシミュレート
	stat = vsnprintf(String, 0, format, ap1);
	SetLength(stat + 1);
	//実際に出力
	stat = vsprintf(String, format, ap2);
	va_end(ap1);
	va_end(ap2);
	len = stat;
	return stat;
#endif
}
//---------------------------------------------------------------------------
//wString 可変引数
int wString::cat_sprintf(const char* format, ...)
{
#ifndef va_copy
	int status;
	va_list ap;
	va_start(ap, format);
	status = this->vtsprintf(format, ap);
	va_end(ap);
	return status;
#else
	int stat;
	//可変引数を２つ作る
	va_list ap1, ap2;
	va_start(ap1, format);
	va_copy(ap2, ap1);
	//最初はダミーで文字列長をシミュレート
	stat = vsnprintf(String, 0, format, ap1);
	SetLength(stat + len + 1);
	//実際に出力
	stat = vsprintf(String + len, format, ap2);
	va_end(ap1);
	va_end(ap2);
	len += stat;
	return stat;
#endif
}
//---------------------------------------------------------------------------
/// <summary>
/// 文字列をデリミタで切って、デリミタ後の文字を返す
/// </summary>
/// <param name="delimstr">切断文字列</param>
/// <returns>切断後の文字列。見つからない場合は長さ０の文字列</returns>
wString wString::strsplit(const char* delimstr)
{
	wString tmp;
	char* ptr = c_str();
	auto delimlen = (int)strlen(delimstr);
	int start = 0;
	for (auto i = start; i < len; i++)
	{
		for (auto j = 0; j < delimlen; j++)
		{
			if (ptr[i] == delimstr[j])
			{
				ptr[i] = 0;
				tmp.Add(&ptr[start]);
				start = i + 1;
				break;
			}
		}
	}
	if (start < len)
	{
		tmp.Add(&ptr[start]);
	}
	return tmp;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void wString::resize(const int newsize)
{
	if (len >= total) {
		printf("not good %u %u", len, total);
		exit(1);
	}
	if ((int)total <= newsize) {
		// すこし大き目に TODO
		total = newsize + 1;
		char* tmp = new char[total];
		memcpy(tmp, String, len);
		tmp[len] = 0;
		delete[] String;
		String = tmp;
	}
	else {
		//指定サイズが元より小さいので何もしない
	}
}
//---------------------------------------------------------------------------
//HTML特殊文字に変換
wString wString::htmlspecialchars(void)
{
	wString temp;
	// 引数チェック
	char* ptr = String;
	for (unsigned int is = 0; is < len; is++) {
		switch (*ptr) {
		case '&':temp += "&amp;"; break;
		case '\"':temp += "&quot;"; break;
			//case '\'':temp += "&#39;";break;
		case '<':temp += "&lt;";  break;
		case '>':temp += "&gt;";  break;
		default:temp += *ptr;    break;
		}
		ptr++;
	}
	return temp;
}
//---------------------------------------------------------------------------
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
wString wString::uri_encode(void)
{
	unsigned int is;
	unsigned char datum;
	wString dst;
	char work[8];
	// 引数チェック
	for (is = 0; is < len; is++) {
		datum = (unsigned char)String[is];
		/* ' '(space) はちと特別扱いにしないとまずい */
		if (datum == ' ') {
			dst += "%20";
			/* エンコードしない文字全員集合 */
			//        }else if ( strchr("!$()*,-./:;?@[]^_`{}~", String[is]) != NULL ){
		}
		else if (strchr("!$()*,-.:;/?@[]^_`{}~", datum) != NULL) {
			dst += datum;
			/* アルファベットと数字はエンコードせずそのまま */
		}
		else if (isalnum(datum)) {
			dst += datum;
		}
		/* \マークはエンコード */
		else if (datum == '\\') {
			dst += "%5c";
			/* それ以外はすべてエンコード */
		}
		else {
			::sprintf(work, "%%%2X", datum);
			dst += work;
		}
	}
	return dst;
}
int   wString::FileCopy(const char* fname_r, const char* fname_w)
{
	int fpr;
	int fpw;
	int size;
	unsigned char buf[8000];

	fpr = myopen(fname_r, O_RDONLY | O_BINARY, S_IREAD);
	if (fpr < 0) {
		return -1;
	}
	fpw = myopen(fname_w, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE);
	if (fpw < 0) {
		close(fpr);
		return -1;
	}
	while (1) {
		size = read(fpr, buf, sizeof(buf));
		if (size <= 0) {
			break;
		}
		write(fpw, buf, size);
	}
	close(fpr);
	close(fpw);

	return 0;
}
//---------------------------------------------------------------------------
//QUOTE,ESCAPE,NULLを保護する
wString wString::addSlashes(void)
{
	wString tmp;
	for (unsigned int i = 0; i < len; i++) {
		if (String[i] == '\'' || String[i] == '\"' || String[i] == '\\' || String[i] == 0) {
			tmp += '\\';
		}
		tmp += String[i];
	}
	return tmp;
}
// **************************************************************************
// URIデコードを行います.
//  機能 : URIデコードを行う
//  引数 : dst 変換した文字の書き出し先.
//                dst_len 変換した文字の書き出し先の最大長.
//                src 変換元の文字.
//                src_len 変換元の文字の長さ.
// 返値 : デコードした文字の数(そのままも含む)
// **************************************************************************
unsigned char htoc(unsigned char x)
{
	if ('0' <= x && x <= '9') return (unsigned char)(x - '0');
	if ('a' <= x && x <= 'f') return (unsigned char)(x - 'a' + 10);
	if ('A' <= x && x <= 'F') return (unsigned char)(x - 'A' + 10);
	return 0;
}
wString wString::uri_decode()
{
	unsigned char   code;
	wString dst;
	// =================
	// メインループ
	// =================
	for (auto i = 0U; i < len && String[i] != '\0'; i++) {
		if (String[i] == '%') {
			if (i + 2 >= len) {
				break;
			}
			code = (unsigned char)(htoc(String[++i]) << 4);
			code += htoc(String[++i]);
			dst += code;
		}
		else if (String[i] == '+') {
			dst += ' ';
		}
		else {
			dst += String[i];
		}
	}
	return dst;
}
#ifdef WEB
void wString::headerInit(size_t content_length, int expire, const char* mime_type)
{
	sprintf("%s", HTTP_OK);
	cat_sprintf("%s", HTTP_CONNECTION);
	cat_sprintf(HTTP_SERVER_NAME, SERVER_NAME);
	cat_sprintf(HTTP_CONTENT_TYPE, mime_type);
	//Date
	time_t timer;
	time(&timer);
	//struct tm *utc;
	//utc = gmtime(&timer);
	struct tm utc;
	gmtime_r(&timer, &utc);
	char work[80] = { 0 };
	char you[7][4] = { "Sun", "Mon","Tue", "Wed", "Thu", "Fri", "Sat" };
	char mon[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	::sprintf(work, "%s, %d %s %d %02d:%02d:%02d",
		you[utc.tm_wday], utc.tm_mday, mon[utc.tm_mon], utc.tm_year + 1900, utc.tm_hour, utc.tm_min, utc.tm_sec);
	this->cat_sprintf("Date: %s GMT\r\n", work);
	//expire
	if (expire) {
		timer += 60 * 60;
		//utc = gmtime(&timer);
		gmtime_r(&timer, &utc);
		::sprintf(work, "%s, %d %s %d %02d:%02d:%02d",
			you[utc.tm_wday], utc.tm_mday, mon[utc.tm_mon], utc.tm_year + 1900, utc.tm_hour, utc.tm_min, utc.tm_sec);
		this->cat_sprintf("Expires: %s GMT\r\n", work);
	}
	if (content_length) {
		this->cat_sprintf(HTTP_CONTENT_LENGTH, content_length);
	}
}
void wString::headerPrint(SOCKET socket, int endflag)
{
	send(socket, String, len, 0);
	if (endflag) {
		send(socket, HTTP_END, (int)strlen(HTTP_END), 0);
	}
}
wString wString::headerPrintMem(void)
{
	return *this + HTTP_END;
}
int wString::header(const char* str, int flag, int status)
{
	char head[80] = { 0 };
	char body[80] = { 0 };
	if (*str) {
		if (!flag) {
			Add(str);
			return 0;
		}
		for (auto i = 0; i < (int)strlen(str); i++) {
			if (str[i] == ' ') {
				memcpy(head, str, i + 1);
				head[i + 1] = 0;
				strcpy(body, str + i + 1);
				break;
			}
		}
		if (*head && *body) {
			if (strcmp(head, "Location: ") == 0) {
				if (status == 301) {
					header("HTTP/1.0 301 Moved Permanetry", true);
				}
				else {
					header("HTTP/1.0 302 Found", true);
				}
			}
			int count = Counts();
			wString str2;
			str2.sprintf("%s%s", head, body);
			//あれば入れ替え
			for (int i = 0; i < count; i++) {
				wString tmp = GetListString(i);
				if (strncmp(tmp.c_str(), head, strlen(head)) == 0) {
					SetListString(str2.c_str(), i);
					return 0;
				}
			}
			//なければ追加
			Add(str2);
			return 0;
		}
	}
	return 1;
}
/********************************************************************************/
// 日本語文字コード変換。
// (libnkfのラッパー関数)
//
//      サポートされている形式は以下の通り。
//      in:S:SJIS E:EUC W:UTF-8
//      out:s:SJIS w:UTF-8
/********************************************************************************/
wString wString::nkfcnv(const wString& option)
{
	wString ptr(len * 3);
	//=================================================
	// libnkf 実行
	//=================================================
	nkf(static_cast<const char*>(String), static_cast<char*>(ptr.c_str()), len * 3, static_cast<const char*>(option.c_str()));
	// 文字列の長さの調整
	ptr.len = (unsigned int)strlen(ptr.c_str());
	return ptr;
}
#endif
//---------------------------------------------------------------------------
void wString::Add(const char* str)
{
	*this += str;
	*this += "\r\n";
}
//---------------------------------------------------------------------------
void wString::Add(const wString& str)
{
	*this += str;
	*this += "\r\n";
}

//---------------------------------------------------------------------------
//　TStringList対策
//  行数を返す
//---------------------------------------------------------------------------
int wString::Counts(void)
{
	int count = 0;
	char* ptr = String;
	while (*ptr) {
		if (*ptr == '\r') {
			ptr++;
			continue;
		}
		if (*ptr++ == '\n') {
			count++;
		}
	}
	return count;
}

//---------------- -----------------------------------------------------------
//　TStringList対策
//  行数を返す
//　リエントラントでないことに注意
//　戻り値は使い捨て
//---------------- -----------------------------------------------------------
bool wString::SetListString(wString& src, int pos)
{
	bool flag;
	flag = SetListString(src.String, pos);
	return flag;
}
//---------------- -----------------------------------------------------------
//　TStringList対策
//  pos行目(0start)にdstを挿入
//---------------- -----------------------------------------------------------
bool wString::SetListString(const char* dst, int pos)
{
	//行数が多い
	std::vector<wString> work;
	int count = 0;
	int ptr = 0;
	int last = 0;
	int cchar = 0;
	while (String[ptr]) {
		switch (String[ptr]) {
		case '\r':
			ptr++;
			cchar++;
			continue;
		case '\n':
			ptr++;
			cchar++;
			count++;
			work.push_back(substr(last, ptr - last - cchar));
			cchar = 0;
			last = ptr;
			break;
		default:
			cchar = 0;
			ptr++;
		}
	}
	//行末にcrlfがなかった場合に対応
	if (ptr - last - cchar) {
		count++;
		work.push_back(substr(last, ptr - last - cchar));
	}
	//要素追加
	if (0 <= pos && pos <= count) {
		work[pos] = dst;
		//元を消す
		clear();
		for (unsigned int i = 0; i < (unsigned int)work.size(); i++) {
			*this += work[i] + "\r\n";
		}
		return true;
	}
	else {
		//なにもしない
		return false;
	}
}
//---------------------------------------------------------------------------
//　TStringList対策
//　pos行目の文字列を返す
wString wString::GetListString(int pos)
{
	int   lcount = 0;
	int   ptr = 0;
	int   curptr = 0;
	int   lstptr;
	int   spcnt = 0;//\r\nまたは\nの文字数
	while (String[ptr]) {
		if (String[ptr] == '\r') {
			ptr++;
			spcnt++;
			continue;
		}
		if (String[ptr++] == '\n') {
			lstptr = curptr;
			curptr = ptr;
			spcnt++;
			if (lcount++ == pos) {
				return substr(lstptr, curptr - lstptr - spcnt);
			}
		}
		spcnt = 0;
	}
	return wString();
}
#if 1
//---------------------------------------------------------------------------
//HTTPより文字列データを取得
//引数
// char* src    :URL
// off_t offset :読み込みオフセット
//戻り値
// wString&     :読み込んだ文字列。失敗したときは長さ０
//---------------------------------------------------------------------------
#define HTTP_STR_BUF_SIZE (1024)
wString wString::HTTPGet(const wString& url, off_t offset)
{
	//int         recv_len;                       //読み取り長さ
	wString     buf;
	wString     ptr;
	wString     host;                           //ホスト名
	wString     target;                         //ファイル名
	int         hostPos;
	int         locPos;
	int         portPos;
	SOCKET      server_socket;                  //サーバーソケット
	int         server_port = HTTP_SERVER_PORT;
	//出力ファイルの設定
	// ================
	// 実体転送開始
	// ================
	//buf = (char*)malloc(HTTP_BUF_SIZE);
	//ptr = buf;
	//準備
	//アドレスから、ホスト名とターゲットを取得
	ptr.SetLength(HTTP_STR_BUF_SIZE + 1);
	buf = url;
	//ptr = 0;
	hostPos = buf.Pos("://") + 3;
	locPos = buf.Pos("/", hostPos);
	if (locPos >= 0)
	{
		target = buf.SubString(locPos, buf.len - locPos);
		host = buf.SubString(hostPos, locPos - hostPos);
	}
	else
	{
		target = "";
		host = buf.SubString(hostPos, buf.len - hostPos);
	}
	portPos = host.Pos(":");
	if (portPos >= 0) {
		server_port = atoi(host.c_str() + portPos + 1);
		host = host.SubString(0, portPos - hostPos);
	}
	//ソケット作成と接続
	server_socket = sock_connect(host.String, server_port);
	if (!SERROR(server_socket)) {
		//HTTP1.0 GET発行
		ptr.sprintf("GET %s HTTP/1.0\r\n"
			"Accept: */*\r\n"
			"User-Agent: %s%s\r\nHost: %s\r\nRange: bytes=%lu-\r\nConnection: close\r\n\r\n",
			target.String,
			//"Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)",
			USERAGENT,
			MACADDR,
			host.String,
			offset);
		//ptr.len = strlen(ptr.String);
		//サーバに繋がった
		if (send(server_socket, ptr.String, ptr.len, 0) != SOCKET_ERROR) {

			//初回分からヘッダを削除
			auto recv_len = recv(server_socket, ptr.String, ptr.Total() - 1, 0);
			ptr.String[recv_len] = 0;
			ptr.len = recv_len;
			//見つからない
			hostPos = atoi(ptr.String + (ptr.Pos(" ") + 1));
			if (hostPos < 200 || 300 <= hostPos) {
				sClose(server_socket);
				return wString();
			}
			//content_length = atoi(buf.String+buf.Pos("Content-Length:" )+16);

			//\r\n\r\nを探す
			hostPos = ptr.Pos(HTTP_DELIMITER) + 4;//sizeof( HTTP_DELIMITER );//実体の先頭
			recv_len -= hostPos;
			buf = ptr.SubString(hostPos, recv_len);
			//転送する
			while (loop_flag) {
				recv_len = recv(server_socket, ptr.String, ptr.Total() - 1, 0);
				if (recv_len <= 0) {
					break;
				}
				//エラーにならない。
				ptr.len = recv_len;
				ptr.String[recv_len] = 0;
				buf += ptr;
			}
		}
		else {
			sClose(server_socket);
			return wString();
		}
		sClose(server_socket);
	}
	else {
		return wString();
	}
	wString tmp(buf);
	//終了
	return tmp;
}
//---------------------------------------------------------------------------
//HTTPより文字列データを取得
//引数
// wString& url    :URL
// wString& data   :送出データ
// method          :HEAD,GET,POST,PUT,DELETE
//戻り値
// wString&     :読み込んだ文字列。失敗したときは長さ０
//---------------------------------------------------------------------------
wString wString::HTTPRest(const wString& methods, const wString& url, const wString& data)
{
	//static char* methods[5]={"HEAD","GET","POST","PUT","DELETE"};
	//int         recv_len;                       //読み取り長さ
	wString     buf;
	wString     ptr;
	wString     host;                           //ホスト名
	wString     target;                         //ファイル名
	int         work1;
	int         work2;
	int         work3;
	SOCKET      server_socket;                  //サーバーソケット
	int         server_port = HTTP_SERVER_PORT;
	//出力ファイルの設定
	// ================
	// 実体転送開始
	// ================
	//buf = (char*)malloc(HTTP_BUF_SIZE);
	//ptr = buf;
	//準備
	//アドレスから、ホスト名とターゲットを取得
	buf = url;
	//ptr = 0;
	work1 = buf.Pos("://") + 3;
	work2 = buf.Pos("/", work1);
	work3 = buf.Pos(":", work1);
	target = buf.SubString(work2, buf.len - work2);
	if (work3 >= 0) {
		host = buf.SubString(work1, work3 - work1);
		server_port = atoi(buf.c_str() + work3 + 1);
	}
	else {
		host = buf.SubString(work1, work2 - work1);
	}
	//ソケット作成と接続
	server_socket = sock_connect(host.String, server_port);
	if (!SERROR(server_socket)) {
		//HTTP1.0 GET発行
		if (methods == "GET") {
			ptr.sprintf("%s %s HTTP/1.0\r\n"
				"Accept: */*\r\n"
				"User-Agent: %s%s\r\n"
				"Host: %s\r\n"
				"Connection: close\r\n\r\n",
				methods.c_str(),
				target.String,
				//"Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)",
				USERAGENT,
				MACADDR,
				host.String);
		}
		else {
			ptr.sprintf("%s %s HTTP/1.0\r\n"
				"Accept: */*\r\n"
				"User-Agent: %s%s\r\n"
				"Host: %s\r\n"
				"Content-Length: %d\r\n"
				"Content-Type: text/plain;charset=UTF-8\r\n"
				"Connection: close\r\n\r\n",
				methods.c_str(),
				target.String,
				//"Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)",
				USERAGENT,
				MACADDR,
				host.String,
				data.Length());
			//送出データ追加
			ptr += data;
		}
		//サーバに繋がった
		if (send(server_socket, ptr.String, ptr.len, 0) != SOCKET_ERROR) {
			ptr.SetLength(HTTP_STR_BUF_SIZE + 1);
			char buff[1024];
			ptr = "";
			int recv_len;
			//初回分からヘッダを削除
			for (;;) {
				recv_len = recv(server_socket, buff, sizeof(buff) - 1, 0);
				if (recv_len > 0) {
					buff[recv_len] = 0;
					ptr += buff;
				}
				else if (recv_len == 0) {
					break;
				}
				else {
					return wString();
				}
			}
			//見つからない
			work1 = atoi(ptr.String + (ptr.Pos(" ") + 1));
			if (work1 < 200 || 300 <= work1) {
				sClose(server_socket);
				return wString();
			}
			//content_length = atoi(buf.String+buf.Pos("Content-Length:" )+16);

			//\r\n\r\nを探す
			work1 = ptr.Pos(HTTP_DELIMITER) + 4;//sizeof( HTTP_DELIMITER );//実体の先頭
			recv_len -= work1;
			buf = ptr.SubString(work1, ptr.Length() - recv_len);
			//転送する
			while (loop_flag) {
				recv_len = recv(server_socket, ptr.String, ptr.Total() - 1, 0);
				if (recv_len <= 0) {
					break;
				}
				//エラーにならない。
				ptr.len = recv_len;
				ptr.String[recv_len] = 0;
				buf += ptr;
			}
		}
		else {
			sClose(server_socket);
			return wString();
		}
		sClose(server_socket);
	}
	else {
		return wString();
	}
	//終了
	return wString(buf);
}
//---------------------------------------------------------------------------
/* ソケットを作成し、相手に接続するラッパ. 失敗 = -1 */
//static int sock_connect(char *host, int port)
//---------------------------------------------------------------------------
SOCKET wString::sock_connect(const wString& host, const int port)
{
	return sock_connect(host.String, port);
}
//---------------------------------------------------------------------------
SOCKET wString::sock_connect(const char* host, const int port)
{
	//  int sock;
	SOCKET sock;
	struct sockaddr_in sockadd = { 0 };     //ＳＯＣＫＥＴ構造体
	struct hostent* hent;
	debug_log_output("sock_connect: %s:%d", host, port);
	//ＳＯＣＫＥＴ作成
	if (SERROR(sock = socket(PF_INET, SOCK_STREAM, 0))) {
		debug_log_output("sock_connect_error:");
		return INVALID_SOCKET;
	}
	//debug_log_output("sock: %d", sock);
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
		sClose(sock);
		return INVALID_SOCKET;
	}
	return sock;
}
//---------------------------------------------------------------------------
// 2004/11/17 Add start
// アクセス可能かチェックする
//形式はwww.make-it.co.jp/index.html
//2005/12/03 引数を壊していたので修正
bool wString::checkUrl(const wString& url)
{
	wString buf;
	//int     recv_len;
	bool    access_flag = false;
	wString host_name;
	wString file_path;
	int     ptr;
	//前処理
	ptr = url.Pos("/");
	// はじめに出てきた"/"の前後で分断
	host_name = url.SubString(0, ptr);
	file_path = url.SubString(ptr, url.Length() - ptr);
	//見つからなかった時
	if (file_path.Length() == 0) {
		file_path = "/";
	}
	SOCKET server_socket = sock_connect(host_name.c_str(), HTTP_SERVER_PORT);//( PF_INET , SOCK_STREAM , 0 );
	if (!SERROR(server_socket)) {
		buf.sprintf("HEAD %s HTTP/1.0\r\n"
			"User%cAgent: "
			USERAGENT
			"\r\n"
			"Host: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			file_path.c_str(),
			'-',
			host_name.c_str()
		);
		int dd = send(server_socket, buf.c_str(), buf.Length(), 0);
		if (dd != SOCKET_ERROR) {
			buf.SetLength(1024);
			auto recv_len = recv(server_socket, buf.c_str(), buf.Total() - 1, 0);
			buf.ResetLength(recv_len);
			//見つからない
			ptr = atoi(buf.String + (buf.Pos(" ") + 1));
			// 受信データありならば(ファイル有り）、データを解析する。
			if (200 <= ptr && ptr < 300) {
				access_flag = true;
			}
		}
		sClose(server_socket);
	}
	return access_flag;
}
//---------------------------------------------------------------------------
//目標のサイズ取得
int wString::HTTPSize(const wString& url)
{

	//int         recv_len;                       //読み取り長さ
	wString     buf;
	int         work1;
	int         work2;
	int         work3;
	wString     host;
	wString     target;
	int         content_length = -1;
	SOCKET      server_socket;                          //サーバーソケット
	int         server_port = HTTP_SERVER_PORT;
	//出力ファイルの設定
	// ================
	// 実体転送開始
	// ================
	//準備
	//アドレスから、ホスト名とターゲットを取得
	work2 = url.Pos("://") + 3;
	work1 = url.Pos("/", work2);
	work3 = url.Pos(":", work1);
	target = url.SubString(work1, url.Length() - work1);
	if (work3 >= 0) {
		host = url.SubString(work2, work3 - work2);
		server_port = atoi(url.c_str() + work3 + 1);
	}
	else {
		host = url.SubString(work2, work1 - work2);
	}
	//strcpy( target, work1);
	//*work1 = 0;
	//strcpy( host, work2 );

	//ソケット作成と接続
	server_socket = sock_connect(host.c_str(), server_port);
	if (!SERROR(server_socket)) {
		//HTTP1.0 GET発行
		buf.sprintf("HEAD %s HTTP/1.0\r\n"
			"Accept: */*\r\n"
			"User-Agent: %s%s\r\n"
			"Host: %s\r\n"
			//                       "%s",
			"Connection: close\r\n\r\n",
			target.uri_encode().c_str(),
			//"Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)",
			USERAGENT,
			MACADDR,
			host.c_str()
			//                        GetAuthorization(void),
		);
		//サーバに繋がった
		if (send(server_socket, buf.c_str(), buf.Length(), 0) != SOCKET_ERROR) {
			//初回分からヘッダを削除
			buf.SetLength(HTTP_STR_BUF_SIZE);
			auto recv_len = recv(server_socket, buf.c_str(), buf.Total() - 1, 0);
			//\r\n\r\nを探す
			buf.ResetLength(recv_len);      //糸止め
			work1 = atoi(buf.String + (buf.Pos(" ") + 1));
			int pos = buf.Pos("Content-Length:");
			if (pos >= 0) {
				if (200 <= work1 && work1 < 300) {
					//コンテンツ長さ TODO Content-Lengthがない場合
					content_length = atoi(buf.c_str() + pos + 16);
				}
			}
			else if (work1 == 302) {
				//Location:----\r\n
				work1 = buf.Pos("Location:");
				if (work1) {
					int num = buf.Pos("\r\n", work1) - work1 - 10;
					buf = buf.SubString(work1 + 10, num);
					content_length = HTTPSize(buf);
				}
			}
		}
		sClose(server_socket);
	}
	//終了
	return content_length;
}
#endif
//---------------------------------------------------------------------------
// linux用ファイル名に変換
//---------------------------------------------------------------------------
char* wString::WindowsFileName(char* FileName)
{
#ifdef linux
	return FileName;
#else
	char* work = FileName;
	int ptr = 0;
	while (work[ptr]) {
		if (work[ptr] == '/') {
			work[ptr] = '\\';
		}
		ptr++;
	}
	return work;
#endif
}
//---------------------------------------------------------------------------
// Linux用ファイル名に変換
//---------------------------------------------------------------------------
char* wString::LinuxFileName(char* FileName)
{
#ifdef linux
	return FileName;
#else
	char* work = FileName;
	int ptr = 0;
	while (work[ptr]) {
		if (work[ptr] == '\\') {
			work[ptr] = '/';
		}
		ptr++;
	}
	return work;
#endif
}
//---------------------------------------------------------------------------
// Linux用ファイル名に変換
//---------------------------------------------------------------------------
wString wString::GetLocalAddress(void)
{
#ifdef linux
	//linux番の自IP/ホスト名を設定する
	struct ifaddrs* ifa_list;
	struct ifaddrs* ifa;
	char addrstr[256] = { 0 };

	// 機器リスト取得
	if (getifaddrs(&ifa_list) == 0) {
		// 一覧作成
		for (ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr->sa_family == AF_INET) {
				// eth0の場合のみ
				if (*ifa->ifa_name == 'e') {
					inet_ntop(AF_INET, &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr, addrstr, sizeof(addrstr));
					break;
				}
			}
		}
		/* (7) */
		freeifaddrs(ifa_list);
		return addrstr;
	}
	else
	{
		return "";
	}
#else

	//ホスト名を取得する
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) != 0) {
		return "";
	}
	//puts(hostname);

	//ホスト名からIPアドレスを取得する
	HOSTENT* hostend = gethostbyname(hostname);
	if (hostend == NULL) {
		return "";
	}
	IN_ADDR inaddr;
	memcpy(&inaddr, hostend->h_addr_list[0], 4);
	//ip.resize(256);
	//static char ip[256];
	//strcpy(ip, inet_ntoa(inaddr));
	return inet_ntoa(inaddr);
#endif
}
//---------------------------------------------------------------------------
unsigned int wString::copy(char* str, unsigned int slen, int index) const
{
	strncpy(str, String + index, slen);
	str[slen] = 0;
	return slen;
}
//---------------------------------------------------------------------------
wString& wString::replace(int index, unsigned int slen, const wString& repstr)
{
	auto rlen = repstr.len;
	//同じ
	if (slen == rlen) {
		memcpy(String + index, repstr.String, rlen);
		//前詰め置換そのままコピーすればいい
	}
	else if (slen > rlen) {
		int num = slen - rlen;
		char* p = String + index;
		char* q = p + num;
		while (*q) {
			*p++ = *q++;
		}
		memcpy(String + index, repstr.String, rlen);
		len -= num;
		String[len] = 0;
		//置換文字が長いので後詰めする
	}
	else {
		int num = rlen - slen;
		resize(len + num + 1);
		for (char* p = (String + len + num); p > String + index + num; p--) {
			*p = *(p - num);
		}
		memcpy(String + index, repstr.String, rlen);
		len += num;
		String[len] = 0;
	}
	return *this;
}

#define ToBase64tbl "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
/// <summary>
/// BASE64エンコード
/// </summary>
/// <param name="">エンコードする文字列</param>
/// <returns>エンコードした文字列</returns>
wString wString::base64(void)
{
	wString tmpout;
	unsigned char* instr = reinterpret_cast<unsigned char*>(String);
	unsigned char ch = 0;
	int count = 0;
	/*
	ABC -> 414243 -> 0100 0001  0100 0010  0100 0011
		-> 010000 010100 001001 000011
		   10 14 09 03
	*/
	while (*instr) {
		switch (count) {
		case 0:
			ch = (unsigned char)((*instr >> 2) & 0x3f);
			break;
		case 1:
			ch = (unsigned char)((*instr++ << 4) & 0x3f);
			ch |= (unsigned char)((*instr >> 4) & 0x3f);
			break;
		case 2:
			ch = (unsigned char)((*instr++ << 2) & 0x3f);
			ch |= (unsigned char)((*instr >> 6) & 0x3f);
			break;
		case 3:
			ch = (unsigned char)((*instr++) & 0x3f);
			break;
		}
		tmpout += ToBase64tbl[ch];
		count++;
		count %= 4;
	}
	count = (4 - count) % 4;
	while (count-- > 0) {
		tmpout += '=';
	}
	return tmpout;
}
/// <summary>
/// BASE64デコード
/// </summary>
/// <param name="">デコードする文字列</param>
/// <returns>デコードされた文字列</returns>
wString wString::unbase64(void)
{
	unsigned char* instr = reinterpret_cast<unsigned char*>(String);

	char FromBase64tbl[256] = { 0 };
	wString tmpout;
	//逆テーブルの作成,=も０になる
	for (unsigned int i = 0; i < sizeof(ToBase64tbl); i++) {
		FromBase64tbl[(int)ToBase64tbl[i]] = (unsigned char)i;
	}

	/*
	ABC -> 414243 -> 0100 0001  0100 0010  0100 0011
	->   010000 010100 001001 000011
	10 14 09 03
	*/
	while (*instr) {
		int s1 = FromBase64tbl[*instr++];
		int s2 = FromBase64tbl[*instr++];
		int s3 = FromBase64tbl[*instr++];
		int s4 = FromBase64tbl[*instr++];
		tmpout += (char)((s1 << 2) | (s2 >> 4));
		tmpout += (char)(((s2 & 0x0f) << 4) | (s3 >> 2));
		tmpout += (char)(((s3 & 0x03) << 6) | s4);
	}
	return tmpout;
}
// **************************************************************************
// accept_socketから、１行(CRLFか、LF単独が現れるまで)受信
// CRLFは削除する。
// 受信したサイズをreturnする。
// CRLFだけの行は0を返す
// エラーは-1を返す
// **************************************************************************
int wString::LineRcv(SOCKET accept_socket)
{
	char 	byte_buf;
	clear();
	// １行受信実行
	while (1) {
		int recv_len = recv(accept_socket, &byte_buf, 1, 0);
		if (recv_len == 0) {
			if (len) {
				return len;
			}
			else {
				return -1;
			}
		}
		else if (recv_len < 0) { // 受信失敗チェック
#ifdef linux
			debug_log_output("header read error cnt = %d error=%s\n", recv_len, strerror(errno));
#else
			debug_log_output("header read error cnt = %d socket=%d error=%d\n", recv_len, accept_socket, WSAGetLastError());
#endif
			return (-1);
		}
		else {
			// CR/LFチェック
			if (byte_buf == '\r') {
				continue;
			}
			else if (byte_buf == '\n') {
				break;
			}
			// バッファにセット
			*this += (char)byte_buf;
		}
	}
	return len;
}
//***************************************************************************
// sentence文字列の、cut_charが最初に出てきた所から前を削除
// もし、cut_charがsentence文字列に入っていなかった場合、文字列全部削除
//***************************************************************************
void    wString::cut_before_character(const char cut_char)
{
	// 削除対象キャラクターが最初に出てくる所を探す。
	int ptr = find(cut_char);
	if (ptr == wString::npos) {
		// 発見できなかった場合、文字列全部削除。
		clear();
		return;
	}
	memmove(String, String + ptr + 1, len - ptr);
	return;
}
//***************************************************************************
// sentence文字列より、cut_charから後ろを削除
//      見つからなければ何もしない。
// 入力  : char* sentence 入力文字列
//         char  cut_char 切り取り文字
// 戻り値: char* 切り取った後ろの文字列
//***************************************************************************
void   wString::cut_after_character(const char cut_char)
{
	int ptr = find(cut_char);
	if (ptr == wString::npos) {
		return;
	}
	String[ptr] = 0;
	len = ptr;
	return;
}
//////////////////////////////////////////////////////////////////////////
/// <summary>
/// pos位置に指定文字を挿入(バイナリ単位）
/// </summary>
/// <param name="pos">挿入位置</param>
/// <param name="fill">挿入文字</param>
/// <returns>挿入された文字</returns>
wString wString::insert(int pos, const wString& fill)
{
	wString str1;
	wString str2;
	str1 = substr(0, pos);
	str2 = substr(pos);
	return str1 + fill + str2;
}

//////////////////////////////////////////////////////////////////////////
/// <summary>
/// PNGフォーマットファイルから、画像サイズを得る。
/// </summary>
/// <param name="png_filename"></param>
/// <returns>json:{"x":</returns>
wString wString::png_size(const wString& png_filename)
{
	int         fd;
	unsigned char       buf[255] = { 0 };
	ssize_t     read_len;
	auto x = 0;
	auto y = 0;
	fd = myopen(png_filename.c_str(), O_BINARY | O_RDONLY);
	if (fd < 0) {
		return "null";
	}
	// ヘッダ+サイズ(0x18byte)  読む
	//memset(buf, 0, sizeof(buf));
	read_len = read(fd, reinterpret_cast<char*>(buf), 0x18);
	if (read_len == 0x18) {
		x = (buf[0x10] << 24) +
			(buf[0x11] << 16) +
			(buf[0x12] << 8) +
			(buf[0x13]);
		y = (buf[0x14] << 24) +
			(buf[0x15] << 16) +
			(buf[0x16] << 8) +
			(buf[0x17]);
	}
	close(fd);
	wString tmp;
	tmp.sprintf("{\"x\":%d,\"y\":%d}", x, y);
	return tmp;
}
/// <summary>
///  GIFフォーマットファイルから、画像サイズを得る
/// </summary>
/// <param name="gif_filename"></param>
/// <returns></returns>
wString wString::gif_size(const wString& gif_filename)
{
	int         fd;
	unsigned char       buf[255] = { 0 };
	ssize_t     read_len;
	auto x = 0;
	auto y = 0;
	fd = myopen(gif_filename.c_str(), O_BINARY | O_RDONLY);
	if (fd < 0) {
		return "null";
	}
	// ヘッダ+サイズ(10byte)  読む
	//memset(buf, 0, sizeof(buf));
	read_len = read(fd, reinterpret_cast<char*>(buf), 10);
	if (read_len == 10) {
		x = buf[6] + (buf[7] << 8);
		y = buf[8] + (buf[9] << 8);
	}
	close(fd);
	wString tmp;
	tmp.sprintf("{\"x\":%d,\"y\":%d}", x, y);
	return tmp;
}
/// <summary>
///  JPEGフォーマットファイルから、画像サイズを得る。
/// </summary>
/// <param name="jpeg_filename"></param>
/// <returns></returns>
wString wString::jpeg_size(const wString& jpeg_filename)
{
	int                 fd;
	unsigned char       buf[255];
	ssize_t             read_len;
	off_t               length;
	auto x = 0;
	auto y = 0;
	//debug_log_output("jpeg_size: '%s'.", jpeg_filename);
	fd = myopen(jpeg_filename.c_str(), O_BINARY | O_RDONLY);
	if (fd < 0) {
		return "null";
	}
	while (1) {
		// マーカ(2byte)  読む
		read_len = read(fd, buf, 2);
		if (read_len != 2) {
			//debug_log_output("fraed() EOF.\n");
			break;
		}
		// Start of Image.
		if ((buf[0] == 0xFF) && (buf[1] == 0xD8)) {
			continue;
		}
		// Start of Frame 検知
		if ((buf[0] == 0xFF) && (buf[1] >= 0xC0) && (buf[1] <= 0xC3)) { // SOF 検知
			//debug_log_output("SOF0 Detect.");
			// sof データ読み込み
			memset(buf, 0, sizeof(buf));
			read_len = read(fd, buf, 0x11);
			if (read_len != 0x11) {
				debug_log_output("fraed() error.\n");
				break;
			}
			y = (buf[3] << 8) + buf[4];
			x = (buf[5] << 8) + buf[6];
			break;
		}
		// SOS検知
		if ((buf[0] == 0xFF) && (buf[1] == 0xDA)) { // SOS 検知
			//debug_log_output("Start Of Scan.\n");
			// 0xFFD9 探す。
			while (1) {
				// 1byte 読む
				read_len = read(fd, buf, 1);
				if (read_len != 1) {
					//debug_log_output("fraed() error.\n");
					break;
				}
				// 0xFFだったら、もう1byte読む
				if (buf[0] == 0xFF) {
					buf[0] = 0;
					if (read(fd, buf, 1) != 1) {
						break;
					}
					// 0xD9だったら 終了
					if (buf[0] == 0xD9) {
						//debug_log_output("End Of Scan.\n");
						break;
					}
				}
			}
			continue;
		}
		// length 読む
		memset(buf, 0, sizeof(buf));
		auto dummy = read(fd, buf, 2);
		length = (buf[0] << 8) + buf[1];
		// length分とばす
		lseek(fd, length - 2, SEEK_CUR);
	}
	close(fd);
	wString tmp;
	tmp.sprintf("{\"x\":%d,\"y\":%d}", x, y);
	return tmp;
}






