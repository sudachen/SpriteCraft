
#include "stdafx.h"
#include "SND_sound.h"
#include "SND_internal.h"
#include "SST.h"


#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$SND$sndsubsys")
#endif

static HRESULT GenXerror()
  {
    return GenSCWideError(L"Sund Subsystem is not initialized");  
  }

extern ISoundStream* SOUNDS_CreateWAVStream(pwide_t resource,mskin::DataSource* ds);
extern ISoundStream* SOUNDS_CreateOGGStream(pwide_t resource,mskin::DataSource* ds);

struct SoundSubsystem_SoundMgr : TeggoObject< TeggoIfs< ISoundMgr > >
  {

    virtual HRESULT __stdcall GetHWND( void *hwndPtr )
      {
        SCREEN::GetWindowHandle(hwndPtr,sizeof(HWND));
        return S_OK;
      }

    virtual HRESULT __stdcall CreateStreamFrom(BSTR resource,ISoundStream **pI )
      {
        AutoDataSource ds ( RSCMAN::CreateSource(resource) );

        if ( !ds ) 
          return GenSCWideError(
            _S*L"has no sound resource '%?'" %resource );

        u32_t t = ds->PeekLong();
        switch( t ) 
          {
          case 0x46464952: // RIFF
            *pI = SOUNDS_CreateWAVStream(resource,ds);
            return S_OK;
          case 0x5367674f: // OggS
            *pI = SOUNDS_CreateOGGStream(resource,ds);
            return S_OK;
          default:;
          }
        
        return GenSCWideError(
          _S*L"unknown format of sound resource '%?'" %resource );
      }
  };

ISoundDevice *SoundSubsystem_::device_ = 0;

rcc_ptr<ISoundDevice> SOUNDS_CreateDxS(void* global_lock);

HRESULT SOUNDS_LoadInfo(pwide_t klass, SNDinfo *info)
{
  if ( Xnode* xmln = SST->GetKlass(klass) )
    {
      if ( wcsicmp(xmln->GetName(),L"sound") != 0 ) 
        return GenSCWideError(_S*L"klass '%?' is not sound" %klass);

      info->klass = klass; info->klass.Trim();

      if ( pwide_t sample = xmln->GetParam(L"sample") ) 
        info->rsc = sample, info->rsc.Trim();
      else
        return GenSCWideError(_S*L"sample source of '%?' is not specified" %klass);

      if ( pwide_t volume = xmln->GetParam(L"volume") )
        info->vol = teggo::ToLong(volume);
      else if ( pwide_t vol = xmln->GetParam(L"vol") ) 
        info->vol = teggo::ToLong(vol);
      else 
        info->vol = 100;
  
      if ( pwide_t type = xmln->GetParam(L"type") )
        {
          StringT<wchar_t,32> stype = type;
          stype.Trim().ToLower();
          if ( stype == L"static" ) 
            info->streamed = false;
          else if ( stype == L"streamed" ) 
            info->streamed = true;
          else 
            info->streamed = false;
        }
      else
        info->streamed = false;

      return S_OK;
    }
  else
    return GenSCWideError(_S*L"sound '%?' is defined" %klass);
}

/// Захватывает устройство
HRESULT SoundSubsystem_::Acquire(rcc_ptr<ISoundDevice> device)
  {
    if ( device_ )
      {
        device_->Purge();
        Unrefe(device_);
      }
    device_ = Refe(+device);
    if ( device_ && SUCCEEDED(device_->Initialize(+RccPtr(new SoundSubsystem_SoundMgr()))) )
      {
        return S_OK;
      }
    else
      {
        Unrefe(device_);
        return E_FAIL;
      }
  }

HRESULT SoundSubsystem_::AcquireDxS(void* synch_locker)
  {
    rcc_ptr<ISoundDevice> dxs = SOUNDS_CreateDxS(synch_locker);
    return Acquire(dxs);
  }

