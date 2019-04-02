﻿#include "stdafx.h"
/*
 * TinyJS
 *
 * A single-file Javascript-alike engine
 *
 * Authored By Gordon Williams <gw@pur3.co.uk>
 *
 * Copyright (C) 2009 Pur3 Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Version 0.1  :  (gw) First published on Google Code
Version 0.11 :  Making sure the 'root' variable never changes
'symbol_base' added for the current base of the sybmbol table
Version 0.12 :  Added findChildOrCreate, changed wString passing to use references
Fixed broken wString encoding in getJSString()
Removed getInitCode and added getJSON instead
Added nil
Added rough JSON parsing
Improved example app
Version 0.13 :  Added tokenEnd/tokenLastEnd to lexer to avoid parsing whitespace
Ability to define functions without names
Can now do "var mine = function(a,b) { ... };"
Slightly better 'trace' function
Added findChildOrCreateByPath function
Added simple test suite
Added skipping of blocks when not executing
Version 0.14 :  Added parsing of more number types
Added parsing of wString defined with '
Changed nil to null as per spec, added 'undefined'
Now set variables with the correct scope, and treat unknown
as 'undefined' rather than failing
Added proper (I hope) handling of null and undefined
Added === check
Version 0.15 :  Fix for possible memory leaks
Version 0.16 :  Removal of un-needed findRecursive calls
symbol_base removed and replaced with 'scopes' stack
Added reference counting a proper tree structure
(Allowing pass by reference)
Allowed JSON output to output IDs, not strings
Added get/set for array indices
Changed Callbacks to include user data pointer
Added some support for objects
Added more Java-esque builtin functions
Version 0.17 :  Now we don't deepCopy the parent object of the class
Added JSON.stringify and eval()
Nicer JSON indenting
Fixed function output in JSON
Added evaluateComplex
Fixed some reentrancy issues with evaluate/execute
Version 0.18 :  Fixed some issues with code being executed when it shouldn't
Version 0.19 :  Added array.length
Changed '__parent' to 'prototype' to bring it more in line with javascript
Version 0.20 :  Added '%' operator
Version 0.21 :  Added array type
String.length() no more - now String.length
Added extra constructors to reduce confusion
Fixed checks against undefined
Version 0.22 :  First part of ardi's changes:
sprintf -> sprintf_s
extra tokens parsed
array memory leak fixed
Fixed memory leak in evaluateComplex
Fixed memory leak in FOR loops
Fixed memory leak for unary minus
Version 0.23 :  Allowed evaluate[Complex] to take in semi-colon separated
statements and then only return the value from the last one.
Also checks to make sure *everything* was parsed.
Ints + doubles are now stored in binary form (faster + more precise)
Version 0.24 :  More useful error for maths ops
Don't dump everything on a match error.
Version 0.25 :  Better wString escaping
Version 0.26 :  Add CScriptVar::equals
Add built-in array functions
Version 0.27 :  Added OZLB's TinyJS.setVariable (with some tweaks)
Added OZLB's Maths Functions
Version 0.28 :  Ternary operator
Rudimentary call stack on error
Added String Character functions
Added shift operators
Version 0.29 :  Added new object via functions
Fixed getString() for double on some platforms
Version 0.30 :  Rlyeh Mario's patch for Math Functions on VC++
Version 0.31 :  Add exec() to TinyJS functions
Now print quoted JSON that can be read by PHP/Python parsers
Fixed postfix increment operator
Version 0.32 :  Fixed Math.randInt on 32 bit PCs, where it was broken
Version 0.33 :  Fixed Memory leak + brokenness on === comparison
Version 0.34 :  Modify for cgi.

NOTE:
Constructing an array with an initial length 'Array(5)' doesn't work
Recursive loops of data such as a.foo = a; fail to be garbage collected
length variable cannot be set
The postfix increment operator returns the current value, not the previous as it should.
There is no prefix increment operator
Arrays are implemented as a linked list - hence a lookup time is O(n)

TODO:
Utility va-args style function in TinyJS for executing a function directly
Merge the parsing of expressions/statements so eval("statement") works like we'd expect.
Move 'shift' implementation into mathsOp

*/

#include "TinyJS.h"
#include <assert.h>
#include "define.h"

#ifndef ASSERT
#define ASSERT(X) assert(X)
#endif
/* Frees the given link IF it isn't owned by anything else */
#define CLEAN(x) { CScriptVarLink *__v = x; if (__v && !__v->owned) { delete __v; } }
/* Create a LINK to point to VAR and free the old link.
 * BUT this is more clever - it tries to keep the old link if it's not owned to save allocations */
#define CREATE_LINK(LINK, VAR) { if (!LINK || LINK->owned) LINK = new CScriptVarLink(VAR); else LINK->replaceWith(VAR); }
#define web
void headerCheckPrint(int mysocket, int* printed, wString* headerBuf,int flag);
static unsigned char cmap[256]={
//+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,//00
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//10
   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//20
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,//30
   0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,//40
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 4,//50
   0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,//60
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0,//70
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//80
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//90
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//A0
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//B0
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//C0
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//D0
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//E0
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//F0
}; 

