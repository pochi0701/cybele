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
    char tbuffer[256];
    GetCurrentDirectory (256, tbuffer);
    sprintf( script_filename,"%s%s%s", tbuffer, "/system/tools","/setip.jss");
    
    while(loop_flag){
        //イベントが合致したら指定jssを起動
        char buffer[SCRIPT_SIZE]={};
        int fd = myopen( script_filename, O_RDONLY | O_BINARY);
        if( fd < 0 ){
            break;
        }
        auto ret = read(fd,buffer, SCRIPT_SIZE);
        // 読み込み失敗
        if (ret == -1)
        {
            close(fd);
            break;
        }
        close( fd );
        CTinyJS  s(STDOUT_FILENO);
        registerFunctions (&s);
        registerMathFunctions (&s);
        s.execute(buffer);
        break;
    }
    return NULL;
}

