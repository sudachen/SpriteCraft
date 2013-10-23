
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

#include "precompile.h"
#include "m_d3d7_device.h"
//#include "../../lib.classes/sysutil.h"

static HINSTANCE dd7hModule__ = 0;

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma code_seg(".text$gfxD3D7")
#pragma comment(lib,"dxguid.lib")
#endif

namespace gfxD3D7 {

static pchar_t gfxd3d7 = "gfxd3d7: ";
typedef HRESULT (WINAPI* DirectDrawCreateEx_FN)( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );

pchar_t const GfxD3D7Object::TypeID__ = "#TYPEID#gfxD3D7#GfxD3D7Object";
void* GfxD3D7Object::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return Adaptable::Adaptate_(id);
}

pchar_t const GfxD3D7Device::TypeID__ = "#TYPEID#gfxD3D7#GfxD3D7Device";
void* GfxD3D7Device::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  if ( void* ifs = GfxDevice::Adaptate_(id) ) return ifs;
  if ( void* ifs = Gfx3DDevice::Adaptate_(id) ) return ifs;
  if ( void* ifs = GfxControl::Adaptate_(id) ) return ifs;
  return IRefObject::Adaptate_(id);
}

widestring GfxD3D7Device::ToString_() const {
  return "gfxD3D7";
}

void GfxD3D7Device::Release_() const { refbase::Release_(); }
void GfxD3D7Device::Grab_() const { refbase::Grab_(); }

bool GfxD3D7Device::IsRubbish() const { return Win32ScreenDevice::IsRubbish(); }
void GfxD3D7Device::MakeRubbish()     { Win32ScreenDevice::MakeRubbish(); }
bool GfxD3D7Device::IsActive() const  { return Win32ScreenDevice::IsActive(); }
int  GfxD3D7Device::GetWidth() const  { return width_; }
int  GfxD3D7Device::GetHeight() const { return height_; }

EVENT GfxD3D7Device::NextEvent(bool processMsgQue) {
  if ( context_restored_ ) {
    EVENT e; e.type = EVENT::CONTEXT_RESET;
    context_restored_ = false;
    return e;
  }
  else
    return Win32ScreenDevice::NextEvent(processMsgQue);
}


GfxD3D7Device::GfxD3D7Device()
{
}

SCERROR GfxD3D7Device::InitializeDDraw()
{
  //dd7hModule__ = LoadLibrary("ddraw.dll");
  //if ( !dd7hModule__ ) fail_because("gfxd3d7: can't load ddraw.dll library");
  //DirectDrawCreateEx_FN fn = (DirectDrawCreateEx_FN)GetProcAddress(dd7hModule__,"DirectDrawCreateEx");
  //if ( !fn ) EMERGENCY(L"gfxd3d7: can't find DirectDrawCreateEx API");
  //fail_if_dxerr(fn(0,(void**)&+idraw_,IID_IDirectDraw7,0));
  +idraw_ = Refe(DD_Instance());
  return SCE_OK;
}

SCERROR GfxD3D7Device::InitializeD3D()
{
  fail_if_dxerr(idraw_->QueryInterface(IID_IDirect3D7,(void**)&+id3d_));
  return SCE_OK;
}

DDPIXELFORMAT GfxD3D7Device::texpf_[D3D7_ITEX_COUNT];

