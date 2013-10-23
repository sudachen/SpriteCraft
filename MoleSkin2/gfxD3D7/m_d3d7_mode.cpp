
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

#if !defined ENUM_CURRENT_SETTINGS
#define ENUM_CURRENT_SETTINGS u32_t(-1)
#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$gfxD3D7")
#endif


namespace gfxD3D7 {

SCERROR GfxD3D7Device::AttachTo(const void* wnd)
{
  idraw_->SetCooperativeLevel(0,DDSCL_NORMAL);
  if ( wndMode_ == GFX_FULLSCREEN )
    idraw_->RestoreDisplayMode();
  TermD3DContext();
  Win32ScreenDevice::AttachTo(*(HWND*)wnd);
  if ( !*(HWND*)wnd )
    Win32ScreenDevice::ReNewWindow(width_,height_);
  SetWindowedMode();
  return SCREEN::RestoreContext();
}

SCERROR GfxD3D7Device::SetMode(GFX_VALUE wmode,GFX_VALUE mode,GFX_VALUE bpp,GFX_VALUE refrate,u32_t flags) 
{ 
  fail_if_fail ( wmode == GFX_UNKNOWN || wmode == GFX_FULLSCREEN || wmode == GFX_WINDOWED );
  bool change_modes = mode != GFX_UNKNOWN;
  long gfx_mode_flags = mode&GFX_VALUE_FLAGS;
  mode = GFX_VALUE(mode & GFX_VALUE_DATA);
  fail_if_fail ( mode == GFX_UNKNOWN 
    || mode == GFX_320x240 || mode == GFX_640x480 
    || mode == GFX_800x600 || mode == GFX_1024x768 
    || mode == GFX_1280x1024 || mode == GFX_1600x1200 );
  fail_if_fail ( bpp == GFX_UNKNOWN || bpp == GFX_BPP16 || bpp == GFX_BPP32 );
  fail_if_fail ( refrate == GFX_UNKNOWN 
    || refrate == GFX_REFRATE_DEFAULT 
    || refrate == GFX_REFRATE_MAXIMUM 
    || refrate == GFX_REFRATE_OPTIMAL );
  
  if ( bpp != GFX_UNKNOWN ) bpp_ = bpp;
  if ( mode != GFX_UNKNOWN ) mode_ = mode; else if ( mode_ == GFX_UNKNOWN ) mode_ = GFX_640x480;
  switch ( mode_ ) {
  case GFX_320x240:   width_ = 320;  height_ = 240; break;
  case GFX_640x480:   width_ = 640;  height_ = 480; break;
  case GFX_800x600:   width_ = 800;  height_ = 600; break;
  case GFX_1024x768:  width_ = 1024; height_ = 768; break;
  case GFX_1280x1024: width_ = 1280; height_ = 1024; break;
  case GFX_1600x1200: width_ = 1600; height_ = 1200; break;
  default:
    retval_because("gfxd3d7: invalid window dimenstion",SCE_FAIL);
  }

  idraw_->SetCooperativeLevel(0,DDSCL_NORMAL);
  idraw_->RestoreDisplayMode();
  TermD3DContext();
  //idraw_->SetCooperativeLevel(Win32ScreenDevice::GetWnd(),DDSCL_NORMAL);

  if (change_modes) {
    wModeNoMenu_  = ( (gfx_mode_flags & GFX_WIN32NOMENU) != 0 );
    wModeNoFrame_ = ( (gfx_mode_flags & GFX_WIN32NOFRAME) != 0 );
  }

  if ( Win32ScreenDevice::IsAttached() )
  {
    Win32ScreenDevice::AttachTo(0);
    Win32ScreenDevice::ReNewWindow(width_,height_);
  }

  if ( 
    ! Win32ScreenDevice::IsAttached()
    && (
      wmode == GFX_FULLSCREEN 
      || (wmode == GFX_UNKNOWN && wndMode_ == GFX_FULLSCREEN) ))
  {
    if ( SCE_OK != SetFullscreenMode() ) {
      TermD3DContext();
      idraw_->RestoreDisplayMode();
      fail_if_fail(SetWindowedMode());
    }
  }
  else 
    fail_if_fail(SetWindowedMode());
  
  SCREEN::RestoreContext();
  return SCE_OK; 
}

SCERROR GfxD3D7Device::GetMode(GFX_VALUE* wmode,GFX_VALUE* mode,GFX_VALUE* bpp,GFX_VALUE* refrate,u32_t* flags) const
{ 
  if ( wmode )   *wmode = wndMode_;
  if ( mode  )   *mode  = mode_;
  if ( bpp   )   *bpp   = bpp_;
  if ( refrate ) *refrate = refrate_;
  return SCE_OK; 
}


void GfxD3D7Device::TermD3DContext()
{
  if ( HWND hwnd = Win32ScreenDevice::GetWnd() )  
  {
    SetTexture(0);
    
    for ( OBJECTS_::Iterator it  = objects_.Iterate(); it.Next(); )
      (*it)->OnTermContext();
    
    Unrefe ( internal_vxb_ );
    Unrefe ( idd_ );
    Unrefe ( iddsBack_ );
    Unrefe ( iddsPrim_ );
    Unrefe ( iddsZbuff_ );
    Unrefe ( iddClip_ );
  }
}

static unsigned FlagsToBitDepth(unsigned dwFlags)
{
  if (dwFlags & DDBD_1)
    return 1UL;
  else if ((dwFlags & DDBD_2) == DDBD_2 )
    return 2UL;
  else if ((dwFlags & DDBD_4) == DDBD_4 )
    return 4UL;
  else if ((dwFlags & DDBD_8) == DDBD_8 )
    return 8UL;
  else if ((dwFlags & DDBD_16) == DDBD_16)
    return 16UL;
  else if ((dwFlags & DDBD_24) == DDBD_24 )
    return 24UL;
  else if ((dwFlags & DDBD_32) == DDBD_32 )
    return 32UL;
  else
    return 0UL; 
}

struct EnumZBufferCallback_t{
  DDPIXELFORMAT result;
  unsigned depth;
};

static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,void* x ){
  EnumZBufferCallback_t* ezb = (EnumZBufferCallback_t*)x;
  //LogF("gfxd3d7:ZBuffer: depth %d (%s)",pddpf->dwZBufferBitDepth,
  //  (pddpf->dwFlags&DDPF_ZBUFFER)?"ZBUFFER":"");    
  if( (pddpf->dwFlags&DDPF_ZBUFFER) && (pddpf->dwZBufferBitDepth == ezb->depth) && !ezb->result.dwSize)
  {
    //LogF("gfxd3d7: select z-buffer depth %d, mask 0x%08x",pddpf->dwZBufferBitDepth,pddpf->dwZBitMask);
    memcpy(&ezb->result, pddpf, sizeof(DDPIXELFORMAT) );
    //return D3DENUMRET_CANCEL;
  }
  return D3DENUMRET_OK;
}

