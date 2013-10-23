
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

#include "scraftapi.h"
#include <stdio.h>
#include <exception>
#include "../Classes/Classes.SRC/string.h"
#include "../Classes/Classes.SRC/guid.h"

using teggo::WideException;
using teggo::Refe;
using teggo::Unrefe;
using teggo::rcc_ptr;
using teggo::FormatT;
using teggo::StringT;
using teggo::BufferT;

#include <windows.h>

#if defined _MSC_VER || defined __MWERKS__ || defined __DMC__
#pragma comment(lib,"rpcrt4.lib")
#pragma comment(lib,"oleaut32.lib")
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"uuid.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"user32.lib")
#endif

static StringT<char>* g_modulepath = 0;

HINSTANCE               Scraft::spritecraft_dll = 0;
IEngine*                Scraft::engine = 0;
Scraft::Terminator*     Scraft::terminators_ = 0;

//Scraft::WideLogF_t      Scraft::WideLogF = 0;
//Scraft::LogF_t          Scraft::LogF = 0;
//Scraft::WideFormat_t    Scraft::WideFormat = 0;
//Scraft::Format_t        Scraft::Format = 0;
Scraft::GetProgramVer_t Scraft::GetProgramVersion = 0;

void Throw_ScraftComException()
  {
    throw ScraftComException();
  }

HRESULT __stdcall ScraftSpriteHandler::OnUpdate(ISprite* sprite,long tickdelta)
  {
    static pwide_t PREFIX = L"<OnUpdate/except>";
    try
      {
        impl_->OnUpdate(tickdelta);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::OnCollide(ISprite* sprite,ISprite* hit)
  {
    static pwide_t PREFIX = L"<OnCollide/except>";
    try
      {
        impl_->OnCollideRaw((ScraftISprite*)hit);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::OnDispose(IDispatch*)
  {
    static pwide_t PREFIX = L"<OnDispose/except>";
    try
      {
        impl_->OnDispose();
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::OnMouseDown(ISprite* sprite,float x, float y,long btn)
  {
    static pwide_t PREFIX = L"<OnMouseDown/except>";
    try
      {
        impl_->OnMouseDown(x,y,btn);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::OnMouseUp(ISprite* sprite,float x, float y,long btn)
  {
    static pwide_t PREFIX = L"<OnMouseUp/except>";
    try
      {
        impl_->OnMouseUp(x,y,btn);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::OnMouseClick(ISprite* sprite,float x, float y,long btn)
  {
    static pwide_t PREFIX = L"<OnMouseClick/except>";
    try
      {
        impl_->OnMouseClick(x,y,btn);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::OnMouseMove(ISprite* sprite,float x, float y)
  {
    static pwide_t PREFIX = L"<OnMouseMove/except>";
    try
      {
        impl_->OnMouseMove(x,y);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::OnMouseOver(ISprite* sprite,VARIANT_BOOL mouseIn)
  {
    static pwide_t PREFIX = L"<OnMouseOver/except>";
    try
      {
        impl_->OnMouseOver(mouseIn!=0);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftSpriteHandler::QueryInterface(REFIID riid,void ** pI)
  {
    if ( (flags_ & SCRAFT_ONUPDATE) && IsEqualIID(riid,TEGGO_GUIDOF(IOnUpdate)) )
      {
        *pI = Refe((IOnUpdate*)this);
        return S_OK;
      }

    if ( flags_ & SCRAFT_ONMOUSEKEY )
      {

        if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseClick)) )
          {
            *pI = Refe((IOnMouseClick*)this);
            return S_OK;
          }

        if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseDown)) )
          {
            *pI = Refe((IOnMouseDown*)this);
            return S_OK;
          }

        if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseUp)) )
          {
            *pI = Refe((IOnMouseUp*)this);
            return S_OK;
          }
      }

    if ( flags_ & SCRAFT_ONMOUSEMOVE )
      {
        if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseMove)) )
          {
            *pI = Refe((IOnMouseMove*)this);
            return S_OK;
          }

        if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseOver)) )
          {
            *pI = Refe((IOnMouseOver*)this);
            return S_OK;
          }
      }

    if ( (flags_ & SCRAFT_ONCOLLIDE) && IsEqualIID(riid,TEGGO_GUIDOF(IOnCollide)) )
      {
        *pI = Refe((IOnCollide*)this);
        return S_OK;
      }

    if ( (flags_ & SCRAFT_ONDISPOSE) && IsEqualIID(riid,TEGGO_GUIDOF(IOnDispose)) )
      {
        *pI = Refe((IOnDispose*)this);
        return S_OK;
      }

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IScraftHandler)) )
      {
        *pI = Refe((IScraftHandler*)this);
        return S_OK;
      }

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IUnknown)) )
      {
        *pI = Refe((IScraftHandler*)this); return S_OK;
      }

    return E_NOINTERFACE;
  }

ScraftSpriteHandler::ScraftSpriteHandler(ScraftSprite* s, u32_t flags)
  : flags_(flags), impl_(s)
  {
  }

ScraftSpriteHandler::~ScraftSpriteHandler()
  {
    REQUIRE ( impl_ == 0 );
  }

ScraftSprite* ScraftSpriteHandler::GetScraftSprite()
  {
    return impl_;
  }

ScraftSprite::ScraftSprite(pwide_t name,u32_t flags)
  : handler_(new ScraftSpriteHandler(this,flags))
  {
    try
      {
        sprite_.reset_(Refe(Scraft::NewDummy((BSTR)name)));
        sprite_->SetSubImpl(handler_->__iunknown());
      }
    catch( ScraftException& )
      {
        handler_->Detach();
        throw;
      }
  }

ScraftSprite::~ScraftSprite()
  {
    handler_->Detach();
    sprite_->Dispose();
  }

void ScraftSprite::OnUpdate(long tickdelta) {}
void ScraftSprite::OnCollide(ScraftSprite*) {}
void ScraftSprite::OnDispose() {}
void ScraftSprite::OnMouseDown(float x,float y,long btn) {}
void ScraftSprite::OnMouseUp(float x,float y,long btn) {}
void ScraftSprite::OnMouseClick(float x,float y,long btn) {}
void ScraftSprite::OnMouseMove(float x,float y) {}
void ScraftSprite::OnMouseOver(bool mouseIn) {}

void ScraftSprite::OnCollideRaw(ScraftISprite* hit)
  {
    rcc_ptr<IScraftHandler> sh;
    if ( SUCCEEDED(((ISprite*)hit)->QueryInterface(TEGGO_GUIDOF(IScraftHandler),(void**)&+sh)) )
      OnCollide(sh->GetScraftSprite());
  }


