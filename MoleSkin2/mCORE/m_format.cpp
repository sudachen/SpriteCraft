
/*

Copyright © 2003-2013, Alexéy Sudachén, alexey@sudachen.name

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
#include "m_sync.h"
#include "m_nonstatic.h"
#include "m_core.h"
#include "m_vsnprintf.h"
#include <stdio.h>
#include <stdarg.h>
//#include <string>

//using std::string;

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN

//static NonStatic<ThreadLocker_> locker;
// static char g_buff[1024*4];
// 

// const char* Format_va(const char* fmt,va_list va)
// {
//   Vsnprintf(g_buff,sizeof(g_buff)-1,fmt,va);
//   return g_buff;
// }
// 
// const wchar_t* WideFormat_va(const wchar_t* fmt,va_list va)
// {
//   Vsnwprintf((wchar_t*)g_buff,sizeof(g_buff)/2-1,fmt,va);
//   return (wchar_t*)g_buff;
// }

// string FormatS_va(const char* fmt,va_list va)
// {
//   vector<char> buff(1024*4);
//   Vsnprintf(&buff[0],buff.size()-1,fmt,va);
//   buff[buff.size()-1] = 0;
//   return string(&buff[0]);
// }
// 
// string FormatS(const char* fmt,...)
// {
//   va_list va;
//   va_start(va,fmt);
//   const string& s = FormatS_va(fmt,va);
//   va_end(va);
//   return s;
// }


void MCORE::InitializeFormater() {
//  InitNonStatic(locker);
}
void MCORE::TerminateFormater() {
//  TermNonStatic(locker);
}

NAMESPACE_MSKIN_END

// extern "C" const char* Format(const char* fmt,...)
// {
//   va_list va;
//   va_start(va,fmt);
//   MSKINSPACE::Format_va(fmt,va);
//   va_end(va);
//   return MSKINSPACE::g_buff;
// }
// 
// extern "C" const wchar_t* WideFormat(const wchar_t* fmt,...)
// {
//   va_list va;
//   va_start(va,fmt);
//   MSKINSPACE::WideFormat_va(fmt,va);
//   va_end(va);
//   return (wchar_t*)MSKINSPACE::g_buff;
// }

