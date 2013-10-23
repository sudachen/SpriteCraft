
/*

Copyright Â© 2003-2013, AlexÃ©y SudachÃ©n, alexey@sudachen.name

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

#if !defined MSKIN_MAX_SINGLETONS
#define MSKIN_MAX_SINGLETONS 256
#endif

#include "m_debug.h"
#include "m_log.h"
#include "m_nonstatic.h"
#include "m_sync.h"
#include "m_singletons.h"
#include "m_core.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN

struct Killer
{
  AtExitHandler* killer;
  int level;
  static const Killer Make(AtExitHandler* killer,int level) {
    Killer k = { killer,level };
    return k;
  }
};

struct Killer_less 
{
  bool operator()(const Killer& x, const Killer& y) const 
  {
    return x.level < y.level;
  }
};

static Killer g_killers[MSKIN_MAX_SINGLETONS];
static unsigned g_count = 0;
static NonStatic<ThreadLocker> locker;

void MCORE::InitializeSingletons()
{
  InitNonStatic(locker);
}

void MCORE::TerminateSingletons()
{
  locker->Lock();
  Qsort(g_killers,g_killers+g_count,Killer_less());
  for ( unsigned u = 0; u < g_count; ++u ) {
    g_killers[u].killer->~AtExitHandler();
  }
  g_count = 0;
  // íå ðàçáëîêèðóåòñÿ !!!
  TermNonStatic(locker);
}

void MOLE_FUNCALL ManageObjectAtExit(AtExitHandler* killer,int level = 0)
{
  REQUIRE ( g_count < MSKIN_MAX_SINGLETONS );
  g_killers[g_count++] = Killer::Make(killer,level);
}
void MOLE_FUNCALL LockSingletonManager()
{
  locker->Lock();
}
void MOLE_FUNCALL UnlockSingletonManager()
{
  locker->Unlock();
}

NAMESPACE_MSKIN_END
