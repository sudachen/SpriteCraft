
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

#ifndef ___15724387_86d6_46a7_8d40_ca2bf44700d6___
#define ___15724387_86d6_46a7_8d40_ca2bf44700d6___

#include "_specific.h"
#include "string.h"
#include "streams.h"
#include "refcounted.h"
#include "xdom.h"

namespace teggo {

  _TEGGO_EXPORTABLE TEGGOt_E  CXX_STDCALL DEFparse(DataStream *ds, XMLdocumentBuilder *db);
  _TEGGO_EXPORTABLE TEGGOt_E  CXX_STDCALL DEFparse(StringParam source, XMLdocumentBuilder *db);
  _TEGGO_EXPORTABLE Xdocument CXX_STDCALL XopenDefDocument(StringParam docname);
  _TEGGO_EXPORTABLE Xdocument CXX_STDCALL XopenDefDocument(DataStream &);
  _TEGGO_EXPORTABLE TEGGOt_E  CXX_STDCALL XwriteDefDocument(Xnode *root_node, StringParam fname, bool use_utf8 = false);
  _TEGGO_EXPORTABLE TEGGOt_E  CXX_STDCALL XwriteDefDocument(Xnode *root_node, DataStream *ds, bool use_utf8 = false);

} // namespace teggo

#if defined _TEGGOINLINE
#include "defparser.inl"
#endif

#endif //___15724387_86d6_46a7_8d40_ca2bf44700d6___
