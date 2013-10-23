
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

#define x_BI_BITFIELDS  3L
#define x_BI_RGB        0L

struct TFFt_BITMAPFILEHEADER_
  {
    u16_t bfType;
    u32_t bfSize;
    u16_t bfReserved1;
    u16_t bfReserved2;
    u32_t bfOffBits;

    void Read(rcc_ptr<DataStream> const &is)
      {
        bfType      = is->Read16le();
        bfSize      = is->Read32le();
        bfReserved1 = is->Read16le();
        bfReserved2 = is->Read16le();
        bfOffBits   = is->Read32le();
      }

    void Write(rcc_ptr<DataStream> const &is)
      {
        is->Write16le(bfType);
        is->Write32le(bfSize);
        is->Write16le(bfReserved1);
        is->Write16le(bfReserved2);
        is->Write32le(bfOffBits);
      }
  };

struct TFFt_BITMAPINFOHEADER_
  {
    u32_t biSize;
    long  biWidth;
    long  biHeight;
    u16_t biPlanes;
    u16_t biBitCount;
    u32_t biCompression;
    u32_t biSizeImage;
    long  biXPelsPerMeter;
    long  biYPelsPerMeter;
    u32_t biClrUsed;
    u32_t biClrImportant;

    void Read(rcc_ptr<DataStream> const &is)
      {
        biSize          = is->Read32le();
        biWidth         = is->Read32le();
        biHeight        = is->Read32le();
        biPlanes        = is->Read16le();
        biBitCount      = is->Read16le();
        biCompression   = is->Read32le();
        biSizeImage     = is->Read32le();
        biXPelsPerMeter = is->Read32le();
        biYPelsPerMeter = is->Read32le();
        biClrUsed       = is->Read32le();
        biClrImportant  = is->Read32le();
      }

    void Write(rcc_ptr<DataStream> const &is)
      {
        is->Write32le(biSize);
        is->Write32le(biWidth);
        is->Write32le(biHeight);
        is->Write16le(biPlanes);
        is->Write16le(biBitCount);
        is->Write32le(biCompression);
        is->Write32le(biSizeImage);
        is->Write32le(biXPelsPerMeter);
        is->Write32le(biYPelsPerMeter);
        is->Write32le(biClrUsed);
        is->Write32le(biClrImportant);
      }
  };

struct TFFt_PICTUREBITMAP : TFFt_PICTURE
  {
    rcc_ptr<DataStream> is_;
    TFFt_BITMAPFILEHEADER_ bmFH_;
    TFFt_BITMAPINFOHEADER_ pDIB_;
    BufferT<rgba8_t>   quad_;
    BufferT<byte_t>    row_;
    int stride_;
    int row_no_;
    bool shortgreen_;
    bool alphabits_;    
    bool info_loaded_;
    TFFt_CopyRowXtoX copyf8_;

    TFFt_PICTUREBITMAP(rcc_ptr<DataStream> const &ds);
    virtual ~TFFt_PICTUREBITMAP();
    
    bool ReadPicInfo();

    virtual TFFMt_E GetInfo(INFO *);
    virtual TFFMt_E NextRow8(rgba8_t *row,int shift,int cnt);
  };

TFFt_PICTUREBITMAP::TFFt_PICTUREBITMAP(rcc_ptr<DataStream> const &ds) 
  : is_(ds), info_loaded_(false)
  {
  }

TFFt_PICTUREBITMAP::~TFFt_PICTUREBITMAP()
  {
  }

bool TFFt_PICTUREBITMAP::ReadPicInfo()
  {
    is_->Seek(0);

    bmFH_.Read(is_); 
    pDIB_.Read(is_); 

    if ( is_->Error() || bmFH_.bfType != 0x4d42 ) 
      {
        Lerr << _S*"[MEDIA ERROR] failed to load bitmap headers from datasource:'%?'" %is_->Identifier();
      }

    if ( pDIB_.biBitCount == 8 ) 
      {
        is_->Seek(pDIB_.biSize + 0xe);
        if ( pDIB_.biClrUsed == 0 ) pDIB_.biClrUsed = 256;
        byte_t buff[256*4];
        is_->Read(&buff[0],4*pDIB_.biClrUsed);
        quad_.Resize(pDIB_.biClrUsed);
        for ( unsigned i = 0; i < pDIB_.biClrUsed; ++i )
          quad_[i] = rgba8_t(buff[i*4+2],buff[i*4+1],buff[i*4+0],255);
      }
  
    alphabits_  = false;
    shortgreen_ = false;

    if ( pDIB_.biBitCount == 32 ) 
      {
        stride_ = ((signed)(pDIB_.biWidth)*4+3)&~0x03U;
        alphabits_ = true;
      }
    else if (pDIB_.biBitCount == 24 ) 
      {
        stride_ = ((signed)(pDIB_.biWidth)*3+3)&~0x03U;
      }
    else if (pDIB_.biBitCount == 16 ) 
      {
        stride_ = ((signed)(pDIB_.biWidth)*2+3)&~0x03U;
        if( pDIB_.biCompression == x_BI_BITFIELDS ) 
          {
            is_->Read32le();
            if ( (u16_t)is_->Read32le() == 0x03e0 )
              shortgreen_ = alphabits_ = true;
            is_->Read32le();
          }
        else 
          alphabits_ = shortgreen_ = true;
      }
    else if (pDIB_.biBitCount == 8 ) 
      {
        stride_ = ((signed)(pDIB_.biWidth)+3)&~0x03U;
      }

    if ( !this->pDIB_.biClrUsed )
      copyf8_ = TffmCopyRowXtoX(pDIB_.biBitCount,32);
    else
      copyf8_ = 0;

    info_loaded_ = true;

    is_->Seek(bmFH_.bfOffBits);
    
    if ( is_->Error() ) 
      {
        Lerr << _S*"[MEDIA ERROR] failed to load bitmap headers from datasource:'%?'" %is_->Identifier();
        return false;
      }    
    else
      return true;

  };

