
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

#if !defined __mutil_com_h__
#define __mutil_com_h__

#include "../mcore/m_autoconf.h"
#include "../../Classes/teggo/all_core.h"
#include "../../Classes/teggo/all_util.h"
#include "../../Classes/teggo/all_mscom.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma warning(disable:4786)
#endif

#include <ole2.h>
#include <ocidl.h>

//extern HINSTANCE mcom_hInstance__; // disp interfaces and reg only

NAMESPACE_MSKIN_BEGIN

template<class T> 
inline T mcom_minimal(T a,T b) { return a <= b ? a : b ; }
template<class T> 
inline T mcom_maximal(T a,T b) { return a >= b ? a : b ; }

inline wchar_t CharToWChar(char c) { return utf16_btowc(c); }
inline char WCharToChar(wchar_t w) { return utf16_wctob(w); }

BSTR StringToBSTR(pchar_t val);

//inline
//wchar_t* UnicodeToLower(const wchar_t* text,unsigned len,wchar_t* outbuf)
//{
//  for ( unsigned i = 0; i < len; ++i ) outbuf[i] = towlower(text[i]);
//  return outbuf;
//}
//
//inline
//wchar_t* UnicodeToUpper(const wchar_t* text,unsigned len,wchar_t* outbuf)
//{
//  for ( unsigned i = 0; i < len; ++i ) outbuf[i] = towupper(text[i]);
//  return outbuf;
//}

//widestring WideToString(const wchar_t* val);
//inline string  BSTRToString(BSTR val) { return WideToString(val); }

//template <class T> inline T* Refe(T* ref) { if ( ref ) ref->AddRef(); return ref; }
//template <class T> inline void Unrefe(T*& ref) { if ( ref ) ref->Release(); ref = 0; }

#if defined __IMPLEMENTS_UUIDOF
template<class T>
struct mcom_uuidof__ {
  static const GUID guid;
};
#define __uuidof(x) (mcom_uuidof__<x>::guid)
#endif

template<class T> inline pchar_t InterfaceToString(T* t,BaseStringT<char>& msb) {
  const GUID* uuidof = &__uuidof(T);
  if ( uuidof ) return teggo::GUIDToRegAsciiString(*uuidof,msb);
  else return "undefined";
}

#define DECLARE_COM_INTERFACE(T) typedef rcc_ptr<T> T ## Ptr

struct QueryIfsFakeWrap : public IUnknown{
  virtual HRESULT InternalQueryInterface(REFIID riid,void ** pI) {
    if ( IsEqualIID(riid,IID_IUnknown) ) 
      { *pI = (IUnknown*)this; this->AddRef(); return S_OK; }
    return E_NOINTERFACE; 
  }
  IUnknown* iunknown_() { return this; }
};

template<class Ifs, class W=QueryIfsFakeWrap>
struct QueryIfsWrap : public Ifs, public W{
  virtual HRESULT InternalQueryInterface(REFIID riid ,void ** pI) {
    if ( IsEqualIID(riid,TEGGO_GUIDOF(Ifs)) ) 
      { *pI = (Ifs*) this; this->AddRef(); return S_OK; }
    return W::InternalQueryInterface(riid,pI);
  }
  virtual u32_t __stdcall AddRef() = 0;
};

#define COM_INTERFACES0                 QueryIfsFakeWrap
#define COM_INTERFACES1(I0)             QueryIfsWrap< I0 >
#define COM_INTERFACES2(I0,I1)          QueryIfsWrap< I0, QueryIfsWrap<I1> >
#define COM_INTERFACES3(I0,I1,I2)       QueryIfsWrap< I0, COM_INTERFACES2(I1,I2) >
#define COM_INTERFACES4(I0,I1,I2,I3)    QueryIfsWrap< I0, COM_INTERFACES3(I1,I2,I3) >
#define COM_INTERFACES5(I0,I1,I2,I3,I4) QueryIfsWrap< I0, COM_INTERFACES4(I1,I2,I3,I4) >

template < class T >
class ComObject : public T {
  long refcount_;
public:
  virtual u32_t __stdcall Release() { 
    long refcount = InterlockedDecrement(&refcount_);
    if ( refcount == 0 ) { __dispose(); }
    return refcount;
  }
  virtual u32_t __stdcall AddRef() {
    long refcount = InterlockedIncrement(&refcount_);
    return refcount_;
  }
  virtual long __stdcall QueryInterface(REFIID riid,void ** pI){
    return /*static_cast<QueryIfsFakeWrap*>(this)->*/InternalQueryInterface(riid,pI);
  }
  u32_t GetRefCount__() { return refcount_; }
  virtual void __dispose() { delete this; }
  ComObject() : refcount_(1) {}
  virtual ~ComObject() {}
};

