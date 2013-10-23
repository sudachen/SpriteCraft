
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
#include "dispsubimpl.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Engine")
#endif

HRESULT Engine_DispSubImpl::InternalQueryInterface2(REFIID riid ,void ** pI) 
  {
    if ( mode_ == DISPATCH 
      && IsEqualIID(riid,TEGGO_GUIDOF(IDispatch)) ) 
      return impl_->QueryInterface(riid,pI);

    if ( disp_IOnUpdate_OnUpdate != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnUpdate)) ) 
      return *pI = Refe((IOnUpdate*)this), S_OK;
    
    if ( disp_IOnCollide_OnCollide != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnCollide)) ) 
      return *pI = Refe((IOnCollide*)this), S_OK;
    
    if ( disp_IOnMouseClick_OnMouseClick != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseClick)) ) 
      return *pI = Refe((IOnMouseClick*)this), S_OK;
    
    if ( disp_IOnMouseUp_OnMouseUp != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseUp)) ) 
      return *pI = Refe((IOnMouseUp*)this), S_OK;
    
    if ( disp_IOnMouseDown_OnMouseDown != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseDown)) ) 
      return *pI = Refe((IOnMouseDown*)this), S_OK;
    
    if ( disp_IOnMouseMove_OnMouseMove != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseMove)) ) 
      return *pI = Refe((IOnMouseMove*)this), S_OK;
    
    if ( disp_IOnMouseOver_OnMouseOver != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseOver)) ) 
      return *pI = Refe((IOnMouseOver*)this), S_OK;
    
    if ( disp_IOnDispose_OnDispose != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnDispose)) ) 
      return *pI = Refe((IOnDispose*)this), S_OK;
    
    if ( disp_IOnLifetimeOut_OnLifetimeOut != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnLifetimeOut)) ) 
      return *pI = Refe((IOnLifetimeOut*)this), S_OK;
    
    if ( disp_IOnEmission_OnEmission != disp_QI 
      && IsEqualIID(riid,TEGGO_GUIDOF(IOnEmission)) ) 
      return *pI = Refe((IOnEmission*)this), S_OK;
    
    if ( IsEqualIID(riid,TEGGO_GUIDOF(IUnknown)) )
      return *pI = Refe((IOnDispose*)this), S_OK;
    
    if ( IsEqualIID(riid,TEGGO_GUIDOF(Engine_IDispSubImpl)) )
      return *pI = Refe((Engine_IDispSubImpl*)this), S_OK;
    
    return E_NOINTERFACE;
  }

void Engine_DispSubImpl::GetDispId(pwide_t method,long* dispid,long def) 
  {
    if ( FAILED(impl_->GetIDsOfNames(
            IID_NULL,(LPOLESTR*)&method,1,LOCALE_SYSTEM_DEFAULT,dispid)) )
      *dispid = def;
  }

