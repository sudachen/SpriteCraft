
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
#pragma code_seg(".text$AAA$Sprites$Hierarchy")
#endif

static pwide_t NOT_EXISTS_INFO = L"Sprite is not exists";

Hierarchy::Hierarchy(ISpriteHelperEx* helper) 
  : helper_(Refe(helper)) 
  {
  }

Hierarchy::~Hierarchy() 
  {
  }

HRESULT __stdcall Hierarchy::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(IHierarchy),riid)?S_OK:S_FALSE;
  }

static HRESULT PutParentDeps(ISpriteHelperEx* helper,VARIANT_BOOL v,u32_t mask) 
  {
    _SYNCHRONIZE_;

    if ( !helper ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    
    u32_t deps = helper->Ex_GetParentDeps();
    if ( deps == 0xffffffff ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    
    deps &= ~mask;
    
    if (v!=VARIANT_FALSE) 
      deps |= mask;
    
    helper->Ex_SetParentDeps(deps);
    
    return S_OK;
  }

static HRESULT GetParentDeps(ISpriteHelperEx* helper,VARIANT_BOOL* p,u32_t mask) 
  {
    _SYNCHRONIZE_;

    if ( !helper  ) 
      return GenSCWideError(NOT_EXISTS_INFO);

    u32_t deps = helper->Ex_GetParentDeps();
    if ( deps == 0xffffffff ) 
      return GenSCWideError(NOT_EXISTS_INFO);
    
    *p = (deps & mask)?VARIANT_TRUE:VARIANT_FALSE;
    
    return S_OK;
  }

HRESULT __stdcall Hierarchy::get_x(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_X); 
  }

HRESULT __stdcall Hierarchy::get_y(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_Y); 
  }

HRESULT __stdcall Hierarchy::get_angle(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_DEGREE); 
  }

HRESULT __stdcall Hierarchy::get_luminance(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_LUMINANCE); 
  }

HRESULT __stdcall Hierarchy::get_transparency(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_TRANSPARENCY); 
  }

HRESULT __stdcall Hierarchy::get_xScale(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_XSCALE); 
  }

HRESULT __stdcall Hierarchy::get_yScale(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_YSCALE); 
  }

HRESULT __stdcall Hierarchy::get_cfilt(VARIANT_BOOL* p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_CFILT); 
  }

HRESULT __stdcall Hierarchy::get_visibility(VARIANT_BOOL *p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_VISIBILITY); 
  }

HRESULT __stdcall Hierarchy::get_parentDispose(VARIANT_BOOL *p) 
  { 
    return GetParentDeps(+helper_,p,SPRITE_DEPS_DISPOSE); 
  }

HRESULT __stdcall Hierarchy::put_x(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_X); 
  }

HRESULT __stdcall Hierarchy::put_y(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_Y); 
  }

HRESULT __stdcall Hierarchy::put_angle(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_DEGREE); 
  }

HRESULT __stdcall Hierarchy::put_luminance(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_LUMINANCE); 
  }

HRESULT __stdcall Hierarchy::put_transparency(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_TRANSPARENCY); 
  }

HRESULT __stdcall Hierarchy::put_xScale(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_XSCALE); 
  }

HRESULT __stdcall Hierarchy::put_yScale(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_YSCALE); 
  }

HRESULT __stdcall Hierarchy::put_cfilt(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_CFILT); 
  }

HRESULT __stdcall Hierarchy::put_visibility(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_VISIBILITY); 
  }

HRESULT __stdcall Hierarchy::put_parentDispose(VARIANT_BOOL p) 
  { 
    return PutParentDeps(+helper_,p,SPRITE_DEPS_DISPOSE); 
  }

