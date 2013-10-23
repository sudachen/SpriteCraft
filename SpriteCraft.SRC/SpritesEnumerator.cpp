
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
#include "SpritesEnumerator.h"
#include "SpriteHelper.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Sprites$Enumerate")
#endif

HRESULT __stdcall SpritesEnumerator::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(ISpritesEnumerator),riid)?S_OK:S_FALSE;
  }

HRESULT __stdcall SpritesEnumerator::get_value(ISprite **pVal)
  {
    _SYNCHRONIZE_;
    if ( value_ != sprites_.End() ) 
      return (*value_)->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pVal);
    else 
      *pVal = 0;
	  return S_OK;
  }

HRESULT __stdcall SpritesEnumerator::Reset(VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    value_ = sprites_.End();
    iter_ = sprites_.Begin();
    *bRet =  ( iter_ == sprites_.End() )?VARIANT_FALSE:VARIANT_TRUE;
	  return S_OK;
  }

HRESULT __stdcall SpritesEnumerator::Next(VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    value_ = iter_;
    if ( iter_ != sprites_.End() ) 
      { 
        ++iter_; 
        *bRet = VARIANT_TRUE; 
      }
    else 
      *bRet = VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall SpritesEnumerator::get_count(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = sprites_.Count();
	  return S_OK;
  }

HRESULT __stdcall SpritesEnumerator::Item(long idx, ISprite **pI)
  {
    _SYNCHRONIZE_;
    if ( idx < 0 || unsigned(idx) >= sprites_.Count() )
      return GenSCWideError(
                _S*L"index %d out of range [0,%d)" %idx %sprites_.Count());
    return sprites_[idx]->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pI);
  }

void SpritesEnumerator::EnumerateSprites(long lnear,long lfar)
  {
    ArrayT<Sprite*> e;
  
    if ( SPRITES::EnumerateSprites(e,lnear,lfar) != SCE_OK )   
      return;
  
    sprites_.Reserve(e.Count());
    for ( Sprite **i = e.Begin(), **iE = e.End(); i != iE; ++i )
      {
        SpriteHelper* helper = Adaptate<SpriteHelper>((*i)->GetSubImpl());
        if ( helper ) sprites_.Push(Refe((ISprite*)helper));
      }
    iter_ = sprites_.Begin();
    value_ = sprites_.End();
  }

HRESULT __stdcall SpritesEnumerator::Get(ISprite **pI)
  {
	  return get_value(pI);
  }
