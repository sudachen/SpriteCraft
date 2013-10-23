
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

#pragma code_seg(".text$tffmedia")
#include "./ddraw_inst.h"

#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dxguid.lib")

IDirectDraw7 *g_tff_dd_instance = 0;
TFFt_PIXEL_FORMAT g_tff_gdi_pixel_format = TFFt_PIXEL_FORMAT(0);

IDirectDraw7 * CXX_STDCALL DD_Instance()
  {
    if ( g_tff_dd_instance )
      return g_tff_dd_instance;
    else
      {
        if ( FAILED(DirectDrawCreateEx(0,(void**)&g_tff_dd_instance,IID_IDirectDraw7,0)) )
          return
            Lout << _S*"%?failed to initialize DirectDraw.7" %MEDIA_ERROR,
            0;
        g_tff_dd_instance->SetCooperativeLevel(0,DDSCL_NORMAL);
        return g_tff_dd_instance;
      }
  }

TFFt_PIXEL_FORMAT CXX_STDCALL DD_CompatiblePf()
  {
    if ( !g_tff_gdi_pixel_format )
      {
        rcc_ptr<IDirectDrawSurface7> gdisurf;
        if ( HRESULT hr = DD_Instance()->GetGDISurface(&+gdisurf) )
          {
            //Lerr << _S*"%?failed(%08x) to get DirectDraw.7 GDI surface" %MEDIA_ERROR %hr;
            DDSURFACEDESC2 ddsd2 = {0};
            ddsd2.dwSize = sizeof(ddsd2);
            ddsd2.dwFlags = DDSD_PIXELFORMAT;
            if ( HRESULT hr = DD_Instance()->GetDisplayMode(&ddsd2) )
              Lerr << _S*"%?failed(%08x) to get DirectDraw.7 screen pixel format" %MEDIA_ERROR %hr;
            else
              g_tff_gdi_pixel_format = DD_RecognizeDdpf(&ddsd2.ddpfPixelFormat);
          }
        else
          {
            DDPIXELFORMAT ddpf = {0};
            ddpf.dwSize = sizeof(ddpf);
            gdisurf->GetPixelFormat(&ddpf);
            g_tff_gdi_pixel_format = DD_RecognizeDdpf(&ddpf);
          }
      }
    if ( !g_tff_gdi_pixel_format )
      {
        Lerr << _S*"%?GDI compatible pixel format unrecognized, using RGBA8" %MEDIA_ERROR;
        g_tff_gdi_pixel_format = TFF_PF_RGBA8;
      }
    return g_tff_gdi_pixel_format;
  }

TFFt_PIXEL_FORMAT CXX_STDCALL DD_RecognizeDdpf(DDPIXELFORMAT const *ddpf)
  {
    if ( !ddpf->dwFourCC )
      {
        if ( ddpf->dwRGBBitCount == 32 )
          return TFF_PF_RGBA8;
        else if ( ddpf->dwRGBBitCount == 24 )
          return TFF_PF_RGB8;
        else if ( ddpf->dwRGBBitCount == 16 )
          if ( ddpf->dwFlags & DDPF_ALPHAPIXELS )
            {
              if ( ddpf->dwRGBAlphaBitMask == 0x0000f000 )
                return TFF_PF_RGBA4;
              else if ( ddpf->dwRGBAlphaBitMask == 0x00008000 )
                return TFF_PF_RGB5A1;
            }
          else
            {
              if ( ddpf->dwGBitMask == 0x000003e0 )
                return TFF_PF_RGB5;
              else
                return TFF_PF_R5G6B5;
            }
      }
    return TFFt_PIXEL_FORMAT(0);
  }

void CXX_STDCALL DD_FillDdpf(DDPIXELFORMAT *ddpf, u32_t pf)
  {
    ddpf->dwSize = sizeof(DDPIXELFORMAT);
    ddpf->dwFourCC = 0;
    if ( !pf ) pf = DD_CompatiblePf();
    if ( pf == TFF_PF_RGBA8 )
      {
        ddpf->dwFlags = DDPF_ALPHAPIXELS|DDPF_RGB;
        ddpf->dwRGBBitCount = 32;
        ddpf->dwBBitMask        = 0x000000ff;
        ddpf->dwGBitMask        = 0x0000ff00;
        ddpf->dwRBitMask        = 0x00ff0000;
        ddpf->dwRGBAlphaBitMask = 0xff000000;
      }
    else if ( pf == TFF_PF_RGBA4 )
      {
        ddpf->dwFlags = DDPF_ALPHAPIXELS|DDPF_RGB;
        ddpf->dwRGBBitCount = 16;
        ddpf->dwBBitMask        = 0x0000000f;
        ddpf->dwGBitMask        = 0x000000f0;
        ddpf->dwRBitMask        = 0x00000f00;
        ddpf->dwRGBAlphaBitMask = 0x0000f000;
      }
    else if ( pf == TFF_PF_RGB8 )
      {
        ddpf->dwFlags = DDPF_RGB;
        ddpf->dwRGBBitCount = 24;
        ddpf->dwBBitMask        = 0x000000ff;
        ddpf->dwGBitMask        = 0x0000ff00;
        ddpf->dwRBitMask        = 0x00ff0000;
        ddpf->dwRGBAlphaBitMask = 0x00000000;
      }
    else if ( pf == TFF_PF_RGB5 )
      {
        ddpf->dwFlags = DDPF_RGB;
        ddpf->dwRGBBitCount = 16;
        ddpf->dwBBitMask        = 0x0000001f;
        ddpf->dwGBitMask        = 0x000003e0;
        ddpf->dwRBitMask        = 0x00007c00;
        ddpf->dwRGBAlphaBitMask = 0x00000000;
      }
    else if ( pf == TFF_PF_R5G6B5 )
      {
        ddpf->dwFlags = DDPF_RGB;
        ddpf->dwRGBBitCount = 16;
        ddpf->dwBBitMask        = 0x0000001f;
        ddpf->dwGBitMask        = 0x000007e0;
        ddpf->dwRBitMask        = 0x0000f800;
        ddpf->dwRGBAlphaBitMask = 0x00000000;
      }
    else if ( pf == TFF_PF_RGB5A1 )
      {
        ddpf->dwFlags = DDPF_ALPHAPIXELS|DDPF_RGB;
        ddpf->dwRGBBitCount = 16;
        ddpf->dwBBitMask        = 0x0000001f;
        ddpf->dwGBitMask        = 0x000003e0;
        ddpf->dwRBitMask        = 0x00007c00;
        ddpf->dwRGBAlphaBitMask = 0x00008000;
      }
    else
      EMERGENCY(L"invalid pixel format");
  }
