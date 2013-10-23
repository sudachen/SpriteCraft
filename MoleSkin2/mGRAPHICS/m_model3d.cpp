
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
#include "m_screen.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

using namespace nvmath;

void Object3D::Release_() const { refbase::Release_(); }
void Object3D::Grab_()    const { refbase::Grab_(); }
Object3D::Object3D() : refer_(0) {}
Object3D::~Object3D() {}

void MaterialInfo::Release_() const { refbase::Release_(); }
void MaterialInfo::Grab_()    const { refbase::Grab_(); }
MaterialInfo::MaterialInfo() {}
MaterialInfo::~MaterialInfo() {}

void Model3D::Release_()  const { refbase::Release_(); }
void Model3D::Grab_()     const { refbase::Grab_(); }
Model3D::Model3D() : cache_frame_(0xffff) {}
Model3D::~Model3D() {}


const mat4& Object3D::GetAbsTransform(u16_t frame) const {
  frame = frame%model_->frames_;
  if ( frame == 0 ) return absT_;
  else return track_[frame-1];
}

void Object3D::Transform(u16_t frame) const {
  Gfx3DDevice* gfx = SCREEN::Get3DDevice();
  return_if ( !gfx );
  gfx->Transform(GetAbsTransform(frame));
}

void Object3D::RelTransform() const {
  Gfx3DDevice* gfx = SCREEN::Get3DDevice();
  return_if ( !gfx );
  gfx->Transform(relT_);
}

void Object3D::GenVertices(VxB_VERTEX* vertices, u16_t fr_no) const
{
  if ( vxcount_ == 0 ) return;
  mat4 tr = GetAbsTransform(fr_no) * oord_;
  VxB_VERTEX* vx = vertices;
  VxB_VERTEX* const vxE = vx+vxcount_;
  memcpy(vx,&model_->vertices_[0]+vxfirst_,sizeof(VxB_VERTEX)*vxcount_);
  for ( ; vx != vxE ; ++vx ) {
    vx->Transform(tr);
  }
}

void Object3D::ComputeNormals(VxB_VERTEX* vertices) const
{
  if ( vxcount_ == 0 ) return;

  {for ( int i=0; i < vxcount_; ++i ) 
    vertices[i].nx = vertices[i].ny = vertices[i].nz = 0;}
  
  { vector<Object3D::SLICE>::const_iterator i = faces_.begin();
    vector<Object3D::SLICE>::const_iterator iE = faces_.end();
    for ( ; i != iE; ++i ) {
      u16_t ix = (*i).ixfirst_;
      u16_t ixE = ix + (*i).ixcount_;
      u16_t* trilist = &model_->slice_[(*i).slice_].indices_[0];
      for ( ; ix < ixE; ix+=3 ) {
        VxB_VERTEX &vx0 = vertices[trilist[ix+0]-vxfirst_];
        VxB_VERTEX &vx1 = vertices[trilist[ix+1]-vxfirst_];
        VxB_VERTEX &vx2 = vertices[trilist[ix+2]-vxfirst_];
        const vec3& p0 = *(vec3*)&vx0.vx;
        const vec3& p1 = *(vec3*)&vx1.vx;
        const vec3& p2 = *(vec3*)&vx2.vx;
        vec3 v0 = p1-p0;
        vec3 v1 = p2-p0;
        vec3 nor;
        cross(nor,v0,v1);
        normalize(nor);
        *(vec3*)&vx0.nx += nor;
        *(vec3*)&vx1.nx += nor;
        *(vec3*)&vx2.nx += nor;
      }
    }
  }

  {for ( int i=0; i < vxcount_; ++i ) 
    normalize(*(vec3*)&vertices[i+0].nx);}
}

void Model3D::GenVertices(VxB_VERTEX* vertices,u16_t frame) const
{
  if ( frames_ > 0 ) {
    int fr_no = frame%frames_;
    vector<AutoObject3D>::const_iterator i = objects_.begin();
    vector<AutoObject3D>::const_iterator iE = objects_.end();
    for ( ; i != iE; ++i ) 
        (*i)->GenVertices(vertices+(*i)->vxfirst_,fr_no);
  }
}

void Model3D::GenVertices(VERTICES& vertices,u16_t frame) const
{
  vertices.resize(vertices_.size(),VxB_VERTEX::EMPTY);
  GenVertices(&vertices[0],frame);
}

void Model3D::ComputeNormals(VxB_VERTEX* vertices) const
{
  vector<AutoObject3D>::const_iterator i = objects_.begin();
  vector<AutoObject3D>::const_iterator iE = objects_.end();
  for ( ; i != iE; ++i ) 
      (*i)->ComputeNormals(vertices+(*i)->vxfirst_);
}

void Model3D::ComputeNormals(VERTICES& vertices) const
{
  return_if ( vertices.size() < vertices_.size() );
  ComputeNormals(&vertices[0]);
}

