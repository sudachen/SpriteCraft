
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

#include "precompile.h"
#include "m_surface.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

template<class T>
inline SCERROR ParseMemOfFont(byte_t* mem,int stride, int width,int height,BufferT<rect_t>& rects,T) {
  int symheight = 0;
  T cpixel = *(T*)mem;
  { for ( int row = 1; row < height; ++row ) {
    if ( *(T*)(mem+row*stride) == cpixel ) {
      symheight = row-1;
      break;
    }
  }}
  //fail_if ( symheight == 0 );
  if ( symheight == 0 ) symheight = height-1;
  { for ( int row = 0; row < height; row+=(symheight+1) ) {
      //if ( *((T*)(mem+row*stride)+1) != cpixel ) continue;
      T* p = (T*)(mem+row*stride);
      int lw = 0;
      for ( int w = 2; w < width; ++w ) {
        if ( p[w]  == cpixel ) {
          rects.Push(rect_t(lw,row+1,w,row+1+symheight));
          lw = w;
          //XMessage(("char %d {%dx%d,%dx%d}",rects.size()-1,
          //  rects.back().left,
          //  rects.back().top,
          //  rects.back().right,
          //  rects.back().bottom));
        }
      }
  }}
  return SCE_OK;
}

SCERROR ParseFontSurface(Surface* surf,BufferT<rect_t>& rects)
{
  fail_if ( surf == 0 );
  unsigned fmtbits = Surface::GET_PIXFORMAT(surf->GetFormat());
  unsigned sheight = surf->GetHeight();
  unsigned swidth  = surf->GetWidth();
  rects.Clear();
  rects.Reserve(128);
  SurfLock lsurf(surf); reterr_if ( lsurf.PeekError() );
  int stride;
  byte_t* mem = (byte_t*) lsurf->GetMemory(stride);
  
  switch( fmtbits ) {
  case Surface::PAL:    
  case Surface::ALPHA:    
    return ParseMemOfFont(mem,stride,swidth,sheight,rects,byte_t(0));
  case Surface::RGB5x5:
    return ParseMemOfFont(mem,stride,swidth,sheight,rects,u16_t(0));
  case Surface::RGBx32:
    return ParseMemOfFont(mem,stride,swidth,sheight,rects,u32_t(0));
  default: fail_if ( "INVALID SURFACE FORMAT" );
  }

  return SCE_OK;
}

NAMESPACE_MSKIN_END

