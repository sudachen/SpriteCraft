
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
#include "m_d3d7_device.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$gfxD3D7")
#endif

namespace gfxD3D7 {

  using mskin::AutoObject;

// относительно этой функции существует один маленький , но важный момент
// она может приводить к уничтожении текстуры если единственная ссылка на текстуру
// бала только в Gfx сервере, при эом будет вызван Texture::Drop который
// НЕ ДОЛЖЕН найти в locked_texture_ значение равное this., для этого
// содается AutoObject который держит текстуру до завершения исполнения функции
SCERROR GfxD3D7Device::SetTexture(Texture* tex)
{
  AutoObject _ ( Grab(locked_texture_) );
  if ( tex == (Texture*)~locked_texture_ ) return SCE_OK;
  if ( !tex ) {
    Reset(locked_texture_,(GfxD3D7Texture*)0);
    idd_->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_DISABLE);
    return SCE_OK;
  }
  if ( GfxD3D7Texture* texture = Adaptate<GfxD3D7Texture>(tex) ) {
    if ( !locked_texture_ )
      idd_->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
    Reset(locked_texture_,Grab(texture));
    return texture->Bind();
  }
  retval_because("gfxd3d7: invalid texture passed",SCE_FAIL);
}

Texture* GfxD3D7Device::CreateDirtyTexture(int width,int height,unsigned flags,unsigned mipmap)
{
  return new GfxD3D7Texture(this,width,height,flags,mipmap);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// GxfD3D7Texture
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static unsigned ConvertTexFormat(unsigned format) {
  switch ( format ) {
  case GFX3D_TEX_RGBA8: return D3D7_ITEX_RGBA8;
  case GFX3D_TEX_RGB5:  return D3D7_ITEX_RGB5;
  case GFX3D_TEX_ALPHA: return D3D7_ITEX_ALPHA;
  case GFX3D_TEX_PAL8:  return D3D7_ITEX_PAL8;
  case GFX3D_TEX_RGBA4: return D3D7_ITEX_RGBA4;
  case GFX3D_TEX_RGB5A1: return D3D7_ITEX_RGB5A1;
  default:
    retval_because("gfxd3d7: invalid texture format, using RGBA8",D3D7_ITEX_RGBA8);
  }
};

static inline pchar_t FormatToString( unsigned fmt ) {
  switch ( fmt ) {
  case GFX3D_TEX_RGBA8: return "GFX3D_TEX_RGBA8";
  case GFX3D_TEX_RGB5 : return "GFX3D_TEX_RGB5";
  case GFX3D_TEX_ALPHA: return "GFX3D_TEX_ALPHA";
  case GFX3D_TEX_RGBA4: return "GFX3D_TEX_RGBA4";
  case GFX3D_TEX_RGB5A1: return "GFX3D_TEX_RGB5A1";
  case GFX3D_TEX_PAL8 :
    {
      if ( GfxD3D7Device::texpf_[D3D7_ITEX_PAL8].dwFlags )
        return "GFX3D_TEX_PAL8/DDPF_PALETTEINDEXED8";
      else {
        return "GFX3D_TEX_PAL8/D3D7_RGBA";
      }
    }
  }
  return "INVALID/D3D7_RGBA";
}

widestring GfxD3D7Texture::ToString_() const
{
  return
    widestring(_S*L"d3d7texture(%?) {%?x%?,%?}"
        % (Texture*)this
        % width_
        % height_
        % FormatToString(format_)
      );
}

void GfxD3D7Texture::Release_() const { refbase::Release_(); }
void GfxD3D7Texture::Grab_() const { refbase::Grab_(); }

SCERROR GfxD3D7Texture::Load(Surface* surf,int level)
{
  LoadSubpicture(surf,0,0,surf->GetRect(),level);
  if ( IS_TEX_PALETTED(format_) ) {
    BufferT<mrgba_t> pal;
    reterr_if(surf->GetPal(pal));
    reterr_if(LoadPalette(pal));
  }
  return SCE_OK;
}

SCERROR GfxD3D7Texture::LoadSubpicture(Surface* surf,int x,int y,const rect_t& prect,int level)
{
  fail_if_fail( Restore() );
  fail_if ( surf == 0 );
  unsigned surf_format = Surface::GET_PIXFORMAT(surf->GetFormat());
  fail_if ( IS_TEX_PALETTED(format_) && surf_format != Surface::PAL );
  fail_if ( !IS_TEX_PALETTED(format_)
    && surf_format != Surface::RGBx32
    && surf_format != Surface::RGBx24
    && surf_format != Surface::RGB5x5
    && surf_format != Surface::ALPHA );
  rect_t surf_rect = prect.
      Intersection(surf->GetRect()).
      ClipSelf(width_-x,height_-y);
  if ( surf_rect.IsZero() ) return SCE_OK;
  SurfLock lsurf(surf);
  int stride;
  byte_t* mem = (byte_t*)lsurf->GetMemory(stride);
  fail_if ( mem == 0 );
  RECT lockRect = {x,y,x+surf_rect.Width(),y+surf_rect.Height()};
  DDSURFACEDESC2 ddsd2 = {0};
  ddsd2.dwSize = sizeof(ddsd2);
  HRESULT hr;
  bool invalid_formats = false;
  rcc_ptr<IDirectDrawSurface7> miptex_( Refe(texture_) );

  {
    DDSCAPS2 caps2 = {0};
    caps2.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
    for ( int l = 0; l < level ; ++l )
    {
      rcc_ptr<IDirectDrawSurface7> miptex1_;
      fail_if_dxerr( miptex_->GetAttachedSurface(&caps2,&+miptex1_) );
      miptex_ = miptex1_;
    }
  }

  if ( SUCCEEDED(hr = miptex_->Lock(&lockRect,&ddsd2,DDLOCK_WRITEONLY,0)) ) {
    if ( TffSurface *tffsurf = Adaptate<TffSurface>(surf) )
      {
        u32_t pf = 0;
        u32_t surf_pf = tffsurf->TffPixelFormat();
        switch ( iformat_ )
          {
            case D3D7_ITEX_RGBA8: pf = TFF_PF_RGBA8; break;
            case D3D7_ITEX_RGBA4: pf = TFF_PF_RGBA4; break;
            case D3D7_ITEX_RGB5:  pf = TFF_PF_RGB5; break;
            case D3D7_ITEX_RGB5A1:pf = TFF_PF_RGB5A1; break;
          }
        if ( pf )
          {
            int x = surf_rect.left, y = surf_rect.top;
            int bpp = (pf&TFF_PF_BPPMASK)/8;
            byte_t *dst = (byte_t*)ddsd2.lpSurface;
            byte_t *src = (byte_t*)mem + (bpp*x + stride*y);
            int cnt = bpp * surf_rect.Width();
            int rows = surf_rect.Height();

            if ( pf == surf_pf )
              for ( int i = 0; i < rows; ++i, dst += ddsd2.lPitch, src += stride )
                {
                  memcpy( dst, src, cnt );
                }
            else
              {
                TFFt_CopyRowXtoX copy_f = TffmCopyRowXtoX(surf_pf,pf);
                for ( int i = 0; i < rows; ++i, dst += ddsd2.lPitch, src += stride )
                  copy_f( src, dst, rows );
              }
          }
        else
          invalid_formats = true;
      }
    else if ( surf_format == Surface::RGBx32 ) {
      mem += surf_rect.left*4 + surf_rect.top*stride;
      if ( iformat_ == D3D7_ITEX_RGBA8 ) {
        byte_t* lpsurf = (byte_t*)ddsd2.lpSurface;
        for ( int row=0, h = surf_rect.Height(); row < h; ++row ) {
          const u32_t* src = (const u32_t*)mem; mem += stride;
          u32_t* dst  = (u32_t*)lpsurf; lpsurf += ddsd2.lPitch;
          u32_t* const dstE = dst+surf_rect.Width();
          for ( ; dst < dstE ; ++dst, ++src ) {
            u32_t srci = *src & 0x00ff00ff;
            *dst = (*src & 0xff00ff00) | ( srci >> 16 ) | ( srci << 16 );
          }
        }
      }else if ( iformat_ == D3D7_ITEX_RGB5 ) {
        byte_t* lpsurf = (byte_t*)ddsd2.lpSurface;
        for ( int row=0, h = surf_rect.Height(); row < h; ++row ) {
          const mrgba_t* src = (const mrgba_t*)mem; mem += stride;
          u16_t* dst  = (u16_t*)lpsurf; lpsurf += ddsd2.lPitch;
          u16_t* const dstE = dst+surf_rect.Width();
          for ( ; dst < dstE ; ++dst, ++src ) {
            *dst = xxx_to_c555(*src);
          }
        }
      }else if ( iformat_ == D3D7_ITEX_RGBA4 ) {
        byte_t* lpsurf = (byte_t*)ddsd2.lpSurface;
        for ( int row=0, h = surf_rect.Height(); row < h; ++row ) {
          const mrgba_t* src = (const mrgba_t*)mem; mem += stride;
          u16_t* dst  = (u16_t*)lpsurf; lpsurf += ddsd2.lPitch;
          u16_t* const dstE = dst+surf_rect.Width();
          for ( ; dst < dstE ; ++dst, ++src ) {
            *dst = xxx_to_c4444(*src);
          }
        }
      }else if ( iformat_ == D3D7_ITEX_RGB5A1 ) {
        byte_t* lpsurf = (byte_t*)ddsd2.lpSurface;
        for ( int row=0, h = surf_rect.Height(); row < h; ++row ) {
          const mrgba_t* src = (const mrgba_t*)mem; mem += stride;
          u16_t* dst  = (u16_t*)lpsurf; lpsurf += ddsd2.lPitch;
          u16_t* const dstE = dst+surf_rect.Width();
          for ( ; dst < dstE ; ++dst, ++src ) {
            *dst = xxx_to_c5551(*src);
          }
        }
      }else invalid_formats = true;
    }
    else if ( surf_format == Surface::PAL ) {
      mem += surf_rect.left + surf_rect.top*stride;
      if ( iformat_ == D3D7_ITEX_PAL8 ) {
        byte_t* lpsurf = (byte_t*)ddsd2.lpSurface;
        byte_t* const lpsurfE = lpsurf+ddsd2.lPitch*surf_rect.Height();
        for ( ; lpsurf < lpsurfE ;  ) {
          memcpy(lpsurf,mem,surf_rect.Width());
          mem += stride;
          lpsurf += ddsd2.lPitch;
        }
      }else invalid_formats = true;
    }
    else invalid_formats = true;
    miptex_->Unlock(&lockRect);
    if ( invalid_formats ) fail_because("gfxd3d7: invalid formats");
  }
  else
    fail_because( _S*"gfxd3d7: texture lock failed with code %08x" %hr );
//      IS_TEX_PALETTED(format_)?D3D7_COLOR_INDEX:surf_format==Surface::ALPHA?D3D7_ALPHA:D3D7_RGBA,
//      D3D7_UNSIGNED_BYTE,
  return SCE_OK;
}

SCERROR GfxD3D7Texture::LoadPalette(const BufferT<mrgba_t>& pal)
{
  fail_if_fail ( Restore() );
  fail_if_fail ( IS_TEX_PALETTED(format_) );
  fail_if_fail ( pal.size() != 0 );
  //if ( !(GfxD3D7Device::texpf_[D3D7_ITEX_PAL8].dwFlags&&GfxD3D7Device::texpf_[D3D7_ITEX_PAL8].dwFlags) )
  if ( !GfxD3D7Device::texpf_[D3D7_ITEX_PAL8].dwFlags )
    fail_because("gfxd3d7: paletted textures unsuported by D3D");
  rcc_ptr<IDirectDrawPalette> idpal;
  HRESULT hr = texture_->GetPalette(&+idpal);
  if ( hr == DDERR_NOPALETTEATTACHED ) {
    GfxD3D7Device* device = GetServer();
    fail_if ( !device );
    BufferT<PALETTEENTRY> palentry (256);
    fail_if_dxerr ( device->idraw_->CreatePalette(
      DDPCAPS_8BIT|DDPCAPS_ALPHA|DDPCAPS_ALLOW256,
      &palentry[0],&+idpal,0) );
    fail_if_dxerr ( texture_->SetPalette(+idpal) );
  }
  //else if ( hr == S_OK )
  //  fail_if_dxerr( idpal->SetEntries( 0, 0, 256, &palentry[0] ) );
  else fail_if_dxerr( hr );
  fail_if_dxerr( idpal->SetEntries( 0, 0, teggo_min<u32_t>(pal.size(),256), (PALETTEENTRY*)&pal[0] ) );
  return SCE_OK;
}

SCERROR GfxD3D7Texture::StoreSubpicture(Surface* surf,int x,int y,const rect_t& prect) const
{
  return SCE_UNSUPPORTED;
}

SCERROR GfxD3D7Texture::StorePalette(BufferT<mrgba_t>& pal) const
{
  return SCE_UNSUPPORTED;
}

int GfxD3D7Texture::GetWidth() const  {return width_;}
int GfxD3D7Texture::GetHeight() const {return height_;}
unsigned GfxD3D7Texture::GetFormat() const {return format_;}

void GfxD3D7Texture::Drop(){
  if ( GfxD3D7Device* device = GetServer() ) {
    if ( Get(device->locked_texture_) == this ) {
      device->SetTexture(0);
    }
  }
  Unrefe( texture_ );
}

SCERROR GfxD3D7Texture::RestoreTextureObject() {
  return SCE_UNSUPPORTED;
}

SCERROR GfxD3D7Texture::Restore(){
  //return SCE_UNSUPPORTED;
  if ( !texture_ ) {
    GfxD3D7Device* device = GetServer();
    if ( device ) {
      iformat_ = ConvertTexFormat(format_);
      u32_t alpha_flag = 0;

      if ( GfxD3D7Device::texpf_[iformat_].dwSize == 0 ) {
        //if ( iformat_ == D3D7_ITEX_ALPHA && (texpf_[D3D7_ITEX_PAL8].dwFlags&&texpf_[D3D7_ITEX_PAL8].dwFlags) )
        if ( iformat_ == D3D7_ITEX_ALPHA && GfxD3D7Device::texpf_[D3D7_ITEX_PAL8].dwFlags )
          iformat_ = D3D7_ITEX_PAL8,
          alpha_flag = DDSCAPS_ALPHA;
        else
          iformat_ = D3D7_ITEX_RGBA8;
      }

      if ( iformat_ ==  D3D7_ITEX_RGB5A1 && !GfxD3D7Device::texpf_[D3D7_ITEX_RGB5A1].dwFlags )
        iformat_ = D3D7_ITEX_RGBA8;
      if ( iformat_ ==  D3D7_ITEX_RGB5 && !GfxD3D7Device::texpf_[D3D7_ITEX_RGB5].dwFlags )
        iformat_ = D3D7_ITEX_RGBA8;

      if ( iformat_ == D3D7_ITEX_RGBA8 && !GfxD3D7Device::texpf_[D3D7_ITEX_RGBA8].dwFlags )
        iformat_ = D3D7_ITEX_RGBA4;
      else if ( iformat_ == D3D7_ITEX_RGBA4 && !GfxD3D7Device::texpf_[D3D7_ITEX_RGBA4].dwFlags )
        iformat_ = D3D7_ITEX_RGBA8;

      DDSURFACEDESC2 ddsd = {0,};
      ddsd.dwSize = sizeof(DDSURFACEDESC2);
      ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
      ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | alpha_flag;
      ddsd.ddsCaps.dwCaps2 = DDSCAPS2_D3DTEXTUREMANAGE;
      ddsd.dwWidth = width_;
      ddsd.dwHeight = height_;
      ddsd.dwTextureStage = 0;
      if ( mipmap_ )
      {
        Lout << _S*"mipmaplevel: %d" %mipmap_;
        ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
        ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP|DDSCAPS_COMPLEX;
        ddsd.dwMipMapCount = mipmap_;
      }
      memcpy( &ddsd.ddpfPixelFormat,&GfxD3D7Device::texpf_[iformat_], sizeof(DDPIXELFORMAT));
      //LogF("Createing surface with format %d, flags 0x%08x",iformat_,GfxD3D7Device::texpf_[iformat_].dwFlags);
      fail_if_dxerr( device->idraw_->CreateSurface(&ddsd,&+texture_,0) );
    }else fail_because("gfxd3d7: has no device");
  }else if ( texture_->IsLost() ) {
    fail_if_dxerr(texture_->Restore());
  }
  warn_if_dxerr(texture_->SetPriority(u32_t(USHRT_MAX*priority_)));
  return SCE_OK;
}

SCERROR GfxD3D7Texture::Bind()
{
  if ( GfxD3D7Device* device = GetServer() ) {
    fail_if ( !texture_ );
    fail_if_dxerr( device->idd_->SetTexture(0,+texture_) );
  }
  return SCE_OK;
}

void GfxD3D7Texture::SetPriority(float priority)
{
  priority_ = priority;
  if ( !!texture_ )
    warn_if_dxerr(texture_->SetPriority(u32_t(USHRT_MAX*priority)));
}

GfxD3D7Texture::GfxD3D7Texture(GfxD3D7Device* device, int width,int height, unsigned format, unsigned mipmaplevel)
: GfxD3D7Object(device)
{
  format_     = format;
  if ( format_ == GFX3D_TEX_PAL8 &&
//    !(texpf_[D3D7_ITEX_PAL8].dwFlags&&texpf_[D3D7_ITEX_PAL8].dwFlags) )
    !GfxD3D7Device::texpf_[D3D7_ITEX_PAL8].dwFlags )
  {
    Lout << "gfxd3d7: paletted textures is not suported by D3D7, yet";
    format_   = GFX3D_TEX_RGB5;
  }
  iformat_    = ConvertTexFormat(format_);
  width_      = width;
  height_     = height;
  priority_   = 0.5f;
  mipmap_     = mipmaplevel;
}

GfxD3D7Texture::~GfxD3D7Texture()
{
  Drop();
}

void GfxD3D7Texture::OnTermContext() {Drop();}
void GfxD3D7Texture::OnInitContext() {}

pchar_t const GfxD3D7Texture::TypeID__ = "#TYPEDEF#gfxD3D7#GfxD3D7Texture";
void* GfxD3D7Texture::Adaptate_(const void* id){
  if ( id == TypeID__ ) return this;
  if ( void* x = Texture::Adaptate_(id) ) return x;
  if ( void* y = GfxD3D7Object::Adaptate_(id) ) return y;
  return 0;
}

bool GfxD3D7Texture::IsLost() const {
  return !texture_ || texture_->IsLost();
}

} // namespace
