
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
#include "../nvmath/algebra.h"
#include "m_gfx3d.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

using namespace nvmath;

const vec3 AXIS_X ( 1, 0, 0 );
const vec3 AXIS_Y ( 0, 1, 0 );
const vec3 AXIS_Z ( 0, 0, 1 );

pchar_t const Gfx3DDevice::TypeID__ = "#TYPEID#mskin::Gfx3DDevice";

void* Gfx3DDevice::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return Adaptable::Adaptate_(id);
}

pchar_t const Texture::TypeID__ = "#TYPEID#mskin::Texture";
void* Texture::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return IRefObject::Adaptate_(id);
}
Texture::Texture() {}
Texture::~Texture() {}

pchar_t const VxB::TypeID__ = "#TYPEID#mskin::VxB";
void* VxB::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return IRefObject::Adaptate_(id);
}
VxB::VxB() {}
VxB::~VxB() {}

const VxB_VERTEX VxB_VERTEX::EMPTY = {0};
void MOLE_FUNCALL ComputeNormals(
  VxB_VERTEX* vertices,int vcount,const u16_t* indices,int icount)
{
  return_if ( icount%3 != 0 );
  {for ( int i=0; i < vcount; ++i ) 
    vertices[i].nx = vertices[i].ny = vertices[i].nz = 0;}
  {for ( int i = 0; i < icount ; i+=3 ) {
    const vec3& p0 = *(vec3*)&vertices[indices[i+0]].vx;
    const vec3& p1 = *(vec3*)&vertices[indices[i+1]].vx;
    const vec3& p2 = *(vec3*)&vertices[indices[i+2]].vx;
    vec3 v0 = p1-p0;
    vec3 v1 = p2-p0;
    vec3 nor;
    cross(nor,v0,v1);
    normalize(nor);
    *(vec3*)&vertices[indices[i+0]].nx += nor;
    *(vec3*)&vertices[indices[i+1]].nx += nor;
    *(vec3*)&vertices[indices[i+2]].nx += nor;
  }}

  {for ( int i=0; i < vcount; ++i ) 
    normalize(*(vec3*)&vertices[i+0].nx);}
}

void MOLE_FUNCALL ComputeNormals(
  BufferT<VxB_VERTEX>& vertices, const BufferT<u16_t>& indices)
{
  return_if ( vertices.Count() == 0 || indices.Count() == 0 );
  ComputeNormals(&vertices[0],vertices.Count(),&indices[0],indices.Count());
}

void FillBlackMaterial(GFX3D_MATERIAL* m) {
  return_if ( !m );
  m->diffuse    = fRGBA_t(0,0,0,1);
  m->emission   = fRGBA_t(0,0,0,1);
  m->shininess  = 0;
  m->specular   = fRGBA_t(0,0,0,1);
}

void FillWhiteMaterial(GFX3D_MATERIAL* m) {
  return_if ( !m );
  m->diffuse    = fRGBA_t(1,1,1,1);
  m->emission   = fRGBA_t(1,1,1,1);
  m->shininess  = 0;
  m->specular   = fRGBA_t(0,0,0,0);
}

SCERROR Gfx3DDevice::LookAt(const vec3& eye, const vec3& point, const vec3& up )
{
  return SCE_OK;
}

SCERROR Gfx3DDevice::LookTo(const vec3& point, float distance, float longitude, float lantitude, float tetha)
{
  Translate(0,0,-distance);
  Rotate(longitude,AXIS_X);
  Rotate(lantitude,AXIS_Z);
  return SCE_OK;
}

SCERROR Gfx3DDevice::LookFrom(const vec3& eye, float longitude, float lantitude, float tetha)
{
  return SCE_OK;
}

NAMESPACE_MSKIN_END


