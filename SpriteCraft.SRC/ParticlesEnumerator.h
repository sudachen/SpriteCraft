
/*

  $Id$
  Copyright (c) 2005-2006, Alexey Suda-Chen aka hedgehog (alexey@suda-chen.biz)

*/

#if !defined __SpriteCraft_Impl_ParticlesEnumerator_h__
#define __SpriteCraft_Impl_ParticlesEnumerator_h__

struct ParticlesEnumerator : 
  TeggoObject<
    TeggoPyDispImpl<IParticlesEnumerator, 0, TeggoISupportErrorInfo> >
  {
    typedef IParticles* e_VALUE;
    typedef ArrayT<e_VALUE> e_VECTOR;
    e_VECTOR particles_;
    e_VALUE *iter_;
    e_VALUE *value_;

    ParticlesEnumerator() 
      { 
        EnumerateParticles(); 
      }

    ~ParticlesEnumerator() 
      {
        for ( e_VALUE *i = particles_.Begin(), *iE = particles_.End(); i != iE; ++i ) 
          {
            (*i)->Release();
          }
      }

    void EnumerateParticles();

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
	  HRESULT __stdcall Item(/*[in]*/ long,/*[out,retval]*/ IParticles** pI);
	  HRESULT __stdcall get_count(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall Next(/*[out,retval]*/ VARIANT_BOOL* bRet);
	  HRESULT __stdcall Reset(/*[out,retval]*/ VARIANT_BOOL* bRet);
	  HRESULT __stdcall get_value(/*[out, retval]*/ IParticles* *pVal);
    HRESULT __stdcall Get(IParticles **);
  };

#endif // __SpriteCraft_Impl_ParticlesEnumerator_h__
