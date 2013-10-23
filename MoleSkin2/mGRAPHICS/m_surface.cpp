
/*

Copyright Â© 2003-2013, AlexÃ©y SudachÃ©n, alexey@sudachen.name

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
#include "../../SpriteCraft.SRC/logout.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

pchar_t const Surface::TypeID__ = "#TYPEID#mskin::Surface";
pchar_t const TffSurface::TypeID__ = "#TYPEID#mskin::TffSurface";
pchar_t const GenericSurface::TypeID__ = "#TYPEID#mskin::GenericSurface";
pchar_t const SurfaceRestorer::TypeID__ = "#TYPEID#mskin::SurfaceRestorer";
pchar_t const MultiLayerSurface::TypeID__ = "#TYPEID#mskin::MultiLayerSurface";

void* Surface::Adaptate_(const void* t) { 
  if ( t == TypeID__) return (void*)this; 
  else return IRefObject::Adaptate_(t); 
}
void* BasicSurface::Adaptate_(const void* t) { 
  return Surface::Adaptate_(t); 
}
void* TffSurface::Adaptate_(const void* t) { 
  if ( t == TypeID__) return (void*)this; 
  else if ( void* x = BasicSurface::Adaptate_(t) ) return x;
  else return 0;
}
void* GenericSurface::Adaptate_(const void* t) { 
  if ( t == TypeID__) return (void*)this; 
  else if ( void* y = MultiLayerSurface::Adaptate_(t) ) return y;
  else if ( void* x = BasicSurface::Adaptate_(t) ) return x;
  else return 0;
}
void* SurfaceRestorer::Adaptate_(const void* t) { 
  if ( t == TypeID__) return (void*)this; 
  else return IRefObject::Adaptate_(t); 
}
void* MultiLayerSurface::Adaptate_(const void* t) { 
  if ( t == MultiLayerSurface::TypeID__) return (void*)this; 
  else return Adaptable::Adaptate_(t); 
}

widestring TffSurface::ToString_() const { return widestring(_S*L"TffSurface 0x%08x" %this); } 
widestring GenericSurface::ToString_() const { return widestring(_S*L"GenericSurface 0x%08x" %this); } 
int      GenericSurface::GetWidth() const {return width_;}
int      GenericSurface::GetHeight() const {return height_;}
rect_t   GenericSurface::GetRect() const { return rect_t(0,0,width_,height_); }
unsigned GenericSurface::GetFormat() const {  return format_; }
bool     GenericSurface::IsLost() const { return memory_==0; }
void     GenericSurface::SetRestorer(SurfaceRestorer* rest) { restorer_ = AutoRef(MSKINSPACE::Grab(rest));}
Surface* GenericSurface::GetAlphaSurface() { return alpha_; }
Surface* GenericSurface::GetMainSurface() { return this; }
void     GenericSurface::Release_() const { refbase::Release_(); }
void     GenericSurface::Grab_() const { refbase::Grab_(); }

SCERROR GenericSurface::SetPal(const BufferT<mrgba_t>& pal/*[256]*/) 
{
  fail_if ( pal.size() > 256 );
  pal_.resize(256);
  teggo::copy(pal.begin(),pal.end(),pal_.begin());
  return SCE_OK;
}

SCERROR GenericSurface::GetPal(BufferT<mrgba_t>& pal/*[256]*/) const
{
  pal.resize(256);
  teggo::copy(pal_.begin(),pal_.end(),pal.begin());
  return SCE_OK;
}

void* GenericSurface::GetMemory(int& stride,Surface::RestoreBehaviour bih)
{ 
  switch ( GET_PIXFORMAT(format_) ) {
  case Surface::ALPHA:  stride = width_; break;
  case Surface::PAL:    stride = width_; break;
  case Surface::RGB5x5: stride = width_*2; break;
  case Surface::RGBx32: stride = width_*4; break;
  default:
    stride = 0;
    retval_if ("INVALID SURFACE FORMAT", 0);
  }
  if ( !memory_ ) RestoreSurface(bih);
  return memory_; 
}

void GenericSurface::Drop()
{
  if ( memory_ ) {
    delete[] memory_;
    memory_ = 0;
  }
}

SCERROR GenericSurface::Restore()
{
  return RestoreSurface(Surface::RESTORECONTENT);
}