static HRESULT CALLBACK D3DEnumPixelFormatsCallback(
  LPDDPIXELFORMAT pDdpf,LPVOID lpContext)
{
  DDPIXELFORMAT* pP = reinterpret_cast<DDPIXELFORMAT*>(lpContext);

  if( pDdpf->dwFlags & DDPF_FOURCC )  return DDENUMRET_OK;
  if( pDdpf->dwFlags & DDPF_LUMINANCE )  return DDENUMRET_OK;
  if( pDdpf->dwFlags & DDPF_COMPRESSED ) return DDENUMRET_OK;
  if( pDdpf->dwFlags & DDPF_RGB ) {
    if ( pDdpf->dwFlags&DDPF_PALETTEINDEXED8 ) {
      pP[D3D7_ITEX_PAL8] = *pDdpf;
      Lout << _S*"%?TEXTURE (+) PALETTE(%d)" %gfxd3d7 %pDdpf->dwRGBBitCount;
    }
    else if( pDdpf->dwFlags & DDPF_ALPHAPIXELS ) {
      bool usable = false;
      if ( pDdpf->dwRGBBitCount == 32 && pDdpf->dwRGBAlphaBitMask == 0xff000000 )
        usable = true, pP[D3D7_ITEX_RGBA8] = *pDdpf;
      else if ( pDdpf->dwRGBBitCount == 16 && pDdpf->dwRGBAlphaBitMask == 0xf000 )
        usable = true, pP[D3D7_ITEX_RGBA4] = *pDdpf;
      else if ( pDdpf->dwRGBBitCount == 16 && pDdpf->dwRGBAlphaBitMask == 0x8000 )
        usable = true, pP[D3D7_ITEX_RGB5A1] = *pDdpf;
      unsigned chno = pDdpf->dwRGBBitCount/8*2;
      Lout << _S*(_S*"%%?TEXTURE (%%s) %%d bit ARGB(%%0%dx,%%0%dx,%%0%dx,%%0%dx)" %chno %chno %chno %chno )
        %gfxd3d7
        %(usable?"+":"-")
        %pDdpf->dwRGBBitCount
        %pDdpf->dwRGBAlphaBitMask
        %pDdpf->dwRBitMask
        %pDdpf->dwGBitMask
        %pDdpf->dwBBitMask;
    }
    else {
      bool usable = false;
      if ( pDdpf->dwRGBBitCount == 16 && pDdpf->dwGBitMask == 0x03e0 ) {
        usable=true,pP[D3D7_ITEX_RGB5] = *pDdpf;
      }
      unsigned chno = pDdpf->dwRGBBitCount/8*2;
      Lout << _S*(_S*"%%?TEXTURE (%%s) %%d bit RGB(%%0%dx,%%0%dx,%%0%dx)" %chno %chno %chno )
        %gfxd3d7
        %(usable?"+":"-")
        %pDdpf->dwRGBBitCount
        %pDdpf->dwRBitMask
        %pDdpf->dwGBitMask
        %pDdpf->dwBBitMask;
    }
    return DDENUMRET_OK;
  }
  else if ( pDdpf->dwFlags&DDPF_ALPHA ) {
    if ( pDdpf->dwRGBBitCount == 8 ) { pP[D3D7_ITEX_ALPHA] = *pDdpf; }
    Lout << _S*"%?TEXTURE (+) %d bit ALPHA" %gfxd3d7 %pDdpf->dwRGBBitCount;
  }
  return DDENUMRET_OK;
}

SCERROR GfxD3D7Device::EnumerateTextureFormats()
{
  memset(texpf_,0,sizeof(texpf_));
  fail_if_dxerr( idd_->EnumTextureFormats( D3DEnumPixelFormatsCallback, texpf_ ) );
  return SCE_OK;
}

static HRESULT CALLBACK LogHALCaps(
  LPSTR lpDeviceDescription,
  LPSTR lpDeviceName,
  LPD3DDEVICEDESC7 lpD3DDeviceDesc,
  LPVOID lpContext
)
{
  if ( memcmp(&lpD3DDeviceDesc->deviceGUID,&IID_IDirect3DHALDevice,sizeof(GUID)) == 0 ) {
    //LogF("%s%s",gfxd3d7,lpDeviceName);
    Lout << _S*"%?%?" %gfxd3d7 %lpDeviceDescription;
    if ( lpD3DDeviceDesc->dwDevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP ) {
      Lout << _S*"%?can queue rendering commands after a page flip" %gfxd3d7;
      ((GfxD3D7Device*)lpContext)->flip_immediate_ = true;
    }
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwMiscCaps & D3DPMISCCAPS_MASKZ  )
//      LogF("%scan enable and disable modification of the depth buffer",gfxd3d7);
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANISOTROPY )
//      LogF("%ssupports anisotropic filtering",gfxd3d7);
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_DITHER )
//      LogF("%scan dither to improve color resolution",gfxd3d7);
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGRANGE )
//      LogF("%ssupports range-based fog",gfxd3d7);
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE )
//      LogF("%scalculates the fog value by referring to a lookup table",gfxd3d7);
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX )
//      LogF("%scalculates the fog value during the lighting operation",gfxd3d7);
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZFOG )
//      LogF("%ssupports z-based fog",gfxd3d7);
    if ( lpD3DDeviceDesc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE  )
      Lout << _S*"%?supports alpha palettized texture surfaces" %gfxd3d7;
