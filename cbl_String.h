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
    unsigned int len;	//ŽÀÛ‚Ì’·‚³
    unsigned int total;	//•Û‘¶—Ìˆæ‚Ì’·‚³
    char* String;

    void replace_character_len(const char *sentence,int slen,const char* p,int klen,const char *rep);
    void replace_str(const char *sentence,int slen,const char* p,int klen,const char *rep);
    #ifndef va_copy
    int tsprintf_string(char* str);
    //int tsprintf_string(wString& str);
    int tsprintf_char(int ch);
    int tsprintf_decimal(signed long val,int zerof,int width);
    int tsprintf_decimalu(unsigned long val,int zerof,int width);
    int tsprintf_octadecimal(unsigned long val,int zerof,int width);
    int tsprintf_hexadecimal(unsigned long val,int capital,int zerof,int width);
    int vtsprintf(const char* fmt,va_list arg);
    #endif
public:
    const static int npos;
    wString(void);
    wString(int mylen);
    wString(const char* str);
    wString(const wString& str);
    ~wString(void);
    void   resize(const int newSize);
    //OPERATION OVERLOAD
    wString  operator+(const wString& str) const;
    wString  operator+(const char* str) const;
    friend wString operator+(const char* str1, const wString str2 );
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
    char            at(unsigned int index) const;
    wString        SubString(int start, int mylen)   const;
    wString        substr(int start, int mylen=-1) const;
    int             compare(const wString& str) const;
    int             compare(const char* str ) const;
    void            clear(void);
    int             Pos(const wString& pattern,int pos=0) const;
    int             Pos(const char* pattern,int pos=0) const;
    size_t          copy( char *str, size_t slen, size_t index ) const;
    wString&        replace( size_t index, size_t len, const wString& repstr);

    size_t          size(void) const;
    size_t          length(void) const;
    int             Length(void) const;
    int             Total(void) const;
    char*           c_str(void) const;
    wString&        SetLength(const unsigned int num);

    wString         Trim(void);
    wString         RTrim(void);
    wString         LTrim(void);
    //void            LTrim(void);
    static void     Rtrimch(char *sentence, const byte cut_char);
    int             sprintf(const char* format, ... );
    int             cat_sprintf(const char* format, ... );
    int             LastDelimiter(const char* delim) const;
    wString         strsplit(const char* delimstr);
    int             find( const wString& str, size_t index=0) const;
    int             find( const char* str, size_t index=0 ) const;
    int             find( char ch, size_t index = 0) const;
    int             rfind( const wString& str, size_t index=0) const;
    int             rfind( const char* str, size_t index=0 ) const;
    int             rfind( char ch, size_t index = 0) const;
    //URL and encode
    static char*    LinuxFileName(char* FileName);
    static char*    WindowsFileName(char* FileName);
    wString         uri_encode(void);
    wString         uri_decode(void);
    wString         htmlspecialchars(void);
    wString         addSlashes(void);
    wString         base64(void);
    wString         unbase64(void);

    //ANISSTRING
    bool            SetListString(wString& src,int pos);
    bool            SetListString(const char* src,int pos);
    wString         GetListString(const int pos);
    int             Counts(void);
    void            ResetLength(unsigned int num);
    void            Add(const wString& str);
    void            Add(const char* str);
    //HEADER
    void            headerInit(size_t content_length, int expire=1, const char* mime_type="text/html");
    void            headerPrint(int socket,int endflag);
    wString         headerPrintMem(void);
    int             header(const char* str,int flag=true, int status=0);
    //FILE OPERATION
    static bool     RenameFile(const wString& src, const wString& dst);
    static int      FileCopy(const char* fname_r, const char* fname_w);
    static int      DeleteFile(const wString& str);
    static int      DirectoryExists(const char* str);
    static int      DirectoryExists(const wString& str);
    static wString  FileStats(const char* str, int mode = 0 );
    static wString  FileStats(const wString& str, int mode = 0 );
    static int      FileExists(const char* str);
    static int      FileExists(const wString& str);
    static wString  ExtractFileDir(wString& str);
    static wString  ExtractFileName(const char* str, const char* delim = DELIMITER);
    static wString  ExtractFileName(const wString& str, const char* delim = DELIMITER);
    static wString  ExtractFileExt(const wString& str );
    static int      CreateDir(const wString& str);
    static wString  ChangeFileExt(wString& str, const char* ext);
    static unsigned long FileSizeByName(char* str);
    static unsigned long FileSizeByName(wString& str);
    static wString  EnumFolder(const wString& Path);
    static wString  EnumFolderjson(const wString& Path);
    static bool     checkUrl( const wString& url );
    int             LoadFromFile(const char* FileName);
    void            LoadFromFile(const wString& str);
    void            LoadFromCSV(const char* FileName);
    void            LoadFromCSV(const wString& str);
    int             SaveToFile(const char* FileName);
    int             SaveToFile(const wString& str);
    wString         nkfcnv(const wString& option);
    int             LineRcv(int accept_socket);
    void            cut_before_character(const char cut_char);
    void            cut_after_character(const char cut_char);
    wString         insert(int pos,const wString& fill);
    //HTTPÚ‘±—p
    static int      HTTPSize(const wString& url);
    static SOCKET   sock_connect(const wString& host, const int port);
    static SOCKET   sock_connect(const char *host, const int port);
    static wString  HTTPGet(const wString& url, const off_t offset=0);
    static wString  HTTPRest(const wString& methods,const wString& url, const wString& data);
    static wString  GetLocalAddress(void);

};
extern wString current_dir;
#endif
