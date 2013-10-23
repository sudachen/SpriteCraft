
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
#pragma code_seg(".text$flat$layout")
#endif

void LayoutOneFrameSprite(KlassOfSprite3D& klass, unsigned max_texture)
  {
    SPRITE_Info& info = klass.info_;
    unsigned max_texture_2 = max_texture-2;
    klass.unisprite_ = true;
    klass.fr_bitmap_.Push(rect_t(0,0,info.bitmap_width,info.bitmap_height));
    unsigned tex_no_hor = (info.bitmap_width+max_texture_2-1)/max_texture_2;
    unsigned tex_no_ver = (info.bitmap_height+max_texture_2-1)/max_texture_2;
    unsigned tex_no =  tex_no_hor * tex_no_ver;
    int processed = 0;
    klass.textures_.Reserve(tex_no);
    BufferT<SPRITE_fRect>().Swap(klass.spr_layout_);
    BufferT<u32_t>().Swap(klass.fr_mapping_);
    for ( int j = 0, jE = tex_no_ver>1?tex_no_ver:1 ; j < jE; ++j )
      for ( int i = 0, iE = tex_no_hor>1?tex_no_hor:1 ; i < iE; ++i )
        {
          SPRITE_TexInfo texinfo;
          // размер текстуры
          u32_t img_w = teggo_min<long>(info.bitmap_width-i*max_texture_2,max_texture_2);
          u32_t img_h = teggo_min<long>(info.bitmap_height-j*max_texture_2,max_texture_2);
          texinfo.width_  = RoundPow2(img_w);
          texinfo.height_ = RoundPow2(img_h);
          u32_t rx = i*max_texture_2;
          u32_t ry = j*max_texture_2;
          SPRITE_Rect tlayout;
          tlayout.rect = rect_t(rx?rx-1:0,ry?ry-1:0,
            rx+img_w<info.bitmap_width?rx+img_w+1:rx+img_w,
            ry+img_h<info.bitmap_height?ry+img_h+1:ry+img_h);
          tlayout.x = rx?0:1;
          tlayout.y = ry?0:1;
          // первый лейоут содержит данные по загрузке текстуры
          texinfo.layout_EX_.Push(tlayout);
          tlayout.rect.left = rx;
          tlayout.rect.top = ry;
          tlayout.rect.right = img_w;
          tlayout.rect.bottom = img_h;
          // второй лейоут содержит данные по отрисовке текстуры
          texinfo.layout_EX_.Push(tlayout);
          klass.textures_.Push(texinfo);
          SPRITE_fRect frect = {
            1.0f/texinfo.width_,
            1.0f/texinfo.height_,
            float(teggo_min<long>(texinfo.width_,img_w+1))/texinfo.width_,
            float(teggo_min<long>(texinfo.height_,img_h+1))/texinfo.height_ };
          klass.spr_layout_.Push(frect);
        }
  }

void LayoutMultiFrameSprite(KlassOfSprite3D& klass, unsigned max_texture)
  {
    SPRITE_Info& info = klass.info_;
    klass.unisprite_ = false;
    bool big_sprite = false;
    if ( info.spr_height > max_texture || info.spr_width > max_texture )
      {
        Xdbg( Ldbg << _S*"maximum sprite frame size %dx%d, but sprite %s has frame %dx%d"
                %max_texture %max_texture
                %info.bitmap
                %info.spr_width %info.spr_height );
        big_sprite = true;
        klass.is_scaled_ = true;
      }
    // количество кадров по горизонтали
    const int spr_hor_no = info.bitmap_width/info.spr_width;
    // считаем количество кадров в одной текстуре
    const int fr_no_hor = big_sprite?1:max_texture/info.spr_width;
    const int fr_no_ver = big_sprite?1:max_texture/info.spr_height;
    const int fr_no_tex = big_sprite?1:fr_no_hor*fr_no_ver;
    // количество текстур
    const int tex_no    = big_sprite?info.spr_no:(info.spr_no+(fr_no_tex-1))/fr_no_tex;
    // размер текстуры
    //const int tex_width  = big_sprite?max_texture:RoundPow2(fr_no_hor*info.spr_width);
    //const int tex_height = big_sprite?max_texture:RoundPow2(fr_no_ver*info.spr_height);
    const int tex_width  = teggo_min<int>(max_texture,RoundPow2(fr_no_hor*info.spr_width));
    const int tex_height = teggo_min<int>(max_texture,RoundPow2(fr_no_ver*info.spr_height));
    Xdbg( Lout << _S*"Assume '%s' tex{%dx%d}:%d, spr {%dx%d}:%d"
                %info.bitmap
                %tex_width %tex_height %tex_no
                %info.spr_width %info.spr_height %info.spr_no );
    u32_t processed = 0;
    u32_t tprocessed = 0;
    klass.textures_.reserve(tex_no);
    // отображение спрайтов на текстуру
    BufferT<u32_t>().swap(klass.fr_mapping_);
    klass.spr_layout_.reserve(klass.info_.spr_no);
    klass.spr_layout_.clear();
    while ( processed < info.spr_no )
      {
        int tspr_no = teggo_min<int>(info.spr_no-processed,fr_no_tex);
        // размещаем все спрайты класса на одной текстуре
        SPRITE_TexInfo texinfo;
        // размер текстуры
        texinfo.width_  = tex_width;
        texinfo.height_ = tex_height;
        // разметка текстуры
        for ( int ti = 0; ti < tspr_no; ++ti )
          {
            SPRITE_Rect tlayout;
            int rx,ry;
            rx = ((processed+ti)%spr_hor_no)*info.spr_width;
            ry = ((processed+ti)/spr_hor_no)*info.spr_height;
            tlayout.rect = rect_t(
              rx,ry,rx+info.spr_width,ry+info.spr_height);
            REQUIRE (tlayout.rect.right <= long(info.bitmap_width));
            REQUIRE (tlayout.rect.bottom <= long(info.bitmap_height));
            tlayout.x = (ti%fr_no_hor)*info.spr_width;
            tlayout.y = (ti/fr_no_hor)*info.spr_height;
            texinfo.layout_EX_.push_back(tlayout);
            klass.fr_bitmap_.push_back(tlayout.rect);
          }

        if ( big_sprite )
          {
            SPRITE_fRect frect = { 0,0,
              float(teggo_min<int>(tex_width,info.spr_width))/float(tex_width),
              float(teggo_min<int>(tex_height,info.spr_height))/float(tex_height) };
            klass.spr_layout_.push_back(frect);
            klass.fr_mapping_.push_back(tprocessed);
          }
        else
          for ( int si = 0 ; si < tspr_no; ++si )
            {
              int x = (si%fr_no_hor)*info.spr_width;
              int y = (si/fr_no_hor)*info.spr_height;
              SPRITE_fRect frect =
                {
                  float(x)/float(texinfo.width_),
                  float(y)/float(texinfo.height_),
                  float(x+info.spr_width)/float(texinfo.width_),
                  float(y+info.spr_height)/float(texinfo.height_)
                };
              klass.spr_layout_.push_back(frect);
              klass.fr_mapping_.push_back(tprocessed);
            }
        klass.textures_.push_back(texinfo);
        processed += tspr_no;
        ++tprocessed;
      }
  }