template<class T> 
struct ComObjectFactory{
  struct ifactory : public COM_INTERFACES1(IClassFactory) {
    virtual HRESULT __stdcall CreateInstance(IUnknown* pOuter,const IID& riid, void** ppv) {
      rcc_ptr< ComObject<T> > cls ( new ComObject<T> );
      return cls->QueryInterface(riid,ppv);
    }
    virtual HRESULT __stdcall LockServer(BOOL lock) { return S_OK; }
  };
  static HRESULT MakeFactory(const GUID& riid,void** ppv) {
    rcc_ptr<IClassFactory> ifc ( new ComObject<ifactory> );
    return ifc->QueryInterface(riid,ppv);
  }
};

//template<class T> 
//struct rcc_ptr
//{
//  mutable T* ref_;
//  explicit rcc_ptr(T* t=0) : ref_(t) {}
//  explicit rcc_ptr(T* t,bool addref) : ref_(t) { if (addref) Refe(ref_); }
//  rcc_ptr(const rcc_ptr& a) : ref_( Refe(a.ref_) ) {}
//  template<class U> rcc_ptr(const rcc_ptr<U>& a,bool = false) : ref_(0) { operator =(a); }
//  ~rcc_ptr() { Unrefe(ref_); }
//  bool operator !() const { return !ref_; }
//  T& operator *() const { return *ref_; }
//  T* operator ->() const { return ref_; }
//  template<class U>
//  const rcc_ptr& operator=(const rcc_ptr<U>& a)
//  { 
//    T* ref = 0;
//    if ( ~a ) a->QueryInterface(__uuidof(T),(void**)&ref);
//    if ( ref ) std::swap(ref,ref_);
//    Unrefe(ref);
//    return *this;
//  }
//  const rcc_ptr& operator=(const rcc_ptr<T>& a) { reset_(Refe(a.ref_)); return *this; }
//  bool operator == ( const rcc_ptr& a ) const { return ref_ == a.ref_; }
//  bool operator < ( const rcc_ptr& a ) const { return ref_ < a.ref_; }
//  T*& operator ~ () const { return  ref_; }
//  template<class U>
//  void reset_(U* t) { if ( t != ref_ ) { Unrefe(ref_); operator = (rcc_ptr<U>(t)); } }
//  void reset_(T* t) { if ( t != ref_ ) { Unrefe(ref_); ref_ = t; } }
//  T* forget_() { T* t=0; std::swap(t,ref_); return t; }
//private:  
//  void operator=(const T*);
//};
//
//template<class T_> inline T_* Refe(const rcc_ptr<T_>& ref) { if ( ref.ref_ ) ref.ref_->AddRef(); return ref.ref_; }
//template <class T> inline void Unrefe(rcc_ptr<T>& ref) { ref.reset_((T*)0); }
//template<class T_> inline T_* Forget(rcc_ptr<T_>& ref) { return ref.forget_(); }
//template<class T_> inline void Reset(rcc_ptr<T_>& ref,T_* p) { ref.reset_(p); }
//template <class T> inline rcc_ptr<T> ComPtr(T* ref) { return rcc_ptr<T>(ref); }
//template <class T> inline rcc_ptr<T> ComRefe(T* ref) { return rcc_ptr<T>(Refe(ref)); }

//template <class T,class U> 
//inline rcc_ptr<T> interface_cast(const rcc_ptr<U>& u) { 
//  return interface_cast<T,U>(~u);
//}
//
//template <class T,class U> 
//inline rcc_ptr<T> interface_cast(U* u) { 
//  T* p = 0;
//  if ( u ) u->QueryInterface(TEGGO_GUIDOF(T),(void**)&p);
//  return rcc_ptr<T>(p);
//}

struct mcom_bstr {
  BSTR val_;
  mcom_bstr() : val_(0) {}
  explicit mcom_bstr(pwide_t val) 
  {
    val_ = 
      val ? SysAllocStringLen(val,wcslen(val)) : SysAllocString(L"");
  }
  mcom_bstr( pchar_t val ) {
    val_ = StringToBSTR(val);
  }
  mcom_bstr& operator =(const BSTR val) { 
    BSTR val0 = SysAllocStringLen(val,wcslen(val));
    if ( val_ ) SysFreeString(val_);
    val_ = val0;
    return *this;
  }
  mcom_bstr& operator =(const mcom_bstr& p) {
    return operator = (p.val_);
  }
  BSTR& operator + () { return val_; }
  bool operator !() { return !val_; }
  ~mcom_bstr() { if ( val_ ) SysFreeString(val_); }
  BSTR forget_() { BSTR val = val_; val_ = 0; return val; }
  void reset_(BSTR val) { 
    if ( val_ == val ) return;
    if ( val_ ) SysFreeString( val_ );
    val_ = val;
  }
  BSTR copy() {
    return SysAllocStringLen(val_,SysStringLen(val_));
  }
};