/// Освобождает устройство
void SoundSubsystem_::Release()
  {
    if ( device_ )
      {
        device_->Purge();
        Unrefe(device_);
      }
  }

/// ....
HRESULT SoundSubsystem_::Restore()
  {
    if ( device_ )
      return device_->OnResetContext();
    else
      return GenXerror();
  }

/// Создает звук на базе класса
rcc_ptr<ISoundObject> SoundSubsystem_::NewSound(pwide_t name,pwide_t klass)
  {
    rcc_ptr<ISoundObject> iso;
    if ( device_ )
      {
        SNDinfo i;
        if ( SUCCEEDED( SOUNDS_LoadInfo(klass,&i) ) )
          {
            SOUND_INFO sinfo = {0};
            i.Fill_SDN_INFO(&sinfo);

            if ( SUCCEEDED( device_->CreateSound(name,&sinfo,&+iso) ) )
              return iso;
          }
        return rcc_ptr<ISoundObject>(0);
      }
    else
      return GenXerror(), rcc_ptr<ISoundObject>(0);
  }

/// Находит звук по имени
rcc_ptr<ISoundObject> SoundSubsystem_::FindSound(pwide_t name)
  {
    rcc_ptr<ISoundObject> iso;
    if ( device_ )
      {
        if ( SUCCEEDED( device_->FindSoundByName((BSTR)name,&+iso) ) )
          return iso;
        else
          return rcc_ptr<ISoundObject>(0);
      }
    else
      return GenXerror(), rcc_ptr<ISoundObject>(0);
  }

/// Удаляет все звуки и самплы
HRESULT SoundSubsystem_::Purge()
  {
    if ( device_ )
      return device_->Purge();
    else
      return GenXerror();
  }

/// Останавливает проигрывание всех звуков
HRESULT SoundSubsystem_::StopAll()
  {
    if ( device_ )
      return device_->StopAll();
    else
      return GenXerror();
  }

/// Убивает звук
HRESULT SoundSubsystem_::DisposeSound(pwide_t name)
  {
    rcc_ptr<ISoundObject> iso;
    if ( device_ )
      {
        if ( SUCCEEDED( device_->FindSoundByName((BSTR)name,&+iso) ) )
          {
            if ( iso )
              return iso->Dispose();
            else
              return S_OK;
          }
        else
          return rcc_ptr<ISoundObject>(0);
      }
    else
      return GenXerror(), rcc_ptr<ISoundObject>(0);
  }

/// Перечисляет все имеющиеся звуки
HRESULT SoundSubsystem_::EnumerateSounds(BufferT< rcc_ptr<ISoundObject> > &sounds)
  {
    return E_FAIL;
  }

/// Прверяет что звук разрешен
HRESULT SoundSubsystem_::IsEnabled(VARIANT_BOOL *bRet)
  {
    if ( device_ )
      return device_->IsEnabled(bRet);
    else
      {
        *bRet = VARIANT_FALSE;
        return S_OK;
      }
  }

/// Разрешает/запрещает звук
HRESULT SoundSubsystem_::Enable(VARIANT_BOOL bVal)
  {
    if ( device_ )
      return device_->Enable(bVal);
    else
      return GenXerror();
  }

/// Прверяет что звук поддерживается устройством
HRESULT SoundSubsystem_::IsSupported(VARIANT_BOOL *bRet)
  {
    if ( device_ )
      return device_->IsSupported(bRet);
    else
      {
        *bRet = VARIANT_FALSE;
        return S_OK;
      }
  }

/// Устанавливает базовую громкость в прцентах от максимальной
HRESULT SoundSubsystem_::SetVolume(long vol)
  {
    if ( device_ )
      return device_->SetMasterVolume(vol);
    else
      return GenXerror();
  }

/// Получает базовую громкость в прцентах от максимальной
HRESULT SoundSubsystem_::GetVolume(long* vol)
  {
    if ( device_ )
      return device_->GetMasterVolume(vol);
    else
      return GenXerror();
  }
