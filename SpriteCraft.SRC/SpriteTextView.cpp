
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
#include "RBGFilter.h"
#include "CollideMgr.h"
#include "SpriteHelper.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Sprites$TextView")
#endif

static pwide_t NOT_EXISTS_INFO = L"Sprite is not exists";

SpriteTextView::SpriteTextView(Sprite *sprite) 
  : sprite_(sprite) 
  {
  }

SpriteTextView::~SpriteTextView() 
  {
  }

SpriteTextView* SpriteTextView::AsTextView()  
  { 
    return this; 
  }

void SpriteTextView::Displace() 
  { 
    sprite_ = 0; 
  }

HRESULT __stdcall SpriteTextView::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(ITextView),riid)?S_OK:S_FALSE;
  }

HRESULT __stdcall SpriteTextView::get_xScroll(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTextViewScrollX();
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::put_xScroll(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetTextViewScrollX(newVal);
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::get_yScroll(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTextViewScrollY();
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::put_yScroll(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetTextViewScrollY(newVal);
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::get_xScrollMax(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTextViewMaxScrollX();
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::get_yScrollMax(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTextViewMaxScrollY();
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::get_width(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTextViewWidth();
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::put_width(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetTextViewWidth(newVal);
	  return S_OK;
  }

HRESULT __stdcall SpriteTextView::get_height(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTextViewHeight();
	  return S_OK;
  }
