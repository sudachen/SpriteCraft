
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

#if defined __cplusplus

#if !defined __m_autoconf_h__
#define __m_autoconf_h__

#define __MOLESKIN2__
//#define __MICROCOM_IMPLEMENTS_SEPARATED
//#define __MICROCOM_NOEXCEPT
//#define __MICROCOM_NODECLARE_TYPES
//#define __MICROCOM_NODECLARE_FORMATERS
//#define __MICROCOM_NONAMESPACE

#define MSKIN_BIT(x) (1L<<x)

#define MSKIN_HAS_FILESYSTEM    1

#define MSKIN_PLATFORM_WIN32    1
#define MSKIN_PLATFORM_DOBL     2

#define MSKIN_PLATFORM_EX_WPC   1
#define MSKIN_PLATFORM_EX_WCE   2

#define MSKIN_THREADS               0

#if !defined MSKIN_TARGET_PLATFORM
# if defined PTVDOBL || defined MSKIN_PTVDOBL
#   define MSKIN_TARGET_PLATFORM  MSKIN_PLATFORM_DOBL
# elif defined _WIN32 || defined MSKIN_WIN32
#   define MSKIN_TARGET_PLATFORM  MSKIN_PLATFORM_WIN32 
# endif
#else
# define MSKIN_TARGET_PLATFORM    0
#endif

#if !defined MSKIN_TARGET_PLATFORM_EX 
# if ( MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 )  
#   if defined _WIN32_WCE
#     define MSKIN_TARGET_PLATFORM_EX   MSKIN_PLATFORM_EX_WCE 
#   else
#     define MSKIN_TARGET_PLATFORM_EX   MSKIN_PLATFORM_EX_WPC 
#     undef  _WIN32_WINNT
#     define _WIN32_WINNT 0x400
#   endif
# endif
#else
# define MSKIN_TARGET_PLATFORM_EX   0
#endif

#if defined _WIN32 && !defined IMPLEMENTS_VSNPRINTF
#define IMPLEMENTS_VSNPRINTF
#endif

#if !defined _RELEASE
# if defined _DEBUG
#   if !defined DEBUG
#     define DEBUG
#   endif
#   define MSKIN_DEBUG
# endif
//#   define MSKIN_DEBUG
#else // defined _RELEASE
#   define MSKIN_RELEASE
#endif

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#endif

#if MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WCE
#define NO_ERRNO_H
#endif

#include "m_detect_compiler.h"

#if CXX_COMPILER__IS_MSVC_BUNY

# define time ___time___
# define ctime ___ctime___
# include <time.h>
# include <stdio.h>
# undef time
# undef ctime
# define ctime ctime
# define time time

extern "C" time_t time(time_t *);
extern "C" char *ctime(const time_t* timer );

#endif

//#if CXX_COMPILER__IS_MSVC_COMPATIBLE
//# define FORCEINLINE             __forceinline
//#pragma warning(disable:4996) // was declared deprecated
//#else
//# define FORCEINLINE             inline
//#endif

#if CXX_COMPILER__IS_GXX
# define __MICROCOM_IMPLEMENTS_UUIDOF
# define __NOVTABLE
#endif
 
#if CXX_COMPILER__IS_MSVC_COMPATIBLE || CXX_COMPILER__IS_MINGW
#define MSKIN_DLLEXPORT_SPEC __declspec(dllexport)
#define MSKIN_DLLIMPORT_SPEC __declspec(dllimport)
#define MSKIN_NORETURN_SPEC  __declspec(noreturn)
#define MSKIN_DLLEXPORT_NORETURN_SPEC __declspec(dllexport) __declspec(noreturn)
#define MSKIN_DLLIMPORT_NORETURN_SPEC __declspec(dllimport) __declspec(noreturn)
#define MSKIN_NOVTABLE_SPEC  __declspec(novtable)
#else
#define MSKIN_DLLEXPORT_SPEC
#define MSKIN_DLLIMPORT_SPEC
#define MSKIN_NORETURN_SPEC
#define MSKIN_DLLEXPORT_NORETURN_SPEC
#define MSKIN_DLLIMPORT_NORETURN_SPEC
#define MSKIN_NOVTABLE_SPEC
#endif

#ifdef MSKIN_USE_DLL
# ifdef MSKIN_API_EXPORT
#  define              MSKIN_DLLEXPORT_SPEC
#  define MSKIN_API_NORETURN     MSKIN_DLLEXPORT_NORETURN_SPEC
# else
#  define              MSKIN_DLLIMPORT_SPEC
#  define MSKIN_API_NORETURN     MSKIN_DLLIMPORT_NORETURN_SPEC
# endif
#else
# define MSKIN_API_NORETURN   
#endif

