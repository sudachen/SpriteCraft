
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
#include "ObjList.inl"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Lists$ObjList")
#endif

HRESULT __stdcall ObjListBase::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(IObjList),riid)?S_OK:S_FALSE;
  }

ObjListBase::ObjListBase()
  : started_(false), removed_(false)
  {
  }

ObjListBase::~ObjListBase()
  {
  }

HRESULT __stdcall ObjListBase::Reset(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    curr_ = objects_.Begin();
    started_ = false;
    removed_ = false;
    *pVal =  curr_ ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ObjListBase::Next(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( started_ )
      *pVal =  curr_.SafeNext() ? VARIANT_TRUE : VARIANT_FALSE;
    else
      {
        *pVal = curr_ ? VARIANT_TRUE : VARIANT_FALSE;
        started_ = true;
      }
    removed_ = false;
	  return S_OK;
  }

HRESULT __stdcall ObjListBase::Get(IUnknown **pI)
  {
    _SYNCHRONIZE_;
    *pI = ( !removed_ && curr_ ) ? Refe(*curr_) : 0;
	  return S_OK;
  }

HRESULT __stdcall ObjListBase::Remove()
  {
    _SYNCHRONIZE_;
    if ( !curr_ ) 
      return GenSCWideError(L"remove unaplicatable because has no operatable element",0);
    if ( !removed_ ) 
      {
        objects_.Erase(curr_);
        removed_ = true;
      }
	  return S_OK;
  }

HRESULT __stdcall ObjListBase::Add(IUnknown *pI)
  {
    _SYNCHRONIZE_;
    objects_.Insert(objects_.End(),RccRefe(pI));
	  return S_OK;
  }

HRESULT __stdcall ObjListBase::Find(IUnknown *pI, VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    OBJLST::Position p = objects_.Begin();
    for ( ; p ; ++p ) 
      {
        if ( +(*p) == pI ) 
          {
            curr_ = p;
            *pVal = VARIANT_TRUE;
            return S_OK;      
          }
      } 
    curr_ = objects_.End();
    *pVal = VARIANT_FALSE;
	  return S_OK;
  }

HRESULT Engine_NewObjList(IObjList** pI)
  {
    *pI = new TeggoObject<ObjListBase>();
    return S_OK;
  }

HRESULT __stdcall ObjListBase::Clear()
  {
    _SYNCHRONIZE_;
    objects_.Clear();
    curr_ = objects_.End();
	  return S_OK;
  }

HRESULT __stdcall ObjListBase::get_count(long* count)
  {
    _SYNCHRONIZE_;
    *count = objects_.Count();
    return S_OK;
  }

HRESULT __stdcall ObjListBase::IsBegin(IUnknown* pI,VARIANT_BOOL* ret)
  {
    _SYNCHRONIZE_;
    *ret = 
      !objects_.Empty() && +objects_.First() == pI
      ? VARIANT_TRUE
      : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ObjListBase::IsEnd(IUnknown* pI,VARIANT_BOOL* ret)
  {
    _SYNCHRONIZE_;
    *ret = 
      !objects_.Empty() && +objects_.Last() == pI
      ? VARIANT_TRUE
      : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall ObjListBase::AtBegin(IUnknown** pI)
  {
    _SYNCHRONIZE_;
    if ( objects_.Empty() ) 
      *pI = 0;
    else 
      *pI = Refe(objects_.First());
    return S_OK;
  }

HRESULT __stdcall ObjListBase::AtEnd(IUnknown** pI)
  {
    _SYNCHRONIZE_;
    if ( objects_.Empty() ) 
      *pI = 0;
    else 
      *pI = Refe(objects_.Last());
    return S_OK;
  }

HRESULT __stdcall ObjListBase::At(long idx,IUnknown** pI)
  {
    _SYNCHRONIZE_;
    *pI = 0;
    if ( !objects_.Empty() ) 
      {
        int i = 0;
        OBJLST::Position p = objects_.Begin();
        for (; p; ++i, ++p )
          if ( i == idx ) 
            { 
              *pI = Refe(+*p); 
              break; 
            }
      }
    return S_OK;
  }
