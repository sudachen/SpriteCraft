
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

#if !defined (___A387A587_D845_4820_BD19_729CBF984B67___)
#define ___A387A587_D845_4820_BD19_729CBF984B67___

#include "m_autoconf.h"

#include <assert.h>
#include <setjmp.h>

#define ETH_NORETURN MSKIN_NORETURN

extern "C" jmp_buf*   __eth_push(pchar_t filename,int lineno);
extern "C" void       __eth_pop();
extern "C" bool       __eth_try(pchar_t* filename,int lineno);
extern "C" void       ETH_NORETURN __eth_throw(pchar_t msg,u32_t code,pchar_t filename,int lineno);
extern "C" void       ETH_NORETURN __eth_wthrow(pwide_t msg,u32_t code,pchar_t filename,int lineno);
extern "C" void       ETH_NORETURN __eth_throwf(u32_t code,pchar_t filename,int lineno,pchar_t msgfmt,...);
extern "C" void       ETH_NORETURN __eth_wthrowf(u32_t code,pchar_t filename,int lineno,const wchar_t* msgfmt,...);
extern "C" void       __eth_rethrow(pchar_t filename,int lineno);
extern "C" void       __eth_throw_if(bool cond,pchar_t msg,pchar_t filename,int lineno);
extern "C" void       __eth_wthrow_if(bool cond,pwide_t msg,pchar_t filename,int lineno);
extern "C" bool       __eth_caught();
extern "C" pchar_t    __eth_message();
extern "C" pwide_t    __eth_wmessage();
extern "C" long       __eth_code();

extern "C" bool       __eth_set_trace_exceptions(bool val);
extern "C" bool       __eth_set_debug_exceptions(bool val);
extern "C" bool       __eth_is;

inline void __eth_throw(pwide_t msg,u32_t code,pchar_t filename,int lineno) 
{ __eth_wthrow(msg,code,filename,lineno); }

inline void __eth_throw_if(bool cond,pwide_t msg,pchar_t filename,int lineno)
{ __eth_wthrow_if(cond,msg,filename,lineno); }

#define __ETH_BEGIN           {if ( !setjmp(*__eth_push(__FILE__,__LINE__)) ){
#define __ETH_END             } __eth_pop();}
#define ETH_RETHROW           if (__eth_is) __eth_rethrow(__FILE__,__LINE__)
#define ETH_THROW0(msg)       __eth_throw(msg,0,__FILE__,__LINE__)
#define ETH_THROW(msg,code)   __eth_throw(msg,code,__FILE__,__LINE__)
#define ETH_THROW_IF(expr)    __eth_throw_if((expr)!=false,#expr,__FILE__,__LINE__)

#endif // ___A387A587_D845_4820_BD19_729CBF984B67___