//    if ( lpD3DDeviceDesc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP )
//      LogF("%ssupports cubic environment mapping",gfxd3d7);
//    if ( lpD3DDeviceDesc->dwStencilCaps ) {
//      string ops = "";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_DECR )
//        ops += "decr ";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_DECRSAT )
//        ops += "decrsat ";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_INCR )
//        ops += "incr ";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_INCRSAT )
//        ops += "incrsat ";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_INVERT )
//        ops += "invert ";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_KEEP )
//        ops += "keep ";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_REPLACE )
//        ops += "replace ";
//      if ( lpD3DDeviceDesc->dwStencilCaps & D3DSTENCILCAPS_ZERO )
//        ops += "zero ";
//      LogF("%sstencil operations (%s)",gfxd3d7,ops);
//    }
//    if ( lpD3DDeviceDesc->dwVertexProcessingCaps & D3DVTXPCAPS_VERTEXFOG )
//      LogF("%ssupports vertex fog",gfxd3d7);
    Lout << _S*"%?number of texture-blending stages supported: %d"
      %gfxd3d7
      %lpD3DDeviceDesc->wMaxTextureBlendStages;
    //LogF("%snumber of texture stages supported: %d",gfxd3d7,lpD3DDeviceDesc->wMaxSimultaneousTextures);
    Lout << _S*"%?maximum texture: %d x %d"
      %gfxd3d7
      %lpD3DDeviceDesc->dwMaxTextureWidth
      %lpD3DDeviceDesc->dwMaxTextureHeight;
  }
  return D3DENUMRET_OK;
}

static void LogVMemSize(IDirectDraw7* ddraw)
{
  DDSCAPS2      ddsCaps2;
  DWORD         dwTotal;
  DWORD         dwFree;

  ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
  ddsCaps2.dwCaps = DDSCAPS_LOCALVIDMEM;
  if ( SUCCEEDED(ddraw->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree) ) ){
    Lout << _S*"%?local free %3.1fM of %3.1fM"
      %gfxd3d7
      %(float(dwFree)/(1024*1024))
      %(float(dwTotal)/(1024*1024));
  }
  ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
  ddsCaps2.dwCaps = DDSCAPS_NONLOCALVIDMEM;
  if ( SUCCEEDED(ddraw->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree) ) ){
    Lout << _S*"%?nonlocal free %3.1fM of %3.1fM"
      %gfxd3d7
      %(float(dwFree)/(1024*1024))
      %(float(dwTotal)/(1024*1024));
  }
//  ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
//  ddsCaps2.dwCaps = DDSCAPS_TEXTURE;
//  if ( SUCCEEDED(ddraw->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree) ) ){
//    LogF("gfxd3d7: textures free: %dM of %dM",dwFree/(1024*1024),dwTotal/(1024*1024));
//  }
}

SCERROR GfxD3D7Device::LogVmemSize()
{
  LogVMemSize(+idraw_);
  return SCE_OK;
}