////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------------- Utils
bool isWhitespace(unsigned char ch) {
    return (cmap[ch]&1);//(ch==' ') || (ch=='\t') || (ch=='\n') || (ch=='\r');
}
////////////////////////////////////////////////////////////////////////////////
//数字チェック
bool isNumeric(unsigned char ch) {
    return (cmap[ch]&2);//(ch>='0') && (ch<='9');
}
////////////////////////////////////////////////////////////////////////////////
//数値チェック
bool isNumber(const wString &str) {
    for (size_t i=0;i<str.size();i++){
        if (!isNumeric(str[i])) return false;
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//１６進チェック
bool isHexadecimal(unsigned char ch) {
    return ((ch>='0') && (ch<='9')) ||
           ((ch>='a') && (ch<='f')) ||
           ((ch>='A') && (ch<='F'));
}
////////////////////////////////////////////////////////////////////////////////
//アルファベットチェック
bool isAlpha(unsigned char ch) {
    return (cmap[ch]&4);//((ch>='a') && (ch<='z')) || ((ch>='A') && (ch<='Z')) || ch=='_';
}
////////////////////////////////////////////////////////////////////////////////
//ID文字列になるかチェック
//bool isIDString(const char *s) {
//    if (!isAlpha(*s)){
//        return false;
//    }
//    while (*s) {
//        if (!(isAlpha(*s) || isNumeric(*s))){
//            return false;
//        }
//        s++;
//    }
//    return true;
//}
////////////////////////////////////////////////////////////////////////////////
//エラー時1行出力
//char* oneLine(char *s, int ptr,int end)
//{
//    size_t cnt=0;
//    static char work[1024];
//    if( end < ptr ){
//        ptr = end;
//    }
//    if( ptr>0) ptr--;
//    if( ptr>0) ptr--;
//    while( ptr > 0  && s[ptr] != '\n'){
//        ptr--;
//    }
//    //ptr++;
//    while( s[ptr] && s[ptr] != '\n' && cnt < sizeof( work )-2 ){
//        work[cnt] = s[ptr];
//        cnt++;
//        ptr++;
//    }
//    work[cnt] = 0;
//    return work;
//}
////////////////////////////////////////////////////////////////////////////////
//エラー時1行出力
wString oneLine(char *s, int ptr,int end)
{
    wString work;
    if( end < ptr ){
        ptr = end;
    }
    //前の行に入るように
    if( ptr>0) ptr--;
    if( ptr>0) ptr--;
    //遡る
    while( ptr > 0  && s[ptr] != '\n'){
        ptr--;
    }
    //ptr++;
    while( s[ptr] && s[ptr] != '\n'  ){
        work += s[ptr++];
    }
    return work;
}
////////////////////////////////////////////////////////////////////////////////
/// convert the given wString into a quoted wString suitable for javascript
wString getJSString(const wString &str) {
    wString nStr = str;
    for (size_t i=0;i<nStr.size();i++) {
        const char *replaceWith = "";
        bool replace = true;

        switch (nStr[i]) {
            case '\\': replaceWith = "\\\\"; break;
            case '\n': replaceWith = "\\n"; break;
            case '\r': replaceWith = "\\r"; break;
            case '\a': replaceWith = "\\a"; break;
            case '"':  replaceWith = "\\\""; break;
            default: {
                int nCh = ((int)nStr[i]) &0xFF;
                if (nCh<32 || nCh>127) {
                    char buffer[5];
                    snprintf(buffer, 5, "\\x%02X", nCh);
                    replaceWith = buffer;
                } else {
                    replace=false;
                }
            }
        }

        if (replace) {
            nStr = nStr.substr(0, i) + replaceWith + nStr.substr(i+1);
            i += strlen(replaceWith)-1;
        }
    }
    return "\"" + nStr + "\"";
}

/** Is the wString alphanumeric */
//英字+[英字|数値]
bool isAlphaNum(const wString &str) {
    if (str.size()==0){
      return true;
    }
    if (!isAlpha(str[0])){
      return false;
    }
    for (size_t i=0;i<str.size();i++){
      if (!(isAlpha(str[i]) || isNumeric(str[i]))){
        return false;
      }
    }
    return true;
}
#ifdef web
void JSTRACE( int socket, const char* format, ... )
{
    char work[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(work,format, ap);
    va_end(ap);
    send( socket, work, strlen(work),0);
}
#endif
// ----------------------------------------------------------------------------------- CSCRIPTEXCEPTION
// 例外はtextに格納
CScriptException::CScriptException(const wString &exceptionText) {
    text = exceptionText;
}

// ----------------------------------------------------------------------------------- CSCRIPTLEX
// スクリプト語彙クラス
CScriptLex::CScriptLex(int mysocket, int* myprinted, wString* myheaderBuf, const wString &input, int executeMode,wString* myPrBuffer,int* myPrPos) {
    data = strdup(input.c_str());//寿命の点からコピーする。deleteで消す
    dataOwned = true;
    dataStart = 0;
    dataEnd   = input.size();
    socket    = mysocket;
    printed   = myprinted;
    headerBuf = myheaderBuf;
    execMode  = executeMode;
    prBuffer  = myPrBuffer;
    prPos     = myPrPos;
    reset();
}

CScriptLex::CScriptLex(int mysocket, int* myprinted, wString* myheaderBuf,CScriptLex *owner, int startChar, int endChar,wString* myPrBuffer,int* myPrPos) {
    data = owner->data;
    dataOwned = false;
    dataStart = startChar;
    dataEnd   = endChar;
    socket    = mysocket;
    printed   = myprinted;
    headerBuf = myheaderBuf;
    execMode  = 1;
    prBuffer  = myPrBuffer;
    prPos     = myPrPos;
    reset();
}
CScriptLex::~CScriptLex(void)
{
    if (dataOwned){
        free((void*)data);
    }
}

void CScriptLex::reset() {
    dataPos = dataStart;
    tokenStart = 0;
    tokenEnd = 0;
    tokenLastEnd = 0;
    tk = LEX_EOF;
    tkStr = "";
    getNextCh();//currch設定 nextchは不定
    getNextCh();//currch,nextch設定
    getNextToken();//１ワード取り込んだ状態で開始
}
//期待する語をチェックして次の１トークンを先読み
//期待はずれなら例外
void CScriptLex::chkread(int expected_tk) {
    if (tk!=expected_tk) {
        wString errorString;
        errorString.sprintf( "Got %s expected %s at %s(%s)\n",getTokenStr(tk).c_str(),getTokenStr(expected_tk).c_str(),getPosition(tokenStart).c_str(),oneLine(data,dataPos,dataEnd).c_str());
        throw new CScriptException(errorString.c_str());
    }
    getNextToken();
}
#ifdef web
//グローバルで申し訳ないが最初に文字を出力する際にheaderを先に出す
void headerCheckPrint(int socket, int* printed, wString* headerBuf, int flag)
{
    if( headerBuf->length() == 0 ){
        headerBuf->headerInit(0,0);
    }
    if( flag && *printed == 0 ){
        *printed = 1;
        headerBuf->headerPrint(socket,1);
    }
}
#endif
//エラー用等にトークンを語彙に変換
wString CScriptLex::getTokenStr(int token) {
    if (token>32 && token<128) {
        char buf[4] = "' '";
        buf[1] = (char)token;
        return wString(buf);
    }
    switch (token) {
        case LEX_EOF            : return "EOF";
        case LEX_ID             : return "ID";
        case LEX_INT            : return "INT";
        case LEX_FLOAT          : return "FLOAT";
        case LEX_STR            : return "STRING";
        case LEX_EQUAL          : return "==";
        case LEX_TYPEEQUAL      : return "===";
        case LEX_NEQUAL         : return "!=";
        case LEX_NTYPEEQUAL     : return "!==";
        case LEX_LEQUAL         : return "<=";
        case LEX_LSHIFT         : return "<<";
        case LEX_LSHIFTEQUAL    : return "<<=";
        case LEX_GEQUAL         : return ">=";
        case LEX_RSHIFT         : return ">>";
        case LEX_RSHIFTUNSIGNED : return ">>";
        case LEX_RSHIFTEQUAL    : return ">>=";
        case LEX_PLUSEQUAL      : return "+=";
        case LEX_MINUSEQUAL     : return "-=";
        case LEX_PLUSPLUS       : return "++";
        case LEX_MINUSMINUS     : return "--";
        case LEX_ANDEQUAL       : return "&=";
        case LEX_ANDAND         : return "&&";
        case LEX_OREQUAL        : return "|=";
        case LEX_OROR           : return "||";
        case LEX_XOREQUAL       : return "^=";
        // reserved words
        case LEX_R_IF           : return "if";
        case LEX_R_ELSE         : return "else";
        case LEX_R_DO           : return "do";
        case LEX_R_WHILE        : return "while";
        case LEX_R_FOR          : return "for";
        case LEX_R_BREAK        : return "break";
        case LEX_R_CONTINUE     : return "continue";
        case LEX_R_FUNCTION     : return "function";
        case LEX_R_RETURN       : return "return";
        case LEX_R_VAR          : return "var";
        case LEX_R_TRUE         : return "true";
        case LEX_R_FALSE        : return "false";
        case LEX_R_NULL         : return "null";
        case LEX_R_UNDEFINED    : return "undefined";
        case LEX_R_NEW          : return "new";
    }
    wString msg;
    msg.sprintf( "?[%s]",token);
    return msg;
}
//次の１文字を取り込む。EOFは０
void CScriptLex::getNextCh() {
    currCh = nextCh;
    if (dataPos < dataEnd){
#if 0
    //これは便利かもしれない
    if( execMode){
        send(socket," ",1,0);
        send(socket,data+dataPos,1,0);
    }else{
        send(socket,".",1,0);
        send(socket,data+dataPos,1,0);
    }
#endif
        nextCh = data[dataPos];
    }else{
        nextCh = 0;
    }
    dataPos++;
}
//１トークン取得
void CScriptLex::getNextToken() {
    tk = LEX_EOF;
    tkStr.clear();
    //非実行ブロック
    if( execMode == 0 ){
        //残りのバッファ出力
        if( prBuffer->length() > 0 ){
           headerCheckPrint(socket, printed, headerBuf, 1);
           send( socket, prBuffer->c_str(),prBuffer->length(),0);
           prBuffer->clear();
           *prPos = -1;
        }
        while (currCh && (currCh!='<' || nextCh!='?')){
            if( *prPos == -1 ) *prPos = dataPos;
            *prBuffer += currCh;
            getNextCh();
        }
        execMode = 1;
        getNextCh();
        getNextCh();
        getNextToken();
        return;
    }
    //無駄文字読み飛ばし
    while (currCh && isWhitespace(currCh)){
        getNextCh();
    }
    // newline comments
    if (currCh=='/' && nextCh=='/') {
        while (currCh && currCh!='\n'){
            getNextCh();
        }
        getNextCh();
        getNextToken();
        return;
    }
    // block comments
    if (currCh=='/' && nextCh=='*') {
        while (currCh && (currCh!='*' || nextCh!='/')) {
            getNextCh();
        }
        getNextCh();
        getNextCh();
        getNextToken();
        return;
    }
    // block comments
    if ( currCh=='?' && nextCh=='>') {
        getNextCh();
        getNextCh();
        execMode = 0;
        getNextToken();
        return;
    }
    // record beginning of this token(pre-read 2 chars );
    tokenStart = dataPos-2;
    // tokens
    if (isAlpha(currCh)) { //  IDs
        while (isAlpha(currCh) || isNumeric(currCh)) {
            tkStr += currCh;
            getNextCh();
        }
        tk = LEX_ID;
             if (tkStr=="if")        tk = LEX_R_IF;
        else if (tkStr=="else")      tk = LEX_R_ELSE;
        else if (tkStr=="do")        tk = LEX_R_DO;
        else if (tkStr=="while")     tk = LEX_R_WHILE;
        else if (tkStr=="for")       tk = LEX_R_FOR;
        else if (tkStr=="break")     tk = LEX_R_BREAK;
        else if (tkStr=="continue")  tk = LEX_R_CONTINUE;
        else if (tkStr=="function")  tk = LEX_R_FUNCTION;
        else if (tkStr=="return")    tk = LEX_R_RETURN;
        else if (tkStr=="var")       tk = LEX_R_VAR;
        else if (tkStr=="true")      tk = LEX_R_TRUE;
        else if (tkStr=="false")     tk = LEX_R_FALSE;
        else if (tkStr=="null")      tk = LEX_R_NULL;
        else if (tkStr=="undefined") tk = LEX_R_UNDEFINED;
        else if (tkStr=="new")       tk = LEX_R_NEW;
    } else if (isNumeric(currCh)) { // Numbers
        bool isHex = false;
        if (currCh=='0') { tkStr += currCh; getNextCh(); }
        if (currCh=='x') {
            isHex = true;
            tkStr += currCh; getNextCh();
        }
        tk = LEX_INT;
        while (isNumeric(currCh) || (isHex && isHexadecimal(currCh))) {
            tkStr += currCh;
            getNextCh();
        }
        if (!isHex && currCh=='.') {
            tk = LEX_FLOAT;
            tkStr += '.';
            getNextCh();
            while (isNumeric(currCh)) {
                tkStr += currCh;
                getNextCh();
            }
        }
        // do fancy e-style floating point
        if (!isHex && (currCh=='e'||currCh=='E')) {
            tk = LEX_FLOAT;
            tkStr += currCh;
            getNextCh();
            if (currCh=='-') {
                tkStr += currCh;
                getNextCh();
            }
            while (isNumeric(currCh)) {
                tkStr += currCh;
                getNextCh();
            }
        }
    } else if (currCh=='"') {
        // strings...
        getNextCh();
        while (currCh && currCh!='"') {
            if (currCh == '\\') {
                getNextCh();
                switch (currCh) {
                    case 'n' : tkStr += '\n'; break;
                    case 'r' : tkStr += '\r'; break;
                    case 't' : tkStr += '\t'; break;
                    case '"' : tkStr += '"'; break;
                    case '\\' : tkStr += '\\'; break;
                    default: tkStr += currCh;
                }
            } else {
                tkStr += currCh;
            }
            getNextCh();
        }
        getNextCh();
        tk = LEX_STR;
    } else if (currCh=='\'') {
        // strings again...
        getNextCh();
        while (currCh && currCh!='\'') {
            if (currCh == '\\') {
                getNextCh();
                switch (currCh) {
                    case 'n' : tkStr += '\n'; break;
                    case 'a' : tkStr += '\a'; break;
                    case 'r' : tkStr += '\r'; break;
                    case 't' : tkStr += '\t'; break;
                    case '\'' : tkStr += '\''; break;
                    case '\\' : tkStr += '\\'; break;
                    case 'x' : { // hex digits
                        char buf[3] = "??";
                        getNextCh(); buf[0] = currCh;
                        getNextCh(); buf[1] = currCh;
                        tkStr += (char)strtol(buf,0,16);
                    } break;
                    default: if (currCh>='0' && currCh<='7') {
                        // octal digits
                        char buf[4] = "???";
                        buf[0] = currCh;
                        getNextCh(); buf[1] = currCh;
                        getNextCh(); buf[2] = currCh;
                        tkStr += (char)strtol(buf,0,8);
                    } else
                    tkStr += currCh;
                }
            } else {
                tkStr += currCh;
            }
            getNextCh();
        }
        getNextCh();
        tk = LEX_STR;
    } else {
        // single chars
        tk = (LEX_TYPES)currCh;
        if (currCh) getNextCh();
        if (tk=='=' && currCh=='=') { // ==
            tk = LEX_EQUAL;
            getNextCh();
            if (currCh=='=') { // ===
                tk = LEX_TYPEEQUAL;
                getNextCh();
            }
        } else if (tk=='!' && currCh=='=') { // !=
            tk = LEX_NEQUAL;
            getNextCh();
            if (currCh=='=') { // !==
                tk = LEX_NTYPEEQUAL;
                getNextCh();
            }
        } else if (tk=='<' && currCh=='=') {
            tk = LEX_LEQUAL;
            getNextCh();
        } else if (tk=='<' && currCh=='<') {
            tk = LEX_LSHIFT;
            getNextCh();
            if (currCh=='=') { // <<=
                tk = LEX_LSHIFTEQUAL;
                getNextCh();
            }
        } else if (tk=='>' && currCh=='=') {
            tk = LEX_GEQUAL;
            getNextCh();
        } else if (tk=='>' && currCh=='>') {
            tk = LEX_RSHIFT;
            getNextCh();
            if (currCh=='=') { // >>=
                tk = LEX_RSHIFTEQUAL;
                getNextCh();
            } else if (currCh=='>') { // >>>
                tk = LEX_RSHIFTUNSIGNED;
                getNextCh();
            }
        }  else if (tk=='+' && currCh=='=') {
            tk = LEX_PLUSEQUAL;
            getNextCh();
        }  else if (tk=='-' && currCh=='=') {
            tk = LEX_MINUSEQUAL;
            getNextCh();
        }  else if (tk=='+' && currCh=='+') {
            tk = LEX_PLUSPLUS;
            getNextCh();
        }  else if (tk=='-' && currCh=='-') {
            tk = LEX_MINUSMINUS;
            getNextCh();
        } else if (tk=='&' && currCh=='=') {
            tk = LEX_ANDEQUAL;
            getNextCh();
        } else if (tk=='&' && currCh=='&') {
            tk = LEX_ANDAND;
            getNextCh();
        } else if (tk=='|' && currCh=='=') {
            tk = LEX_OREQUAL;
            getNextCh();
        } else if (tk=='|' && currCh=='|') {
            tk = LEX_OROR;
            getNextCh();
        } else if (tk=='^' && currCh=='=') {
            tk = LEX_XOREQUAL;
            getNextCh();
        }
    }
    /* This isn't quite right yet */
    tokenLastEnd = tokenEnd;
    tokenEnd = dataPos-3;
}
//部分文字列を返す
wString CScriptLex::getSubString(int lastPosition) {
    int lastCharIdx = tokenLastEnd+1;
    if (lastCharIdx < dataEnd) {
        /* save a memory alloc by using our data array to create the
           subistring */
        char old = data[lastCharIdx];
        data[lastCharIdx] = 0;
        wString value = &data[lastPosition];
        data[lastCharIdx] = old;
        return value;
    } else {
        return wString(&data[lastPosition]);
    }
}

//部分語彙を返す
CScriptLex *CScriptLex::getSubLex(int lastPosition) {
    int lastCharIdx = tokenLastEnd+1;
    if (lastCharIdx < dataEnd)
    return new CScriptLex(socket,printed,headerBuf,this, lastPosition, lastCharIdx,prBuffer,prPos);
    else
    return new CScriptLex(socket,printed,headerBuf,this, lastPosition, dataEnd,prBuffer,prPos);
}
//指定位置を行数、列数に変換
wString CScriptLex::getPosition(int pos) {
    if (pos<0) pos=tokenLastEnd;
    int line = 1;
    int col  = 1;
    for (int i=0;i<pos;i++) {
        char ch;
        if (i < dataEnd){
            ch = data[i];
        }else{
            ch = 0;
        }
        col++;
        if (ch=='\n') {
            line++;
            col = 1;
        }
    }
    wString buf;
    buf.sprintf("(line: %d, col: %d)", line, col);
    return buf;
}

// ----------------------------------------------------------------------------------- CSCRIPTVARLINK

CScriptVarLink::CScriptVarLink(CScriptVar *var, const wString &myname) {
    this->name = myname;
    this->nextSibling = 0;
    this->prevSibling = 0;
    this->var         = var->ref();//thisを参照を増やして返す
    this->owned = false;
}

CScriptVarLink::CScriptVarLink(const CScriptVarLink &link) {
    // Copy constructor
    this->name = link.name;
    this->nextSibling = 0;
    this->prevSibling = 0;
    this->var = link.var->ref();
    this->owned = false;
}

CScriptVarLink::~CScriptVarLink() {
    var->unref();
}

void CScriptVarLink::replaceWith(CScriptVar *newVar) {
    CScriptVar *oldVar = var;
    var = newVar->ref();
    oldVar->unref();
}

void CScriptVarLink::replaceWith(CScriptVarLink *newVar) {
    if (newVar)
      replaceWith(newVar->var);
    else
      replaceWith(new CScriptVar());
}
//名前を数値に変換
int CScriptVarLink::getIntName() {
    return atoi(name.c_str());
}
//名前を設定
void CScriptVarLink::setIntName(int n) {
    char sIdx[64];
    snprintf(sIdx, sizeof(sIdx), "%d", n);
    name = sIdx;
}

// ----------------------------------------------------------------------------------- CSCRIPTVAR

CScriptVar::CScriptVar() {
    refs = 0;
    init();
    flags = SCRIPTVAR_UNDEFINED;
}

CScriptVar::CScriptVar(const wString &str) {
    refs = 0;
    init();
    flags = SCRIPTVAR_STRING;
    data = str;
}


CScriptVar::CScriptVar(const wString &varData, int varFlags) {
    refs = 0;
    init();
    flags = varFlags;
    if (varFlags & SCRIPTVAR_INTEGER) {
        intData = strtol(varData.c_str(),0,0);
    } else if (varFlags & SCRIPTVAR_DOUBLE) {
        doubleData = strtod(varData.c_str(),0);
    } else
      data = varData;
}

CScriptVar::CScriptVar(double val) {
    refs = 0;
    init();
    setDouble(val);
}

CScriptVar::CScriptVar(int val) {
    refs = 0;
    init();
    setInt(val);
}
CScriptVar::CScriptVar(bool val){
    refs = 0;
    init();
    setInt(val);
}

CScriptVar::~CScriptVar(void) {
    removeAllChildren();
}

void CScriptVar::init() {
    firstChild = 0;
    lastChild = 0;
    flags = 0;
    jsCallback = 0;
    jsCallbackUserData = 0;
    data = TINYJS_BLANK_DATA;
    intData = 0;
    doubleData = 0;
}

CScriptVar *CScriptVar::getReturnVar() {
    return getParameter(TINYJS_RETURN_VAR);
}

void CScriptVar::setReturnVar(CScriptVar *var) {
    findChildOrCreate(TINYJS_RETURN_VAR)->replaceWith(var);
}


CScriptVar *CScriptVar::getParameter(const wString &name) {
    return findChildOrCreate(name)->var;
}
//親変数で子供が見つかったらlinkを返す。なければ0
CScriptVarLink *CScriptVar::findChild(const wString &childName) {
    CScriptVarLink *v = firstChild;
    while (v) {
        if (v->name.compare(childName)==0)
            return v;
        v = v->nextSibling;
    }
    return 0;
}

CScriptVarLink *CScriptVar::findChildOrCreate(const wString &childName, int varFlags) {
    CScriptVarLink *l = findChild(childName);
    if (l) return l;

    return addChild(childName, new CScriptVar(TINYJS_BLANK_DATA, varFlags));
}

CScriptVarLink *CScriptVar::findChildOrCreateByPath(const wString &path) {
    int p = path.find('.');
    if (p == wString::npos)
    return findChildOrCreate(path);
    
    return findChildOrCreate(path.substr(0,p), SCRIPTVAR_OBJECT)->var->
    findChildOrCreateByPath(path.substr(p+1));
}

CScriptVarLink *CScriptVar::addChild(const wString &childName, CScriptVar *child) {
    if (isUndefined()) {
        flags = SCRIPTVAR_OBJECT;
    }
    // if no child supplied, create one
    if (!child){
      child = new CScriptVar();
    }
      

    CScriptVarLink *link = new CScriptVarLink(child, childName);
    link->owned = true;
    if (lastChild) {
        lastChild->nextSibling = link;
        link->prevSibling = lastChild;
        lastChild = link;
    } else {
        firstChild = link;
        lastChild = link;
    }
    return link;
}

CScriptVarLink *CScriptVar::addChildNoDup(const wString &childName, CScriptVar *child) {
    // if no child supplied, create one
    if (!child)
      child = new CScriptVar();

    CScriptVarLink *v = findChild(childName);
    if (v) {
        v->replaceWith(child);
    } else {
        v = addChild(childName, child);
    }

    return v;
}

void CScriptVar::removeChild(CScriptVar *child) {
    CScriptVarLink *link = firstChild;
    while (link) {
        if (link->var == child){
            break;
        }
        link = link->nextSibling;
    }
    ASSERT(link);
    removeLink(link);
}

void CScriptVar::removeLink(CScriptVarLink *link) {
    if (!link) return;
    if (link->nextSibling){
      link->nextSibling->prevSibling = link->prevSibling;
    }
    if (link->prevSibling){
      link->prevSibling->nextSibling = link->nextSibling;
    }
    if (lastChild == link){
        lastChild = link->prevSibling;
    }
    if (firstChild == link){
        firstChild = link->nextSibling;
    }
    delete link;
}

void CScriptVar::removeAllChildren() {
    CScriptVarLink *c = firstChild;
    while (c) {
        CScriptVarLink *t = c->nextSibling;
        delete c;
        c = t;
    }
    firstChild = 0;
    lastChild = 0;
}

CScriptVar *CScriptVar::getArrayIndex(int idx) {
    char sIdx[64];
    snprintf(sIdx, sizeof(sIdx), "%d", idx);
    CScriptVarLink *link = findChild(sIdx);
    if (link) return link->var;
    else return new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_NULL); // undefined
}

void CScriptVar::setArrayIndex(int idx, CScriptVar *value) {
    char sIdx[64];
    snprintf(sIdx, sizeof(sIdx), "%d", idx);
    CScriptVarLink *link = findChild(sIdx);

    if (link) {
      if (value->isUndefined())
        removeLink(link);
      else
        link->replaceWith(value);
    } else {
      if (!value->isUndefined())
        addChild(sIdx, value);
    }
}

int CScriptVar::getArrayLength() {
    int highest = -1;
    if (!isArray()) return 0;

    CScriptVarLink *link = firstChild;
    while (link) {
      if (isNumber(link->name)) {
        int val = atoi(link->name.c_str());
        if (val > highest) highest = val;
      }
      link = link->nextSibling;
    }
    return highest+1;
}

int CScriptVar::getChildren() {
    int n = 0;
    CScriptVarLink *link = firstChild;
    while (link) {
      n++;
      link = link->nextSibling;
    }
    return n;
}

int CScriptVar::getInt() {
    /* strtol understands about hex and octal */
    if (isInt()) return intData;
    if (isNull()) return 0;
    if (isUndefined()) return 0;
    if (isDouble()) return (int)doubleData;
    return 0;
}

double CScriptVar::getDouble() {
    if (isDouble()) return doubleData;
    if (isInt()) return intData;
    if (isNull()) return 0;
    if (isUndefined()) return 0;
    return 0; /* or NaN? */
}

const wString &CScriptVar::getString() {
    /* Because we can't return a wString that is generated on demand.
     * I should really just use char* :) */
    static wString s_null = "null";
    static wString s_undefined = "undefined";
    if (isInt()) {
        //char buffer[32];
        //snprintf(buffer, sizeof(buffer), "%ld", intData);
        //data = buffer;
        data.sprintf( "%ld", intData );
        return data;
    }
    if (isDouble()) {
        //char buffer[32];
        //snprintf(buffer, sizeof(buffer), "%f", doubleData);
        //data = buffer;
        data.sprintf( "%f", doubleData );
        return data;
    }
    if (isNull()) return s_null;
    if (isUndefined()) return s_undefined;
    // are we just a wString here?
    return data;
}

void CScriptVar::setInt(int val) {
    flags = (flags&~SCRIPTVAR_VARTYPEMASK) | SCRIPTVAR_INTEGER;
    intData = val;
    doubleData = 0;
    data = TINYJS_BLANK_DATA;
}

void CScriptVar::setDouble(double val) {
    flags = (flags&~SCRIPTVAR_VARTYPEMASK) | SCRIPTVAR_DOUBLE;
    doubleData = val;
    intData = 0;
    data = TINYJS_BLANK_DATA;
}

void CScriptVar::setString(const wString &str) {
    // name sure it's not still a number or integer
    flags = (flags&~SCRIPTVAR_VARTYPEMASK) | SCRIPTVAR_STRING;
    data = str;
    intData = 0;
    doubleData = 0;
}

void CScriptVar::setUndefined() {
    // name sure it's not still a number or integer
    flags = (flags&~SCRIPTVAR_VARTYPEMASK) | SCRIPTVAR_UNDEFINED;
    data = TINYJS_BLANK_DATA;
    intData = 0;
    doubleData = 0;
    removeAllChildren();
}

void CScriptVar::setArray() {
    // name sure it's not still a number or integer
    flags = (flags&~SCRIPTVAR_VARTYPEMASK) | SCRIPTVAR_ARRAY;
    data = TINYJS_BLANK_DATA;
    intData = 0;
    doubleData = 0;
    removeAllChildren();
}

bool CScriptVar::equals(CScriptVar *v) {
    CScriptVar *resV = mathsOp(v, LEX_EQUAL);
    bool res = resV->getBool();
    delete resV;
    return res;
}

CScriptVar *CScriptVar::mathsOp(CScriptVar *b, int op) {
    CScriptVar *a = this;
    // Type equality check
    if (op == LEX_TYPEEQUAL || op == LEX_NTYPEEQUAL) {
      // check type first, then call again to check data
      bool eql = ((a->flags & SCRIPTVAR_VARTYPEMASK) ==
                  (b->flags & SCRIPTVAR_VARTYPEMASK));
      if (eql) {
        CScriptVar *contents = a->mathsOp(b, LEX_EQUAL);
        if (!contents->getBool()) eql = false;
        if (!contents->refs) delete contents;
      }
                 ;
      if (op == LEX_TYPEEQUAL)
        return new CScriptVar(eql);
      else
        return new CScriptVar(!eql);
    }
    // do maths...
    if (a->isUndefined() && b->isUndefined()) {
        if (op == LEX_EQUAL)       return new CScriptVar(true);
        else if (op == LEX_NEQUAL) return new CScriptVar(false);
        else return new CScriptVar(); // undefined
    } else if ((a->isNumeric() || a->isUndefined()) &&
               (b->isNumeric() || b->isUndefined())) {
        if (!a->isDouble() && !b->isDouble()) {
            // use ints
            int da = a->getInt();
            int db = b->getInt();
            switch (op) {
                case '+': return new CScriptVar(da+db);
                case '-': return new CScriptVar(da-db);
                case '*': return new CScriptVar(da*db);
                case '/': return new CScriptVar(da/db);
                case '&': return new CScriptVar(da&db);
                case '|': return new CScriptVar(da|db);
                case '^': return new CScriptVar(da^db);
                case '%':           return new CScriptVar(da%db);
                case LEX_EQUAL:     return new CScriptVar(da==db);
                case LEX_NEQUAL:    return new CScriptVar(da!=db);
                case '<':           return new CScriptVar(da<db);
                case LEX_LEQUAL:    return new CScriptVar(da<=db);
                case '>':           return new CScriptVar(da>db);
                case LEX_GEQUAL:    return new CScriptVar(da>=db);
                default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Int datatype");
            }
        } else {
            // use doubles
            double da = a->getDouble();
            double db = b->getDouble();
            switch (op) {
                case '+': return new CScriptVar(da+db);
                case '-': return new CScriptVar(da-db);
                case '*': return new CScriptVar(da*db);
                case '/': return new CScriptVar(da/db);
                case LEX_EQUAL:     return new CScriptVar(da==db);
                case LEX_NEQUAL:    return new CScriptVar(da!=db);
                case '<':           return new CScriptVar(da<db);
                case LEX_LEQUAL:    return new CScriptVar(da<=db);
                case '>':           return new CScriptVar(da>db);
                case LEX_GEQUAL:    return new CScriptVar(da>=db);
                default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Double datatype");
            }
        }
    } else if (a->isArray()) {
        /* Just check pointers */
        switch (op) {
            case LEX_EQUAL: return new CScriptVar(a==b);
            case LEX_NEQUAL: return new CScriptVar(a!=b);
            default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Array datatype");
        }
    } else if (a->isObject()) {
        /* Just check pointers */
        switch (op) {
            case LEX_EQUAL: return new CScriptVar(a==b);
            case LEX_NEQUAL: return new CScriptVar(a!=b);
            default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Object datatype");
        }
    } else {
        wString da = a->getString();
        wString db = b->getString();
        // use strings
        switch (op) {
            case '+':           return new CScriptVar(da+db, SCRIPTVAR_STRING);
            case LEX_EQUAL:     return new CScriptVar(da==db);
            case LEX_NEQUAL:    return new CScriptVar(da!=db);
            case '<':           return new CScriptVar(da<db);
            case LEX_LEQUAL:    return new CScriptVar(da<=db);
            case '>':           return new CScriptVar(da>db);
            case LEX_GEQUAL:    return new CScriptVar(da>=db);
            default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the wString datatype");
        }
    }
    //実行されないコード
    //ASSERT(0);
    //return 0;
}

void CScriptVar::copySimpleData(CScriptVar *val) {
    data = val->data;
    intData = val->intData;
    doubleData = val->doubleData;
    flags = (flags & ~SCRIPTVAR_VARTYPEMASK) | (val->flags & SCRIPTVAR_VARTYPEMASK);
}

void CScriptVar::copyValue(CScriptVar *val) {
    if (val) {
        copySimpleData(val);
        // remove all current children
        removeAllChildren();
        // copy children of 'val'
        CScriptVarLink *child = val->firstChild;
        while (child) {
            CScriptVar *copied;
            // don't copy the 'parent' object...
            if (child->name != TINYJS_PROTOTYPE_CLASS)
            copied = child->var->deepCopy();
            else
            copied = child->var;
            
            addChild(child->name, copied);
            
            child = child->nextSibling;
        }
    } else {
        setUndefined();
    }
}

CScriptVar *CScriptVar::deepCopy() {
    CScriptVar *newVar = new CScriptVar();
    newVar->copySimpleData(this);
    // copy children
    CScriptVarLink *child = firstChild;
    while (child) {
        CScriptVar *copied;
        // don't copy the 'parent' object...
        if (child->name != TINYJS_PROTOTYPE_CLASS)
        copied = child->var->deepCopy();
        else
        copied = child->var;
        
        newVar->addChild(child->name, copied);
        child = child->nextSibling;
    }
    return newVar;
}

void CScriptVar::trace(int socket, wString indentStr, const wString &name) {
    wString work;
    work.wString::sprintf( "%s'%s' = '%s' %s\n",indentStr.c_str(),name.c_str(),getString().c_str(),getFlagsAsString().c_str());
    send( socket, work.c_str(), work.length(),0);
    wString indent = indentStr+" ";
    CScriptVarLink *link = firstChild;
    while (link) {
        link->var->trace(socket,indent, link->name);
        link = link->nextSibling;
    }
}
wString CScriptVar::trace2(void) {
    wString str;
    CScriptVarLink *link = firstChild;
    while (link) {
        str.Add( link->name );
        link = link->nextSibling;
    }
    return str;
}

wString CScriptVar::getFlagsAsString() {
    wString flagstr = "";
    if (flags&SCRIPTVAR_FUNCTION) flagstr += "FUNCTION ";
    if (flags&SCRIPTVAR_OBJECT)   flagstr += "OBJECT ";
    if (flags&SCRIPTVAR_ARRAY)    flagstr += "ARRAY ";
    if (flags&SCRIPTVAR_NATIVE)   flagstr += "NATIVE ";
    if (flags&SCRIPTVAR_DOUBLE)   flagstr += "DOUBLE ";
    if (flags&SCRIPTVAR_INTEGER)  flagstr += "INTEGER ";
    if (flags&SCRIPTVAR_STRING)   flagstr += "STRING ";
    return flagstr;
}

wString CScriptVar::getParsableString() {
    // Numbers can just be put in directly
    if (isNumeric()) {
        return getString();
    }
    if (isFunction()) {
        wString funcStr;
        funcStr += "function (";
        // get list of parameters
        CScriptVarLink *link = firstChild;
        while (link) {
            funcStr += link->name;
            if (link->nextSibling) funcStr += ",";
            link = link->nextSibling;
        }
        // add function body
        funcStr += ") ";
        funcStr += getString();
        return funcStr.c_str();
    }
    // if it is a wString then we quote it
    if (isString()){
        return getJSString(getString());
    }
    if (isNull()){
        return "null";
    }
    return "undefined";
}

void CScriptVar::getJSON(wString &destination, const wString linePrefix) {
    if (isObject()) {
        wString indentedLinePrefix = linePrefix+"  ";
        // children - handle with bracketed list
        destination += "{ \n";
        CScriptVarLink *link = firstChild;
        while (link) {
            destination += indentedLinePrefix;
            destination += getJSString(link->name).c_str();
            destination += " : ";
            link->var->getJSON(destination, indentedLinePrefix);
            link = link->nextSibling;
            if (link) {
                destination  += ",\n";
            }
        }
        destination += "\n";
        destination += linePrefix;
        destination +=  "}";
    } else if (isArray()) {
        wString indentedLinePrefix = linePrefix+"  ";
        destination += "[\n";
        int len = getArrayLength();
        if (len>10000) len=10000; // we don't want to get stuck here!
        
        for (int i=0;i<len;i++) {
            getArrayIndex(i)->getJSON(destination, indentedLinePrefix);
            if (i<len-1) destination  += ",\n";
        }

        destination += "\n";
        destination += linePrefix;
        destination += "]";
    } else {
        // no children or a function... just write value directly
        destination += getParsableString();
    }
}


void CScriptVar::setCallback(JSCallback callback, void *userdata) {
    jsCallback = callback;
    jsCallbackUserData = userdata;
}

CScriptVar *CScriptVar::ref() {
    refs++;
    return this;
}

void CScriptVar::unref() {
    if (refs<=0) printf("OMFG, we have unreffed too far!\n");
    if ((--refs)==0) {
        delete this;
    }
}

int CScriptVar::getRefs() {
    return refs;
}


// ----------------------------------------------------------------------------------- CSCRIPT

CTinyJS::CTinyJS(int mysocket) {
    l = 0;
    socket = mysocket;
    root = (new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT))->ref();
    // Add built-in classes
    stringClass = (new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT))->ref();
    arrayClass  = (new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT))->ref();
    objectClass = (new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT))->ref();
    root->addChild("String", stringClass);
    root->addChild("Array", arrayClass);
    root->addChild("Object", objectClass);
    printed = 0;
    headerBuf = new wString();
    prBuffer.clear();
    prPos = -1;

}

