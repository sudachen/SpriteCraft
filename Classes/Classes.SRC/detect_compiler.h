
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

#if !defined ___287a6688_2257_4bd9_b5b8_28d490d9e1b0___
#define ___287a6688_2257_4bd9_b5b8_28d490d9e1b0___

#if defined __cplusplus
#define CXX_EXTERNC extern "C"
#else
#define CXX_EXTERNC extern
#endif

#if defined _DEBUG
#define CXX_IF_DEBUG_ELSE(a,b) a
#else
#define CXX_IF_DEBUG_ELSE(a,b) b
#endif

#define CXX_COMPILER_UNKNOWN    0
#define CXX_COMPILER_MSVC       1
#define CXX_COMPILER_GCC        2
#define CXX_COMPILER_ICL        5
#define CXX_COMPILER_BCC        6
#define CXX_COMPILER_DMC        7
#define CXX_COMPILER_MCW        8

#define CXX_COMPILER_NAME       L"unknown compiler"
#define CXX_COMPILER_VERSION    0
#define CXX_COMPILER            CXX_COMPILER_UNKNOWN

#if defined (__BORLANDC__)
# undef   CXX_COMPILER_NAME
# undef   CXX_COMPILER
# define  CXX_COMPILER_NAME "Borland C++"
# define  CXX_COMPILER CXX_COMPILER_BCC
#elif defined (__INTEL_COMPILER)
# undef   CXX_COMPILER_NAME
# undef   CXX_COMPILER
# undef   CXX_COMPILER_VERSION
# define  CXX_COMPILER CXX_COMPILER_ICL
# define  CXX_COMPILER_NAME "Intel C++"
# define  CXX_COMPILER_VERSION __INTEL_COMPILER
#elif defined (_MSC_VER)
# undef   CXX_COMPILER_NAME
# undef   CXX_COMPILER
# undef   CXX_COMPILER_VERSION
# define  CXX_COMPILER CXX_COMPILER_MSVC
# define  CXX_COMPILER_NAME "Microsoft C++"
# if ( _MSC_VER == 1200 )
#   define CXX_COMPILER_VERSION 0x600
# elif ( _MSC_VER == 1300 )
#   define CXX_COMPILER_VERSION 0x700
# elif ( _MSC_VER == 1310 )
#   define CXX_COMPILER_VERSION 0x701
# elif ( _MSC_VER >= 1400 )
#   define CXX_COMPILER_VERSION 0x800
# else
#   define CXX_COMPILER_VERSION 0
# endif
#elif defined (__GNUC__)
# undef   CXX_COMPILER_NAME
# undef   CXX_COMPILER
# undef   CXX_COMPILER_VERSION
# define  CXX_COMPILER CXX_COMPILER_GCC
# define  CXX_COMPILER_NAME "GNU C++"
# define  CXX_COMPILER_VERSION (__GNUC__*0x100+__GNUC_MINOR__)
#elif defined (__DMC__)
# undef   CXX_COMPILER_NAME
# undef   CXX_COMPILER
# undef   CXX_COMPILER_VERSION
# define  CXX_COMPILER CXX_COMPILER_DMC
# define  CXX_COMPILER_NAME "Digital Mars C++"
# define CXX_COMPILER_VERSION 0
#elif defined (__MWERKS__)
# undef   CXX_COMPILER_NAME
# undef   CXX_COMPILER
# undef   CXX_COMPILER_VERSION
# define  CXX_COMPILER CXX_COMPILER_MCW
# define  CXX_COMPILER_NAME "Metrowerks C++"
# define CXX_COMPILER_VERSION 0
#else
# undef   CXX_COMPILER_NAME
# define  CXX_COMPILER_NAME "Unknown C++"
#endif

#define CXX_COMPILER_IS_GXX     (CXX_COMPILER == CXX_COMPILER_GCC )
#define CXX_COMPILER_IS_MSVC    (CXX_COMPILER == CXX_COMPILER_MSVC )
#define CXX_COMPILER_IS_BCC     (CXX_COMPILER == CXX_COMPILER_BCC )
#define CXX_COMPILER_IS_ICL     (CXX_COMPILER == CXX_COMPILER_ICL )
#define CXX_COMPILER_IS_DMC     (CXX_COMPILER == CXX_COMPILER_DMC )
#define CXX_COMPILER_IS_MCW     (CXX_COMPILER == CXX_COMPILER_MCW )

#define CXX_COMPILER_IS_MSVC_COMPATIBLE   \
  (CXX_COMPILER_IS_MSVC || CXX_COMPILER_IS_ICL )

#if CXX_COMPILER_IS_MSVC
# define CXX_COMPILER_IS_MSVC_BUNNY (_MSC_VER >= 1400)
#else
# define CXX_COMPILER_IS_MSVC_BUNNY 0
#endif