SCERROR GenericSurface::RestoreSurface(Surface::RestoreBehaviour bih)
{
  if ( !memory_ ) {
    memory_= new byte_t[memsize_];
    if ( !memory_ ) return SCE_OUT_OF_MEMORY;
    if ( format_ == Surface::ALPHA ) memset(memory_,0xff,memsize_);
    else if ( !IsNull(restorer_) && bih != Surface::DONOTRESTORECONTENT ) 
      return restorer_->Restore(this);
  }
  return SCE_OK;
}

GenericSurface* GenericSurface::Create(int width,int height,unsigned format)
{
  return new GenericSurface(width,height,format);
}

GenericSurface::GenericSurface(int width,int height,unsigned format) 
: width_(width),height_(height),
format_((format&FORMATBITS)|(format&(ALPHALAYER|DOWNUP|SHORTGREEN|ALPHABITS))),
restorer_(0),memory_(0),memsize_(0)
{
  REQUIRE( width_ > 0 && height_ > 0 );
  switch (GET_PIXFORMAT(format_)) {
  case Surface::SURFMT_DEFAULT: 
    format_ = SET_PIXFORMAT(format_,Surface::RGB5x5);
    // NONE break
  case Surface::RGB5x5:
    memsize_ = width_*height_*2;
    break;
  case Surface::RGBx32:
    memsize_ = width_*height_*4;
    break;
  case Surface::PAL:
    memsize_ = width_*height_;
    break;
  case Surface::ALPHA:
    memsize_ = width_*height_;
    break;
  default:
    memsize_ = 0;
    return_if ("INVALID SURFACE FORMAT");
  }
  if ( (format_&Surface::ALPHALAYER) && GET_PIXFORMAT(format_) != Surface::ALPHA ) {
    alpha_ = AutoRef((Surface*)new GenericSurface(width,height,Surface::ALPHA));
  }
}

GenericSurface::~GenericSurface()
{
  Drop();
}

SCERROR Surface::FillRect_(Surface* surf,u32_t clrx,const rect_t& rectArg)
{
  int stride;
  SurfLock lsurf(surf); reterr_if ( lsurf.PeekError() );
  byte_t* const mem = (byte_t*)lsurf->GetMemory(stride);
  int width = surf->GetWidth();
  int height = surf->GetHeight();
  rect_t rect = rectArg;
  if ( rect.bottom > height ) rect.bottom = height;
  if ( rect.right > width ) rect.right = width;
  if ( rect.left < 0 ) rect.left = 0;
  if ( rect.top < 0 ) rect.top = 0;
  unsigned fmt = surf->GetFormat();
  if ( (fmt&FORMATBITS) == RGB5x5 ) {
    const u16_t clr = u16_t(clrx);
    u16_t tmpb[8] = {clr,clr,clr,clr,clr,clr,clr,clr};
    for ( int row = rect.top; row < rect.bottom; ++row )
    {
      u16_t* p = (u16_t*)(mem+row*stride);
      for ( int i = rect.left; i < rect.right; i+=8 )
      {
        FastCopy16(p+i,tmpb,teggo_min(8,rect.right-i));
      }
    }
  }else{
    Lerr << _S*":Surface: FillRect_: format 0x%x is unsupported" %fmt;
    return SCE_UNSUPPORTED;
  }
  return SCE_OK;
}

SCERROR Surface::Clear_(Surface* surf,u32_t clrx)
{
  int stride;
  SurfLock lsurf(surf); reterr_if ( lsurf.PeekError() );
  byte_t* const mem = (byte_t*)lsurf->GetMemory(stride);
  int width = surf->GetWidth();
  int height = surf->GetHeight();
  unsigned fmt = surf->GetFormat();
  if ( (fmt&FORMATBITS) == RGB5x5 ) {
    const u16_t clr = u16_t(clrx);
    for ( int row = 0; row < height; ++row )
    { 
      u16_t* p = (u16_t*)(mem+row*stride);
      for ( int i = 0; i < width; ++i ) p[i] = clr;
    }
  }else
  if ( (fmt&FORMATBITS) == RGBx32 ) {
    for ( int row = 0; row < height; ++row )
    {
      u32_t* p = (u32_t*)(mem+row*stride);
      for ( int i = 0; i < width; ++i ) p[i] = clrx;
    }
  }else
  if ( (fmt&FORMATBITS) == PAL ) {
    for ( int row = 0; row < height; ++row )
      memset(mem+row*stride,byte_t(clrx),width);
  }else{
    Lerr << _S*":Surface: Clear_: format 0x%x is unsupported" %fmt;
    return SCE_UNSUPPORTED;
  }
  return SCE_OK;
}

