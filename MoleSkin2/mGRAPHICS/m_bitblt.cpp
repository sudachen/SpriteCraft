
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

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
#include "../mcore/m_fastcopy.h"
#include <math.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

typedef const u16_t   cu16_t;
typedef const byte_t     cbyte_t;

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL || \
    MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WCE
# define SRC_DST_MSKIN_MASK
# define SRC_MSKIN_R_MASK MSKIN_R_MASK_565;
# define SRC_MSKIN_G_MASK MSKIN_G_MASK_565;
# define SRC_MSKIN_B_MASK MSKIN_B_MASK_565;
# define SRC2_MSKIN_R_MASK MSKIN_R_MASK_565;
# define SRC2_MSKIN_G_MASK MSKIN_G_MASK_565;
# define SRC2_MSKIN_B_MASK MSKIN_B_MASK_565;
# define DST_MSKIN_R_MASK MSKIN_R_MASK_565;
# define DST_MSKIN_G_MASK MSKIN_G_MASK_565;
# define DST_MSKIN_B_MASK MSKIN_B_MASK_565;
#endif

static inline 
bool PrepareRect(rect_t& r,
  Surface* to,
  Surface* from,
  int& x, int& y,
  const rect_t* rect)
{
  rect_t to_rect = to->GetRect();
  rect_t from_rect = from->GetRect();

  //if ( x >= to_rect.right || y >= to_rect.bottom ) return false;
  if ( !rect ) r = from->GetRect(); else r = *rect;

  const rect_t r0 = r.Intersection(from_rect).
    Shift(x-r.left,y-r.top).Intersection(to_rect).Shift(-x+r.left,-y+r.top); 
  

//  XMessage(("{%d,%d,%d,%d}:%dx%d->{%d,%d,%d,%d}:%dx%d",
//    r.left,r.top,r.right,r.bottom,x,y,
//    r0.left,r0.top,r0.right,r0.bottom,x+(r0.left-r.left),y+(r0.top-r.top)));

  if ( r0.IsZero() ) return false;

  x += r0.left-r.left;
  y += r0.top-r.top;
  r = r0;

  
  STRICT_REQUIRE ( to_rect.Contains(r.Move(x,y)) );
  STRICT_REQUIRE ( from_rect.Contains(r) );
  return true;
}

SCERROR Surface::BitBlt_(
  Surface* to,
  Surface* from,
  int x, int y,
  const rect_t* rect)
{
  unsigned surfmt = Surface::GET_PIXFORMAT(from->GetFormat());
  fail_if ( surfmt != Surface::GET_PIXFORMAT(to->GetFormat()) );
  rect_t r;
  if ( !PrepareRect(r,to,from,x,y,rect) ) return SCE_OK;
  const int width = r.right-r.left;
  if ( width == 0 ) return SCE_OK;
  SurfLock lfrom(from); reterr_if ( lfrom.PeekError() );
  SurfLock lto(to);     reterr_if ( lto.PeekError() );
  int src_stride,dst_stride;  // получаем адреса памяти для пикселей
  cbyte_t* src  =  (cbyte_t*)lfrom->GetMemory(src_stride);
  byte_t* dst   =  (byte_t*)lto->GetMemory(dst_stride);
  int depth = 0;
  switch ( surfmt ) {
  case Surface::RGBx32: depth = 4; break;
  case Surface::RGB5x5: depth = 2; break;
  case Surface::PAL:    
  case Surface::ALPHA:  depth = 1; break;
  default: fail_if ("INVALID SURFACE FORMAT");
  }
  dst+=y*dst_stride+x*depth;            //
  src+=r.top*src_stride+r.left*depth;   // поправка на координаты
  const int count = depth*width;
  for ( int row = r.top; row < r.bottom;++row,src+=src_stride,dst+=dst_stride)
    memcpy(dst,src,count);
  return SCE_OK;
}

