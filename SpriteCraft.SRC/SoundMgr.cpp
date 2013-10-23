
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
#include "RBGFilter.h"
#include "SoundMgr.h"
#include <limits.h>
#include "../SpriteCraft.SRC/Logout.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Managers$SoundMgr")
#endif

SoundMgr_Sound::SoundMgr_Sound()
  : sndobj_(0), listener_(0),
    preloaded_(false), available_(true), play_loop_(false)
  {

  //  XMessage(("SoundMgr_Sound::SoundMgr_Sound %p",this));
  }

SoundMgr_Sound::~SoundMgr_Sound()
  {
  //  XMessage(("SoundMgr_Sound::~SoundMgr_Sound %p",this));
  }


rcc_ptr<SoundMgr_Sound> SoundMgr::FindInLRU(pwide_t name,IUnknown *listener)
  {
    for (lru_t::Position i = lru_.Begin(); i ; ++i )
      {
        if ( (*i)->sndname_ == name )
          {
            rcc_ptr<SoundMgr_Sound> snd = (*i);
            snd->listener_.reset_(Refe(listener));
            lru_.Erase(i);
            return snd;
          }
      }
    return rcc_ptr<SoundMgr_Sound>(0);
  }

rcc_ptr<SoundMgr_Sound> SoundMgr::FindInPreloaded(pwide_t name,IUnknown *listener)
  {
    return rcc_ptr<SoundMgr_Sound>(0);
  }

rcc_ptr<SoundMgr_Sound> SoundMgr::FindInChannels(pwide_t name,IUnknown *listener)
  {
    for ( channels_t::ValuePtr i = channels_.Begin(); i != channels_.End(); ++i )
      {
        SoundMgr_Channel::sounds_t::Position j = (*i)->sounds_.Begin();
        for ( ; j; ++j )
          if ( (*j)->sndname_ == name )
            return rcc_ptr<SoundMgr_Sound>((*j)->Clone(listener));
      }
    return rcc_ptr<SoundMgr_Sound>(0);
  }

void SoundMgr::MoveToLRU(rcc_ptr<SoundMgr_Sound> snd)
  {
    REQUIRE ( 0 == +snd->listener_ );
    lru_.Append(snd);
    while ( lru_.Count() > MAX_LRU_SIZE )
      {
        lru_.First()->Dispose();
        lru_.Pop();
      }
  //  {for ( lru_t::iterator i = lru_.begin();  i != lru_.end(); ++i )
  //    for ( lru_t::iterator j = i;  ++j != lru_.end(); )
  //    {
  //      if (+(*i) == +(*j)) LogSoundsList();
  //      REQUIRE ( +(*i) != +(*j) );
  //  }}
  }

void SoundMgr::Purge()
  {
    channels_t::ValuePtr i = channels_.Begin(), iE = channels_.End();
    for ( ; i != iE; ++i )
      delete (*i);

    channels_t().Purge();

    lru_t::Position j = lru_.Begin();
    for ( ; j ; ++j )
      (*j)->Dispose();

    lru_t().Purge();
  }

SoundMgr::SoundMgr()
  {
    invert_stereo_ = false;
  }

SoundMgr::~SoundMgr()
  {
    Purge();
  }

void SoundMgr::CheckForStopped()
  {
    channels_t::ValuePtr i = channels_.Begin(), iE = channels_.End();
    for ( ; i != iE; ++i )
      (*i)->CheckForStopped();
  }

void SoundMgr::StopSound(long channel)
  {
    SoundMgr_Channel* c = RequestChannel(channel,false);
    if ( !c )
      return;
    c->Purge();
  }

HRESULT SoundMgr::PlaySoundOn(long channel,BSTR soundName,IUnknown* listener,long cookie,bool in_loop)
  {
    SoundMgr_Channel *c = RequestChannel(channel,true);
    return c->PlaySoundOn(soundName,listener,cookie,in_loop);
  }

HRESULT SoundMgr::NextSoundOn(long channel,BSTR soundName,long cookie)
  {
    SoundMgr_Channel *c = RequestChannel(channel,true);
    return c->NextSoundOn(soundName,cookie);
  }