SCERROR GfxD3D7Device::AttachDepthBuffer()
{
  DDCAPS caps = {0,};
  caps.dwSize = sizeof(DDCAPS);
  fail_if_dxerr(idraw_->GetCaps(&caps,0));
  if ( !(caps.ddsCaps.dwCaps & DDSCAPS_ZBUFFER) )
    fail_because("gfxd3d7: Z-buffer unsupported by DirectX");
  EnumZBufferCallback_t ezb;
  memset(&ezb,0,sizeof(ezb));
  ezb.depth = 16;
  memset(&ezb.result,0,sizeof(ezb.result));
  fail_if_dxerr(
    id3d_->EnumZBufferFormats(IID_IDirect3DHALDevice,EnumZBufferCallback,&ezb));
  if ( ezb.result.dwSize == 0 ) 
    fail_because("gfxd3d7: could not select valid Z-buffer ");
  
  int bbf_width  = width_;
  int bbf_height = height_;
  if ( Win32ScreenDevice::IsAttached() )
    bbf_width = GetSystemMetrics(SM_CXFULLSCREEN),
    bbf_height = GetSystemMetrics(SM_CYFULLSCREEN);

  DDSURFACEDESC2 ddsd = {0};
  ddsd.dwSize = sizeof(DDSURFACEDESC2);
  ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER|DDSCAPS_VIDEOMEMORY;
  ddsd.dwWidth        = bbf_width;
  ddsd.dwHeight       = bbf_height;
  memcpy( &ddsd.ddpfPixelFormat, &ezb.result, sizeof(DDPIXELFORMAT) );
  fail_if_dxerr(idraw_->CreateSurface(&ddsd,&+iddsZbuff_, NULL ));
  fail_if_dxerr(iddsBack_->AddAttachedSurface(+iddsZbuff_));
  return SCE_OK;
}