template<class T>
static inline void CopyTransparentRect_T(
  int rows,
  cbyte_t* src,int src_stride,
  byte_t* dst,int dst_stride,
  int width, T trans)
{
  // копируем прямоугольник по строкам
  for ( int row = 0; row < rows; ++row,src+=src_stride,dst+=dst_stride) {
    register const T* s  = (const T*)src;
    register const T* const sE = (const T*)src+width;
    register T* d  = (T*)dst;
    for(;s<sE;++s,++d) { register T x = *s; if ( x != trans ) *d = x; }
  }
}


SCERROR Surface::TransparentBlt_(
  Surface* to,
  Surface* from,
  u32_t trans,
  int x, int y ,
  const rect_t* rect)
{
  rect_t r;
  unsigned surfmt = Surface::GET_PIXFORMAT(from->GetFormat());
  fail_if ( surfmt != Surface::GET_PIXFORMAT(to->GetFormat()) );
  if ( !PrepareRect(r,to,from,x,y,rect) ) return SCE_OK;
  const int width = r.right-r.left;
  if ( width == 0 ) return SCE_OK;
  SurfLock lfrom(from); reterr_if ( lfrom.PeekError() );
  SurfLock lto(to);     reterr_if ( lto.PeekError() );
  int src_stride,dst_stride;  // получаем адреса памяти для пикселей
  cbyte_t* src = (cbyte_t*)lfrom->GetMemory(src_stride);
  byte_t* dst  = (byte_t*)lto->GetMemory(dst_stride);
  int depth = 0;
  switch ( surfmt ) {
  case Surface::RGBx32: depth = 4; break;
  case Surface::RGB5x5: depth = 2; break;
  case Surface::PAL:    
  case Surface::ALPHA:  depth = 1; break;
  default: fail_if ("INVALID SURFACE FORMAT");
  }
  dst+=y*dst_stride+x*depth;            //
  src+=r.top*src_stride+r.left*depth;   // поправка на координаты
  switch ( surfmt ) {
  case Surface::RGBx32: 
    CopyTransparentRect_T(r.bottom-r.top,src,src_stride,dst,dst_stride,width,u32_t(trans));
    break;
  case Surface::RGB5x5: 
    CopyTransparentRect_T(r.bottom-r.top,src,src_stride,dst,dst_stride,width,u16_t(trans));
    break;
  case Surface::PAL:    
  case Surface::ALPHA:  
    CopyTransparentRect_T(r.bottom-r.top,src,src_stride,dst,dst_stride,width,byte_t(trans));
    break;
  default: fail_if ("INVALID SURFACE FORMAT");
  }
  return SCE_OK;
}

