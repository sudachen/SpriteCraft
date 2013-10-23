
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

extern "C" {
#include "../../Media/lib.png/png.h"
}


#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

#define __PNG_SYNC__
//static char png_error_text[256] = "unknown PNGlib error";

class PNGImageDataSource : public ImageDataSource {
  u32_t img_width_;
  u32_t img_height_;
  BufferT<byte_t> rowbits_;
  int stride_;
  int row_no_;
  bool info_loaded_;
  bool updown_;
  bool leftright_;
  bool alphabits_;
	png_struct* png_ptr;
  png_info*   info_ptr;
  long bpp_;

  SCERROR ReadNextRow(byte_t* row);
  SCERROR ReadHeader();
public:
  virtual SCERROR ReadPicInfo (ImageInfo&);
  virtual SCERROR ReadRow(BufferT<byte_t>& row);
  PNGImageDataSource(DataSource* ds) : ImageDataSource(ds) , stride_(0), row_no_(0), info_loaded_(false) {
  	png_ptr  = 0;
    info_ptr = 0;
  }
  virtual ~PNGImageDataSource() {
    __PNG_SYNC__;
    if ( png_ptr ) {
      if(setjmp(png_jmpbuf(png_ptr))) {
        Lout << "jpgstream: error in jpeg_dtor";
        return;
      }
      png_destroy_read_struct(&png_ptr,&info_ptr,0);
    }
  }
};

SCERROR PNGImageDataSource::ReadPicInfo (ImageInfo& info) {
  if ( !info_loaded_ ) {
    __PNG_SYNC__;
    fail_if_fail(ReadHeader());
  }

  info.width   = img_width_;
  info.height  = img_height_;
  info.updown  = updown_;
  info.stride  = stride_;
  info.bpp     = bpp_;
  info.palsize = 0; 
  info.shortgreen = false;
  info.alphabits  = alphabits_;
  info.pal.clear();

  return SCE_OK;
}

static void __cdecl mskinpng_read_fn(png_structp png_ptr, png_bytep dest, png_size_t sz)
{
  ((PNGImageDataSource*)png_ptr->io_ptr)->Read(dest,sz);
}

SCERROR PNGImageDataSource::ReadHeader()
{
	unsigned char signature[8];
  fail_if ( this->Read(signature,8) != 8 );
	if ( !png_check_sig(signature,8) ) fail_because( _S*":PNGImageDataSource: %? is no PNG image" %GetName() );
	fail_if ( !(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0)));
	fail_if ( !(info_ptr=png_create_info_struct(png_ptr)) );
  if(setjmp(png_jmpbuf(png_ptr))) {
    Lout << "jpgstream: error in jpeg_ReadHeader";
    return SCE_FAIL;
  }

	png_set_read_fn(png_ptr,this,mskinpng_read_fn);
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

  stride_  = (bppX*img_width_ + 7 ) & ~3UL;
  bpp_ = bppX * 8;
  return SCE_OK;
}

SCERROR PNGImageDataSource::ReadRow(BufferT<byte_t>& row) 
{
  reterr_if ( eof2ok(this->PeekError()) );
  retval_if ( row_no_ >= signed(img_height_) , SCE_EOF ); 
  row.resize(stride_);
  fail_if( !png_ptr || !info_ptr);
  {
    __PNG_SYNC__;
    if(setjmp(png_jmpbuf(png_ptr))) {
      Lout << "pngstream: error in png_ReadRow";
      return SCE_FAIL;
    }
    png_read_row(png_ptr,&row[0],0);
  }
  
  if ( ++row_no_ == signed(img_height_) ) {
    __PNG_SYNC__;
    if(setjmp(png_jmpbuf(png_ptr))) {
      Lout << "pngstream: error in png_ReadRow";
      return SCE_FAIL;
    }
    png_read_end(png_ptr,info_ptr);
  }
  return this->PeekError();
}

ImageDataSource* MOLE_FUNCALL MakePNGImageDataSource(DataSource* ds) {
  return new PNGImageDataSource(Grab(ds));
}

NAMESPACE_MSKIN_END