CTinyJS::~CTinyJS() {
    ASSERT(!l);
    scopes.clear();
    stringClass->unref();
    arrayClass->unref();
    objectClass->unref();
    root->unref();
    delete headerBuf;
}

void CTinyJS::trace(int sock) {
    root->trace(sock);
}
void CTinyJS::FlushBuf(void)
{
    //出力すべきデータがない時にHeader出さないと再送されてループする
    headerCheckPrint(socket, &printed, headerBuf, 1);
    //残りのバッファ出力
    if( prBuffer.length() > 0 ){
        if( l->dataPos>=prPos){
            send( socket, prBuffer.c_str(),prBuffer.length(),0);
            prBuffer.clear();
            prPos = -1;
        }
    }

}
void CTinyJS::execute(const wString &code, int executeMode) {
    //退避する
    CScriptLex *oldLex = l;
    std::vector<CScriptVar*> oldScopes = scopes;
    l = new CScriptLex(socket,&printed, headerBuf, code, executeMode,&prBuffer,&prPos);
    #ifdef TINYJS_CALL_STACK
    //call_stack.clear();
    #endif
    scopes.clear();
    scopes.push_back(root);
    try {
        bool execute = true;
        while (l->tk) {
            LEX_TYPES ret = statement(execute);
            if( ret != LEX_EOF ){
                wString errorString;
                errorString.sprintf( "Syntax error at %s: %s",l->getPosition(l->tokenStart).c_str(),l->getTokenStr(ret).c_str());
                throw new CScriptException(errorString.c_str());
            }
        }
        //他ではフラッシュしない
        if( executeMode == 0 ){
            FlushBuf();
            //SESSION保存
            wString id = evaluate("JSSESSID");
            if( id != "undefined" ){
                wString se = evaluate("JSON.stringify(_SESSION,0);");
                (*session)[id] = se;
            }
        }
        
    } catch (CScriptException *e) {
        wString msg;
        msg.sprintf("Error at %s: %s",l->getPosition().c_str(),e->text.c_str());;
        //msg +=  e->text;
        #ifdef TINYJS_CALL_STACK
        for (int i=(int)call_stack.size()-1;i>=0;i--){
            msg.cat_sprintf("\n%d: %s",i,call_stack.at(i).c_str());
        }
        #endif
        delete l;
        l = oldLex;

        throw new CScriptException(msg.c_str());
    }
    delete l;
    //復帰する
    l = oldLex;
    scopes = oldScopes;
}
//複合式
CScriptVarLink CTinyJS::evaluateComplex(const wString &code) {
    CScriptLex *oldLex = l;
    std::vector<CScriptVar*> oldScopes = scopes;
    
    l = new CScriptLex(socket, &printed, headerBuf, code,1,&prBuffer,&prPos);
    #ifdef TINYJS_CALL_STACK
    //call_stack.clear();
    #endif
    scopes.clear();
    scopes.push_back(root);
    CScriptVarLink *v = 0;
    try {
        bool execute = true;
        do {
            CLEAN(v);
            v = base(execute);
          if (l->tk!=LEX_EOF) l->chkread(';');
        } while (l->tk!=LEX_EOF);
    } catch (CScriptException *e) {
        wString msg;
        msg.sprintf( "Error at %s: %s",l->getPosition().c_str(), e->text.c_str() );
        #ifdef TINYJS_CALL_STACK
        for (int i=(int)call_stack.size()-1;i>=0;i--){
            msg.cat_sprintf("\n%d: %s",i,call_stack.at(i).c_str());
        }
        #endif
        delete l;
        l = oldLex;
        
        throw new CScriptException(msg.c_str());
    }
    delete l;
    l = oldLex;
    scopes = oldScopes;
    
    if (v) {
        CScriptVarLink r = *v;
        CLEAN(v);
        return r;
    }
    // return undefined...
    return CScriptVarLink(new CScriptVar());
}
//式の評価
wString CTinyJS::evaluate(const wString &code) {
    return evaluateComplex(code).var->getString();
}

