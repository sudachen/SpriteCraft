
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
#include "m_loadsurf.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

class TGAImageDataSource : public ImageDataSource {
  u32_t img_width_;
  u32_t img_height_;
  u32_t pixel_depth_;
  BufferT<byte_t> rowbits_;
  int stride_;
  int row_no_;
  bool info_loaded_;
  bool updown_;
  bool leftright_;
public:
  virtual SCERROR ReadPicInfo (ImageInfo&);
  virtual SCERROR ReadRow(BufferT<byte_t>& row);
  TGAImageDataSource(DataSource* ds) : ImageDataSource(ds) , stride_(0), row_no_(0), info_loaded_(false) {}
  virtual ~TGAImageDataSource() {}
};

SCERROR TGAImageDataSource::ReadPicInfo (ImageInfo& info) {
  if ( !info_loaded_ ) {
    byte_t id_len = ReadByte();
    /*byte_t color_map_type =*/ ReadByte();
    byte_t image_type = ReadByte();
    /*u16_t cmap_start =*/ ReadShort();
    u16_t cmap_length = ReadShort();
    /*byte_t cmap_depth =*/ ReadByte();
    /*u16_t x_offset =*/ ReadShort();
    /*u16_t y_offset =*/ ReadShort();
    //updown_ = y_offset == 0;
    //leftright_ = x_offset == 0;
    img_width_ = ReadShort();
    img_height_ = ReadShort();
    pixel_depth_ = ReadByte();
    byte_t image_descriptor = ReadByte();
    REQUIRE ( WasRead() == 0x12 );
    fail_if_fail ( PeekError() );
    if ( image_type != 2 )
      fail_because("tgastream: usupported targa format");

    if ( pixel_depth_ == 32 ) {
      stride_     = img_width_*4;
    } else if ( pixel_depth_ == 24 ) {
      stride_     = img_width_*3;
    } else fail_because("tgastream: usupported targa bitdepth");
    row_no_     = 0;
    info_loaded_ = true;
    
    leftright_ = (image_descriptor & 0x010) == 0;
    updown_ = (image_descriptor & 0x020) == 0;

    this->Skip(id_len+cmap_length); // пропустить все до пикселей изображения
    reterr_if ( eof2err( this->PeekError() , SCE_BAD_STREAM ) );
  }

  info.width   = img_width_;
  info.height  = img_height_;
  info.updown  = !updown_;
  info.stride  = stride_;
  info.bpp     = pixel_depth_;
  info.palsize = 0; 
  info.shortgreen = false;
  info.alphabits  = pixel_depth_ == 32;
  info.pal.clear();

  return SCE_OK;
}

SCERROR TGAImageDataSource::ReadRow(BufferT<byte_t>& row) {
  retval_if ( row_no_ >= signed(img_height_) , SCE_EOF ); 
  reterr_if ( this->PeekError() );
  row.resize(stride_);
  this->Read(&row[0],stride_);
  if ( pixel_depth_ == 32 ) {
    mbgra_t* bgra = (mbgra_t*)&row[0];
    mbgra_t* bgraE = bgra+img_width_;
    mrgba_t* rgba = (mrgba_t*)bgra;
    if ( leftright_  ) 
      while ( bgra != bgraE ) *rgba++ = xxx_to_mrgba(*bgra++);
    else {
      while ( bgra != bgraE ) *rgba++ = xxx_to_mrgba(*--bgraE);
    }
  }
  else{
    mbgr_t* bgr = (mbgr_t*)&row[0];
    mbgr_t* bgrE = bgr+img_width_;
    mrgb_t* rgb = (mrgb_t*)bgr;
    if ( leftright_ ) 
      while ( bgr != bgrE ) *rgb++ = xxx_to_mrgb(*bgr++);
    else {
      while ( bgr != bgrE ) *rgb++ = xxx_to_mrgb(*--bgrE);
    }
  }
  ++row_no_;
  return this->PeekError();
}

ImageDataSource* MOLE_FUNCALL MakeTGAImageDataSource(DataSource* ds) {
  return new TGAImageDataSource(Grab(ds));
}

NAMESPACE_MSKIN_END
