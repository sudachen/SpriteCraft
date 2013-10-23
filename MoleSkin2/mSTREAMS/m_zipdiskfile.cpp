
/*

Copyright Â© 2003-2013, AlexÃ©y SudachÃ©n, alexey@sudachen.name

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
#include "m_zipdiskfile.h"
#include "m_istream.h"
#include "m_ostream.h"
#include "m_ogzstream.h"
#include "m_igzstream.h"
#include "m_cpystream.h"
#include "../mcore/m_debug.h"
#include "../mcore/m_automem.h"

#include <errno.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
# pragma warning(disable:4530)  //C++ exception handler used, but unwind semantics are not enabled.
# pragma warning(disable:4786)  //identifier was truncated to '255' characters in the debug information
#endif

#if defined _WIN32
#define CHECK_GZCRC
#endif

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
class ZipDiskFile;
/**
  \class ZipInputSubStream
  ïîòîê äëÿ ÷òåíèÿ ôàéëà èç Zip àðõèâà
*/
class ZipInputSubStream : public BasicInputStream{
  u32_t was_read_in_;
  u32_t stream_sz_in_;
  u32_t start_pos_;
  bool eof_;
  auto_ref<ZipDiskFile> diskfile_;
public:
  explicit ZipInputSubStream(u32_t pos,u32_t sz,ZipDiskFile*);
  virtual ~ZipInputSubStream();
  virtual unsigned Read(byte_t* dest,unsigned count);
  virtual bool EndOfStream() const { return eof_; }
  virtual bool Reset() {was_read_in_ = 0; return true;}
  virtual unsigned Skip(unsigned);
  virtual unsigned WasRead() const { return was_read_in_; }
  void BrokenStream() { if (!!diskfile_) {diskfile_->ForgetAbout(this),Release(diskfile_);} }
private:
  u32_t GetByte();
  ZipInputSubStream& operator = (const ZipInputSubStream&);
  ZipInputSubStream(const ZipInputSubStream&);
};

// -----------------------------------------------------------------------
ZipDiskFile::ZipDiskFile()
{
  fd_ = 0;
  roMode_ = false;
}

// -----------------------------------------------------------------------
ZipDiskFile::~ZipDiskFile()
{
  Close();
}

// -----------------------------------------------------------------------
void ZipDiskFile::Release_() const { refbase::Release_(); }

