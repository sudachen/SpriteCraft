
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

#if !defined ___6c153da0_cd33_4310_8681_683b7336cb3b___
#define ___6c153da0_cd33_4310_8681_683b7336cb3b___

struct SCE_EVENT
  {
  };

typedef IDirect3DDevice7 *SCED3D; 

enum SCEFX_VALUE
  {
    SCEFX_UNKNOWN = 0,
    SCEFX_WINDOWED,
    SCEFX_FULLSCREEN,
    SCEFX_HIDDEN,
    SCEFX_320x240,
    SCEFX_640x480,
    SCEFX_800x600,
    SCEFX_1024x768,
    SCEFX_1280x1024,
    SCEFX_1600x1200,
    SCEFX_BPP16,
    SCEFX_BPP32,
    SCEFX_REFRATE_DEFAULT,
    SCEFX_REFRATE_OPTIMAL,
    SCEFX_REFRATE_MAXIMUM,
    SCEFX_VALUE_DATA   = 0x00ffffff,
    SCEFX_VALUE_FLAGS  = 0x7f000000,
    SCEFX_WIN32NOMENU  = 0x01000000,
    SCEFX_WIN32NOFRAME = 0x02000000
  };

enum SCEFX_DEVICE
  {
    SCEFX_DEFAULT = 0,
    SCEFX_DEVICE_IDENTIFIER = 0x0000ffff,
    SCEFX_DEVICE_DISABLE_DRVINFO = 0x00800000
  };

struct SCE_WINDOW_D3D_
  {
  
    static SCE_WINDOW_D3D_ *instance_;
  
    struct Gate
      {
        SCE_WINDOW_D3D_ *operator->() { return SCE_WINDOW_D3D_::instance_; }
      };
  
    bool       IsActive();
    bool       IsRubbish();
    bool       IsFullscreen();
    void       MakeRubbish();
    int        GetWidth();
    int        GetHeight();
    int        WndWidth();
    int        WndHeight();
    u32_t      GetCaps();
    SCED3D     GetDevice();
    SCERROR    BeginDraw(unsigned flags = 0);
    SCERROR    EndDraw(unsigned flags = 0);
    SCE_EVENT  NextEvent(bool processMsgQue);
    SCERROR    SetMode(SCEFX_VALUE wndmode=SCEFX_UNKNOWN,SCEFX_VALUE=SCEFX_UNKNOWN,SCEFX_VALUE=SCEFX_UNKNOWN,SCEFX_VALUE=SCEFX_UNKNOWN,u32_t=0);
    SCERROR    GetMode(SCEFX_VALUE* wndmode=0,SCEFX_VALUE* mode=0,SCEFX_VALUE* bpp=0,SCEFX_VALUE* refrate=0,u32_t* = 0);
    SCERROR    RestoreContext();
    SCERROR    Print(int x,int y,pwide_t);
    int        TextWidth(pwide_t);
    int        TextHeight();
    SCERROR    Clear();
    SCERROR    ClearDepth();
    HWND       GetWindowHandle();
    void       SetFillColor(mrgb_t color);
    void       SetOffsetX( float x ) { offsetx_ = x; MakeRubbish(); }
    void       SetOffsetY( float y ) { offsety_ = y; MakeRubbish(); }
    float      OffsetX() { return offsetx_; }
    float      OffsetY() { return offsety_; }
    float      OffsetXfloor() { return floor(offsetx_+0.5f); }
    float      OffsetYfloor() { return floor(offsety_+0.5f); }
    float      offsetx_;
    float      offsety_;
  };
 
static SCE_WINDOW_D3D_::Gate WIND3DFX;
  
#endif ___6c153da0_cd33_4310_8681_683b7336cb3b___
