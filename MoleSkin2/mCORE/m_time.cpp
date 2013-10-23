
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
#include "m_time.h"

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
//#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#elif MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL
#include "m_DOBL.h"
#else
#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
extern "C" void _Yield()
{
  typedef BOOL (__stdcall *SwitchToThread_t)(void);
  static SwitchToThread_t SwitchToThread_f  = 
    (SwitchToThread_t)GetProcAddress(GetModuleHandle("kernel32.dll"),"SwitchToThread");
  if ( SwitchToThread_f ) 
    SwitchToThread_f();
  else
    Sleep(1);
  //Sleep(0);
}
#endif

NAMESPACE_MSKIN_BEGIN

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
#define __MSkin_GetMillis__

static u32_t g_base_millis = 0;
static u32_t g_base_millis_pf = 0;
static u32_t g_base_millis_pf_interval = 0;

static u32_t __get_millis_pf()
{
  static __int64 perffreq = 0;
  static BOOL _fake = QueryPerformanceFrequency((LARGE_INTEGER*)&perffreq);
  if ( perffreq > 1000 ) {
    __int64 val;
    QueryPerformanceCounter((LARGE_INTEGER*)&val);
    return u32_t(val / (perffreq/1000));
  } 
  else
    return timeGetTime();
}

u32_t MOLE_FUNCALL GetMillis(void) {
  static u32_t last_ms = timeGetTime();
  u32_t ms  = timeGetTime();
  u32_t mpf = __get_millis_pf();
  if ( ms - last_ms > (mpf-g_base_millis_pf+20) ) {
    //LogF("timer: perfcounter wrapped, reseting timer");
    OutputDebugString("timer: perfcounter wrapped, reseting timer");
    g_base_millis_pf = mpf;
    g_base_millis_pf_interval = ms - g_base_millis;
  }
  last_ms = ms;
  return mpf-g_base_millis_pf + g_base_millis_pf_interval;
}

void MOLE_FUNCALL ResetMillis()
{
  g_base_millis_pf = __get_millis_pf();
  g_base_millis    = timeGetTime(); 
  g_base_millis_pf_interval = 0;
}

void MOLE_FUNCALL SleepMillis(unsigned ms) { Sleep(ms); }

#else
#error has no sleep implementation
#endif // platform


#undef __MSkin_GetMillis__

u32_t  MOLE_FUNCALL MSkin_time(u32_t*)
{
  return 0;
}

pchar_t  MOLE_FUNCALL MSkin_ctime(u32_t*)
{
  return "";
}

NAMESPACE_MSKIN_END

