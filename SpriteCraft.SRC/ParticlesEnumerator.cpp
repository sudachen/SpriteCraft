
/*

  $Id$
  Copyright (c) 2005-2006, Alexey Suda-Chen aka hedgehog (alexey@suda-chen.biz)

*/

#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "Particles.h"
#include "ParticlesEnumerator.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Particles")
#endif

STDMETHODIMP ParticlesEnumerator::InterfaceSupportsErrorInfo(REFIID riid)
{
  return IsEqualGUID(TEGGO_GUIDOF(IParticlesEnumerator),riid)?S_OK:S_FALSE;
}

STDMETHODIMP ParticlesEnumerator::get_value(IParticles **pVal)
{
  _SYNCHRONIZE_;
  if ( value_ != particles_.End() ) 
    return (*value_)->QueryInterface(TEGGO_GUIDOF(IParticles),(void**)pVal);
  else *pVal = 0;
	return S_OK;
}

STDMETHODIMP ParticlesEnumerator::Reset(VARIANT_BOOL *bRet)
{
  _SYNCHRONIZE_;
  value_ = particles_.End();
  iter_ = particles_.Begin();
  *bRet =  ( iter_ == particles_.End() )?VARIANT_FALSE:VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP ParticlesEnumerator::Next(VARIANT_BOOL *bRet)
{
  _SYNCHRONIZE_;
  value_ = iter_;
  if ( iter_ != particles_.End() ) { ++iter_; *bRet = VARIANT_TRUE; }
  else *bRet = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP ParticlesEnumerator::get_count(long *pVal)
{
  _SYNCHRONIZE_;
  *pVal = particles_.Count();
	return S_OK;
}

STDMETHODIMP ParticlesEnumerator::Item(long idx, IParticles **pI)
{
  _SYNCHRONIZE_;
  if ( idx < 0 || unsigned(idx) >= particles_.Count() )
    return GenSCWideError(_S*L"Index %d out of range [0,%d)" %idx %particles_.Count());
  return particles_[idx]->QueryInterface(TEGGO_GUIDOF(IParticles),(void**)pI);
}

void ParticlesEnumerator::EnumerateParticles()
{
  ArrayT<Sprite*> e;
  if ( SPRITES::EnumerateSprites(e) != SCE_OK ) return;
  particles_.Reserve(e.Count());
  for ( Sprite **i = e.Begin(), **iE = e.End(); i != iE; ++i )
  {
    ParticlesHelper* helper = Adaptate<ParticlesHelper>((*i)->GetSubImpl());
    if ( helper ) particles_.Push(Refe((IParticles*)helper));
  }
  iter_ = particles_.Begin();
  value_ = particles_.End();
}

STDMETHODIMP ParticlesEnumerator::Get(IParticles **pI)
{
	return get_value(pI);
}