#if CXX_COMPILER_IS_GXX && defined (__MINGW32_VERSION)
# define CXX_COMPILER_IS_MINGW 1
#else
# define CXX_COMPILER_IS_MINGW 0
#endif

#define CXX_COMPILER_HAS_MSVCRT \
  (CXX_COMPILER_IS_MSVC_COMPATIBLE || CXX_COMPILER_IS_MINGW)

#if !defined _NOEXCEPTIONS
# if CXX_COMPILER_IS_MSVC_COMPATIBLE
#  if !defined _CPPUNWIND
#   define CXX_USES_EXCEPTIONS 0
#  else
#   define CXX_USES_EXCEPTIONS 1
#  endif
# else
#  define CXX_USES_EXCEPTIONS 1
# endif
#else /* _NOEXCEPTIONS defined */
# define CXX_USES_EXCEPTIONS 0
#endif

#if CXX_USES_EXCEPTIONS
#define CXX_TRY       try
#define CXX_CATCH_ALL catch(...)
#define CXX_RETHROW   throw
#else
#define CXX_TRY       if (true)
#define CXX_CATCH_ALL else
#define CXX_RETHROW
#endif

#if CXX_COMPILER_IS_MSVC && CXX_COMPILER_VERSION >= 0x700
# define CXX_FAKE_INLINE inline __declspec(noinline)
# define CXX_NO_INLINE __declspec(noinline)
#else
# if CXX_COMPILER_IS_GXX
#  define CXX_FAKE_INLINE inline /*__attribute__((noinline))*/
#  define CXX_NO_INLINE __attribute__((noinline))
# else
#  define CXX_FAKE_INLINE inline
#  define CXX_NO_INLINE
# endif
#endif

#define CXX_NIE ,...

#if CXX_COMPILER_IS_MSVC_COMPATIBLE
# define CXX_NO_VTABLE __declspec(novtable)
#else
# define CXX_NO_VTABLE
#endif


#if CXX_COMPILER_IS_MSVC_COMPATIBLE || defined _PLAYSTATION2
# define CXX_HAS_BYTESWAP 1
# define CXX_BYTESWAP(a,b,s) _swab((char*)a,(char*)b,(int)s)
#else
# define CXX_HAS_BYTESWAP 0
#endif

#if CXX_COMPILER_IS_MSVC_COMPATIBLE
# define CXX_FASTCALL     __fastcall
# define CXX_STDCALL      __stdcall
# define CXX_FORCEINLINE  __forceinline
# define CXX_CDECL        __cdecl
#else
# if CXX_COMPILER_IS_GXX
#  define CXX_STDCALL     __attribute__((stdcall))
#  define CXX_FASTCALL    __attribute__((regparm,stdcall))
#  define CXX_CDECL       __attribute__((cdecl))
# else
#  define CXX_FASTCALL
#  define CXX_STDCALL
#  define CXX_CDECL
# endif
# define CXX_FORCEINLINE  inline
#endif

typedef unsigned long     u32_t;
typedef unsigned short    u16_t;
typedef unsigned char     byte_t;
typedef unsigned char     uc8_t;
typedef long              i32_t;
typedef short             i16_t;
typedef char              ic8_t;

#if (CXX_COMPILER_IS_MSVC || CXX_COMPILER_IS_ICL) && _MSC_VER < 1400
typedef u16_t wchar_t;
#endif

typedef char const       *pchar_t;
typedef wchar_t const    *pwide_t;

typedef u32_t  const      cu32_t;
typedef u16_t  const      cu16_t;
typedef byte_t const      cbyte_t;

#if CXX_COMPILER_IS_MSVC_COMPATIBLE
typedef unsigned __int64 u64_t;
typedef __int64 i64_t;
#else
typedef unsigned long long u64_t;
typedef long long i64_t;
#endif

#if defined __cplusplus
inline u64_t cxx_u64_hilo(u32_t h, u32_t l)
  {
    return ((u64_t)h << 32 ) | (u64_t)l;
  }
#else
#define cxx_u64_hilo(h,l) (((u64_t)(h) << 32 ) | (u64_t)(l))
#endif

#define CXX_BIT(x) (1L<<x)

#if defined __cplusplus
template <class tT >
  inline void cxx_swap( tT &a, tT &b)
    { tT foo = a; a= b; b= foo; }
template <class tT >
  inline tT cxx_min( tT a, tT b)
    { return a < b ? a : b; }
template <class tT >
  inline tT cxx_max( tT a, tT b)
    { return a > b ? a : b; }
#endif

#if CXX_COMPILER == CXX_COMPILER_GCC
# define __try if(1)
# define __finally else
# define __except(x) else
#endif

#endif /* ___287a6688_2257_4bd9_b5b8_28d490d9e1b0___ */
