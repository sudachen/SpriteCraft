
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

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$gfxD3D7")
#endif

namespace gfxD3D7 {
  pwide_t const D3D7_x_s_AT_s_d = L"gfxd3d7: ERROR:D3D7(0x%08x) on %? at %?:%?";
  pwide_t const D3D7_herr_x     = L"gfxd3d7: failed with HRESULT 0x%08x";
}

NAMESPACE_MSKIN_BEGIN

GfxDevice* CreateD3D7Device(GFX_VALUE winmod,GFX_VALUE vmode,GFX_VALUE bpp,bool disable_drv_info)
{
  auto_ref<gfxD3D7::GfxD3D7Device> gfx ( new gfxD3D7::GfxD3D7Device());
  if ( gfx->Initialize(winmod,vmode,bpp,disable_drv_info) != SCE_OK ) return 0;
  else return Forget(gfx);
}

NAMESPACE_MSKIN_END