SCERROR Surface::CopyAlphaBits_(Surface* to,Surface* from)
{
  fail_if ( !to || !from);

  const unsigned width  = to->GetWidth();
  const unsigned height = to->GetHeight();

  fail_if ( width != unsigned(from->GetWidth()) );
  fail_if ( height != unsigned(from->GetHeight()) );
  
  const unsigned src_fmt = Surface::GET_PIXFORMAT(from->GetFormat());
  const unsigned dst_fmt = Surface::GET_PIXFORMAT(to->GetFormat());

  fail_if ( src_fmt != Surface::RGBx32 && src_fmt != Surface::ALPHA );
  fail_if ( dst_fmt != Surface::RGBx32 && dst_fmt != Surface::ALPHA );

  int src_stride;
  int dst_stride;

  SurfLock dst_surf(to); reterr_if ( dst_surf.PeekError() );
  byte_t* dst = (byte_t*)dst_surf->GetMemory(dst_stride);
  SurfLock src_surf(from); reterr_if ( src_surf.PeekError() );
  const byte_t* src = (byte_t*)src_surf->GetMemory(src_stride);
  const byte_t* const srcE = src+src_stride*height;

  if ( src_fmt == Surface::RGBx32 && dst_fmt == Surface::RGBx32 ) {
    for ( ; src < srcE ; src+=src_stride, dst+=dst_stride) {
      for ( unsigned i = 0 ; i < width; ++i )
        ((mrgba_t*)dst)[i].A = ((mrgba_t*)src)[i].A;
    }
  }else if (  src_fmt == Surface::RGBx32 && dst_fmt == Surface::ALPHA ) {
    for ( ; src < srcE ; src+=src_stride, dst+=dst_stride) {
      for ( unsigned i = 0 ; i < width; ++i )
        dst[i] = ((mrgba_t*)src)[i].A;
    }
  }else if (  src_fmt == Surface::ALPHA && dst_fmt == Surface::ALPHA ) {
    for ( ; src < srcE ; src+=src_stride, dst+=dst_stride) {
      memcpy(dst,src,width);
    }
  }else if (  src_fmt == Surface::ALPHA && dst_fmt == Surface::RGBx32 ) {
    for ( ; src < srcE ; src+=src_stride, dst+=dst_stride) {
      for ( unsigned i = 0 ; i < width; ++i )
        ((mrgba_t*)dst)[i].A = src[i];
    }
  }
  
  return SCE_OK;
}

SCERROR Surface::SetAlphaBitsByColorKey_(Surface* surf,mrgb_t colorkey)
{
  fail_if ( !surf);
  mrgba_t rgba = mRGBA_t(colorkey.R,colorkey.G,colorkey.B,255);

  const unsigned width  = surf->GetWidth();
  const unsigned height = surf->GetHeight();
  
  const unsigned fmt = Surface::GET_PIXFORMAT(surf->GetFormat());
  fail_if ( fmt != Surface::RGBx32 );

  int stride;
  SurfLock lsurf(surf); reterr_if ( lsurf.PeekError() );
  byte_t* mem = (byte_t*)lsurf->GetMemory(stride);
  const byte_t* const memE = mem+stride*height;

  for ( ; mem < memE ; mem+=stride) {
    for ( unsigned i = 0 ; i < width; ++i ) {
      mrgba_t c = ((mrgba_t*)mem)[i]; c.A = 255;
      if ( c == rgba ) c.A = 0;
      ((mrgba_t*)mem)[i] = c;
    }
  }

  return SCE_OK;
}

SCERROR BasicSurface::BitBlt(Surface* from,int x,int y,const rect_t* rect) 
{
  return BitBlt_(this,from,x,y,rect);
}

SCERROR BasicSurface::TransparentBlt(Surface* from,int x, int y,const rect_t* rect)
{
  return TransparentBlt_(this,from,from->GetTransparent(),x,y,rect);
}

SCERROR BasicSurface::AlphaBlt(Surface* from,Surface* alpha,int x, int y,const rect_t* rect) 
{
  return AlphaBlt_(this,from,alpha,x,y,rect);
}

