
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

#if !defined __C8A32A5F_79BF_4167_8EB2_7BA04B43FD10__
#define __C8A32A5F_79BF_4167_8EB2_7BA04B43FD10__

#include "ObjList.inl"

struct SpritesList : 
  TeggoObject<
    TeggoPyDispImpl<ISpritesList, 0, ObjListBase> >
  {

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
    SpritesList();
    ~SpritesList();

    HRESULT __stdcall get_count(long* count);
    HRESULT __stdcall IsBegin(ISprite* pI,VARIANT_BOOL* ret);
    HRESULT __stdcall IsEnd(ISprite* pI,VARIANT_BOOL* ret);
    HRESULT __stdcall AtBegin(ISprite** pI);
    HRESULT __stdcall AtEnd(ISprite** pI);
    HRESULT __stdcall At(long idx,ISprite** pI);
	  HRESULT __stdcall Clear();
    HRESULT __stdcall Purge() { return Clear(); }
	  HRESULT __stdcall Find(/*[in]*/ ISprite* pI, /*[out,retval]*/ VARIANT_BOOL* pVal);
	  HRESULT __stdcall Add(/*[in]*/ ISprite* pI);
    HRESULT __stdcall Add(/*[in]*/ IUnknown *pI);
	  HRESULT __stdcall Remove();
	  HRESULT __stdcall Get(/*[out,retval]*/ ISprite** pI);
	  HRESULT __stdcall Next(/*[out,retval]*/ VARIANT_BOOL* pVal);
	  HRESULT __stdcall Reset(/*[out,retval]*/ VARIANT_BOOL* pVal);

  };

#endif __C8A32A5F_79BF_4167_8EB2_7BA04B43FD10__
