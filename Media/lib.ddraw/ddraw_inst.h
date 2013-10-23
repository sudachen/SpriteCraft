
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

#ifndef ___a3542dd1_abb4_4bde_bc78_31253e91516e___
#define ___a3542dd1_abb4_4bde_bc78_31253e91516e___

#include "../tffmedia.h"
#include <ddraw.h>

extern IDirectDraw7 *g_tff_dd_instance;

MEDIA_EXPORTABLE IDirectDraw7 * CXX_STDCALL DD_Instance();
MEDIA_EXPORTABLE void CXX_STDCALL DD_FillDdpf(DDPIXELFORMAT *ddpf, u32_t pf);
MEDIA_EXPORTABLE TFFt_PIXEL_FORMAT CXX_STDCALL DD_RecognizeDdpf(DDPIXELFORMAT const *ddpf);
MEDIA_EXPORTABLE TFFt_PIXEL_FORMAT CXX_STDCALL DD_CompatiblePf();

struct MEDIA_EXPORTABLE_DATA TFFt_DCPOOL_ELEMENT
  {
    HDC *dcpool_element_hdc;
    u32_t dcpool_element_marker;
    virtual void _uget_dc(HDC dc) = 0;
    virtual HDC  _aqcu_dc() = 0;
//     HDC operator +()  { return *dcpool_element_hdc; }
    operator bool()   { return !!dcpool_element_hdc; }
    bool operator !() { return !!dcpool_element_hdc; }
    bool operator < (TFFt_DCPOOL_ELEMENT const &a) { return dcpool_element_marker < a.dcpool_element_marker; }
    void Init() { dcpool_element_hdc = 0, dcpool_element_marker = 0; };
    TFFt_DCPOOL_ELEMENT() : dcpool_element_hdc(0), dcpool_element_marker(0) {}
    ~TFFt_DCPOOL_ELEMENT() { REQUIRE(dcpool_element_hdc == 0); }
  };

struct MEDIA_EXPORTABLE_DATA TFFt_DCPOOL
  {
    enum { POOL_SIZE = 32 };
    HDC dc_pool1[POOL_SIZE];
    TFFt_DCPOOL_ELEMENT *dc_pool2[POOL_SIZE];
    u32_t marker;
    TFFt_DCPOOL();
    ~TFFt_DCPOOL();
    HDC PoolDC(TFFt_DCPOOL_ELEMENT *_p);
    void UnpoolDC(TFFt_DCPOOL_ELEMENT *_p);
    void UpdateMarker(TFFt_DCPOOL_ELEMENT *_p);
    void UnpoolAll();
  };

extern MEDIA_EXPORTABLE_DATA TFFt_DCPOOL g_bitmap_dc_pool;

enum
  {
    TFF_BITBLT_OPACITY  = 0x00001,
    TFF_BITBLT_SOLID    = 0x00002,
    TFF_BITBLT_ADDITIVE = 0x00003,
    TFF_BITBLT_AGL0     = 0x00000,
    TFF_BITBLT_AGL90    = 0x01000,
    TFF_BITBLT_AGL180   = 0x02000,
    TFF_BITBLT_AGL270   = 0x03000,
    TFF_BITBLT_AGLMSK   = 0x0f000
  };

struct MEDIA_EXPORTABLE_DATA TFFt_BITMAP : Refcounted, private TFFt_DCPOOL_ELEMENT
  {
    TFFt_PIXEL_FORMAT pixel_format_;
    void *bits_;
    int   stride_;
    int   width_;
    int   height_;
    rcc_ptr<IDirectDrawSurface7> dd_surface;
    WeakLinkHolder<HDC> dc_holder;

    static rcc_ptr<TFFt_BITMAP> CXX_STDCALL CreateFrom(StringParam path, TFFt_PIXEL_FORMAT pf, bool dither=true,u32_t *lms=0);

    TFFt_BITMAP(int width,int height,TFFt_PIXEL_FORMAT pf);
    ~TFFt_BITMAP();

    IDirectDrawSurface7 *AcquireSurface();
    HDC GetDC();
    void UngetDC();
    virtual void _uget_dc(HDC dc);
    virtual HDC  _aqcu_dc();

    void *Bits(int *stride,TFFt_PIXEL_FORMAT *pf);

    HDC GetDC(WeakLink<HDC> *wl) { HDC dc = GetDC(); dc_holder.Link(wl,dc); return dc; }
    int Width() { return width_; }
    int Height() { return height_; }
    TFFt_PIXEL_FORMAT PixelFormat() { return pixel_format_; }
    TFFMt_E LoadRect(StringParam path,rect_t const *from, rect_t const *to, bool dither=true, u32_t *lms=0);
    void BitBlt(TFFt_BITMAP *bsrc,int x,int y,u32_t opt=TFF_BITBLT_OPACITY,int left=0,int top=0,int width=0,int height=0);
  };

MEDIA_EXPORTABLE HCURSOR CXX_STDCALL TffmCreateHcurso32(TFFt_BITMAP *bitmap, int x, int y, int xhsp, int yhsp);

#endif // ___a3542dd1_abb4_4bde_bc78_31253e91516e___
