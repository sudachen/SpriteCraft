
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

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

#include "StdAfx.h"
#include "Particles.h"
#include "dispsubimpl.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Particles")
#endif

using namespace nvmath;
extern "C" PARTICLES_ENGINE *MakeDefaultPsEngine();

void ParticlesHelper::IColorHelper_SetColor(mrgb_t c)
  {
    if ( !sprite_ ) return;
    sprite_->SetRGB(c);
  }

mrgb_t ParticlesHelper::IColorHelper_GetColor() const
  {
    if ( !sprite_ ) return mRGB_t(0,0,0);
    return sprite_->GetRGB();
  }


HRESULT CreateParticlesHalper(Sprite* p,IParticles** pI,bool noname)
  {
    ParticlesHelper* pp = new XComObject__<ParticlesHelper>;
    p->SetSubImpl(pp);
    *pI = pp;
    pp->noname_ = noname;
    return S_OK;
  }

pchar_t const ParticlesHelper::TypeID__ = "#TYPEID#SpriteCraft::ParticlesHelper";
static pwide_t NOT_EXISTS_INFO = L"Particles system is not exists";

WeakParticles::WeakParticles(ParticlesHelper* s) : ph_(s)
  {
  }

WeakParticles::~WeakParticles()
  {
  }

void WeakParticles::ForgetParticles()
  {
    ph_ = 0;
  }

bool WeakParticles::OnUpdateListener(u32_t tickdelta)
  {
    return  !ph_ ? false : ph_->Update_(tickdelta);
  }

HRESULT __stdcall WeakParticles::get_value(IParticles** pI)
  {
    _SYNCHRONIZE_;
    *pI = Refe((IParticles*)ph_);
    return S_OK;
  }

void ParticlesHelper::ReleasePsEngine_()
  {
    if ( ps_engine_ )
      {
        EMITTER_CONTEXT **em = &emcont_[0], **emE = em+emcont_.Count();
        for ( ; em != emE; ++em )
          ps_engine_->freeEmitter(ps_engine_,*em);
        ps_engine_->release(ps_engine_);
        ps_engine_ = 0;
      }
    emcont_.Clear();
  }

ParticlesHelper::~ParticlesHelper()
  {
    ReleasePsEngine_();
    if ( +weak_ )
      weak_->ForgetParticles();
  }

ParticlesHelper::ParticlesHelper()
  : noname_(false), autoupdate_(false), cycled_(true), ps_engine_(0)
  {
    lifetime_ = 0;
    ose_cookie_ = 0;
    startime_ = 0;
    partidx_ = 0;
    emcont_.Resize(1);
  }

PARTICLES_ENGINE *ParticlesHelper::GetPsEngine()
  {
    if ( !ps_engine_ ) ps_engine_ = MakeDefaultPsEngine();
    return ps_engine_;
  }

void ParticlesHelper::CheckDispose()
  {
    // если есть спрайт, он без имени и неконролируется другим спрайтом, то уничтожить спрайт
    if ( sprite_
      && noname_
      && !(sprite_->GetParent() && (sprite_->GetParentDeps() & SPRITE_DEPS_DISPOSE)) )
      sprite_->Dispose();
  }

void* ParticlesHelper::Adaptate_(const void* id)
  {
    if ( TypeID__ == id ) return this;
    if ( ComplexSprite::TypeID__ == id ) return (ComplexSprite*)this;
    return IRefObject::Adaptate_(id);
  }

bool ParticlesHelper::HasBlendState(bool blendstate)
  {
    return blendstate;
  }

bool ParticlesHelper::ComplexSpriteReset(bool blendstate)
  {
    if ( partices_.Count() )
      {
        complex_p_ = partices_.Begin();
        return true;
      }
    else
      return false;
  }

bool ParticlesHelper::ComplexSpriteNext(ComplexSprite::Info* info,bool blendstate)
  {
    while ( complex_p_ != partices_.End()
          && !( complex_p_->flags & ComplexSprite::Info::IS_VISIBLE_R ) )
          ++complex_p_;

    if ( complex_p_ != partices_.End() )
      {
        info->x = complex_p_->x[0];
        info->y = complex_p_->x[1];
        info->alphaCf = 1000-long(complex_p_->trans*10.f);
        info->xscale = info->yscale = complex_p_->scale;
        info->fr_no = complex_p_->frno;
        info->flags =
          ComplexSprite::Info::IS_CENTERED|
          ComplexSprite::Info::IS_ROTATED|
          ComplexSprite::Info::IS_BLENDED|
          ComplexSprite::Info::IS_SCALED|
          complex_p_->flags;
        info->cosv = complex_p_->cosin[0];
        info->sinv = complex_p_->cosin[1];
        ++complex_p_;
        return true;
      }
    return false;
  }

