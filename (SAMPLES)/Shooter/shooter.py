
import sys
from math import sin,cos
sys.path.append("../..")
import scraft
from scraft import engine as oE

PlayerCollideGroup  = 1
AsteroidCollideGroup= 2
EnemyCollideGroup   = 3

PlayerCollide       = scraft.CollideActive  + PlayerCollideGroup
PlayerShotCollide   = scraft.CollidePassive + PlayerCollideGroup
EnemyCollide        = scraft.CollideActive  + EnemyCollideGroup
EnemyShotCollide    = scraft.CollidePassive + EnemyCollideGroup
AsteroidCollide     = scraft.CollideActive  + AsteroidCollideGroup

ASTEROID_SPEED      = 150
ASTEROID_FPS        = 10
WARBOT_SPEED        = 120
WARBOT_SPEED1       = 200
LEFT_CANNON_X       = -15
RIGHT_CANNON_X      = 19
SHOT_SPEED          = 200

class PlayerShotMgr(scraft.Dispatcher) :
    def __init__(self) :
        self._sprites = oE.NewSpritesList()
        oE.executor.Schedule(self)

    def Make(self, x, y, angle) :
        shot = oE.NewSprite_(u"player_warbot_plasma", 1)
        shot.x = x
        shot.y = y
        shot.angle = angle
        shot.xScale = 50
        shot.collide = PlayerShotCollide
        self._sprites.Add(shot)

    def _OnExecute(self, que) :
        scrwidth = oE.scrWidth
        delta = que.delta / 1000.0
        self._sprites.Reset()
        while self._sprites.Next() :
            shot = self._sprites.Get()
            if shot.alive :
                shot.y -= ((delta * SHOT_SPEED) * sin(shot.angle + 3.14 / 2))
                shot.x -= ((delta * SHOT_SPEED) * -cos(shot.angle + 3.14 / 2))
                if shot.outOfScreen :
                    self._sprites.Remove()
                    shot.Dispose()
            else :
                self._sprites.Remove()
        return scraft.CommandStateRepeat