# define MOLE_NOVTABLE           MSKIN_NOVTABLE_SPEC
# define MSKIN_NORETURN          MSKIN_NORETURN_SPEC

// спецификатор типа вызова функций
#if CXX_COMPILER__IS_MSVC_COMPATIBLE
# define MOLE_FUNCALL   __fastcall
#else 
# define MOLE_FUNCALL   
#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma warning(disable:4244) // conversion from 'int' to 'float', possible loss of data
#endif

//typedef unsigned long         u32_t;
//typedef unsigned short        u16_t;
//typedef unsigned char         byte_t;
//typedef const unsigned long   cul_t;
//typedef const unsigned short  cuh_t;
//typedef const unsigned char   cbyte_t;
//typedef const char*           pchar_t;       

#if defined __GNUC__ && !defined __MINGW32_VERSION
  typedef long long __int64;
#endif

#define MOLESTUDIO_TYPES_ARE_DECLARED
#define MOLESTUDIO_DEFINE_VSNxPRINTF

#if !defined MSKIN_DISABLE_NAMESPACE
#define NAMESPACE_MSKIN_BEGIN       namespace mskin{ // namespace begin
#define NAMESPACE_MSKIN_END         } // namespace end
#define MSKINSPACE                  mskin
#define USING_MSKINSPACE            using namespace mskin
#define MSKIN_ANONNAMESPACE_BEGIN   namespace {
#define MSKIN_ANONNAMESPACE_END     } // namespace end
#else
#define NAMESPACE_MSKIN_BEGIN       
#define NAMESPACE_MSKIN_END         
#define MSKINSPACE                  
#define USING_MSKINSPACE            
#define MSKIN_ANONNAMESPACE_BEGIN   
#define MSKIN_ANONNAMESPACE_END     
#endif
//namespace mskin{
//}

#if !MSKIN_THREADS
#define _NOTHREADS
#endif

#if !defined __NOSTLSGI
#define __SGI_STL
#endif

//#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL
// EGCS 1.1 неакуратно работает с инлайнами опреторов new и delete
//#define __NOINLINE_NEWDELETE_OPERATORS   
//#endif

extern "C" {
  void* managed_malloc__(unsigned);
  void  managed_free__(void*);
  void  managed_heapcheck__();
  void* LHunterManage(void* p,unsigned size);
  void* LHunterUnmanage(void* p);
}

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 && !defined(_WINDOWS_)
# include <stdio.h>
# include <windows.h>
#elif MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL && defined(PTVTARGET)
//# include <powertv.h>
//# undef Free
//# undef Malloc
//# undef quad
#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
# pragma warning(disable:4530)  //C++ exception handler used, but unwind semantics are not enabled.
# pragma warning(4:4786)  //identifier was truncated to '255' characters in the debug information
//# pragma warning(disable:4786)  //identifier was truncated to '255' characters in the debug information
#endif

#if defined __MWERKS__
#define __NOSTLSGI
#define __STL_CLASS_PARTIAL_SPECIALIZATION
#endif

//#if !defined __NOSTLSGI
//#include <stl_config.h>
//#endif

//#define MSKIN_DEBUG
// #if !defined _DEBUG
// #define _DEBUG
// #endif

#include <ctype.h>
#include <wctype.h>
#include <wchar.h>

#include <new>
//#include <algorithm>
//#include <functional>
//#include <vector>
//#include <string>
//#include <set>
//#include <list>
//#include <deque>
//#include <map>

#if !defined __STL_NO_NAMESPACES
# if !defined __STD
#   define __STD std
# endif
  using namespace __STD;
#endif

#include "m_static.h"

//#if !defined MSKIN_DISABLE_STATIC_CHECK
//typedef static_check<bool(sizeof(u32_t)==4)>::success   constrain_u32_t;
//typedef static_check<bool(sizeof(void*)==4)>::success     constrain_voidp;
//typedef static_check<bool(sizeof(u16_t)==2)>::success  constrain_u16_t;
//typedef static_check<bool(sizeof(byte_t)==1)>::success    constrain_byte_t;
//#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma warning(disable:4018) //signed/unsigned mismatch
#define MSKIN_NOVTABLE __declspec(novtable)
#endif

NAMESPACE_MSKIN_BEGIN
//extern string WideToString(pwide_t);
//extern void InplaceWideToChar(void*,int charcount);
//extern void InplaceCharToWide(void*,int charcount);
NAMESPACE_MSKIN_END

#endif //__m_autoconf_h__

#endif // __cplusplus

