
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
#include "m_core.h"
#include "m_hmgr.h"
#include "m_irefobj.h"

#include <locale.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN

//jmp_buf MCORE::jmpBuf_;
//static bool use_longjamp = false;
//
//enum ERROR_REASON_FORMAT{
//  REASON_UNICODE = 0,
//  REASON_ASCII = 1
//};
//
//static wchar_t module_path[MAX_PATH+1] = {0};
//static wchar_t module_name[MAX_PATH+1] = {0};
//static char error_reason[1024*sizeof(wchar_t)] = {0};
//static ERROR_REASON_FORMAT error_reason_format = REASON_ASCII;
//
//pwide_t MCORE::GetModuleName() { return module_name; }
//pwide_t MCORE::GetModulePath() { return module_path; }
//
//void MCORE::InitPath()
//{
//#if MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WPC
//  static char cmdl[1024] = "";
//  pchar_t cmdline = GetCommandLine();
//  {
//    unsigned cmdl_size = teggo_min(strlen(cmdline),sizeof(cmdl)-1);
//    char * const cmdlE = cmdl+cmdl_size;
//    *cmdlE = 0;
//    memcpy(cmdl,cmdline,cmdl_size);
//    teggo::replace(cmdl,cmdlE,'\\','/');
//    teggo::transform(cmdl,cmdlE,cmdl,tolower);
//    char* p;
//    if ( *cmdl == '"' ) 
//      p = teggo::linear_find(cmdl+1,cmdlE,'"');
//    else 
//      p = teggo::linear_find(cmdl+1,cmdlE,' ');
//    char* pp  = p;
//    while ( pp != cmdl && *pp != '/' ) --pp; 
//    //++pp;
//    if ( pp != p ) ++pp; else pp = cmdl;
//    if ( !strnicmp(pp,"wscript.exe",p-pp) 
//      || !strnicmp(pp,"cscript.exe",p-pp)
//      || !strnicmp(pp,"python.exe",p-pp)
//      || !strnicmp(pp,"pythonw.exe",p-pp)
//      || !strnicmp(pp,"python_d.exe",p-pp)
//      || !strnicmp(pp,"pythonw_d.exe",p-pp)
//      ) {
//      char* pp0 = &*teggo::linear_find(p+1,cmdlE,' ',teggo::Not<teggo::Equal>());
//      if ( pp0 != cmdlE ) 
//      {
//        char* pp1;
//        if ( *pp0 == '"' ) 
//        {
//          pp1 = teggo::linear_find(pp0+1,cmdlE,'"');
//          pp0++;
//        }
//        else 
//          pp1 = teggo::linear_find(pp0+1,cmdlE,' ');
//
//        char *pp2 = pp1;
//        while( pp2 != pp0 && *pp2 != '/' ) --pp2;
//
//        memcpy(module_name,pp2,pp1-pp2); // нуль там уже есть
//        teggo::InplaceCharToWide(module_name,pp1-pp2);
//
//        if ( pp2 == pp0 ) 
//        {// рабочий каталог текущая директория
//          GetCurrentDirectoryW(MAX_PATH,module_path);
//        }
//        else
//        {
//          *pp2 = 0;
//          GetFullPathNameW(widestring(pp0),MAX_PATH,module_path,0);
//        }
//      }
//      if ( wcslen(module_name) == 0 ) {
//        wcscpy(module_name,L"uscript");
//      }
//    }else if ( !strnicmp("regsvr32.exe",pp,p-pp) ) {
//      module_path[0]=0;
//      module_name[0]=0;
//    }else if ( 
//      !strnicmp("vb6.exe",pp,p-pp) 
//      || !strnicmp("vb5.exe",pp,p-pp) 
//      || !strnicmp("python.exe",pp,p-pp) 
//      || !strnicmp("pythonw.exe",pp,p-pp) ) {
//      strncpy((char*)module_name,pp,p-pp);
//      teggo::InplaceCharToWide(module_name,p-pp);
//      GetCurrentDirectoryW(MAX_PATH,module_path);
//      //GetTempPath(MAX_PATH,module_path);
//    }else{
//      strncpy((char*)module_name,pp,p-pp);
//      teggo::InplaceCharToWide(module_name,p-pp);
//      if  ( pp != cmdl ) 
//      {
//        --pp; *pp = 0;
//        char* binname = cmdl;
//        if ( *binname == '"' ) ++binname;
//        GetFullPathNameW(widestring(binname),MAX_PATH,module_path,0);
//      }
//      else
//      {
//        GetCurrentDirectoryW(MAX_PATH,module_path);
//      }
//    }
//    size_t mpath_len = wcslen(module_path);
//    teggo::replace(module_path,module_path+mpath_len,'\\','/');
//    if ( module_path[mpath_len-1] == '/' ) 
//      module_path[mpath_len-1] = 0;
//  }
//#endif
//}
//
//void MCORE::ReStart(unsigned flags)
//{
//  InitPath();
//}
//
//void MCORE::StartRuntime(unsigned flags,unsigned heapPages)
//{
//  ResetMillis();
//  setlocale( LC_ALL, "" );
//  setlocale( LC_NUMERIC, "C" );
//  use_longjamp = (flags&DISABLE_LJMP) == 0; 
//  InitPath();
//  MCORE::InitializeFormater();
//  MCORE::InitializeLogger(flags);
//  InitializeDefaultHmgr(0);
//  MCORE::InitializeSingletons();
//  Lout << _S*"mskincore: comand line %?" %GetCommandLine()
//       << _S*"mskincore: module name %?" %module_name
//       << _S*"mskincore: module path %?" %module_path;
//}
//
//void MCORE::FinishRuntime()
//{
//  MCORE::TerminateSingletons();
//  TerminateHmgr();
//  MCORE::TerminateLogger();
//  MCORE::TerminateFormater();
//}
//
//
//MSKIN_API_NORETURN void MOLE_FUNCALL Exit(int ecode)
//{
//  if ( use_longjamp )
//    longjmp(MCORE::jmpBuf_,MCORE::EXIT_PROCESS);
//  else exit(ecode);
//}