void CTinyJS::parseFunctionArguments(CScriptVar *funcVar) {
  l->chkread('(');
    while (l->tk!=')') {
        funcVar->addChildNoDup(l->tkStr);
      l->chkread(LEX_ID);
      if (l->tk!=')') l->chkread(',');
    }
  l->chkread(')');
}
//Cで実装されたコードの実行
void CTinyJS::addNative(const wString &funcDesc, JSCallback ptr, void *userdata) {
    CScriptLex *oldLex = l;
    l = new CScriptLex(socket, &printed, headerBuf, funcDesc,1,&prBuffer,&prPos);
    
    CScriptVar *base = root;

    l->chkread(LEX_R_FUNCTION);
    wString funcName = l->tkStr;
    l->chkread(LEX_ID);
    /* Check for dots, we might want to do something like function String.subwString ... */
    while (l->tk == '.') {
      l->chkread('.');
        CScriptVarLink *link = base->findChild(funcName);
        // if it doesn't exist, make an object class
        if (!link) link = base->addChild(funcName, new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT));
        base = link->var;
        funcName = l->tkStr;
      l->chkread(LEX_ID);
    }
    
    CScriptVar *funcVar = new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_FUNCTION | SCRIPTVAR_NATIVE);
    funcVar->setCallback(ptr, userdata);
    parseFunctionArguments(funcVar);
    delete l;
    l = oldLex;
    
    base->addChild(funcName, funcVar);
}