SCERROR Surface::AlphaBlt_(
  Surface* to,
  Surface* from,
  Surface* alpha,
  int x , int y,
  const rect_t* rect)
{
  if ( !alpha ) {
    if ( MultiLayerSurface* gs = Adaptate<MultiLayerSurface>(from) ) {
      alpha = gs->GetAlphaSurface();
    }
  }
  retval_if_fail( alpha != 0 , SCE_INVALIDARG );
  retval_if_fail( from != 0 , SCE_INVALIDARG );
  retval_if_fail( to != 0 , SCE_INVALIDARG );
  retval_if_fail( (from->GetFormat()&Surface::FORMATBITS) == Surface::RGB5x5 , SCE_BAD_FORMAT );
  retval_if_fail( (to->GetFormat()&Surface::FORMATBITS) == Surface::RGB5x5 , SCE_BAD_FORMAT );
  retval_if_fail( (alpha->GetFormat()&Surface::FORMATBITS) == Surface::ALPHA , SCE_BAD_FORMAT );
  retval_if_fail( alpha->GetRect() == from->GetRect() , SCE_FAIL );
  rect_t r;
#if !defined SRC_DST_MSKIN_MASK
  u16_t SRC_MSKIN_R_MASK,SRC_MSKIN_G_MASK,SRC_MSKIN_B_MASK;
  u16_t DST_MSKIN_R_MASK,DST_MSKIN_G_MASK,DST_MSKIN_B_MASK;
  if ( from->GetFormat()&Surface::SHORTGREEN ) {
    SRC_MSKIN_R_MASK = MSKIN_R_MASK_555;
    SRC_MSKIN_G_MASK = MSKIN_G_MASK_555;
    SRC_MSKIN_B_MASK = MSKIN_B_MASK_555;
  }else{
    SRC_MSKIN_R_MASK = MSKIN_R_MASK_565;
    SRC_MSKIN_G_MASK = MSKIN_G_MASK_565;
    SRC_MSKIN_B_MASK = MSKIN_B_MASK_565;
  }
  if ( to->GetFormat()&Surface::SHORTGREEN ) {
    DST_MSKIN_R_MASK = MSKIN_R_MASK_555;
    DST_MSKIN_G_MASK = MSKIN_G_MASK_555;
    DST_MSKIN_B_MASK = MSKIN_B_MASK_555;
  }else{
    DST_MSKIN_R_MASK = MSKIN_R_MASK_565;
    DST_MSKIN_G_MASK = MSKIN_G_MASK_565;
    DST_MSKIN_B_MASK = MSKIN_B_MASK_565;
  }
#endif
  if ( !PrepareRect(r,to,from,x,y,rect) ) return SCE_OK;
  SurfLock lfrom(from);   reterr_if ( lfrom.PeekError() );
  SurfLock lalpha(alpha); reterr_if ( lalpha.PeekError() );
  SurfLock lto(to);       reterr_if ( lto.PeekError() );
  int src_stride,dst_stride,alp_stride;  // получаем адреса памяти для пикселей
  cbyte_t* src = (cbyte_t*)lfrom->GetMemory(src_stride);
  byte_t* dst  = (byte_t*)lto->GetMemory(dst_stride);
  cbyte_t* alp = (cbyte_t*)lalpha->GetMemory(alp_stride);
  dst+=y*dst_stride+x*2;      //
  src+=r.top*src_stride+r.left*2;      // поправка на координаты
  alp+=r.top*alp_stride+r.left;      // 
  const int width = r.Width(); // количество точек по горизонтали
  if ( width == 0 ) return SCE_OK;
  // копируем прямоугольник по строкам
  for ( int row = r.top; row < r.bottom; 
        ++row,src+=src_stride,dst+=dst_stride,alp+=alp_stride) {
    for ( int i = 0; i < width; ++i )
    {
      unsigned int a = alp[i];
      if ( a == 255 ) { 
        int j = i+1;
        for (; j < width && alp[j] == 255; ++j ) ;
        FastCopy16((u16_t*)dst+i,(cu16_t*)src+i,j-i);
        i=j-1;
      }
      else if ( a == 0 ) {/*nothing*/}
      else
      {
        
        unsigned color  = ((cu16_t*)src)[i];
        ++a;
        unsigned dcolor = ((u16_t*)dst)[i];
        register signed int reg2 = (signed)(color&SRC_MSKIN_R_MASK); 
        register signed int reg = (signed)(dcolor&DST_MSKIN_R_MASK);
        register unsigned color2 = ((((reg2-reg)*a)>>8)+reg)&DST_MSKIN_R_MASK;
        reg = (signed)(dcolor&DST_MSKIN_G_MASK);
        reg2 = (signed)(color&SRC_MSKIN_G_MASK); 
        color2 |= ((((reg2-reg)*a)>>8)+reg)&DST_MSKIN_G_MASK;
        reg = (signed)(dcolor&DST_MSKIN_B_MASK);
        reg2 = (signed)(color&SRC_MSKIN_B_MASK); 
        color2 |= ((((reg2-reg)*a)>>8)+reg)&DST_MSKIN_B_MASK;
        ((u16_t*)dst)[i] = color2;
      }
    }
  }
  return SCE_OK;
}

