
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

#include "stdafx.h"
#include "SND_sound.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$SND$wavstream")
#endif

struct SOUNDS_WavStream : 
  TeggoObject < TeggoIfs< ISoundStream, TeggoSupportEInfo<ISoundStream> > >
    {
      auto_ref<mskin::DataSource> ds_;
      SOUND_STREAM_INFO info_;
      widestring resource_;
      bool failed_;
      u32_t startPos_;
      u32_t currPos_;

      void HasBeanFailed_() 
        { 
          ds_.release__(); 
          failed_ = true; 
        }

      SCERROR RestoreDsIfNeed_();

      virtual HRESULT __stdcall PeekError();
      virtual HRESULT __stdcall GetInfo(SOUND_STREAM_INFO *sinfo);
      virtual HRESULT __stdcall NextDataPart( /*out,length(size)*/ char* p, /*inout*/ long *size);
      virtual HRESULT __stdcall Reset();
      virtual HRESULT __stdcall Skip(long bytes);
      SOUNDS_WavStream(pwide_t resource,mskin::DataSource* ds);
      virtual ~SOUNDS_WavStream();
    };


extern ISoundStream* SOUNDS_CreateWAVStream(pwide_t resource,mskin::DataSource* ds)
  {
    SOUNDS_WavStream* wave = new SOUNDS_WavStream(resource,Grab(ds));
    if ( wave->ds_->PeekError() == SCE_OK ) 
      return wave;
    else 
      {
        delete wave;
        GenSCWideError(L":WAVStream: error on WAV stream");
        return 0;
      }
  }

HRESULT __stdcall SOUNDS_WavStream::Skip(long sz)
  {
    if ( ds_->Skip(sz) != sz )
      return E_FAIL;
    else
      return S_OK;
  }

HRESULT __stdcall SOUNDS_WavStream::GetInfo(SOUND_STREAM_INFO *sinfo)
  {
    *sinfo = info_;

    return PeekError();
  }

HRESULT __stdcall SOUNDS_WavStream::NextDataPart(char *p, long *size)
  {
    *size = (long)ds_->Read((byte_t*)p,*size);
    currPos_ += *size;

    return PeekError();
  }

HRESULT __stdcall SOUNDS_WavStream::Reset()
  {
    Xdbg( Ldbg << "> SOUNDS_WavStream::Reset()" );
  
    if ( currPos_ != startPos_ ) 
      {
        if ( ds_->Reset() ) goto lb_end;
        if ( ds_->Skip(startPos_) == startPos_ ) goto lb_end;
        currPos_ = startPos_;
      }

  lb_end:
    return PeekError();
  }

HRESULT __stdcall SOUNDS_WavStream::PeekError()
  {
    if ( ds_ )
      {
        return (!failed_ && SCE_OK == eof2ok(ds_->PeekError())) ? S_OK : E_FAIL;
      }
    else
      return E_FAIL;
  }

SOUNDS_WavStream::SOUNDS_WavStream(pwide_t resource,mskin::DataSource* ds) 
  : ds_(ds),resource_(resource),failed_(false)
  {
    
    if (  SCE_OK != RestoreDsIfNeed_() )
      return; /* ôëàã failed_ óñòàíîâëåí */
  
    u32_t riff    = ds_->ReadLong();
    /*u32_t lenh    =*/ ds_->ReadLong();
    u32_t wave    = ds_->ReadLong();
    u32_t fmt     = ds_->ReadLong();
    u32_t fmtlen  = ds_->ReadLong();

    if ( ds_->PeekError() != SCE_OK 
      || riff != 0x46464952 
      || wave != 0x45564157
      || fmt  != 0x20746d66 )
      {
        HasBeanFailed_();
        return_because("invalid wave format");
      }
  
    u32_t was_read = ds_->WasRead();

    /*u16_t wav_tag       =*/ ds_->ReadShort();
    info_.channels         = ds_->ReadShort();
    info_.sampPerSec       = ds_->ReadLong();
    info_.avgBytesPerSec   = ds_->ReadLong();
    info_.blockAlign       = ds_->ReadShort();
    info_.bitsPerSample    = ds_->ReadShort();
    //u16_t wave_size     = ds_->ReadShort();

    if ( ds_->PeekError() != SCE_OK )
      {
        HasBeanFailed_();
        return_because("invalid wave format");
      }

    was_read = ds_->WasRead() - was_read;
  
    if ( fmtlen < was_read ||
      ds_->Skip(was_read-fmtlen) != was_read-fmtlen )
      {
        HasBeanFailed_();
        return_because("bad header of wave file");
      }

    u32_t wave_dataType = ds_->ReadLong();
    info_.size = ds_->ReadLong();
    startPos_ = ds_->WasRead();
    currPos_ = startPos_;

    if ( ds_->PeekError() != SCE_OK ||
      wave_dataType != 0x61746164 )
      {
        HasBeanFailed_();
        return_because("bad header of wave file");
      }

  }

SOUNDS_WavStream::~SOUNDS_WavStream()
  {
  }

SCERROR SOUNDS_WavStream::RestoreDsIfNeed_()
  {
    if ( failed_ ) 
      return SCE_FAIL;

    if ( !ds_ )
      {
        ds_ = AutoRef( RSCMAN::CreateSource(resource_.Str()) );
        if ( !ds_ ) 
          return SCE_FAIL;
      }

    return SCE_OK;
  }

