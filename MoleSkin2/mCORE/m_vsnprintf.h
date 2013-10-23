
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


#if !defined __m_vsnprintf_h__
#define __m_vsnprintf_h__


#include "m_autoconf.h"

#if defined(IMPLEMENTS_VSNPRINTF) 

#include "m_debug.h"
#include <stdio.h>
#include <stdarg.h>

NAMESPACE_MSKIN_BEGIN

//#if CXX_COMPILER__HAS_MSVCRT

#if defined __MWERKS__ 
#define Vsnwprintf vswprintf
#define Vsnprintf  vsnprintf
#define Snwprintf  snwprintf
#else
#define vsnprintf	@~
#define vsnwprintf	@~
#define snwprintf	@~
#define Vsnprintf	_vsnprintf
#define Vsnwprintf	_vsnwprintf
#define Snwprintf	_snwprintf
#endif

//#else
//
//#define Vsnprintf	vsnprintf
//#define Vsnwprintf	vsnwprintf
//#define Snwprintf	snwprintf
//
////static inline int vsnprintf(char* ch,int bufsize,const char* fmt,va_list va)
////{
////  int was_writen = vsprintf(ch,fmt,va);
////  REQUIRE ( was_writen < bufsize );
////  return was_writen;
////}
//
//#endif // CXX_COMPILER__HAS_MSVCRT

NAMESPACE_MSKIN_END

#endif //IMPLEMENTS_VSNPRINTF

#endif //__mskin_vsnprintf_h__

