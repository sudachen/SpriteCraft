
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

//#pragma comment(lib,"dxguid.lib")

namespace gfxD3D7 {

int GfxD3D7Device::GetMaxTextureSize() const
{
  int size = 0;
  // ...
  return size?size:256;
}

SCERROR GfxD3D7Device::SetFillerColor(float r, float g, float b)
{
  filler_color_ = D3DRGBA__(r,g,b,1);
  return SCE_OK;
}

SCERROR GfxD3D7Device::SetFillerDepth(float depth)
{
  filler_depth_ = depth;
  return SCE_OK;
}

SCERROR GfxD3D7Device::Clear(unsigned flags)
{
  unsigned clr = 0;
  if ( flags ) {
    if ( flags & GFX3D_CLEAR_SCREEN )  clr |= D3DCLEAR_TARGET;
    if ( flags & GFX3D_CLEAR_ZBUFFER ) clr |= D3DCLEAR_ZBUFFER;
  }else clr = D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET;
  fail_if_dxerr( idd_->Clear(0,0,clr,filler_color_,filler_depth_,0) );
  return SCE_OK;
}

SCERROR GfxD3D7Device::SetMaterial(const GFX3D_MATERIAL* material)
{

#if 0
  
//  static GFX3D_MATERIAL default_material = 
//  {
//    /* diffuse  */  {1,1,1,1},
//    /* emission */  {0,0,0,1},
//    /* specular */  {0,0,0,1},
//    /* shininess */ 0
//  };
//
//  if ( !material ) {
//    SetMaterial(&default_material);
//  }else{
//  	D3DMATERIAL8 mat = {0};
//  	memcpy(&mat.Diffuse,&material->diffuse,sizeof(material->diffuse));
//  	memcpy(&mat.Ambient,&material->diffuse,sizeof(material->diffuse));
//	 	memcpy(&mat.Specular,&material->specular,sizeof(material->specular));
//	 	memcpy(&mat.Emissive,&material->emission,sizeof(material->emission));
//	 	mat.Power = material->shininess;
//	 	fail_if_dxerr ( idd_->SetMaterial( &mat ) );
//  }

#endif

  return SCE_OK;
}

} // namespace