SCERROR GfxD3D7Device::Initialize(GFX_VALUE winmod,GFX_VALUE vmode,GFX_VALUE bpp,bool disable_drv_info)
{
  bpp_     = GFX_UNKNOWN;
  wndMode_ = GFX_UNKNOWN;
  mode_    = GFX_UNKNOWN;
  refrate_ = GFX_UNKNOWN;
  width_   = 0;
  height_  = 0;
  filler_color_ = D3DRGBA(0,0,0,1);
  filler_depth_ = 1.0f;
  modelview_stack_count_ = 0;
  wModeNoMenu_ = false;
  wModeNoFrame_ = false;
  safe_ = true;
  flip_immediate_ = false;
  enableFlipping_ = true;
  enableExclusive_ = true;
  vsync_ = false;

  memset( texpf_, 0, sizeof(texpf_) );

  fail_if_fail(InitializeDDraw());

  {
    DDSCAPS2      ddsCaps2;
    DWORD         dwTotal,dwFree;
    ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
    ddsCaps2.dwCaps = DDSCAPS_LOCALVIDMEM;
    if ( SUCCEEDED(idraw_->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree) ) )
      video_local_mem_ = dwTotal;
  }

  if ( !disable_drv_info )
  {
    DDDEVICEIDENTIFIER2 idi;
    HRESULT hr = idraw_->GetDeviceIdentifier(&idi,/*D3DENUM_NO_WHQL_LEVEL*/ 0x00000002L);
    if ( hr == (HRESULT)E_INVALIDARG ) hr = idraw_->GetDeviceIdentifier(&idi,0);
    if ( SUCCEEDED(hr) ) {
      Lout << _S*"%?%?" %gfxd3d7 %idi.szDescription;
      Lout << _S*"%?DRIVER %-12? [%?]"
        %gfxd3d7 %idi.szDriver %teggo::GetModuleVersion(idi.szDriver);
    }
  }

  Lout << _S*"%?DDRAW  %-12? [%?]" %gfxd3d7 %"ddraw.dll" %teggo::GetModuleVersion("ddraw.dll");
  Lout << _S*"%?D3DIM  %-12? [%?]" %gfxd3d7 %"d3dim700.dll" %teggo::GetModuleVersion("d3dim700.dll");

  fail_if_fail(InitializeD3D());

  id3d_->EnumDevices(LogHALCaps,this);

  LogVMemSize(+idraw_);
  //Lout << "";

  //fail_if_fail(SetMode(GFX_WINDOWED,GFX_640x480,GFX_UNKNOWN,GFX_UNKNOWN));
  fail_if_fail(SetMode(winmod,vmode,bpp,GFX_UNKNOWN));
  context_restored_ = false;

  return SCE_OK;
}

GfxD3D7Device::~GfxD3D7Device()
{
  if ( wndMode_ == GFX_FULLSCREEN )
    if ( enableExclusive_ ) idraw_->RestoreDisplayMode();
    else ChangeDisplaySettings(0,0);

  TermD3DContext();

  for ( OBJECTS_::Iterator it  = objects_.Iterate(); it.Next(); )
  {
    Xdbg( Ldbg << _S*"%?unmanaged object: %?" %gfxd3d7 %(*it)->ToString_() );
    (*it)->OnDisposeServer_();
  }
}

u32_t GfxD3D7Device::GetCaps() const
{
  return 0
    |GFX_CAP_3D
    |GFX_CAP_3D_HARDWARE
    |GFX_CAP_ACCEL_ALPHA_BITS
    |GFX_CAP_ACCEL_ALPHA_VAL
    |GFX_CAP_ACCEL_BACKBUFFER
    |GFX_CAP_ACCEL_IN_VMEM
    |GFX_CAP_ACCEL_BITBLT
    |GFX_CAP_ACCEL_TRANSPARENCY
    |GFX_CAP_CONTROL
    |GFX_CAP_DEPTH_32
    |GFX_CAP_DEPTH_16
    |GFX_CAP_FULLSCREEN
    |GFX_CAP_WINDOWED
    |GFX_CAP_RESET_MODE
    |GFX_CAP_TEXNORMALIZE
    |( 0 != texpf_[D3D7_ITEX_PAL8].dwFlags    ? GFX_CAP_PAL8  :0)
    |( 0 != texpf_[D3D7_ITEX_RGBA8].dwFlags   ? GFX_CAP_RGBA8 :0)
    |( 0 != texpf_[D3D7_ITEX_RGBA4].dwFlags   ? GFX_CAP_RGBA4 :0)
    |( 0 != texpf_[D3D7_ITEX_RGB5A1].dwFlags  ? GFX_CAP_RGB5A1:0)
    |( 0 != texpf_[D3D7_ITEX_RGB5].dwFlags    ? GFX_CAP_RGB5  :0)
    ;
}

