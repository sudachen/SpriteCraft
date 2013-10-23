
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

#include <scraftapi.h>
#include <classes.h>

#if CXX_COMPILER_IS_MSVC
#pragma warning(disable:4244) //conversion from 'float' to 'long', possible loss of data
#pragma warning(disable:4018) //signed/unsigned mismatch
#endif

enum
  {
    PlayerCollideGroup = 1,
    AsteroidCollideGroup = 2,
    PlayerCollide = CollideActive + PlayerCollideGroup,
    PlayerShotCollide  = CollidePassive + PlayerCollideGroup,
    AsteroidCollide  = CollideActive + AsteroidCollideGroup
  };

struct PlayerShotMgr : ScraftCommand
  {
    ISpritesListPtr sprites;

    PlayerShotMgr();
    ~PlayerShotMgr();
    virtual u32_t Execute(ScraftIQue* que);
    void Make(float x,float y,float angle);

    static rcc_ptr<PlayerShotMgr> Create() { return RccPtr(new PlayerShotMgr()); }
  };
typedef rcc_ptr<PlayerShotMgr> PlayerShotMgrPtr;

struct Player : TeggoObjectEx < ScraftOnCollide, ScraftOnUpdate >
  {
    PlayerShotMgrPtr shotmgr;
    long leftright;
    long updown;
    float min_x,max_x;
    float min_y,max_y;
    ISpritePtr warbot;
    ISpritePtr leftTrak;
    ISpritePtr rightTrak;
    ISpritePtr left_cannon;
    ISpritePtr right_cannon;
    long fire_last_frame;
    bool toggler;

    void Fire();
    virtual void OnCollide (ScraftISprite * sprite,ScraftISprite * Hit );
    virtual void OnUpdate (ScraftISprite * sprite,long tickdelta );
    ~Player();
    Player();

    static rcc_ptr<Player> Create () { return RccPtr(new Player()); }
  };

struct AsteroidMgr :
  TeggoObjectEx < ScraftCommand, ScraftOnCollide, ScraftOnLifetimeOut >
  {
    ISpritesListPtr sprites;

    virtual u32_t Execute (ScraftIQue * que);
    virtual void    OnCollide (ScraftISprite * sprite,ScraftISprite * Hit );
    virtual void    OnLifetimeOut (ScraftIParticles * particles );

    AsteroidMgr();
    ~AsteroidMgr();
    void Make(float x,float y);

    static rcc_ptr<AsteroidMgr> Create() { return RccPtr(new AsteroidMgr()); }
  };
typedef rcc_ptr<AsteroidMgr> AsteroidMgrPtr;

struct GameTracker : ScraftTask
  {
    AsteroidMgrPtr astrmgr;
    IRandomizerPtr randomizer;
    long lastupdate;
    ITileMapPtr bg0;
    ITileMapPtr bg1;

    virtual u32_t Execute (ScraftIQue* que);
    GameTracker();
    ~GameTracker();

    static rcc_ptr<GameTracker> Create() { return RccPtr(new GameTracker()); }
  };
