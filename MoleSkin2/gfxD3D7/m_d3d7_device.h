
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

#if !defined __gfx_d3d7_device_h__
#define __gfx_d3d7_device_h__

#include "../mcore.h"
#include "../mgraphics.h"
#include "../mGRAPHICS/m_win32wnd.h"

USING_MSKINSPACE;

#define DIRECT3D_VERSION 0x700
#define D3D_OVERLOADS
#include <ddraw.h>
#include <d3d.h>
#include "m_d3d7_debug.h"

#include "../../../Media/lib.ddraw/ddraw_inst.h"

enum {
  D3D7_ITEX_RGBA8 = 0,
  D3D7_ITEX_RGBA4,
  D3D7_ITEX_RGB5,
  D3D7_ITEX_RGB5A1,
  D3D7_ITEX_ALPHA,
  D3D7_ITEX_PAL8,
  D3D7_ITEX_COUNT
};

#define D3DRGBA__(r, g, b, a) \
    ( ((u32_t((a)*255)&0x0ff) << 24) \
    | ((u32_t((r)*255)&0x0ff) << 16) \
    | ((u32_t((g)*255)&0x0ff) << 8)  \
    | (u32_t((b)*255)&0x0ff) \
    )

namespace gfxD3D7 {

class GfxD3D7Device;

class GfxD3D7Object : public virtual Adaptable{
  GfxD3D7Device* srv_;
public:
  static pchar_t const TypeID__;
  virtual widestring  ToString_() const;
  virtual void*   Adaptate_(const void*);
  virtual void    OnTermContext()    {}
  virtual void    OnInitContext()    {}
  virtual void    OnDisposeServer()  { srv_ = 0; }
          void    OnDisposeServer_() { srv_ = 0; OnDisposeServer(); }
  GfxD3D7Device*  GetServer()        { return srv_; }
  GfxD3D7Object(GfxD3D7Device* srv);
  virtual ~GfxD3D7Object();
};

struct GfxD3D7VxB : public VxB, public GfxD3D7Object, private refbase {
  unsigned format_;
  int count_;
  rcc_ptr<IDirect3DVertexBuffer7>  vertices_;

  static pchar_t      const TypeID__;
  virtual widestring  ToString_() const;
  virtual void*       Adaptate_(const void*);
  virtual void        Release_() const;
  virtual void        Grab_() const;
  virtual int         Count() const;
  virtual bool        IsLost() const;
  virtual SCERROR Drop();
  virtual SCERROR Restore();
  virtual SCERROR Fill(const VxB_VERTEX* vertices, int first, int count);

  GfxD3D7VxB(GfxD3D7Device*,int count,unsigned flags);
  virtual ~GfxD3D7VxB();

  virtual void OnTermContext();
  virtual void OnInitContext();
};

struct GfxD3D7Texture : public Texture, public GfxD3D7Object, private refbase{
  int      width_;
  int      height_;
  unsigned format_;
  unsigned iformat_;
  unsigned mipmap_;
  float    priority_;
  rcc_ptr<IDirectDrawSurface7> texture_;

  static  pchar_t     const TypeID__;
  virtual void*       Adaptate_(const void*);
  virtual widestring  ToString_() const;
  virtual void        Release_() const;
  virtual void        Grab_() const;
  virtual SCERROR Load(Surface* surf,int level=0);
  virtual SCERROR LoadSubpicture(Surface*,int x,int y,const rect_t&,int level = 0);
  virtual SCERROR LoadPalette(const BufferT<mrgba_t>& pal);
  virtual SCERROR StoreSubpicture(Surface*,int x,int y,const rect_t&) const;
  virtual SCERROR StorePalette(BufferT<mrgba_t>& pal) const;
  virtual int         GetWidth() const;
  virtual int         GetHeight() const;
  virtual unsigned    GetFormat() const;
  virtual void        Drop();
  virtual SCERROR Restore();
  virtual bool        IsLost() const;
  virtual void        SetPriority(float priority);

  GfxD3D7Texture(GfxD3D7Device*,int width,int height, unsigned format,unsigned mipmap);
  virtual ~GfxD3D7Texture();

  SCERROR         Bind();
  virtual void        OnInitContext();
  virtual void        OnTermContext();

