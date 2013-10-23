
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
#include "m_xxconf.h"
#include <stdarg.h>
#include "../mcore/m_vsnprintf.h"
#include "m_streams.h"
//#include "mskin_guards.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif


#if MSKIN_HAS_FILESYSTEM

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL

# define BAD_FILE_VALUE -1
# undef errno
# define errno 0
# undef strerror
# define strerror(x) ""

#else // generic implementation

#define BAD_FILE_VALUE  0

# if MSKIN_TARGET_PLATFORM_EX != MSKIN_PLATFORM_EX_WCE
#  include <sys/stat.h>
#  include <errno.h>
//#if CXX_COMPILER__IS_MSVC_BUNY
//# undef _stat
//# undef _wstat
//# define _stat _stat
//# define _wstat _wstat
//# define _Stat _stat
//# define _Wstat _wstat
//extern "C" int _stat( pchar_t path, void *buffer );
//extern "C" int _wstat( pwide_t path, void *buffer );
//#else
# define _Stat _stat
# define _Wstat _wstat
//#endif
# else
#  define errno 0
#  define strerror(x) ""
# endif
# include <stdio.h>
# endif
# if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
//#  include <windows.h>
# endif


#endif

NAMESPACE_MSKIN_BEGIN

#if MSKIN_HAS_FILESYSTEM

FileInputStream::FileInputStream(pwide_t path) 
: path_(path),eof_(false) 
{
  hf_ = _wfopen(path,L"rb");
  if ( hf_ == BAD_FILE_VALUE ) {
    Lerr << _S*"finstream: can't open file %?, reason: %?" %path %strerror(errno);
    SetECode(SCE_FAIL);
  }
}

FileInputStream::~FileInputStream() 
{
  if ( hf_ ) { fclose(hf_); hf_ = BAD_FILE_VALUE; }
}


unsigned FileInputStream::Read(byte_t* dest,unsigned count)
{
  retval_if ( ecode_ != SCE_OK , 0 );
  retval_if ( hf_ == BAD_FILE_VALUE , 0 ); 
  unsigned processed = 0;
  if ( eof_ ) return 0;
  while ( processed < count ) {
    int k = fread ( dest+processed,1,count-processed,hf_);
    if ( k == 0 && feof(hf_) ) {
        eof_ = true;
        break;
    }
    else if ( k < 0 ) {
      Lerr << _S*"finstream: read error on file %?, reason: %?" %path_ %strerror(ferror((FILE*)hf_));
      SetECode(SCE_FAIL);
      return 0;
    }
    processed += k;
  }
  return processed;
}

bool FileInputStream::EndOfStream() const
{
  return eof_;
}

bool FileInputStream::Reset()
{
  retval_if ( hf_ == BAD_FILE_VALUE , false ); 
  fseek(hf_,0,0);
  eof_ = false;
  return true;
}

unsigned FileInputStream::Skip(unsigned u)
{
  retval_if ( hf_ == BAD_FILE_VALUE , false ); 
  if ( !eof_ ) {
    long posf = ftell(hf_);
    fseek(hf_,0,SEEK_END);
    long endf = ftell(hf_);
    u = teggo_max<long>(0,teggo_min<long>(u,endf - posf));
    fseek(hf_,posf+u,SEEK_SET);
  }
  return u;
}

unsigned FileInputStream::WasRead() const
{
  retval_if ( hf_ == BAD_FILE_VALUE , false ); 
  return ftell(hf_);
}

bool MOLE_FUNCALL IsFileExists(pwide_t path)
{
 #if MSKIN_TARGET_PLATFORM_EX != MSKIN_PLATFORM_EX_WCE
  struct _stat s;
  return ( _Wstat(path,&s) == 0 && (s.st_mode & S_IFREG) );
 #else
  int len = strlen(path);
  BufferT<u16_t> unipath(len+1);
  retval_if(MultiByteToWideChar(CP_ACP,0,path,len,&unipath[0],unipath.size()),false);
  unipath[len] = 0;
  DWORD attr = GetFileAttributes(&unipath[0]);
  return ((attr & FILE_ATTRIBUTE_NORMAL) || (attr & FILE_ATTRIBUTE_READONLY)) &&
    !(attr & FILE_ATTRIBUTE_DIRECTORY); 
 #endif
}

bool MOLE_FUNCALL IsFileReadable(pwide_t path)
{
 #if MSKIN_TARGET_PLATFORM_EX != MSKIN_PLATFORM_EX_WCE
  struct _stat s;
  return ( _Wstat(path,&s) == 0 && (s.st_mode & S_IFREG) && ( s.st_mode & S_IREAD) );
 #else
  int len = strlen(path);
  BufferT<u16_t> unipath(len+1);
  retval_if(!MultiByteToWideChar(CP_ACP,0,path,len,&unipath[0],unipath.size()),false);
  unipath[len] = 0;
  DWORD attr = GetFileAttributes(&unipath[0]);
  return ((attr & FILE_ATTRIBUTE_NORMAL) || (attr & FILE_ATTRIBUTE_READONLY)) &&
    !(attr & FILE_ATTRIBUTE_DIRECTORY); 
 #endif
}

