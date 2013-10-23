
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
#include "m_core.h"
#include "m_vsnprintf.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

USING_MSKINSPACE;

enum { __eth_JBU_SIZE = 64 };
enum ETH_MSG_ENCODE { __eth_ENCODE_UNICODE = 0, __eth_ENCODE_ASCII = 1 };
static u32_t __eth_error_code = 0;
static char    __eth_error_msg[1024*sizeof(wchar_t)] = {0};
ETH_MSG_ENCODE __eth_error_msg_encode = __eth_ENCODE_ASCII;
static jmp_buf __eth_jbuf_stack[__eth_JBU_SIZE];
static long    __eth_jbuf_top = 0;
static bool    __eth_is_processed = true;
//static 
bool    __eth_is = false;
static bool    __eth_break_on_throw = false;
static bool    __eth_trace_on_throw = false;

#define __eth_assert(x) do { if(!(x)) EMERGENCY(L"<ETH/assert> "L###x); } while(0)

static void __eth_trace_exceptions(pchar_t fname,long lineno,bool rethrow)
{
  if ( __eth_trace_on_throw )
    Lerr << _S*"<ETH/tracer> '%.20s%s' has bean %sthrown at %s:%d"
      % __eth_error_msg
      %(strlen(__eth_error_msg) > 20 ? "...":"")
      %(rethrow?"RE":"")
      %(fname?fname:"<source>",lineno);
}

static void ETH_NORETURN ___eth_throw()
{
  __eth_assert(__eth_is_processed);
  __eth_assert(__eth_jbuf_top);
  if (__eth_break_on_throw) {__debug_break__();}
  __eth_is = true;
  __eth_is_processed = false;
  longjmp(__eth_jbuf_stack[__eth_jbuf_top-1],1);
}

void ETH_NORETURN __eth_throw(pchar_t msg,u32_t code,pchar_t filename,int lineno)
{
  if ( msg ) strncpy(__eth_error_msg,msg,sizeof(__eth_error_msg)/sizeof(wchar_t)-1);
  else  __eth_error_msg[0] = 0;
  __eth_error_msg_encode = __eth_ENCODE_ASCII;
  __eth_error_code = code;
  __eth_trace_exceptions(filename,lineno,false);
  ___eth_throw();
}

void ETH_NORETURN __eth_wthrow(pwide_t msg,u32_t code,pchar_t filename,int lineno)
{
  if ( msg ) wcsncpy((wchar_t*)__eth_error_msg,msg,sizeof(__eth_error_msg)/sizeof(wchar_t)-1);
  else  ((wchar_t*)__eth_error_msg)[0] = 0;
  __eth_error_msg_encode = __eth_ENCODE_UNICODE;
  __eth_error_code = code;
  __eth_trace_exceptions(filename,lineno,false);
  ___eth_throw();
}

void ETH_NORETURN __eth_throwf(u32_t code,pchar_t filename,int lineno,pchar_t msgfmt,...)
{
  if ( msgfmt ) {
    va_list va;
    va_start(va,msgfmt);
    Vsnprintf(__eth_error_msg,sizeof(__eth_error_msg)/sizeof(wchar_t)-1,msgfmt,va);
    va_end(va);
  } 
  else __eth_error_msg[0] = 0;
  __eth_error_msg_encode = __eth_ENCODE_ASCII;
  __eth_error_code = code;
  __eth_trace_exceptions(filename,lineno,false);
  ___eth_throw();
}

void ETH_NORETURN __eth_wthrowf(u32_t code,pchar_t filename,int lineno,pwide_t msgfmt,...)
{
  if ( msgfmt ) {
    va_list va;
    va_start(va,msgfmt);
    Vsnwprintf((wchar_t*)__eth_error_msg,sizeof(__eth_error_msg)/sizeof(wchar_t)-1,msgfmt,va);
    va_end(va);
  }
  else ((wchar_t*)__eth_error_msg)[0] = 0;
  __eth_error_msg_encode = __eth_ENCODE_UNICODE;
  __eth_error_code = code;
  __eth_trace_exceptions(filename,lineno,false);
  ___eth_throw();
}

void __eth_rethrow(pchar_t filename,int lineno)
{
  if ( __eth_is ) {
    __eth_trace_exceptions(filename,lineno,true);
    ___eth_throw();
  }
}

void __eth_throw_if(bool cond,pchar_t msg,pchar_t filename,int lineno)
{
  if ( cond ) 
    __eth_throw(msg,0,filename,lineno);
}

void __eth_wthrow_if(bool cond,pwide_t msg,pchar_t filename,int lineno)
{
  if ( cond ) 
    __eth_wthrow(msg,0,filename,lineno);
}

bool __eth_caught()
{
  return __eth_is;
}

pchar_t __eth_message()
{
  if ( __eth_error_msg_encode != __eth_ENCODE_ASCII ) {
    teggo::InplaceWideToChar(__eth_error_msg,-1);
    __eth_error_msg_encode = __eth_ENCODE_ASCII;
  }
  return __eth_error_msg;
}

pwide_t __eth_wmessage()
{
  if ( __eth_error_msg_encode != __eth_ENCODE_UNICODE ) {
    teggo::InplaceCharToWide(__eth_error_msg,-1);
    __eth_error_msg_encode = __eth_ENCODE_UNICODE;
  }
  return (wchar_t*)__eth_error_msg;
}

long __eth_code()
{
  return __eth_error_code;
}

jmp_buf* __eth_push(pchar_t filename,int lineno)
{
  __eth_assert(__eth_is_processed);
  __eth_assert(__eth_jbuf_top < __eth_JBU_SIZE);
  __eth_is = false;
  __eth_error_msg[0] = 0;
  __eth_error_code = 0;
  return &__eth_jbuf_stack[__eth_jbuf_top++];
}

void __eth_pop()
{
  __eth_is_processed = true;
  --__eth_jbuf_top;
}

bool __eth_set_trace_exceptions(bool val)
{
  bool old = __eth_trace_on_throw;
  __eth_trace_on_throw = val;
  return old;
}

bool __eth_set_debug_exceptions(bool val)
{
  bool old = __eth_break_on_throw;
  __eth_break_on_throw = val;
  return old;
}

