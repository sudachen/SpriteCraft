
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
#include "../ScraftAPI/spritecraft.h"
#include "SND_sound.h"
#include "SND_internal.h"
#include <mmsystem.h>
#include <dsound.h>
#include <process.h>
#include <math.h>

#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$SND$dxsfactory")
#endif

pwide_t S_not_init = L"is not initialized properly";
pwide_t S_dsnd_err = L"DSound error 0x%08x";

static long VolToDB(long val)
  {
    if ( val > 100 ) val = 100;
    float v = float(val)/100.f;
    return DSBVOLUME_MIN + long(ceil((DSBVOLUME_MAX-DSBVOLUME_MIN)*pow(v,0.1f)));
  }

static long DBToVol(long val)
  {
    float v = float(val-DSBVOLUME_MIN)/(DSBVOLUME_MAX-DSBVOLUME_MIN);
    return long(floor(pow(v,10) * 100.f));
  }

struct SOUNDS_dxs_Object;

struct SOUNDS_dxs_Manager
  {
    virtual void UnmanageObject_(SOUNDS_dxs_Object*) = 0;
    virtual void PlayingObject_(SOUNDS_dxs_Object*) = 0;
    virtual ISoundMgr* GetSndMgr_() = 0;
    virtual SOUNDS_dxs_Object* CloneSound_(pwide_t,SOUNDS_dxs_Object* master) = 0;
  };

struct SOUNDS_dxs_Object :
  TeggoObject<
    TeggoIfs< ISoundObject, TeggoSupportEInfo< ISoundObject > > >
  {

    widestring name_;
    SNDinfo info_;
    rcc_ptr<ISoundStream> stream_;
    rcc_ptr<ISoundObjectSubImpl> subimpl_;
    rcc_ptr<ISoundObject> play_next_object_;

    enum
      {
        CACHE_BLOCK_SZ = 64*1024,
        SOUND_PARTS_NUMBER=8
      };

    SOUNDS_dxs_Manager* manager_;
    IDirectSound* dsound_;
    IDirectSoundBuffer* sbuf_;

    struct
      {
        bool isOk_      :1;
        bool play_loop_ :1;
      };

    long vol_;
    long pan_;
    long freq_;

    u32_t at_;
    u32_t track_length_;
    BufferT<byte_t*> cache_;
    u32_t part_sz_;
    u32_t last_no_;
    int eos_no_;

    bool IsStreamed_();
    bool IsPlaying_();
    bool PumpStream_();

    HANDLE event_;

    HRESULT Play_(u32_t at,bool looping);
    HRESULT Preload_(long pos);
    HRESULT PreloadCache_();
    HRESULT DisposeCache_();

    void ForgetManager_() { manager_ = 0; }
    void Terminate_();
    void Dispose_();

    SOUNDS_dxs_Object(
      pwide_t name, SOUND_INFO const *info,
      SOUNDS_dxs_Manager* m, IDirectSound* dsound,
      HANDLE event,IDirectSoundBuffer* sbmaster);

    virtual ~SOUNDS_dxs_Object();

    HRESULT Initialize_(IDirectSoundBuffer *sbmaster);
    pwide_t GetName();

    HRESULT IsNotOkErr()
      {
        return GenSCWideError(L"invalid sound object");
      }

    ISoundMgr* GetSndMgr_()
      {
        REQUIRE(manager_);
        return manager_->GetSndMgr_();
      }

    void PlayNextIfNeed_()
      {
        if ( play_next_object_ )
          play_next_object_->Play(0);
      }

// ISoundObject
    virtual HRESULT __stdcall GetKlass(BSTR *bstrRet);
    virtual HRESULT __stdcall Continue();
    virtual HRESULT __stdcall Play(VARIANT_BOOL f_loop);
    virtual HRESULT __stdcall Stop();
    virtual HRESULT __stdcall IsPlaying(VARIANT_BOOL *bRet);
    virtual HRESULT __stdcall SetVolume(long vol);
    virtual HRESULT __stdcall GetVolume(long* vol);
    virtual HRESULT __stdcall SetPan(long vol);
    virtual HRESULT __stdcall GetPan(long* vol);
    virtual HRESULT __stdcall SetFrequency(long vol);
    virtual HRESULT __stdcall GetFrequency(long* vol);
    virtual HRESULT __stdcall SetSubImpl(ISoundObjectSubImpl *);
    virtual HRESULT __stdcall GetSubImpl(ISoundObjectSubImpl **);
    virtual HRESULT __stdcall Clone(BSTR name,ISoundObject **);
    virtual HRESULT __stdcall Dispose();
    virtual HRESULT __stdcall ChainNextPlay(ISoundObject *);
    virtual HRESULT __stdcall PlayIfNotAlreadyPlayed();
  };

