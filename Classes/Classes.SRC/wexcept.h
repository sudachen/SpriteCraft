
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

#if !defined __E48D916E_9149_4F44_9FA5_10FA6930CFD5__
#define __E48D916E_9149_4F44_9FA5_10FA6930CFD5__

#include "_specific.h"
#include "format.h"
#include "string.h"

namespace teggo
{
  struct WideException
    {
      WideException(pwide_t what=0) : what_(what) {}
      WideException(FormatT<wchar_t> const &fwhat) { fwhat.Write(what_); }
      virtual pwide_t What() const { return what_?what_.Str():L"wexception"; }
      virtual ~WideException() {}

      widestring what_;
    };
}

#endif // __E48D916E_9149_4F44_9FA5_10FA6930CFD5__
