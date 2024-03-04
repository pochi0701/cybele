#ifndef _LIBNKFHPP
#define _LIBNKFHPP
#include "stdafx.h"
/** Network Kanji Filter. (PDS Version)
************************************************************************
** Copyright (C) 1987, Fujitsu LTD. (Itaru ICHIKAWA)
** 連絡先： （株）富士通研究所　ソフト３研　市川　至
** （E-Mail Address: ichikawa@flab.fujitsu.co.jp）
** Copyright (C) 1996,1998
** Copyright (C) 2002
** 連絡先： 琉球大学情報工学科 河野 真治  mime/X0208 support
** （E-Mail Address: kono@ie.u-ryukyu.ac.jp）
** 連絡先： COW for DOS & Win16 & Win32 & OS/2
** （E-Mail Address: GHG00637@niftyserve.or.p）
**
**    このソースのいかなる複写，改変，修正も許諾します。ただし、
**    その際には、誰が貢献したを示すこの部分を残すこと。
**    再配布や雑誌の付録などの問い合わせも必要ありません。
**    営利利用も上記に反しない範囲で許可します。
**    バイナリの配布の際にはversion messageを保存することを条件とします。
**    このプログラムについては特に何の保証もしない、悪しからず。
**
**    Everyone is permitted to do anything on this program
**    including copying, modifying, improving,
**    as long as you don't try to pretend that you wrote it.
**    i.e., the above copyright notice has to appear in all copies.
**    Binary distribution requires original version messages.
**    You don't have to ask before copying, redistribution or publishing.
**    THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE.
***********************************************************************/
/***********************************************************************
** UTF-8 サポートについて
**    従来の nkf と入れかえてそのまま使えるようになっています
**    nkf -e などとして起動すると、自動判別で UTF-8 と判定されれば、
**    そのまま euc-jp に変換されます
**
**    まだバグがある可能性が高いです。
**    (特に自動判別、コード混在、エラー処理系)
**
**    何か問題を見つけたら、
**        E-Mail: furukawa@tcp-ip.or.jp
**    まで御連絡をお願いします。
***********************************************************************/
/***********************************************************************
** 共有ライブラリ化
** 連絡先： 東京工業大学大学院 森本 容介
** （E-Mail Address: morimoto@mr.hum.titech.ac.jp）
** c++化 by pochi pochi0701@gmail.com
***********************************************************************/
#include "config.h"
#include "define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE  (1)
#endif
/* state of output_mode and input_mode */
#define ASCII          (0)
#define X0208          (1)
#define X0201          (2)
#define ISO8859_1      (8)
#define NO_X0201       (3)
/* Input Assumption */
#define JIS_INPUT      (4)
#define SJIS_INPUT     (5)
#define LATIN1_INPUT   (6)
#define FIXED_MIME     (7)
#define STRICT_MIME    (8)
/* MIME ENCODE */
#define ISO2022JP      (9)
#define JAPANESE_EUC  (10)
#define SHIFT_JIS     (11)
#define UTF8          (12)
#define UTF8_INPUT    (13)
#define UTF16_INPUT   (14)
#define UTF16BE_INPUT (15)
/* ASCII CODE */
#define BS    (0x08)
#define NL    (0x0a)
#define CR    (0x0d)
#define ESC   (0x1b)
#define SPACE (0x20)
#define AT    (0x40)
#define SSP   (0xa0)
#define DEL   (0x7f)
#define SI    (0x0f)
#define SO    (0x0e)
#define SSO   (0x8e)
#define HOLD_SIZE  (1024)
#define IOBUF_SIZE (16384)
#define DEFAULT_J 'B'
#define DEFAULT_R 'B'
#define SJ0162 (0x00e1) /* 01 - 62 ku offset */
#define SJ6394 (0x0161) /* 63 - 94 ku offset */
#define RANGE_NUM_MAX (18)
#define GETA1 (0x22)
#define GETA2 (0x2e)
#define sizeof_euc_utf8           (94)
#define sizeof_euc_to_utf8_1byte  (94)
#define sizeof_euc_to_utf8_2bytes (94)
#define sizeof_utf8_to_euc_C2     (64)
#define sizeof_utf8_to_euc_E5B8   (64)
#define sizeof_utf8_to_euc_2bytes (112)
#define sizeof_utf8_to_euc_3bytes (112)
inline bool is_alnum (int c)
{
	return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9'));
}
inline int itoh4 (int c)
{
	return (c >= 10 ? c + 'A' - 10 : c + '0');
}
class nkf_cnv;
struct input_code
{
	const char* name;
	int stat;
	int score;
	int index;
	int buf[3];
	void (nkf_cnv::* status_func)(struct input_code*, int);
	int (nkf_cnv::* iconv_func)(int c2, int c1, int c0);
};
class nkf_cnv
{
public:
	nkf_cnv () {}
	/* buffers */
	unsigned char hold_buf[HOLD_SIZE * 2] = {};
	int           hold_count;
	/* MIME preprocessor fifo */
#define MIME_BUF_SIZE (1024) /* 2^n ring buffer */
#define MIME_BUF_MASK (MIME_BUF_SIZE - 1)
#define Fifo(n) mime_buf[(n) & MIME_BUF_MASK]
	unsigned char mime_buf[MIME_BUF_SIZE];
	unsigned int  mime_top;
	unsigned int  mime_last;  /* decoded */
	unsigned int  mime_input; /* undecoded */
	/* flags */
	int estab_f;
	int rot_f;             /* rot14/43 mode */
	int input_f;           /* non fixed input code */
	int mime_f;            /* convert MIME B base64 or Q */
	int mimebuf_f;         /* MIME buffered input */
	int iso8859_f;         /* ISO8859 through */
	int mimeout_f;         /* base64 mode */
	int x0201_f;           /* Assume NO JISX0201 */
	int w_oconv16_begin_f; /* utf-16 header */

