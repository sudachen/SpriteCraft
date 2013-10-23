
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
#include "m_graphics.h"
#include "../mcore/m_time.h"
#include "../mstreams/m_igzstream.h"
#include "../mutil/m_roundpow2.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

Texture* CreateTextureFrom(pwide_t resname,unsigned fmt,int width,int height)
{
  AutoDataSource source(RSCMAN::CreateSource(resname));
  return CreateTextureFrom(source,fmt,width,height);
}

unsigned RecognizeSurfaceFormat(unsigned& texfmt)
{
  unsigned surf_format = 0;
  if ( IS_TEX_PALETTED(texfmt) && !(SCREEN::GetCaps()&GFX_CAP_PAL8) ) {
    Lout << "texloader: paletted textures is not suported by device";
    texfmt = GFX3D_TEX_RGB5;
  }
  switch ( texfmt ) {
  case GFX3D_TEX_RGBA8:      surf_format = Surface::SYS_RGBx32; break;
  case GFX3D_TEX_RGB5:       surf_format = Surface::SYS_RGBx32; break;
  case GFX3D_TEX_PAL8:       surf_format = Surface::SYS_PAL; break;
  case GFX3D_TEX_ALPHA:      surf_format = Surface::SYS_ALPHA; break;
  case GFX3D_TEX_AUTODETECT: surf_format = Surface::AUTODETECT; break;
  default:
    ret0_because("texloader: invalid texture format");
  }
  return surf_format;
}

Texture* CreateTextureFrom(DataSource* source,unsigned fmt,int width,int height)
{
  u32_t millis = GetMillis();
  retval_if_fail ( source != 0 , 0);
  unsigned surf_format = RecognizeSurfaceFormat(fmt);
  auto_ref<ImageDataSource> ds( MakeImageDataSourceFrom(source) );
  retval_if ( IsNull(ds) , 0 );
  ImageDataSource::ImageInfo info;
  retval_if ( ds->ReadPicInfo(info) != SCE_OK , 0 );
  if ( fmt == GFX3D_TEX_AUTODETECT ) {
    if ( info.bpp == 8 ) { 
      if ( !(SCREEN::GetCaps()&GFX_CAP_PAL8) ) {
        Lout << "texloader: paletted textures is not suported by device";
        fmt = GFX3D_TEX_RGB5; surf_format = Surface::SYS_RGBx32;
      } else {
        fmt = GFX3D_TEX_PAL8; surf_format = Surface::SYS_PAL;
      }
    }
    else {
      if ( info.bpp == 16 ) fmt = GFX3D_TEX_RGB5;
      else if ( info.bpp == 32 || info.bpp == 24 ) fmt = GFX3D_TEX_RGBA8;
      else ret0_because ( "texloader: invalid image format" );
      surf_format = Surface::SYS_RGBx32;
    }
  }
  
  {
    int max_tex_size = SCREEN::Get3DDevice()->GetMaxTextureSize();
    int pic_width  = teggo_min<int>(RoundPow2(info.width),max_tex_size);
    int pic_height = teggo_min<int>(RoundPow2(info.height),max_tex_size);
    if ( width == 0 ) width = pic_width;
    if ( height == 0 ) height = pic_height;
  }

  //surf_format |= Surface::DOWNUP;

  AutoSurface surf ( GenericSurface::Create(info.width,info.height,surf_format) );
  retval_if ( IsNull(surf), 0 );
  retval_if ( SCE_OK != LoadSurfaceFrom( ds, surf, surf_format ), 0 );

  //if ( width != info.width || height != info.height ) {
  //}

  AutoTexture tex ( SCREEN::CreateDirtyTexture(width,height,fmt) );
  retval_if ( IsNull(tex) , 0 );
  retval_if ( tex->Load(surf) , 0);
    
  //XMessage((":CreateTextureFrom: %s (%s)",source->GetName(),tex->ToString_()));
  Lout << _S*"texloader: texture %? in %? ms" %source->GetName() %(GetMillis()-millis);
  return Forget( tex );
}

TextureSet* TextureSet::instance_   = 0;
unsigned    TextureSet::texfmtdef_  = GFX3D_TEX_AUTODETECT;

TextureSet::TextureSet() { 
  REQUIRE ( instance_ == 0 );
  instance_ = this; 
}

