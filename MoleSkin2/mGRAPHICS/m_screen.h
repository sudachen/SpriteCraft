
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

#if !defined (__m_screen_h__)
#define __m_screen_h__

#include "m_gfx.h"
#include "m_gfx3d.h"

NAMESPACE_MSKIN_BEGIN


struct SCREEN_Inst;
struct SCREEN_Font;

struct SCREEN {
  static void         SetImpl(GfxDevice*);
  static IRefObject*  Acquire(GFX_DEVICE devtype,GFX_VALUE,GFX_VALUE,GFX_VALUE);
  static IRefObject*  Acquire(GfxDevice*);
  static bool         IsActive();
  static bool         IsRubbish();
  static bool         IsFullscreen();
  static void         MakeRubbish();
  static int          GetWidth();
  static int          GetHeight();
  static int          WndWidth();
  static int          WndHeight();
  static u32_t      GetCaps();
  static GfxControl*  GetControl();
  static GfxDevice*   GetGfxDevice();
  static Gfx3DDevice* Get3DDevice();
  static Surface*     GetFrameBuffer();
  static SCERROR  BeginDraw(unsigned flags = 0);
  static SCERROR  EndDraw(unsigned flags = 0);
  static EVENT        NextEvent(bool processMsgQue);
  static SCERROR  SetMode(GFX_VALUE wndmode=GFX_UNKNOWN,GFX_VALUE=GFX_UNKNOWN,GFX_VALUE=GFX_UNKNOWN,GFX_VALUE=GFX_UNKNOWN,u32_t=0);
  static SCERROR  GetMode(GFX_VALUE* wndmode=0,GFX_VALUE* mode=0,GFX_VALUE* bpp=0,GFX_VALUE* refrate=0,u32_t* = 0);
  static Surface*     CreateDirtySurface(int w,int h,unsigned fmt=Surface::PREFERRED);
  static Texture*     CreateDirtyTexture(int width,int height,unsigned flags=GFX3D_TEX_DEFAULT);
  static VxB*         CreateDirtyVxB(int count,unsigned flags=GFX3D_VxB_DEFAULT);
  static SCERROR  RestoreContext();
  static SCERROR  Print(int x,int y,pwide_t);
  static int          TextWidth(pwide_t);
  static int          TextHeight();
  static SCERROR  Clear();
  static SCERROR  ClearDepth();
  static SCERROR  GetWindowHandle(void* handle,u32_t sizeof_handle);
  static void         SetFillColor(mrgb_t color);
  static void SetOffsetX( float x ) { offsetx_ = x; MakeRubbish(); }
  static void SetOffsetY( float y ) { offsety_ = y; MakeRubbish(); }
  static float OffsetX() { return offsetx_; }
  static float OffsetY() { return offsety_; }
  static float OffsetXfloor() { return floor(offsetx_+0.5f); }
  static float OffsetYfloor() { return floor(offsety_+0.5f); }
private:
  friend struct SCREEN_Inst;
  friend struct SCREEN_Font;
  SCREEN();
  ~SCREEN();
  SCREEN(const SCREEN&);
  SCREEN& operator=(const SCREEN);
  static GfxDevice*   gfx_;
  static Gfx3DDevice* gfx_3d_;
  static GfxControl*  gfx_control_;
  static SCREEN_Inst* instance_;
  static SCREEN_Font* font_;
  static float offsetx_;
  static float offsety_;
  static SCREEN_Font* GetFont_();
};

NAMESPACE_MSKIN_END

#endif // __m_screen_h__
