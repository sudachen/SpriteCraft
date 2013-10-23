
#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "RBGFilter.h"
#include "Engine.h"
#include "CollideMgr.h"
#include "SoundMgr.h"
#include "AnimationMgr.h"
#include "MListenMgr.h"

u32_t GetMillis_OnFrame()
  {
    return g_current_engine->timer_.GetMillis_OnFrame();
  }

float Engine_GetMouseX()
  {
    return g_current_engine->mouse_X_;
  }

float Engine_GetMouseY()
  {
    return g_current_engine->mouse_Y_;
  }

void Engine_EvtGet(EVENT* e)
  {
    *e = g_current_engine->event_;
  }

bool Engine_EvtIsRClick()
  {
    return
      g_current_engine->event_.type == EVENT::POINTER_UP &&
      g_current_engine->event_.key == KEYVAL::EVK_RBUTTON &&
      (GetMillis()-g_current_engine->last_mbright_down_) < 250 ;
  }

bool Engine_EvtIsLClick()
  {
    return
      g_current_engine->event_.type == EVENT::POINTER_UP &&
      g_current_engine->event_.key == KEYVAL::EVK_LBUTTON &&
      (GetMillis()-g_current_engine->last_mbleft_down_) < 250;
  }

IEngine* Engine_ComIEngine()
  {
    return g_current_engine;
  }

void Engine_ComAddOnUpdateListener(IOnUpdateListener* listener)
  {
    g_current_engine->on_update_set_->Append(RccRefe(listener));
  }

void Engine_ComAddCollidable(ICollidableObject* o,long group,bool active,bool trackable)
  {
    g_current_engine->collidemgr_->AddObject(o,group,active,trackable);
  }

long Engine_ComScheduleWorker(ICommand* cmd)
  {
    long queno = 0;
    if ( S_OK == g_current_engine->executor_->Schedule(cmd,&queno) )
      return queno;
    else
      return 0;
  }

void Engine_ComInterruptWorker(long queno)
  {
    g_current_engine->executor_->DismissQueue(queno);
  }

void Engine_ComStopSpriteAnimation(IWeakSprite* spr)
  {
    g_current_engine->animamgr_->StopSpriteAnimation(spr,true);
  }

void Engine_ComStartSpriteAnimation(IWeakSprite* spr,long startFrno,long stopFrno,float fps,IUnknown* listener,long cookie,bool loop)
  {
    g_current_engine->animamgr_->AddAnimatedSprite(spr,startFrno,stopFrno,fps,listener,cookie,loop);
  }

void Engine_ComAddSpriteMouseListener(IWeakSpriteEx* spr)
  {
    g_current_engine->mlistenmgr_->Add(spr);
  }

void Engine_ComRemoveSpriteMouseListener(IWeakSpriteEx* spr)
  {
    g_current_engine->mlistenmgr_->Remove(spr);
  }

HRESULT __stdcall PyCreateSprite_(ISprite** pI,PyObject *args,PyObject *kwds)
  {
    wchar_t *spr_name = 0;
    wchar_t *spr_klass = 0;
    if ( _PoC_Args(args,kwds,"|uu",&spr_name,&spr_klass) )
      {
        return g_current_engine->NewSprite(spr_name,spr_klass,0,0,0,pI);
      }
    else
      {
        _PoC_RaiseComExcept();
        return E_FAIL;
      }
  }

HRESULT __stdcall Engne_NewTileMap_(ITileMap **pI)
  {
    return g_current_engine->NewTileMap(0,0,0,0,0,pI);
  }

HRESULT __stdcall PyCreateTileMap_(ITileMap **pI,PyObject *,PyObject *)
  {
    return Engne_NewTileMap_(pI);
  }
