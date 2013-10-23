
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "precompile.h"
#include "m_autoconf.h"
#include "m_debug.h"
#include "m_vsnprintf.h"
#include "m_time.h"
#include "m_core.h"
#include "m_sync.h"
#include "m_nonstatic.h"
#include "m_log.h"
#include "m_vsnprintf.h"
#include <time.h>

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
# include <stdio.h>
# include <stdlib.h>
# include <windows.h>
# if MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WPC
#   include <errno.h>
# else 
/// Enbedded C++ для WINCE, не соответствует ANASI C и не имеет errno и strerror
#   define errno 0
#   define strerror(x) ""
# endif
#elif MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL
# include "m_DOBL.h"
#endif


#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN

//#define XSYNC(x) 
#define XSYNC(x) x

#ifndef MSKIN_DISABLE_LOGGER

static char text_buff[1024*sizeof(wchar_t)] = {0};

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
 
static wchar_t log_file_name[MAX_PATH] = L"spritecraft_log.txt";
static FILE* g_logHandle = 0;

XSYNC(static NonStatic<ThreadLocker_> logger_lock);


static bool _g_OpenLog_reopen = false;

static void OpenLog()
{
  XSYNC(Synchronizer_ sync(*logger_lock));
  if ( g_logHandle ) return;
  if ( MCORE::GetModuleName()[0] == 0 ) return;
  wcscpy(log_file_name,MCORE::GetModulePath());
  if ( log_file_name[0] != 0 ) wcscat(log_file_name,L"/");
  swprintf(log_file_name+wcslen(log_file_name),L"%ls-log.txt",MCORE::GetModuleName());
  //g_logHandle = fopen("logfile.txt","w+t");
  //REQUIRE ( INVALID_HANDLE_VALUE != CreateFileW(L"c:\\Tf",GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,0,CREATE_ALWAYS,0,0) );
  g_logHandle = _wfopen( log_file_name,_g_OpenLog_reopen?L"at":L"w+t");
  if ( !g_logHandle )
  {
    EMERGENCY(+~(_S*L":Logger:Create: can't open logHandle file %s\nreason: %s"
      %log_file_name %strerror(errno)));
  }
  if (!_g_OpenLog_reopen) {
    time_t t = time(0);
    fprintf(g_logHandle,":Logger: %s",ctime(&t));
    fflush(g_logHandle);
  }
  _g_OpenLog_reopen = true;
}

static void CloseLog()
{
  XSYNC(Synchronizer_ sync(*logger_lock));
  if ( g_logHandle  ) {
    //fprintf(g_logHandle,"\n:Logger: close");
    fclose(g_logHandle);
    g_logHandle = 0;
  }
}

void LogWrite(pwide_t text) 
{
  if ( g_logHandle ) {
    //static int no = 0;
    //static u32_t millis = GetMillis();
    u32_t xms =   GetMillis();//-millis;
    u32_t ms  =   (xms%1000);
    u32_t sec =   (xms/1000)%60;
    u32_t mins =  (xms/60000);
    fwprintf(g_logHandle,L"[%02ld:%02ld.%03ld] %ls"/*"\n"*/,
      mins,sec,ms,text);
    fflush(g_logHandle);
    //XDbg(OutputDebugString(text));
    //XDbg(OutputDebugString("\n"));
    OutputDebugStringW(text);
    //OutputDebugString("\n");
  }
}


#else
static char log_file_name[] = "none-log-file";
void OpenLog() {}
void CloseLog() {}
#endif


//void Log(pchar_t text)
//{
//  XSYNC(Synchronizer_ sync(*logger_lock));
//  LogWrite(text);
//}

