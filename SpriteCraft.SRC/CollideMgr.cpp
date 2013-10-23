
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

#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "RBGFilter.h"
#include "Engine.h"
#include "Engine_DInput.h"
#include "CollideMgr.h"
#include "SpriteHelper.h"
#include <limits.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Managers$CollideMgr")
#endif

static float g_scrwidth = 0;
static float g_scrheight = 0;

struct CollideMgr_CollideObject : public ICollideUnlink
  {
    float R,x,y,oldx,oldy;
    Sprite* sprite;
    rcc_ptr<ICollidableObject> obj;

    struct 
      {
        bool checkit : 1;
        bool trackable : 1;
        bool tracked : 1;
      };

    CollideMgr_CollideObject() : sprite(0),trackable(false) 
      {
      }

    CollideMgr_CollideObject(ICollidableObject* o,bool xtrackable) : trackable(xtrackable) 
      {
        obj = RccRefe(o);
        tracked = false;
        sprite = obj->ICollidableObject_GetSprite();
        obj->ICollidableObject_Link(this);
      }

    ~CollideMgr_CollideObject() 
      { 
        //XMessage((":CollideMgr: dtor of CollideMgr_CollideObject"));
        Unlink(); 
      }

    virtual void Unlink() 
      {
        if ( +obj ) 
          {
            //XMessage((":CollideMgr: unlink collidable object"));
            obj->ICollidableObject_Link(0);
            Unrefe(obj);
          }
        sprite = 0;
      }
  };

bool CollideMgr::EvaluteRxy(CollideMgr_CollideObject& o) 
  {
    if ( !o.obj ) 
      return false;

    DBG_REQUIRE ( o.sprite );

    o.oldx = o.x; o.oldy = o.y;
    o.sprite->GetRadius(&o.R,&o.x,&o.y);

    if ( o.trackable && !o.tracked ) 
      { 
        o.oldx = o.x; 
        o.oldy = o.y; 
        o.tracked = true; 
      }

    // если запрещена обработка коллизий для спрайтов вне экрана
    // проверяем находится ли спрайт вне экрана или нет
    if ( !offscreen_ ) 
      {
        o.checkit = 
          o.x > -o.R && o.x < o.R + g_scrwidth 
          && o.y > -o.R && o.y < o.R + g_scrheight;

        if ( !o.checkit && o.trackable )
          o.checkit = 
            o.oldx > -o.R && o.oldx < o.R + g_scrwidth 
            && o.oldy > -o.R && o.oldy < o.R + g_scrheight;
      }
    else // проверка вне экрана разрешена, проверяем все
      o.checkit = true;

    return true;
  }

bool CollideMgr::NotifyIt(CollideMgr_CollideObject& o, CollideMgr_CollideObject& p, bool active)
  {
    if ( +listener_ ) 
      {
        listener_->OnCollide(
          +RccRefe(o.obj->ICollidableObject_GetISprite()),
          +RccRefe(p.obj->ICollidableObject_GetISprite()) );
        if ( !o.obj ) 
          return false;
        if ( !p.obj ) 
          return true;
        DBG_REQUIRE ( o.sprite );
        DBG_REQUIRE ( p.sprite );
      }

    o.obj->ICollidableObject_OnCollide(p.sprite);
    
    if ( !o.obj ) 
      return false;
    
    if ( +p.obj ) 
      {
        DBG_REQUIRE ( o.sprite );
        DBG_REQUIRE ( p.sprite );
        p.obj->ICollidableObject_OnCollide(o.sprite);
      }

    return true;
  }

bool CollideMgr::EvaluteTrackCollide(CollideMgr_CollideObject& o, CollideMgr_CollideObject& p, bool active)
  {
    if ( (o.tracked && p.sprite->IntersectLine(o.oldx,o.oldy,o.x,o.y)) 
      || (p.tracked && o.sprite->IntersectLine(p.oldx,p.oldy,p.x,p.y)) )
      return NotifyIt(o,p,active);
    return true;
  }

bool CollideMgr::EvaluteHit(CollideMgr_CollideObject& o, CollideMgr_CollideObject& p, bool active)
  {
    if ( !o.obj ) 
      return false;
    if ( !p.obj ) 
      return true;
    if ( !p.checkit ) 
      return true;

    DBG_REQUIRE ( o.sprite );
    DBG_REQUIRE ( p.sprite );

    if ( o.sprite == p.sprite ) 
      return true;
    const float xx = o.x-p.x;
    const float yy = o.y-p.y;

    if ( sqrtf(xx*xx + yy*yy) < o.R + p.R  && o.sprite->Hit(p.sprite,false) ) 
      return NotifyIt(o,p,active);
    else if ( o.trackable || p.trackable )
      return EvaluteTrackCollide(o,p,active);
    else 
      return true;
  }

