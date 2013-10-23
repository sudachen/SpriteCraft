
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

#if !defined (__m_GraphicsDevice_device_h__)
#define __m_GraphicsDevice_device_h__

#include "../mcore/m_autoconf.h"
#include "../mcore/m_singletons.h"
#include "m_types.h"
#include "m_color.h"
#include "m_surface.h"
#include "m_font.h"
#include "m_input.h"

NAMESPACE_MSKIN_BEGIN

enum GFX_VALUE{
  GFX_UNKNOWN = 0,
  GFX_WINDOWED,
  GFX_FULLSCREEN,
  GFX_HIDDEN,
  GFX_320x240,
  GFX_640x480,
  GFX_800x600,
  GFX_1024x768,
  GFX_1280x1024,
  GFX_1600x1200,
  GFX_BPP16,
  GFX_BPP32,
  GFX_REFRATE_DEFAULT,
  GFX_REFRATE_OPTIMAL,
  GFX_REFRATE_MAXIMUM,
  GFX_VALUE_DATA   = 0x00ffffff,
  GFX_VALUE_FLAGS  = 0x7f000000,
  GFX_WIN32NOMENU  = 0x01000000,
  GFX_WIN32NOFRAME = 0x02000000
};

enum GFX_DEVICE{
  GFX_DEFAULT_DEVICE = 0,
  GFX_GDI_DEVICE     = 1,
  GFX_DD7_DEVICE     = 2,
  GFX_OGL_DEVICE     = 3,
  GFX_D3D7_DEVICE    = 4,
  GFX_D3D8_DEVICE    = 5,
  GFX_DEVICE_IDENTIFIER = 0x0000ffff,
  GFX_DEVICE_DISABLE_DRVINFO = 0x00800000
};

enum {
  GFX_USE_BACKBUFFER  = MSKIN_BIT(0),
  GFX_DISABLE_HWACCEL = MSKIN_BIT(1),
  GFX_HARDWARE_TnL    = MSKIN_BIT(1),
};

enum{
  GFX_CAP_FULLSCREEN          = MSKIN_BIT(0),  // поддерживает полноэкранный режим
  GFX_CAP_WINDOWED            = MSKIN_BIT(1),  // поддерживает оконный режим
  GFX_CAP_ACCEL_BITBLT        = MSKIN_BIT(2),  // аппаратный блиттинг
  GFX_CAP_ACCEL_ALPHA_LAYER   = MSKIN_BIT(3),  // аппаратный альфа-лейер
  GFX_CAP_ACCEL_ALPHA_BITS    = MSKIN_BIT(4),  // аппаратная боддержка альфа компоненты цвета
  GFX_CAP_ACCEL_ALPHA_VAL     = MSKIN_BIT(5),  // аппаратная поддержка константной альфы
  GFX_CAP_ACCEL_TRANSPARENCY  = MSKIN_BIT(6),  // аппаратная поддержка цветового ключа
  GFX_CAP_ACCEL_IN_VMEM       = MSKIN_BIT(7),  // размещение сурфейсов в видео памяти
  GFX_CAP_ACCEL_BACKBUFFER    = MSKIN_BIT(8),  // аппаратный бак-буффер
  GFX_CAP_FB_ACCESS           = MSKIN_BIT(12), // поддерживается доступ к фреймбуфферу
  GFX_CAP_FB_ACCESS_RAW       = MSKIN_BIT(13), // доступ к фреймбуфферу прямой, без копирования во временную поверхность
  GFX_CAP_RESET_MODE          = MSKIN_BIT(14), // разрешается смена видео-режима
  GFX_CAP_DEPTH_32            = MSKIN_BIT(15), // поддеживается глубина цвета 32 бита
  GFX_CAP_DEPTH_16            = MSKIN_BIT(16), // поддеживается глубина цвета 32 бита   
  GFX_CAP_DEPTH_8             = MSKIN_BIT(17), // поддеживается политровый режим
  GFX_CAP_SET_PALETTE         = MSKIN_BIT(18), // разрешена смена палитры
  GFX_CAP_RGB5                = MSKIN_BIT(19), // 
  GFX_CAP_RGB5A1              = MSKIN_BIT(20), // 
  GFX_CAP_CONTROL             = MSKIN_BIT(21), // реализован интерфейс GfxControl
  GFX_CAP_3D                  = MSKIN_BIT(22), // реализован интерфейс Gfx3DDevice
  GFX_CAP_3D_HARDWARE         = MSKIN_BIT(23), // аппаратный 3D
  GFX_CAP_TnL_HARDWARE        = MSKIN_BIT(24), // аппаратный TnL
  GFX_CAP_PAL8                = MSKIN_BIT(25), //
  GFX_CAP_TEXNORMALIZE        = MSKIN_BIT(26), //
  GFX_CAP_RGBA8               = MSKIN_BIT(27), //
  GFX_CAP_RGBA4               = MSKIN_BIT(28), //
};

