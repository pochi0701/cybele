#include "stdafx.h"
// ==========================================================================
//code=UTF8	tab=4
//
// Cybele:	Application SErver.
//
// 		cbl_script.cpp
//		$Revision: 1.0 $
//		$Date: 2018/02/12 21:11:00 $
//
// ==========================================================================
//---------------------------------------------------------------------------
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <signal.h>
#ifdef linux
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <io.h>
#endif
#include "cbl.h"
#include "cbl_tools.h"
#include "cbl_String.h"
#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
#include "define.h"
// =============================================================
// バッチ処理
// =============================================================
#define SCRIPT_SIZE 10000
#ifdef linux
void* batch(void* ptr)
#else
unsigned int __stdcall batch(void* ptr)
#endif
{
    IGNORE_PARAMETER(ptr);
    char script_filename[128];
    //char cur_dir[256];
//#ifdef linux
//    getcwd (cur_dir, sizeof (cur_dir));
//#else
//    GetCurrentDirectory (256, cur_dir);
//#endif
    //sprintf(script_filename, "%s%s%s", cur_dir, "/system/tools", "/setip.jss");
    sprintf(script_filename, "%s%s%s", global_param.server_root, "/system/tools", "/setip.jss");

    while(loop_flag){
        //イベントが合致したら指定jssを起動
        wString buffer;
        buffer.load_from_file (script_filename);
        CTinyJS  s(INVALID_SOCKET);
        registerFunctions (&s);
        registerMathFunctions (&s);
        s.execute (buffer);
        break;
    }
    return NULL;
}