TextureSet::~TextureSet() {
  REQUIRE ( instance_ == this );
  instance_ = 0;
}

TextureSet* TextureSet::AcquireSet() 
{ 
  if ( !instance_ ) return new TextureSet();
  return Grab(instance_);
}

struct ProxyTexture_OfTextureSet : public Texture, private refbase {
  auto_ref<TextureSet> supporter_;
  AutoTexture texture_;
  widestring name_;
  ProxyTexture_OfTextureSet( TextureSet* supporter, pwide_t name ) 
    : supporter_(Grab(supporter)), name_(name) {}
  virtual ~ProxyTexture_OfTextureSet() { supporter_->UnAcquireTexture(this); }

  virtual SCERROR Load(Surface* surf,int level) 
  { return texture_->Load(surf,level); }
  virtual SCERROR LoadSubpicture(Surface* surf,int x,int y,const rect_t& rect,int level) 
  { return texture_->LoadSubpicture(surf,x,y,rect,level);}
  virtual SCERROR LoadPalette(const BufferT<mrgba_t>& pal) 
  { return texture_->LoadPalette(pal); }
  virtual SCERROR StoreSubpicture(Surface* surf,int x,int y,const rect_t& rec) const 
  { return texture_->StoreSubpicture(surf,x,y,rec); }
  virtual int         GetWidth() const  { return texture_->GetWidth(); }
  virtual int         GetHeight() const { return texture_->GetHeight(); }
  virtual unsigned    GetFormat() const { return texture_->GetFormat(); }
//  virtual void        Drop() { texture_->Drop(); }
//  virtual SCERROR Restore() { return texture_->Restore(); }
  virtual void* Adaptate_(const void* id) { 
    if ( void* x = IRefObject::Adaptate_(id) ) return x;
    if ( void* x = texture_->Adaptate_(id) ) return  x;
    return 0;
  }
  virtual void Grab_() const { refbase::Grab_(); }
  virtual void Release_() const { refbase::Release_(); }
  virtual SCERROR Reload() { 
    Release(texture_);
    texture_ = AutoRef(CreateTextureFrom(+name_,supporter_->texfmtdef_));
    if (IsNull(texture_)) fail_because("texloader: (texture proxy) error while loading texture");
    return SCE_OK;
  }
  virtual widestring const& GetName() const { return name_; }
  virtual bool IsLost() const { return texture_->IsLost(); }
  virtual SCERROR Restore() { return texture_->Restore(); }
};

Texture* TextureSet::Acquire_(pwide_t name)
{
  if ( ProxyTexture_OfTextureSet** p = texmap_.Get(name) )
    return Grab(*p);
  else
  {
    auto_ref<ProxyTexture_OfTextureSet> ptex ( new ProxyTexture_OfTextureSet( this, name ) );
    if ( ptex->Reload() != SCE_OK ) ret0_because("texloader: (texture proxy) can't load texture");
    texmap_.Put(name,ptex);
    return Forget(ptex);
  }
}

void TextureSet::UnAcquireTexture(ProxyTexture_OfTextureSet* tex)
{ 
  texmap_.Erase(+tex->GetName());
}

void TextureSet::SetFormat(unsigned texfmt)
{
  if ( texfmt != GFX3D_TEX_AUTODETECT &&
       texfmt != GFX3D_TEX_PAL8 &&
       texfmt != GFX3D_TEX_RGB5 &&
       texfmt != GFX3D_TEX_RGBA8 )
       return_because("texloader: (texture set) invalid texture format");
  if ( texfmt == GFX3D_TEX_PAL8 && !(SCREEN::GetCaps()&GFX_CAP_PAL8) ) {
    Lout << _S*"texloader: (texture set) palette texture format is not supported by device";
    texfmt = GFX3D_TEX_RGB5;
  }
  texfmtdef_ = texfmt;
}

SCERROR TextureSet::Restore()
{
  if ( !instance_ ) return SCE_OK;
  SCERROR result = SCE_OK;
  TEXTURES::Iterator i = instance_->texmap_.Iterate();
  while ( i.Next() ) 
    if ( (*i)->Reload() != SCE_OK ) result = SCE_FAIL;
  return result;
}

NAMESPACE_MSKIN_END

