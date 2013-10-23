
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

#if !defined __SpriteCraft_Impl_RGBFilter_h__
#define  __SpriteCraft_Impl_RGBFilter_h__

HRESULT RBGFilter_NotExists_Error();

template<class Helper>
  struct ColorObjectT :
    TeggoPyDispImpl<IRGBFilter, 0, TeggoISupportErrorInfo>
  {

    ColorObjectT()
      : obj_(0)
      {
      }

    virtual ~ColorObjectT()
      {
      }

    virtual HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid)
      {
        return IsEqualGUID(TEGGO_GUIDOF(IRGBFilter),riid)?S_OK:S_FALSE;
      }

    virtual HRESULT __stdcall get_color(long *pVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        *pVal = mBGRA_t(obj_.Getter()).C();
        return S_OK;
      }

    virtual HRESULT __stdcall put_color(long newVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        obj_.Setter(mRGB_t(*(mbgra_t*)&newVal));
        return S_OK;
      }

    virtual HRESULT __stdcall get_r(long *pVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        mrgb_t c = obj_.Getter();
        *pVal = u32_t(c.R)*100/255;
        return S_OK;
      }

    virtual HRESULT __stdcall put_r(long newVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        mrgb_t c = obj_.Getter();
        u32_t val = teggo_min<u32_t>(u32_t(newVal),100)*255/100;
        c.R = byte_t(val);
        obj_.Setter(c);
        return S_OK;
      }

    virtual HRESULT __stdcall get_g(long *pVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        mrgb_t c = obj_.Getter();
        *pVal = u32_t(c.G)*100/255;
        return S_OK;
      }

    virtual HRESULT __stdcall put_g(long newVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        mrgb_t c = obj_.Getter();
        u32_t val = teggo_min<u32_t>(u32_t(newVal),100)*255/100;
        c.G = byte_t(val);
        obj_.Setter(c);
        return S_OK;
      }

    virtual HRESULT __stdcall get_b(long *pVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        mrgb_t c = obj_.Getter();
        *pVal = u32_t(c.B)*100/255;
        return S_OK;
      }

    virtual HRESULT __stdcall put_b(long newVal)
      {
        _SYNCHRONIZE_;
        if ( !obj_.Exists() ) return RBGFilter_NotExists_Error();
        mrgb_t c = obj_.Getter();
        u32_t val = teggo_min<u32_t>(u32_t(newVal),100)*255/100;
        c.B = byte_t(val);
        obj_.Setter(c);
        return S_OK;
      }

    typename Helper obj_;
  };

struct ColorObjectHelper : rcc_ptr<IColorHelper>
  {
    ColorObjectHelper(IColorHelper* p) : rcc_ptr<IColorHelper>(p) {}
    void    Setter(mrgb_t c) { ref_->IColorHelper_SetColor(c); }
    mrgb_t  Getter() { return ref_->IColorHelper_GetColor(); }
    bool    Exists() { return ref_ != 0; }
  };

struct ColorObject : public TeggoObject< ColorObjectT<ColorObjectHelper> >
  {
    ColorObject(IColorHelper* scf) { obj_.reset_(scf); }
  };

#endif //  __SpriteCraft_Impl_RGBFilter_h__