SoundMgr_Channel* SoundMgr::RequestChannel(long channel,bool createIfNotExists)
  {
    channels_t::ValuePtr i = channels_.Blower(channel,LessChannel());
    if ( i == channels_.End() || (*i)->no != channel )
      if ( createIfNotExists )
        {
          SoundMgr_Channel* c = new SoundMgr_Channel(channel,this);
          channels_.Insert(i,c);
          return c;
        }
      else
        return 0;
    else
      return (*i);
  }

void SoundMgr::SetChannelPan(long channel,long pan)
  {
    SoundMgr_Channel* c = RequestChannel(channel,true);
    c->pan_ = pan;
    c->RestVolAndPan();
  }

long SoundMgr::GetChannelPan(long channel)
  {
    SoundMgr_Channel* c = RequestChannel(channel,false);
    if (c) return c->pan_; else return 0;
  }

void SoundMgr::SetChannelVolume(long channel,long vol)
  {
    SoundMgr_Channel* c = RequestChannel(channel,true);
    c->volume_ = vol;
    c->RestVolAndPan();
  }

long SoundMgr::GetChannelVolume(long channel)
  {
    SoundMgr_Channel* c = RequestChannel(channel,false);
    if (c) return c->volume_; else return 100;
  }

void SoundMgr::LogSoundsList()
  {
    Lout << "engine: *CHANNELS*";

    channels_t::ValuePtr i = channels_.Begin(), iE = channels_.End();
    if ( i == iE ) Lout << "engine:   none channels";

    for ( ; i != iE; ++i )
      {
        SoundMgr_Channel* c = (*i);
        Lout << _S*"engine: [channel] no:%-3d pan:%-3d volume:%-3d" %c->no %c->pan_ %c->volume_;
        SoundMgr_Channel::sounds_t::Position j = c->sounds_.Begin();
        for ( ; j ; ++j )
          {
            rcc_ptr<SoundMgr_Sound> &s = (*j);
            if ( +s->listener_ )
              Lout << _S*"engine:   [playing] %? (listener 0x%08x)"
                %s->sndname_ %+s->listener_;
            else
              Lout << _S*"engine:   [playing] %?" %s->sndname_;
          }
      }

    Lout << "engine: *SOUND_LRU_CACHE*";

    lru_t::Position k = lru_.Begin();
    if ( !k )
      Lout << "engine:   cache empty";

    for ( ; k ; ++k )
      {
        rcc_ptr<SoundMgr_Sound> &s = (*k);
        Lout << _S*"engine: [cached] %?" %s->sndname_;
      }
  }

bool SoundMgr::GetMuteStatus(long channel)
  {
    SoundMgr_Channel* c = RequestChannel(channel,false);
    if (c) return c->mute_; else return false;
  }

void SoundMgr::Mute(long channel,bool mute)
  {
    SoundMgr_Channel* c = RequestChannel(channel,true);
    c->RsetMute(mute);
  }

void SoundMgr::InvertStereo(bool inv)
  {
    if ( invert_stereo_ != inv )
      {
        invert_stereo_ = inv;
        channels_t::ValuePtr i = channels_.Begin(), iE = channels_.End();
        for ( ; i != iE; ++i )
          (*i)->RestVolAndPan();
      }
  }

SoundMgr_Channel::SoundMgr_Channel(long _no,SoundMgr* _mgr)
  {
    mgr_ = _mgr;
    no = _no;
    volume_ = 100;
    pan_ = 0;
    mute_ = false;
  }

SoundMgr_Channel::~SoundMgr_Channel()
  {
    sounds_t::Position i = sounds_.Begin();
    for ( ; i ; ++i )
      (*i)->Dispose();
  }

void SoundMgr_Channel::CheckForStopped()
  {
    sounds_t::Position i = sounds_.Begin();
    int l = sounds_.Count();

    for ( ; l-- && i ; ) {
      if ( !(*i)->IsPlaying() )
        {
          (*i)->Notify(no,false,mgr_);
          mgr_->MoveToLRU(*i);
          sounds_.Erase(i);
        }
      else
        ++i;
    }
  }

void SoundMgr_Channel::Purge()
  {
    sounds_t tmp;
    sounds_.Swap(tmp);

    sounds_t::Position i = tmp.Begin(), iE = tmp.End();
    for ( ; i ; ++i )
      {
        (*i)->Stop();
        (*i)->Notify(no,true,mgr_);
        mgr_->MoveToLRU(*i);
      }
    tmp.Purge();
  }