//inline BSTR Forget(mcom_bstr& bstr) { return bstr.forget_(); } 
//inline void Reset(mcom_bstr& bstr,BSTR val) { bstr.reset_(val); } 
inline bool operator < ( const mcom_bstr& a, const mcom_bstr& b) {
  return memcmp(a.val_,b.val_,mcom_minimal<long>(SysStringLen(a.val_),SysStringLen(a.val_))) < 0;
}
inline bool operator > ( const mcom_bstr& a, const mcom_bstr& b) {
  return memcmp(a.val_,b.val_,mcom_minimal<long>(SysStringLen(a.val_),SysStringLen(a.val_))) > 0;
}
inline bool operator == ( const mcom_bstr& a, const mcom_bstr& b) {
  return memcmp(a.val_,b.val_,mcom_minimal<long>(SysStringLen(a.val_),SysStringLen(a.val_))) == 0;
}
inline bool operator != ( const mcom_bstr& a, const mcom_bstr& b) {
  return !operator ==(a,b);
}
inline bool operator >= ( const mcom_bstr& a, const mcom_bstr& b) {
  return !operator <(a,b);
}
inline bool operator <= ( const mcom_bstr& a, const mcom_bstr& b) {
  return !operator >(a,b);
}

struct Less_BSTR 
{
  bool operator() (BSTR a,BSTR b) const{
    return wcscmp(a,b) < 0;
  }
};

struct Less_iBSTR 
{
  bool operator() (BSTR a,BSTR b) const{
    return wcsicmp(a,b) < 0;
  }
};

//struct ITypeInfo2;

struct GUID_Comparer 
{
  bool operator() (const GUID& a,const GUID& b) const 
    { return memcmp(&a,&b,sizeof(GUID)) < 0; }
};

inline
HRESULT mcom_MYFAILED(HRESULT old,HRESULT result) {
  if ( result != S_OK ) return E_FAIL;
  else return old;
}

HRESULT mcom_WriteRegStr(pchar_t root,pchar_t key,pchar_t subkey,pchar_t value);
HRESULT mcom_DeleteRegKey(pchar_t key);
HRESULT mcom_LoadTypeLib(ITypeLib** pI,wchar_t* p=0);
HRESULT mcom_RegisterTypeLib();
HRESULT mcom_UnRegisterTypeLib();
HRESULT mcom_RegisterCoClass(const GUID& clsid,pchar_t progID,pchar_t desc,long ver=1);
HRESULT mcom_UnRegisterCoClass(const GUID& clsid,pchar_t progID,long ver=1);

class mcom_typeinfo {
public:
  u32_t major_;
  u32_t minor_;
  GUID const *iid_;
  rcc_ptr<ITypeInfo> info_;

  struct DISPID__ {
    DISPID id;
    LPWSTR name;
    int Dispose() { if ( name ) SysFreeString((BSTR)name); name = 0; return 0; }
  };
  BufferT<DISPID__> dispids_;
  
  struct DISPID__Less 
  {
    bool operator() (const DISPID__& a,const DISPID__& b) const 
    {
      if ( !b.name ) return false;
      if ( !a.name ) return true;
      return wcscmp(a.name,b.name) < 0;
    }
  };

public:

  mcom_typeinfo(const GUID* iid);
  ~mcom_typeinfo();
  HRESULT GetTI(LCID lcid, ITypeInfo** info);
  HRESULT GetTI(LCID lcid);
  HRESULT EnsureTI(LCID lcid);
  HRESULT GetTypeInfo(UINT , LCID lcid, ITypeInfo** info);
  HRESULT GetIDsOfNames(REFIID, LPOLESTR* names, UINT count, LCID lcid, DISPID* dispid);
  HRESULT Invoke(IDispatch* p, DISPID dispidMember, REFIID /* riid */,
    LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
    EXCEPINFO* pexcepinfo, UINT* puArgErr);
  HRESULT LoadNameCache(ITypeInfo* pTypeInfo);
};