Surface* GfxD3D7Device::CreateDirtySurface(unsigned w,unsigned h,unsigned fmt)
{
  return GenericSurface::Create(w,h,fmt);
}

SCERROR GfxD3D7Device::BeginDraw(unsigned flags)
{
  if ( !iddsPrim_ )  return SCE_FAIL;
  HRESULT hr;

  if ( safe_ )
    while (  DDERR_WASSTILLDRAWING == (hr = iddsPrim_->GetFlipStatus(DDGFS_ISFLIPDONE)))
      teggo::ProcessYield();
  else
    if (  DDERR_WASSTILLDRAWING == (hr = iddsPrim_->GetFlipStatus(DDGFS_ISFLIPDONE)))
      teggo::ProcessYield();


  if ( hr == DDERR_SURFACELOST )
  {
    if ( enableExclusive_ && wndMode_ == GFX_FULLSCREEN )
      if ( S_OK != idraw_->TestCooperativeLevel() )
        return SCE_SURFLOST;

    if (  iddsPrim_->IsLost() && FAILED(hr = iddsPrim_->Restore()) ) return SCE_SURFLOST;
    if ( wndMode_ != GFX_FULLSCREEN || !enableFlipping_ )
      if (  iddsBack_->IsLost() && FAILED(hr = iddsBack_->Restore()) ) return SCE_SURFLOST;
    if ( iddsZbuff_->IsLost() && FAILED(hr = iddsZbuff_->Restore()) ) return SCE_SURFLOST;

    { DWORD val;  if ( +idd_ ) fail_if_dxerr(idd_->ValidateDevice(&val)); }

    Lout << _S*"%?dirty respawn device" %gfxd3d7;
    if ( iddsPrim_->IsLost() ) return SCE_FAIL;  // Hack !!

    Unrefe(idd_);
    fail_if_dxerr ( CreateDevice() );
    D3DVIEWPORT7 vp = { 0, 0, width_, height_, 0.0f, 1.0f };
    fail_if_dxerr(idd_->SetViewport(&vp));
    RestoreRenderStates();

    SCREEN::RestoreContext();
  }
  if ( FAILED(idd_->BeginScene()) ) return SCE_SURFLOST;
  fail_if_dxerr(idd_->Clear(0,0,D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0xffffff,1,0));
  FlushTransforms();
  return SCE_OK;
}

void GfxD3D7Device::OnPaint()
{
   EndDraw(0x70000000);
}