void* Adaptable::Adaptate_(const void*) { 
  return 0; 
}
widestring Adaptable::ToString_() const 
{ return (_S*L"ADAPTABLE{%p}"%this).Str(); }

pchar_t const IRefObject::TypeID__ = "#TYPEID#mskin::IRefObject";
void* IRefObject::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return Adaptable::Adaptate_(id);
}

//void ResetErrorLog() {
//  error_reason[0] = 0;
//  error_reason_format = REASON_ASCII;
//}
//
//void SetMoleSkinErrorReason(pchar_t val) {
//  strncpy(error_reason,val,sizeof(error_reason)/sizeof(wchar_t)-1);
//  memset(error_reason+sizeof(error_reason)-sizeof(wchar_t),0,sizeof(wchar_t));
//  error_reason_format = REASON_ASCII;
//}
//
//void SetMoleSkinErrorReason(pwide_t val) {
//  wcsncpy((wchar_t*)error_reason,val,sizeof(error_reason)/sizeof(wchar_t)-1);
//  memset(error_reason+sizeof(error_reason)-sizeof(wchar_t),0,sizeof(wchar_t));
//  error_reason_format = REASON_UNICODE;
//}
//
//pchar_t GetMoleSkinErrorReason() {
//  if ( error_reason_format == REASON_UNICODE ) {
//    teggo::InplaceWideToChar(error_reason,-1);
//    error_reason_format = REASON_UNICODE;
//  }
//  return error_reason;
//}
//
//pwide_t GetErrorLog() {
//  if ( error_reason_format == REASON_ASCII ) {
//    teggo::InplaceCharToWide(error_reason,-1);
//    error_reason_format = REASON_ASCII;
//  }
//  return (pwide_t)error_reason;
//}

NAMESPACE_MSKIN_END
