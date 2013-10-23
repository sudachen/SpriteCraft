
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


#if !defined (__mskin_process_h__)
#define __mskin_process_h__

#include "m_autoconf.h"

NAMESPACE_MSKIN_BEGIN

//MSKIN_API_NORETURN void MOLE_FUNCALL FatalError(const char* reason);
//template < class tTc > inline void FatalError(FormatT<tTc> const &fmt)
//  {
//    FatalError( fmt.Str() );
//  }
//
//MSKIN_API_NORETURN void MOLE_FUNCALL Exit(int code=0);
//MSKIN_API_NORETURN void MOLE_FUNCALL Abort();

const char* MOLE_FUNCALL GetSymbol(void* ptr);
void MOLE_FUNCALL DumpStackTrace();

NAMESPACE_MSKIN_END

#endif //__mskin_process_h__

