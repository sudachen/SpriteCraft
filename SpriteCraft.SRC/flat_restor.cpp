
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

#include "stdafx.h"
#include "flat_e3.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$flat$restor")
#endif

static void FillCollideBitmask(u32_t* bitmask,long cbm_cols,long cbm_rows,Surface* surf,const rect_t& rect)
{
  Xdbg( Ldbg << _S*"sprites: filling collide bitmask %dx%d"
    %(cbm_cols*32) %cbm_rows );
  memset ( bitmask, 0, cbm_rows*cbm_cols*4 );
  SurfLock lck(surf); return_if ( lck.PeekError() );
  int stride;
  cbyte_t* pixels = (cbyte_t*)lck->GetMemory(stride);
  //cbyte_t* pixelsE = pixels + rect.bottom*stride;
  pixels += rect.top*stride + rect.left*4;
  long width32 = (rect.Width()<<8)  / (cbm_cols * 32);
  long height32 = (rect.Height()<<8) / cbm_rows;
  {for ( int i = 0, iE = rect.Height(); i < iE; ++i ) {
    for ( int j = 0, jE = rect.Width(); j < jE; ++j ) {
      if ( *(pixels+i*stride+j*4+3) > 127 ) {
        long byte_no = ((i<<8)/height32 * cbm_cols + (((j<<8)/width32) >> 5));
        long bitshift = ((j<<8)/width32) & 31;
        if ( byte_no < cbm_cols*cbm_rows )
          bitmask[byte_no] |= MSKIN_BIT(bitshift);
      }
    }
  }}
//  {for ( int i = 0; i < cbm_rows ; ++i ) {
//    vector<char> v;
//    for ( int j = 0; j < cbm_cols ; ++j ) {
//      for ( int k = 0; k < 32; ++k ) {
//        v.push_back(bitmask[i*cbm_cols+j] & MSKIN_BIT(k) ? '1' : '0' );
//      }
//    }
//    v.push_back(0);
//    LogF(" -- %s",&v[0]);
//  }}
}

SCERROR Impl_3DSPRITES::RestoreKlass(pwide_t klass_name)
{
  if ( KlassOfSprite3D* klass = this->FindKlass_(klass_name) )
    if ( !klass->info_.dummy_ )
      return RestoreKlass_(*klass);
  return SCE_OK;
}

