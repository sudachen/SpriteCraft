
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
