
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

#include "../tffmedia.h"
#include <math.h>

#pragma code_seg(".text$tffmedia")
#include "./ddraw_inst.h"

TFFt_BITMAP::TFFt_BITMAP(int width,int height,TFFt_PIXEL_FORMAT pf)
  {
    if (!pf) pf = DD_CompatiblePf();
    width_ = width;
    height_ = height;
    pixel_format_ = pf;
    int bytes_per_pixel = (pf&TFF_PF_BPPMASK)/8;
    stride_ = (width_ * bytes_per_pixel + 3) & ~3;
    bits_ = VirtualAlloc(0,(stride_*height_ + 4095) & ~4095,MEM_COMMIT,PAGE_READWRITE);
    if ( !bits_ )
      {
        static wchar_t const e[] = L"failed to allocate bitmap storage";
        Lerr << _S*"%?%?" %MEDIA_ERROR %e;
        EMERGENCY(e);
      }
  }

TFFt_BITMAP::~TFFt_BITMAP()
  {
    Unrefe(dd_surface);
    UngetDC();
    if (bits_)
      VirtualFree(bits_,0,MEM_RELEASE);
  }

void *TFFt_BITMAP::Bits(int *stride,TFFt_PIXEL_FORMAT *pf)
  {
    *stride = stride_;
    *pf = pixel_format_;
    return bits_;
  }

IDirectDrawSurface7 *TFFt_BITMAP::AcquireSurface()
  {
    if ( !dd_surface )
      {
        IDirectDraw7 *ddraw = DD_Instance();
        REQUIRE( ddraw != 0 );
        DDSURFACEDESC2 ddsc2 = {0};
        ddsc2.dwSize = sizeof(ddsc2);
        ddsc2.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH
                      | DDSD_PIXELFORMAT | DDSD_LPSURFACE | DDSD_PITCH;
        ddsc2.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        ddsc2.ddsCaps.dwCaps2 = 0;
        ddsc2.dwWidth   = width_;
        ddsc2.dwHeight  = height_;
        ddsc2.lpSurface = bits_;
        ddsc2.lPitch    = stride_;

        DD_FillDdpf(&ddsc2.ddpfPixelFormat,pixel_format_);

        if ( HRESULT hr = DD_Instance()->CreateSurface(&ddsc2,&+dd_surface,0) )
          Lerr << _S*"%?failed(0x%08x) to create sysmem surface" %MEDIA_ERROR %hr;
      }
    REQUIRE( +dd_surface != 0 );
    return +dd_surface;
  }

HDC TFFt_BITMAP::GetDC()
  {
    HDC dc = g_bitmap_dc_pool.PoolDC(this);
    return dc;
  }

void TFFt_BITMAP::UngetDC()
  {
    g_bitmap_dc_pool.UnpoolDC(this);
  }

void TFFt_BITMAP::_uget_dc(HDC dc)
  {
    dd_surface->ReleaseDC(dc);
  }

HDC TFFt_BITMAP::_aqcu_dc()
  {
    HDC dc;
    REQUIRE( SUCCEEDED( AcquireSurface()->GetDC(&dc) ) );
    return dc;
  }

