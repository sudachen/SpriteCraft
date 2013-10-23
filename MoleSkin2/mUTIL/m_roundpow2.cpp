
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
#include "m_roundpow2.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mUTIL")
#endif

NAMESPACE_MSKIN_BEGIN

enum { x1K = 1024 ,x1M = x1K*x1K, x1G = x1K*x1M };
int MOLE_FUNCALL  RoundPow2(int val)
{
  static int pows[] = 
  { 1,2,4,8,16,32,64,128,256,512,x1K
    ,x1K*2,x1K*4,x1K*8,x1K*16,x1K*32,x1K*64,x1K*128,x1K*256,x1K*512,x1M
    ,x1M*2,x1M*4,x1M*8,x1M*16,x1M*32,x1M*64,x1M*128,x1M*256,x1M*512,x1G
  };
  int i = 0;
  for ( ; i < elnof(pows)-1 && val > pows[i]; ++i ) {}
  return pows[i];
}

NAMESPACE_MSKIN_END
