#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		dregex.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#include "define.h"
#include "dregex.h"
#include "cbl_String.h"
#include "cbl_tools.h"
using namespace std;

// ------------------------------------------------------------
// match 事前コンパイル・インスタンス不要版
int dregex::match(const wString text, const wString pattern)
{
	//std::regex_constants::match_flag_type cflags = std::regex_constants::match_default;
	wString tmp = pattern;
	if (tmp[0] == '/') {
		while (tmp[tmp.length() - 1] != '/') {
			if (tmp[tmp.length() - 1] == 'i') {
			//	cflags |= REG_ICASE;
				tmp = tmp.substr(0, tmp.length() - 1);
			}
			else if (tmp[tmp.length() - 1] == 'x') {
			//	cflags |= REG_EXTENDED;
				tmp = tmp.substr(0, tmp.length() - 1);
			}
			else if (tmp[tmp.length() - 1] == 'm') {
			//	cflags &= ~REG_NEWLINE;
				tmp = tmp.substr(0, tmp.length() - 1);
			}
			else if (tmp[tmp.length() - 1] == 's') {
			//	cflags |= REG_NEWLINE;
				tmp = tmp.substr(0, tmp.length() - 1);
			}
			else {
				tmp = tmp.substr(0, tmp.length() - 1);
				//	//not pattern
			//	return 0;
			}
		}
		tmp = tmp.substr(1, tmp.length() - 2);
	}
	else {
		// not pattern
		return 0;
	}

	//if (regcomp(&re, (char*)tmp.c_str(), cflags)) {
	//	return 1; // syntax error.
	//}
	//int res = regexec(&re, (char*)text.c_str(), 0, 0, 0);
	//regfree(&re);
	//return !(res == REG_NOMATCH);



    //wString tmp = pattern;
	try {
		std::regex re(tmp.c_str());
		//マッチしたか？
		return std::regex_match(text.c_str(), re);
	}
	catch (const regex_error& err)
	{
		IGNORE_PARAMETER(err);
		debug_log_output("dregex::match match error");
		return 0;
	}
}

// ------------------------------------------------------------

// replace 本体
// 最初に見つかったマッチ範囲とグループ1～9までの範囲、計10個を保持
// グループを10個以上使うような複雑なパターンは避け、複数回行うべき
#define nbmat (10)

///
int dregex::replace(wString* result, const wString text, const vector<wString> pattern, const vector<wString> replacement)
{
    //regex_t re;
    int repeatable = 1;
	std::regex_constants::match_flag_type cflags =  std::regex_constants::format_default;
	//一回だけマッチ
	if (repeatable == 0) {
		cflags = std::regex_constants::format_first_only;
	}
	//
    wString pat;
    wString temptext;
    if( pattern.size() != replacement.size() ){
        return 1;
    }
    temptext = text;
    //int num;
    for( size_t i = 0 ; i < pattern.size() ; i++ ){
        //パターンから修飾子を取得
        wString tmp = pattern[i];
        if( tmp[0] == '/' ){
            while( tmp[tmp.length()-1] != '/' ){
                if ( tmp[tmp.length()-1] == 'i' ){
                    //cflags |= REG_ICASE;
                    tmp = tmp.substr(0,tmp.length()-1);
                }else if ( tmp[tmp.length()-1] == 'x' ){
                    //cflags |= REG_EXTENDED;
                    tmp = tmp.substr(0,tmp.length()-1);
                }else if ( tmp[tmp.length()-1] == 'm' ){
                    //cflags &= ~REG_NEWLINE;
                    tmp = tmp.substr(0,tmp.length()-1);
                }else if ( tmp[tmp.length()-1] == 's' ){
                    //cflags |= REG_NEWLINE;
                    tmp = tmp.substr(0,tmp.length()-1);
                }else{
                    //not pattern
                    return 1;
                }
            }
			tmp =  tmp.substr(1, tmp.length() - 2);
        }else{
            // not pattern
            return 0;
        }
		try {
			regex re(tmp.c_str());
			//if (regcomp(&re, (char*)tmp.c_str(), cflags&~REG_NOSUB)){
			//   return 1; // syntax error.
			//}
			//string text;
			string mytext = temptext.c_str();
			string repl = replacement[i].c_str();
			temptext = (std::regex_replace(mytext, re, repl)).c_str();
		}
		catch (const regex_error& err)
		{
			IGNORE_PARAMETER(err);
			*result = text;
			debug_log_output("dregex::replace replace error");
			return 0;
		}
    }
	*result = temptext;
    return 1;
}
#if 0
// ------------------------------------------------------------
// split 本体
int dregex::split2(vector<wString>* result, const wString text, regex re, const bool global)
{
    //regmatch mat[2];          //BCCでは２以上でないと正常に動作しない
    char* sp = (char*)text.c_str();
    size_t cnt = 0;
    result->clear();
    char tmp[1024]={0};
    do {
        //if (regexec(&re, (char*)(sp+cnt), 2, mat, 0) == REG_NOMATCH) {
        //    break;
        //}
        text.copy(tmp, mat[0].rm_so, cnt);
        *(tmp+mat[0].rm_so) = '\0';
        result->push_back(tmp);
        cnt += mat[0].rm_eo;
    } while (global);
    // 最終要素
    if (cnt < text.size()) {
        text.copy(tmp, text.size()-cnt, cnt);
        *(tmp+text.size()-cnt) = '\0';
        result->push_back(tmp);
    }
    if (cnt==0) return 1; // split する事がなかった
    return 0;
}

// split 事前コンパイル・インスタンス不要版(REG_NOSUBは無効)
int dregex::split(vector<wString>* result, const wString text, const wString pattern, const bool global)
{
    regex re(pattern.c_str());
    //if (regcomp(&re, (char*)pattern.c_str(), cflags&~REG_NOSUB)){
    //    return 1; // syntax error.
    //}
    int res = dregex::split2(result, text, re, global);
    //regfree(&re);
    return res;
}
#endif
