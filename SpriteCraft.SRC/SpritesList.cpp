
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

#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "SpriteHelper.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Lists$SpritesList")
#endif

#include "SpritesList.h"

HRESULT __stdcall SpritesList::InterfaceSupportsErrorInfo(REFIID riid)
  {
    if ( IsEqualGUID(TEGGO_GUIDOF(ISpritesList),riid) )
      return S_OK;
    else
      return ObjListBase::InterfaceSupportsErrorInfo(riid);
  }

SpritesList::SpritesList()
  {
  }

SpritesList::~SpritesList()
  {
  }

HRESULT __stdcall SpritesList::Reset(VARIANT_BOOL *pVal)
  {
    return ObjListBase::Reset(pVal);
  }

HRESULT __stdcall SpritesList::Next(VARIANT_BOOL *pVal)
  {
    return ObjListBase::Next(pVal);
  }

HRESULT __stdcall SpritesList::Get(ISprite **pI)
  {
    *pI = 0;
    rcc_ptr<IUnknown> ppI;
    if ( HRESULT hr = ObjListBase::Get(&+ppI) )
      return hr;
    else
      {
        return ppI->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pI);
      }
  }

HRESULT __stdcall SpritesList::Remove()
  {
    return ObjListBase::Remove();
  }

HRESULT __stdcall SpritesList::Add(ISprite *pI)
  {
    return ObjListBase::Add(pI);
  }

HRESULT __stdcall SpritesList::Add(IUnknown *pI)
  {
    ISprite *ppI;
    if ( pI 
      && SUCCEEDED( pI->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)&ppI) )
      && ppI )
      {
        ppI->Release();
        return ObjListBase::Add(pI);        
      }
    else
      return GenSCWideError(L"is not sprite object");
  }

HRESULT __stdcall SpritesList::Find(ISprite *pI, VARIANT_BOOL *pVal)
  {
    return ObjListBase::Find(pI,pVal);
  }

HRESULT __stdcall NewSpritesList_(ISpritesList** pI)
  {
    *pI = new SpritesList();
    return S_OK;
  }

HRESULT __stdcall SpritesList::Clear()
  {
    return ObjListBase::Clear();
  }

HRESULT __stdcall SpritesList::get_count(long* count)
  {
    return ObjListBase::get_count(count);
  }

HRESULT __stdcall SpritesList::IsBegin(ISprite* pI,VARIANT_BOOL* ret)
  {
    return ObjListBase::IsBegin(pI,ret);
  }

HRESULT __stdcall SpritesList::IsEnd(ISprite* pI,VARIANT_BOOL* ret)
  {
    return ObjListBase::IsEnd(pI,ret);
  }

HRESULT __stdcall SpritesList::AtBegin(ISprite** pI)
  {
    *pI = 0;
    rcc_ptr<IUnknown> ppI;
    if ( HRESULT hr = ObjListBase::AtBegin(&+ppI) )
      return hr;
    else
      {
        return ppI->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pI);
      }
  }

HRESULT __stdcall SpritesList::AtEnd(ISprite** pI)
  {
    *pI = 0;
    rcc_ptr<IUnknown> ppI;
    if ( HRESULT hr = ObjListBase::AtEnd(&+ppI) )
      return hr;
    else
      {
        return ppI->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pI);
      }
  }

HRESULT __stdcall SpritesList::At(long idx,ISprite** pI)
  {
    *pI = 0;
    rcc_ptr<IUnknown> ppI;
    if ( HRESULT hr = ObjListBase::At(idx,&+ppI) )
      return hr;
    else
      {
        return ppI->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pI);
      }
  }