void ParticlesHelper::OnStopEmission(bool interrupted)
  {
    //rcc_ptr<IOnStopEmission> onStopEmission_;
    //if ( !onStopEmission_  && +subimpl_ )
    //  subimpl_->QueryInterface(TEGGO_GUIDOF(IOnStopEmission),(void**)&+onStopEmission_);
    if ( +onStopEmission_ )
      {
        rcc_ptr<IParticles> guard(Refe(this));
        HRESULT hr = onStopEmission_->OnStopEmission(this,ose_cookie_,interrupted);
        if ( FAILED(hr) )
          Lout << _S*"particles: IOnStopEmission_OnStopEmission('%?') failed, HRESUT = 0x%08x"
                  %sprite_->GetName() %hr;
        Unrefe(onStopEmission_);
      }
  }

void ParticlesHelper::SetSpritePtr(Sprite* p)
  {
    sprite_ = p;
    if ( !sprite_ )
      {
        if ( +subimpl_ )
          {
            rcc_ptr<IOnDispose> on_dispose;
            if ( SUCCEEDED(subimpl_->
                        QueryInterface(TEGGO_GUIDOF(IOnDispose),(void**)&+on_dispose)) )
              on_dispose->OnDispose((IParticles*)this);
          }
        Unrefe(subimpl_);
      }
  }

void ParticlesHelper::Reset_()
  {
    memset(&partices_[0],0,partices_.Count()*sizeof(PARTICLE));
  }

HRESULT ParticlesHelper::Activate(bool activate)
  {
    if ( autoupdate_ == activate )
      return S_OK;
    else
      {
        if ( activate )
          {
            if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
            if ( !weak_ ) weak_.reset_(new WeakParticles(this));
            Engine_ComAddOnUpdateListener(+weak_);
            sprite_->Show();
            Reset_();
            startime_ = GetMillis_OnFrame();
            EMITTER_CONTEXT **em = &emcont_[0], **emE = em+emcont_.Count();
            for ( ; em != emE; ++em )
              {
                EMITTER_CONTEXT *e = *em;
                e->lastemission = startime_;
                e->nextemission = startime_;
                e->timelimit = e->timelimit_cf[0];
                if ( e->timelimit_cf[1] > e->timelimit )
                  e->timelimit += randomizer_.Random()
                                   % ( e->timelimit_cf[1] - e->timelimit );
              }
          }
        else
          {
            OnStopEmission(true);
          }
        autoupdate_ = activate;
      }
    return S_OK;
  }

void ParticlesHelper::NotifyLifeTimeOut()
  {
    if ( !!onLifeTimeOut_ )
      {
        HRESULT hr = onLifeTimeOut_->OnLifetimeOut( this );
        if ( FAILED(hr) )
          Lout << _S*"particles: IOnLifetimeOut_OnLifetimeOut('%?') failed, HRESUT = 0x%08x"
                  %sprite_->GetName() %hr;
        Unrefe(onLifeTimeOut_);
      }
  }

bool ParticlesHelper::Update_(u32_t tickdelta)
  {
    if ( !sprite_ ) return false;
    if ( !autoupdate_ ) return false;
    u32_t frmillis = GetMillis_OnFrame();
    if ( lifetime_ > 0 && frmillis > lifetime_ + startime_ )
      {
        rcc_ptr<IUnknown> guard(Refe((IParticles*)this));
        NotifyLifeTimeOut();
        Activate(false);
        return false;
      }
    else
      {
        {for ( EMITTER_CONTEXT **i = emcont_.Begin(), **iE = emcont_.End(); i != iE; ++i )
          Emit_(**i,frmillis);}
        {for ( int n = 0, nE = partices_.Count(); n < nE; ++n )
          {
            PARTICLE *i = &partices_[n];
            if ( i->context_ )
              {
                i->context_->ops.opUpdate(i->context_,i,tickdelta,n);
              //Update_(*i,tickdelta);
              }
          }}
      }
    return true;
  }

