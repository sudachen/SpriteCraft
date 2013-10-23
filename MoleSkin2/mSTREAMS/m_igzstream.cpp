
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
#include "m_igzstream.h"
#include "../mcore/m_debug.h"
#include "../mcore/m_automem.h"

#if defined _WIN32
#define CHECK_GZCRC
#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

#include  "../../Media/tffmedia.h"

extern "C" {
#include  "../../Media/lib.z/zlib.h"
}

#define Z_BUFSIZE (16*1024UL)

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

NAMESPACE_MSKIN_BEGIN

static const u32_t GZEOF = 0xffffffffUL;

u32_t GZipInputStream::GetByte()
{
  if ( eof_ ) return GZEOF;
  if ( zs_->avail_in == 0 ) {
    zs_->avail_in = is_->Read(inbuff_,Z_BUFSIZE);
    goto_if ( eof2ok(is_->PeekError()) != SCE_OK , error );
    if ( zs_->avail_in == 0 ) {
      eof_ = true;
      return GZEOF;
    }
    zs_->next_in = inbuff_;
  }
  --zs_->avail_in;
  ++wasread_;
  return *(zs_->next_in)++;
error:
  SetECode(SCE_BAD_STREAM);
  return 0;
}

void GZipInputStream::CheckHeader()
{
  u32_t m0,m1,method,flags;
  m0 = GetByte();
  m1 = GetByte();
  goto_if ( m0 != 0x1f || m1 != 0x8b , error ); 
  method = GetByte();
  flags  = GetByte();
  goto_if ( method != Z_DEFLATED || (flags&RESERVED) != 0 , error );
  { for ( unsigned u = 0; u < 6; ++u ) GetByte(); }
  if ((flags & EXTRA_FIELD) != 0) 
  { // пропускаем дополнительную инфу
	  u32_t len = GetByte(); 
	  len += GetByte()<<8;     
  	while (len-- != 0 && GetByte() != GZEOF) {/*nothing*/}
  }
  if ((flags & ORIG_NAME) != 0) 
  { // пропускаем имя файла
    u32_t c;
	  while ((c = GetByte()) != 0 && c != GZEOF) {/*nothing*/}
  }
  if ((flags & COMMENT) != 0) 
  { // пропускаем коментарии
    u32_t c;
	  while ((c = GetByte()) != 0 && c != GZEOF) {/*nothing*/}
  }
  if ((flags & HEAD_CRC) != 0) 
  { // пропускаем контрольную сумму заголовка 
	  for (unsigned u = 0; u < 2; ++u) GetByte();
  }
  goto_if ( eof_ , error );
  return;
error:
  SetECode(SCE_BAD_STREAM);
  return;
}

void GZipInputStream::Init_()
{
  zs_ = new z_stream;
  memset(zs_,0,sizeof(z_stream));
#ifdef CHECK_GZCRC
  crc_ = crc32(0,0,0);
#endif
  zs_->next_in = inbuff_ = new byte_t[Z_BUFSIZE];
  memset ( inbuff_, 0, Z_BUFSIZE );
  REQUIRE ( inflateInit2(zs_,-MAX_WBITS) == Z_OK );
  eof_ = false;
  wasread_ = 0;
  if ( in_gzip_ ) CheckHeader();
  start_ = is_->WasRead() - zs_->avail_in;
  wasread_ = 0;
}

GZipInputStream::GZipInputStream(InputStreamEx* is) 
: is_(is), in_gzip_(true), Xcrc_(0)
{ Init_(); }

GZipInputStream::GZipInputStream(InputStreamEx* is,UNCOMPRESSONLYSWITCHER,u32_t crc) 
: is_(is), in_gzip_(false), Xcrc_(crc)
{ Init_(); }

GZipInputStream::~GZipInputStream()
{
  inflateEnd(zs_);
  delete[] inbuff_;
  delete zs_;
}

unsigned GZipInputStream::Read(byte_t* dest,unsigned count)
{
  if ( eof_ || ecode_ != SCE_OK ) return 0;
#ifdef CHECK_GZCRC
  byte_t* dStart = dest;
#endif
  zs_->next_out = dest;
  zs_->avail_out = count;
  while ( zs_->avail_out != 0 ){
    if ( zs_->avail_in == 0 && !eof_ ) {
      zs_->avail_in = is_->Read(inbuff_,Z_BUFSIZE);
      goto_if ( eof2ok(is_->PeekError()) != SCE_OK , error );
      if ( zs_->avail_in == 0 ) {
        //eof_ = true;
        //break;
      }
      zs_->next_in = inbuff_;
    }
    int res;
    //if ( zs_->avail_in == 0 ) {
    //  res = inflate(zs_,Z_FINISH);
    //}else
    res = inflate(zs_,Z_SYNC_FLUSH);
    if ( res == Z_BUF_ERROR ) 
      res = Z_STREAM_END;
    if ( res == Z_STREAM_END ) {
#ifdef CHECK_GZCRC
      crc_ = crc32(crc_,dStart,u32_t(zs_->next_out-dStart));
      if ( in_gzip_ ) {
        dStart = zs_->next_out;
        u32_t gzcrc = 0;
        u32_t x;
        x = GetByte(); goto_if ( x == GZEOF , error );
        gzcrc |= x;
        x = GetByte(); goto_if ( x == GZEOF , error );
        gzcrc |= x<<8;
        x = GetByte(); goto_if ( x == GZEOF , error );
        gzcrc |= x<<16;
        x = GetByte(); goto_if ( x == GZEOF , error );
        gzcrc |= x<<24;
        goto_if ( gzcrc != crc_ , error);
      }else{
        if ( Xcrc_ != 0 )
          goto_if ( Xcrc_ != crc_ , error);
      }
#endif
      eof_ = true;
    }
    else {
      goto_if ( res != Z_OK , error );
    }
    if ( eof_ ) break;
  }
#ifdef CHECK_GZCRC
  crc_ = crc32(crc_,dStart,u32_t(zs_->next_out-dStart));
#endif
  {
    u32_t wr = count - zs_->avail_out;
    wasread_+=wr;
    return wr; 
  }
error:
  SetECode(SCE_BAD_STREAM);
  return 0;
}

bool GZipInputStream::EndOfStream() const
{
  return  eof_;
}

bool GZipInputStream::Reset()
{
  wasread_ = 0;
  zs_->avail_in = 0;
  eof_ = false;
#ifdef CHECK_GZCRC
  crc_ = crc32(0,0,0);
#endif
  inflateReset(zs_);
  bool parent_reset = is_->Reset();
  unsigned skipped = is_->Skip(start_);
  return parent_reset && skipped == start_; 
}

unsigned GZipInputStream::Skip(unsigned bytes)
{
  byte_t skipbuf[128];
  unsigned skipped = 0;
  for ( ; skipped < bytes; ) {
    u32_t k = /*std::*/teggo_min(u32_t(bytes-skipped),u32_t(128));
    if ( Read(skipbuf,k) == 0 ) break;
    skipped+=k;
  }
  return skipped;
}

unsigned GZipInputStream::WasRead() const
{
  return wasread_;
}

NAMESPACE_MSKIN_END