SCERROR Surface::AlphaBlt3a_(
  Surface* to,
  int x , int y,
  Surface* back,
  const rect_t* brect,
  Surface* from,
  const rect_t* rect,
  Surface* alpha)
{
  if ( !alpha ) {
    if ( MultiLayerSurface* mls = Adaptate<MultiLayerSurface>(from) ) {
      alpha = mls->GetAlphaSurface();
    }
  }
  retval_if_fail( alpha != 0 , SCE_INVALIDARG );
  retval_if_fail( from != 0 , SCE_INVALIDARG );
  retval_if_fail( back != 0 , SCE_INVALIDARG );
  retval_if_fail( to != 0 , SCE_INVALIDARG );
  retval_if_fail( (from->GetFormat()&Surface::FORMATBITS) == Surface::RGB5x5 , SCE_BAD_FORMAT );
  retval_if_fail( (to->GetFormat()&Surface::FORMATBITS) == Surface::RGB5x5 , SCE_BAD_FORMAT );
  retval_if_fail( (alpha->GetFormat()&Surface::FORMATBITS) == Surface::ALPHA , SCE_BAD_FORMAT );
  retval_if_fail( alpha->GetRect() == from->GetRect() , SCE_FAIL );
  rect_t r;
  rect_t r2 = brect?*brect:back->GetRect();
  if ( !PrepareRect(r,to,from,x,y,rect) ) return SCE_OK;
  retval_if_fail ( r.Width() <= r2.Width() , SCE_INVALIDARG );
#if !defined SRC_DST_MSKIN_MASK
  u16_t SRC_MSKIN_R_MASK,SRC_MSKIN_G_MASK,SRC_MSKIN_B_MASK;
  u16_t SRC2_MSKIN_R_MASK,SRC2_MSKIN_G_MASK,SRC2_MSKIN_B_MASK;
  u16_t DST_MSKIN_R_MASK,DST_MSKIN_G_MASK,DST_MSKIN_B_MASK;
  if ( from->GetFormat()&Surface::SHORTGREEN ) {
    SRC_MSKIN_R_MASK = MSKIN_R_MASK_555;
    SRC_MSKIN_G_MASK = MSKIN_G_MASK_555;
    SRC_MSKIN_B_MASK = MSKIN_B_MASK_555;
  }else{
    SRC_MSKIN_R_MASK = MSKIN_R_MASK_565;
    SRC_MSKIN_G_MASK = MSKIN_G_MASK_565;
    SRC_MSKIN_B_MASK = MSKIN_B_MASK_565;
  }
  if ( back->GetFormat()&Surface::SHORTGREEN ) {
    SRC2_MSKIN_R_MASK = MSKIN_R_MASK_555;
    SRC2_MSKIN_G_MASK = MSKIN_G_MASK_555;
    SRC2_MSKIN_B_MASK = MSKIN_B_MASK_555;
  }else{
    SRC2_MSKIN_R_MASK = MSKIN_R_MASK_565;
    SRC2_MSKIN_G_MASK = MSKIN_G_MASK_565;
    SRC2_MSKIN_B_MASK = MSKIN_B_MASK_565;
  }
  if ( to->GetFormat()&Surface::SHORTGREEN ) {
    DST_MSKIN_R_MASK = MSKIN_R_MASK_555;
    DST_MSKIN_G_MASK = MSKIN_G_MASK_555;
    DST_MSKIN_B_MASK = MSKIN_B_MASK_555;
  }else{
    DST_MSKIN_R_MASK = MSKIN_R_MASK_565;
    DST_MSKIN_G_MASK = MSKIN_G_MASK_565;
    DST_MSKIN_B_MASK = MSKIN_B_MASK_565;
  }
#endif
  SurfLock lfrom(from);   reterr_if ( lfrom.PeekError() );
  SurfLock lback(back);   reterr_if ( lfrom.PeekError() );
  SurfLock lalpha(alpha); reterr_if ( lalpha.PeekError() );
  SurfLock lto(to);       reterr_if ( lto.PeekError() );
  int src_stride,dst_stride,alp_stride,sbk_stride;  // получаем адреса памяти для пикселей
  cbyte_t* src = (cbyte_t*)lfrom->GetMemory(src_stride);
  byte_t* dst  = (byte_t*)lto->GetMemory(dst_stride);
  cbyte_t* sbk  = (cbyte_t*)lback->GetMemory(sbk_stride);
  cbyte_t* alp = (cbyte_t*)lalpha->GetMemory(alp_stride);
  dst+=y*dst_stride+x*2;      //
  src+=r.top*src_stride+r.left*2;      // поправка на координаты
  sbk+=r2.top*sbk_stride+r2.left*2;      // поправка на координаты
  alp+=r.top*alp_stride+r.left;      // 
  const int width = r.Width(); // количество точек по горизонтали
  if ( width == 0 ) return SCE_OK;
  // копируем прямоугольник по строкам
  for ( int row = r.top; row < r.bottom; 
        ++row,src+=src_stride,dst+=dst_stride,alp+=alp_stride,sbk+=sbk_stride) {
    for ( int i = 0; i < width; ++i )
    {
      unsigned int a = alp[i];
      if ( a == 255 ) { 
        int j = i+1;
        for (; j < width && alp[j] == 255; ++j ) ;
        FastCopy16((u16_t*)dst+i,(cu16_t*)src+i,j-i);
        i=j-1;
      }
      else if ( a == 0 ) 
      {
        int j = i+1;
        for (; j < width && alp[j] == 0; ++j ) ;
        FastCopy16((u16_t*)dst+i,(cu16_t*)sbk+i,j-i);
        i=j-1;
      }
      else
      {
        unsigned color  = ((cu16_t*)src)[i];
        ++a;
        unsigned dcolor = ((u16_t*)sbk)[i];
        register signed int reg2 = (signed)(color&SRC_MSKIN_R_MASK); 
        register signed int reg = (signed)(dcolor&SRC2_MSKIN_R_MASK);
        register unsigned color2 = ((((reg2-reg)*a)>>8)+reg)&DST_MSKIN_R_MASK;
        reg = (signed)(dcolor&SRC2_MSKIN_G_MASK);
        reg2 = (signed)(color&SRC_MSKIN_G_MASK); 
        color2 |= ((((reg2-reg)*a)>>8)+reg)&DST_MSKIN_G_MASK;
        reg = (signed)(dcolor&SRC2_MSKIN_B_MASK);
        reg2 = (signed)(color&SRC_MSKIN_B_MASK); 
        color2 |= ((((reg2-reg)*a)>>8)+reg)&DST_MSKIN_B_MASK;
        ((u16_t*)dst)[i] = color2;
      }
    }
  }
  return SCE_OK;
}