HRESULT SoundMgr_Channel::RestVolAndPan()
  {
    sounds_t::Position i = sounds_.Begin();
    for ( ; i ; ++i )
      (*i)->SetVolAndPan(
        mute_?0:volume_,
        mgr_->invert_stereo_?-pan_:pan_);
    return S_OK;
  }

HRESULT SoundMgr_Channel::RsetMute(bool mute)
  {
    if ( mute != mute_ )
      {
        mute_ = mute;
        sounds_t::Position i = sounds_.Begin();
        for ( ; i ; ++i )
          (*i)->SetVolAndPan(
            mute_?0:volume_,
            mgr_->invert_stereo_?-pan_:pan_);
      }
    return S_OK;
  }

HRESULT SoundMgr_Channel::PlaySoundOn(BSTR sndname,IUnknown *listener,long cookie, bool in_loop)
  {
    HRESULT res = S_OK;
    rcc_ptr<SoundMgr_Sound> sound;
    sound =  mgr_->FindInLRU(sndname,listener);
    if ( !sound )
      sound = mgr_->FindInPreloaded(sndname,listener);
    if ( !sound )
      sound = mgr_->FindInChannels(sndname,listener);

    if ( !sound )
      {
        sound = rcc_ptr<SoundMgr_Sound>(new SoundMgr_Sound());
        if ( HRESULT e = sound->Initilaize(sndname,listener) )
          {
            Lout << _S*L"sound: failed(%08x) to initialize '%?' sound" %e %sndname;
            goto on_error;
          }
      }

    sound->cookie_ = cookie;
    sound->play_loop_ = in_loop;

    if ( FAILED(sound->SetVolAndPan(
      mute_?0:volume_,
      mgr_->invert_stereo_?-pan_:pan_)) )
        goto on_error;

    if ( HRESULT e = sound->Play(in_loop) )
      {
        Lout << _S*L"sound: failed(%08x) to play '%?' sound" %e %sndname;
        goto on_error;
      }

    sounds_.Append(sound);

  on_exit:
    return res;
  on_error:
    sound->Dispose();
    goto on_exit;
  }

HRESULT SoundMgr_Channel::NextSoundOn(BSTR sndname,long cookie)
  {
    sounds_t::Position i = sounds_.Begin();
    for ( ; i ; ++i )
      if ( (*i)->cookie_ == cookie )
        {
          if ( FAILED( (*i)->ChainForNextPlay(sndname,mgr_) ) )
            return ScraftError("failed to chain sound for play");
          return S_OK;
        }
    return ScraftError(_S*L"there is no cookie %d on channel %d" %cookie %no);
  }

HRESULT SoundMgr_Sound::ChainSoundObjectFor(pwide_t sndname, SoundMgr *mgr)
  {
    HRESULT res = S_OK;
    rcc_ptr<SoundMgr_Sound> sound;
    sound =  mgr->FindInLRU(sndname,+listener_);
    if ( !sound )
      sound = mgr->FindInPreloaded(sndname,+listener_);
    if ( !sound )
      sound = mgr->FindInChannels(sndname,+listener_);

    if ( !sound )
      {
        sound = rcc_ptr<SoundMgr_Sound>(new SoundMgr_Sound());
        if ( HRESULT e = sound->Initilaize(sndname,+listener_) )
          {
            Lout << _S*L"sound: failed(%08x) to initialize '%?' sound" %e %sndname;
            goto on_error;
          }
      }

    sound->cookie_ = cookie_;
    sound->play_loop_ = false;

    if ( HRESULT e = sndobj_->ChainNextPlay(sound->sndobj_) )
      {
        Lout << _S*L"sound: failed(%08x) to chain '%?' sound to '%?'" %e %sndname %sndname_;
        goto on_error;
      }

    chain_next_ = sound;

  on_exit:
    return res;
  on_error:
    sound->Dispose();
    goto on_exit;
  }