enum {
  GFX_ENABLE            = 1,
  GFX_DISABLE           = 0,
};

struct GfxControl : public virtual Adaptable{
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual SCERROR     SetPalette(const BufferT<mrgba_t>& pal) { return SCE_UNSUPPORTED; };
  virtual SCERROR     GetPalette(BufferT<mrgba_t>& pal) const {pal.Clear();return SCE_UNSUPPORTED;};
  virtual SCERROR     SetTitle(pwide_t) = 0; //{return SCE_UNSUPPORTED;};
  virtual widestring      GetTitle() const {return "";};
  virtual SCERROR     NativeCursor(bool) { return SCE_UNSUPPORTED; }
  virtual SCERROR     GetWindowHandle(void* handle,u32_t hsize) { return SCE_UNSUPPORTED; }
  virtual SCERROR     GetVSync(bool* val) { return SCE_UNSUPPORTED; }
  virtual SCERROR     SetVSync(bool val)  { return SCE_UNSUPPORTED; }
  virtual SCERROR     GetSafe(bool* val) { return SCE_UNSUPPORTED; }
  virtual SCERROR     SetSafe(bool val)  { return SCE_UNSUPPORTED; }
  virtual SCERROR     LogVmemSize()  { return SCE_UNSUPPORTED; }
  virtual SCERROR     AttachTo(const void* handle) { return SCE_UNSUPPORTED; }
  virtual SCERROR     GetWindowSize(long* width, long* height) { return SCE_UNSUPPORTED; }
protected:
  GfxControl();
  virtual ~GfxControl();
private:
  GfxControl(GfxControl&);
  GfxControl& operator=(const GfxControl&);
};

class GfxDevice : public virtual IRefObject{
public:
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual Surface*    CreateDirtySurface(unsigned w,unsigned h,unsigned fmt) = 0;
  virtual SCERROR BeginDraw(unsigned flags = 0) = 0;
  virtual SCERROR EndDraw(unsigned flags = 0 ) = 0;
  virtual Surface*    FrameBuffer() = 0;
  virtual bool        IsRubbish() const = 0;
  virtual void        MakeRubbish() = 0;
  virtual bool        IsActive() const = 0;
  virtual int         GetWidth() const = 0;
  virtual int         GetHeight() const = 0;
  virtual SCERROR SetMode(GFX_VALUE,GFX_VALUE,GFX_VALUE,GFX_VALUE,u32_t=0) = 0;
  virtual SCERROR GetMode(GFX_VALUE* wndmode=0,GFX_VALUE* mode=0,GFX_VALUE* bpp=0,GFX_VALUE* refrate=0,u32_t* = 0) const = 0;
  virtual u32_t     GetCaps() const = 0;
  virtual EVENT       NextEvent(bool processMsgQue) = 0;
protected:
  GfxDevice();
  virtual ~GfxDevice();
private:
  GfxDevice(GfxDevice&);
  GfxDevice& operator = (GfxDevice&);
};

GfxDevice*  MOLE_FUNCALL CreateDynamicDevice(GFX_DEVICE,GFX_VALUE,GFX_VALUE,GFX_VALUE);

GfxDevice* CreateD3D7Device(GFX_VALUE,GFX_VALUE,GFX_VALUE,bool disable_drv_info);
//GfxDevice* CreateDD7Device(GFX_VALUE,GFX_VALUE,GFX_VALUE);
//GfxDevice* CreateD3DDevice(unsigned flags=0);
//GfxDevice* CreateOGLDevice(unsigned flags=0);
//GfxDevice* CreateWCEDevice(unsigned flags=0);
//GfxDevice* CreatePTVDevice(unsigned flags=0);

NAMESPACE_MSKIN_END

#endif // __m_GraphicsDevice_device_h__