struct mcom_typeinfo_factory__
{
  typedef PlaneMapT<GUID,mcom_typeinfo*,GUID_Comparer> TLB_MAP;
  TLB_MAP* map_;
  mcom_typeinfo* GetTypeinfo(const GUID* iidGUID);
  void Term();
  ~mcom_typeinfo_factory__();
protected:
  mcom_typeinfo_factory__();
  friend mcom_typeinfo_factory__& mcom_typeinfo_factory();
};

inline
mcom_typeinfo_factory__& mcom_typeinfo_factory() {
  static mcom_typeinfo_factory__ factory_;
  return factory_;
}

template<const GUID* ConIID_Ie,const GUID* CLSID_Co, class Other>
struct ConnIfsWrap : public 
  QueryIfsWrap< IConnectionPoint,
  QueryIfsWrap< IConnectionPointContainer,
  QueryIfsWrap< IProvideClassInfo,Other > > >
{
  virtual HRESULT InternalAdvise(IUnknown* ifs,u32_t* cookie) = 0;
  virtual HRESULT InternalUnadvise(u32_t cookie) = 0;
  virtual HRESULT __stdcall GetConnectionInterface(IID *pIID) {
    *pIID = *ConIID_Ie;
    return S_OK;
  }
  virtual HRESULT __stdcall Advise(IUnknown* pUnkSink,DWORD* pdwCookie) {
    rcc_ptr<IUnknown> ifs;
    pUnkSink->QueryInterface(*ConIID_Ie,(void**)&+ifs);
    return InternalAdvise(+ifs,pdwCookie);
  }
  virtual HRESULT __stdcall Unadvise(DWORD dwCookie) {
    return InternalUnadvise(dwCookie);
  }
  virtual HRESULT __stdcall FindConnectionPoint(REFIID riid,IConnectionPoint **ppCP)
  {
    if ( IsEqualGUID(riid,*ConIID_Ie) ) {
      return InternalQueryInterface(__uuidof(IConnectionPoint),(void**)ppCP);
    }
    return E_NOINTERFACE;
  }
  virtual HRESULT __stdcall GetClassInfo(ITypeInfo** ppTI) {
    return mcom_typeinfo_factory().GetTypeinfo(CLSID_Co)->GetTypeInfo(0,0,ppTI);
  }
  virtual HRESULT __stdcall EnumConnections(IEnumConnections **ppEnum) {return E_UNEXPECTED;}
  virtual HRESULT __stdcall EnumConnectionPoints(IEnumConnectionPoints **ppEnum) {return E_UNEXPECTED;}
  virtual HRESULT __stdcall GetConnectionPointContainer(IConnectionPointContainer **ppCPC) { return E_UNEXPECTED; }
};

#define COM_CONNIFS( IE, CO, Other ) ConnIfsWrap< IE, CO , Other >

void mcom_InterlockedInitHolder(mcom_typeinfo** holder,const GUID* guid);

template<
  class T,
  class W=QueryIfsFakeWrap>
struct DispIfsWrap : public T, public W
{
  
  static mcom_typeinfo& GetHolder() { 
    static mcom_typeinfo* holder = 0;
    if ( !holder ) mcom_InterlockedInitHolder(&holder,&__uuidof(T));
    return *holder; 
  }
  
  DispIfsWrap() {}

  virtual HRESULT __stdcall GetTypeInfoCount(UINT* pctinfo){
    *pctinfo = 1;
    return S_OK;
  }
  virtual HRESULT __stdcall GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo){
    return GetHolder().GetTypeInfo(itinfo, lcid, pptinfo);
  }
  virtual HRESULT __stdcall GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
    LCID lcid, DISPID* rgdispid)
  {
    return GetHolder().GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
  }
  virtual HRESULT __stdcall Invoke(
    DISPID dispidMember, REFIID riid,
    LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
    EXCEPINFO* pexcepinfo, UINT* puArgErr)
  {
    return GetHolder().Invoke(
      this, dispidMember, riid, lcid,
      wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
  }
  virtual HRESULT InternalQueryInterface(REFIID riid ,void ** pI) {
    if ( IsEqualIID(riid,__uuidof(T)) ) 
      { *(T**)pI = this; this->AddRef(); return S_OK; }
    if ( IsEqualIID(riid,__uuidof(IDispatch)) ) 
      { *(IDispatch**)pI = (T*)this; this->AddRef(); return S_OK; }
    return W::InternalQueryInterface(riid,pI);
  }
  virtual u32_t __stdcall AddRef() = 0;
};

