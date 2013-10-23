
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

#include "../Classes/Classes.SRC/_specific.h"

#if CXX_COMPILER_IS_MCW || CXX_COMPILER_IS_GXX
#define Engine __Com_Scraft_Engine
#endif

#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "spritecraft.h"
#undef Engine

#include "../Classes/Classes.SRC/string.h"
#include "../Classes/Classes.SRC/array.h"
#include "../Classes/Classes.SRC/guid.h"
#include "../Classes/Classes.SRC/format.h"

enum Sclog_ { Sclog };
template < class tTc > Sclog_ operator << (Sclog_, teggo::FormatT<tTc> const &fmt);

#include "../Classes/Classes.SRC/_cxxproperties.h"
typedef ::teggo::Property_String<wchar_t> __ScString;

#include "../Classes/Classes.SRC/COM_generic.h"
//#define TeggoIfs0 TeggoIfs0
//#define TeggoObject TeggoObject
#define ScraftIfsUnknown TeggoIfsUnknown

using teggo::IUnknownPtr;

#include "../Classes/Classes.SRC/Wexcept.h"

#include "guids_.h"

/*-------------------------------------------------------------------------------------*/
// scraftapi <= 2.2 compatibility
//#define _scraft_ptr ::teggo::teggo::rcc_ptr
//template < class _Tx > inline teggo::rcc_ptr<_Tx> _ScraftPtr(_Tx *ref) { return RccPtr(ref); }
//template < class _Tx > inline teggo::rcc_ptr<_Tx> _ScraftRefe(_Tx *ref) { return RccPtr(Refe(ref)); }

#if !defined XMessage
#  if defined _DEBUG
#    define XMessage(x) Scraft::LogF x
#  else
#    define XMessage(x)
#  endif
#endif

#if !defined XMessageW
#  if defined _DEBUG
#    define XMessageW(x) Scraft::WideLogF x
#  else
#    define XMessageW(x)
#  endif
#endif

template <class T> inline
  void scraft_delete(T*& p)
    {
      delete p; p = 0;
    }

struct ScraftException : teggo::WideException
  {
  public:
    ScraftException(pwide_t what=0) : teggo::WideException(what) {}
    ScraftException(const teggo::FormatT<wchar_t>& fwhat) : teggo::WideException(fwhat) {}
  };

struct ScraftComException : public Teggo_Com_Exception< ScraftException >
  {
  };

void Throw_ScraftComException();

struct ScraftNullException : public ScraftException
  {
    ScraftNullException() : ScraftException(L"null exception") {}
  };

struct scraft_fake_t
  {
  };

template<class U,class T>
  teggo::rcc_ptr<U> scraft_com_cast(T* obj,const U* fake=0)
    {
      U* ifs;
      if ( !obj ) throw ScraftNullException();
      if ( FAILED(obj->QueryInterface(TEGGO_GUIDOF(U),(void**)&ifs)) )
        throw ScraftException(L"bad COM cast");
      return teggo::rcc_ptr<U>(ifs);
    }

template<class U,class T>
  teggo::rcc_ptr<U> scraft_com_cast(const teggo::rcc_ptr<T>& obj,const U* fake=0)
    {
      return scraft_com_cast<U>(+obj);
    }

template<class U,class T>
  teggo::rcc_ptr<U> scraft_com_cast_(T* obj,const U* fake=0)
    {
      U* ifs = 0;
      if ( obj )
        obj->QueryInterface(TEGGO_GUIDOF(U),(void**)&ifs);
      return teggo::rcc_ptr<U>(ifs);
    }

template<class U,class T>
  teggo::rcc_ptr<U> scraft_com_cast_(const teggo::rcc_ptr<T>& obj,const U* fake=0)
    {
      return scraft_com_cast_<U>(+obj);
    }

struct scraft_bstr_guard
  {
    BSTR bstr_;

    scraft_bstr_guard() : bstr_ (0)
      {
      }

    scraft_bstr_guard(pwide_t str) : bstr_ (SysAllocString(str))
      {
      }

    ~scraft_bstr_guard()
      {
        if ( bstr_ )
          SysFreeString(bstr_);
      }

    BSTR* Ref()
      {
        return &bstr_;
      }

    BSTR  Val()
      {
        return bstr_;
      }

    __ScString Ws()
      {
        return __ScString(bstr_?bstr_:L"");
      }
  };

template < class U > inline
  IUnknown* scraft_query_ifs(U* u,int = 0)
    {
      return (IUnknown*) u;
    }
