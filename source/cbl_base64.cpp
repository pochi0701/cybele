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
#ifndef linux
//#include <mem.h>
#endif

static unsigned char ToBase64tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static unsigned char tmpout[256];
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
unsigned char* unbase64(unsigned char* instr)
{
	int s1;
	int s2;
	int s3;
	int s4;

	char FromBase64tbl[256] = { 0 };
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
		s1 = FromBase64tbl[*instr++];
		s2 = FromBase64tbl[*instr++];
		s3 = FromBase64tbl[*instr++];
		s4 = FromBase64tbl[*instr++];
		*p++ = (unsigned char)((s1 << 2) | (s2 >> 4));
		*p++ = (unsigned char)(((s2 & 0x0f) << 4) | (s3 >> 2));
		*p++ = (unsigned char)(((s3 & 0x03) << 6) | s4);
		*p = 0;
	}
	return tmpout;
}