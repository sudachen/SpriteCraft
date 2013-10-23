import teggo
import teggo.fx as fx
import teggo.fx.xforms as xforms

teggo.OpenLog('test8-xxxx-log.txt')
fx.Init()
fx.NativeCursor(True)
fx.Background().color = 0xff8080


sp0 = fx.Sprite('','$spritecraft$blank$'); sp0.xSize = 350; sp0.ySize = 100
sp0.cfilt.color = 0xe0e0ff
sp0.layer = 100; sp0.paralax = 50
sp0.hotspot = 5;  sp0.x = fx.ScrWidth()/2; sp0.y = fx.ScrHeight()/2

sp1 = fx.Sprite('','$spritecraft$blank$'); sp1.xSize = 70; sp1.ySize = 50
sp1.cfilt.color = 0xff0000; sp1.hotspot = 5; sp1.y = fx.ScrHeight()/2

sp2 = fx.Sprite('','$spritecraft$blank$'); sp2.xSize = 10; sp2.ySize = 90
sp2.cfilt.color = 0x00ff00
sp2.layer = -100;  sp2.paralax = -50
sp2.hotspot = 5;  sp2.x = fx.ScrWidth()/2; sp2.y = fx.ScrHeight()/2


while True:
    fx.NextEvent()

    if fx.EvtIsESC() or fx.EvtIsQuit() :
        break;

    fx.ScrOffsX(fx.FrameTick()/100)
    sp1.x = fx.ScrOffsX() + fx.ScrWidth()/2

    fx.DisplayEx(30) # 60 FPS
