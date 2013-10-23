
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

#include "tffmedia.h"
#pragma code_seg(".text$tffmedia")

MEDIA_EXPORTABLE mem_ptr<TFFt_PICTURE> CXX_STDCALL OpenPicture(StringParam name)
  {
    if ( rcc_ptr<DataStream> ds = DataSource->Open(name) )
      {

        StringT<wchar_t> ident = ds->Identifier();

        if ( ident.EndsWith(L".png") )
          if ( mem_ptr<TFFt_PICTURE> p = TffmPngCreatePicFrom( ds ) )
            return p;

        if ( ident.EndsWith(L".jpg") || ident.EndsWith(L".jpeg") )
          if ( mem_ptr<TFFt_PICTURE> p = TffmJpgCreatePicFrom( ds ) )
            return p;

        if ( ident.EndsWith(L".tga") )
          if ( mem_ptr<TFFt_PICTURE> p = TffmTgaCreatePicFrom( ds ) )
            return p;

        if ( ds->Read16le() == 0x4d42 )
          if ( mem_ptr<TFFt_PICTURE> p = TffmBmpCreatePicFrom( ds ) )
            return p;

        ds->Seek(0);

        Lerr << _S*"%?failed to open picture from stream '%?'" %MEDIA_ERROR %ident;
        return mem_ptr<TFFt_PICTURE>(0);
      }
    else
      {
        Lerr << _S*"%?failed to open picture, no file '%?'" %MEDIA_ERROR %name;
        return mem_ptr<TFFt_PICTURE>(0);
      }

    return mem_ptr<TFFt_PICTURE>(0);
  }