	int  utf16_mode = UTF16_INPUT;
	struct input_code input_code_list[4] = {
	{"EUC-JP",    0, 0, 0, {0, 0, 0}, &nkf_cnv::e_status, &nkf_cnv::e_iconv},
	{"Shift_JIS", 0, 0, 0, {0, 0, 0}, &nkf_cnv::s_status, &nkf_cnv::s_iconv},
	{"UTF-8",     0, 0, 0, {0, 0, 0}, &nkf_cnv::w_status, &nkf_cnv::w_iconv},
	{0}
	};
	int mimeout_mode;
	int base64_count;
	/* options */
	unsigned char kanji_intro, ascii_intro;
	/* converters */
#ifdef DEFAULT_CODE_JIS
#define DEFAULT_CONV &nkf_cnv::j_oconv
#endif
#ifdef DEFAULT_CODE_SJIS
#define DEFAULT_CONV &nkf_cnv::s_oconv
#endif
#ifdef DEFAULT_CODE_EUC
#define DEFAULT_CONV &nkf_cnv::e_oconv
#endif
#ifdef DEFAULT_CODE_UTF8
#define DEFAULT_CONV &nkf_cnv::w_oconv
#endif
/* process default */
	void (nkf_cnv::* output_conv)(int c2, int c1);
	void (nkf_cnv::* oconv)(int c2, int c1);
	/* s_iconv or oconv */
	int (nkf_cnv::* iconv)(int c2, int c1, int c0);
	void (nkf_cnv::* o_zconv)(int c2, int c1);
	void (nkf_cnv::* o_crconv)(int c2, int c1);
	void (nkf_cnv::* o_rot_conv)(int c2, int c1);
	void (nkf_cnv::* o_base64conv)(int c2, int c1);
	/* redirections */
	void (nkf_cnv::* o_putc)(int c);
	int  (nkf_cnv::* i_getc)(char* f); /* general input */
	int  (nkf_cnv::* i_ungetc)(int c, char* f);
	void (nkf_cnv::* o_mputc)(int c); /* output of mputc */
	int  (nkf_cnv::* i_mgetc)(char*); /* input of mgetc */
	int  (nkf_cnv::* i_mungetc)(int c, char* f);
	/* for strict mime */
	int  (nkf_cnv::* i_mgetc_buf)(char*); /* input of mgetc_buf */
	int  (nkf_cnv::* i_mungetc_buf)(int c, char* f);
	/* Global states */
	int output_mode,      /* output kanji mode */
		input_mode,       /* input kanji mode */
		shift_mode;       /* TRUE shift out, or X0201  */
	int mime_decode_mode; /* MIME mode B base64, Q hex */
	/* X0201 / X0208 conversion tables */
	/* X0201 kana conversion table */
	/* 90-9F A0-DF */
	const unsigned char cv[130] = {
	 0x21, 0x21, 0x21, 0x23, 0x21, 0x56, 0x21, 0x57,
	 0x21, 0x22, 0x21, 0x26, 0x25, 0x72, 0x25, 0x21,
	 0x25, 0x23, 0x25, 0x25, 0x25, 0x27, 0x25, 0x29,
	 0x25, 0x63, 0x25, 0x65, 0x25, 0x67, 0x25, 0x43,
	 0x21, 0x3c, 0x25, 0x22, 0x25, 0x24, 0x25, 0x26,
	 0x25, 0x28, 0x25, 0x2a, 0x25, 0x2b, 0x25, 0x2d,
	 0x25, 0x2f, 0x25, 0x31, 0x25, 0x33, 0x25, 0x35,
	 0x25, 0x37, 0x25, 0x39, 0x25, 0x3b, 0x25, 0x3d,
	 0x25, 0x3f, 0x25, 0x41, 0x25, 0x44, 0x25, 0x46,
	 0x25, 0x48, 0x25, 0x4a, 0x25, 0x4b, 0x25, 0x4c,
	 0x25, 0x4d, 0x25, 0x4e, 0x25, 0x4f, 0x25, 0x52,
	 0x25, 0x55, 0x25, 0x58, 0x25, 0x5b, 0x25, 0x5e,
	 0x25, 0x5f, 0x25, 0x60, 0x25, 0x61, 0x25, 0x62,
	 0x25, 0x64, 0x25, 0x66, 0x25, 0x68, 0x25, 0x69,
	 0x25, 0x6a, 0x25, 0x6b, 0x25, 0x6c, 0x25, 0x6d,
	 0x25, 0x6f, 0x25, 0x73, 0x21, 0x2b, 0x21, 0x2c,
	 0x00, 0x00 };
	/* X0201 kana conversion table for daguten */
	/* 90-9F A0-DF */
	const unsigned char dv[130] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x74,
	 0x00, 0x00, 0x00, 0x00, 0x25, 0x2c, 0x25, 0x2e,
	 0x25, 0x30, 0x25, 0x32, 0x25, 0x34, 0x25, 0x36,
	 0x25, 0x38, 0x25, 0x3a, 0x25, 0x3c, 0x25, 0x3e,
	 0x25, 0x40, 0x25, 0x42, 0x25, 0x45, 0x25, 0x47,
	 0x25, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x25, 0x50, 0x25, 0x53,
	 0x25, 0x56, 0x25, 0x59, 0x25, 0x5c, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00 };
	/* X0201 kana conversion table for han-daguten */
	/* 90-9F A0-DF */
	const unsigned char ev[130] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x25, 0x51, 0x25, 0x54,
	 0x25, 0x57, 0x25, 0x5a, 0x25, 0x5d, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00 };
#define CRLF (1)
	int crmode_f; /* CR, NL, CRLF */
	int prev_cr;
	int z_prev2, z_prev1;
	size_t len_in;
	size_t len_out;
	size_t glen;
	char* gout;
	int offlag;
	int ncflag;
	int nkf (const char* in, char* out, size_t len, const char* options)
	{
		mime_top = 0;
		mime_last = 0;
		mime_input = 0;
		estab_f = FALSE;
		rot_f = FALSE;
		input_f = FALSE;
		mime_f = STRICT_MIME;
		mimebuf_f = FALSE;
		iso8859_f = FALSE;
		mimeout_f = FALSE;
		x0201_f = NO_X0201;
		w_oconv16_begin_f = 0;
		utf16_mode = UTF16_INPUT;
		mimeout_mode = 0;
		base64_count = 0;
		kanji_intro = DEFAULT_J;
		ascii_intro = DEFAULT_R;
		output_conv = DEFAULT_CONV;
		oconv = &nkf_cnv::no_connection;
		iconv = &nkf_cnv::no_connection2;
		o_zconv = &nkf_cnv::no_connection;
		o_crconv = &nkf_cnv::no_connection;
		o_rot_conv = &nkf_cnv::no_connection;
		o_base64conv = &nkf_cnv::no_connection;
		o_putc = &nkf_cnv::std_putc;
		i_getc = &nkf_cnv::std_getc;
		i_ungetc = &nkf_cnv::std_ungetc;
		o_mputc = &nkf_cnv::std_putc;
		i_mgetc = &nkf_cnv::std_getc;
		i_mungetc = &nkf_cnv::std_ungetc;
		i_mgetc_buf = &nkf_cnv::std_getc;
		i_mungetc_buf = &nkf_cnv::std_ungetc;
		output_mode = ASCII;
		input_mode = ASCII;
		shift_mode = FALSE;
		mime_decode_mode = FALSE;
		crmode_f = 0;
		prev_cr = 0;
		z_prev2 = 0;
		z_prev1 = 0;
		len_in = 0;
		len_out = 0;
		glen = len;
		gout = out;
		offlag = 0;
		ncflag = 0;
		while (*options) {
			switch (*options++) {
			case 'j':           /* JIS output */
				output_conv = &nkf_cnv::j_oconv;
				continue;
			case 'e':           /* AT&T EUC output */
				output_conv = &nkf_cnv::e_oconv;
				continue;
			case 's':           /* SJIS output */
				output_conv = &nkf_cnv::s_oconv;
				continue;
			case 'l':           /* ISO8859 Latin-1 support, no conversion */
				iso8859_f = TRUE;  /* Only compatible with ISO-2022-JP */
				input_f = LATIN1_INPUT;
				continue;
			case 'i':           /* Kanji IN ESC-$-@/B */
				if (*options == '@' || *options == 'B')
					kanji_intro = *options++;
				continue;
			case 'o':           /* ASCII IN ESC-(-J/B */
				if (*options == 'J' || *options == 'B' || *options == 'H')
					ascii_intro = *options++;
				continue;
			case 'r':
				rot_f = TRUE;
				continue;
			case 'w':           /* UTF-8 output */
				if ('1' == options[0] && '6' == options[1]) {
					output_conv = &nkf_cnv::w_oconv16; options += 2;
					if (options[0] == 'L') {
						w_oconv16_begin_f = 2; options++;
					}
				}
				else
					output_conv = &nkf_cnv::w_oconv;
				continue;
			case 'W':            /* UTF-8 input */
				if ('1' == options[0] && '6' == options[1]) {
					input_f = UTF16_INPUT;
				}
				else
					input_f = UTF8_INPUT;
				continue;
				/* Input code assumption */
			case 'J':   /* JIS input */
			case 'E':   /* AT&T EUC input */
				input_f = JIS_INPUT;
				continue;
			case 'S':   /* MS Kanji input */
				input_f = SJIS_INPUT;
				if (x0201_f == NO_X0201) x0201_f = TRUE;
				continue;
			case 'm':   /* MIME support */
				if (*options == 'B' || *options == 'Q') {
					mime_decode_mode = *options++;
					mimebuf_f = FIXED_MIME;
				}
				else if (*options == 'N') {
					mime_f = TRUE; options++;
				}
				else if (*options == 'S') {
					mime_f = STRICT_MIME; options++;
				}
				else if (*options == '0') {
					mime_f = FALSE; options++;
				}
				continue;
			case 'M':   /* MIME output */
				if (*options == 'B') {
					mimeout_mode = 'B';
					mimeout_f = FIXED_MIME; options++;
				}
				else if (*options == 'Q') {
					mimeout_mode = 'Q';
					mimeout_f = FIXED_MIME; options++;
				}
				else {
					mimeout_f = TRUE;
				}
				continue;
			case 'L':  /* line mode */
				if (*options == 'u') {         /* unix */
					crmode_f = NL; options++;
				}
				else if (*options == 'm') {  /* mac */
					crmode_f = CR; options++;
				}
				else if (*options == 'w') {  /* windows */
					crmode_f = CRLF; options++;
				}
				continue;
			default:
				/* bogus option but ignored */
				continue;
			}
		}
		kanji_convert (const_cast<char*>(in));
		*(out + len_out) = (char)'\0';
		if (offlag != 0)
			return (-1);
		if (ncflag != 0)
			return (-2);
		return (0);
	}
	void set_iconv (int f, int (nkf_cnv::* iconv_func)(int c2, int c1, int c0))
	{
#ifdef INPUT_CODE_FIX
		if (f || !input_f)
#endif
			if (estab_f != f) {
				estab_f = f;
			}
		if (iconv_func
#ifdef INPUT_CODE_FIX
			&& (f == -TRUE || !input_f) /* -TRUE means "FORCE" */
#endif
			) {
			iconv = iconv_func;
		}
	}
