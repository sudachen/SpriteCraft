
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

#define x_BI_BITFIELDS  3L
#define x_BI_RGB        0L

struct x_BITMAPFILEHEADER 
{
  u16_t  bfType;
  u32_t   bfSize;
  u16_t  bfReserved1;
  u16_t  bfReserved2;
  u32_t   bfOffBits;
  void Read(InputStream& is){
    bfType      = is.ReadShort();
    bfSize      = is.ReadLong();
    bfReserved1 = is.ReadShort();
    bfReserved2 = is.ReadShort();
    bfOffBits   = is.ReadLong();
  }
};

struct x_BITMAPINFOHEADER
{
  u32_t   biSize;
  long      biWidth;
  long      biHeight;
  u16_t  biPlanes;
  u16_t  biBitCount;
  u32_t   biCompression;
  u32_t   biSizeImage;
  long      biXPelsPerMeter;
  long      biYPelsPerMeter;
  u32_t   biClrUsed;
  u32_t   biClrImportant;
  void Read(InputStream& is)
  {
    biSize          = is.ReadLong();
    biWidth         = is.ReadLong();
    biHeight        = is.ReadLong();
    biPlanes        = is.ReadShort();
    biBitCount      = is.ReadShort();
    biCompression   = is.ReadLong();
    biSizeImage     = is.ReadLong();
    biXPelsPerMeter = is.ReadLong();
    biYPelsPerMeter = is.ReadLong();
    biClrUsed       = is.ReadLong();
    biClrImportant  = is.ReadLong();
  }
};
  
class BMPImageDataSource : public ImageDataSource {
  x_BITMAPFILEHEADER bmFH_;
  x_BITMAPINFOHEADER pDIB_;
  BufferT<mrgba_t>   quad_;
  BufferT<byte_t>    rowbits_;
  int stride_;
  int row_no_;
  bool info_loaded_;
  bool shortgreen_;
  bool alphabits_;
public:
  virtual SCERROR ReadPicInfo (ImageInfo&);
  virtual SCERROR ReadRow(BufferT<byte_t>& row);
  BMPImageDataSource(DataSource* ds) : ImageDataSource(ds) , stride_(0), row_no_(0), info_loaded_(false) {}
  virtual ~BMPImageDataSource() {}
};

SCERROR BMPImageDataSource::ReadPicInfo (ImageInfo& info) {
  if ( !info_loaded_ ) {
    bmFH_.Read(*this); reterr_if ( eof2err ( this->PeekError() , SCE_BAD_STREAM ) );
    pDIB_.Read(*this); reterr_if ( eof2err ( this->PeekError() , SCE_BAD_STREAM ) );

    if ( pDIB_.biBitCount == 8 ) {
      this->Skip(pDIB_.biSize + 0xe - this->WasRead()); // пропустить все до политры
      BufferT<mbgra_t> quad(256);
      if ( pDIB_.biClrUsed == 0 ) pDIB_.biClrUsed = 256;
      this->Read((byte_t*)&quad[0],4*pDIB_.biClrUsed);
      quad_.resize(256,mRGBA_t(0,0,0));
      for ( unsigned i = 0; i < pDIB_.biClrUsed; ++i ) {
        quad_[i] = xxx_to_mrgba(quad[i]);
        quad_[i].A = 255;
      }

    }
  
    if ( bmFH_.bfType != 0x4D42 ) {
      Lerr << _S*"bmpstream: invalid content of %s" %this->GetName();
      return SCE_BAD_IMAGE;
    }
    
    alphabits_  = false;
    shortgreen_ = false;
    if ( pDIB_.biBitCount == 32 ) {
      stride_ = ((signed)(pDIB_.biWidth)*4+3)&~0x03U;
      alphabits_ = true;
    }else if (pDIB_.biBitCount == 24 ) {
      stride_ = ((signed)(pDIB_.biWidth)*3+3)&~0x03U;
    }else if (pDIB_.biBitCount == 16 ) {
      stride_ = ((signed)(pDIB_.biWidth)*2+3)&~0x03U;
      if( pDIB_.biCompression == x_BI_BITFIELDS ) {
        this->ReadLong();
        u16_t green_field = u16_t(this->ReadLong());
        if ( green_field == 0x03e0 ) {
          shortgreen_ = alphabits_ = true;
        }
        this->ReadLong();
      }else alphabits_ = shortgreen_ = true;
    }else if (pDIB_.biBitCount == 8 ) {
      stride_ = ((signed)(pDIB_.biWidth)+3)&~0x03U;
    }
    info_loaded_ = true;

    this->Skip(bmFH_.bfOffBits-this->WasRead()); // пропустить все до пикселей изображения
    reterr_if ( eof2err( this->PeekError() , SCE_BAD_STREAM ) );

  }

  info.width   = pDIB_.biWidth;
  info.height  = abs(pDIB_.biHeight);
  info.updown  = pDIB_.biHeight < 0 ;
  info.stride  = stride_;
  info.bpp     = pDIB_.biBitCount;
  info.palsize = pDIB_.biClrUsed; 
  info.shortgreen = shortgreen_;
  info.alphabits  = alphabits_;
  info.pal.resize(quad_.size());
  teggo::copy(quad_.begin(),quad_.end(),info.pal.begin());

  return SCE_OK;
}

SCERROR BMPImageDataSource::ReadRow(BufferT<byte_t>& row) {
  reterr_if ( this->PeekError() );
  retval_if ( row_no_ >= pDIB_.biHeight , SCE_EOF ); 
  row.resize(stride_);
  this->Read(&row[0],stride_);
  if ( pDIB_.biBitCount == 32 ) {
    mbgra_t* bgra = (mbgra_t*)&row[0];
    mbgra_t* const bgraE = bgra+pDIB_.biWidth;
    mrgba_t* rgba = (mrgba_t*)bgra;
    while ( bgra != bgraE ) *rgba++ = xxx_to_mrgba(*bgra++);
  }else if ( pDIB_.biBitCount == 24 ) {
    mbgr_t* bgr = (mbgr_t*)&row[0];
    mbgr_t* const bgrE = bgr+pDIB_.biWidth;
    mrgb_t* rgb = (mrgb_t*)bgr;
    while ( bgr != bgrE ) *rgb++ = xxx_to_mrgb(*bgr++);
  }
  ++row_no_;
  return this->PeekError();
}

ImageDataSource* MOLE_FUNCALL MakeBMPImageDataSource(DataSource* ds) {
  return new BMPImageDataSource(Grab(ds));
}

NAMESPACE_MSKIN_END