rcc_ptr<TFFt_BITMAP> CXX_STDCALL TFFt_BITMAP::CreateFrom(StringParam path, TFFt_PIXEL_FORMAT pf, bool dither, u32_t *lms)
  {
    u32_t start_at = GetMillis();
    if (!pf) pf = DD_CompatiblePf();
    if ( mem_ptr<TFFt_PICTURE> pic = OpenPicture(path) )
      {
        TFFt_PICTURE::INFO info;
        if ( TFFM_OK == pic->GetInfo(&info) )
          {
            bool error = false;
            int iS, iE, iI;

            rcc_ptr<TFFt_BITMAP> bmp = RccPtr(new TFFt_BITMAP(info.width,info.height,pf));

            if ( info.updown )
              {
                iS = 0;
                iE = info.height;
                iI = 1;
              }
            else
              {
                iS = info.height-1;
                iE = -1;
                iI = -1;
              }

            int bmp_stride;
            TFFt_PIXEL_FORMAT bmp_pf;
            void *p = bmp->Bits(&bmp_stride,&bmp_pf);

            if ( bmp_pf != TFF_PF_RGBA8 )
              {
                if ( bmp_pf > 16 ) dither = false;
                mem_arr<rgba8_t> row = MemArr(new rgba8_t[info.width]);
                TFFt_CopyRowXtoX x2x = TffmCopyRowXtoX(TFF_PF_RGBA8,pf);
                mem_arr<int> err;
                if ( dither )
                  {
                    err = MemArr( new int[3*(info.width+2)] );
                    memset( +err, 0, 3*(info.width+2)*sizeof(int) );
                  }
                for ( int i = iS; i != iE; i+=iI, (byte_t*&)p += bmp_stride )
                  if ( pic->NextRow8(+row,0,info.width) )
                    { error = true; break; }
                  else
                    {
                      if ( dither )
                        TffmDiffusionColorReduction12( +row, +err, info.width, i&1 == 0  );
                      x2x( +row, p, info.width);
                    }
              }
            else
              for ( int i = iS; i != iE; i+=iI, (byte_t*&)p += bmp_stride )
                if ( pic->NextRow8((rgba8_t*&)p,0,info.width) )
                  { error = true; break; }

            if (error)
              return
                _E.Fail(0,"failed to initialize bitmap storage"),
                rcc_ptr<TFFt_BITMAP>(0);

            if (lms)
              *lms = GetMillis()-start_at;

            return bmp;
          }
        return
          _E.Fail(0,_S*"failed to get picture info '%?'" %path),
          rcc_ptr<TFFt_BITMAP>(0);
      }
    return
      _E.Fail(0,_S*"failed to open picture '%?'" %path),
      rcc_ptr<TFFt_BITMAP>(0);
  }

TFFMt_E TFFt_BITMAP::LoadRect(StringParam path,rect_t const *from, rect_t const *to, bool dither, u32_t *lms)
  {
    u32_t start_at = GetMillis();
    if ( mem_ptr<TFFt_PICTURE> pic = OpenPicture(path) )
      {
        TFFt_PICTURE::INFO info;
        if ( TFFM_OK == pic->GetInfo(&info) )
          {
            if (lms)
              *lms = GetMillis()-start_at;
            return TFFM_OK;
          }
        return
          _E.Fail(TFFM_FAIL,_S*"failed to get picture info '%?'" %path);
      }
    return
      _E.Fail(TFFM_NOFILE,_S*"failed to open picture '%?'" %path);
  }

typedef void (CXX_STDCALL *TFFt_BlendRowXtoX)(void const *src, int w, void *dst, u32_t opt, int ddx, void const *_Sdst, void const *_Edst);
extern "C" TFFt_BlendRowXtoX CXX_STDCALL TffmBlendRowXtoX( int src_pf, int dst_pf );

template < class tScrPxl, class tDstPxl >
  struct TFFt_BlendRowXX
    {
      static void _(tScrPxl *src, int w, tDstPxl *dst, u32_t opt, int ddx, tDstPxl *_Sdst, tDstPxl *_Edst)
        {
          int r,g,b,a;
          for( tScrPxl * const srcE = src + w; src != srcE; ++src )
            {
              if ( dst < _Edst && dst >= _Sdst )
                {
                  a = src->A();
                  switch ( opt )
                    {
                      default:
                      case TFF_BITBLT_OPACITY:
                        {
                          int a1 = 255-a;
                          r = (int(dst->r) * a1 + int(src->r) * a) >> 8;
                          g = (int(dst->g) * a1 + int(src->g) * a) >> 8;
                          b = (int(dst->b) * a1 + int(src->b) * a) >> 8;
                          *dst = tDstPxl(r,g,b,255);
                          break;
                        }
                      case TFF_BITBLT_ADDITIVE:
                        {
                          r = int(dst->r) + (int(src->r) * a >> 8); if ( r >= 0x100 ) r = 0xff;
                          g = int(dst->g) + (int(src->g) * a >> 8); if ( g >= 0x100 ) g = 0xff;
                          b = int(dst->b) + (int(src->b) * a >> 8); if ( b >= 0x100 ) b = 0xff;
                          *dst = tDstPxl(r,g,b,255);
                          break;
                        }
                      case TFF_BITBLT_SOLID:
                        {
                          *dst = *src;
                          break;
                        }
                    }
                }
              (byte_t*&)dst += ddx;
            }
        }
    };