#define DISPIFS0(T) DispIfsWrap<T,QueryIfsFakeWrap>
#define DISPIFS(T,ifss) DispIfsWrap<T, ifss >

struct ComDispResult {
  HRESULT hResult_;
  VARIANT result_;
  ComDispResult(const VARIANT& v,HRESULT h) : hResult_(h), result_(v)  {}
  ComDispResult(HRESULT h) : hResult_(h)  { VariantClear( &result_ ); }
  ~ComDispResult() { VariantClear( &result_ ); }
  ComDispResult(const ComDispResult& r) {
    hResult_ = r.hResult_;
    VariantInit(&result_);
    VariantCopyInd(&result_,(VARIANT*)&r.result_);
  }
  const ComDispResult& operator = (const ComDispResult& r) {
    hResult_ = r.hResult_;
    VariantCopyInd(&result_,(VARIANT*)&r.result_);
    return *this;
  }
  bool IsSuccess() { return SUCCEEDED(hResult_); }
  bool HasResult() { return result_.vt != VT_EMPTY; }
  long ToLong() {
    if ( result_.vt == VT_I2 ) return result_.iVal;
    if ( result_.vt == VT_I4 ) return result_.lVal;
    if ( result_.vt == VT_R4 ) return long(result_.fltVal);
    if ( result_.vt == VT_UI2 ) return result_.uiVal;
    if ( result_.vt == VT_UI4 ) return result_.ulVal;
    else {
      VARIANT v;
      VariantInit(&v);
      VariantChangeType(&v,&result_,0,VT_I4);
      return v.lVal;
    }
  }
};

struct ComDispArgs {
  mutable VARIANT args_[16];
  mutable VARIANT* carg_;
  ComDispArgs() : carg_(args_+sizeof(args_)/sizeof(VARIANT)) {}
  const ComDispArgs& operator ,(long val) {
    if ( args_ != carg_ ) {
      --carg_;
      carg_->vt = VT_I4;
      carg_->lVal = val;
    }
    return *this;
  }
  const ComDispArgs& operator ,(float val) const {
    if ( args_ != carg_ ) {
      --carg_;
      carg_->vt = VT_R4;
      carg_->fltVal = val;
    }
    return *this;
  }
  const ComDispArgs& operator ,(IDispatch* pI) const {
    if ( args_ != carg_ ) {
      --carg_;
      carg_->vt = VT_DISPATCH;
      carg_->pdispVal = pI;
    }
    return *this;
  }
  const ComDispArgs& operator ,(IUnknown* pI) const {
    if ( args_ != carg_ ) {
      --carg_;
      carg_->vt = VT_UNKNOWN;
      carg_->punkVal = pI;
    }
    return *this;
  }
  long argsno() const {
    return 16-(carg_-args_);
  }
};

ComDispResult ComDispInvoke(IDispatch* disp,DISPID dispid,const ComDispArgs& args);
ComDispResult ComDispInvoke(IDispatch* disp,const wchar_t* method,const ComDispArgs& args);

class VARIANTEnum : public ComObject<COM_INTERFACES1(IEnumVARIANT)>
{
private:
  BufferT<VARIANT> variants_;
  u32_t iter_;
  void Clear_();
  void Copy_( BufferT<VARIANT>& src);
  void CopyOne_(VARIANT* v,int i) { VariantCopy(v,&variants_[i]); }
public:
  VARIANTEnum(BufferT<VARIANT>& variants,bool copy=false);
  ~VARIANTEnum();

  virtual HRESULT __stdcall Next(u32_t celt,VARIANT* rgvar,u32_t* pceltFetched);
  virtual HRESULT __stdcall Skip(unsigned long celt);
  virtual HRESULT __stdcall Reset();
  virtual HRESULT __stdcall Clone(IEnumVARIANT** ppenum);
};

template<class T> 
struct mcom_factory{
  struct ifactory : public COM_INTERFACES1(IClassFactory) {
    virtual HRESULT __stdcall CreateInstance(IUnknown* pOuter,const IID& riid, void** ppv) {
      rcc_ptr< ComObject<T> > cls ( new ComObject<T> );
      return cls->QueryInterface(riid,ppv);
    }
    virtual HRESULT __stdcall LockServer(BOOL lock) { return S_OK; }
  };
  static HRESULT MakeFactory(const GUID& riid,void** ppv) {
    rcc_ptr<IClassFactory> ifc ( new ComObject<ifactory> );
    return ifc->QueryInterface(riid,ppv);
  }
};

NAMESPACE_MSKIN_END

#endif // __mutil_com_h__
