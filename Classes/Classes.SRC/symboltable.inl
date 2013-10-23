
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

#ifndef ___66e66e00_3224_4300_a779_4b8e401dadb7___
#define ___66e66e00_3224_4300_a779_4b8e401dadb7___

#include "symboltable.h"

#if defined _TEGGO_SYMTABLE_HERE
# define _TEGGO_SYMTABLE_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_SYMTABLE_FAKE_INLINE CXX_FAKE_INLINE
#endif

namespace teggo {

  _TEGGO_SYMTABLE_FAKE_INLINE
    pwide_t SymbolTable::FindSymbol(pwide_t val)
      {
        if ( !val ) return 0;
        PWIDE pw;
        pw.text = val;
        pw.len  = wcslen(val);
        if ( PWIDE *ppw = table_.Get(pw) )
          return ppw->text;
        return 0;
      }

  _TEGGO_SYMTABLE_FAKE_INLINE
    pwide_t SymbolTable::GetSymbol(pwide_t val)
      {
        PWIDE pw;
        pw.text = val;
        pw.len  = wcslen(val);
        if ( PWIDE *ppw = table_.Get(pw) )
          return ppw->text;
        else
          {
            pw.text = new wchar_t[pw.len+1];
            for ( unsigned i = 0; i < pw.len; ++i ) ((wchar_t*)pw.text)[i] = towlower(val[i]);
            ((wchar_t*)pw.text)[pw.len] = 0;
            table_.Put(pw);
            return pw.text;
          }
      }

  _TEGGO_SYMTABLE_FAKE_INLINE
    SymbolTable::SymbolTable()
      {
      }

  _TEGGO_SYMTABLE_FAKE_INLINE
    SymbolTable::~SymbolTable()
      {
        for ( FlatsetT<PWIDE,PWideLess>::Iterator i = table_.Iterate(); i.Next(); )
          delete[] (wchar_t*)i->text;
      }
} // namespace teggo

#endif //___66e66e00_3224_4300_a779_4b8e401dadb7___