CScriptVarLink *CTinyJS::parseFunctionDefinition() {
    // actually parse a function...
    l->chkread(LEX_R_FUNCTION);
    wString funcName = TINYJS_TEMP_NAME;
    /* we can have functions without names */
    if (l->tk==LEX_ID) {
        funcName = l->tkStr;
        l->chkread(LEX_ID);
    }
    CScriptVarLink *funcVar = new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_FUNCTION), funcName);
    parseFunctionArguments(funcVar->var);
    int funcBegin = l->tokenStart;
    bool noexecute = false;
    block(noexecute);
    funcVar->var->data = l->getSubString(funcBegin);
    return funcVar;
}

/** Handle a function call (assumes we've parsed the function name and we're
* on the start bracket). 'parent' is the object that contains this method,
* if there was one (otherwise it's just a normnal function).
*/
CScriptVarLink *CTinyJS::functionCall(bool &execute, CScriptVarLink *function, CScriptVar *parent) {
    if (execute) {
        if (!function->var->isFunction()) {
            wString errorMsg = "Expecting '" + function->name + "' to be a function";
            throw new CScriptException(errorMsg.c_str());
        }
    l->chkread('(');
        // create a new symbol table entry for execution of this function
        CScriptVar *functionRoot = new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_FUNCTION);
        if (parent)
        functionRoot->addChildNoDup("this", parent);
        // grab in all parameters
        CScriptVarLink *v = function->var->firstChild;
        while (v) {
            CScriptVarLink *value = base(execute);
            if (execute) {
                if (value->var->isBasic()) {
                    // pass by value
                    functionRoot->addChild(v->name, value->var->deepCopy());
                } else {
                    // pass by reference
                    functionRoot->addChild(v->name, value->var);
                }
            }
            CLEAN(value);
            if (l->tk!=')') l->chkread(',');
            v = v->nextSibling;
        }
        l->chkread(')');
        // setup a return variable
        CScriptVarLink *returnVar = NULL;
        // execute function!
        // add the function's execute space to the symbol table so we can recurse
        CScriptVarLink *returnVarLink = functionRoot->addChild(TINYJS_RETURN_VAR);
        scopes.push_back(functionRoot);
        #ifdef TINYJS_CALL_STACK
        call_stack.push_back(function->name + " from " + l->getPosition());
        #endif

        if (function->var->isNative()) {
            ASSERT(function->var->jsCallback);
            function->var->jsCallback(functionRoot, function->var->jsCallbackUserData);
        } else {
            /* we just want to execute the block, but something could
            * have messed up and left us with the wrong ScriptLex, so
            * we want to be careful here... */
            CScriptException *exception = 0;
            CScriptLex *oldLex = l;
            CScriptLex *newLex = new CScriptLex(socket, &printed, headerBuf, function->var->getString(),1,&prBuffer,&prPos);
            l = newLex;
            try {
                block(execute);
                // because return will probably have called this, and set execute to false
                execute = true;
            } catch (CScriptException *e) {
                exception = e;
            }
            delete newLex;
            l = oldLex;

            if (exception){
                throw exception;
            }
        }
        #ifdef TINYJS_CALL_STACK
        if (!call_stack.empty()) call_stack.pop_back();
        #endif
        scopes.pop_back();
        /* get the real return var before we remove it from our function */
        //resource leak
        returnVar = new CScriptVarLink(returnVarLink->var);
        functionRoot->removeLink(returnVarLink);
        delete functionRoot;
        if (returnVar)  return returnVar;
        else            return new CScriptVarLink(new CScriptVar());
    } else {
        // function, but not executing - just parse args and be done
        l->chkread('(');
        while (l->tk != ')') {
            CScriptVarLink *value = base(execute);
            CLEAN(value);
            if (l->tk!=')') l->chkread(',');
        }
        l->chkread(')');
        if (l->tk == '{') { // TODO: why is this here?
            block(execute);
        }
        /* function will be a blank scriptvarlink if we're not executing,
        * so just return it rather than an alloc/free */
        return function;
    }
}

