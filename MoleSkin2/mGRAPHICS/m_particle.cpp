
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

#include "precompile.h"
#include "m_sprites.h"
#include "m_sprites0.h"
#include "m_loadsurf.h"
#include "m_screen.h"
#include "../mUTIL/m_randomizer.h"
#include "../mUTIL/m_collection.h"
#include "../mUTIL/m_widestring.h"
#include <limits.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

#include "m_lwsa_basic.inl"

class Particles_Impl;

class ParticlesFactory_Impl : public ParticlesFactory {
  typedef CollectionT<Particles_Impl> PARTICLES_MAP;
  PARTICLES_MAP particles_;
public:
  SPRITES_Inst* eng_;    
  ParticlesFactory_Impl (SPRITES_Inst* eng) : eng_(eng) {}
  virtual ~ParticlesFactory_Impl () { Purge(); }
  virtual void Purge();
  virtual Particles* NewParticles(pwide_t name,pwide_t klass,int layer);
  virtual Particles* FindParticles(pwide_t name);
  virtual SCERROR DisposeParticles(pwide_t name);
  virtual SCERROR Enumerate(BufferT<Particles*>&);
};

ParticlesFactory* CreateParticlesFactory(SPRITES_Inst* eng) {
  return new ParticlesFactory_Impl(eng);
}

class Particles_Impl : public LwsArraySprite<Particles> {
  struct PHYSICUE {
    float speed_x; 
    float speed_y;
  };
  ParticlesFactory_Impl* factory_;
  BufferT<PHYSICUE> physicue_;
  Randomizer randomizer_;
  long  max_fr_no_;
  long  min_frno_;
  long  mod_frno_;
  float min_angle_;
  float mod_angle_;
  float min_speed_angle_;
  float mod_speed_angle_;
  float min_speed_value_;
  float mod_speed_value_;
  float min_trans_;
  float mod_trans_;
  float min_scale_;
  float mod_scale_;
  float angle_delta_;
  float scale_delta_;
  float trans_delta_;
  float gravity_;
  long  pulse_;
  float xsource_;
  float ysource_;
  long  active_no_;
  u32_t last_update_;
  u32_t start_update_;
  u32_t pulse_tick_;
  u32_t pulse_count_;
  long    lifetime_;
  bool    cycled_;
  auto_ref<ParticlesSubImpl> subimpl_;
public:
  virtual pwide_t GetName() const { return CollectionT<Particles_Impl>::KeyOf(this);}
  virtual void  SetSubImpl(ParticlesSubImpl* x) {
    if ( !!subimpl_ ) {
      subimpl_->SetParticlesPtr(0);
      Release(subimpl_);
    }
    subimpl_ = AutoRef(Grab(x)); 
    if ( !!subimpl_ ) subimpl_->SetParticlesPtr(this);
  }
  virtual ParticlesSubImpl*  GetSubImpl() const { return subimpl_; }
  Particles_Impl() {}
  virtual ~Particles_Impl();
  void Init(Sprite* spr,ParticlesFactory_Impl* fac);
  virtual SCERROR Resize(long count);
  virtual float GetAngleDelta() const { return angle_delta_; }
  virtual void  SetAngleDelta(float val) { angle_delta_ = val; }
  virtual float GetTransparencyDelta() const { return trans_delta_; }
  virtual void  SetTransparencyDelta(float val) { trans_delta_ = val; }
  virtual float GetScaleDelta() const { return scale_delta_*100; }
  virtual void  SetScaleDelta(float val) { scale_delta_ = val/100; }
  virtual float GetSourceY() const { return ysource_; }
  virtual void  SetSourceY(float val) { ysource_ = val; }
  virtual float GetSourceX() const { return xsource_; }
  virtual void  SetSourceX(float val) { xsource_ = val; }
  virtual void  SetStartAngle(float minval,float maxval) { 
    min_angle_ = minval-nv_pi/2; mod_angle_ = maxval-minval; 
  }
  virtual void  SetStartFrno(long minval,long maxval) { min_frno_ = minval; mod_frno_ = maxval-minval; }
  virtual void  SetStartScale(float minval,float maxval) { 
    min_scale_ = minval/100; 
    mod_scale_ = (maxval-minval)/100; 
  }
  virtual void  SetStartTransparency(float minval,float maxval) { 
    min_trans_ = minval; mod_trans_ = maxval-minval; 
  }
  virtual void  SetStartSpeed(float minval,float maxval) {
    min_speed_value_ = minval;
    mod_speed_value_ = maxval-minval;
  }
	virtual void  SetStartDirection(float minangle,float maxangle)
  {
    min_speed_angle_ = minangle-nv_pi/2;
    mod_speed_angle_ = maxangle-minangle;
  }
  virtual float GetGravity() const { return gravity_; }
  virtual void  SetGravity(float val) { gravity_ = val; }
  virtual long  GetPulse() const { return pulse_; }
  virtual void  SetPulse(long val) { pulse_ = val; }
  virtual long  GetLifeTime() const { return lifetime_; };
  virtual void  SetLifeTime(long ms) { lifetime_ = ms; };
  virtual long  GetPulseCount() const { return pulse_count_; }
  virtual void  SetPulseCount(long pcnt) { pulse_count_ = pcnt; }
  virtual bool  IsCycled() const { return cycled_; }
  virtual void  SetCycled(bool c) { cycled_ = c; }
	virtual void  Reset();
  virtual bool  Update(u32_t);
  virtual void    Dispose() { factory_->DisposeParticles(GetName()); }

