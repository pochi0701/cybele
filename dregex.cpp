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
#include "dregex.h"
#include "cbl_String.h"
#include "cbl_tools.h"
using namespace std;

// ------------------------------------------------------------
// match ���O�R���p�C���E�C���X�^���X�s�v��
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
		regex re(tmp.c_str());
		//�}�b�`�������H
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

// replace �{��
// �ŏ��Ɍ��������}�b�`�͈͂ƃO���[�v1�`9�܂ł͈̔́A�v10��ێ�
// �O���[�v��10�ȏ�g���悤�ȕ��G�ȃp�^�[���͔����A������s���ׂ�
#define nbmat (10)

///
int dregex::replace(wString* result, const wString text, const vector<wString> pattern, const vector<wString> replacement)
{
    //regex_t re;
    int repeatable = 1;
	std::regex_constants::match_flag_type cflags =  std::regex_constants::format_default;
	//��񂾂��}�b�`
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
        //�p�^�[������C���q���擾
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
// split �{��
int dregex::split2(vector<wString>* result, const wString text, regex re, const bool global)
{
    //regmatch mat[2];          //BCC�ł͂Q�ȏ�łȂ��Ɛ���ɓ��삵�Ȃ�
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
    // �ŏI�v�f
    if (cnt < text.size()) {
        text.copy(tmp, text.size()-cnt, cnt);
        *(tmp+text.size()-cnt) = '\0';
        result->push_back(tmp);
    }
    if (cnt==0) return 1; // split ���鎖���Ȃ�����
    return 0;
}

// split ���O�R���p�C���E�C���X�^���X�s�v��(REG_NOSUB�͖���)
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