void CollideMgr::CollideCheck()
  {
    g_scrwidth  = float(SCREEN::WndWidth());
    g_scrheight = float(SCREEN::WndHeight());
    // пересчитываем граничную хону и выбрасываем обьекты для которых
    // отсутствует обработчик коллизий

    {for ( int g = 0; g < MAX_GROUPS; ++g ) {
      for ( int j = 0; j < 2; ++j ) 
        {
          objects_t::Position i  = objects_[g][j].Begin();
          while ( i.IsValid() ) 
            {
              CollideMgr_CollideObject& o = **i;
              if ( !EvaluteRxy(o) ) 
                {
                  //XMessage((":CollideMgr: remove on check"));
                  delete *i;
                  objects_[g][j].Erase(i);
                }
              else
                ++i;
            }
        }
    }}

    // ищем коллизии и вызываем обработчик колизий
    {for ( int g = 0; g < MAX_GROUPS; ++g ) {
      objects_t::Position i  = objects_[g][ACTIVE].Begin();
      while ( i.IsValid() ) 
        {
          CollideMgr_CollideObject& o = **i;
          if ( !o.checkit ) 
            goto next_object;

          // check passive
          {for ( int g0 = 0; g0 < MAX_GROUPS; ++g0 ) {
            if ( collide_mask_[g] & MSKIN_BIT(g0) ) 
              {
                objects_t::Position j  = objects_[g0][PASSIVE].Begin();
                for ( ; j.IsValid() ; ++j ) 
                  if ( !EvaluteHit(o,**j,false) )
                    goto next_object;
              }
          }}

          // check active
          {for ( int g0 = g; g0 < MAX_GROUPS; ++g0 ) {
            if ( collide_mask_[g] & MSKIN_BIT(g0) ) 
              {
                objects_t::Position j  = 
                  g0==g ? i : objects_[g0][ACTIVE].Begin();
                for ( ; j.IsValid() ; ++j ) 
                  if ( !EvaluteHit(o,**j,true) ) 
                    goto next_object;
              }
          }}

    next_object:; 
          // переходим сюда если дальнейшая обработка коллизий для o не нужна
          ++i;
        }
    }}
  }

void CollideMgr::AddObject(ICollidableObject* obj,long group,bool active,bool trackable)
  {
    //XMessage((":CollideMgr: add collide object %d(%s)",group,active?"active":"passive"));
    return_if(group >=  MAX_GROUPS);
    return_if(!obj);
    objects_[group][active?ACTIVE:PASSIVE].Append(new CollideMgr_CollideObject(obj,trackable));
  }

void CollideMgr::AllowCollide(unsigned group1,unsigned group2)
  {
    return_if(group1 >=  MAX_GROUPS);
    return_if(group2 >=  MAX_GROUPS);
    collide_mask_[group1] |= MSKIN_BIT(group2);
    collide_mask_[group2] |= MSKIN_BIT(group1);
  }

void CollideMgr::DisallowCollide(unsigned group1,unsigned group2)
  {
    return_if(group1 >=  MAX_GROUPS);
    return_if(group2 >=  MAX_GROUPS);
    collide_mask_[group1] &= +(MSKIN_BIT(group2));
    collide_mask_[group2] &= +(MSKIN_BIT(group1));
  }

CollideMgr::CollideMgr()
  {
    //XMessage((":CollideMgr: ctor"));
    unique_ = 0;
    offscreen_ = false;
    memset(collide_mask_,0,sizeof(collide_mask_));
  }

CollideMgr::~CollideMgr()
  {
    //XMessage((":CollideMgr: dtor"));
    for ( int g=0; g < MAX_GROUPS; ++g ) 
      {
        for ( int j=0; j <2; ++j ) 
          {
            objects_t::Position i  = objects_[g][j].Begin();
            for ( ; i.IsValid() ; ++i ) delete *i;
          }
      }
  }

rcc_ptr<IOnCollide> DispatchIOnCollide( IUnknown *l )
  {
    return rcc_ptr<IOnCollide>(0);
  }

void CollideMgr::SetListener(IUnknown *l) 
  {
    rcc_ptr<IOnCollide> ioc = teggo::ifs_cast<IOnCollide>(l);
    if ( !ioc && l )
      ioc = DispatchIOnCollide( l );
    listener_.Swap(ioc);
  }

