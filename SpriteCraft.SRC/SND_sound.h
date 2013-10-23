
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


#if !defined __92BA56DC_FBEF_4C66_B713_A65A2C7437CB__
#define __92BA56DC_FBEF_4C66_B713_A65A2C7437CB__

struct SOUND_INFO
  {
    wchar_t const *klass;
    wchar_t const *resource;
    bool streamed;
    long vol;
  };

struct SOUND_STREAM_INFO
  {
    long  channels;
    long  sampPerSec;
    long  avgBytesPerSec;
    long  blockAlign;
    long  bitsPerSample;
    long  size;
  };

//A7CE385F-825B-4A39-9CE4-F74A9D5B0A50
TEGGO_DECLARE_GUIDOF_(ISoundDevice,0xA7CE385F,0x825B,0x4A39,0x9C,0xE4,0xF7,0x4A,0x9D,0x5B,0x0A,0x50);
//40976F44-F10A-4B98-ABE4-04471A843388
TEGGO_DECLARE_GUIDOF_(ISoundObject,0x40976F44,0xF10A,0x4B98,0xAB,0xE4,0x04,0x47,0x1A,0x84,0x33,0x88);
//DFAB4133-159E-429B-B27E-65B49CE7A037
TEGGO_DECLARE_GUIDOF_(ISoundObjectSubImpl,0xDFAB4133,0x159E,0x429B,0xB2,0x7E,0x65,0xB4,0x9C,0xE7,0xA0,0x37);
//E285330B-993A-4253-B7D7-3BC219C1AECE
TEGGO_DECLARE_GUIDOF_(ISoundStream,0xE285330B,0x993A,0x4253,0xB7,0xD7,0x3B,0xC2,0x19,0xC1,0xAE,0xCE);
//34F17A1D-0139-4B22-B051-F71B6B64C1D2
TEGGO_DECLARE_GUIDOF_(ISoundMgr,0x34F17A1D,0x0139,0x4B22,0xB0,0x51,0xF7,0x1B,0x6B,0x64,0xC1,0xD2);

struct ISoundStream : TeggoInterface<IUnknown>
  {
    virtual HRESULT __stdcall GetInfo(SOUND_STREAM_INFO *sinfo) = 0;
    virtual HRESULT __stdcall NextDataPart( /*out,length(size)*/ char* p, /*inout*/ long *size) = 0;
    virtual HRESULT __stdcall Reset() = 0;
    virtual HRESULT __stdcall Skip(long bytes) = 0;
  };

struct ISoundMgr : TeggoInterface<IUnknown>
  {
    virtual HRESULT __stdcall GetHWND( void *hwndPtr ) = 0;
    virtual HRESULT __stdcall CreateStreamFrom(BSTR klass,ISoundStream **pI ) = 0;
  };

struct ISoundDevice : TeggoInterface<IUnknown>
  {
    virtual HRESULT __stdcall CreateSound(pwide_t name, SOUND_INFO *info, ISoundObject **pI ) = 0;
    virtual HRESULT __stdcall IsSupported(VARIANT_BOOL *bRet) = 0;
    virtual HRESULT __stdcall IsEnabled(VARIANT_BOOL *bRet) = 0;
    virtual HRESULT __stdcall Enable(VARIANT_BOOL bVal) = 0;
    virtual HRESULT __stdcall SetMasterVolume(long vol) = 0;
    virtual HRESULT __stdcall GetMasterVolume(long *vol) = 0;
    virtual HRESULT __stdcall StopAll() = 0;
    virtual HRESULT __stdcall OnResetContext() = 0;
    virtual HRESULT __stdcall Purge() = 0;
    virtual HRESULT __stdcall Enumerate( IObjList **sounds ) = 0;
    virtual HRESULT __stdcall FindSoundByName( BSTR name, ISoundObject **pI ) = 0;
    virtual HRESULT __stdcall Initialize( ISoundMgr *sndmgr ) = 0;
  };

struct ISoundObjectSubImpl : TeggoInterface<IUnknown>
  {
    virtual HRESULT __stdcall SetSoundObjectPtr(ISoundObject*) = 0;
  };

struct ISoundObject : TeggoInterface<IUnknown>
  {
    virtual HRESULT __stdcall GetKlass(BSTR *bstrRet) = 0;
    virtual HRESULT __stdcall Play(VARIANT_BOOL f_loop) = 0;
    virtual HRESULT __stdcall Stop() = 0;
    virtual HRESULT __stdcall IsPlaying(VARIANT_BOOL *bRet) = 0;
    virtual HRESULT __stdcall SetVolume(long vol) = 0;
    virtual HRESULT __stdcall GetVolume(long* vol) = 0;
    virtual HRESULT __stdcall SetPan(long vol) = 0;
    virtual HRESULT __stdcall GetPan(long* vol) = 0;
    virtual HRESULT __stdcall SetFrequency(long vol) = 0;
    virtual HRESULT __stdcall GetFrequency(long* vol) = 0;
    virtual HRESULT __stdcall SetSubImpl(ISoundObjectSubImpl *) = 0;
    virtual HRESULT __stdcall GetSubImpl(ISoundObjectSubImpl **) = 0;
    virtual HRESULT __stdcall Clone(BSTR name,ISoundObject **) = 0;
    virtual HRESULT __stdcall Dispose() = 0;
    virtual HRESULT __stdcall ChainNextPlay(ISoundObject *) = 0;
    virtual HRESULT __stdcall PlayIfNotAlreadyPlayed() = 0;
  };

// менеджер звуковой подсистемы
struct SoundSubsystem_
  {
    /// Захватывает устройство
    static HRESULT Acquire(rcc_ptr<ISoundDevice> device);
    static HRESULT AcquireDxS(void* synch_locker);

    /// Освобождает устройство
    static void Release();
    /// ....
    static HRESULT Restore();
    /// Создает звук на базе класса
    static rcc_ptr<ISoundObject> NewSound(pwide_t name,pwide_t klass);
    /// Находит звук по имени
    static rcc_ptr<ISoundObject> FindSound(pwide_t name);
    /// Удаляет все звуки и самплы
    static HRESULT Purge();
    /// Останавливает проигрывание всех звуков
    static HRESULT StopAll();
    /// Убивает звук
    static HRESULT DisposeSound(pwide_t);
    /// Перечисляет все имеющиеся звуки
    static HRESULT EnumerateSounds(BufferT< rcc_ptr<ISoundObject> > &sounds);
    /// Прверяет что звук разрешен
    static HRESULT IsEnabled(VARIANT_BOOL *);
    /// Разрешает/запрещает звук
    static HRESULT Enable(VARIANT_BOOL);
    /// Прверяет что звук поддерживается устройством
    static HRESULT IsSupported(VARIANT_BOOL *);
    /// Устанавливает базовую громкость в прцентах от максимальной
    static HRESULT SetVolume(long vol);
    /// Получает базовую громкость в прцентах от максимальной
    static HRESULT GetVolume(long* vol);


    struct Instance
      {
        SoundSubsystem_* operator->()
          {
            return 0;
          }
      };

    static ISoundDevice *device_;
  };

static SoundSubsystem_::Instance SndSubsys;

#endif // __92BA56DC_FBEF_4C66_B713_A65A2C7437CB__