/**
  Восстанавливает(загружает) текстуры класса
*/
SCERROR Impl_3DSPRITES::RestoreKlass_(KlassOfSprite3D& klass)
{
  STRICT_REQUIRE ( !klass.info_.dummy_ );
  Gfx3DDevice* gfx = SCREEN::Get3DDevice();
  if ( !gfx ) fail_because("3D Device has not initialized");
  u32_t millis = GetMillis();
  if ( klass.info_.is_font )
  {
    if ( !klass.font_ && klass.info_.bitmap.Length() )
    {
      klass.font_ = AutoRef(
        TexFontSet::Acquire(
          +klass.info_.bitmap,
          klass.info_.symbols.Length()?klass.info_.symbols.Str():0,
          (klass.info_.type == SPRITE_COLORKEY ? GlyphSource::SRCCOLORKEY
          :klass.info_.type == SPRITE_BLEND||klass.info_.type == SPRITE_BITMASK ? GlyphSource::SRCBLEND
          :GlyphSource::SRCSOLID),
          klass.info_.colorkey) );
    }
    fail_if(!klass.font_);
  }
  else
  {
    SPRITE_Info& info = klass.info_;
    u32_t gfxcaps = SCREEN::GetCaps();
    // будет ли спрайт представлен палитровой текстурой
    bool pal8 =
      info.type == SPRITE_SOLID
      && info.is_paletted
      && !klass.is_scaled_
      && (gfxcaps&GFX_CAP_PAL8);
    // выбираем формат для сурфейса в который загружается картинка со спрайтом
    unsigned sfmt = pal8 ? Surface::PAL : Surface::RGBx32;

    // для класса отсутствует разметка текстур?
    //fail_if ( klass.textures_.empty() );
    if ( klass.textures_.empty() )
      fail_because( _S*"textures layout of klass '%?' broken" %klass.GetName() );
    // создаем/загружаем текстуры согласно разметке
    SPRITE_TexInfo* tex = &klass.textures_[0];
    SPRITE_TexInfo* texE = &klass.textures_.back()+1;

    AutoSurface bitmap;
    bool update_bitmask =
          klass.info_.collide == SPRITE_COLLIDE_BITMASK
          && !klass.collide_bitmask_
          && info.type != SPRITE_SOLID;

    for ( ; tex != texE ; ++tex ) {
      fail_if ( tex->layout_EX_.empty() );
      bool load_texture = false;

      if ( !!tex->image_ && tex->image_->IsLost() ) {
        load_texture = true;
        if ( tex->image_->Restore() != SCE_OK ) {
          if ( klass.use_shared_texture_ ) texset_.Forget(~tex->image_);
          Release( tex->image_ );
        }
      }
      else if ( klass.need_update_ )
        load_texture = true;

      bool tex_created = false;
      if ( !tex->image_ ) {
        u32_t textype;
        if ( !pal8 && info.type == SPRITE_SOLID && info.bits_rgb32  )
          textype = (gfxcaps&GFX_CAP_RGBA8)?GFX3D_TEX_RGBA8:GFX3D_TEX_RGB5;
        else if ( pal8 )
          textype = GFX3D_TEX_PAL8;
        else if ( info.type == SPRITE_SOLID )
          textype = GFX3D_TEX_RGB5;
        else if ( info.type == SPRITE_BITMASK )
          textype = (gfxcaps&GFX_CAP_RGBA8 && !info.bits_lowcolor )
                     ?GFX3D_TEX_RGBA8:GFX3D_TEX_RGB5A1;
        else // info.type == SPRITE_BLEND
          textype = (gfxcaps&GFX_CAP_RGBA8 && !info.bits_lowcolor )
                     ?GFX3D_TEX_RGBA8:GFX3D_TEX_RGBA4;

        if ( klass.use_shared_texture_ )
        {
          Texture* texture = texset_.Get(+info.bitmap,textype);
          if ( !texture || texture->IsLost() ) {
            //XMessageW((L"creating shared texture %s/%08x",info.bitmap.Str(),textype));
            Lout << _S*"sprites: creating shared texture [%s,%.0fx%.0f +%d,%ls]"
              %info.bitmap
              %tex->width_
              %tex->height_
              %info.mipmap
              %GFX3D_TEXTYPE_TO_STRING(textype);
            texture = texset_.Put(
              +info.bitmap,textype,
              gfx->CreateDirtyTexture(
                (int)tex->width_,(int)tex->height_,(int)textype,info.mipmap));
            fail_if ( !texture );
            load_texture = true;
            tex_created = true;
          }
          tex->image_.reassign__(texture);
        }
        else
        {
          Lout << _S*"sprites: creating texture object %d [%s, %.0fx%.0f +%d, %ls]"
            %(tex-&klass.textures_[0])
            %info.bitmap
            %tex->width_ %tex->height_
            %info.mipmap
            %GFX3D_TEXTYPE_TO_STRING(textype);
          load_texture = true;
          fail_if_fail(tex->image_ =
            AutoRef(gfx->CreateDirtyTexture(
              (int)tex->width_,(int)tex->height_,(int)textype,
              klass.info_.mipmap)));
          tex_created = true;
        }
      }

      if ( (load_texture || update_bitmask) && !bitmap )
      {
        bitmap =
          klass.provider_
            ?AutoRef( (Surface*)new TffSurface(klass.provider_->GetBitmap()) )
            :AutoRef( (Surface*)CreateSurfaceFrom(+info.bitmap,sfmt) );
        fail_if ( !bitmap );
        if ( info.alpha.Length() != 0 && info.type != SPRITE_SOLID ) {
          // у сплошных спрайтов не бывает альфаканала
          AutoSurface alpha; // нужен для альфа-канала в отдельном файле
          alpha = AutoRef( CreateSurfaceFrom(+info.alpha,Surface::ALPHA) );
          // копируем альфаканал в основную поверхность
          Surface::CopyAlphaBits_(bitmap,alpha);
        }
        if ( info.type == SPRITE_COLORKEY )
          Surface::SetAlphaBitsByColorKey_(bitmap,info.colorkey);
      }

      if ( update_bitmask  )
      {
        long cbm_cols,cbm_rows;
        //if ( klass.info_.spr_width >= 96 ) cbm_cols = 3;
        //else
        if ( klass.info_.spr_width >= 96 /*64*/ ) cbm_cols = 2;
        else cbm_cols = 1;
        //if ( klass.info_.spr_height >= 96 ) cbm_rows = 96;
        //else
        if ( klass.info_.spr_height >= 96 /*64*/ ) cbm_rows = 64;
        else cbm_rows = 32;
        klass.cbm_cols = cbm_cols;
        klass.cbm_rows = cbm_rows;
        klass.collide_bitmask_.reset_(new u32_t[cbm_cols*cbm_rows*klass.info_.spr_no]);
        if ( klass.unisprite_ ) {
          FillCollideBitmask(
            ~klass.collide_bitmask_,
            cbm_cols,
            cbm_rows,
            bitmap,bitmap->GetRect());
        }else{
          for ( int i=0, iE = klass.info_.spr_no; i != iE; ++i ) {
            FillCollideBitmask(
              ~klass.collide_bitmask_+cbm_cols*cbm_rows*i,
              cbm_cols,
              cbm_rows,
              bitmap,
              klass.fr_bitmap_[i]);
          }
        }
      }

      if ( load_texture ) {
        if ( pal8 )
        {
          BufferT<mrgba_t> pal;
          bitmap->GetPal(pal);
          fail_if_err(tex->image_->LoadPalette(pal));
        }

        if ( klass.use_shared_texture_ )
        {
          if ( !tex_created )
            Lout << _S*"sprites: filling shared texture [%s,%.0fx%.0f]"
              %info.bitmap %tex->width_ %tex->height_ %(tex-&klass.textures_[0]);
          if ( klass.is_scaled_ )
          {
            AutoSurface s ( GenericSurface::Create(
              (int)tex->width_,(int)tex->height_,Surface::SYS_RGBx32) );
            fail_if_err ( Surface::StretchBlt_(s,s->GetRect(),bitmap,bitmap->GetRect()) );
            bitmap = s;
          }
          fail_if_err ( tex->image_->Load(bitmap,0) );
          if ( info.mipmap )
          {
            rect_t lrect = bitmap->GetRect();
            AutoSurface s ( GenericSurface::Create(
              (int)tex->width_,(int)tex->height_,Surface::SYS_RGBx32) );
            for ( unsigned i = 1; i < info.mipmap; ++i )
            {
              Lout << _S*"sprites: filling mipmap level %d of texture object %d [%s,%.0fx%.0f]"
                %i
                %(tex-&klass.textures_[0])
                %info.bitmap %tex->width_ %tex->height_ %(tex-&klass.textures_[0]);
              rect_t rect(0,0,lrect.right>>i,lrect.bottom>>i);
              fail_if_err ( Surface::StretchBlt_(s,rect,bitmap,lrect) );
              fail_if_err ( tex->image_->LoadSubpicture(s,0,0,rect,i) );
            }
          }
        }
        else
        {
          if ( !tex_created && !klass.provider_ )
            Lout << _S*"sprites: filling texture object %d [%s,%.0fx%.0f +%d]"
              %(tex-&klass.textures_[0])
              %info.bitmap %tex->width_ %tex->height_ %info.mipmap;
          SPRITE_Rect* layout = tex->layout_EX_.Begin();
          SPRITE_Rect* layoutE = tex->layout_EX_.End();
          if ( klass.unisprite_ ) layoutE = layout+1;
          for ( ;layout != layoutE; ++layout ) {
            if ( klass.is_scaled_ ) {
              int lwidth  = teggo_min<int>(layout->rect.Width(),(int)tex->width_);
              int lheight = teggo_min<int>(layout->rect.Height(),(int)tex->height_);
              rect_t rect(0,0,lwidth,lheight);
              AutoSurface s ( GenericSurface::Create(lwidth,lheight,Surface::SYS_RGBx32) );
              fail_if_err ( Surface::StretchBlt_(s,rect,bitmap,layout->rect) );
              fail_if_err ( tex->image_->LoadSubpicture(s,layout->x,layout->y,rect) );
            }
            else
            {
//               Lout << _S*"loading: %ls: %d,%d <- %d,%d,%d,%d"
//                %info.bitmap.Str()
//                %layout->x %layout->y
//                %layout->rect.left %layout->rect.top
//                %layout->rect.right %layout->rect.bottom;
              fail_if_err ( tex->image_->LoadSubpicture(bitmap,layout->x,layout->y,layout->rect) );
            }
            if ( info.mipmap > 1 )
            {
              unsigned d = 2;
              int lwidth  = teggo_min<int>(layout->rect.Width(),(int)tex->width_);
              int lheight = teggo_min<int>(layout->rect.Height(),(int)tex->height_);
              AutoSurface s ( GenericSurface::Create(lwidth,lheight,Surface::SYS_RGBx32) );
              for ( unsigned i = 1; i < info.mipmap; ++i, d *= 2 )
              {
                _S*"sprites: filling mipmap level %d of texture object %d [%s,%.0fx%.0f]"
                  %i
                  %(tex-&klass.textures_[0])
                  %info.bitmap %tex->width_%tex->height_ %(tex-&klass.textures_[0]);
                rect_t rect(0,0,lwidth/d,lheight/d);
                fail_if_err ( Surface::StretchBlt_(s,rect,bitmap,layout->rect) );
                fail_if_err ( tex->image_->LoadSubpicture(s,layout->x/d,layout->y/d,rect,i) );
              }
            }
          }
        }
      }
    }
  }
  klass.need_update_ = false;
  klass.is_unloaded_ = false;
  return SCE_OK;
}