HRESULT GfxD3D7Device::CreateDevice()
{
  HRESULT hr = id3d_->CreateDevice(IID_IDirect3DHALDevice,+iddsBack_,&+idd_); 
  if ( FAILED(hr) ) 
    return (void)(Lerr << _S*"gfxd3d7: failed(0x%08x) to initialize D3DHAL device" %hr), hr;
  return S_OK;
}

HRESULT GfxD3D7Device::RestoreRenderStates()
{
  fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_ZENABLE,D3DZB_TRUE));
  fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL) );
  fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL) );
  fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL) );
  fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL) );
  fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE) );
  fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_CLIPPING,TRUE) );
  //fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE) );
  return S_OK;
}

SCERROR GfxD3D7Device::InitD3DContext()
{
  HWND hwnd;
  fail_if_fail ( hwnd = Win32ScreenDevice::GetWnd() );

  int bbf_width  = width_;
  int bbf_height = height_;
  if ( Win32ScreenDevice::IsAttached() )
    bbf_width = GetSystemMetrics(SM_CXFULLSCREEN),
    bbf_height = GetSystemMetrics(SM_CYFULLSCREEN);

  {
    DDSURFACEDESC2 desc;
	  memset(&desc,0,sizeof(desc));
	  desc.dwSize = sizeof(desc);
    if ( wndMode_ == GFX_FULLSCREEN && enableFlipping_ ) {
  	  desc.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
      desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|
        DDSCAPS_COMPLEX|DDSCAPS_FLIP|DDSCAPS_3DDEVICE;
      desc.dwBackBufferCount = 1;
    }else{
  	  desc.dwFlags = DDSD_CAPS;
      desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    }
	  
    fail_if_dxerr(idraw_->CreateSurface(&desc,&+iddsPrim_,0));
    if ( wndMode_ != GFX_FULLSCREEN  || !enableFlipping_ ) {
  	  fail_if_dxerr(idraw_->CreateClipper(0,&+iddClip_,0));
	    fail_if_dxerr(iddClip_->SetHWnd(0,Win32ScreenDevice::GetWnd()));
	    fail_if_dxerr(iddsPrim_->SetClipper(+iddClip_));
      //idraw_->FlipToGDISurface();
    }else{
    }
  	
    memset(&desc,0,sizeof (desc));
    desc.dwSize = sizeof (desc);
    if ( wndMode_ != GFX_FULLSCREEN || !enableFlipping_ ) {
      desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
      desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
      desc.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
      desc.dwWidth        = bbf_width;
      desc.dwHeight       = bbf_height;
	    fail_if_dxerr(idraw_->CreateSurface(&desc,&+iddsBack_,0));
    } else {
      DDSCAPS2 ddsCaps = { DDSCAPS_BACKBUFFER, 0, 0, 0 };
      fail_if_dxerr(iddsPrim_->GetAttachedSurface(&ddsCaps,&+iddsBack_));
    }
  }

  fail_if_dxerr(CreateDevice());
  OnResize();

  fail_if_fail(AttachDepthBuffer());
  fail_if_dxerr(idd_->SetRenderTarget(+iddsBack_,0));
  
  fail_if_fail(EnumerateTextureFormats());

  ShowWindow(hwnd,SW_SHOW);
  SetForegroundWindow(hwnd);
  SetFocus(hwnd);

  RestoreRenderStates();

  SetFillerColor(0,0,0);

  for ( OBJECTS_::Iterator it  = objects_.Iterate(); it.Next(); )
  {
    Xdbg( Ldbg << _S*"gfxd3d7: (after context reset) %?" %(*it)->ToString_() );
    (*it)->OnInitContext();
  }
  
  context_restored_ = true;
  return SCE_OK;  
}

