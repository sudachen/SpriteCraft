
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

ParticlesHelper::~ParticlesHelper() 
  { 
    if ( +weak_ ) 
      weak_->ForgetParticles(); 
  }

ParticlesHelper::ParticlesHelper() 
  : noname_(false), autoupdate_(false), cycled_(true) 
  {
    lifetime_ = 0;
    ose_cookie_ = 0;
    startime_ = 0;
    partidx_ = 0;
    emcont_.Resize(1);
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
            EMITTER_CONTEXT *em = &emcont_[0], *emE = em+emcont_.Count();
            for ( ; em != emE; ++em ) 
              {
                em->lastemission = startime_;
                em->nextemission = startime_;
                em->timelimit = em->timelimit_cf[0];
                if ( em->timelimit_cf[1] > em->timelimit ) 
                  em->timelimit += randomizer_.Random() 
                                   % ( em->timelimit_cf[1] - em->timelimit );
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
        {for ( EMITTER_CONTEXT *i = emcont_.Begin(), *iE = emcont_.End(); i != iE; ++i ) 
          Emit_(*i,frmillis);}
        {for ( PARTICLE *i = partices_.Begin(), *iE = partices_.End(); i != iE; ++i ) 
          if ( i->context_ ) Update_(*i,tickdelta);}
      }
    return true;
  }

enum 
  {
    CMD_LDfl0 = 0x80000000,
    CMD_LDfl1 = 0x80000001,
    CMD_LDpx  = 0x80000002,
    CMD_STpx  = 0x80000003,
    CMD_STpx0 = 0x81000003,
    CMD_STpx1 = 0x82000003,
    CMD_LDpv  = 0x80000004,
    CMD_STpv  = 0x80000005,
    CMD_STpv0 = 0x81000005,
    CMD_STpv1 = 0x82000005,
    CMD_LDpa  = 0x80000006,
    CMD_STpa  = 0x80000007,
    CMD_LDpf  = 0x80000008,
    CMD_STpf  = 0x80000009,
    CMD_LDpfr = 0x8000000a,
    CMD_STpfr = 0x8000000b,
    CMD_LDps  = 0x8000000c,
    CMD_STps  = 0x8000000d,
    CMD_LDpt  = 0x8000000e,
    CMD_STpt  = 0x8000000f,
    CMD_LDpr  = 0x80000010,
    CMD_STpr  = 0x80000011,
    CMD_STpr0 = 0x81000011,
    CMD_STpr1 = 0x82000011,
    CMD_UPDAT = 0x80000012,
    CMD_UPDATONLY = 0x80000013,
    CMD_INDEP = 0x80000014,
    CMD_LDfv  = 0x80000015,
    CMD_NEGs  = 1,
    CMD_NEGv  , 
    CMD_ADDss , 
    CMD_ADDvv ,
    CMD_ADDvs ,
    CMD_MULss ,
    CMD_MULvs ,
    CMD_MODss ,
    CMD_MODvs ,
    CMD_DIVss ,
    CMD_DIVvs ,
    CMD_ROTvs ,
    CMD_MOVs  ,
    CMD_MOVv  ,
    CMD_RND   
  };

struct REGISTER
  {
    enum 
      {
        IS_VALUE    = (1UL << 8),
        IS_HIPART   = (1UL << 9),
        IS_VECTOR   = (1UL << 10),
        IS_PARTICLE = (1UL << 11),
        IS_VALUE1   = (1UL << 12),
        PAR_REG_X   = (1UL << 0),
        PAR_REG_V   = (1UL << 1),
        PAR_REG_R   = (1UL << 2),
        PAR_REG_MASK = PAR_REG_X|PAR_REG_V|PAR_REG_R
      };
    u32_t flags;
    u32_t value;
    u32_t value1;
    long    regno;
    u32_t ldcmd;
    u32_t stcmd;
    u32_t operator *() 
      { 
        return regno; 
      }
  };

extern "C" const char particles_default_program[];

// val0 - reg, const, part :  const -> shadow00
// val1 - reg, const       :  const -> shadow01
// res  - reg, part

