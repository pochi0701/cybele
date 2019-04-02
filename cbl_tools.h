// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_tools.h
//		$Revision: 1.7 $
//		$Date: 2004/03/10 04:47:55 $
//
//	���ׂĎ��ȐӔC�ł��Ȃ������܂B
// ==========================================================================
#ifndef	_CBL_TOOLS_H
#define	_CBL_TOOLS_H
#ifdef linux
#define O_BINARY                0
#define INVALID_SOCKET          (-1)
#define SOCKET_ERROR            (-1)
typedef int SOCKET;
#else
#include <winsock.h>
#include <time.h>
#include <sys/types.h>
#endif
#define		CR		(0x0D)
#define		LF		(0x0A)
#define		OUTPUT_LOG_ROTATE_SIZE		(1024*1024*1900)	/* 1.9Gbyte */
#define		OUTPUT_LOG_ROTATE_MAX		5					/* �����rotate�������� */
#include        "cbl_String.h"

// ==========================================================================
// �ėp�I�[�v��(OS/�����R�[�h���ًz��)
// ==========================================================================
int myopen(const char* filename,int amode,int option=0);
// uri�G���R�[�h�^�f�R�[�h
extern int      uri_encode(char *dst, unsigned int dst_len, const char *src, unsigned int src_len);
extern int      uri_decode(char *dst, unsigned int dst_len, const char *src, unsigned int src_len);

// �e�L�X�g�����C���C���B
extern char*    cut_after_character(char *sentence, char cut_char);
//extern void     cut_before_character(char *sentence, char cut_char);
//extern void     cut_character(char *sentence, char cut_char);
extern void     ltrim(char *sentence, char cut_char=' ');
//extern void     cut_before_last_character(char *sentence, char cut_char);
extern void     cut_after_last_character(char *sentence, char cut_char);
extern int      sentence_split(char *sentence, char cut_char, char *split1, char *split2);
extern void     duplex_character_to_unique(char *sentence, char unique_char);
extern void     replace_character(char *sentence, const char *key, const char *rep);
extern void     replace_character_first(char *sentence, const char *key, const char *rep);
extern void 	make_datetime_string(char *sentence);
extern void 	cut_character_at_linetail(char *sentence, char cut_char);
extern void     filename_to_extension(char *filename, char *extension_buf, unsigned int extension_buf_size);
extern char*    buffer_distill_line(char *text_buf_p, char *line_buf_p, unsigned int line_buf_size );
//extern void     extension_add_rename(char *rename_filename_p, size_t rename_filename_size);
//extern void     extension_del_rename(char *rename_filename_p);
extern wString  insertStr(int pos,const wString& fill);
extern int      nkf(const char *in,char *out,size_t len,const char *options);

extern int      HTTPDownload(char* src, char* dst, off_t offset);
//extern char* HTTPGet(char* src, off_t offset);
// path ���� ./ �� ../ ����菜���Bdir �Ɍ��ʂ��i�[�����B
extern char*    path_sanitize(char *dir, size_t dir_size);
// DebugLog �o��
extern void     debug_log_initialize(const char *set_debug_log_filename);
extern void     debug_log_output(const char *fmt, ...);
// �摜�n
extern void     jpeg_size(char *jpeg_filename, unsigned int *x, unsigned int *y);
extern void     gif_size(char *gif_filename,   unsigned int *x, unsigned int *y);
extern void     png_size(char *png_filename,   unsigned int *x, unsigned int *y);
extern void     filename_to_extension(char *filename, char *extension_buf, unsigned int extension_buf_size);

//�V�X�e������
extern bool   FileExists(char* str);
extern size_t  FileSizeByName(char* str);
//extern SOCKET sock_connect(char *host, int port);
//extern void set_nonblocking_mode(int fd, int flag);
extern int sClose(SOCKET socket);
extern int readLine(int fd, char *line_buf_p, int line_max);
extern wString GetAuthorization(wString& AuthorizedString);

#ifdef linux
//extern int      send(int fd,char* buffer, unsigned int length, int mode);
//extern int      recv(int fd,char* buffer, unsigned int length, int mode);
extern int      getTargetFile( const char *LinkFile, char *TargetFile );
extern void     Sleep(unsigned int milliseconds);
//extern void     ExitThread(DWORD dwExitCode);

#endif
char*           mymalloc(size_t size);
char*           mycalloc(size_t size1, int num);
void            myfree(char* ptr);
int             split(const char* cut_char, wString &split1, wString &split2);
class mp3{
public:
    static unsigned char   mp3_id3v1_flag;         // MP3 �^�O ���݃t���O
    static unsigned char   mp3_id3v1_title[128];   // MP3 �Ȗ�
    static unsigned char   mp3_id3v1_album[128];   // MP3 �A���o����
    static unsigned char   mp3_id3v1_artist[128];  // MP3 �A�[�e�B�X�g
    static unsigned char   mp3_id3v1_year[128];    // MP3 ����N�x
    static unsigned char   mp3_id3v1_comment[128]; // MP3 �R�����g
    //int init;
    static wString mp3_id3_tag       (const char* filename);
    static wString mp3_id3_tag       (const wString& filename);
    static int     mp3_id3_tag_read  (const char *mp3_filename );
    static int     mp3_id3v1_tag_read(const char *mp3_filename );
    static int     mp3_id3v2_tag_read(const char *mp3_filename );
    static size_t  id3v2_len(unsigned char *buf);
};
extern struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf);
extern struct tm *localtime_r(const time_t *timer, struct tm *tmbuf);
extern wString asctimew(const struct tm *tm);
extern wString ctimew(const time_t *timer);
extern char *_strdate(char *s);
extern char *_strtime(char *s);
#endif