void Model3D::ComputeNormals()
{
  ComputeNormals(&vertices_[0]);
}

Model3D::SLICE* Model3D::FindSliceByMaterial(pchar_t name) const
{
  vector<SLICE>::const_iterator i = slice_.begin(); 
  vector<SLICE>::const_iterator iE = slice_.end(); 
  for ( ; i != iE ; ++i ) 
    if ( (*i).Cmp(name) == 0 ) 
      return const_cast<SLICE*>(&(*i));
  return 0;
}

Object3D* Model3D::FindObjectByName(pchar_t name) const
{
  vector<AutoObject3D>::const_iterator i  = objects_.begin(); 
  vector<AutoObject3D>::const_iterator iE = objects_.end(); 
  const string nameS(name);
  for ( ; i != iE ; ++i ) 
    if ( (*i)->name_ == nameS ) 
      return (*i);
  return 0;
}

void Object3D::Draw_(bool use_vxb, unsigned vxfirst) const
{
  Gfx3DDevice* gfx = SCREEN::Get3DDevice();
  return_if ( !gfx );
  gfx->SetState(GFX3D_STATE_ENVMODE,GFX3D_TEXTURE_MODULATE);
  gfx->Transform(oord_);
  const vector<SLICE>& faces = refer_?refer_->faces_:faces_;
  vector<SLICE>::const_iterator i  = faces.begin();
  vector<SLICE>::const_iterator iE = faces.end();
  for ( ; i != iE; ++i ) {
    const Model3D::SLICE* slice = &model_->slice_[(*i).slice_];
    if ( slice->indices_.size() == 0 ) continue;
    if ( slice->material_ ) {
      if ( Texture* tex = slice->material_->texture_ ) {
        gfx->SetMaterial(0);
        gfx->SetTexture(tex);
      }else{
        gfx->SetMaterial(&slice->material_->colors_);
        gfx->SetTexture(0);
      }
    }else{ 
      gfx->SetMaterial(0);
      gfx->SetTexture(0);
    }
    if ( !use_vxb )
      gfx->DrawIndexedPrimitive(GFX3D_TRILIST,
        &model_->vertices_[0],
        model_->vertices_.size(),
        &slice->indices_[(*i).ixfirst_],(*i).ixcount_,
        GFX3D_VxB_VNT);
    else
      gfx->DrawIndexedVxB(GFX3D_TRILIST,&slice->indices_[(*i).ixfirst_],(*i).ixcount_,vxfirst);
  }
}

void Object3D::Draw() const
{
  Draw_();
}

void Object3D::DrawVxB(unsigned vxfirst) const
{
  Draw_(true,vxfirst);
}

void Model3D::Draw_(bool use_vxb,unsigned vxfirst) const
{
  Gfx3DDevice* gfx = SCREEN::Get3DDevice();
  return_if (!gfx);
  gfx->SetState(GFX3D_STATE_ENVMODE,GFX3D_TEXTURE_MODULATE);
  vector<SLICE>::const_iterator i  = slice_.begin();
  vector<SLICE>::const_iterator iE = slice_.end();
  for ( ; i != iE; ++i ) {
    if ( (*i).indices_.size() == 0 ) continue;
    if ( (*i).material_ ) {
      if ( Texture* tex = (*i).material_->texture_ ) {
        gfx->SetMaterial(0);
        gfx->SetTexture(tex);
      }else{
        gfx->SetMaterial(&(*i).material_->colors_);
        gfx->SetTexture(0);
      }
    }else{ 
      gfx->SetMaterial(0);
      gfx->SetTexture(0);
    }
    if ( !use_vxb ) 
      gfx->DrawIndexedPrimitive(GFX3D_TRILIST,
        &cache_vertices_[0],
        cache_vertices_.size(),
        &(*i).indices_[0],(*i).indices_.size(),
        GFX3D_VxB_VNT);
    else
      gfx->DrawIndexedVxB(GFX3D_TRILIST,
        &(*i).indices_[0],(*i).indices_.size(),vxfirst);
  }
}

void Model3D::Draw(u16_t frame) const
{
  frame = frame%frames_;
  if ( cache_frame_ != frame ) {
    GenVertices(cache_vertices_,frame);
    ComputeNormals(cache_vertices_);
    cache_frame_ = frame;
  }
  Draw_();
}

void Model3D::DrawVxB(unsigned vxfirst) const
{
  Draw_(true,vxfirst);
}

void Model3D::FillVxB(VxB* vxb,unsigned first) const
{
  vxb->Fill(&vertices_[0],first,vertices_.size());
}

void Model3D::GenVxB(u16_t frame,VxB* vxb,unsigned first) const
{
  VERTICES vertices;
  GenVertices(vertices,frame);
  vxb->Fill(&vertices[0],first,vertices.size());
}

NAMESPACE_MSKIN_END