Engine_DispSubImpl::Engine_DispSubImpl(IDispatch* impl,MODE mode) 
  : impl_(Refe(impl)) , mode_(mode)
  {
    GetDispId(L"QueryInterface", &disp_QI, -1);
    
    if ( mode == DISPATCH ) 
      {
        GetDispId(L"IOnUpdate_OnUpdate",        &disp_IOnUpdate_OnUpdate,         disp_QI);
        GetDispId(L"IOnCollide_OnCollide",      &disp_IOnCollide_OnCollide,       disp_QI);
        GetDispId(L"IOnMouseClick_OnMouseClick",&disp_IOnMouseClick_OnMouseClick, disp_QI); 
        GetDispId(L"IOnMouseUp_OnMouseUp",      &disp_IOnMouseUp_OnMouseUp,       disp_QI); 
        GetDispId(L"IOnMouseDown_OnMouseDown",  &disp_IOnMouseDown_OnMouseDown,   disp_QI); 
        GetDispId(L"IOnMouseMove_OnMouseMove",  &disp_IOnMouseMove_OnMouseMove,   disp_QI); 
        GetDispId(L"IOnMouseOver_OnMouseOver",  &disp_IOnMouseOver_OnMouseOver,   disp_QI); 
        GetDispId(L"IOnDispose_OnDispose",      &disp_IOnDispose_OnDispose,       disp_QI); 
        GetDispId(L"IOnLifetimeOut_OnLifetimeOut",&disp_IOnLifetimeOut_OnLifetimeOut,disp_QI);
        GetDispId(L"IOnEmission_OnEmission",    &disp_IOnEmission_OnEmission,     disp_QI);
      }
    else // ADVISE
      {
        disp_IOnUpdate_OnUpdate       = 2;
        disp_IOnCollide_OnCollide     = 1;
        disp_IOnMouseClick_OnMouseClick = 7;
        disp_IOnMouseUp_OnMouseUp     = 5;
        disp_IOnMouseDown_OnMouseDown = 4;
        disp_IOnMouseMove_OnMouseMove = 3;
        disp_IOnMouseOver_OnMouseOver = 6;
        disp_IOnLifetimeOut_OnLifetimeOut = 100;
        disp_IOnEmission_OnEmission       = 101;  
        disp_IOnDispose_OnDispose     = 1000;
      }
  }

HRESULT __stdcall Engine_DispSubImpl::OnUpdate(ISprite* sprite,long tickdelta)
  {
    if ( disp_IOnUpdate_OnUpdate != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnUpdate_OnUpdate)
                               % sprite % tickdelta).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnCollide(ISprite* sprite,ISprite* hit)
  {
    if ( disp_IOnCollide_OnCollide != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnCollide_OnCollide)
                               % sprite % hit).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnDispose(IDispatch* obj)
  {
    if ( Engine_IsAlive() ) 
      if ( disp_IOnDispose_OnDispose != disp_QI )
        return (teggo::DispInvoker(+impl_,disp_IOnDispose_OnDispose)
                                  % obj).Invoke().hResult_;
      else 
        return E_FAIL;
    else
      return S_OK;
  }

HRESULT __stdcall Engine_DispSubImpl::OnMouseDown(ISprite* sprite, float x, float y,long btn)
  {
    if ( disp_IOnMouseDown_OnMouseDown != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnMouseDown_OnMouseDown)
                                % sprite % x % y % btn).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnMouseUp(ISprite* sprite, float x, float y,long btn)
  {
    if ( disp_IOnMouseUp_OnMouseUp != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnMouseUp_OnMouseUp)
                                % sprite % x % y % btn).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnMouseClick(ISprite* sprite, float x, float y,long btn)
  {
    if ( disp_IOnMouseClick_OnMouseClick != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnMouseClick_OnMouseClick)
                                % sprite % x % y % btn).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnMouseMove(ISprite* sprite, float x, float y)
  {
    if ( disp_IOnMouseMove_OnMouseMove != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnMouseMove_OnMouseMove)
                                % sprite % x % y).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnMouseOver(ISprite* sprite, VARIANT_BOOL mouseIn)
  {
    if ( disp_IOnMouseOver_OnMouseOver != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnMouseOver_OnMouseOver)
                                % sprite % (mouseIn!=VARIANT_FALSE)).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnLifetimeOut(IParticles* particles)
  {
    if ( disp_IOnLifetimeOut_OnLifetimeOut != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnLifetimeOut_OnLifetimeOut)
                                % particles).Invoke().hResult_;
    else 
      return E_FAIL;
  }

HRESULT __stdcall Engine_DispSubImpl::OnEmission(IParticles* particles, long tickdelta)
  {
    if ( disp_IOnEmission_OnEmission != disp_QI )
      return (teggo::DispInvoker(+impl_,disp_IOnEmission_OnEmission)
                                % particles % tickdelta).Invoke().hResult_;
    else 
      return E_FAIL;
  }