CScriptVarLink *CTinyJS::factor(bool &execute) {
    if (l->tk=='(') {
        l->chkread('(');
        CScriptVarLink *a = base(execute);
        l->chkread(')');
        return a;
    }
    if (l->tk==LEX_R_TRUE) {
        l->chkread(LEX_R_TRUE);
        return new CScriptVarLink(new CScriptVar(1));
    }
    if (l->tk==LEX_R_FALSE) {
        l->chkread(LEX_R_FALSE);
        return new CScriptVarLink(new CScriptVar(0));
    }
    if (l->tk==LEX_R_NULL) {
        l->chkread(LEX_R_NULL);
        return new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA,SCRIPTVAR_NULL));
    }
    if (l->tk==LEX_R_UNDEFINED) {
        l->chkread(LEX_R_UNDEFINED);
        return new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA,SCRIPTVAR_UNDEFINED));
    }
    if (l->tk==LEX_ID) {
        CScriptVarLink *a = execute ? findInScopes(l->tkStr) : new CScriptVarLink(new CScriptVar());
        //printf("0x%08X for %s at %s\n", (unsigned int)a, l->tkStr.c_str(), l->getPosition().c_str());
        /* The parent if we're executing a method call */
        CScriptVar *parent = 0;
        
        void* alone = NULL;
        if (execute && !a) {
            /* Variable doesn't exist! JavaScript says we should create it
            * (we won't add it here. This is done in the assignment operator)*/
            a = new CScriptVarLink(new CScriptVar(), l->tkStr);
          alone = (void*)a;
        }
        l->chkread(LEX_ID);
        while (l->tk=='(' || l->tk=='.' || l->tk=='[') {
            if (l->tk=='(') { // ------------------------------------- Function Call
                a = functionCall(execute, a, parent);
            } else if (l->tk == '.') { // ------------------------------------- Record Access
                l->chkread('.');
                if (execute) {
                    int aa = 0;
                    const wString &name = l->tkStr;
                    CScriptVarLink *child = a->var->findChild(name);
                    if (!child) child = findInParentClasses(a->var, name);
                    if (!child) {
                        /* if we haven't found this defined yet, use the built-in
                        'length' properly */
                        if (a->var->isArray() && name == "length") {
                            int ll = a->var->getArrayLength();
                            //aa = 1;
                            child = new CScriptVarLink(new CScriptVar(ll));
                        } else if (a->var->isString() && name == "length") {
                            int ll = a->var->getString().size();
                            //aa = 1;
                            child = new CScriptVarLink(new CScriptVar(ll));
                        } else {
                            child = a->var->addChild(name);
                        }
                    }
                    parent = a->var;
                    //不明な変数にchildを作らない                  
                    if( a == alone){
                        wString errorMsg = "Object variable not defined '";
                        errorMsg = errorMsg + a->name + "' must be defined";
                        throw new CScriptException(errorMsg.c_str());
                    }
                    //多分aをCLEANしないとメモリーリーク ひでえ実装
                    if( aa == 1 ){
                        CLEAN(a);
                    }
                    a = child;
                }
                l->chkread(LEX_ID);
            } else if (l->tk == '[') { // ------------------------------------- Array Access
                l->chkread('[');
                CScriptVarLink *index = base(execute);
                l->chkread(']');
                if (execute) {
                    CScriptVarLink *child = a->var->findChildOrCreate(index->var->getString());
                    parent = a->var;
                    a = child;
                }
                CLEAN(index);
            } else ASSERT(0);
        }
        return a;
    }
    if (l->tk==LEX_INT || l->tk==LEX_FLOAT) {
        CScriptVar *a = new CScriptVar(l->tkStr,
        ((l->tk==LEX_INT)?SCRIPTVAR_INTEGER:SCRIPTVAR_DOUBLE));
        l->chkread(l->tk);
        return new CScriptVarLink(a);
    }
    if (l->tk==LEX_STR) {
        CScriptVar *a = new CScriptVar(l->tkStr, SCRIPTVAR_STRING);
        l->chkread(LEX_STR);
        return new CScriptVarLink(a);
    }
    if (l->tk=='{') {
        CScriptVar *contents = new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT);
        /* JSON-style object definition */
        l->chkread('{');
            while (l->tk != '}') {
                wString id = l->tkStr;
                // we only allow strings or IDs on the left hand side of an initialisation
          if (l->tk==LEX_STR) l->chkread(LEX_STR);
          else l->chkread(LEX_ID);
          l->chkread(':');
                if (execute) {
                    CScriptVarLink *a = base(execute);
                    contents->addChild(id, a->var);
                    CLEAN(a);
                }
                // no need to clean here, as it will definitely be used
          if (l->tk != '}') l->chkread(',');
        }
        
        l->chkread('}');
    return new CScriptVarLink(contents);
}
if (l->tk=='[') {
    CScriptVar *contents = new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_ARRAY);
    /* JSON-style array */
        l->chkread('[');
    int idx = 0;
    while (l->tk != ']') {
        if (execute) {
            char idx_str[16]; // big enough for 2^32
            snprintf(idx_str, sizeof(idx_str), "%d",idx);
            
            CScriptVarLink *a = base(execute);
            contents->addChild(idx_str, a->var);
            CLEAN(a);
        }
        // no need to clean here, as it will definitely be used
          if (l->tk != ']') l->chkread(',');
        idx++;
    }
        l->chkread(']');
    return new CScriptVarLink(contents);
}
if (l->tk==LEX_R_FUNCTION) {
    CScriptVarLink *funcVar = parseFunctionDefinition();
    if (funcVar->name != TINYJS_TEMP_NAME)
    JSTRACE(socket,"Functions not defined at statement-level are not meant to have a name");
    return funcVar;
}
if (l->tk==LEX_R_NEW) {
    // new -> create a new object
      l->chkread(LEX_R_NEW);
    const wString &className = l->tkStr;
    if (execute) {
        CScriptVarLink *objClassOrFunc = findInScopes(className);
        if (!objClassOrFunc) {
            JSTRACE(socket,"%s is not a valid class name", className.c_str());
            return new CScriptVarLink(new CScriptVar());
        }
        l->chkread(LEX_ID);
        CScriptVar *obj = new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT);
        CScriptVarLink *objLink = new CScriptVarLink(obj);
        if (objClassOrFunc->var->isFunction()) {
            CLEAN(functionCall(execute, objClassOrFunc, obj));
        } else {
            obj->addChild(TINYJS_PROTOTYPE_CLASS, objClassOrFunc->var);
            if (l->tk == '(') {
                l->chkread('(');
                l->chkread(')');
            }
        }
        return objLink;
    } else {
        l->chkread(LEX_ID);
        if (l->tk == '(') {
            l->chkread('(');
            l->chkread(')');
        }
    }
}
// Nothing we can do here... just hope it's the end...
    l->chkread(LEX_EOF);