// -----------------------------------------------------------------------
void ZipDiskFile::Grab_() const { refbase::Grab_(); }

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::Create(pwide_t fname,bool failIfExists)
{
  if ( fd_ ) fail_because( _S*"zipfile::Create: already opened file '%?'" %fname );
  if ( IsFileExists(fname) && failIfExists )
    fail_because( _S*"zipfile::Create: file '%?' already exists" %fname );
  fd_ = _wfopen(fname,L"w+b");
  if ( !fd_ )
    fail_because( _S*"zipfile::Create: error on open file '%?', reason %?" %fname %strerror(errno) );
  fail_if_fail(CreateZipHeader());
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::Open(pwide_t fname,bool createIfNotExists)
{
  if ( fd_ ) fail_because( _S*"zipfile::Open: already opened file '%?'" %fname );
  bool exists = IsFileExists(fname);
  if ( !exists && !createIfNotExists )
    fail_because( _S*"zipfile::Open: file '%?' is not exists" %fname );
  if ( exists ) 
    if ( !IsFileRewritable(fname)  ) 
      fd_ = _wfopen(fname,L"rb");
    else 
      fd_ = _wfopen(fname,L"r+b");
  else fd_ = _wfopen(fname,L"w+b");
  if ( !fd_ )
    fail_because( _S*"zipfile::Open: error on open file '%?', reason %?" %fname %strerror(errno) );
  if ( exists )
    fail_if_fail(ParseZipHeader());
  else
    fail_if_fail(CreateZipHeader());
  return SCE_OK;
}

// -----------------------------------------------------------------------
//SCERROR ZipDiskFile::OpenRO(pchar_t fname)
//{
//}

// -----------------------------------------------------------------------
//SCERROR ZipDiskFile::OpenRO(byte_t* mem, u32_t size)
//{
//}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::Close()
{
  InvalidateAllStreams();
  if ( !fd_ ) return SCE_OK;
  fclose ( fd_ );
  records_.Clear();
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::PackStreamInplace(pwide_t fname,InputStream* stream,u32_t level)
{
  if ( records_.Get(fname) )
    fail_because( _S*"zipfile::PackStreamInplace: file %? already exists in archive" %fname );
  long localhdr_pos = centraldir_;
  bool dir = false;
  u32_t crc = 0;
  long compressed = 0;
  long uncompressed = 0;
  unsigned hdr_sz = 4+5*2+3*4+2*2+wcslen(fname);
  fail_if_fail( SeekFd(centraldir_,SEEK_SET) );
  {
    FileOutputStream fdffos(fd_);
    fdffos.Skip(hdr_sz);
    if ( level != 0 ) {
      GZipOutputStream gzos(Grab(&fdffos),GZipOutputStream::COMPRESSONLY,level);
      fail_if_fail( CopyStream(*stream,gzos,0) );
      fail_if_fail( gzos.Finish() );
      crc = gzos.GetCRC();
      compressed = fdffos.WasWritten()-hdr_sz;
      uncompressed = stream->WasRead();
    }else if ( stream ) {
      fail_if_fail( CopyStreamCRC(*stream,fdffos,0,&crc) );
      compressed = uncompressed = fdffos.WasWritten()-hdr_sz;
      REQUIRE ( uncompressed == (long)stream->WasRead() );
    }else{
      dir = true;
      compressed = uncompressed = 0;
    }
    centraldir_ = fdffos.WasWritten()+localhdr_pos;
  }
  fail_if_fail( SeekFd(localhdr_pos,SEEK_SET) );
  {
    FileOutputStream fdffos(fd_);
    fail_if_fail( fdffos.WriteLong(0x04034b50) );
    fdffos.Skip(4);
    fail_if_fail( fdffos.WriteShort(level?8:0) );
    fdffos.Skip(4);
    fail_if_fail( fdffos.WriteLong(crc) );
    fail_if_fail( fdffos.WriteLong(compressed) );
    fail_if_fail( fdffos.WriteLong(uncompressed) );
    fail_if_fail( fdffos.WriteShort(wcslen(fname)) );
    fail_if_fail( fdffos.WriteShort(0) );
    fail_if_fail( fdffos.Write((cbyte_t*)fname,wcslen(fname)) );
  }
  {
    RECORD rec;
    rec.bits_flags = 0;
    rec.comment_length = 0;
    rec.compress_method = level?8:0;
    rec.compressed_sz = compressed;
    rec.crc32_val = crc;
    rec.disk_skip = 0;
    rec.external_attr = 0;
    rec.extra_length = 0;
    rec.extract_version = 0;
    rec.fname = fname;
    rec.fname_length = wcslen(fname);
    rec.internal_attr = 0;
    rec.last_mod_date = 0;
    rec.last_mod_time = 0;
    rec.rel_offset    = localhdr_pos;
    rec.uncompressed_sz = uncompressed;
    rec.version = 0;
    records_.Put(+rec.fname,rec);
  }
  fail_if_fail(StoreCentralDirectory(centraldir_));
  fflush(fd_);
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::ExtractStream(pwide_t fname,InputStreamEx** stream)
{
  fail_if (!stream || *stream != 0);
  if ( RECORD* i = records_.Get(fname) )
  {
    u32_t signature;
    u16_t method;
    u32_t crc;
    u32_t stream_sz;
    u16_t skip_fname;
    u16_t skip_extra;
    u32_t rel_offs = i->rel_offset;
    fail_if_fail( SeekFd( rel_offs, SEEK_SET ) );
    fail_if_fail( ReadLongFd(&signature) );
    if (signature != 0x04034b50) 
      fail_because( _S*"zipfile::ExtractStream: bed signature of record '%?'" %fname );
    fail_if_fail( SeekFd(4,SEEK_CUR) );
    fail_if_fail( ReadShortFd(&method) );
    fail_if_fail( SeekFd(4,SEEK_CUR) );
    fail_if_fail( ReadLongFd(&crc) );
    fail_if_fail( ReadLongFd(&stream_sz) );
    fail_if_fail( SeekFd(4,SEEK_CUR) );
    fail_if_fail( ReadShortFd(&skip_fname) );
    fail_if_fail( ReadShortFd(&skip_extra) );
    fail_if_fail( SeekFd(skip_fname+skip_extra,SEEK_CUR) );

    auto_ref<ZipInputSubStream> zstream(new ZipInputSubStream(WasReadFd(),stream_sz,Grab(this)));
    auto_ref<InputStreamEx> xstream;
    if ( method == 0 ) xstream = zstream;
    else if ( method == 8 ) 
      xstream = AutoRef(
        new GZipInputStream(Grab(zstream),GZipInputStream::UNCOMPRESSONLY,crc));
    else
      fail_because( _S*"zipfile::ExtractStream: invalid compression method of record %?" %fname );
    fail_if_fail( xstream->PeekError() );
    substreams_.Put(Get(zstream));
    *stream = Forget(xstream);
    return SCE_OK;
  }
  else
    fail_because( _S*"zipfile::ExtractStream: stream %? is not exists" %fname );
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::GetStats(pwide_t fname,STATS& stats)
{
  if ( RECORD *i = records_.Get(fname) )
  {
    RECORD& r = *i;
    stats.compressed = r.compress_method != 0;
    stats.packed_size = r.compressed_sz;
    stats.unpacked_size = r.uncompressed_sz;
    stats.date = 0;
    return SCE_OK;
  }
  else return SCE_FAIL;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::Enumerate(BufferT<widestring>& e)
{
  e.clear();
  e.reserve(records_.Count());
  for ( RECMAP::Iterator i = records_.Iterate(); i.Next(); )
    e.push_back(widestring(i.Key()));
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::CreateZipHeader()
{
  centraldir_ = 0;
  fail_if_fail(StoreCentralDirectory(centraldir_));
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::ParseZipHeader()
{
  fail_if_fail(SearchCentralDirectory(&centraldir_));
  fail_if_fail(LoadCentralDirectory(centraldir_));
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::SearchCentralDirectory(u32_t* pos)
{
  static byte_t cdend_sign[4] = {0x50,0x4b,0x05,0x06};
  //if ( !fd_ ) fail_because("zipfile::SearchCentralDirectory: file is not opened");
  fail_if_fail ( SeekFd(0,SEEK_END) );
  long backsz = teggo_min<long>(1024,WasReadFd()-1);
  fail_if_fail ( SeekFd(-backsz,SEEK_END) );
  {
    BufferT<byte_t> backbf(backsz);
    fail_if_fail( ReadBuffer(&backbf[0],backsz) );
    byte_t* i = teggo::search(&backbf[0],&backbf.back()+1,cdend_sign,cdend_sign+sizeof(cdend_sign));
    if ( i == &backbf.back()+1 ) 
      fail_because(L"zipfile::SearchCentralDirectory: can't find central directory");
    fail_if_fail ( SeekFd(-backsz+i-&backbf[0],SEEK_END) );
  }
  { u32_t signature;
    fail_if_fail( ReadLongFd(&signature) );
    fail_if( signature != 0x06054b50 );
    fail_if_fail( SeekFd(2+2+2+2,SEEK_CUR) );
    u32_t central_dir_offs;
    u32_t central_dir_sz;
    fail_if_fail( ReadLongFd(&central_dir_sz) );
    fail_if_fail( ReadLongFd(&central_dir_offs) );
    *pos  = central_dir_offs;
  }
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::LoadCentralDirectory(u32_t pos)
{
  RECMAP().Swap(records_);
  if ( !fd_ ) fail_because(L"zipfile::SearchCentralDirectory: file is not opened");
  fail_if_fail( SeekFd(pos,SEEK_SET) );
  for (;;) {
    u32_t  signature;
    RECORD rec;
    BufferT<byte_t> fname;
    
    fail_if_fail( ReadLongFd(&signature) );
    if ( signature == 0x06054b50 ) break;
    else if ( signature != 0x02014b50 ) 
      fail_because(L"zipfile::LoadCentralDirectory: invalid signature");
    
    fail_if_fail(ReadShortFd(&rec.version));
    fail_if_fail(ReadShortFd(&rec.extract_version));
    fail_if_fail(ReadShortFd(&rec.bits_flags));
    fail_if_fail(ReadShortFd(&rec.compress_method));
    fail_if_fail(ReadShortFd(&rec.last_mod_time));
    fail_if_fail(ReadShortFd(&rec.last_mod_date));
    fail_if_fail(ReadLongFd(&rec.crc32_val));
    fail_if_fail(ReadLongFd(&rec.compressed_sz));
    fail_if_fail(ReadLongFd(&rec.uncompressed_sz));
    fail_if_fail(ReadShortFd(&rec.fname_length));
    fail_if_fail(ReadShortFd(&rec.extra_length));
    fail_if_fail(ReadShortFd(&rec.comment_length));
    fail_if_fail(SeekFd(2,SEEK_CUR));
    fail_if_fail(ReadShortFd(&rec.internal_attr));
    fail_if_fail(ReadLongFd(&rec.external_attr));
    fail_if_fail(ReadLongFd(&rec.rel_offset));

    fname.resize(rec.fname_length+1);
    fail_if_fail(ReadBuffer(&fname[0],rec.fname_length));
    fname.back() = 0;
    rec.fname = pchar_t(&fname[0]);
    fail_if_fail(SeekFd(rec.extra_length+rec.comment_length,SEEK_CUR));
    //XMessage((":+: '%s' at %d, c/u %d/%d",
    //  &fname[0],rec.rel_offset,rec.compressed_sz,rec.uncompressed_sz));
    records_.Put(+rec.fname,rec);
  }
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::StoreCentralDirectory(u32_t pos)
{
  fail_if_fail ( SeekFd(pos, SEEK_SET) );
  FileOutputStream fdffos(fd_);
  for ( RECMAP::Iterator i = records_.Iterate(); i.Next(); ) 
  {
    RECORD& r = *i;
    fail_if_fail( fdffos.WriteLong( 0x02014b50) );
    fail_if_fail( fdffos.WriteShort(r.version) );
    fail_if_fail( fdffos.WriteShort(r.extract_version) );
    fail_if_fail( fdffos.WriteShort(r.bits_flags) );
    fail_if_fail( fdffos.WriteShort(r.compress_method) );
    fail_if_fail( fdffos.WriteShort(r.last_mod_time) );
    fail_if_fail( fdffos.WriteShort(r.last_mod_date) );
    fail_if_fail( fdffos.WriteLong(r.crc32_val) );
    fail_if_fail( fdffos.WriteLong(r.compressed_sz) );
    fail_if_fail( fdffos.WriteLong(r.uncompressed_sz) );
    fail_if_fail( fdffos.WriteShort(r.fname_length) );
    fail_if_fail( fdffos.WriteShort(r.extra_length) );
    fail_if_fail( fdffos.WriteShort(r.comment_length) );
    fail_if_fail( fdffos.WriteShort(0) );
    fail_if_fail( fdffos.WriteShort(r.internal_attr) );
    fail_if_fail( fdffos.WriteLong(r.external_attr) );
    fail_if_fail( fdffos.WriteLong(r.rel_offset) );
    fail_if_fail( fdffos.Write((cbyte_t*)+r.fname,r.fname_length) );
  }
  long cdirctory_sz = fdffos.WasWritten();
  fail_if_fail( fdffos.WriteLong( 0x06054b50) );
  fail_if_fail( fdffos.WriteShort(0) );
  fail_if_fail( fdffos.WriteShort(0) );
  fail_if_fail( fdffos.WriteShort(0) );
  fail_if_fail( fdffos.WriteShort(records_.Count()) );
  fail_if_fail( fdffos.WriteLong(cdirctory_sz) );
  fail_if_fail( fdffos.WriteLong(pos) );
  fail_if_fail( fdffos.WriteShort(0) );
  fdffos.Flush();
  return SCE_OK;
}

// -----------------------------------------------------------------------
u32_t ZipDiskFile::WasReadFd()
{
  return ftell(fd_);
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::SeekFd(long pos,int seek_set)
{
  if ( !fd_ ) fail_because( "zipfile: seek failed, file is not opened" );
  if ( fseek( fd_, pos, seek_set ) != 0 )
    fail_because( _S*"zipfile: seek failed, reason %?" %strerror(ferror(fd_)) );
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::ReadBuffer_(byte_t* buff,u32_t* sz)
{
  if ( !fd_ ) fail_because( "zipfile: read failed, file is not opened" );
  u32_t k = 0;
  while ( k < *sz ) {
    int u = fread(buff+k,1,*sz-k,fd_);
    if ( u < 0 ) {
      fail_because( _S*"zipfile: read error, reason %?" %strerror(ferror(fd_)) );
    } else if ( u == 0 && feof(fd_) ) {
      *sz = k;
      return SCE_EOF;
    } else {
      k+=u;
    }
  }
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::ReadBuffer(byte_t* buff,u32_t sz)
{
  fail_if_fail ( eof2fail( ReadBuffer_(buff,&sz) ) );
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::ReadLongFd(u32_t* val)
{
  fail_if ( !val );
  byte_t val_[4];
  fail_if_fail(ReadBuffer(val_,4));
  *val = ReadLong_(val_);
  return SCE_OK;
}

// -----------------------------------------------------------------------
SCERROR ZipDiskFile::ReadShortFd(u16_t* val)
{
  fail_if ( !val );
  byte_t val_[2];
  fail_if_fail(ReadBuffer(val_,2));
  *val = ReadShort_(val_);
  return SCE_OK;
}

// -----------------------------------------------------------------------
void ZipDiskFile::InvalidateAllStreams()
{
  while ( !substreams_.Empty()  ) (*substreams_.Iterate())->BrokenStream();
}

// -----------------------------------------------------------------------
void ZipDiskFile::ForgetAbout(ZipInputSubStream* stream)
{
  substreams_.Erase(stream);
}

/* ----------------------------------------------------------------------

  Zip Input Substream implementaion

---------------------------------------------------------------------- */


// -----------------------------------------------------------------------
unsigned ZipInputSubStream::Skip(unsigned bytes) {
  if ( !diskfile_) {
    Lerr << "zipsubstream: skip failed, file is not opened";
    SetECode(SCE_FAIL);
    return 0;
  }
  bytes = teggo_min<unsigned>(teggo_max<int>(stream_sz_in_-was_read_in_,0),bytes);
  was_read_in_+=bytes;
  if ( bytes == 0 ) eof_ = true;
  return bytes;
}

// -----------------------------------------------------------------------
unsigned ZipInputSubStream::Read(byte_t* buff,unsigned count) {
  fail_if ( !diskfile_ );
  if ( count+was_read_in_ > stream_sz_in_ ) count = stream_sz_in_-was_read_in_;
  //count = teggo_min<u32_t>(teggo_max<long>(stream_sz_in_-was_read_in_,0),count);
  if ( count == 0 ) { 
    eof_ = true; return 0; 
  }
  //unsigned k = 0;
  if ( !diskfile_ ) {
    Lerr << "zipsubstream: read failed, file is not opened";
    SetECode(SCE_FAIL);
    return 0;
  }
  fail_if_fail ( diskfile_->SeekFd(start_pos_+was_read_in_,SEEK_SET) );
  fail_if_fail ( diskfile_->ReadBuffer_(buff,(u32_t*)&count) );
  was_read_in_ += count;
  return count;
}

// -----------------------------------------------------------------------
ZipInputSubStream::ZipInputSubStream(u32_t pos,u32_t sz,ZipDiskFile* diskfile) 
: diskfile_(diskfile)
{
  start_pos_ = pos;
  stream_sz_in_ = sz;
  was_read_in_ = 0;
  eof_ = false;
}

// -----------------------------------------------------------------------
ZipInputSubStream::~ZipInputSubStream()
{
  if ( !!diskfile_ ) {
    diskfile_->ForgetAbout(this);
    Release(diskfile_);
  }
}

NAMESPACE_MSKIN_END