template < class tScrPxl >
  struct TFFt_BlendRowXX_
    {
      static inline TFFt_BlendRowXtoX _(int dest_pf)
        {
          switch( dest_pf )
            {
              case TFF_PF_RGB5A1:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,rgb5a1_t>::_;
              case TFF_PF_R5G6B5:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,r5g6b5_t>::_;
              case TFF_PF_RGB5:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,rgb5_t>::_;
              case TFF_PF_RGB8:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,rgb8_t>::_;
              case TFF_PF_RGBA8:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,rgba8_t>::_;
              case TFF_PF_RGBA4:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,rgba4_t>::_;
              case TFF_PF_RGB8 | TFF_PF_BGR:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,bgr8_t>::_;
              case TFF_PF_RGBA8 | TFF_PF_BGR:
                return (TFFt_BlendRowXtoX)&TFFt_BlendRowXX<tScrPxl,bgra8_t>::_;
              default:
                EMERGENCY(L"unsupported pixel format");
                return 0;
            }
        }
    };

extern "C" TFFt_BlendRowXtoX CXX_STDCALL TffmBlendRowXtoX(int src_pf, int dst_pf)
  {
    switch( src_pf )
      {
        case TFF_PF_RGB5A1:
          return TFFt_BlendRowXX_<rgb5a1_t>::_(dst_pf);
        case TFF_PF_RGBA8:
          return TFFt_BlendRowXX_<rgba8_t>::_(dst_pf);
        case TFF_PF_RGBA4:
          return TFFt_BlendRowXX_<rgba4_t>::_(dst_pf);
        case TFF_PF_RGBA8 | TFF_PF_BGR:
          return TFFt_BlendRowXX_<bgra8_t>::_(dst_pf);
        default:
          EMERGENCY(L"unsupported pixel format");
          return 0;
      }
  }


void TFFt_BITMAP::BitBlt(TFFt_BITMAP *bsrc,int x,int y,u32_t opt,int left,int top,int width,int height)
  {
    int src_stride;
    TFFt_PIXEL_FORMAT src_pf;
    byte_t * src = (byte_t*)bsrc->Bits(&src_stride,&src_pf);
    TFFt_BlendRowXtoX f_blend = TffmBlendRowXtoX(src_pf,this->pixel_format_);

    int dx = 1, dy = 0, lx = 0, ly = 1;

    switch ( opt & TFF_BITBLT_AGLMSK )
      {
        case TFF_BITBLT_AGL0:
          if ( x >= this->width_ || y >= this->height_ ) return;
          if ( x + width > this->width_ ) width = this->width_ - x;
          if ( y + height > this->height_ ) width = this->height_ - y;
          if ( x < 0 ) x = 0, width += x, left += x;
          if ( y < 0 ) y = 0, height += y, top += y;
          break;
        case TFF_BITBLT_AGL90:
          dx = -0, lx = -1, dy = 1, ly = 0;
          break;
        case TFF_BITBLT_AGL180:
          dx = -1, lx = 0, dy = 0, ly = -1;
          break;
        case TFF_BITBLT_AGL270:
          dx = 0, lx = 1, dy = -1, ly = 0;
          break;
      }

    void *_Sdst = this->bits_;
    void *_Edst = (byte_t*)this->bits_ + this->stride_ * this->height_;

    int const dst_bpp = (this->pixel_format_ & 0x0ff)/8;
    int ddx = (dx*dst_bpp+dy*this->stride_);
    int const src_bpp = (src_pf & 0x0ff)/8;
    int sdx = (ly * this->stride_ + lx*src_bpp);

    byte_t *dst = (byte_t*)this->bits_;

    src += left * src_bpp + top * src_stride;
    dst += x * dst_bpp + y * this->stride_;

    if ( width <= 0 || height <= 0 ) return;

    //printf("%d,%d,%d,%d,%d\n",opt,width,height,ddx,sdx);

    for ( int i = 0; i < height; ++i )
      {
        f_blend(src,width,dst,opt&0x0f,ddx,_Sdst,_Edst);
        src += src_stride;
        dst += sdx;
      }
  }
