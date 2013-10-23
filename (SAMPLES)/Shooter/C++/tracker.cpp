
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

#include "shooter.h"
#include <math.h>

enum 
  {
    ASTEROID_SPEED = 150,
    ASTEROID_FPS = 10
  };

void AsteroidMgr::OnCollide (ScraftISprite * sprite,ScraftISprite * Hit )
  {
    IParticlesPtr p;
    p = Engine->NewParticles(
      0|_S*L"expl%?" %sprite->GetNameAsPWIDE(), L"explosive", 2);
    p->SetSubImpl(__iunknown());
    p->SetEmissionQuantity(6,6);
    p->SetEmissionPeriod(33,33);
    p->SetCount(18);
    p->SetEmitterCf(EmCfIncAngle, 1, 3);
    p->SetEmitterCf(EmCfInitDirect, -3.14f, 3.14f);
    p->SetEmitterCf(EmCfInitSpeed, 30, 90);
    p->SetLuminance(sprite->GetRandom() * 50 + 50);
    p->SetEmitterCf(EmCfIncTrans, 150, 150);
    p->SetLifeTime(700);
    p->SetCycled(false);
    p->SetX(sprite->GetX());
    p->SetY(sprite->GetY());
    p->StartEmission();
    sprite->Dispose();
    if ( Hit->collide == PlayerShotCollide ) Hit->Dispose();
    Engine->channelVolume[2] = 50;
    Engine->PlaySound(L"explode_sound", 2, 0,0);
  }

void AsteroidMgr::OnLifetimeOut (ScraftIParticles * particles )
  {
    particles->Dispose();
  }

u32_t AsteroidMgr::Execute (ScraftIQue * que)
  {
    float delta = que->delta / 1000.f;
    ISpritePtr asteroid;
    Engine->layerY[2] += delta * ASTEROID_SPEED;
    sprites->Reset();
    while ( sprites->Next() ) 
      {
        asteroid = sprites->Get();
        if ( asteroid->IsAlive() ) 
          {
            if ( asteroid->scrY > Engine->scrHeight + asteroid->height ) 
              {
                sprites->Remove();
                asteroid->Dispose();
              }
            else
              asteroid->frno = long(((Engine->frameTick / 1000.0) * ASTEROID_FPS
                + (asteroid->random * asteroid->frcount))) % asteroid->frcount;
          }
        else 
          sprites->Remove();
      }
    return CommandStateRepeat;
  }

AsteroidMgr::AsteroidMgr()
  {
    sprites = Engine->NewSpritesList();
    Engine->executor->Schedule(__iunknown());
    Engine->Log(L":SHOOTER2: asteroid manager initialized and started");
  }

AsteroidMgr::~AsteroidMgr()
  {
    Engine->Log(L":SHOOTER2: asteroid manager terminated");
  }

void AsteroidMgr::Make(float x,float y)
  {
    ISpritePtr asteroid;
    asteroid = Engine->NewSprite_(L"asteroid", 2);
    asteroid->x = x;
    asteroid->y = -Engine->layerY[2] + y;
    asteroid->collide = AsteroidCollide;
    asteroid->subimpl = RccRefe(__iunknown());
    sprites->Add(asteroid);
  }

u32_t GameTracker::Execute (ScraftIQue * que )
  {
    float delta = que->delta / 1000.f;
    if ( lastupdate + 300 < Engine->frameTick ) 
      {
        astrmgr->Make((randomizer->Next() * Engine->scrWidth), -100);
        lastupdate = Engine->frameTick;
      }
    Engine->layerY[100] += delta * 120.f;
    Engine->layerY[101] += delta * 100.f;
    return CommandStateRepeat;
  }

GameTracker::GameTracker()
  {
    astrmgr = AsteroidMgr::Create();
    randomizer = Engine->NewRandomizer();
    lastupdate = Engine->frameTick;
    bg0 = Engine->NewPredefinedTileMap(L"", L"tmap_stars0", 100);
    bg1 = Engine->NewPredefinedTileMap(L"", L"tmap_stars1", 101);
    bg1->x = 5551;
    Engine->executor->Schedule(__iunknown());
    Engine->Log(L":SHOOTER2: game tracker initialized and started");
  }

GameTracker::~GameTracker()
  {
    Engine->Log(L":SHOOTER2: game tracker terminated");
  }