/*void ParticlesHelper::Update_(PARTICLE& particle,u32_t tickdelta)
  {
    EMITTER_CONTEXT* emcont = particle.context_;
    if ( tickdelta ) {
      if ( emcont->program.Count() ) ExecuteProgram_(particle,tickdelta);
    }
  }*/

void ParticlesHelper::Emit_(EMITTER_CONTEXT& emcont,u32_t frmillis)
  {
    if ( partices_.Empty() )
      return;

    /*if ( emcont.program.Empty() && FAILED(SetDefaultProgram(&emcont-&emcont_[0])) )
      return; */

    if ( unsigned(emcont.nextemission) <= frmillis )
      {
        long count = emcont.quantity_cf[0];
        long period = emcont.period_cf[0];

        if ( emcont.quantity_cf[1] > count )
          count += (randomizer_.Random() % (emcont.quantity_cf[1] - count ));
        if ( count > partices_.Count() || count == 0 ) count = partices_.Count();
        emcont.lastemission = emcont.nextemission;
        if ( emcont.period_cf[1] > period )
          period += (randomizer_.Random() % (emcont.period_cf[1] - period ));
        emcont.nextemission += period;

        while ( count-- )
          {
            if ( partidx_ >= partices_.Count() )
              if ( cycled_ ) partidx_ = 0;
              else break;
            int idx = partidx_++;
            PARTICLE& par = partices_[idx];
            memset(&par,0,sizeof(PARTICLE));
            par.cosin[0] = 1;
            par.scale = 1;
            par.context_ = &emcont;
            par.flags = ComplexSprite::Info::IS_VISIBLE_R;
            if ( emcont.ops.opEmit ) emcont.ops.opEmit(&emcont,&par,idx);
          }
      }
  }

HRESULT __stdcall ParticlesHelper::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(IParticles),riid)?S_OK:S_FALSE;
  }

u32_t ParticlesHelper::Ex_GetParentDeps() const
  {
    if ( !sprite_ ) return 0xffffffff;
    else return sprite_->GetParentDeps();
  }

void ParticlesHelper::Ex_SetParentDeps(u32_t val)
  {
    if ( !sprite_ ) return;
    else sprite_->SetParentDeps(val);
  }