static bool RegResolve(pwide_t name,REGISTER* reg)
  {
    typedef ParticlesHelper::EMITTER_CONTEXT EMITTER_CONTEXT;
    long r0,r1;
    wchar_t c;
    float fval;
    float fval1;
    memset( reg, 0, sizeof(REGISTER) );  
    /**/ if ( !wcscmp(name,L"a") ) 
      { 
        reg->regno = EMITTER_CONTEXT::REG_A;
      }
    else if ( !wcscmp(name,L"v") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE|REGISTER::PAR_REG_V;
        reg->ldcmd = CMD_LDpv;
        reg->stcmd = CMD_STpv;
      }
    else if ( !wcscmp(name,L"x") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE|REGISTER::PAR_REG_X;
        reg->ldcmd = CMD_LDpx;
        reg->stcmd = CMD_STpx;
      }
    else if ( !wcscmp(name,L"r") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE|REGISTER::PAR_REG_R;
        reg->ldcmd = CMD_LDpr;
        reg->stcmd = CMD_STpr;
      }
    else if ( !wcscmp(name,L"t") )  { reg->regno = EMITTER_CONTEXT::REG_T;}
    else if ( !wcscmp(name,L"dt") ) { reg->regno = EMITTER_CONTEXT::REG_DT; } 
    else if ( !wcscmp(name,L"idx") ) { reg->regno = EMITTER_CONTEXT::REG_IDX; } 
    else if ( !wcscmp(name,L"trans") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_PARTICLE;
        reg->ldcmd = CMD_LDpt;
        reg->stcmd = CMD_STpt;
      }
    else if ( !wcscmp(name,L"scale") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_PARTICLE;
        reg->ldcmd = CMD_LDps;
        reg->stcmd = CMD_STps;
      }
    else if ( !wcscmp(name,L"angle") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_PARTICLE;
        reg->ldcmd = CMD_LDpa;
        reg->stcmd = CMD_STpa;
      }
    else if ( !wcscmp(name,L"frno") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_PARTICLE;
        reg->ldcmd = CMD_LDpfr;
        reg->stcmd = CMD_STpfr;
      }
    else if ( !wcscmp(name,L"f") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_PARTICLE;
        reg->ldcmd = CMD_LDpf;
        reg->stcmd = CMD_STpf;
      }
    else if ( !wcscmp(name,L"r") ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1; 
        reg->flags = REGISTER::IS_VECTOR|REGISTER::IS_PARTICLE;
        reg->ldcmd = CMD_LDpr;
        reg->stcmd = CMD_STpr;
      }
    else if ( 2 == swscanf(name,L"a%d.%d%c",&r0,&r1,&c) ) 
      { 
        if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_Ai_COUNT ) return false;
        if ( r1 != 0 && r1 != 1 ) return false;
        reg->regno = EMITTER_CONTEXT::REG_A0+r0*2+r1; 
      }
    else if ( 1 == swscanf(name,L"a%d%c",&r0,&c) ) 
      {
        if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_Ai_COUNT ) return false;
        reg->regno = EMITTER_CONTEXT::REG_A0+r0*2; 
        reg->flags = REGISTER::IS_VECTOR;
      }
    else if ( 2 == swscanf(name,L"em%d.%d%c",&r0,&r1,&c) ) 
      { 
        if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_CF_COUNT ) return false;
        if ( r1 != 0 && r1 != 1 ) return false;
        reg->regno = EMITTER_CONTEXT::REG_CF+r0*2+r1; 
      }
    else if ( 1 == swscanf(name,L"em%d%c",&r0,&c) ) 
      {
        if ( r0  < 0 || r0  > EMITTER_CONTEXT::REG_CF_COUNT ) return false;
        reg->regno = EMITTER_CONTEXT::REG_CF+r0*2; 
        reg->flags = REGISTER::IS_VECTOR;
      }
    else if ( 1 == swscanf(name,L"v.%d%c",&r1,&c) ) 
      { 
        if ( r1 != 0 && r1 != 1 ) return false;
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1+r1; 
        reg->flags = REGISTER::IS_PARTICLE|REGISTER::PAR_REG_V;
        reg->ldcmd = CMD_LDpv;
        reg->stcmd = r1?CMD_STpv1:CMD_STpv0;
      }
    else if ( 1 == swscanf(name,L"x.%d%c",&r1,&c) ) 
      { 
        if ( r1 != 0 && r1 != 1 ) return false;
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1+r1; 
        reg->flags = REGISTER::IS_PARTICLE|REGISTER::PAR_REG_X;
        reg->ldcmd = CMD_LDpx;
        reg->stcmd = r1?CMD_STpx1:CMD_STpx0;
      }
    else if ( 1 == swscanf(name,L"r.%d%c",&r1,&c) ) 
      { 
        if ( r1 != 0 && r1 != 1 ) return false;
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1+r1; 
        reg->flags = REGISTER::IS_PARTICLE|REGISTER::PAR_REG_X;
        reg->ldcmd = CMD_LDpr;
        reg->stcmd = r1?CMD_STpr1:CMD_STpr0;
      }
    else if ( 1 == swscanf(name,L"%f%c",&fval,&c) ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW00;
        reg->flags = REGISTER::IS_VALUE;
        reg->ldcmd = CMD_LDfl0;
        reg->value = *(u32_t*)&fval;
      }
    else if ( 2 == swscanf(name,L"(%f;%f)%c",&fval,&fval1,&c) ) 
      {
        reg->regno = EMITTER_CONTEXT::REG_SHADOW1;
        reg->flags = REGISTER::IS_VALUE|REGISTER::IS_VALUE1|REGISTER::IS_VECTOR;
        reg->ldcmd = CMD_LDfv;
        reg->value = *(u32_t*)&fval;
        reg->value1 = *(u32_t*)&fval1;
      }
    else 
      return false;

    return true;
  }