void GfxD3D7Device::OnResize()
{
  if ( +idd_ /*&& wndMode_ != GFX_FULLSCREEN*/ )
  {
    D3DVIEWPORT7 vp = { 0, 0, width_, height_, 0.0f, 1.0f };
    if ( Win32ScreenDevice::IsAttached() )
    {
      RECT r;
      GetClientRect( Win32ScreenDevice::GetWnd(), &r );  
      vp.dwHeight = r.bottom - r.top;
      vp.dwWidth  = r.right - r.left;
    }
    if ( vp.dwWidth > 1 && vp.dwHeight > 1 ) 
    {
      wnd_width_  = vp.dwWidth;
      wnd_height_ = vp.dwHeight;
      warn_if_dxerr(idd_->SetViewport(&vp));
    }
  }
}

SCERROR GfxD3D7Device::SetFullscreenMode()
{
  HWND hwnd;
  int bpp;
  fail_if_fail ( hwnd = Win32ScreenDevice::GetWnd() );
  switch ( bpp_ ) {
  case GFX_UNKNOWN: bpp = 16;  break;
  case GFX_BPP16:   bpp = 16; break;
  case GFX_BPP32:   bpp = 32; break;
  default:
    retval_because("gfxd3d7: unsupported bitdepth",SCE_FAIL);
  }

  if ( bpp == 32 && video_local_mem_ <= 8*1024*1024 ) 
  {
    Lout << "gfxd3d7: videocard is lowpower, using 16 bit mode";
    bpp = 16;
  }

  if ( enableExclusive_ ) 
    fail_if_dxerr(idraw_->SetCooperativeLevel(hwnd,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN));
  else
    fail_if_dxerr(idraw_->SetCooperativeLevel(hwnd,DDSCL_NORMAL|DDSCL_FULLSCREEN));

  RECT wrect = {0};
  wrect.right = width_;
  wrect.bottom = height_;
  SetWindowLong(hwnd,GWL_STYLE,(LONG)(WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_POPUP));
  SetWindowLong(hwnd,GWL_EXSTYLE,(LONG)(WS_EX_APPWINDOW));
  AdjustWindowRectEx(&wrect,WS_POPUP,FALSE,WS_EX_APPWINDOW);
  if ( enableExclusive_ ) SetWindowPos(hwnd,HWND_TOPMOST,0,0,wrect.right - wrect.left,wrect.bottom - wrect.top,SWP_SHOWWINDOW);
  else SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,wrect.right - wrect.left,wrect.bottom - wrect.top,SWP_SHOWWINDOW);

  static DEVMODE dmScreenSettings;
  memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
  
  dmScreenSettings.dmSize=sizeof(dmScreenSettings);
  dmScreenSettings.dmPelsWidth  = width_;
  dmScreenSettings.dmPelsHeight = height_;
  dmScreenSettings.dmBitsPerPel = bpp;

  static DEVMODE dmScreenSettings2[2]; // memory overrun ???
  memset(dmScreenSettings2,0,sizeof(dmScreenSettings2));
  
  if ( refrate_ == GFX_UNKNOWN || refrate_ == GFX_REFRATE_OPTIMAL ) {
    if ( EnumDisplaySettings(0,ENUM_CURRENT_SETTINGS,dmScreenSettings2) )
      dmScreenSettings.dmDisplayFrequency = dmScreenSettings2[0].dmDisplayFrequency;
  }
  dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|
    (dmScreenSettings.dmDisplayFrequency?DM_DISPLAYFREQUENCY:0);

