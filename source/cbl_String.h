#ifndef CBLSTRINGH
#define CBLSTRINGH
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
//#include <dirent.h>
#include <assert.h>
#include "const.h"
//#include "cbl.h"


class wString
{
private:
	unsigned int len;	//実際の長さ
	unsigned int total;	//保存領域の長さ
	char* String;

	void replace_character_len(const char* sentence, int slen, const char* p, int klen, const char* rep);
	void replace_str(const char* sentence, int slen, const char* p, int klen, const char* rep);
#ifndef va_copy
	int tsprintf_string(char* str);
	//int tsprintf_string(wString& str);
	int tsprintf_char(int ch);
	int tsprintf_decimal(signed long val, int zerof, int width);
	int tsprintf_decimalu(unsigned long val, int zerof, int width);
	int tsprintf_octadecimal(unsigned long val, int zerof, int width);
	int tsprintf_hexadecimal(unsigned long val, int capital, int zerof, int width);
	int vtsprintf(const char* fmt, va_list arg);
#endif
public:
	const static int npos;
	wString(void);
	explicit wString(int mylen);
	wString(const char* str);
	wString(const wString& str);
	~wString(void);
	void   resize(const int newSize);
	//OPERATION OVERLOAD
	wString  operator+(const wString& str) const;
	wString  operator+(const char* str) const;
	friend wString operator+(const char* str1, const wString& str2);
	void     operator=(const wString& str);
	void     operator=(const char* str);
	void     operator=(const int num);
	void     operator=(const double num);
	bool     operator==(const wString& str) const;
	bool     operator==(const char* str) const;
	bool     operator!=(const wString& str) const;
	bool     operator!=(const char* str) const;
	bool     operator>=(const wString& str) const;
	bool     operator>=(const char* str) const;
	bool     operator<=(const wString& str) const;
	bool     operator<=(const char* str) const;
	bool     operator> (const wString& str) const;
	bool     operator> (const char* str) const;
	bool     operator< (const wString& str) const;
	bool     operator< (const char* str) const;
	void     operator+=(const char ch);
	void     operator+=(const wString& str);
	void     operator+=(const char* str);
	char     operator[](unsigned int index) const;
	//STRING FUNCTION
	bool            startsWith(const char* str);
	bool            endsWith(const char* str);
	void            setBinary(const void* str, int addLen);
	char            at(unsigned int index) const;
	wString         SubString(int start, int mylen)   const;
	wString         substr(int start, int mylen = -1) const;
	int             compare(const wString& str) const;
	int             compare(const char* str) const;
	void            clear(void);
	int             Pos(const wString& pattern, int pos = 0) const;
	int             Pos(const char* pattern, int pos = 0) const;
	unsigned int    copy(char* str, unsigned int slen, int index) const;
	wString&        replace(int index, unsigned int len, const wString& repstr);
	wString         dump(void) const;

	unsigned int    size(void) const;
	unsigned int    length(void) const;
	unsigned int    Length(void) const;
	unsigned int    Total(void) const;
	char* c_str(void) const;
	wString& SetLength(const unsigned int num);

	wString         ToUpper(void);
	wString         ToLower(void);
	wString         Trim(void);
	wString         RTrim(void);
	wString         LTrim(void);
	static void     Rtrimch(char* sentence, const unsigned char cut_char);
	int             sprintf(const char* format, ...);
	int             cat_sprintf(const char* format, ...);
	int             LastDelimiter(const char* delim) const;
///	wString         strsplit(const wString& str, const char* delimstr);
	wString         strsplit(const char* delimstr);
	int             find(const wString& str, int index = 0) const;
	int             find(const char* str, int index = 0) const;
	int             find(char ch, int index = 0) const;
	int             rfind(const wString& str, int index = 0) const;
	int             rfind(const char* str, int index = 0) const;
	int             rfind(char ch, int index = 0) const;
	//URL and encode
	static char* LinuxFileName(char* FileName);
	static char* WindowsFileName(char* FileName);
	wString         uri_encode(void);
	wString         uri_decode(void);
	wString         htmlspecialchars(void);
	wString         addSlashes(void);
	wString         base64(void);
	wString         unbase64(void);
	static wString  escape(const wString& str);
	//ANISSTRING
	bool            SetListString(wString& src, int pos);
	bool            SetListString(const char* src, int pos);
	wString         GetListString(const int pos);
	int             Counts(void);
	void            ResetLength(unsigned int num);
	void            Add(const wString& str);
	void            Add(const char* str);
	//HEADER
	void            headerInit(size_t content_length, int expire = 1, const char* mime_type = "text/html");
	void            headerPrint(SOCKET socket, int endflag);
	wString         headerPrintMem(void);
	int             header(const char* str, int flag = true, int status = 0);
	//FILE OPERATION
	static bool     RenameFile(const wString& src, const wString& dst);
	static int      FileCopy(const char* fname_r, const char* fname_w);
	static int      DeleteFile(const wString& str);
	static int      DirectoryExists(const char* str);
	static int      DirectoryExists(const wString& str);
	static wString  FileStats(const char* str, int mode = 0);
	static wString  FileStats(const wString& str, int mode = 0);
	static int      FileExists(const char* str);
	static int      FileExists(const wString& str);
	int             FileExists(void);
	static wString  ExtractFileDir(wString& str);
	static wString  ExtractFileName(const char* str, const char* delim = DELIMITER);
	static wString  ExtractFileName(const wString& str, const char* delim = DELIMITER);
	static wString  ExtractFileExt(const wString& str);
	static wString  GetMimeType(const wString& file_name);
	static int      CreateDir(const wString& str);
	static wString  ChangeFileExt(const wString& str, const char* ext);
	static unsigned long FileSizeByName(char* str);
	static unsigned long FileSizeByName(wString& str);
	static wString  EnumFolder(const wString& Path);
	static wString  EnumFolderjson(const wString& Path);
	static bool     checkUrl(const wString& url);
	static wString  png_size(const wString& png_filename);
	static wString  gif_size(const wString& gif_filename);
	static wString  jpeg_size(const wString& jpeg_filename);
	int             LoadFromFile(const char* FileName);
	int             LoadFromFile(const wString& str);
	void            LoadFromCSV(const char* FileName);
	void            LoadFromCSV(const wString& str);
	int             SaveToFile(const char* FileName);
	int             SaveToFile(const wString& str);
	wString         nkfcnv(const wString& option);
	int             LineRcv(SOCKET accept_socket);
	void            cut_before_character(const char cut_char);
	void            cut_after_character(const char cut_char);
	wString         insert(int pos, const wString& fill);
	//HTTP接続用
	static int      HTTPSize(const wString& url);
	static SOCKET   sock_connect(const wString& host, const int port);
	static SOCKET   sock_connect(const char* host, const int port);
	static wString  HTTPGet(const wString& url, const off_t offset = 0);
	static wString  HTTPRest(const wString& methods, const wString& url, const wString& data);
	static wString  GetLocalAddress(void);

};
extern wString current_dir;
#endif