HRESULT __stdcall ParticlesHelper::get_count(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = partices_.Count();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_count(long newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    ResetErrorLog();
    PARTICLE p = {0};
    //memset(&p,0,sizeof(PARTICLE));
    partices_.Resize(newVal,p);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_x(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetX();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_x(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->MoveX(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_y(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetY();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_y(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->MoveY(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_xScale(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetScaleX();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_xScale(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->ScaleX(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_yScale(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetScaleY();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_yScale(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->ScaleY(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_xyScale(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->ScaleX(newVal);
    sprite_->ScaleY(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_angle(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetAngle();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_angle(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetAngle(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_luminance(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetLuminance();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_luminance(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetLuminance(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_transparency(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTransparency();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_transparency(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetTransparency(newVal);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_cfilt(IRGBFilter **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = new ColorObject(Refe((IColorHelper*)this));
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_parent(ISprite **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = 0;

    if ( Sprite* spr = sprite_->GetParent() )
      if ( ISpriteHelperEx* helper = Adaptate<SpriteHelper>(spr->GetSubImpl()) )
        return helper->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pVal);

    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_parent(ISprite *newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( newVal )
      if ( rcc_ptr<ISpriteHelperEx2> helper = teggo::ifs_cast<ISpriteHelperEx2>(newVal) )
        if ( Sprite* spr = helper->GetSprite_() )
          sprite_->SetParent(spr);
        else
          return GenSCWideError(NOT_EXISTS_INFO);
      else
        return GenSCWideError(L"invalid sprite",L":Set parent sprite:");
    else
      sprite_->SetParent(0);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_visible(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->IsVisible()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_visible(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( newVal!=VARIANT_FALSE )
      sprite_->Show();
    else
      sprite_->Hide();
    return S_OK;
  }

HRESULT __stdcall  ParticlesHelper::get_hierarchy(IHierarchy** pI)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pI = new Hierarchy(this);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::Dispose()
  {
    _SYNCHRONIZE_;
    if ( sprite_ )
      {
        if ( autoupdate_ )
          StopEmission(0);
        sprite_->Dispose();
      }
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::Reset()
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    Reset_();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_active(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = autoupdate_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_name(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = SysAllocString(sprite_->GetName());
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::GetNameAsPWIDE(wchar_t** pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = (wchar_t*)sprite_->GetName();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_klass(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = SysAllocString(sprite_->GetKlass());
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::GetKlassAsPWIDE(wchar_t** pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = (wchar_t*)sprite_->GetKlass();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_layer(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetLayer();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_layer(long val)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetLayer(val);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_sublayer(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetSubLayer();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_sublayer(long val)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetSubLayer(val);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_subimpl(IUnknown **pVal)
  {
    return get_dispatcher(pVal);
  }

HRESULT __stdcall ParticlesHelper::get_dispatcher(IUnknown **pVal)
  {
    _SYNCHRONIZE_;
    *pVal = Refe(subimpl_);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_subimpl(IUnknown *newVal)
  {
    return put_dispatcher(newVal);
  }

HRESULT __stdcall ParticlesHelper::put_dispatcher(IUnknown *newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    rcc_ptr<IUnknown> tmp_(Refe(newVal));
    subimpl_.Swap(tmp_);

    if ( autoupdate_ )
      {
        Unrefe(onEmission_);
        Unrefe(onLifeTimeOut_);
        Unrefe(onStopEmission_);
        if ( +subimpl_ )
          {
            subimpl_->QueryInterface(
              TEGGO_GUIDOF(IOnEmission),(void**)&+onEmission_);
            subimpl_->QueryInterface(
              TEGGO_GUIDOF(IOnLifetimeOut),(void**)&+onLifeTimeOut_);
            subimpl_->QueryInterface(
              TEGGO_GUIDOF(IOnStopEmission),(void**)&+onStopEmission_);
          }
      }

    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_alive(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = sprite_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_weakref(IWeakParticles **pVal)
  {
    _SYNCHRONIZE_;
    if ( !weak_ ) weak_.reset_(new WeakParticles(this));
    *pVal = Refe(weak_);
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::StartEmission(IUnknown *listener, long cookie)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    Unrefe(onStopEmission_);
    Unrefe(onEmission_);
    ose_cookie_ = cookie;
    if ( listener )
      {
        listener->QueryInterface(TEGGO_GUIDOF(IOnStopEmission),(void**)&+onStopEmission_);
        listener->QueryInterface(TEGGO_GUIDOF(IOnEmission),(void**)&+onEmission_);
        listener->QueryInterface(TEGGO_GUIDOF(IOnLifetimeOut),(void**)&+onLifeTimeOut_);
      }
    if ( !onEmission_ && +subimpl_ )
      subimpl_->QueryInterface(TEGGO_GUIDOF(IOnEmission),(void**)&+onEmission_);
    if ( !onLifeTimeOut_ && +subimpl_ )
      subimpl_->QueryInterface(TEGGO_GUIDOF(IOnLifetimeOut),(void**)&+onLifeTimeOut_);
    if ( !onStopEmission_ && +subimpl_ )
      subimpl_->QueryInterface(TEGGO_GUIDOF(IOnStopEmission),(void**)&+onStopEmission_);
    return Activate(true);
  }

HRESULT __stdcall ParticlesHelper::StopEmission(long timeout)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    return Activate(false);
  }

HRESULT __stdcall ParticlesHelper::SetEmitterCf(/*[in]*/ long cfno, /*[in]*/ float val0,/*[in]*/ float val1,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( cfno < 0 || cfno >= CF_COUNT )
      return GenSCWideError(_S*L"cfno %d out of range [0..%d]" %cfno %CF_COUNT);
    if ( emitter >= long(emcont_.Count()) )
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    if ( emitter < 0 )
      for ( EMITTER_CONTEXT **i = emcont_.Begin(), **iE = emcont_.End(); i != iE; ++i )
        (*i)->rg[cfno*2] = val0, (*i)->rg[cfno*2+1] = val1;
    else
      emcont_[emitter]->rg[cfno*2] = val0, emcont_[emitter]->rg[cfno*2+1] = val1;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetEmissionQuantity(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) )
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    if ( emitter < 0 )
      for ( EMITTER_CONTEXT **i = emcont_.Begin(), **iE = emcont_.End(); i != iE; ++i )
        (*i)->quantity_cf[0] = min, (*i)->quantity_cf[1] = max;
    else
      emcont_[emitter]->quantity_cf[0] = min, emcont_[emitter]->quantity_cf[1] = max;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetEmissionPeriod(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) )
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    if ( emitter < 0 )
      for ( EMITTER_CONTEXT **i = emcont_.Begin(), **iE = emcont_.End(); i != iE; ++i )
        (*i)->period_cf[0] = min, (*i)->period_cf[1] = max;
    else
      emcont_[emitter]->period_cf[0] = min, emcont_[emitter]->period_cf[1] = max;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetEmissionTime(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) )
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    if ( emitter < 0 )
      for ( EMITTER_CONTEXT **i = emcont_.Begin(), **iE = emcont_.End(); i != iE; ++i )
        (*i)->timelimit_cf[0] = min, (*i)->timelimit_cf[1] = max;
    else
      emcont_[emitter]->timelimit_cf[0] = min, emcont_[emitter]->timelimit_cf[1] = max;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetProgram(/*[in]*/ BSTR program,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    wchar_t errorS[128] = L"";
    bool r;

    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) )
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);

    if ( emitter >= 0 )
      {
        EMITTER_CONTEXT *e = emcont_[emitter];
        if ( e->ops.opProgram ) r = e->ops.opProgram(e,program,errorS);
      }
    else
      for ( EMITTER_CONTEXT **i = emcont_.Begin(), **iE = emcont_.End(); i != iE; ++i )
        {
          EMITTER_CONTEXT *e = *i;
          if ( e->ops.opProgram ) r = e->ops.opProgram(e,program,errorS);
          if ( !r ) break;
        }

    if ( !r )
      return GenSCWideError(errorS);

    return S_OK;
  }

//HRESULT __stdcall ParticlesHelper::SetDefaultProgram(/*[in]*/ long emitter)
//  {
//    return SetProgram((BSTR)widestring(particles_default_program).Str(),emitter);
//  }

HRESULT __stdcall ParticlesHelper::get_emCount(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = emcont_.Count();
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_emCount(long newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( newVal <= 0 ) newVal = 1;
    if ( newVal < emcont_.Count() )
      {
        for ( int i = newVal; i < emcont_.Count(); ++i )
          ps_engine_->freeEmitter(ps_engine_,emcont_[i]);
        emcont_.Resize(newVal);
      }
    else
      {
        int i = emcont_.Count();
        emcont_.Resize(newVal);
        for ( ;i < newVal; ++i )
          {
            emcont_[i] = ps_engine_->allocEmitter(ps_engine_);
            emcont_[i]->sprite = sprite_;
          }
      }
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::get_lifeTime(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = lifetime_;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_lifeTime(long newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( newVal < 0 ) newVal = 0;
    lifetime_ = newVal;
    return S_OK;
  }

/*void ParticlesHelper::DumpProgram_(long emitter)
  {
    //if ( !emcont_[emitter].program.Count() ) SetDefaultProgram(emitter);
    //widestring sbuf;
    //Decompile(emcont_[emitter].program,sbuf);
    //Lout << _S*L"particles: %s: emitter %d program microps\n%s" %sprite_->GetName() %emitter %sbuf;
  }*/

/*HRESULT __stdcall ParticlesHelper::DumpProgramMicrops(long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) )
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    if ( emitter < 0 )
      for ( int i = 0, iE = emcont_.Count(); i != iE; ++i )
        DumpProgram_(i);
    else
      DumpProgram_(emitter);
    return S_OK;
  }*/

HRESULT __stdcall ParticlesHelper::get_cycled(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = cycled_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_cycled(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    cycled_ = newVal != VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::DispatchSubImpl(IDispatch *subimpl)
  {
    _SYNCHRONIZE_;
    return put_subimpl(
      +rcc_ptr<IUnknown>(
        subimpl
        ? ((IOnUpdate*) new Engine_DispSubImpl(subimpl,Engine_DispSubImpl::DISPATCH))
          : 0
          ));
  }

HRESULT __stdcall ParticlesHelper::get_autodispose(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = noname_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::put_autodispose(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    noname_ = newVal!=VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ParticlesHelper::Suspend()
  {
    return 0;
  }

HRESULT __stdcall ParticlesHelper::Resume()
  {
    return 0;
  }

HRESULT __stdcall ParticlesHelper::Simulate(long millis,long fps)
  {
    return 0;
  }
