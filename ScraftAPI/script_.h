
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

#define __SCRAFT_ENABLE_SCRIPTING__ 1

/* --------------------------------------------------------------------------------------------- */
struct ScraftScript : ScraftObject<>
{
  struct SubShare; friend struct SubShare;
  struct SubShare : ScraftObject<>
  {
    struct Context_ { DISPPARAMS* basic_params_; };
    template <unsigned argcount> struct Context : Context_
    {
      VARIANT     args_[argcount>0?argcount:1];
      DISPPARAMS  params_;
      Context& Bind(long no,long val) {
        return args_[argcount-no-1].vt = VT_I4,
          args_[argcount-no-1].lVal = val, *this; }
      Context& Bind(long no,float val) {
        return args_[argcount-no-1].vt = VT_R4,
          args_[argcount-no-1].fltVal = val, *this; }
      Context& Bind(long no,pwide_t val) {
        return args_[argcount-no-1].vt = VT_BSTR,
          args_[argcount-no-1].bstrVal = SysAllocString(val), *this; }
      Context& Bind(long no,IDispatch* val) {
        return args_[argcount-no-1].vt = VT_DISPATCH,
          args_[argcount-no-1].pdispVal = Refe(val), *this; }
      Context& Bind(long no,scraft_fake_t) { return *this; }
      void Invoke(const scraft_ptr<SubShare>& sub) { sub->Invoke(*this); }
      Context() {
        memset(args_,0,sizeof(args_));
        params_.cArgs = argcount;
        params_.cNamedArgs = 0;
        params_.rgdispidNamedArgs = 0;
        params_.rgvarg = args_;
        basic_params_ = &params_;
      }
      ~Context() { for ( int i = 0; i < argcount; ++i ) VariantClear(args_+i); }
      template<class T> Context& Bind(long no,const scraft_ptr<T>& p) {
        return Bind(no,~com_cast<IDispatch>(scraft_query_ifs(~p))); }
    };
    SubShare(const scraft_ptr<ScraftScript>& script, pwide_t subroutine)
      : script_(script), subroutine_(subroutine), dispid_(0), bound_(false) {}
    template<class A> void Bind(long no,const A&);
    void Invoke(Context_& ctx);
  private:
    scraft_ptr<ScraftScript> script_;
    SCRFSTR subroutine_;
    long dispid_;
    bool bound_;
  };
  struct Subroutine
  {
    operator bool () { return !!sub_; }
    bool operator!() { return !sub_; }
  protected:
    scraft_ptr<SubShare> sub_;
    friend struct ScraftScript;
  };
  void AddCode(pwide_t codepart);
  void AddUniqueSub(Subroutine& sub,pwide_t code,pwide_t arglist=0,pwide_t file=0,long lineno=0);
  void AddUniqueSub(Subroutine& sub,pwide_t attr,const IXMLPtr& def,pwide_t arglist=0);
  void CompileAndRun();
  void BindSub(Subroutine& sub,pwide_t subroutine);
  bool HasSub(pwide_t);
  ScraftScript();
  ~ScraftScript();
private:
  BufferT<wchar_t> text_;
  IDispatchPtr  script_;
};

/* --------------------------------------------------------------------------------------------- */
template<
  class A0 = scraft_fake_t,
  class A1 = scraft_fake_t,
  class A2 = scraft_fake_t,
  class A3 = scraft_fake_t,
  class A4 = scraft_fake_t>
struct ScraftSub : ScraftScript::Subroutine {
  void operator() () { ScraftScript::SubShare::Context<0>().Invoke(sub_);}
  void operator() (const A0& a0) { ScraftScript::SubShare::Context<1>().Bind(0,a0).Invoke(sub_);  }
  void operator() (const A0& a0,const A1& a1) {
    ScraftScript::SubShare::Context<2> context;
    c.Bind(0,a0).Bind(1,a1).Invoke(sub_);  }
  void operator() (const A0& a0,const A1& a1,const A2& a2) {
    ScraftScript::SubShare::Context<3> c;
    c.Bind(0,a0).Bind(1,a1).Bind(2,a2).Invoke(sub_); }
  void operator() (const A0& a0,const A1& a1,const A2& a2,const A3& a3) {
    ScraftScript::SubShare::Context<4> c;
    c.Bind(0,a0).Bind(1,a1).Bind(2,a2).Bind(3,a3).Invoke(sub_); }
  void operator() (const A0& a0,const A1& a1,const A2& a2,const A3& a3,const A4& a4) {
    ScraftScript::SubShare::Context<5> c;
    c.Bind(0,a0).Bind(1,a1).Bind(2,a2).Bind(3,a3).Bind(4,a4).Invoke(sub_); }
};
