
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

SCERROR GfxD3D7Device::LightON(unsigned no,bool enable)
{
  if ( no == GFX3D_LIGHTx ) { 
    if ( enable ) { for ( int i=0; i<8 ; ++ i ) warn_if_dxerr(idd_->LightEnable(i,TRUE)); }
    else { for ( int i=0; i<8 ; ++ i ) warn_if_dxerr(idd_->LightEnable(i,FALSE)); }
  }else{
    if ( no > 7 ) fail_because("gfxd3d7: only 8 lights can be enabled");
    if ( enable ) fail_if_dxerr(idd_->LightEnable(no,TRUE));
    else fail_if_dxerr(idd_->LightEnable(no,FALSE));
  }
  return SCE_OK;
}

static GFX3D_LIGHT default_direct_light = {
  GFX3D_DIRECTION_LIGHT,
  {1,1,1,1}, // diffuse
  {0},       // specular
  {0,0,1,0}  // position
};

SCERROR GfxD3D7Device::SetLight(unsigned no,const GFX3D_LIGHT* info)
{
  if ( no > 7 ) fail_because("gfxd3d7: only 8 lights can be enabled");
  if ( !info ) 
  	info = &default_direct_light;

  D3DLIGHT7 light;
  memset(&light,0,sizeof(light));
  memcpy(&light.dvDirection,&info->position,sizeof(info->position));
  memcpy(&light.dcvDiffuse,&info->diffuse,sizeof(info->diffuse));
  memcpy(&light.dcvSpecular,&info->specular,sizeof(info->specular));
  light.dltType = D3DLIGHT_DIRECTIONAL;
  fail_if_dxerr ( idd_->SetLight(no,&light) ); 

  return SCE_OK;
}

SCERROR GfxD3D7Device::SetAmbientLight(float r,float g,float b,float a)
{
  fail_if_dxerr ( idd_->SetRenderState(
    D3DRENDERSTATE_AMBIENT,	D3DRGBA__(r,g,b,a)));
  return SCE_OK;
}


} // namespace

