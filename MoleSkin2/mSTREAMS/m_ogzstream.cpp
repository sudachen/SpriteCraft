
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
#include "m_ogzstream.h"
#include "../mcore/m_debug.h"
#include "../mcore/m_automem.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

extern "C" {
#include "../../Media/lib.z/zlib.h"
#include "../../Media/lib.z/zutil.h"
}

#define Z_BUFSIZE (16*1024UL)

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */
#define ZLEVEL       Z_DEFAULT_COMPRESSION
#define ZSTRATEGY    Z_DEFAULT_STRATEGY
NAMESPACE_MSKIN_BEGIN

SCERROR GZipOutputStream::WriteHeader(pchar_t name) {
  reterr_if ( os_->WriteByte(0x1f) );
  reterr_if ( os_->WriteByte(0x8b) );
  reterr_if ( os_->WriteByte(Z_DEFLATED) );
  if ( name ) {
    reterr_if ( os_->WriteByte(ORIG_NAME) );
  }else{
    reterr_if ( os_->WriteByte(0) );
  }
  reterr_if ( os_->Fill(0,5) );
  reterr_if ( os_->WriteByte(OS_CODE) );
  if ( name ) {
    reterr_if ( os_->Write((const byte_t*)name,strlen(name)+1) );
  }
  return SCE_OK;
}

void GZipOutputStream::Init_(pchar_t name,u32_t level,bool write_hdr)
{
  crc_ = crc32(0,0,0);
  written_ = 0;
  finished_ = false;
  zs_ = new z_stream;
  memset(zs_,0,sizeof(z_stream));
  zs_->next_out = buff_ = new byte_t[Z_BUFSIZE];

  if ( Z_OK != deflateInit2(zs_,level, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL,ZSTRATEGY) ){
    ecode_ = SCE_GZIPFAIL; 
    goto broken;
  }
  zs_->avail_out = Z_BUFSIZE;
  if ( write_hdr ) if ( (ecode_ = WriteHeader(name)) != SCE_OK ) goto broken;
  return;

broken:
  finished_ = true;
  return; 
}

GZipOutputStream::GZipOutputStream(OutputStreamEx* os,pchar_t name,u32_t level) : osex_(os),os_(os), is_gzip_(true){
  Init_(name,level,true);
}

GZipOutputStream::GZipOutputStream(OutputStreamEx* os,COMPRESSONLYSWITCHER,u32_t level) : osex_(os),os_(os), is_gzip_(false){
  Init_(0,level,false);
}

GZipOutputStream::GZipOutputStream(OutputStream& os,u32_t level) : os_(&os), is_gzip_(false){
  Init_(0,level,false);
}

GZipOutputStream::~GZipOutputStream()
{
  deflateEnd(zs_);
  delete[] buff_;
  delete zs_;
}

SCERROR GZipOutputStream::Write(const byte_t* src,unsigned count)
{
  retval_if ( finished_ , SCE_FINISHED);
  retval_if ( ecode_!=SCE_OK, ecode_ );

  zs_->next_in = (byte_t*)src;
  zs_->avail_in = count;
  while (zs_->avail_in != 0) {
    if (zs_->avail_out == 0) {
      zs_->next_out = buff_;
      if ( ( ecode_ = os_->Write(buff_,Z_BUFSIZE) ) != SCE_OK ) goto broken;
      zs_->avail_out = Z_BUFSIZE;
    }
    if ( Z_OK != deflate(zs_,Z_NO_FLUSH) ) {
      ecode_ = SCE_GZIPFAIL;
      goto broken;
    }
  }
  written_ += count;
  crc_ = crc32(crc_, src, count);
  return SCE_OK;
broken:
  finished_ = true;
  return ecode_;
}

SCERROR GZipOutputStream::Fill(byte_t val,unsigned count)
{
  byte_t b[8] = { val,val,val,val,val,val,val,val };
  unsigned u = 0;
  for ( ; u < count%8; ++u ) {
    reterr_if(Write(b,8));
  }
  reterr_if(Write(b,count-u));
  return SCE_OK;
}

SCERROR GZipOutputStream::Flush() { return os_->Flush(); }

SCERROR GZipOutputStream::Finish() { return Finish_(false); }

SCERROR GZipOutputStream::Finish_(bool gzonly)
{
  retval_if ( finished_ , SCE_FINISHED);
  retval_if ( ecode_!=SCE_OK, ecode_ );
  finished_ = true;
  zs_->avail_in = 0;

  bool done = false;
  for(;;) {
    unsigned len = Z_BUFSIZE - zs_->avail_out;
    if (len != 0) {
      if ( ( ecode_ = os_->Write(buff_,len) ) != SCE_OK ) goto broken;
        zs_->next_out = buff_;
        zs_->avail_out = Z_BUFSIZE;
    }
    if (done) break;
    int gzerr = deflate(zs_,Z_FINISH);
    //int gzerr = deflate(zs_,Z_FULL_FLUSH);
    //int gzerr = deflate(zs_,Z_SYNC_FLUSH);
    if ( Z_OK != gzerr ) {
      if ( len == 0 && gzerr == Z_BUF_ERROR ) {gzerr = Z_OK;}
      else if ( gzerr != Z_STREAM_END ){ 
        ecode_ = SCE_GZIPFAIL;
        goto broken;
      }
    }
    done = (zs_->avail_out != 0 || gzerr == Z_STREAM_END);
    if (gzerr == Z_STREAM_END && Z_BUFSIZE - zs_->avail_out == 0 ) break;
  }
  if ( is_gzip_ ) {
    reterr_if ( os_->WriteLong(crc_) );
    reterr_if ( os_->WriteLong(written_) );
  }
  if ( !gzonly )
    return os_->Finish();
  else
    return os_->Flush();
broken:
  return ecode_;
}

unsigned GZipOutputStream::WasWritten() const
{
  return written_;
}

NAMESPACE_MSKIN_END
