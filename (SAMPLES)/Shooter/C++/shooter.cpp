
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

int APIENTRY WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow)
  {

    try
      {
        Engine->Init();
        //Engine->rscPath = L".";

        Engine->SST   = L"shooter.sst";
        Engine->title = L"Space Shooter 2";
        Engine->vMode = VMODE_640x480;
        Engine->showFps = true;
        //Engine->fullscreen = true;

        Engine->SetBackground(0x101010);
        Engine->AllowCollide(PlayerCollideGroup, AsteroidCollideGroup);

        Player::Create();
        GameTracker::Create();

        for (;;)
          {
            if ( Engine->NextEvent() )
              {
                if ( Engine->EvtIsESC() || Engine->EvtIsQuit() ) break;
                if ( Engine->EvtIsKeyDown() )
                  {
                    switch ( Engine->EvtKey() )
                      {
                      case Key_F4:
                        Engine->fullscreen = !Engine->fullscreen;
                        break;
                      case Key_F9:
                        Engine->DumpObjectsList();
                        break;
                      default:; // nothing
                      }
                  }
              }
            Engine->DisplayEx(60); //FPS
          }
      }
    catch(WideException& e)
      {
        Engine->Minimize();
        MessageBoxW(0,e.What(),L"WideException",MB_OK|MB_ICONERROR);
      }

    Engine->Term();
    return 0;
  }