  void UpdateParticle(LWSA_MICROSPRITE&,PHYSICUE&,float delta,float delta2,long difCf);
//  virtual bool HasBlendState(bool blendstate) { 
//    return blendstate; 
//  }
};

void Particles_Impl::Init(Sprite* spr,ParticlesFactory_Impl* fac)
{
  LwsArraySprite<Particles>::Init(spr);

  factory_  = fac;

  min_frno_ = 0;
  mod_frno_ = 0;
  min_speed_angle_ = 0;
  mod_speed_angle_ = 0;
  min_speed_value_ = 0;
  mod_speed_value_ = 0;
  min_trans_ = 0;
  mod_trans_ = 0;
  min_scale_ = 1;
  mod_scale_ = 0;
  angle_delta_ = 0;
  scale_delta_ = 0;
  trans_delta_ = 0;
  gravity_ = 0;
  pulse_   = 0;
  xsource_ = 0;
  ysource_ = 0;
  min_angle_ = 0;
  mod_angle_ = 0;
  active_no_ = 0;
  last_update_ = 0;
  pulse_tick_ = 0;
  lifetime_ = -1;
  start_update_ = 0;
  pulse_count_ = 1;
  cycled_ = true;
}

Particles_Impl::~Particles_Impl()
{
  if ( spr_ ) { spr_->Dispose(); spr_ = 0; }
}

SCERROR Particles_Impl::Resize(long count)
{
  LwsArraySprite<Particles>::Resize(count);
  PHYSICUE phempty = {0};
  physicue_.resize(count,phempty);
  return SCE_OK;
}

Particles* ParticlesFactory_Impl::NewParticles(pwide_t name,pwide_t klass,int layer)
{
  if ( !klass ) ret0_because(L"sprites: invalid klass");
  
  if ( name && particles_.Get(name) ) 
    ret0_because( WideFormat(L"sprites: particle system %s alredy exists",name) );
  
  Sprite* spr = eng_->NewSprite(0,klass,layer);
  if ( !spr ) return 0;

  widestring autoname;
  pwide_t name_Sk;
  static long unique_no = 0;
  
  name_Sk = ( name && wcslen(name) ) ? name : autoname.Expand(128).SetF(L"{%s@%08x}",klass,++unique_no);

  Particles_Impl* pp = &particles_.Rec(name_Sk);
  pp->Init(spr,this);
  
  return pp;
}

Particles* ParticlesFactory_Impl::FindParticles(pwide_t name)
{
  Particles* p = particles_.Get(name);
  if ( p ) return p;
  ret0_because( WideFormat(L"sprites: particle system %s is not exists",name) );
}

SCERROR ParticlesFactory_Impl::DisposeParticles(pwide_t name)
{
  particles_.Erase(name);
  return SCE_OK;
}

void ParticlesFactory_Impl::Purge()
{
  particles_.Clear();
}

SCERROR ParticlesFactory_Impl::Enumerate(BufferT<Particles*>& ve)
{
  BufferT<Particles*>().Swap(ve);
  ve.resize(particles_.Count());

  PARTICLES_MAP::iterator i = particles_.Iterate();
  Particles **ii = ve.Begin();

  while ( i.Next() ) *ii++ = &*i;

  return SCE_OK;
}

void Particles_Impl::Reset()
{
  active_no_ = 0;
  last_update_ = 0;
  pulse_tick_ = 0;
  start_update_ = 0;
  spr_->Show();
  LwsArraySprite<Particles>::Show(0,LwsArraySprite<Particles>::Count(),0);
  randomizer_.Reset(time(0)+GetMillis());
}

