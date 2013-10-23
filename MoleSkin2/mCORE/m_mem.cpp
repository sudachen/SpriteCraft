
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
#include "m_process.h"
#include "m_debug.h"
#include "m_hmgr.h"

//#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#include <crtdbg.h>
//#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN

//static void* (*mskin_malloc__)(unsigned) = 0;
//static void (*mskin_free__)(void*) = 0;
//extern void (*mskin_termhmgr__)() = 0;

void InitializeDefaultHmgr(unsigned flags)
{
//  REQUIRE ( mskin_malloc__ == 0  && mskin_free__ == 0 );
//  mskin_termhmgr__= 0;
//  mskin_malloc__  = malloc;
//  mskin_free__    = free;
}
void TerminateHmgr()
{
//  if ( mskin_termhmgr__ ) mskin_termhmgr__();
//  mskin_malloc__  = 0;
//  mskin_free__    = 0;
}

NAMESPACE_MSKIN_END

USING_MSKINSPACE;

void managed_heapcheck__()
{
  STRICT_REQUIRE ( _CrtCheckMemory() );
}

void* managed_malloc__(unsigned size)
{
  //STRICT_REQUIRE ( _CrtCheckMemory() );
  //REQUIRE ( mskin_malloc__ != 0 );
  //void* p = fatal_if_null(mskin_malloc__(size));
  //u32_t millis = GetMillis();
  void* p = fatal_if_null(malloc(teggo_max<unsigned>(size,8)));
  //XMessage(("--MALLOC %08x",p));
#if defined MSKIN_DEBUG || defined FORCE_LEAKHUNTER
  LHunterManage(p,size);
#endif
  //millis = GetMillis() - millis;
  //if ( millis > 1 ) LogF("-- allocate memory in %d millis",millis);
  //STRICT_REQUIRE ( _CrtCheckMemory() );
  //LogF("alloc mem: %08x",p);
  return p;
}

void managed_free__(void* p)
{
  //STRICT_REQUIRE ( _CrtCheckMemory() );
  //REQUIRE ( mskin_free__ != 0 );
  if ( p ) {
    //u32_t millis = GetMillis();
#if defined MSKIN_DEBUG || defined FORCE_LEAKHUNTER
    LHunterUnmanage(p);
#endif
    //mskin_free__(p);
    //LogF("free mem: %08x",p);
    //XMessage(("--FREE %08x",p));
    free(p);
    //millis = GetMillis() - millis;
    //if ( millis > 1 ) LogF("-- release memory in %d millis",millis);
  }
  //STRICT_REQUIRE ( _CrtCheckMemory() );
}

#if defined __NOINLINE_NEWDELETE_OPERATORS
void* operator new(unsigned size) throw () { return managed_malloc__(size?size:4); }
void* operator new[](unsigned size) throw ()  { return managed_malloc__(size?size:4); }
void operator delete(void* p) throw() { managed_free__(p); }
void operator delete[](void* p) throw() { managed_free__(p); }
#endif