  SCERROR RestoreTextureObject();
};

typedef D3DMATRIX GFXD3D_MATRIX;

class GfxD3D7Device :
  public GfxDevice,
  public GfxControl,
  public Gfx3DDevice,
  public virtual IRefObject,
  private refbase,
  private Win32ScreenDevice
{
  GFX_VALUE bpp_;
  GFX_VALUE wndMode_;
  GFX_VALUE mode_;
  GFX_VALUE refrate_;

  bool wModeNoMenu_;
  bool wModeNoFrame_;
  bool enableFlipping_;
  bool enableExclusive_;
  bool vsync_;
  bool safe_;

  rcc_ptr<IDirectDraw7>         idraw_;
  rcc_ptr<IDirectDrawSurface7>  iddsPrim_;
  rcc_ptr<IDirectDrawSurface7>  iddsBack_;
  rcc_ptr<IDirectDrawSurface7>  iddsZbuff_;
  rcc_ptr<IDirectDrawClipper>   iddClip_;
  rcc_ptr<IDirect3D7>           id3d_;
  rcc_ptr<IDirect3DDevice7>     idd_;
  rcc_ptr<IDirect3DVertexBuffer7> internal_vxb_;

  int width_;
  int height_;

  D3DCOLOR filler_color_;
  float filler_depth_;
  bool  context_restored_;

  typedef FlatmapT<GfxD3D7Object*,GfxD3D7Object*> OBJECTS_;
  OBJECTS_ objects_;

  friend struct GfxD3D7Texture;
  friend struct GfxD3D7VxB;

  auto_ref<GfxD3D7Texture> locked_texture_;
  auto_ref<GfxD3D7VxB>     locked_vxb_;
  BufferT<GFXD3D_MATRIX>    modelview_stack_;
  unsigned                 modelview_stack_count_;

  unsigned internal_vxfirst_;
  unsigned internal_vxcount_;
  unsigned internal_vxflags_;
  unsigned internal_max_vx_;
  unsigned video_local_mem_;

  int wnd_width_;
  int wnd_height_;

  SCERROR FillInternalVxB_(const VxB_VERTEX* vertices,unsigned count,unsigned flags);
  SCERROR ReCreatePrimarySurface();
  HRESULT CreateDevice();
  HRESULT RestoreRenderStates();

  virtual void OnResize();
  virtual void OnPaint();

public:
  bool flip_immediate_;

  static DDPIXELFORMAT texpf_[D3D7_ITEX_COUNT];
  static pchar_t const TypeID__;
  virtual widestring  ToString_() const;
  virtual void*       Adaptate_(const void*);
  virtual void        Release_() const;
  virtual void        Grab_() const;
// GfxControl
  virtual SCERROR SetTitle(pwide_t title);
  virtual widestring  GetTitle() const;
  virtual SCERROR NativeCursor(bool);
  virtual SCERROR GetWindowHandle(void*,u32_t);
  virtual SCERROR GetVSync(bool* val) { return *val = vsync_, SCE_OK;}
  virtual SCERROR SetVSync(bool val)  { return vsync_ = val, SCE_OK; }
  virtual SCERROR GetSafe(bool* val) { return *val = safe_, SCE_OK;}
  virtual SCERROR SetSafe(bool val)  { return safe_ = val, SCE_OK; }
  virtual SCERROR LogVmemSize();
  virtual SCERROR AttachTo(const void* handle);
  virtual SCERROR GetWindowSize(long* width,long* height);

// GfxDevice
  virtual u32_t     GetCaps() const;
  virtual Surface*    CreateDirtySurface(unsigned w,unsigned h,unsigned fmt);
  virtual SCERROR BeginDraw(unsigned flags = 0);
  virtual SCERROR EndDraw(unsigned flags = 0 );
  virtual Surface*    FrameBuffer();
  virtual bool        IsRubbish() const;
  virtual void        MakeRubbish();
  virtual bool        IsActive() const;
  virtual int         GetWidth() const;
  virtual int         GetHeight() const;
  virtual SCERROR SetMode(GFX_VALUE,GFX_VALUE,GFX_VALUE,GFX_VALUE,u32_t=0);
  virtual SCERROR GetMode(GFX_VALUE*,GFX_VALUE*,GFX_VALUE*,GFX_VALUE*,u32_t* = 0) const;
  virtual EVENT       NextEvent(bool processMsgQue);

// Gfx3DDevice
  virtual Texture*    CreateDirtyTexture(int width,int height,unsigned flags,unsigned mipmap);
  virtual VxB*        CreateDirtyVxB(int count,unsigned flags=GFX3D_VxB_DEFAULT);
  virtual SCERROR SetState(GFX3D_STATE,u32_t val,float ref=0);
  virtual int         GetMaxTextureSize() const;
  virtual SCERROR SetVxB(VxB* vxb);
  virtual SCERROR DrawVxB(GFX3D_PRIMITIVE,int first=0,int count=-1);
  virtual SCERROR DrawIndexedVxB(GFX3D_PRIMITIVE, const u16_t* idx,int count,unsigned vxfirst,unsigned vxcount);
  virtual SCERROR DrawPrimitive(GFX3D_PRIMITIVE,const VxB_VERTEX*,int count,unsigned flags);
  virtual SCERROR DrawIndexedPrimitive(GFX3D_PRIMITIVE,const VxB_VERTEX*, unsigned vxcount,const u16_t* idx,int count,unsigned flags);
  virtual SCERROR SetFillerColor(float r, float g, float b);
  virtual SCERROR SetFillerDepth(float depth);
  virtual SCERROR Clear(unsigned);
  virtual SCERROR SetTexture(Texture*);
  virtual SCERROR SetMaterial(const GFX3D_MATERIAL*);
  virtual SCERROR SetLight(unsigned no,const GFX3D_LIGHT*);
  virtual SCERROR LightON(unsigned no, bool);
  virtual SCERROR SetAmbientLight(float r,float g,float b,float a);
  virtual SCERROR Ortho2D(float width,float height,float zNear,float zFar);
  virtual SCERROR Perspective(float fovy,float scale,float zNear,float zFar,float aspect);
  virtual SCERROR Translate(float x,float y, float z);
  virtual SCERROR Scale(float x,float y, float z);
  virtual SCERROR Rotate(float angle,float x,float y,float z);
  virtual SCERROR RotateRad(float angle,float x,float y,float z);
  virtual SCERROR PushTransform();
  virtual SCERROR PopTransform(int depth=0);
  virtual SCERROR TopTransform();
  virtual SCERROR FlushTransforms();
  virtual SCERROR Transform(const mat4&);

  GfxD3D7Device();
  virtual ~GfxD3D7Device();

  void DetachObject(GfxD3D7Object*);
  void AttachObject(GfxD3D7Object*);

  SCERROR Initialize(GFX_VALUE,GFX_VALUE,GFX_VALUE,bool disable_drv_info);
  SCERROR InitializeDDraw();
  SCERROR InitializeD3D();
  SCERROR AttachDepthBuffer();
  SCERROR EnumerateTextureFormats();

private:

  SCERROR SetFullscreenMode();
  SCERROR SetWindowedMode();
  SCERROR InitD3DContext();
  void        TermD3DContext();
};

} // namespace

#endif // __gfx_ogldevice_h__