class Player(scraft.Dispatcher) :
    def __init__(self) :
        self._shotMgr = PlayerShotMgr()
        self._toggler = False
        self._warbot = oE.NewSprite_(u"player_warbot", 0)
        self._warbot.collide = PlayerCollide
        self._warbot.frno = 12
        self._warbot.hotspot = scraft.HotspotCenter
        self._warbot.dispatcher = self
        self._warbot.x = oE.scrWidth / 2
        self._warbot.y = oE.scrHeight - self._warbot.height

        self._minX = self._warbot.width / 2
        self._maxX = oE.scrWidth - self._minX
        self._minY = self._warbot.height / 2
        self._maxY = oE.scrHeight - self._minY
        self._updown = 0
        self._leftright = 0

        self._leftTrak = oE.NewSprite_(u"player_warbot_trak", 0)
        self._leftTrak.parent = self._warbot
        self._leftTrak.x = 28 - (self._warbot.width / 2)
        self._leftTrak.y = 59 - (self._warbot.height / 2)

        self._rightTrak = oE.NewSprite_(u"player_warbot_trak", 0)
        self._rightTrak.parent = self._warbot
        self._rightTrak.x = 38 - (self._warbot.width / 2)
        self._rightTrak.y = 59 - (self._warbot.height / 2)

        self._leftCannon = oE.NewSprite_(u"player_warbot_shot", 0)
        self._leftCannon.visible = False
        self._leftCannon.parent = self._warbot
        self._leftCannon.x = LEFT_CANNON_X
        self._leftCannon.y = 5
        self._leftCannon.frno = self._leftCannon.random

        self._rightCannon = oE.NewSprite_(u"player_warbot_shot", 0)
        self._rightCannon.visible = False
        self._rightCannon.parent = self._warbot
        self._rightCannon.x = RIGHT_CANNON_X
        self._rightCannon.y = 5
        self._rightCannon.frno = self._rightCannon.random

        self._fire_last_frame = oE.frameTick
        oE.SetChannelVolume(1,50)

    def Dispose(self) :
        self._warbot.Dispose()
        self._leftTrak.Dispose()
        self._rightTrak.Dispose()


    def _OnUpdate(self, sprite, tickdelta) :
        self._warbot.x += (WARBOT_SPEED * self._leftright * tickdelta) / 1000.0
        if self._updown > 0 :
            self._warbot.y += (WARBOT_SPEED1 * self._updown * tickdelta) / 1000.0
        else :
            self._warbot.y += (WARBOT_SPEED * self._updown * tickdelta) / 1000.0

        self._leftright = 0
        self._updown = 0

        if oE.IsKeyPressed(scraft.Key_StLeft) :
            self._leftright = -1
        if oE.IsKeyPressed(scraft.Key_StRight) :
            self._leftright += 1
        if oE.IsKeyPressed(scraft.Key_StUp) :
            self._updown = -1
        if oE.IsKeyPressed(scraft.Key_StDown) :
            self._updown += 1

        if self._warbot.x < self._minX :
            self._warbot.x = self._minX
        elif self._warbot.x > self._maxX :
            self._warbot.x = self._maxX

        oE.SetChannelPan(1, (self._warbot.x - (oE.scrWidth / 2)) * 200 / oE.scrWidth)

        if self._warbot.y < self._minY :
            self._warbot.y = self._minY
        elif self._warbot.y > self._maxY :
            self._warbot.y = self._maxY

        if self._leftright < 0 :
            self._warbot.frno = 0
            self._leftCannon.x = LEFT_CANNON_X + 2
            self._rightCannon.x = RIGHT_CANNON_X - 3
        elif self._leftright > 0 :
            self._warbot.frno = 2
            self._leftCannon.x = LEFT_CANNON_X
            self._rightCannon.x = RIGHT_CANNON_X - 3
        else :
            self._warbot.frno = 1
            self._leftCannon.x = LEFT_CANNON_X
            self._rightCannon.x = RIGHT_CANNON_X

        if self._updown < 0 :
            self._leftTrak.yScale = 150
            self._rightTrak.yScale = 150
        elif self._updown > 0 :
            self._leftTrak.yScale = 50
            self._rightTrak.yScale = 50
        else :
            self._leftTrak.yScale = 100
            self._rightTrak.yScale = 100

        if oE.IsKeyPressed(scraft.Key_StFire) or oE.IsKeyPressed(scraft.Key_SPACE) :
            self.Fire()
        else :
            self._leftCannon.visible = False
            self._rightCannon.visible = False

    def Fire(self) :
        if self._fire_last_frame + 100 < oE.frameTick :
            self._fire_last_frame = oE.frameTick
            x = self._warbot.x - 15
            if self._toggler :
              x += 32
              self._rightCannon.frno += 1
              self._rightCannon.visible = True
            else :
              self._leftCannon.frno += 1
              self._leftCannon.visible = True
            self._toggler = not self._toggler
            self._shotMgr.Make(x, self._warbot.y - 10, 0 )
            oE.PlaySound(u"player_fire_sound", 1)

