
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

#ifndef ___9edf61b2_cac8_4452_b9f5_62d63e759207___
#define ___9edf61b2_cac8_4452_b9f5_62d63e759207___

#include "_specific.h"
#include "string.h"
#include "flatset.h"
#include "refcounted.h"

namespace teggo {

  struct SymbolTable : Refcounted
    {
      _TEGGO_EXPORTABLE pwide_t FindSymbol(pwide_t);
      _TEGGO_EXPORTABLE pwide_t GetSymbol(pwide_t);
      _TEGGO_EXPORTABLE SymbolTable();
      _TEGGO_EXPORTABLE ~SymbolTable();
    private:
      struct PWIDE { unsigned len; pwide_t text; };
      struct PWideLess
        {
          bool operator() (const PWIDE& a,const PWIDE& b) const
            {
              return a.len < b.len || (a.len == b.len && wcsicmp(a.text,b.text) < 0);
            }
        };
      FlatsetT<PWIDE,PWideLess> table_;
    };
}

#if defined _TEGGOINLINE
#include "symboltable.inl"
#endif

#endif //___9edf61b2_cac8_4452_b9f5_62d63e759207___