void LayoutSmallSprite(KlassOfSprite3D& klass, unsigned max_texture)
  {
    SPRITE_Info& info = klass.info_;

    klass.use_shared_texture_ = true;
    klass.unisprite_ = false;
    // размещаем все спрайты класса на одной текстуре
    klass.textures_.resize(1);
    SPRITE_TexInfo& texinfo = klass.textures_.front();
    // размер текстуры
    texinfo.width_  = teggo_min<long>(RoundPow2(info.bitmap_width),max_texture);
    texinfo.height_ = teggo_min<long>(RoundPow2(info.bitmap_height),max_texture);
    texinfo.layout_EX_.resize(1);
    // разметка текстуры
    SPRITE_Rect& tlayout =  texinfo.layout_EX_.front();
    tlayout.rect = rect_t ( 0,0, info.bitmap_width, info.bitmap_height );
    tlayout.x = tlayout.y = 0;
    // отображение спрайтов на текстуру
    BufferT<u32_t>().swap(klass.fr_mapping_);
    klass.spr_layout_.clear();
    klass.spr_layout_.reserve(klass.info_.spr_no);
    // для каждого спрайта, вычислить текстурные координаты
    if ( info.is_region )
      {
        SPRITE_fRect frect =
          {
            float(info.spr_x)/float(texinfo.width_),
            float(info.spr_y)/float(texinfo.height_),
            float(info.spr_x+info.spr_width)/float(texinfo.width_),
            float(info.spr_y+info.spr_height)/float(texinfo.height_)
          };
        klass.spr_layout_.push_back(frect);
        klass.fr_bitmap_.push_back(rect_t(
          info.spr_x,
          info.spr_y,
          info.spr_x+info.spr_width,
          info.spr_y+info.spr_height));
      }
    else
      {
        for ( int j = 0, jE = info.bitmap_height; j < jE; j+=info.spr_height )
          for ( int i = 0, iE = info.bitmap_width; i < iE ; i+=info.spr_width )
            {
              SPRITE_fRect frect =
                {
                  float(i)/float(texinfo.width_),
                  float(j)/float(texinfo.height_),
                  float(i+info.spr_width)/float(texinfo.width_),
                  float(j+info.spr_height)/float(texinfo.height_)
                };
              klass.spr_layout_.push_back(frect);
              klass.fr_bitmap_.push_back(rect_t(i,j,i+info.spr_width,j+info.spr_height));
            }
      }
  }

/**
  вычисляет разметку текстур
*/
SCERROR Impl_3DSPRITES::LayoutKlass_(KlassOfSprite3D& klass)
{
  STRICT_REQUIRE ( !klass.info_.dummy_ );
  if ( klass.textures_.size() ) return SCE_OK; // класс уже размечен
  if ( klass.info_.is_font ) return SCE_OK; // не требует разметки
  unsigned max_texture = SCRAFT_MAX_TEXTURE_WIDTH;
  klass.is_scaled_ = false;
  klass.unisprite_ = false;
  SPRITE_Info& info = klass.info_;
  if (  info.bitmap_width > max_texture || info.bitmap_height > max_texture )
    {
      if ( info.spr_no == 1 && !info.compress_it )
        // спрайт имеет один кадр
        LayoutOneFrameSprite(klass,max_texture);
      else
        // спрайт имеет несколько кадров
        LayoutMultiFrameSprite(klass,max_texture);
    }
  else
    LayoutSmallSprite(klass,max_texture);
  return SCE_OK;
}