void WideLog(pwide_t text)
{
  XSYNC(Synchronizer_ sync(*logger_lock));
  unsigned u = wcslen(text);
  unsigned c = u*sizeof(wchar_t);
  if ( c > (sizeof(text_buff) - sizeof(wchar_t)*2) ) c = sizeof(text_buff) - sizeof(wchar_t)*2;
  memcpy(text_buff,text,c);
  memset(text_buff+c,0,sizeof(wchar_t)*2);
  text_buff[u*sizeof(wchar_t)] = '\n';
  text_buff[(u+1)*sizeof(wchar_t)] = 0;
  LogWrite((pwide_t)text_buff);
}

void Log(pchar_t text)
{
  XSYNC(Synchronizer_ sync(*logger_lock));
  unsigned u = strlen(text);
  u = teggo_min<unsigned>( u, (sizeof(text_buff)/2 - 2) );
  memcpy(text_buff,text,u);
  memset(text_buff+u,0,2);
  teggo::InplaceCharToWide(text_buff,u+1);
  text_buff[u*sizeof(wchar_t)] = '\n';
  text_buff[(u+1)*sizeof(wchar_t)] = 0;
  LogWrite((pwide_t)text_buff);
}

void LogF_va(pchar_t text,va_list va)
{
  XSYNC(Synchronizer_ sync(*logger_lock));
  memset(text_buff,0,sizeof(text_buff));
  int u = Vsnprintf(text_buff,sizeof(text_buff)-2,text,va);
  text_buff[sizeof(text_buff)-1] = 0;
  text_buff[sizeof(text_buff)-2] = 0;
  if ( u < 0 ) u = sizeof(text_buff)-2;
  *(text_buff+u) = '\n';
  teggo::InplaceCharToWide(text_buff,u+1);
  LogWrite((pwide_t)text_buff);
}

void WideLogF_va(pwide_t text,va_list va)
{
  XSYNC(Synchronizer_ sync(*logger_lock));
  Vsnwprintf((wchar_t*)text_buff,sizeof(text_buff)/sizeof(wchar_t)-2,text,va);
  memset(text_buff+sizeof(text_buff)-sizeof(wchar_t)*2,0,sizeof(wchar_t)*2);
  wcscat((wchar_t*)text_buff,L"\n");
  LogWrite((pwide_t)text_buff);
}

void LogF(pchar_t text,...)
{
  va_list va;
  va_start(va,text);
  LogF_va(text,va);
  va_end(va);
}

void WideLogF(pwide_t text,...)
{
  va_list va;
  va_start(va,text);
  WideLogF_va(text,va);
  va_end(va);
}

pwide_t GetLogFilename()
{
  return log_file_name;
}

#else

void LogF(pchar_t,...) {}

#endif // disable_logger

void MCORE::InitializeLogger(unsigned flags) {
  XSYNC(InitNonStatic(logger_lock));
#ifndef MSKIN_DISABLE_LOGGER
  if ( !(flags&MCORE::DISABLE_LOGGER) )
    MSKINSPACE::OpenLog();
#endif  
}

void MCORE::TerminateLogger() {
#ifndef MSKIN_DISABLE_LOGGER
  MSKINSPACE::CloseLog();
#endif  
  XSYNC(TermNonStatic(logger_lock));
}

#ifndef MSKIN_DISABLE_LOGGER
static bool logger_enabled = false;
#endif

bool MCORE::IsLogging() 
{
#ifndef MSKIN_DISABLE_LOGGER
  return logger_enabled;
#else
  return false;
#endif
}

bool MCORE::SetLogging(bool newval)
{
#ifndef MSKIN_DISABLE_LOGGER
  if ( newval != logger_enabled ) {
    if ( newval ) OpenLog();
    else CloseLog();
    bool oldval = logger_enabled;
    logger_enabled = newval;
    return oldval;
  }
  else return newval;
#else
  return false;
#endif
}

void Log_andErrReason(pchar_t val)
{
  SetMoleSkinErrorReason(val);
  Lout << val;
}

void Log_andErrReason(pwide_t val)
{
  SetMoleSkinErrorReason(val);
  Lout << val;
}

NAMESPACE_MSKIN_END

