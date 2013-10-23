
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

#if !defined __SpriteCraft_SoundMgr_h__
#define __SpriteCraft_SoundMgr_h__

#include "SND_sound.h"

struct SoundMgr;
struct SoundMgr_Sound;
struct SoundMgr_Channel;

namespace teggo
{
  template < class _Tx, class _Mh >
    inline bool com_bcall(_Tx *o, _Mh mh)
      {
        VARIANT_BOOL b;
        (o->*mh)(&b);
        return b != VARIANT_FALSE;
      }
}

struct SoundMgr_Sound :
  TeggoObject<
    TeggoIfs< ISoundObjectSubImpl > >
  {
    widestring              sndname_;
    long                    cookie_;
    ISoundObject*           sndobj_;
    rcc_ptr<IUnknown>       listener_;
    rcc_ptr<SoundMgr_Sound> chain_next_;

    mem_ptr< SlistT<widestring> > chain_;

    struct
      {
        bool available_ : 1;
        bool preloaded_ : 1;
        bool play_loop_ : 1;
        //bool chaned_in_ : 1;
      };


    bool IsPlaying()
      {
        return sndobj_ && teggo::com_bcall(sndobj_,&ISoundObject::IsPlaying);
      }

    SoundMgr_Sound();
    ~SoundMgr_Sound();

    void Dispose()
      {
        if ( sndobj_ )
          {
            sndobj_->Dispose(); sndobj_ = 0;
          }
      }

    HRESULT __stdcall SetSoundObjectPtr(ISoundObject* so)
      {
        sndobj_ = so;
        return S_OK;
      }

    void Notify(long channel,bool interrupted,SoundMgr *);
    bool OnStopNotifyUpdateChain(long channel,SoundMgr *);
    HRESULT ChainForNextPlay(pwide_t snd_name,SoundMgr *);
    HRESULT ChainSoundObjectFor(pwide_t snd_name,SoundMgr *);

    HRESULT Initilaize(pwide_t sndname,IUnknown *listener,ISoundObject* master=0);
    SoundMgr_Sound* Clone(IUnknown *listener);
    HRESULT SetVolAndPan(long vol,long pan);
    HRESULT Play(bool f_loop = false);
    HRESULT Stop();
  };

struct SoundMgr_Channel
  {
    long no;
    long volume_;
    long pan_;
    typedef SlistT< rcc_ptr<SoundMgr_Sound> > sounds_t;
    sounds_t sounds_;
    SoundMgr* mgr_;

    struct
      {
        bool mute_ : 1;
      };

    SoundMgr_Channel(long _no,SoundMgr *_mgr);
    ~SoundMgr_Channel();
    void CheckForStopped();
    void Purge();

    HRESULT PlaySoundOn(BSTR soundName,IUnknown *listener,long cookie,bool in_loop = false);
    HRESULT NextSoundOn(BSTR soundName,long cookie);
    HRESULT RestVolAndPan();
    HRESULT RsetMute(bool mute);
  };

struct SoundMgr
  {
    enum { MAX_LRU_SIZE = 16 };

    typedef BufferT<SoundMgr_Channel*> channels_t;
    typedef SlistT< rcc_ptr<SoundMgr_Sound> > lru_t;

    channels_t channels_;
    lru_t lru_;
    bool invert_stereo_;

    struct LessChannel
      {
        bool operator()(SoundMgr_Channel* c,long no) const
          {
            return c->no < no;
          }
        bool operator()(long no,SoundMgr_Channel* c) const
          {
            return no < c->no;
          }
      };

    rcc_ptr<SoundMgr_Sound> FindInLRU(pwide_t name,IUnknown *listener);
    rcc_ptr<SoundMgr_Sound> FindInPreloaded(pwide_t name,IUnknown *listener);
    rcc_ptr<SoundMgr_Sound> FindInChannels(pwide_t name,IUnknown *listener);
    void MoveToLRU(rcc_ptr<SoundMgr_Sound> snd);
    void Purge();
    SoundMgr();
    ~SoundMgr();
    void CheckForStopped();
    void StopSound(long channel);
    HRESULT PlaySoundOn(long channel,BSTR soundName,IUnknown *listener,long cookie, bool in_loop);
    HRESULT NextSoundOn(long channel,BSTR soundName,long cookie);
    SoundMgr_Channel* RequestChannel(long channel,bool createIfNotExists);
    void SetChannelPan(long channel,long pan);
    long GetChannelPan(long channel);
    void SetChannelVolume(long channel,long vol);
    long GetChannelVolume(long channel);
    void LogSoundsList();
    bool GetMuteStatus(long channel);
    void Mute(long channel,bool mute);
    void InvertStereo(bool inv);
    bool GetInvertStereoStatus()
      {
        return invert_stereo_;
      }
  };

#endif // __SpriteCraft_SoundMgr_h__