return 0;
}
//単項演算子!
CScriptVarLink *CTinyJS::unary(bool &execute) {
    CScriptVarLink *a;
    if (l->tk=='!') {
        l->chkread('!'); // binary not
        a = factor(execute);
        if (execute) {
            CScriptVar zero(0);
            CScriptVar *res = a->var->mathsOp(&zero, LEX_EQUAL);
            CREATE_LINK(a, res);
        }
    } else {
        a = factor(execute);
    }
    return a;
}

CScriptVarLink *CTinyJS::term(bool &execute) {
    CScriptVarLink *a = unary(execute);
    while (l->tk=='*' || l->tk=='/' || l->tk=='%') {
        int op = l->tk;
        l->chkread(l->tk);
        CScriptVarLink *b = unary(execute);
        if (execute) {
            CScriptVar *res = a->var->mathsOp(b->var, op);
            CREATE_LINK(a, res);
        }
        CLEAN(b);
    }
    return a;
}
////////////////////////////////////////////////////////////////////////////////
//表現(-a++とか)
CScriptVarLink *CTinyJS::expression(bool &execute) {
    bool negate = false;
    if (l->tk=='-') {
        l->chkread('-');
        negate = true;
    }
    CScriptVarLink *a = term(execute);
    if (negate) {
        CScriptVar zero(0);
        CScriptVar *res = zero.mathsOp(a->var, '-');
        CREATE_LINK(a, res);
    }
    
    while (l->tk=='+' || l->tk=='-' ||
    l->tk==LEX_PLUSPLUS || l->tk==LEX_MINUSMINUS) {
        int op = l->tk;
        l->chkread(l->tk);
        if (op==LEX_PLUSPLUS || op==LEX_MINUSMINUS) {
            if (execute) {
                CScriptVar one(1);
                CScriptVar *res = a->var->mathsOp(&one, op==LEX_PLUSPLUS ? '+' : '-');
                CScriptVarLink *oldValue = new CScriptVarLink(a->var);
                // in-place add/subtract
                a->replaceWith(res);
                CLEAN(a);
                a = oldValue;
            }
        } else {
            CScriptVarLink *b = term(execute);
            if (execute) {
                // not in-place, so just replace
                CScriptVar *res = a->var->mathsOp(b->var, op);
                CREATE_LINK(a, res);
            }
            CLEAN(b);
        }
    }
    return a;
}
////////////////////////////////////////////////////////////////////////////////
//シフト演算子
CScriptVarLink *CTinyJS::shift(bool &execute) {
    CScriptVarLink *a = expression(execute);
    if (l->tk==LEX_LSHIFT || l->tk==LEX_RSHIFT || l->tk==LEX_RSHIFTUNSIGNED) {
        int op = l->tk;
        l->chkread(op);
        CScriptVarLink *b = base(execute);
        int shift = execute ? b->var->getInt() : 0;
        CLEAN(b);
        if (execute) {
            if      (op==LEX_LSHIFT) a->var->setInt(a->var->getInt() << shift);
            else if (op==LEX_RSHIFT)         a->var->setInt(a->var->getInt() >> shift);
            else if (op==LEX_RSHIFTUNSIGNED) a->var->setInt(((unsigned int)a->var->getInt()) >> shift);
        }
    }
    return a;
}
////////////////////////////////////////////////////////////////////////////////
//条件式
CScriptVarLink *CTinyJS::condition(bool &execute) {
    CScriptVarLink *a = shift(execute);
    CScriptVarLink *b;
    while (l->tk==LEX_EQUAL || l->tk==LEX_NEQUAL ||
    l->tk==LEX_TYPEEQUAL || l->tk==LEX_NTYPEEQUAL ||
    l->tk==LEX_LEQUAL || l->tk==LEX_GEQUAL ||
    l->tk=='<' || l->tk=='>') {
        int op = l->tk;
        l->chkread(l->tk);
        b = shift(execute);
        if (execute) {
            CScriptVar *res = a->var->mathsOp(b->var, op);
            CREATE_LINK(a,res);
        }
        CLEAN(b);
    }
    return a;
}
////////////////////////////////////////////////////////////////////////////////
//結合条件式
CScriptVarLink *CTinyJS::logic(bool &execute) {
    CScriptVarLink *a = condition(execute);
    CScriptVarLink *b;
    while (l->tk=='&' || l->tk=='|' || l->tk=='^' || l->tk==LEX_ANDAND || l->tk==LEX_OROR) {
        bool noexecute = false;
        int op = l->tk;
        l->chkread(l->tk);
        bool shortCircuit = false;
        bool boolean = false;
        // if we have short-circuit ops, then if we know the outcome
        // we don't bother to execute the other op. Even if not
        // we need to tell mathsOp it's an & or |
        if (op==LEX_ANDAND) {
            op = '&';
            shortCircuit = !a->var->getBool();
            boolean = true;
        } else if (op==LEX_OROR) {
            op = '|';
            shortCircuit = a->var->getBool();
            boolean = true;
        }
        b = condition(shortCircuit ? noexecute : execute);
        if (execute && !shortCircuit) {
            if (boolean) {
                CScriptVar *newa = new CScriptVar(a->var->getBool());
                CScriptVar *newb = new CScriptVar(b->var->getBool());
                CREATE_LINK(a, newa);
                CREATE_LINK(b, newb);
            }
            CScriptVar *res = a->var->mathsOp(b->var, op);
            CREATE_LINK(a, res);
        }
        CLEAN(b);
    }
    return a;
}
////////////////////////////////////////////////////////////////////////////////
//三項演算子
CScriptVarLink *CTinyJS::ternary(bool &execute) {
    CScriptVarLink *lhs = logic(execute);
    bool noexec = false;
    if (l->tk=='?') {
        l->chkread('?');
        if (!execute) {
            CLEAN(lhs);
            CLEAN(base(noexec));
            l->chkread(':');
            CLEAN(base(noexec));
        } else {
            bool first = lhs->var->getBool();
            CLEAN(lhs);
            if (first) {
                lhs = base(execute);
                l->chkread(':');
                CLEAN(base(noexec));
            } else {
                CLEAN(base(noexec));
                l->chkread(':');
                lhs = base(execute);
            }
        }
    }
    return lhs;
}
////////////////////////////////////////////////////////////////////////////////
//a=1、a+=1、a-=等
CScriptVarLink *CTinyJS::base(bool &execute) {
    CScriptVarLink *lhs = ternary(execute);
    if (l->tk=='=' || l->tk==LEX_PLUSEQUAL || l->tk==LEX_MINUSEQUAL) {
        /* If we're assigning to this and we don't have a parent,
        * add it to the symbol table root as per JavaScript. */
        if (execute && !lhs->owned) {
            if (lhs->name.length()>0) {
                CScriptVarLink *realLhs = root->addChildNoDup(lhs->name, lhs->var);
                CLEAN(lhs);
                lhs = realLhs;
            } else
            JSTRACE(socket,"Trying to assign to an un-named type\n");
        }
        
        int op = l->tk;
        l->chkread(l->tk);
        CScriptVarLink *rhs = base(execute);
        if (execute) {
            if (op=='=') {
                lhs->replaceWith(rhs);
            } else if (op==LEX_PLUSEQUAL) {
                CScriptVar *res = lhs->var->mathsOp(rhs->var, '+');
                lhs->replaceWith(res);
            } else if (op==LEX_MINUSEQUAL) {
                CScriptVar *res = lhs->var->mathsOp(rhs->var, '-');
                lhs->replaceWith(res);
            } else ASSERT(0);
        }
        CLEAN(rhs);
    }
    return lhs;
}
//execute==trueならblock内を実施
LEX_TYPES CTinyJS::block(bool &execute) {
    LEX_TYPES ret=LEX_EOF;
    l->chkread('{');
        if (execute) {
            while (l->tk && l->tk!='}'){
                ret = statement(execute);
                //この場合のみ末尾まで読み飛ばし
                if( ret == LEX_R_BREAK || ret == LEX_R_CONTINUE){
                    //末尾まで読み飛ばし
                    int brackets = 1;
                    while (l->tk && brackets) {
                        if (l->tk == '{') brackets++;
                        if (l->tk == '}') brackets--;
                        l->chkread(l->tk);
                    }
                    return ret;
                }
            }
      l->chkread('}');
    } else {
        // fast skip of blocks
        int brackets = 1;
        while (l->tk && brackets) {
            if (l->tk == '{') brackets++;
            if (l->tk == '}') brackets--;
        l->chkread(l->tk);
        }
    }
    return ret;
}
//記述
LEX_TYPES  CTinyJS::statement(bool &execute) {
    LEX_TYPES ret;
    if (l->tk==LEX_ID ||
    l->tk==LEX_INT ||
    l->tk==LEX_FLOAT ||
    l->tk==LEX_STR ||
    l->tk=='-') {
        /* Execute a simple statement that only contains basic arithmetic... */
        CLEAN(base(execute));
        l->chkread(';');
    } else if (l->tk=='{') {
        /* A block of code */
        ret = block(execute);
        //単なるreturnでいいのでは？
        if( ret == LEX_R_BREAK || ret == LEX_R_CONTINUE){
            return ret;
        }
    } else if (l->tk==';') {
        /* Empty statement - to allow things like ;;; */
        l->chkread(';');
    } else if (l->tk==LEX_R_BREAK){
        l->chkread(LEX_R_BREAK);
        l->chkread(';');
        return LEX_R_BREAK;
    } else if (l->tk==LEX_R_CONTINUE){
        l->chkread(LEX_R_CONTINUE);
        l->chkread(';');
        return LEX_R_CONTINUE;
    } else if (l->tk==LEX_R_VAR) {
        /* variable creation. TODO - we need a better way of parsing the left
        * hand side. Maybe just have a flag called can_create_var that we
        * set and then we parse as if we're doing a normal equals.*/
        l->chkread(LEX_R_VAR);
        while (l->tk != ';') {
            CScriptVarLink *a = 0;
            if (execute)
            a = scopes.back()->findChildOrCreate(l->tkStr);
          l->chkread(LEX_ID);
            // now do stuff defined with dots
            while (l->tk == '.') {
              l->chkread('.');
                if (execute) {
                    CScriptVarLink *lastA = a;
                    a = lastA->var->findChildOrCreate(l->tkStr);
                }
              l->chkread(LEX_ID);
            }
            // sort out initialiser
            if (l->tk == '=') {
              l->chkread('=');
                CScriptVarLink *var = base(execute);
                if (execute){
                    a->replaceWith(var);
                }
                CLEAN(var);
            }
            if (l->tk != ';')
            l->chkread(',');
        }
        l->chkread(';');
        //IF
    } else if (l->tk==LEX_R_IF) {
        l->chkread(LEX_R_IF);
        l->chkread('(');
        CScriptVarLink *var = base(execute);
        l->chkread(')');
        bool cond = execute && var->var->getBool();
        CLEAN(var);
        bool noexecute = false; // because we need to be able to write to it
        ret = statement(cond ? execute : noexecute);
        if( ret == LEX_R_BREAK || ret == LEX_R_CONTINUE){
            return ret;
        }
        if (l->tk==LEX_R_ELSE) {
            l->chkread(LEX_R_ELSE);
            //break continue対応. LEX_R_BREAK,LEX_R_CONTINUE以外ではLEX_EOFがかえる
            return statement(cond ? noexecute : execute);
        }
        //WHILE
    } else if (l->tk==LEX_R_WHILE) {
        // We do repetition by pulling out the wString representing our statement
        // there's definitely some opportunity for optimisation here
        l->chkread(LEX_R_WHILE);
        l->chkread('(');
        int whileCondStart = l->tokenStart;
        bool noexecute = false;
        CScriptVarLink *cond = base(execute);
        bool loopCond = execute && cond->var->getBool();
        CLEAN(cond);
        CScriptLex *whileCond = l->getSubLex(whileCondStart);
        l->chkread(')');
        int whileBodyStart = l->tokenStart;
        ret = statement(loopCond ? execute : noexecute);
        if( ret != LEX_EOF ){
            wString errorString;
            errorString.sprintf( "Syntax error at %s: %s",l->getPosition(l->tokenStart).c_str(),l->getTokenStr(ret).c_str());
            throw new CScriptException(errorString.c_str());
        }
        CScriptLex *whileBody = l->getSubLex(whileBodyStart);
        CScriptLex *oldLex = l;
        int loopCount = TINYJS_LOOP_MAX_ITERATIONS;
        while (loopCond && loopCount-->0) {
            whileCond->reset();
            l = whileCond;
            cond = base(execute);
            loopCond = execute && cond->var->getBool();
            CLEAN(cond);
            if (loopCond) {
                whileBody->reset();
                l = whileBody;
                ret = statement(execute);
                if( ret == LEX_R_BREAK ){
                    break;
                }
                if( ret == LEX_R_CONTINUE ){
                    continue;
                }
            }
        }
        l = oldLex;
        delete whileCond;
        delete whileBody;
        
        if (loopCount<=0) {
            root->trace(socket);
            JSTRACE(socket,"WHILE Loop exceeded %d iterations at %s\n", TINYJS_LOOP_MAX_ITERATIONS, l->getPosition().c_str());
            throw new CScriptException("LOOP_ERROR");
        }
    } else if (l->tk==LEX_R_FOR) {
        l->chkread(LEX_R_FOR);
        l->chkread('(');
        ret = statement(execute); // initialisation
        if( ret > LEX_EOF ){
            wString errorString;
            errorString.sprintf( "Syntax error at %s: %s",l->getPosition(l->tokenStart).c_str(),l->getTokenStr(ret).c_str());
            throw new CScriptException(errorString.c_str());
        }
        //l->chkread(';');
        int forCondStart = l->tokenStart;
        bool noexecute = false;
        CScriptVarLink *cond = base(execute); // condition
        bool loopCond = execute && cond->var->getBool();
        CLEAN(cond);
        CScriptLex *forCond = l->getSubLex(forCondStart);
        l->chkread(';');
        int forIterStart = l->tokenStart;
        CLEAN(base(noexecute)); // iterator
        CScriptLex *forIter = l->getSubLex(forIterStart);
        l->chkread(')');
        int forBodyStart = l->tokenStart;
        ret = statement(loopCond ? execute : noexecute);
        if( ret == LEX_R_BREAK || ret == LEX_R_CONTINUE ){
            wString errorString;
            errorString.sprintf( "Syntax error at %s: %s ",l->getPosition(l->tokenStart).c_str(),l->getTokenStr(ret).c_str());
            throw new CScriptException(errorString.c_str());
        }
        CScriptLex *forBody = l->getSubLex(forBodyStart);
        CScriptLex *oldLex = l;
        if (loopCond) {
            forIter->reset();
            l = forIter;
            CLEAN(base(execute));
        }
        int loopCount = TINYJS_LOOP_MAX_ITERATIONS;
        while (execute && loopCond && loopCount-->0) {
            forCond->reset();
            l = forCond;
            cond = base(execute);
            loopCond = cond->var->getBool();
            CLEAN(cond);
            if (execute && loopCond) {
                forBody->reset();
                l = forBody;
                ret = statement(execute);
                if( ret == LEX_R_BREAK ){
                    break;
                }
                if( ret == LEX_R_CONTINUE ){
                    continue;
                }
            }
            if (execute && loopCond) {
                forIter->reset();
                l = forIter;
                CLEAN(base(execute));
            }
        }
        l = oldLex;
        delete forCond;
        delete forIter;
        delete forBody;
        if (loopCount<=0) {
            root->trace(socket);
            JSTRACE(socket,"FOR Loop exceeded %d iterations at %s\n", TINYJS_LOOP_MAX_ITERATIONS, l->getPosition().c_str());
            throw new CScriptException("LOOP_ERROR");
        }
    } else if (l->tk==LEX_R_RETURN) {
        l->chkread(LEX_R_RETURN);
        CScriptVarLink *result = 0;
        if (l->tk != ';')
        result = base(execute);
        if (execute) {
            CScriptVarLink *resultVar = scopes.back()->findChild(TINYJS_RETURN_VAR);
            if (resultVar)
            resultVar->replaceWith(result);
            else
            JSTRACE(socket,"RETURN statement, but not in a function.\n");
            execute = false;
        }
        CLEAN(result);
        l->chkread(';');
    } else if (l->tk==LEX_R_FUNCTION) {
        CScriptVarLink *funcVar = parseFunctionDefinition();
        if (execute) {
            if (funcVar->name == TINYJS_TEMP_NAME)
            JSTRACE(socket,"Functions defined at statement-level are meant to have a name\n");
            else
            scopes.back()->addChildNoDup(funcVar->name, funcVar->var);
        }
        CLEAN(funcVar);
    } else {
        l->chkread(LEX_EOF);
    }
    return LEX_EOF;
}