SCERROR Surface::AlphaValBlt_(
  Surface* to,
  Surface* from,
  unsigned alpha,
  int x , int y,
  const rect_t* rect)
{
  retval_if_fail( (from->GetFormat()&Surface::FORMATBITS) == Surface::RGB5x5 , SCE_BAD_FORMAT );
  retval_if_fail( (to->GetFormat()&Surface::FORMATBITS) == Surface::RGB5x5 , SCE_BAD_FORMAT );
  retval_if_fail( alpha < 256 , SCE_FAIL ); 
  rect_t r;
  alpha = (alpha&0x0ff)+1;
  if ( !PrepareRect(r,to,from,x,y,rect) ) return SCE_OK;
#if !defined SRC_DST_MSKIN_MASK
  u16_t SRC_MSKIN_R_MASK,SRC_MSKIN_G_MASK,SRC_MSKIN_B_MASK;
  u16_t DST_MSKIN_R_MASK,DST_MSKIN_G_MASK,DST_MSKIN_B_MASK;
  if ( from->GetFormat()&Surface::SHORTGREEN ) {
    SRC_MSKIN_R_MASK = MSKIN_R_MASK_555;
    SRC_MSKIN_G_MASK = MSKIN_G_MASK_555;
    SRC_MSKIN_B_MASK = MSKIN_B_MASK_555;
  }else{
    SRC_MSKIN_R_MASK = MSKIN_R_MASK_565;
    SRC_MSKIN_G_MASK = MSKIN_G_MASK_565;
    SRC_MSKIN_B_MASK = MSKIN_B_MASK_565;
  }
  if ( to->GetFormat()&Surface::SHORTGREEN ) {
    DST_MSKIN_R_MASK = MSKIN_R_MASK_555;
    DST_MSKIN_G_MASK = MSKIN_G_MASK_555;
    DST_MSKIN_B_MASK = MSKIN_B_MASK_555;
  }else{
    DST_MSKIN_R_MASK = MSKIN_R_MASK_565;
    DST_MSKIN_G_MASK = MSKIN_G_MASK_565;
    DST_MSKIN_B_MASK = MSKIN_B_MASK_565;
  }
#endif
  SurfLock lfrom(from);   reterr_if ( lfrom.PeekError() );
  SurfLock lto(to);       reterr_if ( lto.PeekError() );
  int src_stride,dst_stride;  // получаем адреса памяти для пикселей
  cbyte_t* src = (cbyte_t*)lfrom->GetMemory(src_stride);
  byte_t* dst  = (byte_t*)lto->GetMemory(dst_stride);
  dst+=y*dst_stride+x*2;      //
  src+=r.top*src_stride+r.left*2;      // поправка на координаты
  const int width = r.right-r.left; // количество точек по горизонтали
  if ( width == 0 ) return SCE_OK;
  // копируем прямоугольник по строкам
  for ( int row = r.top; row < r.bottom; 
        ++row,src+=src_stride,dst+=dst_stride) {
    for ( int i = 0; i < width; ++i )
    {
      {
        unsigned color  = ((cu16_t*)src)[i];
        unsigned dcolor = ((u16_t*)dst)[i];
        register signed int reg2 = (signed)(color&SRC_MSKIN_R_MASK); 
        register signed int reg = (signed)(dcolor&DST_MSKIN_R_MASK);
        register unsigned color2 = ((((reg2-reg)*alpha)>>8)+reg)&DST_MSKIN_R_MASK;
        reg = (signed)(dcolor&DST_MSKIN_G_MASK);
        reg2 = (signed)(color&SRC_MSKIN_G_MASK); 
        color2 |= ((((reg2-reg)*alpha)>>8)+reg)&DST_MSKIN_G_MASK;
        reg = (signed)(dcolor&DST_MSKIN_B_MASK);
        reg2 = (signed)(color&SRC_MSKIN_B_MASK); 
        color2 |= ((((reg2-reg)*alpha)>>8)+reg)&DST_MSKIN_B_MASK;
        ((u16_t*)dst)[i] = color2;
      }
    }
  }
  return SCE_OK;
}