HRESULT __stdcall SOUNDS_dxs_Object::GetKlass(BSTR *bstrRet)
  {
    *bstrRet = SysAllocString(info_.klass.Str());
    return S_OK;
  }

bool SOUNDS_dxs_Object::IsStreamed_()
  {
    return info_.streamed;
  }

typedef CollectionT< rcc_ptr<SOUNDS_dxs_Object> > SOUNDS_MAP;
typedef BufferT<SOUNDS_dxs_Object*> SOUNDS_VEC;

struct SOUNDS_dxs_Device
  : TeggoObject<
      TeggoIfs< ISoundDevice, TeggoSupportEInfo< ISoundDevice > > >
  , SOUNDS_dxs_Manager
  {

//    enum
//      {
//        MAX_EVENTS_QUEUE_LENGTH = 8
//      };

    struct
      {
        bool isOk_          :1;
        bool enabled_       :1;
        bool stop_pumper_   :1;
      };

    rcc_ptr<ISoundMgr> sndmgr_;
    virtual ISoundMgr* GetSndMgr_()
      {
        return +sndmgr_;
      }

    IDirectSound* dsound_;
    IDirectSoundBuffer* sbPrim_;

    HANDLE event_;
    HANDLE eventThStop_;
    long   thandle_;

    CRITICAL_SECTION* global_lock_;
    static ThreadLocker_ locker_;
    static ThreadLocker_& GetLocker()
      {
        return locker_;
      }

    SOUNDS_MAP sounds_;
    SOUNDS_VEC playing_sounds_;

    void UnmanageAllSounds_();
    virtual void UnmanageObject_(SOUNDS_dxs_Object*);
    virtual void PlayingObject_(SOUNDS_dxs_Object*);

    HRESULT Initialize_();
    HRESULT Terminate_();

    void CheckBuffers_();
    static void __cdecl Pumper(void*);

    virtual SOUNDS_dxs_Object* CloneSound_(pwide_t,SOUNDS_dxs_Object* master);
    SOUNDS_dxs_Object* CreateSound_(pwide_t, SOUND_INFO *info, IDirectSoundBuffer* sbmaster);

    virtual HRESULT __stdcall CreateSound(pwide_t name, SOUND_INFO *info, ISoundObject **pI );
    virtual HRESULT __stdcall IsSupported(VARIANT_BOOL *bRet);
    virtual HRESULT __stdcall IsEnabled(VARIANT_BOOL *bRet);
    virtual HRESULT __stdcall Enable(VARIANT_BOOL bVal);
    virtual HRESULT __stdcall SetMasterVolume(long vol);
    virtual HRESULT __stdcall GetMasterVolume(long *vol);
    virtual HRESULT __stdcall StopAll();
    virtual HRESULT __stdcall OnResetContext();
    virtual HRESULT __stdcall Purge();
    virtual HRESULT __stdcall Enumerate( IObjList **sounds );
    virtual HRESULT __stdcall FindSoundByName( BSTR name, ISoundObject **pI );

    virtual HRESULT __stdcall Initialize( ISoundMgr *sndmgr )
      {
        sndmgr_ = RccRefe(sndmgr);
        isOk_ = Initialize_() == S_OK;
        enabled_ = true;
        return isOk_?S_OK:E_FAIL;
      }

    SOUNDS_dxs_Device(void*);
    virtual ~SOUNDS_dxs_Device();

    HRESULT FailIfNotOk(pwide_t prefix)
      {
        if ( !isOk_ )
          return GenSCWideError( _S*L":%?: %?" %prefix %S_not_init );
        else
          return S_OK;
      }
  };

