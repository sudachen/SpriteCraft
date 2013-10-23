
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

#include "tffmedia.h"
#pragma code_seg(".text$tffmedia")

struct TFFt_PICTURE_TGA : TFFt_PICTURE
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
    u16_t pixel_depth_;

    TFFt_CopyRowXtoX copyf8_;

    TFFt_PICTURE_TGA(rcc_ptr<DataStream> const &ds);
    virtual ~TFFt_PICTURE_TGA();
    
    bool ReadPicInfo();

    virtual TFFMt_E GetInfo(TFFt_PICTURE::INFO *);
    virtual TFFMt_E NextRow8(rgba8_t *row,int shift,int cnt);
  };

TFFt_PICTURE_TGA::TFFt_PICTURE_TGA(rcc_ptr<DataStream> const &ds) 
  : is_(ds), info_loaded_(false)
  {
  }

TFFt_PICTURE_TGA::~TFFt_PICTURE_TGA()
  {
  }

bool TFFt_PICTURE_TGA::ReadPicInfo()
  {
    byte_t id_len = is_->Read8();
    /*byte_t color_map_type =*/ is_->Read8();
    byte_t image_type = is_->Read8();
    /*u16_t cmap_start =*/ is_->Read16le();
    u16_t cmap_length = is_->Read16le();
    /*byte_t cmap_depth =*/ is_->Read8();
    /*u16_t x_offset =*/ is_->Read16le();
    /*u16_t y_offset =*/ is_->Read16le();
    img_width_ = is_->Read16le();
    img_height_ = is_->Read16le();
    pixel_depth_ = is_->Read8();
    byte_t image_descriptor = is_->Read8();
    REQUIRE ( is_->Tell() == 0x12 );
    if ( image_type != 2 )
      {
        Lerr << "[MEDIA ERROR] failed to read picture info, bad targa format";
        return TFFM_FAIL;
      }

    if ( pixel_depth_ == 32 )
      stride_ = img_width_*4;
    else if ( pixel_depth_ == 24 )
      stride_ = img_width_*3;
    else 
      {
        Lerr << "[MEDIA ERROR] failed to read picture info, unsupported targa bitdepth";
        return TFFM_FAIL;
      }

    row_no_     = 0;
    alphabits_  = pixel_depth_ == 32;
    info_loaded_ = true;
    
    leftright_ = (image_descriptor & 0x010) == 0;
    updown_ = (image_descriptor & 0x020) != 0;

    is_->Seek(id_len+cmap_length + is_->Tell());
    copyf8_ = TffmCopyRowXtoX(pixel_depth_,TFF_PF_RGBA8);
    return true;
  }

TFFMt_E TFFt_PICTURE_TGA::GetInfo(TFFt_PICTURE::INFO *picnfo)
  {
    if ( info_loaded_ )
      {
        picnfo->alphabits = this->alphabits_;
        picnfo->bpp       = pixel_depth_;
        picnfo->frames    = 1;
        picnfo->height    = img_height_;
        picnfo->width     = img_width_;
        picnfo->palsize   = 0;
        picnfo->updown    = updown_;
        return TFFM_OK;
      }
    else
      return TFFM_FAIL;
  }

TFFMt_E TFFt_PICTURE_TGA::NextRow8(rgba8_t *row,int shift,int cnt)
  {
    row_.Resize(stride_);
    if ( is_->Read(&row_[0],stride_) != stride_ )
      return TFFM_FAIL;
    if ( cnt+shift > img_width_ )
      return TFFM_BADARGUMENT;
    
    if ( !leftright_ )
      {
        int w = pixel_depth_/8;
        byte_t *i = &row_[0];
        byte_t *k = &row_[0] + w*(img_width_-1);
        for ( ;i < k; i += w, k -= w )
          _swab((char*)i,(char*)k,w);
      }

    byte_t *p = &row_[0]+shift*pixel_depth_/8;    
    copyf8_(p,row,cnt);

    return TFFM_OK;
  }

mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmTgaCreatePicFrom( rcc_ptr<DataStream> const &ds )
  { 
    mem_ptr<TFFt_PICTURE_TGA> bmp = MemPtr( new TFFt_PICTURE_TGA(ds));
    if ( bmp->ReadPicInfo() )
      return mem_ptr<TFFt_PICTURE>(bmp.forget_());
    else
      return mem_ptr<TFFt_PICTURE>(0);
  }


TFFMt_E CXX_STDCALL TffmWriteTgaBits8( rcc_ptr<DataStream> const &ds, rgba8_t *img, int width, int height)
  {
    ds->Write8(0); // id_len
    ds->Write8(0); /*byte_t color_map_type =*/
    ds->Write8(2); // image_type
    ds->Write16le(0);/*u16_t cmap_start =*/ 
    ds->Write16le(0);//u16_t cmap_length = 
    ds->Write8(0);  /*byte_t cmap_depth =*/ 
    ds->Write16le(0); /*u16_t x_offset =*/ 
    ds->Write16le(0); /*u16_t y_offset =*/
    ds->Write16le(width); //img_width_ = is_->Read16le();
    ds->Write16le(height); //img_height_ = is_->Read16le();
    ds->Write8(32); //pixel_depth_ = is_->Read8();
    ds->Write8(0x020); //byte_t image_descriptor = is_->Read8();
       
    for ( int i = 0; i < height; ++i )
      {
        ds->Write( (byte_t*)(img + width*i), width*4 );
      }

    ds->Flush();
    return TFFM_OK;
  }

extern "C" TFFMt_E CXX_STDCALL TffmWriteTgaBits8(pwide_t fname, rgba8_t *img, int width, int height)
  {
    //DeleteFile(fname);
    _wunlink(fname);
    if ( rcc_ptr<DataStream> ds = teggo::OpenFile( fname, FILEOPEN_WRITE|FILEOPEN_CREATE ) )
      {
        TffmWriteTgaBits8(ds,img,width,height);
        return TFFM_OK;
      }
    else
      return TFFM_NOFILE;
  }