inline void Particles_Impl::UpdateParticle(
  LWSA_MICROSPRITE& p,PHYSICUE& ph,
  float delta,float delta2,long difCf) 
{
  const float _d0 = delta * ph.speed_x;
  const float _d1 = delta * ph.speed_y;
  p.alphaCf = teggo_max<long>(p.alphaCf - difCf,0);
  p.x += _d0;
  p.y += gravity_ * delta2 + _d1;
  ph.speed_y += gravity_ * delta;
  if ( p.scale > 0 && p.scale < 100 ) p.scale   = p.scale + delta*scale_delta_;
  p.angle   = p.angle + delta*angle_delta_;
  if ( !EPSILON_EQUAL0(p.angle) ) 
    p.flags |= (ComplexSprite::Info::IS_ROTATED|ComplexSprite::Info::IS_UPDATED_R);
  else p.flags &= ~ComplexSprite::Info::IS_ROTATED;
}


bool Particles_Impl::Update(u32_t tickdelta)
{
  unsigned range_size;
  LWSA_MICROSPRITE* range = LwsArraySprite<Particles>::GetRange(&range_size);
  if ( !range_size ) return false;
  u32_t tick = last_update_ + tickdelta;
  if ( start_update_ && lifetime_ > 0 && lifetime_ < signed(tick-start_update_) ) {
    spr_->Hide();
    return false;
  }
  if ( last_update_ ) {
    float delta = float(tickdelta)/1000;
    float delta2 = delta * delta / 2;
    long difCf = long(trans_delta_*10*delta);
    for ( unsigned i = 0, iE = range_size; i < iE; ++i) {
      LWSA_MICROSPRITE& p = range[i];
      if ( p.flags & ComplexSprite::Info::IS_VISIBLE_R ) 
        UpdateParticle(p,physicue_[i],delta,delta2,difCf);
    }
    SPRITES::SetNeedRefresh();
  }
  if ( !last_update_ || pulse_tick_+pulse_ < tick )
  {
    if ( !last_update_ ) start_update_ = tick;
    long count = -1;
    if ( cycled_ )
    {
      pulse_tick_ = tick;
      count = teggo_min<u32_t>(pulse_count_,range_size);
    }
    else
    {
      pulse_tick_ = tick;
      count = teggo_min<u32_t>(pulse_count_,range_size-active_no_);
    }
    if ( count > 0 ) {
      long layer = spr_->GetLayer();
      const float sprX = spr_->GetScrX();
      const float sprY = spr_->GetScrY();
      const float layerX =  SPRITES::GetLayerX(layer);
      const float layerY =  SPRITES::GetLayerY(layer);
      float x =  sprX - layerX;
      float y =  sprY - layerY;
      for ( int i = 0; i < count; ++i ) {
        LWSA_MICROSPRITE& p = range[active_no_];
        PHYSICUE& ph   = physicue_[active_no_];
        //REQUIRE ( &p >= &*particles_.begin() && &p < &*particles_.end() );
        float speed_value = randomizer_.RandomF()*mod_speed_value_+min_speed_value_;
        float speed_angle = randomizer_.RandomF()*mod_speed_angle_+min_speed_angle_;
        p.flags = 
          ComplexSprite::Info::IS_VISIBLE_R|
          ComplexSprite::Info::IS_SCALED|
          ComplexSprite::Info::IS_INDEPENDED|
          ComplexSprite::Info::IS_BLENDED;
        p.x = x + xsource_;
        p.y = y + ysource_;
        p.fr_no = long(randomizer_.RandomF()*mod_frno_+min_frno_);
        p.angle = randomizer_.RandomF()*mod_angle_+min_angle_;
        p.alphaCf = 1000-teggo_min<long>(long((randomizer_.RandomF()*mod_trans_+min_trans_)*10.0f),1000);
        p.scale = randomizer_.RandomF()*mod_scale_+min_scale_;
        ph.speed_x = speed_value * cosf(speed_angle);
        ph.speed_y = speed_value * sinf(speed_angle);
        if ( !EPSILON_EQUAL0(p.angle) ) p.flags |= (ComplexSprite::Info::IS_ROTATED|ComplexSprite::Info::IS_UPDATED_R);
        else p.flags &= ~ComplexSprite::Info::IS_ROTATED;
        ++active_no_;
        if ( cycled_ ) active_no_ %= range_size;
      }
      SPRITES::SetNeedRefresh();
    }
  }
  last_update_ = tick;
  NeedCheckBlendingState();
  return true;
}

NAMESPACE_MSKIN_END