#define SCORE_KANA     (1)                   /* いわゆる半角カナ */
#define SCORE_DEPEND   (SCORE_KANA << 1)     /* 機種依存文字 */
#define SCORE_NO_EXIST (SCORE_DEPEND << 1)   /* 存在しない文字 */
#define SCORE_ERROR    (SCORE_NO_EXIST << 1) /* エラー */
	int score_table_A0[16] = {
	 0, 0, 0, 0,
	 0, 0, 0, 0,
	 0, SCORE_DEPEND, SCORE_DEPEND, SCORE_DEPEND,
	 SCORE_DEPEND, SCORE_DEPEND, SCORE_DEPEND, SCORE_NO_EXIST,
	};
	int score_table_F0[16] = {
	 0, 0, 0, 0,
	 0, SCORE_DEPEND, SCORE_NO_EXIST, SCORE_NO_EXIST,
	 SCORE_DEPEND, SCORE_DEPEND, SCORE_DEPEND, SCORE_DEPEND,
	 SCORE_DEPEND, SCORE_NO_EXIST, SCORE_NO_EXIST, SCORE_ERROR,
	};
	void code_score (struct input_code* ptr)
	{
		int s = ptr->score;
		int c2 = ptr->buf[0];
		int c1 = ptr->buf[1];
		if (c2 < 0) {
			s |= SCORE_ERROR;
		}
		else if ((c2 & 0xf0) == 0xa0) {
			s |= score_table_A0[c2 & 0x0f];
		}
		else if ((c2 & 0xf0) == 0xf0) {
			s |= score_table_F0[c2 & 0x0f];
		}
		else if (c2 == SSO) {
			s |= SCORE_KANA;
		}
		else if (!e2w_conv (c2, c1)) {
			s |= SCORE_NO_EXIST;
		}
		ptr->score = s;
	}
	void status_disable (struct input_code* ptr)
	{
		ptr->stat = -1;
		ptr->buf[0] = -1;
		code_score (ptr);
		if (iconv == ptr->iconv_func) set_iconv (FALSE, 0);
	}
	void status_push_ch (struct input_code* ptr, int c)
	{
		ptr->buf[ptr->index++] = c;
	}
	void status_reset (struct input_code* ptr)
	{
		ptr->stat = 0;
		ptr->score = 0;
		ptr->index = 0;
	}
	void status_check (struct input_code* ptr, int c)
	{
		if (c <= DEL && estab_f) {
			status_reset (ptr);
		}
	}
	void s_status (struct input_code* ptr, int c)
	{
		switch (ptr->stat) {
		case -1:
			status_check (ptr, c);
			break;
		case 0:
			if (c <= DEL) {
				break;
			}
			else if (0xa1 <= c && c <= 0xef) {
				status_push_ch (ptr, SSO);
				status_push_ch (ptr, c);
				code_score (ptr);
				status_reset (ptr);
			}
			else if ((0x81 <= c && c < 0xa0) || (0xe0 <= c && c <= 0xea)) {
				ptr->stat = 1;
				status_push_ch (ptr, c);
			}
			else {
				status_disable (ptr);
			}
			break;
		case 1:
			if ((0x40 <= c && c <= 0x7e) || (0x80 <= c && c <= 0xfd)) {
				status_push_ch (ptr, c);
				s2e_conv (ptr->buf[0], ptr->buf[1], &ptr->buf[0], &ptr->buf[1]);
				code_score (ptr);
				status_reset (ptr);
			}
			else {
				status_disable (ptr);
			}
			break;
		}
	}
	void e_status (struct input_code* ptr, int c)
	{
		switch (ptr->stat) {
		case -1:
			status_check (ptr, c);
			break;
		case 0:
			if (c <= DEL) {
				break;
			}
			else if (SSO == c || (0xa1 <= c && c <= 0xfe)) {
				ptr->stat = 1;
				status_push_ch (ptr, c);
			}
			else {
				status_disable (ptr);
			}
			break;
		case 1:
			if (0xa1 <= c && c <= 0xfe) {
				status_push_ch (ptr, c);
				code_score (ptr);
				status_reset (ptr);
			}
			else {
				status_disable (ptr);
			}
			break;
		}
	}
	void w_status (struct input_code* ptr, int c)
	{
		switch (ptr->stat) {
		case -1:
			status_check (ptr, c);
			break;
		case 0:
			if (c <= DEL) {
				break;
			}
			else if (0xc0 <= c && c <= 0xdf) {
				ptr->stat = 1;
				status_push_ch (ptr, c);
			}
			else if (0xe0 <= c && c <= 0xef) {
				ptr->stat = 2;
				status_push_ch (ptr, c);
			}
			else {
				status_disable (ptr);
			}
			break;
		case 1:
		case 2:
			if (0x80 <= c && c <= 0xbf) {
				status_push_ch (ptr, c);
				if (ptr->index > ptr->stat) {
					w2e_conv (ptr->buf[0], ptr->buf[1], ptr->buf[2], &ptr->buf[0], &ptr->buf[1]);
					code_score (ptr);
					status_reset (ptr);
				}
			}
			else {
				status_disable (ptr);
			}
			break;
		}
	}
	void code_status (int c)
	{
		int action_flag = 1;
		struct input_code* result = 0;
		struct input_code* p = input_code_list;
		while (p->name) {
			(this->*p->status_func)(p, c);
			if (p->stat > 0) {
				action_flag = 0;
			}
			else if (p->stat == 0) {
				if (result) {
					action_flag = 0;
				}
				else {
					result = p;
				}
			}
			++p;
		}
		if (action_flag) {
			if (result) {
				set_iconv (TRUE, result->iconv_func);
			}
			else if (c <= DEL) {
				struct input_code* ptr = input_code_list;
				while (ptr->name) {
					status_reset (ptr);
					++ptr;
				}
			}
		}
	}
	int std_getc (char* f)
	{
		if (*(f + len_in) == (char)'\0')
			return (EOF);
		return (unsigned char)(*(f + len_in++));
	}
	int std_ungetc (int c, char* f)
	{
		IGNORE_PARAMETER (c);
		if (len_in < 1 || offlag != 0)
			return (EOF);
		return (unsigned char)(*(f + len_in--));
	}
	void std_putc (int c)
	{
		if (len_out >= glen - 1)
			offlag = 1;
		else if (c != EOF)
			*(gout + len_out++) = (unsigned char)c;
	}
	void   module_connection (void)
	{
		oconv = output_conv;
		o_putc = &nkf_cnv::std_putc;
		/* replace continucation module, from output side */
		/* output redicrection */
		if (mimeout_f) {
			o_mputc = o_putc;
			o_putc = &nkf_cnv::mime_putc;
			if (mimeout_f == TRUE) {
				o_base64conv = oconv; oconv = &nkf_cnv::base64_conv;
			}
		}
		if (crmode_f) {
			o_crconv = oconv; oconv = &nkf_cnv::cr_conv;
		}
		if (rot_f) {
			o_rot_conv = oconv; oconv = &nkf_cnv::rot_conv;
		}
		if (x0201_f) {
			o_zconv = oconv; oconv = &nkf_cnv::z_conv;
		}
		i_getc = &nkf_cnv::std_getc;
		/* input redicrection */
		if (mime_f && mimebuf_f == FIXED_MIME) {
			i_mgetc = i_getc; i_getc = &nkf_cnv::mime_getc;
			i_mungetc = i_ungetc; i_ungetc = &nkf_cnv::mime_ungetc;
		}
		if (input_f == JIS_INPUT || input_f == LATIN1_INPUT) {
			set_iconv (-TRUE, &nkf_cnv::e_iconv);
		}
		else if (input_f == SJIS_INPUT) {
			set_iconv (-TRUE, &nkf_cnv::s_iconv);
		}
		else if (input_f == UTF8_INPUT) {
			set_iconv (-TRUE, &nkf_cnv::w_iconv);
		}
		else if (input_f == UTF16_INPUT) {
			set_iconv (-TRUE, &nkf_cnv::w_iconv16);
		}
		else {
			set_iconv (FALSE, &nkf_cnv::e_iconv);
		}
		{
			struct input_code* p = input_code_list;
			while (p->name) {
				status_reset (p++);
			}
		}
	}
	/* Conversion main loop. Code detection only. */
	int kanji_convert (char* f)
	{
		int c1, c2;
		module_connection ();
		c2 = 0;
		input_mode = ASCII;
		output_mode = ASCII;
		shift_mode = FALSE;
#define NEXT continue /* no output, get next */
#define SEND ;        /* output c1 and c2, get next */
#define LAST break    /* end of loop, go closing  */
		while ((c1 = (this->*i_getc)(f)) != EOF) {
			code_status (c1);
			if (c2) {
				/* second byte */
				if (c2 > DEL) {
					/* in case of 8th bit is on */
					if (!estab_f) {
						/* in case of not established yet */
						/* It is still ambiguious */
						if (h_conv (f, c2, c1) == EOF)
							LAST;
						else
							c2 = 0;
						NEXT;
					}
					else
						/* in case of already established */
						if (c1 < AT) {
							/* ignore bogus code */
							c2 = 0;
							NEXT;
						}
						else
							SEND;
				}
				else
					/* second byte, 7 bit code */
					/* it might be kanji shitfted */
					if ((c1 == DEL) || (c1 <= SPACE)) {
						/* ignore bogus first code */
						c2 = 0;
						NEXT;
					}
					else
						SEND;
			}
			else {
				/* first byte */
				if (iconv == &nkf_cnv::w_iconv16) {
					c2 = c1;
					c1 = (this->*i_getc)(f);
					SEND;
				}
				else if (c1 > DEL) {
					/* 8 bit code */
					if (!estab_f && !iso8859_f) {
						/* not established yet */
						c2 = c1;
						NEXT;
					}
					else { /* estab_f==TRUE */
						if (iso8859_f) {
							c2 = ISO8859_1;
							c1 &= 0x7f;
							SEND;
						}
						else if (SSP <= c1 && c1 < 0xe0 && iconv == &nkf_cnv::s_iconv) {
							/* SJIS X0201 Case... */
							c2 = X0201;
							c1 &= 0x7f;
							SEND;
						}
						else if (c1 == SSO && iconv != &nkf_cnv::s_iconv) {
							/* EUC X0201 Case */
							c1 = (this->*i_getc)(f);  /* skip SSO */
							code_status (c1);
							if (SSP <= c1 && c1 < 0xe0) {
								c2 = X0201;
								c1 &= 0x7f;
								SEND;
							}
							else { /* bogus code, skip SSO and one byte */
								NEXT;
							}
						}
						else {
							/* already established */
							c2 = c1;
							NEXT;
						}
					}
				}
				else if ((c1 > SPACE) && (c1 != DEL)) {
					/* in case of Roman characters */
					if (shift_mode) {
						/* output 1 shifted byte */
						if (iso8859_f) {
							c2 = ISO8859_1;
							SEND;
						}
						else if (SPACE <= c1 && c1 < (0xe0 & 0x7f)) {
							/* output 1 shifted byte */
							c2 = X0201;
							SEND;
						}
						else {
							/* look like bogus code */
							NEXT;
						}
					}
					else if (input_mode == X0208) {
						/* in case of Kanji shifted */
						c2 = c1;
						NEXT;
					}
					else if (c1 == '=' && mime_f && !mime_decode_mode) {
						/* Check MIME code */
						if ((c1 = (this->*i_getc)(f)) == EOF) {
							(this->*oconv)(0, '=');
							LAST;
						}
						else if (c1 == '?') {
							/* =? is mime conversion start sequence */
							if (mime_f == STRICT_MIME) {
								/* check in real detail */
								if (mime_begin_strict (f) == EOF)
									LAST;
								else
									NEXT;
							}
							else if (mime_begin (f) == EOF)
								LAST;
							else
								NEXT;
						}
						else {
							(this->*oconv)(0, '=');
							(this->*i_ungetc)(c1, f);
							NEXT;
						}
					}
					else {
						/* normal ASCII code */
						SEND;
					}
				}
				else if (c1 == SI) {
					shift_mode = FALSE;
					NEXT;
				}
				else if (c1 == SO) {
					shift_mode = TRUE;
					NEXT;
				}
				else if (c1 == ESC) {
					if ((c1 = (this->*i_getc)(f)) == EOF) {
						LAST;
					}
					else if (c1 == '$') {
						if ((c1 = (this->*i_getc)(f)) == EOF) {
							LAST;
						}
						else if (c1 == '@' || c1 == 'B') {
							/* This is kanji introduction */
							input_mode = X0208;
							shift_mode = FALSE;
							NEXT;
						}
						else if (c1 == '(') {
							if ((c1 = (this->*i_getc)(f)) == EOF) {
								LAST;
							}
							else if (c1 == '@' || c1 == 'B') {
								/* This is kanji introduction */
								input_mode = X0208;
								shift_mode = FALSE;
								NEXT;
							}
							else {
								/* could be some special code */
								(this->*oconv)(0, ESC);
								(this->*oconv)(0, '$');
								(this->*oconv)(0, '(');
								(this->*oconv)(0, c1);
								NEXT;
							}
						}
						else {
							(this->*oconv)(0, ESC);
							(this->*oconv)(0, '$');
							(this->*oconv)(0, c1);
							NEXT;
						}
					}
					else if (c1 == '(') {
						if ((c1 = (this->*i_getc)(f)) == EOF) {
							LAST;
						}
						else {
							if (c1 == 'I') {
								/* This is X0201 kana introduction */
								input_mode = X0201; shift_mode = X0201;
								NEXT;
							}
							else if (c1 == 'B' || c1 == 'J' || c1 == 'H') {
								/* This is X0208 kanji introduction */
								input_mode = ASCII; shift_mode = FALSE;
								NEXT;
							}
							else {
								(this->*oconv)(0, ESC);
								(this->*oconv)(0, '(');
								/* maintain various input_mode here */
								SEND;
							}
						}
					}
					else if (c1 == 'N' || c1 == 'n') {
						/* SS2 */
						c1 = (this->*i_getc)(f);  /* skip SS2 */
						if (SPACE <= c1 && c1 < 0xe0) {
							c2 = X0201;
							SEND;
						}
					}
					else {
						/* lonely ESC  */
						(this->*oconv)(0, ESC);
						SEND;
					}
				}
				else
					SEND;
			}
			/* send: */
			if (input_mode == X0208)
				(this->*oconv)(c2, c1); /* this is JIS, not SJIS/EUC case */
			else if (input_mode)
				(this->*oconv)(input_mode, c1); /* other special case */
			else if ((this->*iconv)(c2, c1, 0) < 0) { /* can be EUC/SJIS */
				int c0 = (this->*i_getc)(f);
				if (c0 != EOF) {
					code_status (c0);
					(this->*iconv)(c2, c1, c0);
				}
			}
			c2 = 0;
			continue;
			/* goto next_word */
		}
		/* epilogue */
		(this->*iconv)(EOF, 0, 0);
		return 1;
	}
	int h_conv (char* f, int c2, int c1)
	{
		int wc, c3;
		/** it must NOT be in the kanji shifte sequence      */
		/** it must NOT be written in JIS7                   */
		/** and it must be after 2 byte 8bit code            */
		hold_count = 0;
		push_hold_buf (c2);
		push_hold_buf (c1);
		c2 = 0;
		while ((c1 = (this->*i_getc)(f)) != EOF) {
			if (c1 == ESC) {
				(this->*i_ungetc)(c1, f);
				break;
			}
			code_status (c1);
			if (push_hold_buf (c1) == EOF || estab_f) {
				break;
			}
		}
		if (!estab_f) {
			struct input_code* p = input_code_list;
			struct input_code* result = p;
			while (p->name) {
				if (p->score < result->score) {
					result = p;
				}
				++p;
			}
			set_iconv (FALSE, p->iconv_func);
		}
		/** now,
		** 1) EOF is detected, or
		** 2) Code is established, or
		** 3) Buffer is FULL (but last word is pushed)
		**
		** in 1) and 3) cases, we continue to use
		** Kanji codes by oconv and leave estab_f unchanged.
		**/
		wc = 0;
		while (wc < hold_count) {
			c2 = hold_buf[wc++];
			if (c2 <= DEL) {
				(this->*iconv)(0, c2, 0);
				continue;
			}
			else if (iconv == &nkf_cnv::s_iconv && 0xa1 <= c2 && c2 <= 0xdf) {
				(this->*iconv)(X0201, c2, 0);
				continue;
			}
			if (wc < hold_count) {
				c1 = hold_buf[wc++];
			}
			else {
				c1 = (this->*i_getc)(f);
				if (c1 == EOF) break;
				code_status (c1);
			}
			if ((this->*iconv)(c2, c1, 0) < 0) {
				int c0;
				if (wc < hold_count) {
					c0 = hold_buf[wc++];
				}
				else {
					c0 = (this->*i_getc)(f);
					if (c0 == EOF) break;
					code_status (c0);
				}
				(this->*iconv)(c2, c1, c0);
				c1 = c0;
			}
		}
		c3 = c1;
		return c3;
	}
	int push_hold_buf (int c2)
	{
		if (hold_count >= HOLD_SIZE * 2)
			return (EOF);
		hold_buf[hold_count++] = (unsigned char)c2;
		return ((hold_count >= HOLD_SIZE * 2) ? EOF : hold_count);
	}
	int s2e_conv (int c2, int c1, int* p2, int* p1)
	{
		c2 = c2 + c2 - ((c2 <= 0x9f) ? SJ0162 : SJ6394);
		if (c1 < 0x9f)
			c1 = c1 - ((c1 > DEL) ? SPACE : 0x1f);
		else {
			c1 = c1 - 0x7e;
			c2++;
		}
		if (p2) *p2 = c2;
		if (p1) *p1 = c1;
		return (c2 << 8) | c1;
	}
	int s_iconv (int c2, int c1, int c0)
	{
		IGNORE_PARAMETER (c0);
		if (c2 == X0201) {
			c1 &= 0x7f;
		}
		else if ((c2 == EOF) || (c2 == 0) || c2 < SPACE) {
			/* NOP */
		}
		else {
			s2e_conv (c2, c1, &c2, &c1);
		}
		(this->*oconv)(c2, c1);
		return 0;
	}
	int e_iconv (int c2, int c1, int c0)
	{
		IGNORE_PARAMETER (c0);
		if (c2 == X0201) {
			c1 &= 0x7f;
		}
		else if (c2 == SSO) {
			c2 = X0201;
			c1 &= 0x7f;
		}
		else if ((c2 == EOF) || (c2 == 0) || c2 < SPACE) {
			/* NOP */
		}
		else {
			c1 &= 0x7f;
			c2 &= 0x7f;
		}
		(this->*oconv)(c2, c1);
		return 0;
	}
	int w2e_conv (int c2, int c1, int c0, int* p2, int* p1)
	{
		extern unsigned short* utf8_to_euc_2bytes[];
		extern unsigned short** utf8_to_euc_3bytes[];
		if (0xc0 <= c2 && c2 <= 0xef) {
			if (0xe0 <= c2) {
				if (c0 == 0) return -1;
				auto pp = utf8_to_euc_3bytes[c2 - 0x80];
				return w_iconv_common (c1, c0, pp, sizeof_utf8_to_euc_C2, p2, p1);
			}
			else {
				return w_iconv_common (c2, c1, utf8_to_euc_2bytes, sizeof_utf8_to_euc_2bytes, p2, p1);
			}
		}
		else if (c2 == X0201) {
			c1 &= 0x7f;
		}
		if (p2) *p2 = c2;
		if (p1) *p1 = c1;
		return 0;
	}
	int  w_iconv (int c2, int c1, int c0)
	{
		int ret = w2e_conv (c2, c1, c0, &c2, &c1);
		if (ret == 0) {
			(this->*oconv)(c2, c1);
		}
		return ret;
	}
	int  w_iconv16 (int c2, int c1, int c0)
	{
		extern unsigned short* utf8_to_euc_2bytes[];
		extern unsigned short** utf8_to_euc_3bytes[];
		unsigned short** pp;
		unsigned short val;
		int psize;
		int ret;
		if (c2 == 0376 && c1 == 0377) {
			utf16_mode = UTF16_INPUT;
			return 0;
		}
		else if (c2 == 0377 && c1 == 0376) {
			utf16_mode = UTF16BE_INPUT;
			return 0;
		}
		if (utf16_mode == UTF16BE_INPUT) {
			int tmp;
			tmp = c1; c1 = c2; c2 = tmp;
		}
		if (c2 == 0 || c2 == EOF) {
			(this->*oconv)(c2, c1);
			return 0;
		}
		val = (unsigned short)(((c2 << 8) & 0xff00) + c1);
		if (c2 < 0x8) {
			c0 = (0x80 | (c1 & 0x3f));
			c1 = (0xc0 | (val >> 6));
			pp = utf8_to_euc_2bytes;
			psize = sizeof_utf8_to_euc_2bytes;
		}
		else {
			c0 = (0x80 | (c1 & 0x3f));
			c2 = (0xe0 | (val >> 12));
			c1 = (0x80 | ((val >> 6) & 0x3f));
			////if (c0 == 0) return -1;
			if (0 <= c2 - 0x80 && c2 - 0x80 < sizeof_utf8_to_euc_3bytes) {
				pp = utf8_to_euc_3bytes[c2 - 0x80];
				psize = sizeof_utf8_to_euc_C2;
			}
			else {
				return 0;
			}
		}
		ret = w_iconv_common (c1, c0, pp, psize, &c2, &c1);
		if (ret) return ret;
		(this->*oconv)(c2, c1);
		return 0;
	}
	int  w_iconv_common (int c1, int c0, unsigned short** pp, int psize, int* p2, int* p1)
	{
		int c2;
		unsigned short* p;
		unsigned short val;
		if (pp == 0) return 1;
		c1 -= 0x80;
		if (c1 < 0 || psize <= c1) return 1;
		p = pp[c1];
		if (p == 0) return 1;
		c0 -= 0x80;
		if (c0 < 0 || sizeof_utf8_to_euc_E5B8 <= c0) return 1;
		val = p[c0];
		if (val == 0) return 1;
		c2 = val >> 8;
		if (c2 == SO) c2 = X0201;
		c1 = val & 0x7f;
		if (p2) *p2 = c2;
		if (p1) *p1 = c1;
		return 0;
	}
	int  e2w_conv (int c2, int c1)
	{
		extern unsigned short euc_to_utf8_1byte[];
		extern unsigned short* euc_to_utf8_2bytes[];
		unsigned short* p;
		if (c2 == X0201) {
			p = euc_to_utf8_1byte;
		}
		else {
			c2 &= 0x7f;
			c2 = (c2 & 0x7f) - 0x21;
			if (0 <= c2 && c2 < sizeof_euc_to_utf8_2bytes)
				p = euc_to_utf8_2bytes[c2];
			else
				return 0;
		}
		if (!p) return 0;
		c1 = (c1 & 0x7f) - 0x21;
		if (0 <= c1 && c1 < sizeof_euc_to_utf8_1byte)
			return p[c1];
		return 0;
	}
	void  w_oconv (int c2, int c1)
	{
		if (c2 == EOF) {
			(this->*o_putc)(EOF);
			return;
		}
		else if (c2 == 0) {
			output_mode = ASCII;
			(this->*o_putc)(c1);
		}
		else if (c2 == ISO8859_1) {
			output_mode = ISO8859_1;
			(this->*o_putc)(c1 | 0x080);
		}
		else {
			unsigned short val = (unsigned short)e2w_conv (c2, c1);
			output_mode = UTF8;
			if (0 < val && val < 0x80) {
				(this->*o_putc)(val);
			}
			else if (val < 0x800) {
				(this->*o_putc)(0xc0 | (val >> 6));
				(this->*o_putc)(0x80 | (val & 0x3f));
			}
			else {
				(this->*o_putc)(0xe0 | (val >> 12));
				(this->*o_putc)(0x80 | ((val >> 6) & 0x3f));
				(this->*o_putc)(0x80 | (val & 0x3f));
			}
		}
	}
	void  w_oconv16 (int c2, int c1)
	{
		if (w_oconv16_begin_f == 2) {
			(this->*o_putc)('\376');
			(this->*o_putc)('\377');
			w_oconv16_begin_f = 1;
		}
		if (c2 == EOF) {
			(this->*o_putc)(EOF);
			return;
		}
		else if (c2 == 0) {
			(this->*o_putc)(0);
			(this->*o_putc)(c1);
		}
		else if (c2 == ISO8859_1) {
			(this->*o_putc)(0);
			(this->*o_putc)(c1 | 0x080);
		}
		else {
			unsigned short val = (unsigned short)e2w_conv (c2, c1);
			(this->*o_putc)((val & 0xff00) >> 8);
			(this->*o_putc)(val & 0xff);
		}
	}
	void  e_oconv (int c2, int c1)
	{
		if (c2 == EOF) {
			(this->*o_putc)(EOF);
			return;
		}
		else if (c2 == 0) {
			output_mode = ASCII;
			(this->*o_putc)(c1);
		}
		else if (c2 == X0201) {
			output_mode = JAPANESE_EUC;
			(this->*o_putc)(SSO); (this->*o_putc)(c1 | 0x80);
		}
		else if (c2 == ISO8859_1) {
			output_mode = ISO8859_1;
			(this->*o_putc)(c1 | 0x080);
		}
		else {
			if ((c1 < 0x20 || 0x7e < c1) || (c2 < 0x20 || 0x7e < c2)) {
				set_iconv (FALSE, 0);
				return; /* too late to rescue this char */
			}
			output_mode = JAPANESE_EUC;
			(this->*o_putc)(c2 | 0x080);
			(this->*o_putc)(c1 | 0x080);
		}
	}
	void  s_oconv (int c2, int c1)
	{
		if (c2 == EOF) {
			(this->*o_putc)(EOF);
			return;
		}
		else if (c2 == 0) {
			output_mode = ASCII;
			(this->*o_putc)(c1);
		}
		else if (c2 == X0201) {
			output_mode = SHIFT_JIS;
			(this->*o_putc)(c1 | 0x80);
		}
		else if (c2 == ISO8859_1) {
			output_mode = ISO8859_1;
			(this->*o_putc)(c1 | 0x080);
		}
		else {
			if ((c1 < 0x20 || 0x7e < c1) || (c2 < 0x20 || 0x7e < c2)) {
				set_iconv (FALSE, 0);
				return; /* too late to rescue this char */
			}
			output_mode = SHIFT_JIS;
			(this->*o_putc)((((c2 - 1) >> 1) + ((c2 <= 0x5e) ? 0x71 : 0xb1)));
			(this->*o_putc)((c1 + ((c2 & 1) ? ((c1 < 0x60) ? 0x1f : 0x20) : 0x7e)));
		}
	}
	void  j_oconv (int c2, int c1)
	{
		if (c2 == EOF) {
			if (output_mode != ASCII && output_mode != ISO8859_1) {
				(this->*o_putc)(ESC);
				(this->*o_putc)('(');
				(this->*o_putc)(ascii_intro);
				output_mode = ASCII;
			}
			(this->*o_putc)(EOF);
		}
		else if (c2 == X0201) {
			if (output_mode != X0201) {
				output_mode = X0201;
				(this->*o_putc)(ESC);
				(this->*o_putc)('(');
				(this->*o_putc)('I');
			}
			(this->*o_putc)(c1);
		}
		else if (c2 == ISO8859_1) {
			/* iso8859 introduction, or 8th bit on */
			/* Can we convert in 7bit form using ESC-'-'-A ? Is this popular? */
			output_mode = ISO8859_1;
			(this->*o_putc)(c1 | 0x80);
		}
		else if (c2 == 0) {
			if (output_mode != ASCII && output_mode != ISO8859_1) {
				(this->*o_putc)(ESC);
				(this->*o_putc)('(');
				(this->*o_putc)(ascii_intro);
				output_mode = ASCII;
			}
			(this->*o_putc)(c1);
		}
		else {
			if (output_mode != X0208) {
				output_mode = X0208;
				(this->*o_putc)(ESC);
				(this->*o_putc)('$');
				(this->*o_putc)(kanji_intro);
			}
			if (c1 < 0x20 || 0x7e < c1)
				return;
			if (c2 < 0x20 || 0x7e < c2)
				return;
			(this->*o_putc)(c2);
			(this->*o_putc)(c1);
		}
	}
	void  base64_conv (int c2, int c1)
	{
		if (base64_count > 50 && !mimeout_mode && c2 == 0 && c1 == SPACE) {
			(this->*o_putc)(NL);
		}
		else if (base64_count > 66 && mimeout_mode) {
			(this->*o_base64conv)(EOF, 0);
			(this->*o_putc)(NL);
			(this->*o_putc)('\t'); base64_count += 7;
		}
		(this->*o_base64conv)(c2, c1);
	}
	void  cr_conv (int c2, int c1)
	{
		if (prev_cr) {
			prev_cr = 0;
			if (!(c2 == 0 && c1 == NL)) {
				cr_conv (0, '\n');
			}
		}
		if (c2) {
			(this->*o_crconv)(c2, c1);
		}
		else if (c1 == '\r') {
			prev_cr = c1;
		}
		else if (c1 == '\n') {
			if (crmode_f == CRLF) {
				(this->*o_crconv)(0, '\r');
			}
			else if (crmode_f == CR) {
				(this->*o_crconv)(0, '\r');
				return;
			}
			(this->*o_crconv)(0, NL);
		}
		else if (c1 != '\032' || crmode_f != NL) {
			(this->*o_crconv)(c2, c1);
		}
	}
	void  z_conv (int c2, int  c1)
	{
		/* if (c2) c1 &= 0x7f; assertion */
		if (x0201_f && z_prev2 == X0201) { /* X0201 */
			if (c1 == (0xde & 0x7f)) { /* 濁点 */
				z_prev2 = 0;
				(this->*o_zconv)(dv[(z_prev1 - SPACE) * 2], dv[(z_prev1 - SPACE) * 2 + 1]);
				return;
			}
			else if (c1 == (0xdf & 0x7f) && ev[(z_prev1 - SPACE) * 2]) { /* 半濁点 */
				z_prev2 = 0;
				(this->*o_zconv)(ev[(z_prev1 - SPACE) * 2], ev[(z_prev1 - SPACE) * 2 + 1]);
				return;
			}
			else {
				z_prev2 = 0;
				(this->*o_zconv)(cv[(z_prev1 - SPACE) * 2], cv[(z_prev1 - SPACE) * 2 + 1]);
			}
		}
		if (c2 == EOF) {
			(this->*o_zconv)(c2, c1);
			return;
		}
		if (x0201_f && c2 == X0201) {
			if (dv[(c1 - SPACE) * 2] || ev[(c1 - SPACE) * 2]) {
				/* wait for 濁点 or 半濁点 */
				z_prev1 = c1; z_prev2 = c2;
				return;
			}
			else {
				(this->*o_zconv)(cv[(c1 - SPACE) * 2], cv[(c1 - SPACE) * 2 + 1]);
				return;
			}
		}
		(this->*o_zconv)(c2, c1);
	}
	inline int rot13 (int c)
	{
		return ((c < 'A') ? c :
				(c <= 'M') ? (c + 13) :
				(c <= 'Z') ? (c - 13) :
				(c < 'a') ? (c) :
				(c <= 'm') ? (c + 13) :
				(c <= 'z') ? (c - 13) :
				(c));
	}
	inline int rot47 (int c)
	{
		return((c < '!') ? c :
			   (c <= 'O') ? (c + 47) :
			   (c <= '~') ? (c - 47) :
			   (c));
	}
	void  rot_conv (int c2, int c1)
	{
		if (c2 == 0 || c2 == X0201 || c2 == ISO8859_1) {
			c1 = rot13 (c1);
		}
		////else if (c2) {
		else
		{
			c1 = rot47 (c1);
			c2 = rot47 (c2);
		}
		(this->*o_rot_conv)(c2, c1);
	}
	/* This converts =?ISO-2022-JP?B?HOGE HOGE?= */
	unsigned char* mime_pattern[7] = {
	 (unsigned char*)"\075?EUC-JP?B?",
	 (unsigned char*)"\075?SHIFT_JIS?B?",
	 (unsigned char*)"\075?ISO-8859-1?Q?",
	 (unsigned char*)"\075?ISO-2022-JP?B?",
	 (unsigned char*)"\075?ISO-2022-JP?Q?",
	 (unsigned char*)"\075?UTF-8?B?",
	 NULL
	};
	int mime_encode[7] = {
	 JAPANESE_EUC, SHIFT_JIS,ISO8859_1, X0208, X0201,
	 UTF8,
	 0
	};
	int mime_encode_method[7] = {
	 'B', 'B','Q', 'B', 'Q',
	 'B',
	 0
	};