/// Get the given variable specified by a path (var1.var2.etc), or return 0
CScriptVar *CTinyJS::getScriptVariable(const wString &path) {
    // traverse path
    size_t prevIdx = 0;
    int thisIdx = path.find('.');
    if (thisIdx == wString::npos){
        thisIdx = path.length();
    }
    CScriptVar *var = root;
    while (var && prevIdx<path.length()) {
        //着目している変数の最初の方
        wString el = path.substr(prevIdx, thisIdx-prevIdx);
        CScriptVarLink *varl = var->findChild(el);
        var = varl?varl->var:0;
        prevIdx = thisIdx+1;
        thisIdx = path.find('.', prevIdx);
        if (thisIdx == wString::npos) thisIdx = path.length();
    }
    return var;
}

/// Get the value of the given variable, or return 0
//const wString *CTinyJS::getVariable(const wString &path) {
//    CScriptVar *var = getScriptVariable(path);
//    // return result
//    if (var){
//        return &var->getString();
//    }else{
//        return 0;
//    }
//}

/// set the value of the given variable, return trur if it exists and gets set
bool CTinyJS::setVariable(const wString &path, const wString &varData) {
    CScriptVar *var = getScriptVariable(path);
    // return result
    if (var) {
        if (var->isInt())
        var->setInt((int)strtol(varData.c_str(),0,0));
        else if (var->isDouble())
        var->setDouble(strtod(varData.c_str(),0));
        else
        var->setString(varData.c_str());
        return true;
    }else{
        return false;
    }
}

/// Finds a child, looking recursively up the scopes
CScriptVarLink *CTinyJS::findInScopes(const wString &childName) {
    for (int s=scopes.size()-1;s>=0;s--) {
        CScriptVarLink *v = scopes[s]->findChild(childName);
        if (v) return v;
    }
    return NULL;

}

/// Look up in any parent classes of the given object
CScriptVarLink *CTinyJS::findInParentClasses(CScriptVar *object, const wString &name) {
    // Look for links to actual parent classes
    CScriptVarLink *parentClass = object->findChild(TINYJS_PROTOTYPE_CLASS);
    while (parentClass) {
        CScriptVarLink *implementation = parentClass->var->findChild(name);
        if (implementation) return implementation;
        parentClass = parentClass->var->findChild(TINYJS_PROTOTYPE_CLASS);
    }
    // else fake it for strings and finally objects
    if (object->isString()) {
        CScriptVarLink *implementation = stringClass->findChild(name);
        if (implementation) return implementation;
    }
    if (object->isArray()) {
        CScriptVarLink *implementation = arrayClass->findChild(name);
        if (implementation) return implementation;
    }
    CScriptVarLink *implementation = objectClass->findChild(name);
    if (implementation) return implementation;

    return 0;
}
