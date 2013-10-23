
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

HCURSOR CXX_STDCALL TffmCreateHcurso32(TFFt_BITMAP *bitmap, int x, int y, int xhsp, int yhsp)
  {

    if ( x < 0 || x + 32 > bitmap->Width() 
      || y < 0 || y + 32 > bitmap->Height() )
      {
        _E.Fail(0,"failed to create cursor, bad bitmap region");
        return 0;
      }

    ICONINFO iconinfo = {0};
    HDC mdc     = ::GetDC(0);
    HDC dc_and  = ::CreateCompatibleDC(mdc);
    HDC dc_xor  = ::CreateCompatibleDC(mdc);
    iconinfo.hbmMask  = (HBITMAP)::CreateCompatibleBitmap(mdc,32,32);
    iconinfo.hbmColor = (HBITMAP)::CreateCompatibleBitmap(mdc,32,32);
    iconinfo.hbmMask  = (HBITMAP)::SelectObject(dc_and,iconinfo.hbmMask);
    iconinfo.hbmColor = (HBITMAP)::SelectObject(dc_xor,iconinfo.hbmColor);

    int stride;
    TFFt_PIXEL_FORMAT pf;
    byte_t *bits = (byte_t*)bitmap->Bits(&stride,&pf);
    TFFt_CopyRowXtoX x2x = TffmCopyRowXtoX(pf,TFF_PF_RGBA8);

    for ( int i = 0; i < 32; ++i )
      {
        rgba8_t c[32];
        x2x((bits+(stride*(i+y)+(pf&TFF_PF_BPPMASK)/8*x)), c, 32);
        for ( int j = 0; j < 32; ++j )
          {
            if ( c[j].A() )
              {
                ::SetPixel(dc_and,j,i,RGB(0,0,0));
                ::SetPixel(dc_xor,j,i,RGB(c[j].r,c[j].g,c[j].b));
              }
            else
              {
                ::SetPixel(dc_and,j,i,RGB(255,255,255));
                ::SetPixel(dc_xor,j,i,RGB(0,0,0));
              }
          }
      }
   
    iconinfo.hbmMask  = (HBITMAP)::SelectObject(dc_and,iconinfo.hbmMask);
    iconinfo.hbmColor = (HBITMAP)::SelectObject(dc_xor,iconinfo.hbmColor);

    iconinfo.xHotspot = xhsp;
    iconinfo.yHotspot = yhsp;
    iconinfo.fIcon = FALSE;

    HCURSOR hc = CreateIconIndirect(&iconinfo);
    if ( !hc )
      {
        _E.Fail(0,_S*"failed to create cursor, %?" %teggo::SysFormatError());
        //Lerr << _S*"%?%?" %MEDIA_ERROR %_E.reason;
      }

    DeleteObject(iconinfo.hbmColor);
    DeleteObject(iconinfo.hbmMask);
    DeleteDC(dc_and);    
    DeleteDC(dc_xor);    
    ReleaseDC(0,mdc);    

    return hc;
  }