class AsteroidMgr(scraft.Dispatcher) :
    def __init__(self) :
        self._sprites = oE.NewSpritesList()
        oE.executor.Schedule(self)

    def _OnExecute(self, que) :
      delta = que.delta / 1000.0
      oE.IncLayerY(2,delta * ASTEROID_SPEED)
      self._sprites.Reset()
      while self._sprites.Next() :
        asteroid = self._sprites.Get()
        if asteroid.alive :
            if asteroid.scrY > oE.scrHeight + asteroid.height :
                self._sprites.Remove()
                asteroid.Dispose()
            else :
                asteroid.frno = ((oE.frameTick / 1000.0) * ASTEROID_FPS +
                    (asteroid.random * asteroid.frcount)) % asteroid.frcount
        else :
            self._sprites.Remove()

      return scraft.CommandStateRepeat


    def _OnCollide(self, master, hit ) :
        p = oE.NewParticles(u"expl" + master.name, u"explosive", 2)
        p.dispatcher = self
        p.SetEmissionQuantity(6)
        p.SetEmissionPeriod(33)
        p.count = 18
        p.SetEmitterCf(scraft.EmCfIncAngle, 1, 3)
        p.SetEmitterCf(scraft.EmCfInitDirect, -3.14, 3.14)
        p.SetEmitterCf(scraft.EmCfInitSpeed, 30, 90)
        p.luminance = master.random * 50 + 50
        p.SetEmitterCf(scraft.EmCfIncTrans, 150, 150)
        p.lifeTime = 700
        p.cycled = False
        p.x = master.x
        p.y = master.y
        p.StartEmission()
        oE.SetChannelVolume(2,50)
        oE.PlaySound(u"explode_sound", 2)
        master.Dispose()
        if hit.collide == PlayerShotCollide :
            hit.Dispose()

    def _OnLifetimeOut(self, p) :
        p.Dispose()

    def Make(self, x, y ) :
        asteroid = oE.NewSprite_(u"asteroid", 2)
        asteroid.SetItem(0,1);
        asteroid.GetItem(0);
        asteroid.x = x
        asteroid.y = -oE.GetLayerY(2) + y
        asteroid.collide = AsteroidCollide
        asteroid.dispatcher = self
        self._sprites.Add(asteroid)

class GameTracker(scraft.Dispatcher) :
    def __init__(self) :
        self._lastupdate  = oE.frameTick
        self._randomizer  = oE.NewRandomizer()
        self._astermgr    = AsteroidMgr()
        self._bg0   = oE.NewPredefinedTileMap(u"", u"tmap_stars0", 100)
        self._bg1   = oE.NewPredefinedTileMap(u"", u"tmap_stars1", 101)
        self._bg1.x = 5551

    def _OnExecute(self, que) :
        delta = que.delta / 1000.0
        if self._lastupdate + 300 < oE.frameTick :
            self._astermgr.Make( (self._randomizer.Next() * oE.scrWidth), -100 )
            self._lastupdate = oE.frameTick
        oE.IncLayerY(100, delta * 120)
        oE.IncLayerY(101, delta * 100)
        return scraft.CommandStateRepeat

modulepath = unicode(sys.argv[0][0:sys.argv[0].rfind("\\")+1])
oE.logging = True
oE.Init(scraft.DevDisableDrvInfo)
oE.background.color = 0x202020
oE.rscpath = modulepath
oE.SST = u"shooter.sst"
oE.PlaceWindowAt(scraft.PositionCenter)
oE.showFps = True
oE.nativeCursor = False

oE.AllowCollide(PlayerCollideGroup, AsteroidCollideGroup)
oE.AllowCollide(PlayerCollideGroup, EnemyCollideGroup)

player = Player()
oE.executor.Schedule(GameTracker())

while True:
    oE.NextEvent()
    if oE.EvtIsESC() or oE.EvtIsQuit() :
        break
    if oE.EvtIsKeyDown() :
        if oE.EvtKey() == scraft.Key_F4 :
            oE.fullscreen = not oE.fullscreen
        if oE.EvtKey() == scraft.Key_ENTER and oE.IsKeyPressed(scraft.Key_ALT) :
            oE.fullscreen = not oE.fullscreen
        if oE.EvtKey() == scraft.Key_F9 :
            oE.DumpObjectsList()
        if oE.EvtKey() == scraft.Key_F2 :
            oE.invertStereo = not oE.invertStereo
        if oE.EvtKey() == scraft.Key_F8 :
            p = oE.EnumerateParticles()
            while p.Next() :
                oE.Log("particles " + p.Get().name)
    oE.DisplayEx(60) # 60 FPS