SCERROR BasicSurface::AlphaValBlt(Surface* from,unsigned alpha,int x, int y,const rect_t* rect)
{
  return AlphaValBlt_(this,from,alpha,x,y,rect);
}

SCERROR BasicSurface::FillRect(u32_t color ,const rect_t& rect)
{
  return FillRect_(this,color,rect);
}

SCERROR BasicSurface::Clear(u32_t color)
{
  return Clear_(this,color);
}

widestring BasicSurface::ToString_() const { return widestring(L"derived from basic surface"); }
//void BasicSurface::Grab_() const {}
//void BasicSurface::Release_() const {}

bool BasicSurface::IsLost() const { return false; }
void BasicSurface::SetRestorer(SurfaceRestorer*){}
void BasicSurface::Drop(){}
SCERROR BasicSurface::Restore(){ return SCE_OK; }

void* BasicSurface::GetMemory(int& stride,RestoreBehaviour)
{
  Lout << ":WARNING: dummy BasicSurface::GetMemory called";
  stride = 0;
  return 0;
}

/// âîçâðàùàåò çíà÷åíèå ïèêñåëà
u32_t BasicSurface::GetPixel(int x,int y) {
  if ( x < 0 || x >= GetWidth() || y < 0 || y >= GetWidth() ) return 0;
  SurfLock lsurf(this);
  int stride;
  byte_t* mem = (byte_t*)lsurf->GetMemory(stride);
  switch ( GetFormat() & FORMATBITS ) {
  case RGB5x5: 
    return u32_t(((u16_t*)(mem+stride*y))[x]);
  case ALPHA:
  case PAL:
    return u32_t((mem+stride*y)[x]);
  case RGBx32:
    return ((u32_t*)(mem+stride*y))[x];
  default:
    return 0;
  }
}

void BasicSurface::SetTransparent(u32_t t) { 
  transparent_ = t;
}

u32_t BasicSurface::GetTransparent() { 
  return transparent_;
}

SCERROR BasicSurface::Lock(void** p,int* stride, RestoreBehaviour bih) 
{
  REQUIRE ( p != 0 );
  REQUIRE ( stride != 0 );
  *p = GetMemory(*stride,bih);
  fail_if ( !*p );
  return SCE_OK;
}
SCERROR BasicSurface::Unlock() { return SCE_OK; }

SCERROR BasicSurface::Load(DataSource* source) 
{ 
  return Load_(this,source);
}

SCERROR BasicSurface::SetPal(const BufferT<mrgba_t>& pal/*[256]*/) 
{
  Lout << _S*"Surface::SetPal is unsupported for surface %?"%ToString_();
  return SCE_UNSUPPORTED;
}

SCERROR BasicSurface::GetPal(BufferT<mrgba_t>& pal/*[256]*/) const
{
  Lout << _S*"Surface::GetPal is unsupported for surface %?"%ToString_();
  return SCE_UNSUPPORTED;
}

mrgba_t BasicSurface::Pixel2Color(u32_t pixel) const
{
  return Surface::Pixel2Color_(this,pixel);
}

u32_t BasicSurface::Color2Pixel(mrgba_t color) const
{
  return Surface::Color2Pixel_(this,color);
}

mrgba_t Surface::Pixel2Color_(const Surface* surf,u32_t pixel)
{
  switch ( surf->GetFormat() & Surface::FORMATBITS ) {
  case RGB5x5: 
    if ( surf->GetFormat() & Surface::SHORTGREEN )
      return c555_to_mrgba(u16_t(pixel));
    else
      return c565_to_mrgba(u16_t(pixel));
  case RGBx32: return *(mrgba_t*)&pixel;
  default:; // nothing
  }
  return mRGBA_t(0,0,0,0);
}

u32_t Surface::Color2Pixel_(const Surface* surf,mrgba_t color)
{
  switch ( surf->GetFormat() & Surface::FORMATBITS ) {
  case RGB5x5: 
    if ( surf->GetFormat() & Surface::SHORTGREEN )
      return xxx_to_c555(color);
    else
      return xxx_to_c565(color);
  case RGBx32: return color.C();
  default:; // nothing
  }
  return 0;
}

SCERROR Surface::Load(pwide_t source)
{
  AutoDataSource ds ( RSCMAN::CreateSource(source) );
  return Load(ds);
}

NAMESPACE_MSKIN_END