#define MAXRECOVER (20)
	/* I don't trust portablity of toupper */
	inline int nkf_toupper (int c)
	{
		return (('a' <= c && c <= 'z') ? (c - ('a' - 'A')) : c);
	}
	inline int nkf_isdigit (int c)
	{
		return ('0' <= c && c <= '9');
	}
	inline int nkf_isxdigit (int c)
	{
		return (nkf_isdigit (c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'));
	}
	void  switch_mime_getc (void)
	{
		if (i_getc != &nkf_cnv::mime_getc) {
			i_mgetc = i_getc; i_getc = &nkf_cnv::mime_getc;
			i_mungetc = i_ungetc; i_ungetc = &nkf_cnv::mime_ungetc;
			if (mime_f == STRICT_MIME) {
				i_mgetc_buf = i_mgetc; i_mgetc = &nkf_cnv::mime_getc_buf;
				i_mungetc_buf = i_mungetc; i_mungetc = &nkf_cnv::mime_ungetc_buf;
			}
		}
	}
	void  unswitch_mime_getc (void)
	{
		if (mime_f == STRICT_MIME) {
			i_mgetc = i_mgetc_buf;
			i_mungetc = i_mungetc_buf;
		}
		i_getc = i_mgetc;
		i_ungetc = i_mungetc;
	}
	int  mime_begin_strict (char* f)
	{
		int c1 = 0;
		int i, j, k;
		unsigned char* p, * q;
		int r[MAXRECOVER]; /* recovery buffer, max mime pattern lenght */
		mime_decode_mode = FALSE;
		/* =? has been checked */
		j = 0;
		p = mime_pattern[j];
		r[0] = '='; r[1] = '?';
		for (i = 2; p[i] > ' '; i++) { /* start at =? */
			if (((r[i] = c1 = (this->*i_getc)(f)) == EOF) || nkf_toupper (c1) != p[i]) {
				/* pattern fails, try next one */
				q = p;
				while ((p = mime_pattern[++j]) != 0) {
					for (k = 2; k < i; k++) /* assume length(p) > i */
						if (p[k] != q[k]) break;
					if (k == i && nkf_toupper (c1) == p[k]) break;
				}
				if (p) continue; /* found next one, continue */
				/* all fails, output from recovery buffer */
				(this->*i_ungetc)(c1, f);
				for (j = 0; j < i; j++) {
					(this->*oconv)(0, r[j]);
				}
				return c1;
			}
		}
		mime_decode_mode = p[i - 2];
		if (mime_decode_mode == 'B') {
			if (!mimebuf_f) {
				/* do MIME integrity check */
				return mime_integrity (f, mime_pattern[j]);
			}
		}
		switch_mime_getc ();
		mimebuf_f = TRUE;
		return c1;
	}
	int  mime_getc_buf (char* f)
	{
		/* we don't keep eof of Fifo, becase it contains ?= as a terminator. It was checked in mime_integrity. */
		return ((mimebuf_f) ? (this->*i_mgetc_buf)(f) : Fifo (mime_input++));
	}
	int  mime_ungetc_buf (int c, char* f)
	{
		if (mimebuf_f)
			(this->*i_mungetc_buf)(c, f);
		else
			Fifo (--mime_input) = (unsigned char)c;
		return c;
	}
	inline int hex (int c)
	{
		return (('0' <= c && c <= '9') ? (c - '0') : 
				('A' <= c && c <= 'F') ? (c - 'A' + 10) :
				('a' <= c && c <= 'f') ? (c - 'a' + 10) : 0);
	}
	int  mime_begin (char* f)
	{
		int c1;
		int i, k;
		/* In NONSTRICT mode, only =? is checked. In case of failure, we  */
		/* re-read and convert again from mime_buffer.  */
		/* =? has been checked */
		k = mime_last;
		Fifo (mime_last++) = '='; Fifo (mime_last++) = '?';
		for (i = 2; i < MAXRECOVER; i++) { /* start at =? */
			/* We accept any character type even if it is breaked by new lines */
			c1 = (this->*i_getc)(f);
			Fifo (mime_last++) = (unsigned char)c1;
			if (c1 == '\n' || c1 == ' ' || c1 == '\r' || c1 == '-' || c1 == '_' || is_alnum (c1))
				continue;
			if (c1 == '=') {
				/* Failed. But this could be another MIME preemble */
				(this->*i_ungetc)(c1, f);
				mime_last--;
				break;
			}
			if (c1 != '?') break;
			else {
				/* c1=='?' */
				c1 = (this->*i_getc)(f);
				Fifo (mime_last++) = (unsigned char)c1;
				if (!(++i < MAXRECOVER) || c1 == EOF) break;
				if (c1 == 'b' || c1 == 'B') {
					mime_decode_mode = 'B';
				}
				else if (c1 == 'q' || c1 == 'Q') {
					mime_decode_mode = 'Q';
				}
				else {
					break;
				}
				c1 = (this->*i_getc)(f);
				Fifo (mime_last++) = (unsigned char)c1;
				if (!(++i < MAXRECOVER) || c1 == EOF) break;
				if (c1 != '?') {
					mime_decode_mode = FALSE;
				}
				break;
			}
		}
		switch_mime_getc ();
		if (!mime_decode_mode) {
			/* false MIME premble, restart from mime_buffer */
			mime_decode_mode = 1; /* no decode, but read from the mime_buffer */
			/* Since we are in MIME mode until buffer becomes empty, */
			/* we never go into mime_begin again for a while. */
			return c1;
		}
		/* discard mime preemble, and goto MIME mode */
		mime_last = k;
		/* do no MIME integrity check */
		return c1; /* used only for checking EOF */
	}
	int  mime_getc (char* f)
	{
		int c1, c2, c3, c4, cc;
		int t1, t2, t3, t4, mode, exit_mode;
		if (mime_top != mime_last) { /* Something is in FIFO */
			return Fifo (mime_top++);
		}
		if (mime_decode_mode == 1 || mime_decode_mode == FALSE) {
			mime_decode_mode = FALSE;
			unswitch_mime_getc ();
			return (this->*i_getc)(f);
		}
		if (mimebuf_f == FIXED_MIME)
			exit_mode = mime_decode_mode;
		else
			exit_mode = FALSE;
		if (mime_decode_mode == 'Q') {
			if ((c1 = (this->*i_mgetc)(f)) == EOF) return (EOF);
restart_mime_q:
			if (c1 == '_') return ' ';
			if (c1 != '=' && c1 != '?') {
				return c1;
			}
			mime_decode_mode = exit_mode; /* prepare for quit */
			if (c1 <= ' ') return c1;
			if ((c2 = (this->*i_mgetc)(f)) == EOF) return (EOF);
			if (c1 == '?' && c2 == '=' && mimebuf_f != FIXED_MIME) {
				/* end Q encoding */
				input_mode = exit_mode;
				while ((c1 = (this->*i_getc)(f)) != EOF && c1 == SPACE);
				return c1;
			}
			if (c1 == '=' && c2 < ' ') { /* this is soft wrap */
				while ((c1 = (this->*i_mgetc)(f)) <= ' ') {
					if ((c1 = (this->*i_mgetc)(f)) == EOF) return (EOF);
				}
				mime_decode_mode = 'Q'; /* still in MIME */
				goto restart_mime_q;
			}
			if (c1 == '?') {
				mime_decode_mode = 'Q'; /* still in MIME */
				(this->*i_mungetc)(c2, f);
				return c1;
			}
			if ((c3 = (this->*i_mgetc)(f)) == EOF) return (EOF);
			if (c2 <= ' ') return c2;
			mime_decode_mode = 'Q'; /* still in MIME */
			return ((hex (c2) << 4) + hex (c3));
		}
		if (mime_decode_mode != 'B') {
			mime_decode_mode = FALSE;
			return (this->*i_mgetc)(f);
		}
		/* Base64 encoding */
		/*
		MIME allows line break in the middle of
		Base64, but we are very pessimistic in decoding
		in unbuf mode because MIME encoded code may broken by
		less or editor's control sequence (such as ESC-[-K in unbuffered
		mode. ignore incomplete MIME.
		*/
		mode = mime_decode_mode;
		mime_decode_mode = exit_mode; /* prepare for quit */
		while ((c1 = (this->*i_mgetc)(f)) <= ' ') {
			if (c1 == EOF)
				return (EOF);
		}
mime_c2_retry:
		if ((c2 = (this->*i_mgetc)(f)) <= ' ') {
			if (c2 == EOF)
				return (EOF);
			if (mime_f != STRICT_MIME) goto mime_c2_retry;
			if (mimebuf_f != FIXED_MIME) input_mode = ASCII;
			return c2;
		}
		if ((c1 == '?') && (c2 == '=')) {
			input_mode = ASCII;
			while ((c1 = (this->*i_getc)(f)) != EOF && c1 == SPACE);
			return c1;
		}
mime_c3_retry:
		if ((c3 = (this->*i_mgetc)(f)) <= ' ') {
			if (c3 == EOF)
				return (EOF);
			if (mime_f != STRICT_MIME) goto mime_c3_retry;
			if (mimebuf_f != FIXED_MIME) input_mode = ASCII;
			return c3;
		}
mime_c4_retry:
		if ((c4 = (this->*i_mgetc)(f)) <= ' ') {
			if (c4 == EOF)
				return (EOF);
			if (mime_f != STRICT_MIME) goto mime_c4_retry;
			if (mimebuf_f != FIXED_MIME) input_mode = ASCII;
			return c4;
		}
		mime_decode_mode = mode; /* still in MIME sigh... */
		/* BASE 64 decoding */
		t1 = 0x3f & base64decode (c1);
		t2 = 0x3f & base64decode (c2);
		t3 = 0x3f & base64decode (c3);
		t4 = 0x3f & base64decode (c4);
		cc = ((t1 << 2) & 0x0fc) | ((t2 >> 4) & 0x03);
		if (c2 != '=') {
			Fifo (mime_last++) = (unsigned char)cc;
			cc = ((t2 << 4) & 0x0f0) | ((t3 >> 2) & 0x0f);
			if (c3 != '=') {
				Fifo (mime_last++) = (unsigned char)cc;
				cc = ((t3 << 6) & 0x0c0) | (t4 & 0x3f);
				if (c4 != '=')
					Fifo (mime_last++) = (unsigned char)cc;
			}
		}
		else {
			return c1;
		}
		return Fifo (mime_top++);
	}
	int  mime_ungetc (int c, char* f)
	{
		IGNORE_PARAMETER (f);
		Fifo (--mime_top) = (unsigned char)c;
		return c;
	}
	int  mime_integrity (char* f, unsigned char* p)
	{
		int c, d;
		unsigned int q;
		/* In buffered mode, read until =? or NL or buffer full */
		mime_input = mime_top;
		mime_last = mime_top;
		while (*p) Fifo (mime_input++) = *p++;
		d = 0;
		q = mime_input;
		while ((c = (this->*i_getc)(f)) != EOF) {
			if (((mime_input - mime_top) & MIME_BUF_MASK) == 0) {
				break; /* buffer full */
			}
			if (c == '=' && d == '?') {
				/* checked. skip header, start decode */
				Fifo (mime_input++) = (unsigned char)c;
				/* mime_last_input = mime_input; */
				mime_input = q;
				switch_mime_getc ();
				return 1;
			}
			if (!((c == '+' || c == '/' || c == '=' || c == '?' || is_alnum (c))))
				break;
			/* Should we check length mod 4? */
			Fifo (mime_input++) = (unsigned char)c;
			d = c;
		}
		/* In case of Incomplete MIME, no MIME decode  */
		Fifo (mime_input++) = (unsigned char)c;
		mime_last = mime_input; /* point undecoded buffer */
		mime_decode_mode = 1; /* no decode on Fifo last in mime_getc */
		switch_mime_getc (); /* anyway we need buffered getc */
		return 1;
	}
	int  base64decode (int c)
	{
		int i;
		if (c > '@') {
			if (c < '[') {
				i = c - 'A';
			}
			else {
				i = c - 'G';
			}
		}
		else if (c > '/') {
			i = c - '0' + '4';
		}
		else if (c == '+') {
			i = '>';
		}
		else {
			i = '?';
		}
		return (i);
	}
	const char basis_64[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int b64c;
	void  open_mime (int mode)
	{
		unsigned char* p;
		int i;
		p = mime_pattern[0];
		for (i = 0; mime_encode[i]; i++) {
			if (mode == mime_encode[i]) {
				p = mime_pattern[i];
				break;
			}
		}
		mimeout_mode = mime_encode_method[i];
		/* (*o_mputc)(' '); */
		while (*p) {
			(this->*o_mputc)(*p++);
			base64_count++;
		}
	}
	void  close_mime (void)
	{
		(this->*o_mputc)('?');
		(this->*o_mputc)('=');
		(this->*o_mputc)(' ');
		base64_count += 3;
		mimeout_mode = 0;
	}
	void  mime_putc (int c)
	{
		if (mimeout_f == FIXED_MIME) {
			if (base64_count > 71) {
				(this->*o_mputc)('\n');
				base64_count = 0;
			}
		}
		else if (c == NL) {
			base64_count = 0;
		}
		if (c != EOF) {
			if (c <= DEL && (output_mode == ASCII || output_mode == ISO8859_1) && mimeout_f != FIXED_MIME) {
				if (mimeout_mode == 'Q') {
					if (c <= SPACE) {
						close_mime ();
					}
					(this->*o_mputc)(c);
					return;
				}
				if (mimeout_mode != 'B' || c != SPACE) {
					if (mimeout_mode) {
						mime_putc (EOF);
						mimeout_mode = 0;
					}
					(this->*o_mputc)(c);
					base64_count++;
					return;
				}
			}
			else if (!mimeout_mode && mimeout_f != FIXED_MIME) {
				open_mime (output_mode);
			}
		}
		else { /* c == EOF */
			switch (mimeout_mode) {
			case 'Q':
			case 'B':
				break;
			case 2:
				(this->*o_mputc)(basis_64[((b64c & 0x3) << 4)]);
				(this->*o_mputc)('=');
				(this->*o_mputc)('=');
				base64_count += 3;
				break;
			case 1:
				(this->*o_mputc)(basis_64[((b64c & 0xF) << 2)]);
				(this->*o_mputc)('=');
				base64_count += 2;
				break;
			}
			if (mimeout_mode) {
				if (mimeout_f != FIXED_MIME) {
					close_mime ();
				}
				else if (mimeout_mode != 'Q')
					mimeout_mode = 'B';
			}
			return;
		}
		switch (mimeout_mode) {
		case 'Q':
			if (c >= DEL) {
				(this->*o_mputc)('=');
				(this->*o_mputc)(itoh4 (((c >> 4) & 0xf)));
				(this->*o_mputc)(itoh4 ((c & 0xf)));
			}
			else {
				(this->*o_mputc)(c);
			}
			break;
		case 'B':
			b64c = c;
			(this->*o_mputc)(basis_64[c >> 2]);
			mimeout_mode = 2;
			base64_count++;
			break;
		case 2:
			(this->*o_mputc)(basis_64[((b64c & 0x3) << 4) | ((c & 0xF0) >> 4)]);
			b64c = c;
			mimeout_mode = 1;
			base64_count++;
			break;
		case 1:
			(this->*o_mputc)(basis_64[((b64c & 0xF) << 2) | ((c & 0xC0) >> 6)]);
			(this->*o_mputc)(basis_64[c & 0x3F]);
			mimeout_mode = 'B';
			base64_count += 2;
			break;
		}
	}
	void  no_connection (int c2, int c1)
	{
		no_connection2 (c2, c1, 0);
	}
	int  no_connection2 (int c2, int c1, int c0)
	{
		IGNORE_PARAMETER (c2);
		IGNORE_PARAMETER (c1);
		IGNORE_PARAMETER (c0);
		ncflag = 1;
		return (EOF);
	}
	/**
	** パッチ制作者
	**  void@merope.pleiades.or.jp (Kusakabe Youichi)
	**  NIDE Naoyuki <nide@ics.nara-wu.ac.jp>
	**  ohta@src.ricoh.co.jp (Junn Ohta)
	**  inouet@strl.nhk.or.jp (Tomoyuki Inoue)
	**  kiri@pulser.win.or.jp (Tetsuaki Kiriyama)
	**  Kimihiko Sato <sato@sail.t.u-tokyo.ac.jp>
	**  a_kuroe@kuroe.aoba.yokohama.jp (Akihiko Kuroe)
	**  kono@ie.u-ryukyu.ac.jp (Shinji Kono)
	**  GHG00637@nifty-serve.or.jp (COW)
	**
	**/
	/* end */
};
#endif