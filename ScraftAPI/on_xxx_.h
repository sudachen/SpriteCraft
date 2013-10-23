
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

struct CXX_NO_VTABLE ScraftOnStopAnimation : public TeggoIfs0 < IOnStopAnimation >
  {
    virtual void OnStopAnimation(ScraftISprite*,long cookie,bool interrupted) = 0;
    virtual HRESULT __stdcall OnStopAnimation(ISprite* sprite,long cookie,VARIANT_BOOL interrupted);
  };

struct CXX_NO_VTABLE ScraftOnStopSound : public TeggoIfs0 < IOnStopSound >
  {
    virtual void OnStopSound(pwide_t name,long channel,long cookie,bool interrupted) = 0;
    virtual HRESULT __stdcall OnStopSound(BSTR name,long channel,long cookie,VARIANT_BOOL interrupted);
  };

struct CXX_NO_VTABLE ScraftOnUpdate : public TeggoIfs0 < IOnUpdate >
  {
    virtual void OnUpdate(ScraftISprite*,long tickdelta) = 0;
    virtual HRESULT __stdcall OnUpdate(ISprite* sprite,long tickdelta);
  };

struct CXX_NO_VTABLE ScraftOnCollide : public TeggoIfs0 < IOnCollide >
  {
    virtual void OnCollide(ScraftISprite*,ScraftISprite* hit) = 0;
    virtual HRESULT __stdcall OnCollide(ISprite* sprite,ISprite* hit);
  };

struct CXX_NO_VTABLE ScraftOnDispose : public TeggoIfs0 < IOnDispose >
  {
    virtual void OnDispose() = 0;
    virtual HRESULT __stdcall OnDispose(IDispatch*);
  };

struct CXX_NO_VTABLE ScraftOnMouseDown : public TeggoIfs0 < IOnMouseDown >
  {
    virtual void OnMouseDown(ScraftISprite*,float x,float y,long btn) = 0;
    virtual HRESULT __stdcall OnMouseDown(ISprite*, float x, float y,long btn);
  };

struct CXX_NO_VTABLE ScraftOnMouseUp : public TeggoIfs0 < IOnMouseUp >
  {
    virtual void OnMouseUp(ScraftISprite*,float x,float y,long btn) = 0;
    virtual HRESULT __stdcall OnMouseUp(ISprite*, float x, float y,long btn);
  };

struct CXX_NO_VTABLE ScraftOnMouseClick : public TeggoIfs0 < IOnMouseClick >
  {
    virtual void OnMouseClick(ScraftISprite*,float x,float y,long btn) = 0;
    virtual HRESULT __stdcall OnMouseClick(ISprite*, float x, float y,long btn);
  };

struct CXX_NO_VTABLE ScraftOnMouseMove : public TeggoIfs0 < IOnMouseMove >
  {
    virtual void OnMouseMove(ScraftISprite*,float x,float y) = 0;
    virtual HRESULT __stdcall OnMouseMove(ISprite*, float x, float y);
  };

struct CXX_NO_VTABLE ScraftOnMouseOver : public TeggoIfs0 < IOnMouseOver >
  {
    virtual void OnMouseOver(ScraftISprite*,bool mouseIn) = 0;
    virtual HRESULT __stdcall OnMouseOver(ISprite*, VARIANT_BOOL mouseIn);
  };

struct CXX_NO_VTABLE ScraftOnLifetimeOut : public TeggoIfs0 < IOnLifetimeOut >
  {
    virtual void OnLifetimeOut(ScraftIParticles* particles) = 0;
    virtual HRESULT __stdcall OnLifetimeOut(IParticles* particles);
  };