//    LogF("gfxd3d7: *fullscreen* %dx%dx%d@%d",
//      dmScreenSettings.dmPelsWidth,
//      dmScreenSettings.dmPelsHeight,
//      dmScreenSettings.dmBitsPerPel,
//      dmScreenSettings.dmDisplayFrequency);
  
  if( dmScreenSettings2[0].dmPelsWidth < dmScreenSettings.dmPelsWidth ||
     dmScreenSettings2[0].dmPelsHeight < dmScreenSettings.dmPelsHeight )
  {
    dmScreenSettings.dmDisplayFrequency = 0;
  }

  if( FAILED(idraw_->SetDisplayMode(
    dmScreenSettings.dmPelsWidth,
    dmScreenSettings.dmPelsHeight,
    dmScreenSettings.dmBitsPerPel,
    dmScreenSettings.dmDisplayFrequency,0)) && dmScreenSettings.dmDisplayFrequency != 0 )
  {
    fail_if_dxerr (
      idraw_->SetDisplayMode(
        dmScreenSettings.dmPelsWidth,
        dmScreenSettings.dmPelsHeight,
        dmScreenSettings.dmBitsPerPel,
        0,0));
  }
  
  memset(dmScreenSettings2,0,sizeof(dmScreenSettings2));
  dmScreenSettings2[0].dmSize = sizeof(DEVMODE);
  if ( EnumDisplaySettings(0,ENUM_CURRENT_SETTINGS,dmScreenSettings2) )
  {
    Lout << _S*"gfxd3d7: *fullscreen* %dx%dx%d@%d"
      %dmScreenSettings2[0].dmPelsWidth      
      %dmScreenSettings2[0].dmPelsHeight
      %dmScreenSettings2[0].dmBitsPerPel
      %dmScreenSettings2[0].dmDisplayFrequency;
  }
    
  wndMode_ = GFX_FULLSCREEN;
  reterr_if ( InitD3DContext() );
  return SCE_OK;
}

SCERROR GfxD3D7Device::SetWindowedMode()
{
  HWND hwnd = 0;
  fail_if_fail ( hwnd = Win32ScreenDevice::GetWnd() );

  fail_if_dxerr(idraw_->SetCooperativeLevel(hwnd,DDSCL_NORMAL));
  
  if ( ! Win32ScreenDevice::IsAttached() )
  {
    RECT wrect = {0};
    wrect.right = width_;
    wrect.bottom = height_;
    unsigned style = 
        ( wModeNoFrame_ ? WS_POPUP : (WS_OVERLAPPED| WS_CAPTION) ) 
        | ( !wModeNoMenu_ ? (WS_SYSMENU| WS_MINIMIZEBOX) : 0 ) 
        ;
    AdjustWindowRectEx(&wrect,style,FALSE,WS_EX_APPWINDOW);
    SetWindowLong(hwnd,GWL_STYLE,(LONG)(WS_CLIPSIBLINGS|WS_CLIPCHILDREN|style));
    SetWindowLong(hwnd,GWL_EXSTYLE,(LONG)(WS_EX_APPWINDOW));
    SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,wrect.right - wrect.left,wrect.bottom - wrect.top,SWP_SHOWWINDOW);
  }
  else
  {
    unsigned style = GetWindowLong(hwnd,GWL_STYLE);
    style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
    SetWindowLong(hwnd,GWL_STYLE,style);
  }
  
  UpdateWindow( hwnd );

  static DEVMODE dmScreenSettings2[2]; // memory overrun ???
  memset(dmScreenSettings2,0,sizeof(dmScreenSettings2));
  dmScreenSettings2[0].dmSize = sizeof(DEVMODE);
  if ( EnumDisplaySettings(0,ENUM_CURRENT_SETTINGS,dmScreenSettings2) )
  {
    Lout << _S*"gfxd3d7: *windowed* %dx%dx%d@%d"
      %width_
      %height_
      %dmScreenSettings2[0].dmBitsPerPel
      %dmScreenSettings2[0].dmDisplayFrequency;
  }
  wndMode_ = GFX_WINDOWED;
  reterr_if ( InitD3DContext() );
  return SCE_OK;
}

}
