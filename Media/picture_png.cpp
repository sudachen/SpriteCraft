
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

#if !defined _GFXNOPNG

#include "tffmedia.h"
#pragma code_seg(".text$tffmedia")

extern "C" {
#include "lib.png/png.h"
}

struct TFFt_PICTURE_PNG : TFFt_PICTURE
  {
    rcc_ptr<DataStream> is_;

    u32_t img_width_;
    u32_t img_height_;
    BufferT<byte_t> row_;
    int stride_;
    int row_no_;
    bool info_loaded_;
    bool updown_;
    bool leftright_;
    bool alphabits_;
    png_struct* png_ptr;
    png_info*   info_ptr;
    long bpp_;

    TFFt_CopyRowXtoX copyf8_;

    TFFt_PICTURE_PNG(rcc_ptr<DataStream> const &ds);
    virtual ~TFFt_PICTURE_PNG();
    
    bool ReadPicInfo();

    virtual TFFMt_E GetInfo(TFFt_PICTURE::INFO *);
    virtual TFFMt_E NextRow8(rgba8_t *row,int shift,int cnt);
  };

static void __cdecl TFF__PNGread(png_structp png_ptr, png_bytep dest, png_size_t sz)
  {
    ((DataStream*)png_ptr->io_ptr)->Read(dest,sz);
  }

TFFt_PICTURE_PNG::TFFt_PICTURE_PNG(rcc_ptr<DataStream> const &ds) 
  : is_(ds), info_loaded_(false)
  {
  }

TFFt_PICTURE_PNG::~TFFt_PICTURE_PNG()
  {
    if ( png_ptr ) 
      {
        if(setjmp(png_jmpbuf(png_ptr))) 
          Lerr << _S*"[MEIA ERROR] failed to dispose PNG decoder of '%?'" %is_->Identifier();
        png_destroy_read_struct(&png_ptr,&info_ptr,0);
      }
  }

bool TFFt_PICTURE_PNG::ReadPicInfo()
  {
    unsigned char signature[8];
    if ( is_->Read(signature,8) != 8 )
      return false;
    if ( !png_check_sig(signature,8) ) 
      {
        Lerr << _S*"[MEDIA ERROR] '%?' is no PNG image" %is_->Identifier();
        return false;
      }
    REQUIRE( png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0) );
    REQUIRE( info_ptr = png_create_info_struct(png_ptr) );
    if(setjmp(png_jmpbuf(png_ptr))) 
      {
        Lerr << "[MEDIA ERROR] failed to read png header";
        return false;
      }
    png_set_read_fn(png_ptr,+is_,&TFF__PNGread);
    png_set_sig_bytes(png_ptr,8);
    png_read_info(png_ptr,info_ptr);
    png_set_strip_16(png_ptr);
    png_set_packing(png_ptr);

    if ( png_ptr->bit_depth < 8 
      || png_ptr->color_type == PNG_COLOR_TYPE_PALETTE
      || png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS) )
      png_set_expand(png_ptr);

    png_read_update_info(png_ptr,info_ptr);
    img_width_  = png_get_image_width(png_ptr,info_ptr);
    img_height_ = png_get_image_height(png_ptr,info_ptr);
    updown_     = true;
  
    long bppX = 0;

    if ( png_ptr->color_type & PNG_COLOR_MASK_PALETTE )
      {
        if ( png_ptr->color_type & PNG_COLOR_MASK_COLOR )
          bppX = 4;
        else
          bppX = 3;
      }
    else
      {
        if ( png_ptr->color_type & PNG_COLOR_MASK_ALPHA ) bppX = 4;
        else bppX = 3;
      }

    row_no_ = 0;
    stride_  = (bppX*img_width_ + 7 ) & ~3UL;
    bpp_ = bppX * 8 | TFF_PF_BGR;
    copyf8_ = TffmCopyRowXtoX(bpp_,TFF_PF_RGBA8);
    info_loaded_ = true;
    return true;
  };

TFFMt_E TFFt_PICTURE_PNG::GetInfo(TFFt_PICTURE::INFO *picnfo)
  {
    if ( info_loaded_ )
      {
        picnfo->alphabits = this->alphabits_;
        picnfo->bpp       = this->bpp_;
        picnfo->frames    = 1;
        picnfo->height    = this->img_height_;
        picnfo->width     = this->img_width_;
        picnfo->palsize   = 0;
        picnfo->updown    = 1;
        return TFFM_OK;
      }
    else
      return TFFM_FAIL;
  }

TFFMt_E TFFt_PICTURE_PNG::NextRow8(rgba8_t *row,int shift,int cnt)
  {
    row_.Resize(stride_);

    REQUIRE( row_no_ < signed(img_height_) ); 
    REQUIRE( png_ptr && info_ptr);

    {
      if(setjmp(png_jmpbuf(png_ptr))) 
        {
          Lerr << _S*"[MEDIA ERROR] failed to read PNG row from '%?'" %is_->Identifier();
          return TFFM_FAIL;
        }
      png_read_row(png_ptr,(byte_t*)&row_[0],0);
    }
  
    if ( ++row_no_ == signed(img_height_) ) 
      {
        if(setjmp(png_jmpbuf(png_ptr))) 
          {
            Lerr << _S*"[MEDIA ERROR] failed to read PNG row from '%?'" %is_->Identifier();
            return TFFM_FAIL;
          }
        png_read_end(png_ptr,info_ptr);
      }

    byte_t *p = &row_[0]+shift*bpp_/8;
    copyf8_(p,row,cnt);

    return TFFM_OK;
  }

mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmPngCreatePicFrom( rcc_ptr<DataStream> const &ds )
  { 
    mem_ptr<TFFt_PICTURE_PNG> bmp = MemPtr( new TFFt_PICTURE_PNG(ds));
    if ( bmp->ReadPicInfo() )
      return mem_ptr<TFFt_PICTURE>(bmp.forget_());
    else
      return mem_ptr<TFFt_PICTURE>(0);
  }

#endif // _GFXNOPNG

