
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
#include "m_gfx.h"
#include "m_gfx3d.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

pchar_t const GfxControl::TypeID__ = "#TYPEID#mskin::GfxControl";
void* GfxControl::Adaptate_(const void* id)
{
  if ( id == GfxControl::TypeID__ ) return this; 
  return Adaptable::Adaptate_(id);
}

pchar_t const GfxDevice::TypeID__ = "#TYPEID#mskin::GfxDevice";
void* GfxDevice::Adaptate_(const void* id)
{
  if ( id == GfxDevice::TypeID__ ) return this; 
  return IRefObject::Adaptate_(id);
}

#if defined MSKIN_USE_DLL && MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
static
GfxDevice* CreateGfxDeviceFromDLL(pchar_t dll_name)
{
  if ( HMODULE hlib = LoadLibrary(dll_name) )
    if ( FARPROC proc = GetProcAddress(hlib,"CreateGfxDevice") )
      return ((GfxDevice*(*)())proc)();
    else ret0_because("plugin dll has not symbol CreateGfxDevice");
  else ret0_because("could not found plugin dll");
}
#endif

# if defined _DEBUG   
# define PLUGIN_SUFFIX "_D"
# else
# define PLUGIN_SUFFIX 
#endif

GfxDevice* MOLE_FUNCALL CreateDynamicDevice( GFX_DEVICE device, GFX_VALUE winmod, GFX_VALUE vmode, GFX_VALUE bpp )
{
//#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL
//  return CreatePTVDevice( 0 );
//#elif MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 && \
//      MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WCE
//  return CreateWCEDevice( 0 );
//#elif MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 && \
//      MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WPC
  switch ( device&GFX_DEVICE_IDENTIFIER )
  {
  case GFX_D3D7_DEVICE:
    return CreateD3D7Device(winmod,vmode,bpp,(device&GFX_DEVICE_DISABLE_DRVINFO)!=0);
//  case GFX_OGL_DEVICE:
//  case GFX_DD7_DEVICE:
//  case GFX_GDI_DEVICE:
//  case GFX_DEFAULT_DEVICE:
//    return 0;
  default: ; // nothing
  }
  ret0_because("invalid device");
//#else
//  ret0_because("unknown device");
//#endif
}

GfxDevice::GfxDevice() {}
GfxDevice::~GfxDevice() {}
Gfx3DDevice::Gfx3DDevice() {}
Gfx3DDevice::~Gfx3DDevice() {}

GfxControl::GfxControl() {}
GfxControl::~GfxControl() {}

NAMESPACE_MSKIN_END
