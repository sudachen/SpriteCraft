
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
#include "m_loadsurf.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

class TAFImageDataSource : public ImageDataSource {
  u32_t img_width_;
  u32_t img_height_;
  long frames_;
  BufferT<byte_t> rowbits_;
  int row_no_;
  int fr_no_;
  bool info_loaded_;
  BufferT<byte_t> alpha_;
  BufferT<byte_t> color_;
  long alpha_idx_;
  long color_idx_;
  long left_,right_,top_,bottom_;
  u16_t encoding_flags_;
public:
  virtual SCERROR ReadPicInfo (ImageInfo&);
  virtual SCERROR ReadRow(BufferT<byte_t>& row);
  TAFImageDataSource(DataSource* ds) : ImageDataSource(ds), info_loaded_(false) {}
  virtual ~TAFImageDataSource() {}
};

SCERROR TAFImageDataSource::ReadPicInfo (ImageInfo& info) {
  if ( !info_loaded_ ) {
    byte_t signature[4] = {0};
    long foo;
    Read(signature,4);
    if ( memcmp(signature,"TAF1",4) != 0)
      fail_because("tafstream: invalid TAF signature");
    foo = ReadByte(); // comment
    Skip(foo);
    img_height_ = ReadShort();
    img_width_  = ReadShort();
    frames_     = ReadByte();
    reterr_if ( eof2err( this->PeekError() , SCE_BAD_STREAM ) );
    info_loaded_  = true;
  }

  info.width   = img_width_;
  info.height  = img_height_ * frames_;
  info.updown  = true;
  info.stride  = img_width_*4;
  info.bpp     = 32;
  info.palsize = 0; 
  info.shortgreen = false;
  info.alphabits  = true;
  info.pal.clear();
  info.frames  = frames_;

  fr_no_  = -1;
  row_no_ = img_height_;

  return SCE_OK;
}

SCERROR TAFImageDataSource::ReadRow(BufferT<byte_t>& row) {
  if ( !info_loaded_ ) {
    ImageInfo i;
    fail_if_fail(ReadPicInfo(i));
  }
  reterr_if ( eof2ok(this->PeekError()) );
  if ( row_no_ == img_height_ ) {
    if ( fr_no_ == frames_-1 ) return SCE_EOF;
    byte_t signature[4];
    Read(signature,4);
    if ( memcmp(signature,"PICT",4) != 0 )
      fail_because("tafstream: invalid PICT signature");
    encoding_flags_ = ReadShort();
    left_     = ReadShort();
    top_      = ReadShort();
    right_    = ReadShort();
    bottom_   = ReadShort();
    u32_t  alpha_len = ReadLong();
    u32_t  color_len = ReadLong();
    alpha_.resize(alpha_len);
    color_.resize(color_len);
    alpha_idx_ = 0;
    color_idx_ = 0;
    Read(&alpha_[0],alpha_.size());
    Read(&color_[0],color_.size());
    ++fr_no_;
    row_no_ = 0;
  }
  reterr_if ( eof2ok(this->PeekError()) );
  row.resize(img_width_*4);
  memset(&row[0],0,img_width_*4);
  if ( row_no_ >= top_ && row_no_ < bottom_ ) {
    long count = 0;
    while ( count < right_-left_ ) {
      REQUIRE ( alpha_idx_ < alpha_.size() );
      byte_t a = alpha_[alpha_idx_++];
      byte_t aa = 0;
      switch (a>>6){
        case 1: aa = 64; break;
        case 2: aa = 127; break;
        case 3: aa = 255; break;
      }
      a &= 0x3f;
      if ( aa != 0 ) {
        REQUIRE ( count + a <= right_-left_ );
        for ( int i = 0; i < a; ++i ) {
          u16_t colur = u16_t(color_[color_idx_++]) << 8;
          colur |= u16_t(color_[color_idx_++]);
          mrgba_t rgba = c565_to_mrgba(colur);
          rgba.A = aa;
          if (left_+count+i < img_width_)
            ((mrgba_t*)&row[0])[left_+count+i] = rgba;
        }
      }
      count += a;
    }
  }
  ++row_no_;
  return this->PeekError();
}

ImageDataSource* MOLE_FUNCALL MakeTAFImageDataSource(DataSource* ds) {
  return new TAFImageDataSource(Grab(ds));
}

NAMESPACE_MSKIN_END