HRESULT __stdcall  ScraftOnStopAnimation::OnStopAnimation(ISprite* sprite,long cookie,VARIANT_BOOL interrupted)
  {
    static pwide_t PREFIX = L"<OnStopAnimation/except>";
    try
      {
        OnStopAnimation((ScraftISprite*)sprite,cookie,interrupted!=0);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnStopSound::OnStopSound(BSTR name,long channel,long cookie,VARIANT_BOOL interrupted)
  {
    static pwide_t PREFIX = L"<OnStopSound/except>";
    try
      {
        OnStopSound(pwide_t(name),channel,cookie,interrupted!=0);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnUpdate::OnUpdate(ISprite* sprite,long tickdelta)
  {
    static pwide_t PREFIX = L"<OnUpdate/except>";
    try
      {
        OnUpdate((ScraftISprite*)sprite,tickdelta);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnCollide::OnCollide(ISprite* sprite,ISprite* hit)
  {
    static pwide_t PREFIX = L"<OnCollide/except>";
    try
      {
        OnCollide((ScraftISprite*)sprite,(ScraftISprite*)hit);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnDispose::OnDispose(IDispatch*)
  {
    static pwide_t PREFIX = L"<OnDispose/except>";
    try
      {
        OnDispose();
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnMouseDown::OnMouseDown(ISprite* sprite,float x, float y,long btn)
  {
    static pwide_t PREFIX = L"<OnMouseDown/except>";
    try
      {
        OnMouseDown((ScraftISprite*)sprite,x,y,btn);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnMouseUp::OnMouseUp(ISprite* sprite,float x, float y,long btn)
  {
    static pwide_t PREFIX = L"<OnMouseUp/except>";
    try
      {
        OnMouseUp((ScraftISprite*)sprite,x,y,btn);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnMouseClick::OnMouseClick(ISprite* sprite,float x, float y,long btn)
  {
    static pwide_t PREFIX = L"<OnMouseClick/except>";
    try
      {
        OnMouseClick((ScraftISprite*)sprite,x,y,btn);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnMouseMove::OnMouseMove(ISprite* sprite,float x, float y)
  {
    static pwide_t PREFIX = L"<OnMouseMove/except>";
    try
      {
        OnMouseMove((ScraftISprite*)sprite,x,y);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnMouseOver::OnMouseOver(ISprite* sprite,VARIANT_BOOL mouseIn)
  {
    static pwide_t PREFIX = L"<OnMouseOver/except>";
    try
      {
        OnMouseOver((ScraftISprite*)sprite,mouseIn!=0);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftExecutable::Execute(IQue* que,long* sleeptime)
  {
    static pwide_t PREFIX = L"<Execute/except>";
    try
      {
        *sleeptime = Execute((ScraftIQue*)que);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

HRESULT __stdcall ScraftOnLifetimeOut::OnLifetimeOut(IParticles* p)
  {
    static pwide_t PREFIX = L"<OnLifeTimeOut/except>";
    try
      {
        OnLifetimeOut((ScraftIParticles*)p);
        return S_OK;
      }
    catch(std::exception &e)  { Sclog << _S*L"%? %?" %PREFIX %e.what(); }
    catch(WideException &e)   { Sclog << _S*L"%? %?" %PREFIX %e.What(); }
    catch(...)                { Sclog << _S*L"%? %?" %PREFIX %L"unknown exception"; }
    return E_FAIL;
  }

const IID LIBID_SpriteCraft = {0x35E4F96E,0x1B5D,0x4490,{0xB2,0x8B,0xF0,0xA3,0xB2,0x47,0x00,0x20}};
const CLSID CLSID_Engine = {0x03C4B35B,0x0A4B,0x426c,{0xA7,0x94,0x97,0x18,0xD1,0x3A,0x84,0x2C}};
const CLSID CLSID__CoQueueConnectionPoint = {0x38E7A96E,0x445C,0x463a,{0xB1,0x0C,0x84,0x31,0xAA,0x45,0x89,0x61}};
const IID DIID__IQueueEvents = {0x00797D81,0xA843,0x4ffc,{0xB7,0xF1,0x57,0x3C,0x49,0xA2,0x14,0x32}};

#define SCRAFT_COMCALL(obj,meth)            if ( FAILED((obj)->meth()) ) Throw_ScraftComException();
#define SCRAFT_COMCALL1(obj,meth,a)         if ( FAILED((obj)->meth(a)) ) Throw_ScraftComException();
#define SCRAFT_COMCALL2(obj,meth,a,b)       if ( FAILED((obj)->meth(a,b)) ) Throw_ScraftComException();
#define SCRAFT_COMCALL3(obj,meth,a,b,c)     if ( FAILED((obj)->meth(a,b,c)) ) Throw_ScraftComException();
#define SCRAFT_COMCALL4(obj,meth,a,b,c,d)   if ( FAILED((obj)->meth(a,b,c,d)) ) Throw_ScraftComException();
#define SCRAFT_COMCALL_B(obj,meth)          VARIANT_BOOL r; if ( FAILED((obj)->meth(&r)) ) Throw_ScraftComException(); return r!=0;
#define SCRAFT_COMCALL1_B(obj,meth,a)       VARIANT_BOOL r; if ( FAILED((obj)->meth(a,&r)) ) Throw_ScraftComException(); return r!=0;
#define SCRAFT_COMCALL2_B(obj,meth,a,b)     VARIANT_BOOL r; if ( FAILED((obj)->meth(a,b,&r)) ) Throw_ScraftComException(); return r!=0;
#define SCRAFT_COMCALL_T(obj,meth,T)        T r; if ( FAILED((obj)->meth(&r)) ) Throw_ScraftComException(); return r;
#define SCRAFT_COMCALL1_T(obj,meth,a,T)     T r; if ( FAILED((obj)->meth(a,&r)) ) Throw_ScraftComException(); return r;
#define SCRAFT_COMCALL2_T(obj,meth,a,b,T)   T r; if ( FAILED((obj)->meth(a,b,&r)) ) Throw_ScraftComException(); return r;
#define SCRAFT_COMCALL3_T(obj,meth,a,b,c,T) T r; if ( FAILED((obj)->meth(a,b,c,&r)) ) Throw_ScraftComException(); return r;
#define SCRAFT_COMCALL_S(obj,method)        scraft_bstr_guard r; if ( FAILED((obj)->method(r.Ref())) ) Throw_ScraftComException(); return r.Ws();
#define SCRAFT_COMCALL1_S(obj,method,a)     scraft_bstr_guard r; if ( FAILED((obj)->method(a,r.Ref())) ) Throw_ScraftComException(); return r.Ws();
#define SCRAFT_PROP_GET(obj,prop,T)         T r; if ( FAILED((obj)->get_##prop(&r)) ) Throw_ScraftComException(); return r;
#define SCRAFT_PROP_GET_META(obj,prop,T,Meta) T* r; if ( FAILED((obj)->get_##prop(&r)) ) Throw_ScraftComException(); return rcc_ptr<Meta>((Meta*)r);
#define SCRAFT_PROP_iGET(obj,prop,idx,T)    T r; if ( FAILED((obj)->get_##prop(idx,&r)) ) Throw_ScraftComException(); return r;
#define SCRAFT_PROP_GET_B(obj,prop)         VARIANT_BOOL r; if ( FAILED((obj)->get_##prop(&r)) ) Throw_ScraftComException(); return r!=0;
#define SCRAFT_PROP_iGET_B(obj,prop,idx)    VARIANT_BOOL r; if ( FAILED((obj)->get_##prop(idx,&r)) ) Throw_ScraftComException(); return r!=0;
#define SCRAFT_PROP_GET_S(obj,prop)         scraft_bstr_guard r; if ( FAILED((obj)->get_##prop(r.Ref())) ) Throw_ScraftComException(); return r.Ws();
#define SCRAFT_PROP_SET(obj,prop,val)       if ( FAILED((obj)->put_##prop(val)) ) Throw_ScraftComException();
#define SCRAFT_PROP_SET_B(obj,prop,val)     if ( FAILED((obj)->put_##prop(val?VARIANT_TRUE:VARIANT_FALSE)) ) Throw_ScraftComException();
#define SCRAFT_PROP_iSET(obj,prop,idx,val)  if ( FAILED((obj)->put_##prop(idx,val)) ) Throw_ScraftComException();
#define SCRAFT_COMCALL_META(obj,method,T,Meta) T* p = 0; if ( FAILED((obj)->method(&p)) ) Throw_ScraftComException(); return rcc_ptr<Meta>((Meta*)p);
#define SCRAFT_COMCALL1_META(obj,method,a,T,Meta) T* p = 0; if ( FAILED((obj)->method(a,&p)) ) Throw_ScraftComException(); return rcc_ptr<Meta>((Meta*)p);
#define SCRAFT_COMCALL2_META(obj,method,a,b,T,Meta) T* p = 0; if ( FAILED((obj)->method(a,b,&p)) ) Throw_ScraftComException(); return rcc_ptr<Meta>((Meta*)p);
#define SCRAFT_COMCALL3_META(obj,method,a,b,c,T,Meta) T* p = 0; if ( FAILED((obj)->method(a,b,c,&p)) ) Throw_ScraftComException(); return rcc_ptr<Meta>((Meta*)p);

// ----------------------------------------------------------------------------------------------
// Scraft
// ----------------------------------------------------------------------------------------------

void Scraft::SetBackground(u32_t color)
{
  rcc_ptr<IRGBFilter> filt;
  if ( FAILED(Scraft::engine->get_background(&+filt)) ) Throw_ScraftComException();
  if ( FAILED(filt->put_color(color)) ) Throw_ScraftComException();
}

u32_t       Scraft::DisplayEx(u32_t millis)               { SCRAFT_COMCALL1_T(Scraft::engine,DisplayEx,millis,long) }
u32_t       Scraft::DisplayExNoWait(u32_t millis)         { SCRAFT_COMCALL1_T(Scraft::engine,DisplayExNoWait,millis,long) }
bool          Scraft::NextEvent(bool processMsgQue)           { SCRAFT_COMCALL1_B(engine,NextEventEx,processMsgQue?VARIANT_TRUE:VARIANT_FALSE) }
void          Scraft::Display()                               { SCRAFT_COMCALL(engine,Display) }
void          Scraft::SetTitle(pwide_t title)                 { SCRAFT_PROP_SET(engine,title,(BSTR)title) }
__ScString    Scraft::GetTitle()                              { SCRAFT_PROP_GET_S(engine,title) }
void          Scraft::PlaceWindowAt(enum tagPosition pos)     { SCRAFT_COMCALL1(engine,PlaceWindowAt,pos) }
void          Scraft::MoveWindowTo(long x,long y)             { SCRAFT_COMCALL2(engine,MoveWindowTo,x,y) }
void          Scraft::Purge()                                 { SCRAFT_COMCALL(engine,Purge) }
bool          Scraft::EvtIsQuit()                             { SCRAFT_COMCALL_B(engine,EvtIsQuit) }
bool          Scraft::EvtIsESC()                              { SCRAFT_COMCALL_B(engine,EvtIsESC) }
bool          Scraft::EvtIsKeyUp()                            { SCRAFT_COMCALL_B(engine,EvtIsKeyUp) }
bool          Scraft::EvtIsKeyDown()                          { SCRAFT_COMCALL_B(engine,EvtIsKeyDown) }
bool          Scraft::EvtIsMouseMove()                        { SCRAFT_COMCALL_B(engine,EvtIsMouseMove) }
bool          Scraft::EvtIsMouseLeave()                       { SCRAFT_COMCALL_B(engine,EvtIsMouseLeave) }
long          Scraft::EvtKeyEx()                              { SCRAFT_COMCALL_T(engine,EvtKeyEx,long) }
bool          Scraft::EvtIsLClick()                           { SCRAFT_COMCALL_B(engine,EvtIsLClick) }
bool          Scraft::EvtIsRClick()                           { SCRAFT_COMCALL_B(engine,EvtIsRClick) }
void          Scraft::MapKey(tagKeyVal key,tagKeyVal stkey)   { SCRAFT_COMCALL2(engine,MapKey,key,stkey) }
u32_t       Scraft::RefreshInput()                          { SCRAFT_COMCALL_T(engine,EvtKeyEx,long) }
bool          Scraft::EvtIsMouseUp()                          { SCRAFT_COMCALL_B(engine,EvtIsMouseUp) }
bool          Scraft::EvtIsMouseDown()                        { SCRAFT_COMCALL_B(engine,EvtIsMouseDown) }
bool          Scraft::EvtIsMouse()                            { SCRAFT_COMCALL_B(engine,EvtIsMouse) }
bool          Scraft::IsKeyPressed(tagKeyVal key)             { SCRAFT_COMCALL1_B(engine,IsKeyPressed,key) }
bool          Scraft::IsKeyPressedNow(tagKeyVal key)          { SCRAFT_COMCALL1_B(engine,IsKeyPressedNow,key) }
bool          Scraft::IsKeyReleasedNow(tagKeyVal key)         { SCRAFT_COMCALL1_B(engine,IsKeyReleasedNow,key) }
void          Scraft::SleepMillis(u32_t millis)             { SCRAFT_COMCALL1(engine,SleepMillis,millis) }
void          Scraft::SuspendGameClock()                      { SCRAFT_COMCALL(engine,SuspendGameClock) }
void          Scraft::ResumeGameClock()                       { SCRAFT_COMCALL(engine,ResumeGameClock) }
tagEventType  Scraft::EvtEvent()                              { SCRAFT_COMCALL_T(engine,EvtEvent,tagEventType) }
tagKeyVal     Scraft::EvtKey()                                { SCRAFT_COMCALL_T(engine,EvtKey,tagKeyVal) }
long          Scraft::GetMouseX()                             { SCRAFT_PROP_GET(engine,mouseX,long) }
void          Scraft::SetMouseX(long val)                     { SCRAFT_PROP_SET(engine,mouseX,long(val)) }
long          Scraft::GetMouseY()                             { SCRAFT_PROP_GET(engine,mouseY,long) }
void          Scraft::SetMouseY(long val)                     { SCRAFT_PROP_SET(engine,mouseY,long(val)) }
bool          Scraft::IsMouseLDown()                          { SCRAFT_PROP_GET_B(engine,mouseLDown) }
bool          Scraft::IsMouseRDown()                          { SCRAFT_PROP_GET_B(engine,mouseRDown) }
bool          Scraft::NeedRefresh()                           { SCRAFT_PROP_GET_B(engine,needRefresh) }
u32_t       Scraft::GetMillis()                             { SCRAFT_PROP_GET(engine,millis,long) }
bool          Scraft::IsFullscreen()                          { SCRAFT_PROP_GET_B(engine,fullscreen) }
void          Scraft::SetFullscreen(bool val)                 { SCRAFT_PROP_SET(engine,fullscreen,val) }
void          Scraft::ShowFps(bool val)                       { SCRAFT_PROP_SET(engine,showFps,val) }
bool          Scraft::IsMouseIn()                             { SCRAFT_PROP_GET_B(engine,mouseIn); }
__ScString    Scraft::GetRscPath()                            { SCRAFT_PROP_GET_S(engine,rscpath) }
void          Scraft::SetRscPath(pwide_t val)                 { SCRAFT_PROP_SET(engine,rscpath,(BSTR)val) }
void          Scraft::SetSST(pwide_t val)                     { SCRAFT_PROP_SET(engine,SST,(BSTR)val) }
long          Scraft::GetScrWidth()                           { SCRAFT_PROP_GET(engine,scrWidth,long) }
long          Scraft::GetScrHeight()                          { SCRAFT_PROP_GET(engine,scrHeight,long) }
tagVideoMode  Scraft::GetVMode()                              { SCRAFT_PROP_GET(engine,vMode,tagVideoMode) }
void          Scraft::SetVMode(/*tagVideoMode*/ u32_t val)  { SCRAFT_PROP_SET(engine,vMode,tagVideoMode(val)) }
long          Scraft::GetWindowX()                            { SCRAFT_PROP_GET(engine,windowX,long) }
void          Scraft::SetWindowX(long val)                    { SCRAFT_PROP_SET(engine,windowX,val) }
long          Scraft::GetWindowY()                            { SCRAFT_PROP_GET(engine,windowX,long) }
void          Scraft::SetWindowY(long val)                    { SCRAFT_PROP_SET(engine,windowY,val) }
long          Scraft::GetVolume()                             { SCRAFT_PROP_GET(engine,volume,long) }
void          Scraft::SetVolume(LONG val)                     { SCRAFT_PROP_SET(engine,volume,val) }
bool          Scraft::IsSoundSupported()                      { SCRAFT_PROP_GET_B(engine,supportSound) }
bool          Scraft::IsSoundEnabled()                        { SCRAFT_PROP_GET_B(engine,enableSound) }
void          Scraft::SetSoundEnabled(bool val)               { SCRAFT_PROP_SET(engine,enableSound,val) }
u32_t       Scraft::GetFrameTick()                          { SCRAFT_PROP_GET(engine,frameTick,long) }
u32_t       Scraft::GetFrameTickDelta()                     { SCRAFT_PROP_GET(engine,frameTickDelta,long) }
void          Scraft::StopSound(long channel)                 { SCRAFT_COMCALL1(engine,StopSound,channel) }
void          Scraft::Preload(pwide_t rscname)                { SCRAFT_COMCALL1(engine,Preload,(BSTR)rscname) }
void          Scraft::Unload(pwide_t rscname)                 { SCRAFT_COMCALL1(engine,Unload,(BSTR)rscname) }
void          Scraft::UnloadEx(pwide_t rscname,bool f)        { SCRAFT_COMCALL2(engine,UnloadEx,(BSTR)rscname,f?VARIANT_TRUE:VARIANT_FALSE) }
void          Scraft::AllowCollide(long group1, long group2)  { SCRAFT_COMCALL2(engine,AllowCollide,group1,group2) }
void          Scraft::DisallowCollide(long group1, long group2){ SCRAFT_COMCALL2(engine,DisallowCollide,group1,group2) }
void          Scraft::SetCollideMgr(IOnCollide* mgr)          { SCRAFT_COMCALL1(engine,SetCollideMgr,mgr) }
void          Scraft::DumpObjectsList()                       { SCRAFT_COMCALL(engine,DumpObjectsList) }
float         Scraft::GetLayerX(long no)                      { SCRAFT_PROP_iGET(engine,layerX,no,float) }
void          Scraft::SetLayerX(long no, float val)           { SCRAFT_PROP_iSET(engine,layerX,no,val) }
float         Scraft::GetLayerY(long no)                      { SCRAFT_PROP_iGET(engine,layerY,no,float) }
void          Scraft::SetLayerY(long no, float val)           { SCRAFT_PROP_iSET(engine,layerY,no,val) }
bool          Scraft::IsLayerVisible(long no)                 { SCRAFT_PROP_iGET_B(engine,layerVisibility,no) }
void          Scraft::SetLayerVisible(long no,bool val)       { SCRAFT_PROP_iSET(engine,layerVisibility,no,val) }
long          Scraft::GetChannelVolume(long channel)          { SCRAFT_PROP_iGET(engine,channelVolume,channel,long) }
void          Scraft::SetChannelVolume(long channel,long val) { SCRAFT_PROP_iSET(engine,channelVolume,channel,val) }
long          Scraft::GetChannelPan(long channel)             { SCRAFT_PROP_iGET(engine,channelPan,channel,long) }
void          Scraft::SetChannelPan(long channel,long val)    { SCRAFT_PROP_iSET(engine,channelPan,channel,val) }
bool          Scraft::IsChannelMute(long channel)             { SCRAFT_PROP_iGET_B(engine,channelMute,channel) }
void          Scraft::SetChannelMute(long channel,bool val)   { SCRAFT_PROP_iSET(engine,channelMute,channel,val) }
bool          Scraft::IsOffscreenCollideAllowed()             { SCRAFT_PROP_GET_B(engine,offscreenCollide) }
void          Scraft::AllowOffscreenCollide()                 { SCRAFT_PROP_SET(engine,offscreenCollide,VARIANT_TRUE) }
void          Scraft::DisallowOffscreenCollide()              { SCRAFT_PROP_SET(engine,offscreenCollide,VARIANT_FALSE) }
bool          Scraft::IsInvertStereo()                        { SCRAFT_PROP_GET_B(engine,invertStereo) }
void          Scraft::SetInvertStereo(bool val)               { SCRAFT_PROP_SET(engine,invertStereo,val) }
void          Scraft::IncLayerX(long no,float val)            { SCRAFT_COMCALL2(engine,IncLayerX,no,val) }
void          Scraft::IncLayerY(long no,float val)            { SCRAFT_COMCALL2(engine,IncLayerY,no,val) }
bool          Scraft::IsClearColorBits()                      { SCRAFT_PROP_GET_B(engine,clearColorBits) }
void          Scraft::SetClearColorBits(bool val)             { SCRAFT_PROP_SET(engine,clearColorBits,val) }
void          Scraft::SetMouseNotifierRange(long nearLayer,long farLayer) { SCRAFT_COMCALL2(engine,SetMouseNotifierRange,nearLayer,farLayer) }
void          Scraft::ResetMouseNotifierRange()               { SCRAFT_COMCALL(engine,ResetMouseNotifierRange); }
void          Scraft::GrabMouse(const ISpritePtr& sprite)     { SCRAFT_COMCALL1(engine,GrabMouse,sprite->QueryIfs()) }
void          Scraft::GrabMouse(ScraftSprite* sprite)         { GrabMouse(sprite->GetSprite()); }
IXdataPtr     Scraft::ParseXML(pwide_t source,long f)         { SCRAFT_COMCALL2_META(engine,ParseXML,(BSTR)source,(tagParsFlags)f,IXdata,ScraftIXdata) }
IXdataPtr     Scraft::ParseDEF(pwide_t source,long f)         { SCRAFT_COMCALL2_META(engine,ParseDEF,(BSTR)source,(tagParsFlags)f,IXdata,ScraftIXdata) }
ISpritePtr    Scraft::NewDummy(pwide_t name)                  { SCRAFT_COMCALL1_META(engine,NewDummy,(BSTR)name,ISprite,ScraftISprite) }
ISpritePtr    Scraft::FindSprite(pwide_t name)                { SCRAFT_COMCALL1_META(engine,FindSprite,(BSTR)name,ISprite,ScraftISprite) }
ISpritePtr    Scraft::NewSprite_(pwide_t klass,long layer)    { SCRAFT_COMCALL2_META(engine,NewSprite_,(BSTR)klass,layer,ISprite,ScraftISprite) }
ISpritePtr    Scraft::FindSpriteAtMouse(long n,long f)        { SCRAFT_COMCALL2_META(engine,FindSpriteAtMouse,n,f,ISprite,ScraftISprite) }
ITileMapPtr   Scraft::FindTileMap(pwide_t name)               { SCRAFT_COMCALL1_META(engine,FindTileMap,(BSTR)name,ITileMap,ScraftITileMap) }
IExecutorPtr  Scraft::GetExecutor()                           { SCRAFT_PROP_GET_META(engine,executor,IExecutor,ScraftIExecutor) }
void          Scraft::SstDefKlass(pwide_t k,const IXdataPtr& i) { SCRAFT_COMCALL2(engine,SstDefKlass,(BSTR)k,i->QueryIfs()) }
void          Scraft::SstUndefKlass(pwide_t k)                { SCRAFT_COMCALL1(engine,SstUndefKlass,(BSTR)k) }
IXdataPtr     Scraft::SstFindKlass(pwide_t k)                 { SCRAFT_COMCALL1_META(engine,SstFindKlass,(BSTR)k,IXdata,ScraftIXdata) }

long          Scraft::GetMouseDifX()                          { SCRAFT_PROP_GET(engine,mouseDifX,long) }
long          Scraft::GetMouseDifY()                          { SCRAFT_PROP_GET(engine,mouseDifY,long) }
tagMouseMode  Scraft::GetMouseMode()                          { SCRAFT_PROP_GET(engine,mouseMode,tagMouseMode) }
void          Scraft::SetMouseMode(tagMouseMode m)            { SCRAFT_PROP_SET(engine,mouseMode,m) }
void          Scraft::SetVSync(bool val)                      { SCRAFT_PROP_SET_B(engine,vSync,val) }
bool          Scraft::GetVSync()                              { SCRAFT_PROP_GET_B(engine,vSync) }
void          Scraft::SetSafeDraw(bool val)                   { SCRAFT_PROP_SET_B(engine,safeDraw,val) }
bool          Scraft::GetSafeDraw()                           { SCRAFT_PROP_GET_B(engine,safeDraw) }

IParticlesPtr  Scraft::NewParticles(pwide_t name,pwide_t klass,long layer) { SCRAFT_COMCALL3_META(engine,NewParticles,(BSTR)name,(BSTR)klass,layer,IParticles,ScraftIParticles) }
IParticlesPtr  Scraft::NewParticles_(pwide_t klass,long layer)             { SCRAFT_COMCALL2_META(engine,NewParticles_,(BSTR)klass,layer,IParticles,ScraftIParticles) }
IParticlesPtr  Scraft::FindParticles(pwide_t name)                         { SCRAFT_COMCALL1_META(engine,FindParticles,(BSTR)name,IParticles,ScraftIParticles) }

ISpriteEnumPtr Scraft::EnumerateSprites()                     { SCRAFT_COMCALL_META(engine,EnumerateSprites,ISpritesEnumerator,ScraftISpriteEnum) }
ISpriteEnumPtr Scraft::EnumerateSpritesEx(long n,long f)      { SCRAFT_COMCALL2_META(engine,EnumerateSpritesEx,n,f,ISpritesEnumerator,ScraftISpriteEnum) }
IRandomizerPtr Scraft::NewRandomizer()                        { SCRAFT_COMCALL_META(engine,NewRandomizer,IRandomizer,ScraftIRandomizer) }

ISpritesListPtr Scraft::NewSpritesList()                      { SCRAFT_COMCALL_META(engine,NewSpritesList,ISpritesList,ScraftISpritesList) }

long          Scraft::GetScVersion()                          { SCRAFT_PROP_GET(engine,scVersion,long) }
IColorPtr     Scraft::BackgroundColor()                       { SCRAFT_PROP_GET_META(engine,background,IRGBFilter,ScraftIColor) }
void          Scraft::SetNativeCursor(bool val)               { SCRAFT_PROP_SET_B(engine,nativeCursor,val) }
bool          Scraft::IsSoftRefresh()                         { SCRAFT_PROP_GET_B(engine,softRefresh) }
void          Scraft::SetSoftRefresh(bool val)                { SCRAFT_PROP_SET_B(engine,softRefresh,val) }
bool          Scraft::IsPulseRefresh()                        { SCRAFT_PROP_GET_B(engine,pulseRefresh) }
void          Scraft::SetPulseRefresh(bool val)               { SCRAFT_PROP_SET_B(engine,pulseRefresh,val) }
bool          Scraft::IsShowFps()                             { SCRAFT_PROP_GET_B(engine,showFps) }

IXdataPtr     Scraft::NewXdata(pwide_t tag)                   { SCRAFT_COMCALL1_META(engine,NewXdata,(BSTR)tag,IXdata,ScraftIXdata) }
void          Scraft::Update()                                { SCRAFT_COMCALL(engine,Update) }

IConsolePtr   Scraft::GetConsloe()                            { SCRAFT_PROP_GET_META(engine,console,IConsole,ScraftIConsole) }

void          Scraft::AttachTo( HWND hwnd )                   { SCRAFT_COMCALL1(engine,AttachTo,(long)hwnd) }
long          Scraft::GetWndWidth()                           { SCRAFT_PROP_GET(engine,wndWidth,long) }
long          Scraft::GetWndHeight()                          { SCRAFT_PROP_GET(engine,wndHeight,long) }

float         Scraft::GetScrOffsX()                           { SCRAFT_PROP_GET(engine,scrOffsX,float) }
float         Scraft::GetScrOffsY()                           { SCRAFT_PROP_GET(engine,scrOffsY,float) }
void          Scraft::SetScrOffsX(float val)                  { SCRAFT_PROP_SET(engine,scrOffsX,val) }
void          Scraft::SetScrOffsY(float val)                  { SCRAFT_PROP_SET(engine,scrOffsY,val) }
void          Scraft::UnloadAll()                             { SCRAFT_COMCALL(engine,UnloadAll) }

HWND Scraft::GetHWND()
  {
    long p;
    if ( FAILED( engine->get_HWND(&p) ) )
      throw ScraftComException();
    return (HWND)p;
  }

// ?? ??????? ?????????? ???????
void Scraft::Log(pwide_t text)
  {
    engine->Log((BSTR)text);
  }

// ?? ??????? ?????????? ???????
void Scraft::Log(const FormatT<wchar_t>& fmt)
  {
    engine->Log((BSTR)fmt.Str());
  }

StringT<char> Scraft::GetProgramFolder()
  {
    if ( !g_modulepath )
      throw ScraftException(L"folder of program is unknown");
    return *g_modulepath;
  }

void Scraft::PlaySound(pwide_t sound,long channel,IOnStopSound* listener,long cookie)
  {
    if ( FAILED(engine->PlaySound((BSTR)sound,channel,listener,cookie)) )
      Throw_ScraftComException();
  }

ISpritePtr Scraft::NewSprite(pwide_t name,pwide_t klass,long layer, long x, long y)
  {
    ISprite* p;
    if ( FAILED(engine->NewSprite((BSTR)name,(BSTR)klass,layer,x,y,&p)) )
      Throw_ScraftComException();
    return ISpritePtr((ScraftISprite*)p);
  }

bool Scraft::FindSpriteAtMouseEx(long nearLayer,long farLayer,ISpritePtr& isp)
  {
    ISprite* p;
    if ( FAILED(engine->FindSpriteAtMouse(nearLayer,farLayer,&p)) )
      Throw_ScraftComException();
    isp.reset_((ScraftISprite*)p);
    return p!=0;
  }

ITileMapPtr Scraft::NewTileMap(pwide_t name,long columns,long rows,long tileSize,long layer)
  {
    ITileMap* p;
    if ( FAILED(engine->NewTileMap((BSTR)name,columns,rows,tileSize,layer,&p)) )
      Throw_ScraftComException();
    return ITileMapPtr((ScraftITileMap*)p);
  }

ITileMapPtr Scraft::NewTileMap_(long columns,long rows,long tileSize,long layer)
  {
    ITileMap* p;
    if ( FAILED(engine->NewTileMap_(columns,rows,tileSize,layer,&p)) )
      Throw_ScraftComException();
    return ITileMapPtr((ScraftITileMap*)p);
  }

ITileMapPtr Scraft::NewPredefinedTileMap(pwide_t name,pwide_t pattern,long layer)
  {
    ITileMap* p;
    if ( FAILED(engine->NewPredefinedTileMap((BSTR)name,(BSTR)pattern,layer,&p)) )
      Throw_ScraftComException();
    return ITileMapPtr((ScraftITileMap*)p);
  }

void Scraft::Term()
  {
    while ( terminators_ )
      {
        Terminator* term = terminators_;
        terminators_ = terminators_->next_;
        delete term;
      }

    if ( g_modulepath )
      scraft_delete(g_modulepath);

    if ( engine )
      {
        engine->Purge();
        Unrefe(engine);
        if (spritecraft_dll)
          {
            FreeLibrary(spritecraft_dll);
            spritecraft_dll = 0;
          }
        CoUninitialize();
      }
  }

void Scraft::Minimize()
  {
    if ( engine )
      {
        HWND hwnd;
        if ( SUCCEEDED(engine->get_HWND((long*)&hwnd)) )
          ShowWindow(hwnd,SW_MINIMIZE);
      }
  }

void Scraft::Init(tagScDevice dev,bool logging)
  {
    InitEx(dev,VMODE_640x480,logging);
  }

void Scraft::InitEx(tagScDevice dev,u32_t vmode,bool logging)
  {
    struct CoInitializer
      {
        bool deinit_;
        CoInitializer() : deinit_(true) { CoInitialize(0); }
        ~CoInitializer() { if ( deinit_ ) CoUninitialize(); }
        void Forget() { deinit_ = false; }
      };
    CoInitializer coinitializer;

    if ( engine )
      throw ScraftException(L"engine already initialized");
    typedef HRESULT (__stdcall* CreateEngine_t)(SpriteCraftVer,IEngine**);

    teggo::BufferT<char> buf(2048,0);
    spritecraft_dll = 0;

    if ( GetModuleFileNameA(GetModuleHandleA(0),&buf[0],buf.Count()-1) != 0 )
      {
        char* p = &buf[0];
        char* pE = p+buf.Count();
        char* pS = strrchr(p,'\\');
        if ( pS != pE )
          {
            g_modulepath = new StringT<char>;
            g_modulepath->AppendRange(p,pS-p);
            strcpy(pS,"\\scraft.dll.ln");
            FILE* fd = fopen(&buf[0],"rt");
            if ( fd )
              {
                BufferT<char> dllpath(buf.Count()+1024,0);
                memcpy(&dllpath[0],p,pS-p+1);
                void* status = fgets(&dllpath[0]+(pS-p+1),dllpath.Count()-1-(pS-p+1),fd);
                fclose(fd);
                if ( status != 0 )
                  {
                    teggo::replace(dllpath.Begin(),dllpath.End(),'/','\\');
                    teggo::remove(dllpath.Begin(),dllpath.End(),'\n');
                    spritecraft_dll = LoadLibraryA(&dllpath[0]);
                    if ( !spritecraft_dll )
                      throw ScraftException(_S*L"could not load library %?" %&dllpath[0] );
                  }
              }
            else
              {
                strcpy(pS,"\\scraft.dll");
                spritecraft_dll = LoadLibraryA(&buf[0]);
                if ( !spritecraft_dll )
                  {
                    char subkey[128];
                    BufferT<char> dllpath(1024,0);
                    sprintf(subkey,"CLSID\\{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\\InprocServer32",
                      CLSID_Engine.Data1,CLSID_Engine.Data2,CLSID_Engine.Data3,
                      CLSID_Engine.Data4[0],CLSID_Engine.Data4[1],CLSID_Engine.Data4[2],CLSID_Engine.Data4[3],
                      CLSID_Engine.Data4[4],CLSID_Engine.Data4[5],CLSID_Engine.Data4[6],CLSID_Engine.Data4[7]);
                    long sz = dllpath.Count()-1;
                    if ( ERROR_SUCCESS == RegQueryValueA(HKEY_CLASSES_ROOT,subkey,&dllpath[0],&sz) )
                      spritecraft_dll = LoadLibraryA(&dllpath[0]);
                    if ( !spritecraft_dll )
                      throw ScraftException(L"could not find/load spritecraft engine");
                  }
              }
          }
      }

    try
      {
        CreateEngine_t cef = (CreateEngine_t)GetProcAddress(spritecraft_dll,"CreateEngine");

        if ( !cef )
          throw ScraftException(L"could not found function CreateEngine");

        if (  FAILED(cef(SCVersion,&engine))
           || FAILED(engine->put_logging(logging))
           || FAILED(engine->InitEx(dev,tagVideoMode(vmode))) )
           Throw_ScraftComException();

        //LogF = (LogF_t)GetProcAddress(spritecraft_dll,"LogF");
        //WideLogF = (WideLogF_t)GetProcAddress(spritecraft_dll,"WideLogF");
        //Format = (Format_t)GetProcAddress(spritecraft_dll,"Format");
        //WideFormat = (WideFormat_t)GetProcAddress(spritecraft_dll,"WideFormat");
        GetProgramVersion = (GetProgramVer_t)GetProcAddress(spritecraft_dll,"GetProgramVersion");
      }
    catch(...)
      {
        Unrefe(engine);
        if (spritecraft_dll) FreeLibrary(spritecraft_dll);
        spritecraft_dll = 0;
        throw;
      }

    coinitializer.Forget();
  }

// ----------------------------------------------------------------------------------------------
// XML (ScraftIXdata,ScraftIXdataIter)
// ----------------------------------------------------------------------------------------------

IXdataPtr     ScraftIXdata::GetTag(pwide_t name)      { SCRAFT_COMCALL1_META(QueryIfs(),GetTag,(BSTR)name,IXdata,ScraftIXdata) }
IXdataIterPtr ScraftIXdata::Iterate()                 { SCRAFT_COMCALL_META(QueryIfs(),Iterate,IXdataEnumerator,ScraftIXdataIter) }
IXdataIterPtr ScraftIXdata::IterateTag(pwide_t name)  { SCRAFT_COMCALL1_META(QueryIfs(),IterateTag,(BSTR)name,IXdataEnumerator,ScraftIXdataIter) }
__ScString    ScraftIXdata::GetName()                 { SCRAFT_COMCALL_S(QueryIfs(),GetName) }
pwide_t       ScraftIXdata::GetNameAsPWIDE()          { SCRAFT_COMCALL_T(QueryIfs(),GetNameAsPWIDE,wchar_t*) }
__ScString    ScraftIXdata::GetContent()              { SCRAFT_COMCALL_S(QueryIfs(),GetContent) }
pwide_t       ScraftIXdata::GetContentAsPWIDE()       { SCRAFT_COMCALL_T(QueryIfs(),GetContentAsPWIDE,wchar_t*) }
__ScString    ScraftIXdata::GetStrAttr(pwide_t attr)  { SCRAFT_COMCALL1_S(QueryIfs(),GetStrAttr,(BSTR)attr) }
pwide_t       ScraftIXdata::GetStrAttrAsPWIDE(pwide_t attr) { SCRAFT_COMCALL1_T(QueryIfs(),GetStrAttrAsPWIDE,(BSTR)attr,wchar_t*) }
long          ScraftIXdata::GetIntAttr(pwide_t attr)  { SCRAFT_COMCALL1_T(QueryIfs(),GetIntAttr,(BSTR)attr,long) }
float         ScraftIXdata::GetFltAttr(pwide_t attr)  { SCRAFT_COMCALL1_T(QueryIfs(),GetFltAttr,(BSTR)attr,float) }
bool          ScraftIXdata::GetBoolAttr(pwide_t attr) { SCRAFT_COMCALL1_B(QueryIfs(),GetBoolAttr,(BSTR)attr) }
unsigned      ScraftIXdata::Count()                   { SCRAFT_COMCALL_T(QueryIfs(),GetCount,long) }
unsigned      ScraftIXdata::CountTag(pwide_t tag)     { SCRAFT_COMCALL1_T(QueryIfs(),GetCountTag,(BSTR)tag,long) }
void          ScraftIXdata::Freeze()                  { SCRAFT_COMCALL(QueryIfs(),Freeze) }
IXdataPtr     ScraftIXdata::Clone()                   { SCRAFT_COMCALL_META(QueryIfs(),Clone,IXdata,ScraftIXdata) }
bool          ScraftIXdataIter::Reset()               { SCRAFT_COMCALL_B(QueryIfs(),Reset) }
bool          ScraftIXdataIter::Next()                { SCRAFT_COMCALL_B(QueryIfs(),Next) }
IXdataPtr     ScraftIXdataIter::Get()                 { SCRAFT_COMCALL_META(QueryIfs(),Get,IXdata,ScraftIXdata) }
void          ScraftIXdata::SetContent(pwide_t cont)  { SCRAFT_COMCALL1(QueryIfs(),SetContent,(BSTR)cont) }
void          ScraftIXdata::SetStrAttr(pwide_t attr,pwide_t val) { SCRAFT_COMCALL2(QueryIfs(),SetStrAttr,(BSTR)attr,(BSTR)val) }
void          ScraftIXdata::SetIntAttr(pwide_t attr,long val)    { SCRAFT_COMCALL2(QueryIfs(),SetIntAttr,(BSTR)attr,val) }
void          ScraftIXdata::SetFltAttr(pwide_t attr,float val)   { SCRAFT_COMCALL2(QueryIfs(),SetFltAttr,(BSTR)attr,val) }
void          ScraftIXdata::SetBoolAttr(pwide_t attr,bool val)   { SCRAFT_COMCALL2(QueryIfs(),SetBoolAttr,(BSTR)attr,(val?VARIANT_TRUE:VARIANT_FALSE)) }
void          ScraftIXdata::StoreToXML(pwide_t fname, long e )   { SCRAFT_COMCALL2(QueryIfs(),StoreToXML,(BSTR)fname,e) }
void          ScraftIXdata::StoreToDEF(pwide_t fname, long e )   { SCRAFT_COMCALL2(QueryIfs(),StoreToDEF,(BSTR)fname,e) }

__ScString    ScraftIXdata::GetSrcName()              { SCRAFT_COMCALL_S(QueryIfs(),GetSrcName) }
pwide_t       ScraftIXdata::GetSrcNameAsPWIDE()       { SCRAFT_COMCALL_T(QueryIfs(),GetSrcNameAsPWIDE,wchar_t*) }
IXdataPtr     ScraftIXdata::GetRoot()                 { SCRAFT_COMCALL_META(QueryIfs(),GetRoot,IXdata,ScraftIXdata) }

IXdataPtr     ScraftIXdata::Insert(pwide_t tag)       { SCRAFT_COMCALL1_META(QueryIfs(),Insert,(BSTR)tag,IXdata,ScraftIXdata) }
void          ScraftIXdata::Erase()                   { SCRAFT_COMCALL(QueryIfs(),Erase) }

IXdataPtr     ScraftIXdata::InsertCopyOf(IXdataPtr const &c) { SCRAFT_COMCALL2_META(QueryIfs(),InsertCopyOf,c->QueryIfs(),1,IXdata,ScraftIXdata) }


IXdataPtr ScraftIXdata::GetTagSafe(pwide_t name)
  {
    IXdataPtr p( GetTag(name) );
    if ( !p )
      throw ScraftException(_S*L"has no tag %?" %name );
    return p;
  }

long ScraftIXdata::GetLineNo()
  {
    return GetIntAttr(L"$/lineno");
  }

long ScraftIXdata::GetAttrLineNo(pwide_t attr)
  {
    return GetIntAttr(+StringT<wchar_t,64>(_S*L"$/!%?" %attr));
  }

// ----------------------------------------------------------------------------------------------
// Sprite (ScraftISprite)
// ----------------------------------------------------------------------------------------------

void          ScraftISprite::Dispose()                          { SCRAFT_COMCALL(QueryIfs(),Dispose) }
bool          ScraftISprite::Equal(const ISpritePtr& s)         { SCRAFT_COMCALL1_B(QueryIfs(),Equal,(!!s?s->QueryIfs():0)) }
bool          ScraftISprite::Contains(float x,float y)          { SCRAFT_COMCALL2_B(QueryIfs(),Contains,x,y) }
void          ScraftISprite::MoveTo(float x,float y)            { SCRAFT_COMCALL2(QueryIfs(),MoveTo,x,y) }
long          ScraftISprite::GetAttr(long no)                   { SCRAFT_PROP_iGET(QueryIfs(),item,no,long) }
void          ScraftISprite::SetAttr(long no,long val)          { SCRAFT_PROP_iSET(QueryIfs(),item,no,val)}
__ScString    ScraftISprite::GetName()                          { SCRAFT_PROP_GET_S(QueryIfs(),name) }
__ScString    ScraftISprite::GetKlass()                         { SCRAFT_PROP_GET_S(QueryIfs(),klass) }
pwide_t       ScraftISprite::GetNameAsPWIDE()                   { SCRAFT_COMCALL_T(QueryIfs(),GetNameAsPWIDE,wchar_t*) }
pwide_t       ScraftISprite::GetKlassAsPWIDE()                  { SCRAFT_COMCALL_T(QueryIfs(),GetKlassAsPWIDE,wchar_t*) }
long          ScraftISprite::GetLayer()                         { SCRAFT_PROP_GET(QueryIfs(),layer,long) }
void          ScraftISprite::SetLayer(long val)                 { SCRAFT_PROP_SET(QueryIfs(),layer,val) }
long          ScraftISprite::GetSubLayer()                      { SCRAFT_PROP_GET(QueryIfs(),sublayer,long) }
void          ScraftISprite::SetSubLayer(long val)              { SCRAFT_PROP_SET(QueryIfs(),sublayer,val) }
float         ScraftISprite::GetX()                             { SCRAFT_PROP_GET(QueryIfs(),x,float) }
void          ScraftISprite::SetX(float val)                    { SCRAFT_PROP_SET(QueryIfs(),x,val) }
float         ScraftISprite::GetY()                             { SCRAFT_PROP_GET(QueryIfs(),y,float) }
void          ScraftISprite::SetY(float val)                    { SCRAFT_PROP_SET(QueryIfs(),y,val) }
float         ScraftISprite::GetDegree()                        { SCRAFT_PROP_GET(QueryIfs(),degree,float) }
void          ScraftISprite::SetDegree(float val)               { SCRAFT_PROP_SET(QueryIfs(),degree,val) }
float         ScraftISprite::GetAngle()                         { SCRAFT_PROP_GET(QueryIfs(),angle,float) }
void          ScraftISprite::SetAngle(float val)                { SCRAFT_PROP_SET(QueryIfs(),angle,val) }
long          ScraftISprite::GetFrame()                         { SCRAFT_PROP_GET(QueryIfs(),frno,long) }
bool          ScraftISprite::IsVisible()                        { SCRAFT_PROP_GET_B(QueryIfs(),visible) }
void          ScraftISprite::SetVisible(bool val)               { SCRAFT_PROP_SET_B(QueryIfs(),visible,val) }
float         ScraftISprite::GetHeight()                        { SCRAFT_PROP_GET(QueryIfs(),height,float) }
float         ScraftISprite::GetWidth()                         { SCRAFT_PROP_GET(QueryIfs(),width,float) }
float         ScraftISprite::GetSizeX()                         { SCRAFT_PROP_GET(QueryIfs(),xSize,float) }
float         ScraftISprite::GetSizeY()                         { SCRAFT_PROP_GET(QueryIfs(),ySize,float) }
float         ScraftISprite::GetScaleX()                        { SCRAFT_PROP_GET(QueryIfs(),xScale,float) }
float         ScraftISprite::GetScaleY()                        { SCRAFT_PROP_GET(QueryIfs(),yScale,float) }
void          ScraftISprite::SetSizeX(float val)                { SCRAFT_PROP_SET(QueryIfs(),xSize,val) }
void          ScraftISprite::SetSizeY(float val)                { SCRAFT_PROP_SET(QueryIfs(),ySize,val) }
void          ScraftISprite::SetScaleX(float val)               { SCRAFT_PROP_SET(QueryIfs(),xScale,val) }
void          ScraftISprite::SetScaleY(float val)               { SCRAFT_PROP_SET(QueryIfs(),yScale,val) }
void          ScraftISprite::SetScale(float val)                { SCRAFT_PROP_SET(QueryIfs(),xyScale,val) }
float         ScraftISprite::GetTransparency()                  { SCRAFT_PROP_GET(QueryIfs(),transparency,float) }
void          ScraftISprite::SetTransparency(float val)         { SCRAFT_PROP_SET(QueryIfs(),transparency,val) }
float         ScraftISprite::GetLuminance()                     { SCRAFT_PROP_GET(QueryIfs(),luminance,float) }
void          ScraftISprite::SetLuminance(float val)            { SCRAFT_PROP_SET(QueryIfs(),luminance,val) }
bool          ScraftISprite::IsMouseOver()                      { SCRAFT_PROP_GET_B(QueryIfs(),mouseOver) }
void          ScraftISprite::SetHotSpot(tagHotspot val)         { SCRAFT_PROP_SET(QueryIfs(),hotspot,val) }
float         ScraftISprite::GetHotSpotX()                      { SCRAFT_PROP_GET(QueryIfs(),xHotspot,float) }
float         ScraftISprite::GetHotSpotY()                      { SCRAFT_PROP_GET(QueryIfs(),yHotspot,float) }
void          ScraftISprite::SetHotSpotX(float val)             { SCRAFT_PROP_SET(QueryIfs(),xHotspot,val) }
void          ScraftISprite::SetHotSpotY(float val)             { SCRAFT_PROP_SET(QueryIfs(),yHotspot,val) }
bool          ScraftISprite::IsFlippedByX()                     { SCRAFT_PROP_GET_B(QueryIfs(),xFlip) }
bool          ScraftISprite::IsFlippedByY()                     { SCRAFT_PROP_GET_B(QueryIfs(),xFlip) }
void          ScraftISprite::SetFlippedByX(bool val)            { SCRAFT_PROP_SET(QueryIfs(),xFlip,val) }
void          ScraftISprite::SetFlippedByY(bool val)            { SCRAFT_PROP_SET(QueryIfs(),yFlip,val) }
void          ScraftISprite::SetText(pwide_t val)               { SCRAFT_PROP_SET(QueryIfs(),text,(BSTR)val) }
__ScString    ScraftISprite::GetText()                          { SCRAFT_PROP_GET_S(QueryIfs(),text) }
void          ScraftISprite::SetParent(const ISpritePtr& val)   { SCRAFT_PROP_SET(QueryIfs(),parent,(!!val?val->QueryIfs():0)) }
void          ScraftISprite::SetParentNothing()                 { SCRAFT_PROP_SET(QueryIfs(),parent,0) }
float         ScraftISprite::GetScrX()                          { SCRAFT_PROP_GET(QueryIfs(),scrX,float) }
float         ScraftISprite::GetScrY()                          { SCRAFT_PROP_GET(QueryIfs(),scrY,float) }
float         ScraftISprite::GetScrAngle()                      { SCRAFT_PROP_GET(QueryIfs(),scrAngle,float) }
long          ScraftISprite::GetCollideGroup()                  { SCRAFT_PROP_GET(QueryIfs(),collide,long) }
void          ScraftISprite::SetCollideGroup(long val)          { SCRAFT_PROP_SET(QueryIfs(),collide,val) }
bool          ScraftISprite::IsAlive()                          { SCRAFT_PROP_GET_B(QueryIfs(),alive) }
float         ScraftISprite::GetRandom()                        { SCRAFT_PROP_GET(QueryIfs(),random,float) }
long          ScraftISprite::FrCount()                          { SCRAFT_PROP_GET(QueryIfs(),frcount,long) }
long          ScraftISprite::GetCookie()                        { SCRAFT_PROP_GET(QueryIfs(),cookie,long) }
void          ScraftISprite::SetCookie(long val)                { SCRAFT_PROP_SET(QueryIfs(),cookie,val) }
bool          ScraftISprite::IsOutOfScreen()                    { SCRAFT_PROP_GET_B(QueryIfs(),outOfScreen) }

bool          ScraftISprite::Hit(const ISpritePtr& val)         { SCRAFT_COMCALL1_B(QueryIfs(),Hit,val->QueryIfs()) }
void          ScraftISprite::Randomize()                        { SCRAFT_COMCALL(QueryIfs(),Randomize) }

void          ScraftISprite::IncAngle(float val)                { SCRAFT_COMCALL1(QueryIfs(),IncAngle,val) }
void          ScraftISprite::IncX(float val)                    { SCRAFT_COMCALL1(QueryIfs(),IncX,val) }
void          ScraftISprite::IncY(float val)                    { SCRAFT_COMCALL1(QueryIfs(),IncY,val) }

void          ScraftISprite::SpriteToScreen(float* x,float* y)  { SCRAFT_COMCALL2(QueryIfs(),SpriteToScreen,x,y) }
void          ScraftISprite::ScreenToSprite(float* x,float* y)  { SCRAFT_COMCALL2(QueryIfs(),ScreenToSprite,x,y) }
void          ScraftISprite::StopAnimation()                    { SCRAFT_COMCALL(QueryIfs(),StopAnimation) }

IColorPtr     ScraftISprite::Color()                            { SCRAFT_PROP_GET_META(QueryIfs(),cfilt,IRGBFilter,ScraftIColor) }
ISpritePtr    ScraftISprite::GetParent()                        { SCRAFT_PROP_GET_META(QueryIfs(),parent,ISprite,ScraftISprite) }
IHierarchyPtr ScraftISprite::GetHierarchy()                     { SCRAFT_PROP_GET_META(QueryIfs(),hierarchy,IHierarchy,ScraftIHierarchy) }
float         ScraftISprite::Distance(const ISpritePtr& val)    { SCRAFT_COMCALL1_T(QueryIfs(),Distance,val->QueryIfs(),float) }
float         ScraftISprite::AngleWith(const ISpritePtr& val)   { SCRAFT_COMCALL1_T(QueryIfs(),AngleWith,val->QueryIfs(),float) }

ISpritePtr    ScraftISprite::GetChild(pwide_t key)              { SCRAFT_COMCALL1_META(QueryIfs(),GetChild,(BSTR)key,ISprite,ScraftISprite) }
ILwsArrayPtr  ScraftISprite::GetLwsArray()                      { SCRAFT_PROP_GET_META(QueryIfs(),lwsArray,ILwsArray,ScraftILwsArray) }
ITextViewPtr  ScraftISprite::GetTextView()                      { SCRAFT_PROP_GET_META(QueryIfs(),textview,ITextView,ScraftITextView) }
ISpritePrimitivePtr  ScraftISprite::GetPrimitive()              { SCRAFT_PROP_GET_META(QueryIfs(),primitive,ISpritePrimitive,ScraftISpritePrimitive) }

void          ScraftISprite::SetStyle(tagStyle val)             { SCRAFT_PROP_SET(QueryIfs(),style,val) }
tagStyle      ScraftISprite::GetStyle()                         { SCRAFT_PROP_GET(QueryIfs(),style,tagStyle) }

long          ScraftISprite::GetParalax()                       { SCRAFT_PROP_GET(QueryIfs(),paralax,long) }
void          ScraftISprite::SetParalax(long val)               { SCRAFT_PROP_SET(QueryIfs(),paralax,val) }

ScraftSprite* ScraftISprite::QueryScraftSprite()
  {
    if ( rcc_ptr<IScraftHandler> handler = scraft_com_cast<IScraftHandler>(GetSubImpl()) )
      return handler->GetScraftSprite();
    return 0;
  }

void ScraftISprite::AttachChild(pwide_t key,const ISpritePtr& val)
  {
    if ( FAILED((QueryIfs())->AttachChild((BSTR)key,val->QueryIfs())) )
      Throw_ScraftComException();
  }

void ScraftISprite::SetFrame(long frame,pwide_t klass)
  {
    if ( klass )
      {
        if ( FAILED((QueryIfs())->SetFrameOfKlass(frame,(BSTR)klass)) )
          Throw_ScraftComException();
      }
    else
      {
        if ( FAILED((QueryIfs())->put_frno(frame)) )
          Throw_ScraftComException();
      }
  }

void ScraftISprite::AnimateOnce(float fps,long startFrno,long stopFrno,IUnknown* listener,long cookie)
  {
    if ( FAILED((QueryIfs())->AnimateOnce(fps,startFrno,stopFrno,listener,cookie)) )
      Throw_ScraftComException();
  }

void ScraftISprite::AnimateLoop(float fps,long startFrno,long stopFrno,IUnknown* listener,long cookie)
  {
    if ( FAILED((QueryIfs())->AnimateLoop(fps,startFrno,stopFrno,listener,cookie)) )
      Throw_ScraftComException();
  }

void ScraftISprite::SetSubImpl(const IUnknownPtr& iunk)
  {
    if ( FAILED(QueryIfs()->put_subimpl(+iunk)) )
      Throw_ScraftComException();
  }

IUnknownPtr ScraftISprite::GetSubImpl()
  {
    IUnknownPtr iunk;

    if ( FAILED(QueryIfs()->get_subimpl(&+iunk)) )
      Throw_ScraftComException();

    return iunk;
  }

// ----------------------------------------------------------------------------------------------
// Color (ScraftIColor)
// ----------------------------------------------------------------------------------------------

void         ScraftIColor::SetRed(long val)                    { SCRAFT_PROP_SET(QueryIfs(),r,val) }
long         ScraftIColor::GetRed()                            { SCRAFT_PROP_GET(QueryIfs(),r,long) }
void         ScraftIColor::SetGreen(long val)                  { SCRAFT_PROP_SET(QueryIfs(),g,val) }
long         ScraftIColor::GetGreen()                          { SCRAFT_PROP_GET(QueryIfs(),g,long) }
void         ScraftIColor::SetBlue(long val)                   { SCRAFT_PROP_SET(QueryIfs(),b,val) }
long         ScraftIColor::GetBlue()                           { SCRAFT_PROP_GET(QueryIfs(),b,long) }
void         ScraftIColor::SetColor(u32_t val)               { SCRAFT_PROP_SET(QueryIfs(),color,val) }
u32_t      ScraftIColor::GetColor()                          { SCRAFT_PROP_GET(QueryIfs(),color,long) }

// ----------------------------------------------------------------------------------------------
// Executor Queue (ScraftIQue)
// ----------------------------------------------------------------------------------------------

long         ScraftIQue::QueNo()                               { SCRAFT_PROP_GET(QueryIfs(),queno,long) }
u32_t      ScraftIQue::Millis()                              { SCRAFT_PROP_GET(QueryIfs(),millis,long) }
u32_t      ScraftIQue::Delta()                               { SCRAFT_PROP_GET(QueryIfs(),delta,long) }
u32_t      ScraftIQue::CommandTime()                         { SCRAFT_PROP_GET(QueryIfs(),cmdtime,long) }
bool         ScraftIQue::IsSuspended()                         { SCRAFT_COMCALL_B(QueryIfs(),IsSuspended) }
bool         ScraftIQue::IsEmpty()                             { SCRAFT_COMCALL_B(QueryIfs(),IsEmpty) }
void         ScraftIQue::Dismiss()                             { SCRAFT_COMCALL(QueryIfs(),Dismiss) }
void         ScraftIQue::Schedule(IUnknown* command)           { SCRAFT_COMCALL1(QueryIfs(),Schedule,command) }
void         ScraftIQue::Suspend()                             { SCRAFT_COMCALL(QueryIfs(),Suspend) }
void         ScraftIQue::Resume()                              { SCRAFT_COMCALL(QueryIfs(),Resume) }
void         ScraftIQue::Clear()                               { SCRAFT_COMCALL(QueryIfs(),Clear) }
tagQueState  ScraftIQue::ExecuteNext()                         { SCRAFT_COMCALL_T(QueryIfs(),ExecuteNext,tagQueState) }
bool         ScraftIQue::CheckCond(long condno)                { SCRAFT_COMCALL1_B(QueryIfs(),CheckCond,condno) }
void         ScraftIQue::SetCondVal(long condno,bool val)      { SCRAFT_COMCALL2(QueryIfs(),SetCondVal,condno,val) }
void         ScraftIQue::ScheduleSleep(u32_t millis)         { SCRAFT_COMCALL1(QueryIfs(),ScheduleSleep,millis) }
void         ScraftIQue::ScheduleDismiss()                     { SCRAFT_COMCALL(QueryIfs(),ScheduleDismiss) }
void         ScraftIQue::ScheduleHolder(IUnknown* holder)      { SCRAFT_COMCALL1(QueryIfs(),ScheduleHolder,holder) }
void         ScraftIQue::Alert()                               { SCRAFT_COMCALL(QueryIfs(),Alert) }

// ----------------------------------------------------------------------------------------------
// Executor (ScraftIExecutor)
// ----------------------------------------------------------------------------------------------

void         ScraftIExecutor::ScheduleEx(long q,IUnknown* c)   { SCRAFT_COMCALL2(QueryIfs(),ScheduleEx,q,c) }
long         ScraftIExecutor::Schedule(IUnknown* command)      { SCRAFT_COMCALL1_T(QueryIfs(),Schedule,command,long) }
void         ScraftIExecutor::DismissQueue(long queno)         { SCRAFT_COMCALL1(QueryIfs(),DismissQueue,queno) }
IQuePtr      ScraftIExecutor::CreateQueue(long queno)          { SCRAFT_COMCALL1_META(QueryIfs(),CreateQueue,queno,IQue,ScraftIQue) }
IQuePtr      ScraftIExecutor::GetQueue(long queno)             { SCRAFT_COMCALL1_META(QueryIfs(),GetQueue,queno,IQue,ScraftIQue) }


// ----------------------------------------------------------------------------------------------
// TimeMap (ScraftITaileMap)
// ----------------------------------------------------------------------------------------------

IColorPtr    ScraftITileMap::Color()                            { SCRAFT_PROP_GET_META(QueryIfs(),cfilt,IRGBFilter,ScraftIColor) }
void         ScraftITileMap::Dispose()                          { SCRAFT_COMCALL(QueryIfs(),Dispose) }
bool         ScraftITileMap::IsAlive()                          { SCRAFT_PROP_GET_B(QueryIfs(),alive) }
__ScString ScraftITileMap::GetName()                          { SCRAFT_PROP_GET_S(QueryIfs(),name) }
bool         ScraftITileMap::IsVisible()                        { SCRAFT_PROP_GET_B(QueryIfs(),visible) }
void         ScraftITileMap::SetVisible(bool val)               { SCRAFT_PROP_SET_B(QueryIfs(),visible,val) }
float        ScraftITileMap::GetX()                             { SCRAFT_PROP_GET(QueryIfs(),x,float) }
void         ScraftITileMap::SetX(float val)                    { SCRAFT_PROP_SET(QueryIfs(),x,val) }
float        ScraftITileMap::GetY()                             { SCRAFT_PROP_GET(QueryIfs(),y,float) }
void         ScraftITileMap::SetY(float val)                    { SCRAFT_PROP_SET(QueryIfs(),y,val) }
long         ScraftITileMap::GetLayer()                         { SCRAFT_PROP_GET(QueryIfs(),layer,long) }
void         ScraftITileMap::SetLayer(long val)                 { SCRAFT_PROP_SET(QueryIfs(),layer,val) }
long         ScraftITileMap::GetSubLayer()                      { SCRAFT_PROP_GET(QueryIfs(),sublayer,long) }
void         ScraftITileMap::SetSubLayer(long val)              { SCRAFT_PROP_SET(QueryIfs(),sublayer,val) }
float        ScraftITileMap::GetLuminance()                     { SCRAFT_PROP_GET(QueryIfs(),luminance,float) }
void         ScraftITileMap::SetLuminance(float val)            { SCRAFT_PROP_SET(QueryIfs(),luminance,val) }
float        ScraftITileMap::GetTransparency()                  { SCRAFT_PROP_GET(QueryIfs(),transparency,float) }
void         ScraftITileMap::SetTransparency(float val)         { SCRAFT_PROP_SET(QueryIfs(),transparency,val) }
float        ScraftITileMap::GetScaleX()                        { SCRAFT_PROP_GET(QueryIfs(),xScale,float) }
void         ScraftITileMap::SetScaleX(float val)               { SCRAFT_PROP_SET(QueryIfs(),xScale,val) }
float        ScraftITileMap::GetScaleY()                        { SCRAFT_PROP_GET(QueryIfs(),yScale,float) }
void         ScraftITileMap::SetScaleY(float val)               { SCRAFT_PROP_SET(QueryIfs(),yScale,val) }
void         ScraftITileMap::SetScale(float val)                { SCRAFT_PROP_SET(QueryIfs(),xyScale,val) }
long         ScraftITileMap::GetTileSize()                      { SCRAFT_PROP_GET(QueryIfs(),tileSize,long) }
void         ScraftITileMap::SetTileSize(long val)              { SCRAFT_PROP_SET(QueryIfs(),tileSize,val) }
long         ScraftITileMap::GetTileWidth()                     { SCRAFT_PROP_GET(QueryIfs(),tileWidth,long) }
void         ScraftITileMap::SetTileWidth(long val)             { SCRAFT_PROP_SET(QueryIfs(),tileWidth,val) }
long         ScraftITileMap::GetTileHeight()                    { SCRAFT_PROP_GET(QueryIfs(),tileHeight,long) }
void         ScraftITileMap::SetTileHeight(long val)            { SCRAFT_PROP_SET(QueryIfs(),tileHeight,val) }
long         ScraftITileMap::GetRows()                          { SCRAFT_PROP_GET(QueryIfs(),rows,long) }
long         ScraftITileMap::GetCols()                          { SCRAFT_PROP_GET(QueryIfs(),cols,long) }
void         ScraftITileMap::Clear()                            { SCRAFT_COMCALL(QueryIfs(),Clear) }
long         ScraftITileMap::GetTile(long col,long row)         { SCRAFT_COMCALL2_T(QueryIfs(),GetTile,col,row,long) }
bool         ScraftITileMap::IsXCycled()                        { SCRAFT_PROP_GET_B(QueryIfs(),xCycled) }
void         ScraftITileMap::SetXCycled(bool val)               { SCRAFT_PROP_SET_B(QueryIfs(),xCycled,val) }
bool         ScraftITileMap::IsYCycled()                        { SCRAFT_PROP_GET_B(QueryIfs(),yCycled) }
void         ScraftITileMap::SetYCycled(bool val)               { SCRAFT_PROP_SET_B(QueryIfs(),yCycled,val) }
long         ScraftITileMap::GetTileByIdx(long idx)             { SCRAFT_COMCALL1_T(QueryIfs(),GetTileByIdx,idx,long) }
void         ScraftITileMap::Purge()                            { SCRAFT_COMCALL(QueryIfs(),Purge) }
pwide_t      ScraftITileMap::GetNameAsPWIDE()                   { SCRAFT_COMCALL_T(QueryIfs(),GetNameAsPWIDE,wchar_t*) }

void         ScraftITileMap::SetTile(long col,long row,long tilno)  { SCRAFT_COMCALL3(QueryIfs(),SetTile,col,row,tilno) }
void         ScraftITileMap::SetTileByIdx(long idx,long tilno)      { SCRAFT_COMCALL2(QueryIfs(),SetTileByIdx,idx,tilno) }
long         ScraftITileMap::AddTilesFrom(pwide_t klass,long frno,long count) { SCRAFT_COMCALL3_T(QueryIfs(),AddTilesFrom,((BSTR)klass),frno,count,long) }

// ----------------------------------------------------------------------------------------------
// LwsArray (ScraftILwsArray)
// ----------------------------------------------------------------------------------------------

long         ScraftILwsArray::GetCount()                        { SCRAFT_PROP_GET(QueryIfs(),count,long) }
void         ScraftILwsArray::SetCount(long val)                { SCRAFT_PROP_SET(QueryIfs(),count,val) }
bool         ScraftILwsArray::IsCentred()                       { SCRAFT_PROP_GET_B(QueryIfs(),isCentered) }
void         ScraftILwsArray::SetCentred(bool val)              { SCRAFT_PROP_SET_B(QueryIfs(),isCentered,val) }
float        ScraftILwsArray::GetOneX(long no)                  { SCRAFT_COMCALL1_T(QueryIfs(),GetOneX,no,float) }
float        ScraftILwsArray::GetOneY(long no)                  { SCRAFT_COMCALL1_T(QueryIfs(),GetOneY,no,float) }
float        ScraftILwsArray::GetOneAngle(long no)              { SCRAFT_COMCALL1_T(QueryIfs(),GetOneAngle,no,float) }
float        ScraftILwsArray::GetOneScale(long no)              { SCRAFT_COMCALL1_T(QueryIfs(),GetOneScale,no,float) }
float        ScraftILwsArray::GetOneTransparency(long no)       { SCRAFT_COMCALL1_T(QueryIfs(),GetOneTransparency,no,float) }
long         ScraftILwsArray::GetOneFrno(long no)               { SCRAFT_COMCALL1_T(QueryIfs(),GetOneFrno,no,long) }

void         ScraftILwsArray::CopyRange(long dst,long src,long count)        { SCRAFT_COMCALL3(QueryIfs(),CopyRange,dst,src,count) }
void         ScraftILwsArray::ShowRange(long no,long count,bool on)          { SCRAFT_COMCALL3(QueryIfs(),ShowRange,no,count,(on?VARIANT_TRUE:VARIANT_FALSE)) }
void         ScraftILwsArray::SetRangeXY(long no,long count,float x,float y) { SCRAFT_COMCALL4(QueryIfs(),SetRangeXY,no,count,x,y) }
void         ScraftILwsArray::SetRangeAngle(long no,long count,float angle)  { SCRAFT_COMCALL3(QueryIfs(),SetRangeAngle,no,count,angle) }
void         ScraftILwsArray::SetRangeScale(long no,long count,float scale)  { SCRAFT_COMCALL3(QueryIfs(),SetRangeScale,no,count,scale) }
void         ScraftILwsArray::SetRangeTransparency(long no,long cn,float tr) { SCRAFT_COMCALL3(QueryIfs(),SetRangeTransparency,no,cn,tr) }
void         ScraftILwsArray::SetRangeFrno(long no,long count,long frno)     { SCRAFT_COMCALL3(QueryIfs(),SetRangeFrno,no,count,frno) }
void         ScraftILwsArray::DiffRangeScale(long no,long count,float diff)  { SCRAFT_COMCALL3(QueryIfs(),DiffRangeScale,no,count,diff) }
void         ScraftILwsArray::DiffRangeXY(long no,long count,float x,float y){ SCRAFT_COMCALL4(QueryIfs(),DiffRangeXY,no,count,x,y) }
void         ScraftILwsArray::DiffRangeTransparency(long no,long cn,float tr){ SCRAFT_COMCALL3(QueryIfs(),DiffRangeTransparency,no,cn,tr) }
void         ScraftILwsArray::DiffRangeAngle(long no,long count,float diff)  { SCRAFT_COMCALL3(QueryIfs(),DiffRangeAngle,no,count,diff) }
void         ScraftILwsArray::DiffRangeFrno(long no,long count,long diff)    { SCRAFT_COMCALL3(QueryIfs(),DiffRangeFrno,no,count,diff) }
void         ScraftILwsArray::GetRange(MICROSPRITE* s,long no,long count)    { SCRAFT_COMCALL3(QueryIfs(),GetRange,s,no,count) }
void         ScraftILwsArray::SetRange(MICROSPRITE* s,long no,long count)    { SCRAFT_COMCALL3(QueryIfs(),SetRange,s,no,count) }
void         ScraftILwsArray::Order(tagLwsaOrder a,tagHotspot b)             { SCRAFT_COMCALL2(QueryIfs(),Order,a,b) }

void ScraftILwsArray::SetOne(long no,float x,float y,float scale,float angle,float transparency,long frno)
  {
    if ( FAILED(QueryIfs()->SetOne(no,x,y,scale,angle,transparency,frno)) )
      Throw_ScraftComException();
  }

// ----------------------------------------------------------------------------------------------
// Randomizer (ScraftIRandomizer)
// ----------------------------------------------------------------------------------------------

void         ScraftIRandomizer::Reset(u32_t cookie)           { SCRAFT_COMCALL1(QueryIfs(),Reset,cookie) }
float        ScraftIRandomizer::Next()                          { SCRAFT_COMCALL_T(QueryIfs(),Next,float) }

// ----------------------------------------------------------------------------------------------
// SpritesList (ScraftISpritesList)
// ----------------------------------------------------------------------------------------------

bool         ScraftISpritesList::Reset()                        { SCRAFT_COMCALL_B(QueryIfs(),Reset) }
bool         ScraftISpritesList::Next()                         { SCRAFT_COMCALL_B(QueryIfs(),Next) }
void         ScraftISpritesList::Remove()                       { SCRAFT_COMCALL(QueryIfs(),Remove) }
void         ScraftISpritesList::Purge()                        { SCRAFT_COMCALL(QueryIfs(),Purge) }
void         ScraftISpritesList::Add(const ISpritePtr& val)     { SCRAFT_COMCALL1(QueryIfs(),Add,val->QueryIfs()) }
bool         ScraftISpritesList::Find(const ISpritePtr& val)    { SCRAFT_COMCALL1_B(QueryIfs(),Find,val->QueryIfs()) }
ISpritePtr   ScraftISpritesList::Get()                          { SCRAFT_COMCALL_META(QueryIfs(),Get,ISprite,ScraftISprite) }

// ----------------------------------------------------------------------------------------------
// TextView (ScraftITextView)
// ----------------------------------------------------------------------------------------------

float        ScraftITextView::GetScrollX()                      { SCRAFT_PROP_GET(QueryIfs(),xScroll,float) }
void         ScraftITextView::SetScrollX(float val)             { SCRAFT_PROP_SET(QueryIfs(),xScroll,val) }
float        ScraftITextView::GetScrollY()                      { SCRAFT_PROP_GET(QueryIfs(),yScroll,float) }
void         ScraftITextView::SetScrollY(float val)             { SCRAFT_PROP_SET(QueryIfs(),yScroll,val) }
float        ScraftITextView::ScrollMaxX()                      { SCRAFT_PROP_GET(QueryIfs(),xScrollMax,float) }
float        ScraftITextView::ScrollMaxY()                      { SCRAFT_PROP_GET(QueryIfs(),yScrollMax,float) }
float        ScraftITextView::GetWidth()                        { SCRAFT_PROP_GET(QueryIfs(),width,float) }
void         ScraftITextView::SetWidth(float val)               { SCRAFT_PROP_SET(QueryIfs(),width,val) }
float        ScraftITextView::GetHeight()                       { SCRAFT_PROP_GET(QueryIfs(),height,float) }
void         ScraftITextView::SetHeight(float val)              { SCRAFT_PROP_SET(QueryIfs(),height,val) }

// ----------------------------------------------------------------------------------------------
// Particles System (ScraftIParticles)
// ----------------------------------------------------------------------------------------------

long         ScraftIParticles::GetCount()                        { SCRAFT_PROP_GET(QueryIfs(),count,long) }
void         ScraftIParticles::SetCount(long val)                { SCRAFT_PROP_SET(QueryIfs(),count,val) }

__ScString   ScraftIParticles::GetName()                          { SCRAFT_PROP_GET_S((IParticles*)this,name) }
__ScString   ScraftIParticles::GetKlass()                         { SCRAFT_PROP_GET_S((IParticles*)this,klass) }
pwide_t      ScraftIParticles::GetNameAsPWIDE()                   { SCRAFT_COMCALL_T((IParticles*)this,GetNameAsPWIDE,wchar_t*) }
pwide_t      ScraftIParticles::GetKlassAsPWIDE()                  { SCRAFT_COMCALL_T((IParticles*)this,GetKlassAsPWIDE,wchar_t*) }
long         ScraftIParticles::GetLayer()                         { SCRAFT_PROP_GET((IParticles*)this,layer,long) }
void         ScraftIParticles::SetLayer(long val)                 { SCRAFT_PROP_SET((IParticles*)this,layer,val) }
long         ScraftIParticles::GetSubLayer()                      { SCRAFT_PROP_GET((IParticles*)this,sublayer,long) }
void         ScraftIParticles::SetSubLayer(long val)              { SCRAFT_PROP_SET((IParticles*)this,sublayer,val) }
float        ScraftIParticles::GetX()                             { SCRAFT_PROP_GET((IParticles*)this,x,float) }
void         ScraftIParticles::SetX(float val)                    { SCRAFT_PROP_SET((IParticles*)this,x,val) }
float        ScraftIParticles::GetY()                             { SCRAFT_PROP_GET((IParticles*)this,y,float) }
void         ScraftIParticles::SetY(float val)                    { SCRAFT_PROP_SET((IParticles*)this,y,val) }
float        ScraftIParticles::GetAngle()                         { SCRAFT_PROP_GET((IParticles*)this,angle,float) }
void         ScraftIParticles::SetAngle(float val)                { SCRAFT_PROP_SET((IParticles*)this,angle,val) }
bool         ScraftIParticles::IsAlive()                          { SCRAFT_PROP_GET_B((IParticles*)this,alive) }
bool         ScraftIParticles::IsVisible()                        { SCRAFT_PROP_GET_B((IParticles*)this,visible) }
void         ScraftIParticles::SetVisible(bool val)               { SCRAFT_PROP_SET_B((IParticles*)this,visible,val) }
float        ScraftIParticles::GetScaleX()                        { SCRAFT_PROP_GET((IParticles*)this,xScale,float) }
float        ScraftIParticles::GetScaleY()                        { SCRAFT_PROP_GET((IParticles*)this,yScale,float) }
void         ScraftIParticles::SetScaleX(float val)               { SCRAFT_PROP_SET((IParticles*)this,xScale,val) }
void         ScraftIParticles::SetScaleY(float val)               { SCRAFT_PROP_SET((IParticles*)this,yScale,val) }
void         ScraftIParticles::SetScale(float val)                { SCRAFT_PROP_SET((IParticles*)this,xyScale,val) }
float        ScraftIParticles::GetTransparency()                  { SCRAFT_PROP_GET((IParticles*)this,transparency,float) }
void         ScraftIParticles::SetTransparency(float val)         { SCRAFT_PROP_SET((IParticles*)this,transparency,val) }
float        ScraftIParticles::GetLuminance()                     { SCRAFT_PROP_GET((IParticles*)this,luminance,float) }
void         ScraftIParticles::SetLuminance(float val)            { SCRAFT_PROP_SET((IParticles*)this,luminance,val) }
IColorPtr    ScraftIParticles::Color()                            { SCRAFT_PROP_GET_META((IParticles*)this,cfilt,IRGBFilter,ScraftIColor) }

void         ScraftIParticles::Dispose()                          { SCRAFT_COMCALL(QueryIfs(),Dispose) }

void         ScraftIParticles::Reset()                            { SCRAFT_COMCALL(QueryIfs(),Reset) }
void         ScraftIParticles::StartEmission(IUnknown* listener,long cookie)      { SCRAFT_COMCALL2(QueryIfs(),StartEmission,listener,cookie) }
void         ScraftIParticles::StopEmission(long timeout)         { SCRAFT_COMCALL1(QueryIfs(),StopEmission,timeout) }
void         ScraftIParticles::SetSubImpl(IUnknown* val)          { SCRAFT_PROP_SET(QueryIfs(),subimpl,val) }

void         ScraftIParticles::SetEmCount(long emcount)           { SCRAFT_PROP_SET(QueryIfs(),emCount,emcount) }
long         ScraftIParticles::GetEmCount()                       { SCRAFT_PROP_GET(QueryIfs(),emCount,long) }
void         ScraftIParticles::SetLifeTime(long lifetime)         { SCRAFT_PROP_SET(QueryIfs(),lifeTime,lifetime) }
long         ScraftIParticles::GetLifeTime()                      { SCRAFT_PROP_GET(QueryIfs(),lifeTime,long) }
void         ScraftIParticles::SetCycled(bool val)                { SCRAFT_PROP_SET_B(QueryIfs(),cycled,val) }
bool         ScraftIParticles::IsCycled()                         { SCRAFT_PROP_GET_B(QueryIfs(),cycled) }
void         ScraftIParticles::SetAutodispose(bool val)           { SCRAFT_PROP_SET_B(QueryIfs(),autodispose,val) }
bool         ScraftIParticles::IsAutodispose()                    { SCRAFT_PROP_GET_B(QueryIfs(),autodispose) }

void         ScraftIParticles::SetEmitterCf(long cfno, float val0,float val1,long emitter) { SCRAFT_COMCALL4(QueryIfs(),SetEmitterCf,cfno,val0,val1,emitter) }
void         ScraftIParticles::SetEmissionQuantity(long minval,long maxval,long emitter)   { SCRAFT_COMCALL3(QueryIfs(),SetEmissionQuantity,minval,maxval,emitter) }
void         ScraftIParticles::SetEmissionPeriod(long minval,long maxval,long emitter)     { SCRAFT_COMCALL3(QueryIfs(),SetEmissionPeriod,minval,maxval,emitter) }
void         ScraftIParticles::SetEmissionTime(long minval,long maxval,long emitter)       { SCRAFT_COMCALL3(QueryIfs(),SetEmissionTime,minval,maxval,emitter) }
void         ScraftIParticles::SetProgram(pwide_t program, long emitter)                   { SCRAFT_COMCALL2(QueryIfs(),SetProgram,(BSTR)program,emitter) }

void         ScraftIParticles::SetParent(const ISpritePtr& p)     { SCRAFT_PROP_SET(QueryIfs(),parent,p->QueryIfs()) }
void         ScraftIParticles::SetParentNothing()                 { SCRAFT_PROP_SET(QueryIfs(),parent,0) }
ISpritePtr   ScraftIParticles::GetParent()                        { SCRAFT_PROP_GET_META(QueryIfs(),parent,ISprite,ScraftISprite) }
IHierarchyPtr ScraftIParticles::GetHierarchy()                    { SCRAFT_PROP_GET_META(QueryIfs(),hierarchy,IHierarchy,ScraftIHierarchy) }

// ----------------------------------------------------------------------------------------------
// ScraftIHierarchy
// ----------------------------------------------------------------------------------------------
void         ScraftIHierarchy::SetX(bool val)                     { SCRAFT_PROP_SET_B(QueryIfs(),x,val) }
bool         ScraftIHierarchy::GetX()                             { SCRAFT_PROP_GET_B(QueryIfs(),x) }
void         ScraftIHierarchy::SetY(bool val)                     { SCRAFT_PROP_SET_B(QueryIfs(),y,val) }
bool         ScraftIHierarchy::GetY()                             { SCRAFT_PROP_GET_B(QueryIfs(),y) }
void         ScraftIHierarchy::SetAngle(bool val)                 { SCRAFT_PROP_SET_B(QueryIfs(),angle,val) }
bool         ScraftIHierarchy::GetAngle()                         { SCRAFT_PROP_GET_B(QueryIfs(),angle) }
void         ScraftIHierarchy::SetLuminance(bool val)             { SCRAFT_PROP_SET_B(QueryIfs(),luminance,val) }
bool         ScraftIHierarchy::GetLuminance()                     { SCRAFT_PROP_GET_B(QueryIfs(),luminance) }
void         ScraftIHierarchy::SetTransparency(bool val)          { SCRAFT_PROP_SET_B(QueryIfs(),transparency,val) }
bool         ScraftIHierarchy::GetTransparency()                  { SCRAFT_PROP_GET_B(QueryIfs(),transparency) }
void         ScraftIHierarchy::SetColor(bool val)                 { SCRAFT_PROP_SET_B(QueryIfs(),cfilt,val) }
bool         ScraftIHierarchy::GetColor()                         { SCRAFT_PROP_GET_B(QueryIfs(),cfilt) }
void         ScraftIHierarchy::SetScaleX(bool val)                { SCRAFT_PROP_SET_B(QueryIfs(),xScale,val) }
bool         ScraftIHierarchy::GetScaleX()                        { SCRAFT_PROP_GET_B(QueryIfs(),xScale) }
void         ScraftIHierarchy::SetScaleY(bool val)                { SCRAFT_PROP_SET_B(QueryIfs(),yScale,val) }
bool         ScraftIHierarchy::GetScaleY()                        { SCRAFT_PROP_GET_B(QueryIfs(),yScale) }
void         ScraftIHierarchy::SetVisibility(bool val)            { SCRAFT_PROP_SET_B(QueryIfs(),visibility,val) }
bool         ScraftIHierarchy::GetVisibility()                    { SCRAFT_PROP_GET_B(QueryIfs(),visibility) }
void         ScraftIHierarchy::SetParentDispose(bool val)         { SCRAFT_PROP_SET_B(QueryIfs(),parentDispose,val) }
bool         ScraftIHierarchy::GetParentDispose()                 { SCRAFT_PROP_GET_B(QueryIfs(),parentDispose) }

// ----------------------------------------------------------------------------------------------
// ScraftISpritePrimitive
// ----------------------------------------------------------------------------------------------
long         ScraftISpritePrimitive::GetCount()                   { SCRAFT_PROP_GET(QueryIfs(),count,long) }
void         ScraftISpritePrimitive::SetCount(long val)           { SCRAFT_PROP_SET(QueryIfs(),count,val) }
float        ScraftISpritePrimitive::GetOffsetX()                 { SCRAFT_PROP_GET(QueryIfs(),xOffset,float) }
void         ScraftISpritePrimitive::SetOffsetX(float val)        { SCRAFT_PROP_SET(QueryIfs(),xOffset,val) }
float        ScraftISpritePrimitive::GetOffsetY()                 { SCRAFT_PROP_GET(QueryIfs(),yOffset,float) }
void         ScraftISpritePrimitive::SetOffsetY(float val)        { SCRAFT_PROP_SET(QueryIfs(),yOffset,val) }
bool         ScraftISpritePrimitive::IsClip()                     { SCRAFT_PROP_GET_B(QueryIfs(),clip) }
void         ScraftISpritePrimitive::SetClip(bool val)            { SCRAFT_PROP_SET_B(QueryIfs(),clip,val) }
bool         ScraftISpritePrimitive::IsColorized()                { SCRAFT_PROP_GET_B(QueryIfs(),colorized) }
void         ScraftISpritePrimitive::SetColorized(bool val)       { SCRAFT_PROP_SET_B(QueryIfs(),colorized,val) }
tagBlendMode ScraftISpritePrimitive::GetBlendMode()               { SCRAFT_PROP_GET(QueryIfs(),blendMode,tagBlendMode) }
void         ScraftISpritePrimitive::SetBlendMode(tagBlendMode val) { SCRAFT_PROP_SET(QueryIfs(),blendMode,val) }
bool         ScraftISpritePrimitive::IsNormUV()                   { SCRAFT_PROP_GET_B(QueryIfs(),uvNorm) }
void         ScraftISpritePrimitive::SetNormUV(bool val)          { SCRAFT_PROP_SET_B(QueryIfs(),uvNorm,val) }
bool         ScraftISpritePrimitive::IsCW()                       { SCRAFT_PROP_GET_B(QueryIfs(),cw) }
void         ScraftISpritePrimitive::SetCW(bool val)              { SCRAFT_PROP_SET_B(QueryIfs(),cw,val) }
void         ScraftISpritePrimitive::SetColor(long no,long count,long color) { SCRAFT_COMCALL3(QueryIfs(),SetColor,no,count,color) }
void         ScraftISpritePrimitive::SetXY(long no,float x,float y) { SCRAFT_COMCALL3(QueryIfs(),SetXY,no,x,y) }
void         ScraftISpritePrimitive::SetUV(long no,float u,float v) { SCRAFT_COMCALL3(QueryIfs(),SetUV,no,u,v) }
float        ScraftISpritePrimitive::GetX(long no)                { SCRAFT_COMCALL1_T(QueryIfs(),GetX,no,float) }
float        ScraftISpritePrimitive::GetY(long no)                { SCRAFT_COMCALL1_T(QueryIfs(),GetY,no,float) }
float        ScraftISpritePrimitive::GetU(long no)                { SCRAFT_COMCALL1_T(QueryIfs(),GetU,no,float) }
float        ScraftISpritePrimitive::GetV(long no)                { SCRAFT_COMCALL1_T(QueryIfs(),GetV,no,float) }
long         ScraftISpritePrimitive::GetColor(long no)            { SCRAFT_COMCALL1_T(QueryIfs(),GetColor,no,long) }
void         ScraftISpritePrimitive::SetVertexEx(long no,struct PRIMITIVE_VERTEX* vertex,long count) { SCRAFT_COMCALL3(QueryIfs(),SetVertexEx,no,vertex,count) }
void         ScraftISpritePrimitive::GetVertexEx(long no,struct PRIMITIVE_VERTEX* vertex,long count) { SCRAFT_COMCALL3(QueryIfs(),GetVertexEx,no,vertex,count) }

void ScraftISpritePrimitive::SetVertex(long no,long color,float x,float y,float u,float v)
  {
    if ( FAILED(QueryIfs()->SetVertex(no,color,x,y,u,v)) )
      Throw_ScraftComException();
  }

void ScraftISpritePrimitive::DiffVertex(long no,long count,float x,float y,float u,float v)
  {
    if ( FAILED(QueryIfs()->DiffVertex(no,count,x,y,u,v)) )
      Throw_ScraftComException();
  }


// ----------------------------------------------------------------------------------------------
// ScraftIConsole
// ----------------------------------------------------------------------------------------------
void        ScraftIConsole::Println(pwide_t text) { SCRAFT_COMCALL1(QueryIfs(),Println,(BSTR)text) }
void        ScraftIConsole::Print(pwide_t text)   { SCRAFT_COMCALL1(QueryIfs(),Print,(BSTR)text) }
void        ScraftIConsole::Clear()               { SCRAFT_COMCALL(QueryIfs(),Clear) }
bool        ScraftIConsole::ProcessInput()        { SCRAFT_COMCALL_B(QueryIfs(),ProcessInput) }
void        ScraftIConsole::SetLines(long val)    { SCRAFT_PROP_SET(QueryIfs(),lines,val) }
long        ScraftIConsole::GetLines()            { SCRAFT_PROP_GET(QueryIfs(),lines,long) }
void        ScraftIConsole::SetMaxLines(long val) { SCRAFT_PROP_SET(QueryIfs(),maxlines,val) }
long        ScraftIConsole::GetMaxLines()         { SCRAFT_PROP_GET(QueryIfs(),maxlines,long) }
void        ScraftIConsole::SetVisible(bool val)  { SCRAFT_PROP_SET_B(QueryIfs(),visible,val) }
bool        ScraftIConsole::IsVisible()           { SCRAFT_PROP_GET_B(QueryIfs(),visible) }
void        ScraftIConsole::SetTransparent(bool val) { SCRAFT_PROP_SET_B(QueryIfs(),transparent,val) }
bool        ScraftIConsole::IsTransparent()       { SCRAFT_PROP_GET_B(QueryIfs(),transparent) }
void        ScraftIConsole::SetCmdLine(pwide_t c) { SCRAFT_PROP_SET(QueryIfs(),cmdline,(BSTR)c) }
__ScString  ScraftIConsole::GetCmdLine()          { SCRAFT_PROP_GET_S(QueryIfs(),cmdline) }

//#include "actorscript_.inl"