HRESULT __stdcall SOUNDS_dxs_Device::CreateSound(pwide_t name, SOUND_INFO *info, ISoundObject **pI )
  {
    *pI = Refe(CreateSound_(name,info,0));
    if ( *pI )
      return S_OK;
    else
      return E_FAIL;
  }


rcc_ptr<ISoundDevice> SOUNDS_CreateDxS(void* global_lock)
  {
    return rcc_ptr<ISoundDevice>(new SOUNDS_dxs_Device(global_lock));
  }

ThreadLocker_ SOUNDS_dxs_Device::locker_;

SOUNDS_dxs_Device::SOUNDS_dxs_Device(void* global_lock)
  {
    dsound_ = 0;
    sbPrim_ = 0;
    event_ = 0;
    eventThStop_ = 0;
    stop_pumper_ = 0;
    thandle_ = 0;
    global_lock_ = (CRITICAL_SECTION*)global_lock;
    isOk_ = false;
    enabled_ = false;
  }

SOUNDS_dxs_Device::~SOUNDS_dxs_Device()
  {
    UnmanageAllSounds_();
    Terminate_();
  }

HRESULT __stdcall SOUNDS_dxs_Device::Purge()
  {
    UnmanageAllSounds_();
    return S_OK;
  }

void SOUNDS_dxs_Device::UnmanageObject_(SOUNDS_dxs_Object* obj)
  {Synchronizer_ sync(locker_);
    if ( SOUNDS_dxs_Object **i = playing_sounds_.Find(obj) )
      playing_sounds_.Erase(i);
    sounds_.Erase(obj->GetName());
  }

void SOUNDS_dxs_Device::PlayingObject_(SOUNDS_dxs_Object* obj)
  {Synchronizer_ sync(locker_);
    if ( !playing_sounds_.Find(obj) )
      playing_sounds_.Push(obj);
  }

void SOUNDS_dxs_Device::UnmanageAllSounds_()
  {Synchronizer_ sync(locker_);
    playing_sounds_.Clear();
    sounds_.Clear();
  }

SOUNDS_dxs_Object* SOUNDS_dxs_Device::CreateSound_(
  pwide_t name, SOUND_INFO *info, IDirectSoundBuffer* sbmaster)
  {
    static int auto_name_no = 0;

    StringT<wchar_t,32> nameX(name?name:L"");
    nameX.ToLower();

    if (FAILED(FailIfNotOk(L"CreateSound")))
      return 0;

    if ( !nameX.Length() )
      {
        nameX << _S*L"{AUTO@%s@%ld}" %info->klass %++auto_name_no;
        nameX.ToLower();
      }

    if ( sounds_.Get(nameX.Str()) )
      {
        GenSCWideError(_S*L":CreateSound: sound %? already exists" %nameX);
        return 0;
      }

    SOUNDS_dxs_Object* obj =
      new SOUNDS_dxs_Object(nameX.Str(),info,this,dsound_,event_,sbmaster);

    sounds_.Put(obj->GetName(),RccPtr(obj));

    return obj;
  }

SOUNDS_dxs_Object* SOUNDS_dxs_Device::CloneSound_(pwide_t name,SOUNDS_dxs_Object* master)
  {
    SOUND_INFO i;
    master->info_.Fill_SDN_INFO(&i);
    return CreateSound_(name,&i,master->sbuf_);
  }

HRESULT __stdcall SOUNDS_dxs_Device::FindSoundByName(BSTR name, ISoundObject** pI )
  {
    if ( rcc_ptr<SOUNDS_dxs_Object> *val = sounds_.GetLower(name)  )
      {
        *pI = Refe(+*val);
      }
    else
      *pI = 0;

    return S_OK;
  }

extern HRESULT Engine_NewObjList(IObjList**);

