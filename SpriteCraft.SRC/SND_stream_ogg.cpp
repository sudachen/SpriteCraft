
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

#include "stdafx.h"
#include "SND_sound.h"
#include "../../Media/lib.oggvorbis/include/vorbisfile.h"
#include <errno.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$SND$oggstream")
#endif

#define BITS_PER_SAMPLE  16
#define BYTES_PER_SAMPLE (BITS_PER_SAMPLE/8)

static size_t VORBIS_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
  {
    size_t was_read = 0;
    mskin::DataSource* ds = (mskin::DataSource*)datasource;
    broken_if ( !ds );
    was_read = ds->Read((byte_t*)ptr,size*nmemb);
    broken_if_fail ( eof2ok( ds->PeekError() ) );
  exit:
    return was_read;
  broken:
    was_read = 0;
    errno = -1;
    goto exit;
  }

static int VORBIS_close_func(void*) 
  { 
    return 0; 
  }

static int VORBIS_seek_func(void*, ogg_int64_t, int) 
  { 
    return -1;
  }

static long VORBIS_tell_func(void *datasource)
  {
    mskin::DataSource* ds_ = (mskin::DataSource*)datasource;
    ret0_if ( !ds_ );
    return ds_->WasRead();
  }


struct SOUNDS_OggStream : 
  TeggoObject < TeggoIfs< ISoundStream, TeggoSupportEInfo<ISoundStream> > >
    {
      auto_ref<mskin::DataSource> ds_;
      
      SOUND_STREAM_INFO info_;
      widestring resource_;
      bool failed_;
      OggVorbis_File ovf_;
      bool ovf_isOk_;
      int curr_section_;
      bool was_read_;

      void HasBeanFailed_() 
        { 
          ds_.release__(); 
          failed_ = true; 
        }

      virtual HRESULT __stdcall PeekError();
      virtual HRESULT __stdcall GetInfo(SOUND_STREAM_INFO *sinfo);
      virtual HRESULT __stdcall NextDataPart( /*out,length(size)*/ char* p, /*inout*/ long *size);
      virtual HRESULT __stdcall Reset();
      virtual HRESULT __stdcall Skip(long bytes);

      SOUNDS_OggStream(pwide_t resource,mskin::DataSource* ds);
      virtual ~SOUNDS_OggStream();
    };

extern ISoundStream* SOUNDS_CreateOGGStream(pwide_t resource,mskin::DataSource* ds)
  {
    SOUNDS_OggStream* oggS = new SOUNDS_OggStream(resource,Grab(ds));
    if ( oggS->PeekError() == SCE_OK ) 
      return oggS;
    else 
      {
        delete oggS;
        GenSCWideError(L":OggStream: error on OGG stream");
        return 0;
      }
  }

HRESULT __stdcall SOUNDS_OggStream::Skip(long sz)
  {
    if ( ov_raw_seek(&ovf_,ov_raw_tell(&ovf_)+sz) < 0 )
      return 
        GenSCWideError(L"out of stream");
    else
      return S_OK;
  }


HRESULT __stdcall SOUNDS_OggStream::GetInfo(SOUND_STREAM_INFO *sinfo)
  {
    *sinfo = info_;
    return PeekError();
  }

HRESULT __stdcall SOUNDS_OggStream::NextDataPart(char *p, /*in,out*/long *size)
  {
    was_read_ = true;
    if ( eof2ok(ds_->PeekError()) != SCE_OK )
      {
        HasBeanFailed_();
        return PeekError();
      }
    u32_t size_left = *size;
    while ( size_left != 0 ) 
      {
        long k = ov_read(&ovf_,(char*)p+(*size-size_left),size_left,0,BYTES_PER_SAMPLE,1,&curr_section_);
        if ( k == 0 ) break; // endofstream
        if ( k < 0 ) 
          {
            HasBeanFailed_();
            return PeekError();
          }
        REQUIRE ( k <= (long)size_left );
        size_left -= k;
      }
    *size -= size_left;
    return PeekError();
  }

HRESULT __stdcall SOUNDS_OggStream::Reset()
  {
    Xdbg( Ldbg << "> SOUNDS_OggStream::Reset()" );

    if ( ovf_isOk_ && !was_read_ ) 
      return S_OK;

    if ( FAILED(ds_->Reset()) ) 
      return E_FAIL;

    if ( ovf_isOk_ ) 
      ov_clear(&ovf_);

    ovf_isOk_ = false;
    ov_callbacks ovc;

    ovc.close_func = VORBIS_close_func;
    ovc.read_func  = VORBIS_read_func;
    ovc.seek_func  = VORBIS_seek_func;
    ovc.tell_func  = VORBIS_tell_func;

    if ( ov_open_callbacks(ds_,&ovf_,0,0,ovc) != 0 ) 
      {
        HasBeanFailed_();
        return GenSCWideError(L"open error on OGG stream");
      }

    ovf_isOk_ = true;
    was_read_ = false;
    return S_OK;
  }

HRESULT __stdcall SOUNDS_OggStream::PeekError()
  {
    if( ds_ )
      {
        return (!failed_ && SCE_OK == eof2ok(ds_->PeekError()) ) ? S_OK : SCE_FAIL;
      }
    else
      return GenSCWideError(L"here is no data stream");
  }

SOUNDS_OggStream::SOUNDS_OggStream(pwide_t resource,mskin::DataSource* ds) 
  : ds_(ds),resource_(resource),failed_(false)
  {
    ov_callbacks ovc;
    ovc.close_func = VORBIS_close_func;
    ovc.read_func  = VORBIS_read_func;
    ovc.seek_func  = VORBIS_seek_func;
    ovc.tell_func  = VORBIS_tell_func;

    ovf_isOk_ = false;

    if ( ov_open_callbacks(ds,&ovf_,0,0,ovc) != 0 ) 
      {
        HasBeanFailed_();
        GenSCWideError(L"open error on OGG stream");
        return;
      }

    ovf_isOk_ = true;

    vorbis_info* vi = ov_info(&ovf_,-1);
    if ( !vi )  
      {
        HasBeanFailed_();
        GenSCWideError(L"info fetching error on OGG stream");
        return;
      }

    info_.channels         = vi->channels;
    info_.sampPerSec       = vi->rate;
    info_.bitsPerSample    = BITS_PER_SAMPLE;
    info_.blockAlign       = (info_.bitsPerSample/8)*vi->channels;
    info_.avgBytesPerSec   = vi->rate*(info_.bitsPerSample/8)*vi->channels;
    //u16_t wave_size     = ds_->ReadShort();

    was_read_ = false;

    if ( eof2ok(ds_->PeekError()) != SCE_OK )
      {
        HasBeanFailed_();
        GenSCWideError(L"invalid OGG format");
        return;
      }
  }

SOUNDS_OggStream::~SOUNDS_OggStream()
  {
    if (ovf_isOk_) 
      ov_clear(&ovf_);  
  }