SCERROR GfxD3D7Device::EndDraw(unsigned flags)
{
  if ( !!idd_ ) {
    HRESULT hr = S_OK;
    if ( !(flags & 0x70000000) )
    {
      hr = idd_->EndScene();
      teggo::ProcessYield();
    }
    //fail_if_dxerr( hr );
    if ( FAILED(hr) ) return SCE_FAIL;
    if ( wndMode_ != GFX_FULLSCREEN || !enableFlipping_ ) {
      RECT crect;
      HWND hwnd = Win32ScreenDevice::GetWnd();
      GetClientRect (hwnd, &crect);
      POINT point1 = { crect.left, crect.top };
      POINT point2 = { crect.right, crect.bottom };
      //POINT point1 = { crect.left, crect.top };
      //POINT point2 = { crect.left+width_, crect.left+height_ };
      ClientToScreen (hwnd, &point1);
      ClientToScreen (hwnd, &point2);
      RECT drect = { point1.x, point1.y, point2.x, point2.y };
      RECT srect = { 0, 0, crect.right, crect.bottom };
      //RECT srect = { 0, 0, width_, height_ };
      //iddsBack_->GetFlipStatus()

      if ( srect.right-srect.left > 0 && srect.bottom-srect.top > 0 ) {
        HRESULT hr = S_OK;
        do {
          hr = iddsPrim_->Blt(&drect, +iddsBack_, &srect, DDBLT_DONOTWAIT, 0);
          //hr = iddsPrim_->Blt(&drect, ~iddsBack_, &srect, DDBLT_WAIT, 0);
        } while ( hr == DDERR_WASSTILLDRAWING );
        if ( hr == DDERR_INVALIDRECT )
          Lout <<
            _S*"%?(flip) Blt IVALIDRECT from {%d,%d,%d,%d} to {%d,%d,%d,%d}"
              %gfxd3d7
              %srect.left %srect.top %srect.right %srect.bottom
              %drect.left %drect.top %drect.right %drect.bottom;
        else if ( hr == DDERR_SURFACELOST ) return SCE_SURFLOST;
        else fail_if_dxerr(hr);
      }

    }else{
      HRESULT hr;
      //while (  DDERR_WASSTILLDRAWING == (hr = iddsPrim_->GetFlipStatus(DDGFS_ISFLIPDONE))) _Yield();
      while (  DDERR_WASSTILLDRAWING == (hr = iddsPrim_->GetFlipStatus(DDGFS_CANFLIP)))
        teggo::ProcessYield();
      if ( hr == S_OK ) {
        //hr = iddsPrim_->Flip(0,DDFLIP_WAIT);
        while ( DDERR_WASSTILLDRAWING == (
          hr = iddsPrim_->Flip(0,(!vsync_? DDFLIP_NOVSYNC|DDFLIP_DONOTWAIT : DDFLIP_DONOTWAIT) )) )
            teggo::ProcessYield();
//        hr = iddsPrim_->Flip(0,(!vsync_? DDFLIP_NOVSYNC|DDFLIP_DONOTWAIT : DDFLIP_DONOTWAIT) );
        if ( hr == DDERR_SURFACELOST ) return SCE_SURFLOST;
        warn_if_dxerr(hr);
      }
      else {
        if ( hr == DDERR_SURFACELOST ) return SCE_SURFLOST;
        warn_if_dxerr(hr);
      }
    }
  }
  return SCE_OK;
}

Surface* GfxD3D7Device::FrameBuffer()
{
  return 0;
}

void GfxD3D7Device::DetachObject(GfxD3D7Object* rst)
{
  if ( GfxD3D7Object **o = objects_.Get(rst) )
    objects_.Erase(rst);
  else
    return_because(_S*"%?object %p is not listed on server" %gfxd3d7  %rst);
}

void GfxD3D7Device::AttachObject(GfxD3D7Object* rst)
{
  if ( GfxD3D7Object **o = objects_.Get(rst) )
    return_because(_S*"%?object %p is already listed on server" %gfxd3d7 %rst);
  else
    objects_.Put(rst,rst);
}

widestring GfxD3D7Object::ToString_() const
{
  return Adaptable::ToString_();
}

GfxD3D7Object::GfxD3D7Object(GfxD3D7Device* srv)
{
  REQUIRE ( srv != 0 );
  srv_ = srv;
  srv_->AttachObject(this);
}

GfxD3D7Object::~GfxD3D7Object()
{
  if (srv_) srv_->DetachObject(this);
}


SCERROR GfxD3D7Device::SetTitle(pwide_t title) {
  Win32ScreenDevice::SetWndTitle(title);
  return SCE_OK;
}

widestring GfxD3D7Device::GetTitle() const {
  return Win32ScreenDevice::GetWndTitle();
}

SCERROR GfxD3D7Device::NativeCursor(bool enable) {
  Win32ScreenDevice::EnableMouseCursor(enable);
  return SCE_OK;
}

SCERROR GfxD3D7Device::GetWindowHandle(void* hwnd,u32_t sz) {
  fail_if ( sz != sizeof(HWND) );
  *(HWND*)hwnd = Win32ScreenDevice::GetWnd();
  return SCE_OK;
}

SCERROR GfxD3D7Device::GetWindowSize(long* width,long* height)
{
  if ( width )  *width  = wnd_width_;
  if ( height ) *height = wnd_height_;
  return SCE_OK;
}

} // namespace
