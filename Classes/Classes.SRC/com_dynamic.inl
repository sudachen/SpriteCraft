
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

#if !defined ___3e68845a_b66f_46ec_be91_2d8510432bf2___
#define ___3e68845a_b66f_46ec_be91_2d8510432bf2___

#include "com_dynamic.h"

#if defined _TEGGO_COMDYNAMIC_HERE
# define _TEGGO_COMDYNAMIC_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_COMDYNAMIC_FAKE_INLINE CXX_FAKE_INLINE
#endif

namespace teggo {

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  HRESULT TypeInfo_::GetTI(LCID lcid, ITypeInfo** info, HinstanceType &hinstance)
    {
      HRESULT hr = S_OK;
      if (!info_)
        hr = GetTI(lcid,hinstance);
      *info = Refe(+info_);
      return hr;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  HRESULT TypeInfo_::EnsureTI(LCID lcid, HinstanceType &hinstance)
    {
      if (!info_)
        return GetTI(lcid,hinstance);
      else
        return S_OK;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  HRESULT TypeInfo_::GetTypeInfo(UINT , LCID lcid, ITypeInfo** info, HinstanceType &hinstance)
    {
      if ( info )
        return GetTI(lcid, info, hinstance);
      else
        return E_POINTER;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  HRESULT TypeInfo_::GetIDsOfNames(
    REFIID, LPOLESTR* names, UINT count, LCID lcid, DISPID* dispid,
    HinstanceType &hinstance)
    {
      HRESULT hr = EnsureTI(lcid,hinstance);
      if ( info_ )
        {
          for ( unsigned i=0; i<count; ++i )
            {
              DISPID__ v = {0,names[i]};
              if ( DISPID__ *di = dispids_.Bfind(v) )
                dispid[i] = di->id;
              else
                {
                  hr = info_->GetIDsOfNames(names+i, 1, dispid+i);
                  if (FAILED(hr)) break;
                }
            }
        }
      return hr;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  HRESULT TypeInfo_::Invoke(
    IDispatch* p, DISPID dispidMember, REFIID /* riid */,
    LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
    EXCEPINFO* pexcepinfo, UINT* puArgErr,
    HinstanceType &hinstance)
    {
      HRESULT hr = EnsureTI(lcid,hinstance);
      if ( info_ )
        hr = info_->Invoke(
                      p,
                      dispidMember, wFlags, pdispparams, pvarResult,
                      pexcepinfo, puArgErr);
      return hr;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  HRESULT TypeInfo_::LoadNameCache( ITypeInfo* pTypeInfo )
    {
      TYPEATTR* pta;
      HRESULT hr = pTypeInfo->GetTypeAttr(&pta);

      if (SUCCEEDED(hr))
        {
          dispids_.Reserve(pta->cFuncs);
          for ( int i=0, iE = dispids_.Count(); i != iE ; ++i)
            {
              FUNCDESC* pfd;
              if (SUCCEEDED(pTypeInfo->GetFuncDesc(i, &pfd)))
              {
                bstr_ptr name;
                if (SUCCEEDED(pTypeInfo->GetDocumentation(pfd->memid, &+name, NULL, NULL, NULL)))
                {
                  DISPID__ v = {pfd->memid,name.forget_()};
                  dispids_.Push(v);
                }
                pTypeInfo->ReleaseFuncDesc(pfd);
              }
            }
          pTypeInfo->ReleaseTypeAttr(pta);
          qsort(dispids_.Begin(),dispids_.Count(),sizeof(DISPID__),DISPID__::Qscmp);
        }
      return S_OK;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  TypeInfo_::TypeInfo_(const GUID* iid)
    {
      iid_   = iid;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  TypeInfo_::~TypeInfo_()
    {
      for ( DISPID__ *i = dispids_.Begin(); i != dispids_.End(); ++i )
        i->Dispose();
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  HRESULT TypeInfo_::GetTI(LCID lcid, HinstanceType &hinstance)
    {
      if ( !info_ )
        {
          HRESULT hr = S_OK;
          // synhronize ???
          rcc_ptr<ITypeLib> pTypeLib;
          hr = ::LoadTypeLib(hinstance->GetModulePath(),&+pTypeLib);
          if (SUCCEEDED(hr))
            {
              hr = pTypeLib->GetTypeInfoOfGuid(*iid_, &+info_);
              if (SUCCEEDED(hr))
                LoadNameCache(+info_);
            }
          return hr;
        }
      else
        return S_OK;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  TypeInfo_* TypeInfo_Factory_::GetTypeinfo(const GUID* iidGUID)
    {
      if ( !map_ )
        map_ = new TLB_MAP;
      if ( TypeInfo_ **i = map_->Get(*iidGUID) )
        {
          return *i;
        }
      else
        {
          TypeInfo_ *val = new TypeInfo_(iidGUID);
          map_->Put(*iidGUID,val);
          return val;
        }
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  void TypeInfo_Factory_::Term()
    {
      if ( map_ )
        {
          TLB_MAP::Iterator i = map_->Iterate();
          while ( i.Next()  ) delete *i;
          delete map_; map_ = 0;
        }
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  TypeInfo_Factory_::~TypeInfo_Factory_()
    {
      Term();
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  TypeInfo_Factory_::TypeInfo_Factory_() : map_(0)
    {
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  TypeInfo_Factory_& TypeInfoFactory()
    {
      static TypeInfo_Factory_ factory_;
      return factory_;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  void InterlockedInitHolder(TypeInfo_ **holder,GUID const *guid)
    {
      TypeInfo_ *h = TypeInfoFactory().GetTypeinfo(guid);
      //InterlockedExchange((u32_t*)holder,(u32_t)h);
      *holder = h;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  int CXX_CDECL TypeInfo_::DISPID__::Qscmp(void const* aa,void const* bb)
    {
      DISPID__ const *a = (DISPID__ const *)aa;
      DISPID__ const *b = (DISPID__ const *)bb;
      if ( !b->name ) return false;
      if ( !a->name ) return true;
      return wcscmp(a->name,b->name) < 0;
    }

  _TEGGO_COMDYNAMIC_FAKE_INLINE
  DispResult CXX_STDCALL Disp_Invoker_Proceed_Common_(
    Disp_Invoker_Base_ const *di, int argCnt, VARIANT *arguments )
    {

      for ( int u = 0 ; u < argCnt; ++u )
        {
          STRICT_REQUIRE( di->next_ != 0 );
          arguments[u] = di->_.value_;
          di = di->next_;
        }

      STRICT_REQUIRE( di->next_ == 0 );

      if ( !di->_.invoker_.pdisp )
        return DispResult(di->_.invoker_.dispid_or_hresult);

      HRESULT hResult = E_FAIL;
      DISPPARAMS params = {0};
      params.rgvarg = arguments;
      params.cArgs  = argCnt;

      VARIANT result;
      memset(&result,0,sizeof(result));

      hResult =
        di->_.invoker_.pdisp->Invoke(
          di->_.invoker_.dispid_or_hresult,
            IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,
            &params,
            &result,
            0,0);

      return DispResult(result,hResult);
    }

} // namespace

#endif // #define ___3e68845a_b66f_46ec_be91_2d8510432bf2___