HRESULT __stdcall SOUNDS_dxs_Device::Enumerate(IObjList **sounds)
  {
    Engine_NewObjList(sounds);

    SOUNDS_MAP::Iterator sobji = sounds_.Iterate();
    while ( sobji.Next() )
      {
        (*sounds)->Add( (*sobji)->__iunknown() );
      }

    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Device::IsSupported(VARIANT_BOOL *bRet)
  {
    *bRet = !isOk_? VARIANT_FALSE : VARIANT_TRUE;
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Device::IsEnabled(VARIANT_BOOL *bRet)
  {
    *bRet = !isOk_? VARIANT_FALSE : ( enabled_ ? VARIANT_TRUE : VARIANT_FALSE );
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Device::Enable(VARIANT_BOOL enable)
  {
    if (FAILED(FailIfNotOk(L"SetMasterVolume"))) return E_FAIL;
    enabled_ = enable!=VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Device::SetMasterVolume(long vol)
  {
    if (FAILED(FailIfNotOk(L"SetMasterVolume"))) return E_FAIL;
    return sbPrim_->SetVolume(VolToDB(vol));
  }

HRESULT __stdcall SOUNDS_dxs_Device::GetMasterVolume(long* vol)
  {
    if (FAILED(FailIfNotOk(L"GetMasterVolume"))) return E_FAIL;

    long dxvol = 0;
    sbPrim_->GetVolume(&dxvol);
    *vol = long(DBToVol(dxvol));
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Device::StopAll()
  {
    if (FAILED(FailIfNotOk(L"StopAll"))) return E_FAIL;
    return S_OK;
  }

void SOUNDS_dxs_Device::CheckBuffers_()
  {
    for ( unsigned i = 0; i < playing_sounds_.Count(); )
      {
        SOUNDS_dxs_Object* obj = playing_sounds_[i];

        bool is_playing = obj->IsPlaying_();

        if ( is_playing && obj->IsStreamed_() )
          is_playing = obj->PumpStream_();

        if ( !is_playing )
          {
            obj->PlayNextIfNeed_();
            playing_sounds_.Erase(playing_sounds_.Begin()+i);
          }
        else
          ++i;
      }
  }

void __cdecl SOUNDS_dxs_Device::Pumper(void* p)
  {
    SOUNDS_dxs_Device* fac = (SOUNDS_dxs_Device*)p;
    REQUIRE ( fac->global_lock_ != 0 );

    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);

    for(;;)
      {
        Sleep(30);
        {
          Synchronizer_ sync(fac->locker_);

          if ( fac->stop_pumper_ )
            goto exit;

          fac->CheckBuffers_();
        }
      }

  exit:
    SetEvent( fac->eventThStop_ );
  }

HRESULT SOUNDS_dxs_Device::Initialize_()
  {
    HRESULT hr ;

    if ( !dsound_ )
      if ( FAILED(( hr = DirectSoundCreate(0,&dsound_,0) )) )
        return GenSCWideError(
          _S*L":SOUNDS_dxs_Device: can't intialize sound subsystem (HRESULT:0x%08x)" %hr);

    HWND hWnd = 0;
    SCREEN::GetWindowHandle(&hWnd,sizeof(hWnd));

    if ( FAILED((hr = dsound_->SetCooperativeLevel(hWnd,DSSCL_PRIORITY))) )
      return GenSCWideError(_S*S_dsnd_err %hr);

    if ( !sbPrim_ )
      {
        DSBUFFERDESC dsbdesc;
        memset(&dsbdesc,0,sizeof(dsbdesc));
        dsbdesc.dwSize = sizeof(dsbdesc);
        dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRLVOLUME;
        if ( FAILED((hr = dsound_->CreateSoundBuffer(&dsbdesc, &sbPrim_, 0))) )
          return GenSCWideError(_S*S_dsnd_err %hr);
      }

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    sbPrim_->SetFormat(&wfx);

    event_ = CreateEvent(0,0,0,0);
    eventThStop_ = CreateEvent(0,0,0,0);
    stop_pumper_ = false;

    if ( global_lock_ )
      if ( (thandle_ = _beginthread(SOUNDS_dxs_Device::Pumper,0,this))
           == long(0xffffffff) )
        return GenSCWideError(L"opps... could not start pumper thread");

    return S_OK;
  }

HRESULT SOUNDS_dxs_Device::Terminate_()
  {
    if ( thandle_ )
      {Synchronizer_ sync(locker_);
        TerminateThread((HANDLE)thandle_,0);
        thandle_ = 0;
      }

    Unrefe(sbPrim_);
    Unrefe(dsound_);

    if ( event_ )
      {
        CloseHandle(event_);
        event_ = 0;
      }

    if ( eventThStop_ )
      {
        CloseHandle(eventThStop_);
        eventThStop_ = 0;
      }

    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Device::OnResetContext()
  {
    if ( isOk_ )
      {
        HWND hWnd = 0;
        sndmgr_->GetHWND(&hWnd);
        dsound_->SetCooperativeLevel(hWnd,DSSCL_PRIORITY);
      }
    return S_OK;
  }

HRESULT SOUNDS_dxs_Object::Play_(u32_t at,bool looping)
  {
    if ( !manager_ )
      return GenSCWideError(L"invalid object");

    if ( IsPlaying_() )
      Stop();

    if ( at != 0xffffffff )
      {
        if ( !info_.streamed )
          sbuf_->SetCurrentPosition(at);
        else
          if ( info_.streamed )
            play_loop_ = looping;
      }
    else
      {
        if ( !info_.streamed )
          {
            DWORD status;
            sbuf_->GetStatus(&status);
            looping = (status & DSBSTATUS_LOOPING)!=0;
          }
        else;
          // save value of play_loop_
      }

    if ( info_.streamed && at != 0xffffffff )
      {
        if ( FAILED(Preload_(at)) )
          return E_FAIL;
        // currpos has bean autoset by at
      }

    sbuf_->SetVolume(vol_);
    sbuf_->SetPan(pan_);
    //sbuf_->SetFrequency(freq_);

    HRESULT hr = sbuf_->Play(0,0,looping|info_.streamed?DSBPLAY_LOOPING:0);

    if ( hr == DSERR_BUFFERLOST )
      {
        if ( FAILED(Preload_(/*info_.streamed?at:*/0)) )
          return E_FAIL;
        sbuf_->Play(0,0,looping|info_.streamed?DSBPLAY_LOOPING:0);
      }
    else if ( FAILED(hr) )
      return GenSCWideError( _S*S_dsnd_err %hr );

    {Synchronizer_ sync(SOUNDS_dxs_Device::GetLocker());
      manager_->PlayingObject_(this);
    }

    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Object::Play(VARIANT_BOOL f_loop)
  {
    if ( isOk_ )
      return Play_(0,f_loop != 0);
    else
      return IsNotOkErr();
  }

HRESULT __stdcall SOUNDS_dxs_Object::PlayIfNotAlreadyPlayed()
  {
    if ( IsPlaying_() )
      return Play(0);
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Object::Stop()
  {
    if ( isOk_ )
      {Synchronizer_ sync(SOUNDS_dxs_Device::GetLocker());
        if ( HRESULT hr = sbuf_->Stop() )
          return GenSCWideError( _S*S_dsnd_err %hr );
        else
          return S_OK;
      }
    else
      return IsNotOkErr();
  }

HRESULT __stdcall SOUNDS_dxs_Object::Continue()
  {
    if ( isOk_ )
      if ( IsPlaying_() )
        return S_OK;
    return Play_(0xffffffff,0);
  }

bool SOUNDS_dxs_Object::IsPlaying_()
  {
    if ( !isOk_ )
      return false;
    u32_t status;
    if ( !sbuf_ || sbuf_->GetStatus(&status) != S_OK )
      return false;
    return (status & DSBSTATUS_PLAYING) != 0;
  }

HRESULT __stdcall SOUNDS_dxs_Object::IsPlaying(VARIANT_BOOL *bRet)
  {
    *bRet = IsPlaying_();
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Object::SetVolume(long vol)
  {
    if ( isOk_ )
      {
        vol_ = VolToDB(vol);
        if ( IsPlaying_() )
          sbuf_->SetVolume(vol_);
        return S_OK;
      }
    else
      return IsNotOkErr();
  }

HRESULT __stdcall SOUNDS_dxs_Object::GetVolume(long *vol)
  {
    if ( isOk_ )
      {
        VIOLATION ( !vol , L"GetVolume argument shuld be not null" );
        *vol = DBToVol(vol_);
        return S_OK;
      }
    else
      return IsNotOkErr();
  }

HRESULT __stdcall SOUNDS_dxs_Object::SetPan(long pan)
  {
    if ( isOk_ )
      {
        long sign = pan > 0 ? 1 : -1;
        if ( pan > 100 ) pan = 100;
        if ( pan < -100 ) pan = -100;

        if ( pan == 0 )
          pan_ = 0;
        else
          pan_ = VolToDB(100-abs(pan)) * -sign;

        if ( IsPlaying_() )
          sbuf_->SetPan(pan_);

        return S_OK;
      }
    else
      return IsNotOkErr();
  }

HRESULT __stdcall SOUNDS_dxs_Object::GetPan(long* pan)
  {
    if ( isOk_ )
      {
        VIOLATION ( !pan , L"GetPan argument shuld be not null" );
        long sign = pan_ > 0 ? 1 : pan_ < 0 ? -1 : 0;
        *pan = (100-DBToVol(-abs(pan_))) * sign;
        return S_OK;
      }
    else
      return IsNotOkErr();
  }

HRESULT __stdcall SOUNDS_dxs_Object::SetFrequency(long freq)
  {
    if ( isOk_ )
      {
        if ( freq > DSBFREQUENCY_MAX ) freq = DSBFREQUENCY_MAX;
        if ( freq < DSBFREQUENCY_MIN ) freq = DSBFREQUENCY_MIN;
        freq_ = freq;

        if ( IsPlaying_() )
          sbuf_->SetFrequency(freq_);

        return S_OK;
      }
    else
      return IsNotOkErr();
  }

HRESULT SOUNDS_dxs_Object::GetFrequency(long* freq)
  {
    if ( isOk_ )
      {
        VIOLATION ( !freq , L"GetFrequency argument shuld be not null" );
        *freq = freq_;
        return S_OK;
      }
    else
      return IsNotOkErr();
  }

SOUNDS_dxs_Object::SOUNDS_dxs_Object(
  pwide_t name,
  SOUND_INFO const *info,
  SOUNDS_dxs_Manager* m, IDirectSound* dsound,
  HANDLE event,IDirectSoundBuffer* sbmaster)
  : name_(name),info_(info),manager_(m),dsound_(dsound),event_(event)
  {
    sbuf_ = 0;
    vol_ = 0;
    pan_ = 0;
    freq_ = 0;
    at_ = 0;
    track_length_ = 0;
    isOk_ = SUCCEEDED(Initialize_(sbmaster));
    if ( !isOk_ ) Terminate_();
  }

HRESULT SOUNDS_dxs_Object::PreloadCache_()
  {
    HRESULT hr = S_OK;
    if( FAILED((hr = stream_->Reset())) ) return hr;
    if( FAILED((hr = DisposeCache_())) ) return hr;

    for (;;)
      {
        cache_.Push(new byte_t[CACHE_BLOCK_SZ]);
        long was_read = CACHE_BLOCK_SZ;

        if(FAILED((hr = stream_->NextDataPart((char*)cache_.Last(),&was_read))))
          return hr;

        if ( was_read != CACHE_BLOCK_SZ )
          {
            track_length_ = was_read+(cache_.Count()-1)*CACHE_BLOCK_SZ;
            break;
          }
      }
    return S_OK;
  }

HRESULT SOUNDS_dxs_Object::DisposeCache_()
  {
    if ( !cache_.Empty() )
      for ( u32_t i=0; i < cache_.Count(); ++i )
        delete[] cache_[i];
    cache_.Clear();
    return S_OK;
  }

HRESULT SOUNDS_dxs_Object::Initialize_(IDirectSoundBuffer* sbmaster)
  {
    HRESULT hr;

    if ( !sbmaster || info_.streamed )
      {
        WAVEFORMATEX wfx = {0,};
        DSBUFFERDESC dsbd = {0,};

        if ( FAILED(( hr = GetSndMgr_()->CreateStreamFrom((BSTR)info_.rsc.Str(),&+stream_) )) )
          return hr;

        SOUND_STREAM_INFO snd_info;
        stream_->GetInfo(&snd_info);

        if ( !info_.streamed )
          {
            if( FAILED((hr = PreloadCache_())) )
              return hr;
          }
        else
          track_length_ = 0;

        dsbd.dwSize = sizeof(dsbd);

        if ( info_.streamed ) dsbd.dwBufferBytes =
          teggo_max<u32_t>(
            (snd_info.avgBytesPerSec/2+4095)&+4095,
            4096*SOUND_PARTS_NUMBER);
        else
          dsbd.dwBufferBytes = track_length_;

        part_sz_ =  dsbd.dwBufferBytes/SOUND_PARTS_NUMBER;
        wfx.cbSize = sizeof(wfx);
        wfx.nAvgBytesPerSec = snd_info.avgBytesPerSec;
        wfx.nBlockAlign     = snd_info.blockAlign;
        wfx.nChannels       = snd_info.channels;
        wfx.nSamplesPerSec  = snd_info.sampPerSec;
        wfx.wBitsPerSample  = snd_info.bitsPerSample;
        wfx.wFormatTag      =  WAVE_FORMAT_PCM;
        dsbd.lpwfxFormat = &wfx;
        dsbd.dwFlags =
          DSBCAPS_GETCURRENTPOSITION2|
          DSBCAPS_CTRLVOLUME|
          DSBCAPS_CTRLPAN|
          DSBCAPS_CTRLFREQUENCY|
          DSBCAPS_GLOBALFOCUS;
        if ( FAILED((hr = dsound_->CreateSoundBuffer(&dsbd,&sbuf_,0))) )
          return GenSCWideError(_S*S_dsnd_err %hr);
      }
    else
      if ( FAILED((hr = dsound_->DuplicateSoundBuffer(sbmaster,&sbuf_))) )
        return GenSCWideError(_S*S_dsnd_err %hr);

    sbuf_->GetFrequency( (u32_t*)&freq_ );
    vol_ = VolToDB(info_.vol);
    pan_ = 0;

    sbuf_->SetCurrentPosition(0);

    if ( info_.streamed || !sbmaster )
      {
        if( FAILED((hr = Preload_(0))) )
          return hr;

        DisposeCache_();
        if ( !info_.streamed ) Unrefe(stream_);
      }

    return S_OK;
  }

void SOUNDS_dxs_Object::Terminate_()
  {
    dsound_ = 0;
    Unrefe(stream_);
    Unrefe(sbuf_);
  }

HRESULT SOUNDS_dxs_Object::Preload_(long pos)
  {
    REQUIRE ( sbuf_ );

    byte_t* ptr = 0;
    long len = 0;
    long len0 = 0;

    HRESULT hr = sbuf_->Lock(0,0,(void**)&ptr,(LPDWORD)&len,0,0,DSBLOCK_ENTIREBUFFER);

    if ( hr == DSERR_BUFFERLOST )
      {
        if ( FAILED(( hr = sbuf_->Restore() )) )
          return GenSCWideError(_S*S_dsnd_err %hr);
        if ( FAILED(( hr = sbuf_->Lock(0,0,(void**)&ptr,(LPDWORD)&len,0,0,DSBLOCK_ENTIREBUFFER) )) )
          return GenSCWideError(_S*S_dsnd_err %hr);
      }
    else if ( FAILED(hr) )
      return GenSCWideError(_S*S_dsnd_err %hr);

    hr = S_OK;

    len0 = len;
    if ( !info_.streamed )
      {
        REQUIRE ( track_length_ == len0 );
        if ( !cache_.Empty() )
          {
            for ( u32_t i = 0 ; i < cache_.Count(); ++i )
              {
                memcpy(ptr+i*CACHE_BLOCK_SZ,cache_[i],teggo_min<size_t>(len0,CACHE_BLOCK_SZ));
                len0-=CACHE_BLOCK_SZ;
              }
          }
        else
          {
            if ( !stream_ )
              if ( FAILED(( hr = GetSndMgr_()->CreateStreamFrom((BSTR)info_.rsc.Str(),&+stream_) )) )
                return hr;

            stream_->Reset();
            hr = stream_->NextDataPart((char*)ptr,&len0);
            Unrefe( stream_ );
          }
      }
    else
      {
        eos_no_ = -1;
        len0 = len;
        stream_->Reset();
        if ( pos != 0 )
          stream_->Skip(pos);
        hr = stream_->NextDataPart((char*)ptr,&len0);

        last_no_ = 0;
        if ( len0 < len-part_sz_ )
          eos_no_ = (len0+part_sz_-1)/part_sz_;
      }

    sbuf_->Unlock(ptr,len,0,0);
    if ( info_.streamed )
      sbuf_->SetCurrentPosition(0);

    return hr;
  }

bool SOUNDS_dxs_Object::PumpStream_()
  {
    if ( ! isOk_ ) return false;

    REQUIRE ( info_.streamed );

    u32_t pos;
    sbuf_->GetCurrentPosition(&pos,0);

    int cur_no = pos/part_sz_;
    if ( cur_no == eos_no_ )
      {
        sbuf_->Stop();
        eos_no_ = -1;
        return false;
      }

    while ( cur_no != (int)last_no_ && eos_no_ < 0 )
      {
        byte_t* ptr = 0;
        long len = part_sz_;
        long len0 = 0;
        HRESULT hr = sbuf_->Lock(last_no_*part_sz_,part_sz_,(void**)&ptr,(LPDWORD)&len,0,0,0);

        if ( hr == DSERR_BUFFERLOST )
          {
            if ( FAILED(( hr = sbuf_->Restore() )) )
              {
                GenSCWideError(_S*S_dsnd_err %hr);
                return false;
              }
            sbuf_->Lock(last_no_*part_sz_,part_sz_,(void**)&ptr,(LPDWORD)&len,0,0,0);
          }
        else if ( hr )
          {
            GenSCWideError(_S*S_dsnd_err %hr);
            Lerr << _S*"[SCRAFT] failed(%08x) to lock sound buffer " %hr;
            return false;
          }

        len0 = len;

        HRESULT read_err = S_OK;

        read_err = stream_->NextDataPart((char*)ptr,&len0);
        if ( SUCCEEDED(read_err) && len0 < len )
          {
            if ( play_loop_ )
              {
                stream_->Reset();
                len0 = len-len0;
                read_err = stream_->NextDataPart((char*)ptr,&len0);
              }
            else if ( len0 == 0 )
              {
                eos_no_ = last_no_;
                memset(ptr+len0,0,len);
              }
          }
        last_no_ = (last_no_+1) % SOUND_PARTS_NUMBER;
        sbuf_->Unlock(ptr,len,0,0);
        sbuf_->GetCurrentPosition(&pos,0);
        cur_no = pos/part_sz_;
        if ( FAILED(read_err) )
          {
            GenSCWideError(L":PumpStream_(): error on reading stream");
            Lerr << _S*"[SCRAFT] failed to read sound stream";
            return false;
          }
      }
    return true;
  }

HRESULT __stdcall SOUNDS_dxs_Object::Clone(BSTR name,ISoundObject **pI )
  {
    *pI = 0;
    if ( manager_ )
      {
        *pI = Refe(manager_->CloneSound_(name,this));
        return *pI?S_OK:E_FAIL;
      }
    else
      return GenSCWideError(L"invalid object");
  }

HRESULT __stdcall SOUNDS_dxs_Object::SetSubImpl(ISoundObjectSubImpl *subimpl)
  {
    rcc_ptr<ISoundObjectSubImpl> pI = RccRefe(subimpl);
    if ( subimpl_ )
      subimpl_->SetSoundObjectPtr(0);
    if ( pI )
      pI->SetSoundObjectPtr(this);
    subimpl_ = pI;
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Object::GetSubImpl(ISoundObjectSubImpl **pI)
  {
    *pI = Refe(+subimpl_);
    return S_OK;
  }

pwide_t SOUNDS_dxs_Object::GetName()
  {
    return name_.Str();
  }

void SOUNDS_dxs_Object::Dispose_()
  {
    SetSubImpl(0);
    DisposeCache_();
    Terminate_();
    if ( manager_ )
      manager_->UnmanageObject_(this);
  }

HRESULT SOUNDS_dxs_Object::Dispose()
  {
    Synchronizer_ sync(SOUNDS_dxs_Device::GetLocker());
    Dispose_();
    return S_OK;
  }

HRESULT __stdcall SOUNDS_dxs_Object::ChainNextPlay(ISoundObject *chained_object)
  {
    play_next_object_ = RccRefe(chained_object);
    ((SOUNDS_dxs_Object*)chained_object)->vol_ = vol_;
    ((SOUNDS_dxs_Object*)chained_object)->pan_ = pan_;
    return S_OK;
  }

SOUNDS_dxs_Object::~SOUNDS_dxs_Object()
  {
    Dispose_();
  }