SCERROR Surface::StretchBlt_(
  Surface* to,const rect_t& rout,
  Surface* from,const rect_t& rin)
{
  //static const u32_t MAXCF_SHIFT = 10;
  //static const u32_t MAXCF = 1<<MAXCF_SHIFT;
  cbyte_t* datain;
  byte_t* dataout;
  int stridein;
  int strideout;
  retval_if_fail( (from->GetFormat()&Surface::FORMATBITS) == Surface::RGBx32 , SCE_BAD_FORMAT );
  retval_if_fail( (to->GetFormat()&Surface::FORMATBITS) == Surface::RGBx32 , SCE_BAD_FORMAT );
  {
    rect_t r = to->GetRect();
    if ( rout.right > r.right || rout.left < r.left ||
         rout.bottom > r.bottom || rout.top < r.top )
         fail_because("result image must be in target surface area");
  }
  {
    rect_t r = from->GetRect();
    if ( rin.right > r.right || rin.left < r.left ||
         rin.bottom > r.bottom || rin.top < r.top )
         fail_because("source image must be in source surface area");
  }
  const u32_t widthin = rin.Width();
  const u32_t heightin = rin.Height();
  const u32_t widthout = rout.Width();
  const u32_t heightout = rout.Height();
  SurfLock lfrom(from);   reterr_if ( lfrom.PeekError() );
  SurfLock lto(to);       reterr_if ( lto.PeekError() );
  datain  = (cbyte_t*)lfrom->GetMemory(stridein);
  datain += rin.top*stridein + rin.left;
  dataout = (byte_t*)lto->GetMemory(strideout);
  dataout += rout.top*strideout + rout.left;
  for ( u32_t l=0; l < heightout; ++l ) {
    for ( u32_t j=0; j < widthout; ++j ) {
      ((u32_t*)(dataout+strideout*l))[j] =
        ((u32_t*)(datain+stridein*(l*heightin/heightout)))[j*widthin/widthout];
    }
  }
  return SCE_OK;
}


NAMESPACE_MSKIN_END