bool MOLE_FUNCALL IsFileRewritable(pwide_t path)
{
 #if MSKIN_TARGET_PLATFORM_EX != MSKIN_PLATFORM_EX_WCE
  struct _stat s;
  return ( _Wstat(path,&s) == 0 && (s.st_mode & S_IFREG) && ( s.st_mode & S_IWRITE )  && ( s.st_mode & S_IREAD ));
 #else
  int len = strlen(path);
  BufferT<u16_t> unipath(len+1);
  retval_if(!MultiByteToWideChar(CP_ACP,0,path,len,&unipath[0],unipath.size()),false);
  unipath[len] = 0;
  DWORD attr = GetFileAttributes(&unipath[0]);
  return ((attr & FILE_ATTRIBUTE_NORMAL) || (attr & FILE_ATTRIBUTE_READONLY) == 0 ) &&
    !(attr & FILE_ATTRIBUTE_DIRECTORY); 
 #endif
}

widestring MOLE_FUNCALL GetModulePath()
{
#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
  BufferT<wchar_t> unipath(MAX_PATH+1);
  retval_if(!GetModuleFileNameW(GetModuleHandle(0),&unipath[0],MAX_PATH),widestring());
  wchar_t *p = wcsrchr(&unipath[0],'\\');
  if ( p == 0 ) EMERGENCY(L"invalid module name");
  *p = 0;
  return widestring(&unipath[0]);
#else
# error "invalid platforme"
#endif
}

void FileOutputStream::Init(pwide_t path)
{
  own_fd_ = true;
  written_ = 0;
  path_ = path;
  //start_pos_ = 0;
  hf_ = _wfopen(path,L"w+b");
  if ( hf_ == BAD_FILE_VALUE ) {
    Lerr << _S*"foutstream: can't open file %?, reason: %?" %path %strerror(errno);
    SetECode(SCE_FAIL);
  }
}

FileOutputStream::FileOutputStream(pwide_t path)
{
  Init( path );
}

FileOutputStream::FileOutputStream(FILE* fd)
{
  if (fd == BAD_FILE_VALUE) SetECode(SCE_FAIL);
  else {
    own_fd_ = false;
    written_ = 0;
    path_ = L"##-";
    hf_ = fd;
  }
}

FileOutputStream::~FileOutputStream()
{
  if ( own_fd_ && hf_ != BAD_FILE_VALUE ) fclose(hf_);
}

SCERROR FileOutputStream::Write(const byte_t* src,unsigned count)
{
  retval_if ( ecode_ != SCE_OK , ecode_ );
  retval_if ( !hf_ , SCE_FINISHED );
  int writen = 0;
  writen = fwrite(src,1,count,hf_);
  if ( writen < 0 || writen != (signed)count ) {
    Lerr << _S*"foutstream: write file %?, error %?" %path_ %strerror(ferror(hf_));
    return SetECode(SCE_FAIL);
  }
  written_ += count;
  return SCE_OK;
}


SCERROR FileOutputStream::Fill(byte_t val,unsigned count)
{
  retval_if ( ecode_ != SCE_OK , ecode_ );
  retval_if ( !hf_ , SCE_FINISHED );
  for ( unsigned i =0; i < count; ++i ) {
    if ( fputc(val,hf_) == EOF ) 
    {
      Lerr<< _S*"foutstream: fill file %?, error %?" %path_ %strerror(ferror(hf_));
      return SetECode(SCE_FAIL);
    }
    ++written_;
  }
  return SCE_OK;
}

SCERROR FileOutputStream::Flush()
{
  retval_if ( ecode_ != SCE_OK , ecode_ );
  retval_if ( !hf_ , SCE_FINISHED );
  fflush(hf_);
  return SCE_OK;
}

SCERROR FileOutputStream::Finish()
{
  retval_if ( ecode_ != SCE_OK , ecode_ );
  retval_if ( !hf_ , SCE_FINISHED );
  retval_if_fail ( Flush() , ecode_ );
  if ( own_fd_ && hf_ != BAD_FILE_VALUE ) 
  { fclose(hf_); hf_ = BAD_FILE_VALUE; }
  return SCE_OK;
}

unsigned FileOutputStream::WasWritten() const
{
  return written_;
}

SCERROR FileOutputStream::Skip(unsigned k)
{
  if ( fseek(hf_,k,SEEK_CUR) != 0 ) {
    SetECode(SCE_FAIL);
    return ecode_;
  }
  written_+=k;
  return SCE_OK;
}

#endif

NAMESPACE_MSKIN_END