u32_t inline MK_PSPCMD(u32_t cmd,u32_t r0,u32_t r1,u32_t r2)
  {
    return r0 | ( r1 << 8 ) | ( r2 << 16 ) | ( cmd << 24 );
  }

#define SCANF_REGFMT L"%31[-a-z0-9.();]"

bool ParticlesHelper::PspCompile(
  pwide_t source, BufferT<u32_t> &program, BaseStringT<wchar_t> &errorS)
  {
    enum 
      { 
        XUPDAT,
        XUPDATO,
        XINDEP,
        ARGS2_CMD,
        XMOV,
        XNEG,
        ARGS3_CMD,
        XADD,
        XSUB,
        XDIV,
        XMUL,
        XMOD,
        XROT,
        XRND 
      };

    widestring sourceS(source);
    source = sourceS.ToLower().Trim().Str();
    program.Resize(0);
    u32_t cmd;
    wchar_t r0[32] = {0};
    wchar_t r1[32] = {0};
    wchar_t r2[32] = {0};
    REGISTER reg0,reg1,reg2;
    wchar_t c;
    pwide_t s  = source;
    long lineno = 1;

    while ( *s ) 
      {
        while ( *s && (*s == L'\t' || *s == L' ' || *s == L'\r') ) ++s;
        
        if ( *s == '\n' ) 
          { 
            ++s; 
            ++lineno; 
            continue; 
          }

        if ( *s ) 
          {
            if ( *s == L';' ) { while ( *s && *s++ != L'\n' ) {} ++lineno; continue; }
            else if ( swscanf(s,L"updateonl%c",&c) == 1 && c == L'y' ) cmd = XUPDATO;
            else if ( swscanf(s,L"updat%c",&c) == 1 && c == L'e' ) cmd = XUPDAT;
            else if ( swscanf(s,L"inde%c",&c) == 1 && c == L'p' ) cmd = XINDEP;
            else if ( swscanf(s,L"add " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XADD;
            else if ( swscanf(s,L"rnd " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XRND;
            else if ( swscanf(s,L"sub " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XSUB;
            else if ( swscanf(s,L"mul " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XMUL;
            else if ( swscanf(s,L"mod " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XMOD;
            else if ( swscanf(s,L"div " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XDIV;
            else if ( swscanf(s,L"rot " SCANF_REGFMT L" , " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r1,r2) == 3 ) cmd = XROT;
            else if ( swscanf(s,L"mov " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r2) == 2 ) cmd = XMOV;
            else if ( swscanf(s,L"neg " SCANF_REGFMT L" , " SCANF_REGFMT,r0,r2) == 2 ) cmd = XNEG;
            else 
              return errorS.Set(_S*L"synatx error, line %d" %lineno),false;
          }
        
        if ( cmd > ARGS2_CMD ) 
          {
            if ( !RegResolve(r0,&reg0) ) 
              return errorS.Set(_S*L"invalid first source register/value, line %d" %lineno),false;
            if ( (cmd > ARGS3_CMD)  && !RegResolve(r1,&reg1) ) 
              return errorS.Set(_S*L"invalid target register, line %d" %lineno),false;
            if ( !RegResolve(r2,&reg2) ) 
              return errorS.Set(_S*L"invalid target register, line %d" %lineno),false;

            if ( reg2.flags&REGISTER::IS_VALUE ) 
              return errorS.Set(_S*L"invalid target register, line %d" %lineno),false;
            if ( reg0.flags&(REGISTER::IS_PARTICLE|REGISTER::IS_VALUE) ) 
              {
                program.Push(reg0.ldcmd);
                if ( reg0.flags&REGISTER::IS_VALUE)  program.Push(reg0.value);
                if ( reg0.flags&REGISTER::IS_VALUE1) program.Push(reg0.value1);
              }
            if ( cmd > ARGS3_CMD ) 
              {
                if ( reg1.flags&(REGISTER::IS_PARTICLE|REGISTER::IS_VALUE) ) 
                  {
                    if ( (reg0.flags&REGISTER::IS_VALUE) && reg0.ldcmd == CMD_LDfl0 && reg1.ldcmd == CMD_LDfl0 ) 
                      reg1.ldcmd = CMD_LDfl1, ++reg1.regno;
                    program.Push(reg1.ldcmd);
                    if ( reg1.flags&REGISTER::IS_VALUE) program.Push(reg1.value);
                  }
              }
            if ( (reg0.flags & REGISTER::IS_VECTOR) != (reg2.flags & REGISTER::IS_VECTOR) ) 
              return errorS.Set(_S*L"invalid target register type, line %d" %lineno),false;
          }
    
        bool is_val0v = (reg0.flags & REGISTER::IS_VECTOR) != 0;
        bool is_val1v = (reg1.flags & REGISTER::IS_VECTOR) != 0;

        /**/ if ( cmd == XNEG && is_val0v )   
          program.Push( MK_PSPCMD( CMD_NEGv, *reg0, 0, *reg2 ) );
        else if ( cmd == XNEG && !is_val0v )  
          program.Push( MK_PSPCMD( CMD_NEGs, *reg0, 0, *reg2 ) );
        else if ( cmd == XMOV && is_val0v )   program.Push( MK_PSPCMD( CMD_MOVv, *reg0, 0, *reg2 ) );
        else if ( cmd == XMOV && !is_val0v )  program.Push( MK_PSPCMD( CMD_MOVs, *reg0, 0, *reg2 ) );
        else if ( cmd == XSUB || cmd == XADD ) 
          {
            if ( !is_val0v && is_val1v ) return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
            if ( cmd == XSUB ) program.Push( MK_PSPCMD( (is_val1v?CMD_NEGv:CMD_NEGs), *reg0,0,*reg0 ) );
            if ( is_val0v ) program.Push( MK_PSPCMD( (is_val1v?CMD_ADDvv:CMD_ADDvs), *reg0,*reg1,*reg2 ) );
            else            program.Push( MK_PSPCMD( CMD_ADDss, *reg0,*reg1,*reg2 ) );
          }
        else if ( cmd == XMUL ) 
          {
            if ( is_val1v ) 
              return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
            program.Push( MK_PSPCMD( (is_val0v?CMD_MULvs:CMD_MULss), *reg0,*reg1,*reg2 ) );
          }
        else if ( cmd == XMOD ) 
          {
            if ( is_val1v ) 
              return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
            program.Push( MK_PSPCMD( (is_val0v?CMD_MODvs:CMD_MODss), *reg0,*reg1,*reg2 ) );
          }
        else if ( cmd == XDIV ) 
          {
            if ( is_val1v ) 
              return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
            program.Push( MK_PSPCMD( (is_val0v?CMD_DIVvs:CMD_DIVss), *reg0,*reg1,*reg2 ) );
          }
        else if ( cmd == XRND ) 
          {
            if ( is_val0v ) return errorS.Set(_S*L"invalid first register type, line %d" %lineno),false;
            if ( is_val1v ) return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
            program.Push( MK_PSPCMD( CMD_RND, *reg0,*reg1,*reg2 ) );
          }
        else if ( cmd == XROT) 
          {
            if ( !is_val0v ) return errorS.Set(_S*L"invalid first register type, line %d" %lineno),false;
            if ( is_val1v )  return errorS.Set(_S*L"invalid second register type, line %d" %lineno),false;
            program.Push( MK_PSPCMD( CMD_ROTvs, *reg0,*reg1,*reg2 ) );
          }
        else if ( cmd == XUPDAT ) program.Push(CMD_UPDAT);
        else if ( cmd == XUPDATO ) program.Push(CMD_UPDATONLY);
        else if ( cmd == XINDEP ) program.Push(CMD_INDEP);
        else
          return errorS.Set(_S*L"invalid command, line %d" %lineno),false;

        if ( cmd > ARGS2_CMD && (reg2.flags&REGISTER::IS_PARTICLE) ) 
          program.Push(reg2.stcmd);
        while ( *s && *s++ != L'\n' ) {}
        ++lineno;
      } // end while ( *s )

    //widestring disasm;
    //XDbg(Decompile(program,disasm));
    //XDbg(WideLogF(L"Particles Program Disassembler\n%s",disasm.Str()));
    return true;
  }

pwide_t ParticlesHelper::RegToSS(u32_t reg, BaseStringT<wchar_t>& ws)
  {
    if ( reg >= EMITTER_CONTEXT::REG_CF &&  reg < EMITTER_CONTEXT::REG_CF_END ) 
      return ws.Set(_S*L"em%d.%d" %(reg/2) %(reg%2));
    if ( reg >= EMITTER_CONTEXT::REG_SHADOW00 && reg < EMITTER_CONTEXT::REG_A ) 
      return ws.Set(_S*L"s%d.%d" %((reg-EMITTER_CONTEXT::REG_SHADOW00)/2) %((reg-EMITTER_CONTEXT::REG_SHADOW00)%2));
    if ( reg == EMITTER_CONTEXT::REG_A ) 
      return ws.Set(L"a");
    if ( reg >= EMITTER_CONTEXT::REG_A0 && reg < EMITTER_CONTEXT::REG_A_END ) 
      return ws.Set(_S*L"a%d.%d" %((reg-EMITTER_CONTEXT::REG_A0)/2) %((reg-EMITTER_CONTEXT::REG_A0)%2));
    if ( reg == EMITTER_CONTEXT::REG_DT ) 
      return ws.Set(L"dt");
    if ( reg == EMITTER_CONTEXT::REG_IDX ) 
      return ws.Set(L"idx");
    if ( reg == EMITTER_CONTEXT::REG_T ) 
      return ws.Set(L"t");
    return ws.Set(L"###");
  }

pwide_t ParticlesHelper::RegToVS(u32_t reg, BaseStringT<wchar_t>& ws)
  {
    if ( reg >= EMITTER_CONTEXT::REG_CF &&  reg < EMITTER_CONTEXT::REG_CF_END ) 
      return ws.Set(_S*L"em%d" %(reg/2));
    if ( reg >= EMITTER_CONTEXT::REG_SHADOW00 && reg < EMITTER_CONTEXT::REG_A ) 
      return ws.Set(_S*L"s%d" %((reg-EMITTER_CONTEXT::REG_SHADOW00)/2));
    if ( reg >= EMITTER_CONTEXT::REG_A0 && reg < EMITTER_CONTEXT::REG_A_END ) 
      return ws.Set(_S*L"a%d" %((reg-EMITTER_CONTEXT::REG_A0)/2));
    if ( reg == EMITTER_CONTEXT::REG_DT)  return ws.Set(L"dt");
    if ( reg == EMITTER_CONTEXT::REG_T)   return ws.Set(L"t");
    if ( reg == EMITTER_CONTEXT::REG_IDX) return ws.Set(L"idx");
    return ws.Set(L"###");
  }

void ParticlesHelper::Decompile(
  BufferT<u32_t> const &program, BaseStringT<wchar_t> &sbuf)
  {
    const u32_t* p = &program[0];
    while ( p != &program[0] + program.Count() ) 
      {
        sbuf << _S*L"%04d:" %(p-&program[0]);
        u32_t c = *p++;
        if ( c & 0x80000000UL ) 
          {
            switch ( c ) 
              {
              case CMD_LDfl0: sbuf<< _S*L"ldfl0\t%f\n" %*(float*)p++; break;
              case CMD_LDfl1: sbuf<< _S*L"ldfl1\t%f\n" %*(float*)p++; break;
              case CMD_LDfv:  
                {
                  float val0 = *(float*)p++;
                  float val1 = *(float*)p++;
                  sbuf<< _S*L"ldfv\t(%f;%f)\n" %val0 %val1; break;
                }
              case CMD_LDpx:  sbuf.Append(L"ldpx\n"); break;
              case CMD_STpx:  sbuf.Append(L"stpx\n"); break;
              case CMD_STpx0: sbuf.Append(L"stpx0\n"); break;
              case CMD_STpx1: sbuf.Append(L"stpx1\n"); break;
              case CMD_LDpv:  sbuf.Append(L"ldpv\n"); break;
              case CMD_STpv:  sbuf.Append(L"stpv\n"); break;
              case CMD_STpv0: sbuf.Append(L"stpv0\n"); break;
              case CMD_STpv1: sbuf.Append(L"stpv1\n"); break;
              case CMD_LDpr:  sbuf.Append(L"ldpr\n"); break;
              case CMD_STpr:  sbuf.Append(L"stpr\n"); break;
              case CMD_STpr0: sbuf.Append(L"stpr0\n"); break;
              case CMD_STpr1: sbuf.Append(L"stpr1\n"); break;
              case CMD_LDpt:  sbuf.Append(L"ldpt\n"); break;
              case CMD_STpt:  sbuf.Append(L"stpt\n"); break;
              case CMD_LDpa:  sbuf.Append(L"ldpa\n"); break;
              case CMD_STpa:  sbuf.Append(L"stpa\n"); break;
              case CMD_LDpf:  sbuf.Append(L"ldpf\n"); break;
              case CMD_STpf:  sbuf.Append(L"stpf\n"); break;
              case CMD_LDpfr: sbuf.Append(L"ldpfr\n"); break;
              case CMD_STpfr: sbuf.Append(L"stpfr\n"); break;
              case CMD_LDps:  sbuf.Append(L"ldps\n"); break;
              case CMD_STps:  sbuf.Append(L"stps\n"); break;
              case CMD_INDEP: sbuf.Append(L"indep\n"); break;
              case CMD_UPDAT: sbuf.Append(L"update\n"); break;
              case CMD_UPDATONLY: sbuf.Append(L"updateonly\n"); break;
              default: sbuf<< _S*L"%08x\n" %c;
              }
          }
        else
          {
            u32_t vl0 = (c & 0x0ff); c >>= 8;
            u32_t vl1 = (c & 0x0ff); c >>= 8;
            u32_t res = (c & 0x0ff); c >>= 8;
            widestring w0,w1,w2;
            switch (c ) 
              {
              case CMD_NEGs:  
                sbuf<< _S*L"negs\t%s,%s\n" %RegToSS(vl0,w0) %RegToSS(res,w2); break;
              case CMD_NEGv:  
                sbuf<< _S*L"negv\t%s,%s\n" %RegToVS(vl0,w0) %RegToVS(res,w2); break;
              case CMD_RND:   sbuf<< _S*L"rnd\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
              case CMD_ADDvv: sbuf<< _S*L"addvv\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToVS(vl1,w1) %RegToVS(res,w2);  break;
              case CMD_ADDvs: sbuf<< _S*L"addvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
              case CMD_ADDss: sbuf<< _S*L"addss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
              case CMD_MULvs: sbuf<< _S*L"mulvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
              case CMD_MULss: sbuf<< _S*L"mulss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
              case CMD_MODvs: sbuf<< _S*L"modvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
              case CMD_MODss: sbuf<< _S*L"modss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
              case CMD_DIVvs: sbuf<< _S*L"divvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
              case CMD_DIVss: sbuf<< _S*L"divss\t%s,%s,%s\n" %RegToSS(vl0,w0) %RegToSS(vl1,w1) %RegToSS(res,w2);  break;
              case CMD_ROTvs: sbuf<< _S*L"rotvs\t%s,%s,%s\n" %RegToVS(vl0,w0) %RegToSS(vl1,w1) %RegToVS(res,w2);  break;
              case CMD_MOVs:  sbuf<< _S*L"movs\t%s,%s\n" %RegToSS(vl0,w0) %RegToSS(res,w2);  break;
              case CMD_MOVv:  sbuf<< _S*L"movv\t%s,%s\n" %RegToVS(vl0,w0) %RegToVS(res,w2);  break;
              default: sbuf<< _S*L"%08x\n" %c;
              }
          }
      }
  }

void ParticlesHelper::Update_(PARTICLE& particle,u32_t tickdelta)
  {
    EMITTER_CONTEXT* emcont = particle.context_;
    if ( tickdelta ) {
      if ( emcont->program.Count() ) ExecuteProgram_(particle,tickdelta);
    }
  }

void ParticlesHelper::ExecuteProgram_(PARTICLE& particle,u32_t tickdelta)
  {
    EMITTER_CONTEXT* emcont = particle.context_;
    u32_t* p = &emcont->program[0]+particle.epi;
    u32_t* pE = &emcont->program[0]+emcont->program.Count();
    float* rg = emcont->rg;
    rg[EMITTER_CONTEXT::REG_DT]  = float(tickdelta)/1000.f;
    rg[EMITTER_CONTEXT::REG_IDX] = float(&particle - &partices_[0]);
    while ( p < pE ) 
      {
        u32_t c = *p++;
        if ( c & 0x80000000UL ) 
          {
            switch ( c ) 
              {
              case CMD_LDfl0: rg[EMITTER_CONTEXT::REG_SHADOW00] = *(float*)p++; break;
              case CMD_LDfl1: rg[EMITTER_CONTEXT::REG_SHADOW01] = *(float*)p++; break;
              case CMD_LDfv:  
                rg[EMITTER_CONTEXT::REG_SHADOW1+0] = *(float*)p++; 
                rg[EMITTER_CONTEXT::REG_SHADOW1+1] = *(float*)p++; 
                break;
              case CMD_LDpx: memcpy(&rg[EMITTER_CONTEXT::REG_SHADOW1],&particle.x,sizeof(float)*2); break;
              case CMD_STpx: memcpy(&particle.x,&rg[EMITTER_CONTEXT::REG_SHADOW1],sizeof(float)*2); break;
              case CMD_STpx0: particle.x[0] = rg[EMITTER_CONTEXT::REG_SHADOW1+0]; break;
              case CMD_STpx1: particle.x[1] = rg[EMITTER_CONTEXT::REG_SHADOW1+1]; break;
              case CMD_LDpv: memcpy(&rg[EMITTER_CONTEXT::REG_SHADOW1],&particle.v,sizeof(float)*2); break;
              case CMD_STpv: memcpy(&particle.v,&rg[EMITTER_CONTEXT::REG_SHADOW1],sizeof(float)*2); break;
              case CMD_STpv0: particle.v[0] = rg[EMITTER_CONTEXT::REG_SHADOW1+0]; break;
              case CMD_STpv1: particle.v[1] = rg[EMITTER_CONTEXT::REG_SHADOW1+1]; break;
              case CMD_LDpr: memcpy(&rg[EMITTER_CONTEXT::REG_SHADOW1],&particle.r,sizeof(float)*2); break;
              case CMD_STpr: memcpy(&particle.r,&rg[EMITTER_CONTEXT::REG_SHADOW1],sizeof(float)*2); break;
              case CMD_STpr0: particle.r[0] = rg[EMITTER_CONTEXT::REG_SHADOW1+0]; break;
              case CMD_STpr1: particle.r[1] = rg[EMITTER_CONTEXT::REG_SHADOW1+1]; break;
              case CMD_LDpa: rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.angle; break;
              case CMD_STpa: 
                particle.angle = rg[EMITTER_CONTEXT::REG_SHADOW1]; 
                particle.cosin[0] = cosf(particle.angle);
                particle.cosin[1] = sinf(particle.angle);
                break;
              case CMD_LDpfr: rg[EMITTER_CONTEXT::REG_SHADOW1] = float(particle.frno); break;
              case CMD_STpfr: particle.frno = long(rg[EMITTER_CONTEXT::REG_SHADOW1]); break;
              case CMD_LDpf:  rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.f; break;
              case CMD_STpf:  particle.f = rg[EMITTER_CONTEXT::REG_SHADOW1]; break;
              case CMD_LDps:  rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.scale*100.f; break;
              case CMD_STps:  particle.scale = rg[EMITTER_CONTEXT::REG_SHADOW1]*0.01f; break;
              case CMD_LDpt:  rg[EMITTER_CONTEXT::REG_SHADOW1] = particle.trans; break;
              case CMD_STpt: 
                particle.trans = rg[EMITTER_CONTEXT::REG_SHADOW1];
                if ( particle.trans < 0 ) particle.trans = 0;
                else if ( particle.trans > 100 ) particle.trans = 100;
                break;
              case CMD_INDEP:
                {
                  long layer = sprite_->GetLayer();
                  particle.flags |= ComplexSprite::Info::IS_INDEPENDED;
                  particle.x[0] += sprite_->GetScrX() - SPRITES::GetLayerX(layer);
                  particle.x[1] += sprite_->GetScrY() - SPRITES::GetLayerY(layer);
                }
                break;
              case CMD_UPDAT: particle.epi = p-&emcont->program[0]; break;
              case CMD_UPDATONLY: particle.epi = p-&emcont->program[0]; goto lb_ret;
              }
          }
        else
          {
            float* vl0 = rg + (c & 0x0ff); c >>= 8;
            float* vl1 = rg + (c & 0x0ff); c >>= 8;
            float* res = rg + (c & 0x0ff); c >>= 8;
            switch ( c ) 
              {
              case CMD_NEGs:  
                *res = -*vl0; break;
              case CMD_NEGv:  
                *res = -*vl0; *(res+1) = -*(vl0+1);  break;
              case CMD_ADDvv: 
                *res = *vl0 + *vl1; *(res+1) = *(vl0+1) + *(vl1+1);  break;
              case CMD_ADDvs: 
                *res = *vl0 + *vl1; *(res+1) = *(vl0+1) + *vl1;  break;
              case CMD_ADDss: 
                *res = *vl0 + *vl1;  break;
              case CMD_MULvs: 
                *res = *vl0 * *vl1; *(res+1) = *(vl0+1) * *vl1;  break;
              case CMD_MULss: 
                *res = *vl0 * *vl1;  break;
              case CMD_MODvs: 
                *res = float(long(*vl0) % long(*vl1)); *(res+1) = float(long(*(vl0+1)) % long(*vl1));  break;
              case CMD_MODss: 
                *res = float(long(*vl0) % long(*vl1));  break;
              case CMD_DIVvs: 
                *res = *vl0 / *vl1; *(res+1) = *(vl0+1) / *vl1;  break;
              case CMD_DIVss: 
                *res = *vl0 / *vl1;  break;
              case CMD_ROTvs: 
                {
                  const float cosval = cosf(*vl1);
                  const float sinval = sinf(*vl1);
                  const float x = vl0[0];
                  const float y = vl0[1];
                  res[0] = (x*cosval  + y*sinval);
                  res[1] = (-x*sinval + y*cosval);
                  break;
                }
              case CMD_RND:   
                {
                  float d = *vl1-*vl0;
                  *res = fabs(d) > 0.0001f ? randomizer_.RandomF()*d + *vl0 : *vl0;  
                  break;
                }
              case CMD_MOVs:  
                *res = *vl0; break;
              case CMD_MOVv:  
                *res = *vl0; *(res+1) = *(vl0+1); break;
              }
          }
      }
  lb_ret:;
  }

void ParticlesHelper::Emit_(EMITTER_CONTEXT& emcont,u32_t frmillis)
  {
    if ( partices_.Empty() ) 
      return;

    if ( emcont.program.Empty() && FAILED(SetDefaultProgram(&emcont-&emcont_[0])) ) 
      return;

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
            PARTICLE& par = partices_[partidx_++];
            memset(&par,0,sizeof(PARTICLE));
            par.cosin[0] = 1;
            par.scale = 1;
            par.context_ = &emcont;
            par.flags = ComplexSprite::Info::IS_VISIBLE_R;
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
      for ( EMITTER_CONTEXT *i = emcont_.Begin(), *iE = emcont_.End(); i != iE; ++i )
        (*i).rg[cfno*2] = val0, (*i).rg[cfno*2+1] = val1;
    else
      emcont_[emitter].rg[cfno*2] = val0, emcont_[emitter].rg[cfno*2+1] = val1;
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
      for ( EMITTER_CONTEXT *i = emcont_.Begin(), *iE = emcont_.End(); i != iE; ++i )
        (*i).quantity_cf[0] = min, (*i).quantity_cf[1] = max;
    else
      emcont_[emitter].quantity_cf[0] = min, emcont_[emitter].quantity_cf[1] = max;
	  return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetEmissionPeriod(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) ) 
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    if ( emitter < 0 ) 
      for ( EMITTER_CONTEXT *i = emcont_.Begin(), *iE = emcont_.End(); i != iE; ++i )
        (*i).period_cf[0] = min, (*i).period_cf[1] = max;
    else
      emcont_[emitter].period_cf[0] = min, emcont_[emitter].period_cf[1] = max;
	  return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetEmissionTime(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) ) 
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    if ( emitter < 0 ) 
      for ( EMITTER_CONTEXT *i = emcont_.Begin(), *iE = emcont_.End(); i != iE; ++i )
        (*i).timelimit_cf[0] = min, (*i).timelimit_cf[1] = max;
    else
      emcont_[emitter].timelimit_cf[0] = min, emcont_[emitter].timelimit_cf[1] = max;
	  return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetProgram(/*[in]*/ BSTR program,/*[in]*/ long emitter)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ ) return GenSCWideError(NOT_EXISTS_INFO);
    if ( emitter >= long(emcont_.Count()) ) 
      return GenSCWideError(_S*L"emitter %d out of range" %emitter);
    BufferT<u32_t> bytecode;
    widestring errmsg;
    if ( !PspCompile(program,bytecode,errmsg) ) 
      return GenSCWideError(errmsg.Str());
    if ( emitter < 0 ) 
      for ( EMITTER_CONTEXT *i = emcont_.Begin(), *iE = emcont_.End(); i != iE; ++i )
        BufferT<u32_t>(bytecode.Begin(),bytecode.End()).Swap((*i).program);
    else
      BufferT<u32_t>(bytecode.Begin(),bytecode.End()).Swap(emcont_[emitter].program);
	  return S_OK;
  }

HRESULT __stdcall ParticlesHelper::SetDefaultProgram(/*[in]*/ long emitter)
  {
    return SetProgram((BSTR)widestring(particles_default_program).Str(),emitter);
  }

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
    emcont_.Resize(newVal,emcont_.First());
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

void ParticlesHelper::DumpProgram_(long emitter)
  {
    if ( !emcont_[emitter].program.Count() ) SetDefaultProgram(emitter);
    widestring sbuf;
    Decompile(emcont_[emitter].program,sbuf);
    Lout << _S*L"particles: %s: emitter %d program microps\n%s" %sprite_->GetName() %emitter %sbuf;
  }

HRESULT __stdcall ParticlesHelper::DumpProgramMicrops(long emitter)
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
  }

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