HRESULT SoundMgr_Sound::ChainForNextPlay(pwide_t snd_name,SoundMgr *mgr)
  {
    if ( chain_next_ )
      {
        if ( !chain_ ) chain_ = MemPtr(new SlistT<widestring>);
        chain_->Append(snd_name);
      }
    else
      return ChainSoundObjectFor(snd_name,mgr);
    return S_OK;
  }

rcc_ptr<IOnNextSound> DispatchIOnNextSound(rcc_ptr<IUnknown>& pI)
  {
    rcc_ptr<IOnNextSound> l = teggo::ifs_cast<IOnNextSound>(pI);
    return l;
  }

bool SoundMgr_Sound::OnStopNotifyUpdateChain(long channel,SoundMgr *mgr)
  {
    if ( chain_next_ )
      {
        if ( listener_ )
          {
            teggo::bstr_ptr bstr ( SysAllocString(+chain_next_->sndname_) );
            if ( rcc_ptr<IOnNextSound> l = DispatchIOnNextSound(listener_) )
              l->OnNextSound(+bstr,channel,cookie_);
          }
        chain_next_->sndobj_->PlayIfNotAlreadyPlayed();
        mgr->RequestChannel(channel,false)->sounds_.Append(chain_next_);
        if ( chain_ && !chain_->Empty() )
          {
            widestring &n = chain_->Top();
            chain_next_->ChainSoundObjectFor(+n,mgr);
            chain_->Pop();
            chain_next_->chain_ = chain_;
          }
        Unrefe(chain_next_);
        return true;
      }
    return false;
  }

rcc_ptr<IOnStopSound> DispatchIOnStopSound(rcc_ptr<IUnknown>& pI)
  {
    rcc_ptr<IOnStopSound> l = teggo::ifs_cast<IOnStopSound>(pI);
    return l;
  }

void SoundMgr_Sound::Notify(long channel,bool interrupted,SoundMgr *mgr)
  {
    //XMessageW((L"NOTIFY: %p, %p, %d,%d: %s",this,+listener_,channel,interrupted,sndname_.Str()));
    bool f_chained = false;

    if ( !interrupted )
      f_chained = OnStopNotifyUpdateChain(channel,mgr);

    if ( listener_ && !f_chained )
      {
        teggo::bstr_ptr bstr ( SysAllocString(+sndname_) );
        if ( rcc_ptr<IOnStopSound> l = DispatchIOnStopSound(listener_) )
          l->OnStopSound(+bstr,channel,cookie_,interrupted?VARIANT_TRUE:VARIANT_FALSE);
      }

    play_loop_ = false;
    Unrefe(listener_);
  }

HRESULT SoundMgr_Sound::Initilaize(pwide_t sndname,IUnknown *listener,ISoundObject *master)
  {
    listener_.reset_(Refe(listener));
    sndname_ = sndname;
    rcc_ptr<ISoundObject> sndobj;

    if ( !master )
      sndobj = SndSubsys->NewSound(0,sndname_.Str());
    else
      master->Clone(0,&+sndobj);

    sndobj->SetSubImpl(this);

    if ( !sndobj_ )
      return E_FAIL;
    else
      return S_OK;
  }

SoundMgr_Sound* SoundMgr_Sound::Clone(IUnknown *listener)
  {
    REQUIRE( sndobj_ != 0 );
    rcc_ptr<SoundMgr_Sound> sound ( new SoundMgr_Sound() );

    if ( SUCCEEDED(sound->Initilaize(+sndname_,listener,sndobj_)) )
      return teggo::Forget(sound);
    else
      return 0;
  }

HRESULT SoundMgr_Sound::SetVolAndPan(long vol,long pan)
  {
    HRESULT hr = S_OK;
    if ( sndobj_ )
      {
        if ( FAILED((hr = sndobj_->SetVolume(vol))) )
          return hr;
        if ( FAILED((hr = sndobj_->SetPan(pan))) )
          return hr;
        return S_OK;
      }
    else
      return E_FAIL;
  }

HRESULT SoundMgr_Sound::Play(bool f_loop)
  {
    if (!sndobj_)
      return E_FAIL;
    return sndobj_->Play(f_loop?VARIANT_TRUE:VARIANT_FALSE);
  }

HRESULT SoundMgr_Sound::Stop()
  {
    if (!sndobj_)
      return E_FAIL;
    return sndobj_->Stop();
  }
