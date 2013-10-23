
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
    WARBOT_SPEED = 120,
    WARBOT_SPEED1 = 200,
    LEFT_CANNON_X = -15,
    RIGHT_CANNON_X = 19
  };

Player::Player()
  {
    toggler = false;

    shotmgr = PlayerShotMgr::Create();
    warbot  = Engine->NewSprite_(L"player_warbot", 0);
    warbot->collide = PlayerCollide;
    warbot->frno = 12;
    warbot->hotspot = HotspotCenter;
    warbot->SetSubImpl(__iunknown());
    warbot->x = Engine->scrWidth / 2;
    warbot->y = Engine->scrHeight - warbot->height;

    min_x = warbot->width / 2;
    max_x = Engine->scrWidth - min_x;
    min_y = warbot->height / 2;
    max_y = Engine->scrHeight - min_y;
    updown = 0;
    leftright = 0;

    leftTrak = Engine->NewSprite_(L"player_warbot_trak", 0);
    warbot->AttachChild(L"leftTrack",leftTrak);
    leftTrak->x = 28 - (warbot->width / 2);
    leftTrak->y = 59 - (warbot->height / 2);

    rightTrak = Engine->NewSprite_(L"player_warbot_trak", 0);
    warbot->AttachChild(L"rightTrack",rightTrak);
    rightTrak->x = 38 - (warbot->width / 2);
    rightTrak->y = 59 - (warbot->height / 2);

    left_cannon = Engine->NewSprite_(L"player_warbot_shot", 0);
    warbot->AttachChild(L"leftCannon",left_cannon);
    left_cannon->visible = false;
    left_cannon->x = LEFT_CANNON_X;
    left_cannon->y = 5;
    left_cannon->frno = left_cannon->random*left_cannon->frcount;

    right_cannon = Engine->NewSprite_(L"player_warbot_shot", 0);
    warbot->AttachChild(L"rightCannon",right_cannon);
    right_cannon->visible = false;
    right_cannon->x = RIGHT_CANNON_X;
    right_cannon->y = 5;
    right_cannon->frno = right_cannon->random*left_cannon->frcount;

    fire_last_frame = Engine->frameTick;
    Engine->channelVolume[1] = 50;
    Engine->Log(L":SHOOTER2: player initialized");
  }

Player::~Player()
  {
    Engine->Log(L":SHOOTER2: player terminated");
  }

void Player::Fire()
  {
    float x;
    long frametick = Engine->frameTick;
    if ( fire_last_frame + 100 < frametick )
      {
        fire_last_frame = frametick;
        x = warbot->x - 15;
        if ( toggler )
          {
            x += 32;
            right_cannon->visible = true;
            right_cannon->frno += 1;
          }
        else
          {
            left_cannon->visible = true;
            left_cannon->frno += 1;
          }
        toggler = !toggler;
        shotmgr->Make(x, warbot->y - 10, 0);
        Engine->PlaySound(L"player_fire_sound",1,0,0);
      }
  }

void Player::OnCollide(ScraftISprite * sprite,ScraftISprite * Hit )
  {
  }

void Player::OnUpdate (ScraftISprite * sprite,long tickdelta )
  {
    warbot->x += (WARBOT_SPEED * leftright * tickdelta) / 1000.f;
    warbot->y +=
      ((updown > 0 ? WARBOT_SPEED1 : WARBOT_SPEED ) * updown * tickdelta) / 1000.f;

    leftright = 0;
    updown = 0;

    if ( Engine->IsKeyPressed(Key_LEFT) ) leftright = -1;
    if ( Engine->IsKeyPressed(Key_RIGHT) ) leftright = leftright + 1;
    if ( Engine->IsKeyPressed(Key_UP) ) updown = -1;
    if ( Engine->IsKeyPressed(Key_DOWN) ) updown = updown + 1;

    if ( warbot->x < min_x )
      warbot->x = min_x;
    else if ( warbot->x > max_x )
      warbot->x = max_x;

    Engine->channelPan[1] =
      (warbot->x - (Engine->scrWidth / 2)) * 200 / Engine->scrWidth;

    if ( warbot->y < min_y )
      warbot->y = min_y;
    else if ( warbot->y > max_y )
      warbot->y = max_y;

    if ( leftright < 0 )
      {
        warbot->frno = 0;
        left_cannon->x = LEFT_CANNON_X + 2;
        right_cannon->x = RIGHT_CANNON_X - 3;
      }
    else if ( leftright > 0 )
      {
        warbot->frno = 2;
        left_cannon->x = LEFT_CANNON_X;
        right_cannon->x = RIGHT_CANNON_X - 3;
      }
    else
      {
        warbot->frno = 1;
        left_cannon->x = LEFT_CANNON_X;
        right_cannon->x = RIGHT_CANNON_X;
      }

    /**/ if ( updown < 0 )
      leftTrak->yScale = 150, rightTrak->yScale = 150;
    else if ( updown > 0 )
      leftTrak->yScale = 50,  rightTrak->yScale = 50;
    else
      leftTrak->yScale = 100, rightTrak->yScale = 100;

    if ( Engine->IsKeyPressed(Key_CTRL) || Engine->IsKeyPressed(Key_SPACE) )
      Fire();
    else
      {
        left_cannon->visible = false;
        right_cannon->visible = false;
      }
  }

enum
  {
    SHOT_SPEED = 200
  };

PlayerShotMgr::PlayerShotMgr()
  {
    sprites = Engine->NewSpritesList();
    Engine->executor->Schedule(__iunknown());
    Engine->Log(L":SHOOTER2: player's shot manager initialized and started");
  }

PlayerShotMgr::~PlayerShotMgr()
  {
    Engine->Log(L":SHOOTER2: player's shot manager terminated");
  }

u32_t PlayerShotMgr::Execute (ScraftIQue * que )
  {
    float delta = que->delta / 1000.f;
    ISpritePtr shot;
    sprites->Reset();
    while ( sprites->Next() )
      {
        shot = sprites->Get();
        if ( shot->IsAlive() )
          {
            shot->y -= (delta * SHOT_SPEED) * sin(shot->angle + 3.14 / 2);
            shot->x -= (delta * SHOT_SPEED) * -cos(shot->angle + 3.14 / 2);
            if ( shot->outOfScreen )
              {
                sprites->Remove();
                shot->Dispose();
              }
          }
        else
          sprites->Remove();
      }
    return CommandStateRepeat;
  }

void PlayerShotMgr::Make(float x,float y,float angle)
  {
    ISpritePtr shot;
    shot = Engine->NewSprite_(L"player_warbot_plasma", 1);
    shot->x = x;
    shot->y = y;
    shot->angle = angle;
    shot->xScale= 50;
    shot->collide = PlayerShotCollide;
    sprites->Add(shot);
  }