TFFMt_E TFFt_PICTUREBITMAP::GetInfo(INFO *picnfo)
  {
    if ( info_loaded_ )
      {
        picnfo->alphabits = this->alphabits_;
        picnfo->bpp       = this->pDIB_.biBitCount;
        picnfo->frames    = 1;
        picnfo->height    = this->pDIB_.biHeight;
        picnfo->width     = this->pDIB_.biWidth;
        picnfo->palsize   = this->pDIB_.biClrUsed;
        picnfo->updown    = 0;
        return TFFM_OK;
      }
    else
      return TFFM_FAIL;
  }

TFFMt_E TFFt_PICTUREBITMAP::NextRow8(rgba8_t *row,int shift,int cnt)
  {
    row_.Resize(stride_);
    if ( is_->Read(&row_[0],stride_) != stride_ )
      return TFFM_FAIL;
    if ( cnt+shift > pDIB_.biWidth )
      return TFFM_BADARGUMENT;
    
    byte_t *p = &row_[0]+shift*pDIB_.biBitCount/8;

    if ( this->pDIB_.biClrUsed )
      for ( int i = 0; i < cnt; ++i )
        {
          STRICT_REQUIRE(p[i] < quad_.Count());
          row[i] = quad_[p[i]];
        }
    else
      copyf8_(p,row,cnt);

    return TFFM_OK;
  }

mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmBmpCreatePicFrom( rcc_ptr<DataStream> const &ds )
  { 
    mem_ptr<TFFt_PICTUREBITMAP> bmp = MemPtr( new TFFt_PICTUREBITMAP(ds));
    if ( bmp->ReadPicInfo() )
      return mem_ptr<TFFt_PICTURE>(bmp.forget_());
    else
      return mem_ptr<TFFt_PICTURE>(0);
  }

TFFMt_E CXX_STDCALL TffmWriteBmpBitsStream8( rcc_ptr<DataStream> const &ds, rgba8_t *img, int width, int height,bool invert)
  {
    TFFt_BITMAPFILEHEADER_ bmFH_;
    TFFt_BITMAPINFOHEADER_ pDIB_;
    bmFH_.bfType = 0x4d42;
    bmFH_.bfOffBits = 54;
    bmFH_.bfSize = bmFH_.bfOffBits + ((width*3 + 3) & ~3)*height;
    bmFH_.bfReserved1 = 0;
    bmFH_.bfReserved2 = 0;
    bmFH_.Write(ds);
    ds->Flush();
    pDIB_.biSize = 40;
    pDIB_.biBitCount = 24;
    pDIB_.biClrImportant = 0;
    pDIB_.biClrUsed = 0;
    pDIB_.biCompression = BI_RGB;
    pDIB_.biHeight = height;
    pDIB_.biWidth  = width;
    pDIB_.biPlanes = 1;
    pDIB_.biSizeImage = 0;
    pDIB_.biXPelsPerMeter = 0;
    pDIB_.biYPelsPerMeter = 0;
    pDIB_.Write(ds);
    ds->Flush();
    REQUIRE( ds->Tell() == bmFH_.bfOffBits );
    
    mem_arr<rgb8_t> p = MemArr( new rgb8_t[width+1] );
    TFFt_CopyRowXtoX copy8_f = TffmCopyRowXtoX(TFF_PF_RGBA8,TFF_PF_RGB8);

    if ( invert )
      for ( int i = height-1; i >= 0; --i )
        {
          copy8_f( img + width*i, +p, width );
          ds->Write( (byte_t*)+p, (width*3 + 3) & ~3 );
        }
    else
      for ( int i = 0; i < height; ++i )
        {
          copy8_f( img + width*i, +p, width );
          ds->Write( (byte_t*)+p, (width*3 + 3) & ~3 );
        }

    ds->Flush();
    return TFFM_OK;
  }

extern "C" TFFMt_E CXX_STDCALL TffmWriteBmpBits8(pwide_t fname, rgba8_t *img, int width, int height, bool invert)
  {
    _wunlink(fname);
    if ( rcc_ptr<DataStream> ds = teggo::OpenFile( fname, FILEOPEN_WRITE|FILEOPEN_CREATE ) )
      {
        TffmWriteBmpBitsStream8(ds,img,width,height,invert);
        return TFFM_OK;
      }
    else
      return TFFM_NOFILE;
  }
