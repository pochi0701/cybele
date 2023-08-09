#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_base64.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================


static unsigned char ToBase64tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static unsigned char tmpout[256];

/// <summary>
/// 文字列からBASE64表現の文字列に変換
/// 返還後の文字列が255文字まで。
/// </summary>
/// <param name="instr">変換する文字列</param>
/// <returns>変換した文字列</returns>
unsigned char* base64(unsigned char* instr)
{
	unsigned char* p = tmpout;
	int count = 0;
	/*
	ABC ->
	414243 ->
	0100 0001  0100 0010  0100 0011 ->
	010000 010100 001001 000011
	10 14 09 03
	*/
	while (*instr) {
		switch (count) {
		case 0:
			*p = (unsigned char)((*instr >> 2) & 0x3f);
			break;
		case 1:
			*p = (unsigned char)((*instr++ << 4) & 0x3f);
			*p |= (unsigned char)((*instr >> 4) & 0x3f);
			break;
		case 2:
			*p = (unsigned char)((*instr++ << 2) & 0x3f);
			*p |= (unsigned char)((*instr >> 6) & 0x3f);
			break;
		case 3:
			*p = (unsigned char)((*instr++) & 0x3f);
			break;
		}
		count++;
		count %= 4;
		*p = ToBase64tbl[*p];
		*++p = '\0';
	}
	count = (4 - count) % 4;
	while (count-- > 0) {
		*p++ = '=';
		*p = '\0';
	}
	return tmpout;
}

/// <summary>
/// BASE64表現の文字列から変換前の文字列に復元
/// 返還後の文字列が255文字まで。
/// </summary>
/// <param name="instr">変換する文字列</param>
/// <returns>変換した文字列</returns>
unsigned char* unbase64(unsigned char* instr)
{
	char FromBase64tbl[256] = {};
	unsigned char* p = tmpout;
	//逆テーブルの作成,=も０になる
	for (unsigned int i = 0; i < sizeof(ToBase64tbl); i++) {
		FromBase64tbl[ToBase64tbl[i]] = (unsigned char)i;
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
		*p++ = (unsigned char)((s1 << 2) | (s2 >> 4));
		*p++ = (unsigned char)(((s2 & 0x0f) << 4) | (s3 >> 2));
		*p++ = (unsigned char)(((s3 & 0x03) << 6) | s4);
		*p = 0;
	}
	return tmpout;
}