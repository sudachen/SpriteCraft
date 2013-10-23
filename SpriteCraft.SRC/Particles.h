
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
#if !defined __SpriteCraft_Impl_Particles_h__
#define __SpriteCraft_Impl_Particles_h__

#include "../ScraftAPI/spritecraft.h"
#include "RBGFilter.h"
#include "SpriteHelper.h"

struct ParticlesHelper;

struct WeakParticles :
  TeggoObject<
    TeggoDispImpl< IWeakParticles,
         TeggoIfs< IOnUpdateListener,
                   TeggoISupportErrorInfo 
  > > >
  {
    ParticlesHelper* ph_;
    WeakParticles(ParticlesHelper* s);
    virtual ~WeakParticles();
    void ForgetParticles();
    virtual HRESULT __stdcall get_value(IParticles** pI);
    virtual bool OnUpdateListener(u32_t tickdelta);
    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid)
      {
        return IsEqualGUID(TEGGO_GUIDOF(IWeakParticles),riid)?S_OK:S_FALSE;
      }
  };

typedef 
  TeggoPyDispImpl< IParticles, 0,
       TeggoIfs< ISpriteHelperEx,
                 TeggoISupportErrorInfo > > ParticlesHelperBasic; 

struct ParticlesHelper 
  : ParticlesHelperBasic, SpriteSubImpl, ComplexSprite, IColorHelper
  {
    enum { CF_COUNT = 16 };

    struct EMITTER_CONTEXT 
      {
        enum 
          {
            REG_CF        = 0,
            REG_CF_COUNT  = 16,
            REG_CF_END    = REG_CF + REG_CF_COUNT*2,
            REG_SHADOW00  = REG_CF_END,
            REG_SHADOW01,
            REG_SHADOW1,
            REG_SHADOW1_HI,
            REG_A,
            REG_A0,
            REG_Ai_COUNT  = 4,
            REG_A_END     = REG_A0 + REG_Ai_COUNT*2,
            REG_DT        = REG_A_END,
            REG_IDX, 
            REG_T, 
            REG_COUNT,
          };
        float rg[REG_COUNT];
        long  quantity_cf[2];
        long  period_cf[2];
        long  timelimit_cf[2];
        long  timelimit;
        long  lastemission;
        long  nextemission;
        BufferT<u32_t> program;
        void Clear()
          {
            memset(rg,0,sizeof(rg));
            memset(quantity_cf,0,sizeof(quantity_cf));
            memset(period_cf,0,sizeof(period_cf));
            memset(timelimit_cf,0,sizeof(timelimit_cf));
            rg[EmCfInitScale*2] = rg[EmCfInitScale*2+1] = 100.0f;
            program.Clear();
          }
        EMITTER_CONTEXT() 
          { 
            Clear(); 
          }
      };

    struct PARTICLE 
      {
        long  flags;
        float x[2];
        float scale;
        float cosin[2];
        float trans;
        long  frno;
        // -- 32 bytes
        float v[2];
        float r[2];
        float f;
        long  epi;
        EMITTER_CONTEXT* context_;
        float angle;
      };

    Sprite* sprite_;
    u32_t lifetime_;
    u32_t startime_;
    rcc_ptr<WeakParticles> weak_;
    rcc_ptr<IUnknown> subimpl_;
    rcc_ptr<IOnStopEmission> onStopEmission_; // listener
    rcc_ptr<IOnEmission> onEmission_;
    rcc_ptr<IOnLifetimeOut> onLifeTimeOut_;

    long ose_cookie_;
    BufferT<PARTICLE> partices_;
    PARTICLE *complex_p_;
    long partidx_;
    ArrayT<EMITTER_CONTEXT> emcont_;
    Randomizer randomizer_;
    
    struct 
      {
        bool autoupdate_ : 1;
        bool noname_ : 1;
        bool cycled_: 1;
      };
  
    virtual bool ComplexSpriteReset(bool blendstate);
    virtual bool ComplexSpriteNext(Info* info, bool blendstate);
    virtual bool HasBlendState(bool);
    virtual void   IColorHelper_SetColor(mrgb_t);
    virtual mrgb_t IColorHelper_GetColor() const;
    virtual u32_t __stdcall  AddRef() = 0;
    virtual u32_t __stdcall  Release() = 0;

    static pchar_t const TypeID__;
    virtual void* Adaptate_(const void* id);
    virtual void  SetSpritePtr(Sprite* p);

    virtual void Grab_() const 
      { 
        ((IParticles*)this)->AddRef(); 
      }

    virtual void Release_() const 
      { 
        ((IParticles*)this)->Release(); 
      }

    ParticlesHelper();
    virtual ~ParticlesHelper();
    virtual u32_t Ex_GetParentDeps() const;
    virtual void Ex_SetParentDeps(u32_t);
    virtual void XComObject__check() = 0;

    void CheckDispose();
    void OnStopEmission(bool);
    void NotifyLifeTimeOut();

    HRESULT Activate(bool activate);
    bool    IsActive() 
      { 
        return autoupdate_; 
      } 

    bool Update_(u32_t tickdelta);
    void Update_(PARTICLE &particle,u32_t tickdelta);
    void DumpProgram_(long emitter);
    void ExecuteProgram_(PARTICLE &particle,u32_t tickdelta);
    void Emit_(EMITTER_CONTEXT& emitter,u32_t framemillis);
    void Reset_();
    long Count_() 
      { 
        return partices_.Count(); 
      }

    bool PspCompile(pwide_t source,BufferT<u32_t> &program,BaseStringT<wchar_t> &errorS);
    void Decompile(BufferT<u32_t> const &program, BaseStringT<wchar_t> &sbuf);
    static pwide_t RegToSS(u32_t reg, BaseStringT<wchar_t> &ws);
    static pwide_t RegToVS(u32_t reg, BaseStringT<wchar_t> &ws);

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
	  HRESULT __stdcall DispatchSubImpl(/*[in]*/ IDispatch* subimpl);
	  HRESULT __stdcall get_cycled(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall put_cycled(/*[in]*/ VARIANT_BOOL newVal);
	  HRESULT __stdcall DumpProgramMicrops(/*[in,defaultvalue(0)]*/ long emitter);
	  HRESULT __stdcall get_lifeTime(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_lifeTime(/*[in]*/ long newVal);
	  HRESULT __stdcall get_emCount(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_emCount(/*[in]*/ long newVal);
    HRESULT __stdcall GetKlassAsPWIDE(/*[out,retval]*/ wchar_t** p);
    HRESULT __stdcall GetNameAsPWIDE(/*[out,retval]*/ wchar_t** p);
	  HRESULT __stdcall StopEmission(long timeout);
	  HRESULT __stdcall StartEmission(/*[in]*/ IUnknown* litener,/*[in]*/ long cookie);
	  HRESULT __stdcall get_weakref(/*[out, retval]*/ IWeakParticles* *pVal);
	  HRESULT __stdcall get_alive(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall get_subimpl(/*[out, retval]*/ IUnknown* *pVal);
	  HRESULT __stdcall put_subimpl(/*[in]*/ IUnknown* newVal);
	  HRESULT __stdcall get_dispatcher(/*[out, retval]*/ IUnknown* *pVal);
	  HRESULT __stdcall put_dispatcher(/*[in]*/ IUnknown* newVal);
	  HRESULT __stdcall put_sublayer(/*[in]*/ long layer);
	  HRESULT __stdcall get_sublayer(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_layer(/*[in]*/ long layer);
	  HRESULT __stdcall get_layer(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall get_name(/*[out, retval]*/ BSTR *pVal);
	  HRESULT __stdcall get_klass(/*[out, retval]*/ BSTR *pVal);
	  HRESULT __stdcall get_active(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall Reset();
	  HRESULT __stdcall Dispose();
	  HRESULT __stdcall get_hierarchy(/*[in]*/IHierarchy** pI);
	  HRESULT __stdcall get_visible(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall put_visible(/*[in]*/ VARIANT_BOOL newVal);
	  HRESULT __stdcall get_parent(/*[out, retval]*/ ISprite* *pVal);
	  HRESULT __stdcall put_parent(/*[in]*/ ISprite* newVal);
	  HRESULT __stdcall get_cfilt(/*[out, retval]*/ IRGBFilter **pVal);
	  HRESULT __stdcall get_transparency(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_transparency(/*[in]*/ float newVal);
	  HRESULT __stdcall get_luminance(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_luminance(/*[in]*/ float newVal);
	  HRESULT __stdcall get_angle(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_angle(/*[in]*/ float newVal);
	  HRESULT __stdcall put_xyScale(/*[in]*/ float newVal);
	  HRESULT __stdcall get_yScale(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_yScale(/*[in]*/ float newVal);
	  HRESULT __stdcall get_xScale(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_xScale(/*[in]*/ float newVal);
	  HRESULT __stdcall get_y(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_y(/*[in]*/ float newVal);
	  HRESULT __stdcall get_x(/*[out, retval]*/ float *pVal);
	  HRESULT __stdcall put_x(/*[in]*/ float newVal);
	  HRESULT __stdcall get_count(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall put_count(/*[in]*/ long newVal);
    HRESULT __stdcall SetEmitterCf(/*[in]*/ long cfno, /*[in]*/ float val0,/*[in]*/ float val1,/*[in]*/ long emitter);
    HRESULT __stdcall SetEmissionQuantity(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter);
    HRESULT __stdcall SetEmissionPeriod(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter);
    HRESULT __stdcall SetEmissionTime(/*[in]*/ long min,/*[in]*/ long max,/*[in]*/ long emitter);
    HRESULT __stdcall SetProgram(/*[in]*/ BSTR program,/*[in]*/ long emitter);
    HRESULT __stdcall SetDefaultProgram(/*[in]*/ long emitter);
	  HRESULT __stdcall get_autodispose(/*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall put_autodispose(/*[in]*/ VARIANT_BOOL newVal);
  };

HRESULT CreateParticlesHalper(Sprite* p,IParticles** pI,bool noname);

#endif //__SpriteCraft_Impl_Particles_h__
