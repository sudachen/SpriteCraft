
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

SCERROR GfxD3D7Device::Translate(float x,float y, float z)
{
  GFXD3D_MATRIX matrix = *(GFXD3D_MATRIX*)&nvmath::mat4_id;
  matrix._41 = x;
  matrix._42 = y;
  matrix._43 = z;
  fail_if_dxerr(idd_->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&matrix));
  return SCE_OK;
}

SCERROR GfxD3D7Device::Scale(float x,float y, float z)
{
  GFXD3D_MATRIX matrix;
  memset(&matrix,0,sizeof(matrix));
  matrix._11 = x;
  matrix._22 = y;
  matrix._33 = z;
  matrix._44 = 1;
  fail_if_dxerr(idd_->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&matrix));
  return SCE_OK;
}

SCERROR GfxD3D7Device::Rotate(float angle,float x,float y,float z)
{
  nvmath::mat4 m = nvmath::mat4_id;
  m.set_rot(Radians(angle),nvmath::vec3(x,y,z));
  fail_if_dxerr(idd_->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)&m));
  return SCE_OK;
}

SCERROR GfxD3D7Device::RotateRad(float angle,float x,float y,float z)
{
  nvmath::mat4 m = nvmath::mat4_id;
  m.set_rot(angle,nvmath::vec3(x,y,z));
  fail_if_dxerr(idd_->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)&m));
  return SCE_OK;
}

SCERROR GfxD3D7Device::PushTransform()
{
  GFXD3D_MATRIX matrix;
  fail_if_dxerr( idd_->GetTransform(D3DTRANSFORMSTATE_WORLD,&matrix) );
  ++modelview_stack_count_;
  modelview_stack_.push_back(matrix);
  return SCE_OK;
}

SCERROR GfxD3D7Device::PopTransform(int depth)
{
  fail_if ( !modelview_stack_count_ );
  fail_if ( modelview_stack_count_ < unsigned(depth)+1 );
  if ( signed(modelview_stack_.size()) > depth ) {
    modelview_stack_.resize(modelview_stack_.size()-depth);
    fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_WORLD,&modelview_stack_.back()) );
    modelview_stack_.pop_back();
  }else{
    modelview_stack_.clear();
    fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)&nvmath::mat4_id) );
  }
  modelview_stack_count_ -= depth+1;

  return SCE_OK;
}

SCERROR GfxD3D7Device::TopTransform()
{
  if ( modelview_stack_.empty() ) {
    fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)&nvmath::mat4_id) );
  }else{
    fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_WORLD,&modelview_stack_.back()) );
  }
  return SCE_OK;
}

SCERROR GfxD3D7Device::FlushTransforms()
{
  if ( modelview_stack_count_ ) {
    modelview_stack_count_ = 0;
    modelview_stack_.clear();
    fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)&nvmath::mat4_id) );
  }
  return SCE_OK;
}

SCERROR GfxD3D7Device::Transform(const mat4& tm)
{
  fail_if_dxerr( idd_->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)&tm) );
  return SCE_OK;
}


SCERROR GfxD3D7Device::Ortho2D(float width,float height,float zNear,float zFar)
{
  if ( width == 0 )  width  = float(wnd_width_);
  if ( height == 0 ) height = float(wnd_height_);
  D3DMATRIX proj;
//  idd_->GetTransform(D3DTRANSFORMSTATE_PROJECTION,&proj);
//  XMessage(("%f, %f, %f, %f",proj._11,proj._12,proj._13,proj._14));
//  XMessage(("%f, %f, %f, %f",proj._21,proj._22,proj._23,proj._24));
//  XMessage(("%f, %f, %f, %f",proj._31,proj._32,proj._33,proj._34));
//  XMessage(("%f, %f, %f, %f",proj._41,proj._42,proj._43,proj._44));
  memset(&proj,0,sizeof(proj));
  proj._11 = 1.0f/(width/2);
  proj._22 = 1.0f/(height/2);
  proj._33 = -1.0f/(zFar-zNear);
  proj._44 = 1;
  proj._41 = -1.0f;
  proj._42 = -1.0f;
  proj._43 = 0;
  fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_PROJECTION,&proj) );
  fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)&nvmath::mat4_id) );
  fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,(D3DMATRIX*)&nvmath::mat4_id) );
  fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE));

//  nvmath::mat4 m = nvmath::mat4_id;
//  m.set_translation(nvmath::vec3(0.5,0.5,0));
//  fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT2));
//  fail_if_dxerr( idd_->SetTransform(D3DTRANSFORMSTATE_TEXTURE0,(D3DMATRIX*)&m) );

  return SCE_OK;
}

SCERROR GfxD3D7Device::Perspective(float fovy,float scale,float zNear,float zFar,float aspect)
{
  return SCE_OK;
}

} // namespace