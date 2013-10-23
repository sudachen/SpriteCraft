
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

#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "RBGFilter.h"
#include "CollideMgr.h"
#include "SpriteHelper.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Sprites$Primitive")
#endif

SpritePrimitive::SpritePrimitive(tagStyle style)
  {
    style_      = style;
    colorized_  = false;
    clip_       = false;
    needupdate_ = false;
    uvnorm_     = false;
    cw_         = false;
    blendmode_  = BlendModeDefault;
    xoffset_    = 0;
    yoffset_    = 0;
  }

SpritePrimitive::~SpritePrimitive()
  {
  }

HRESULT __stdcall SpritePrimitive::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(ISpritePrimitive),riid)?S_OK:S_FALSE;
  }

void SpritePrimitive::SetSpriteWH(float w,float h) 
  { 
    spr_width_ = w; 
    spr_height_ = h; 
    needupdate_ = true; 
  }

TriangledSprite* SpritePrimitive::GetTriangled() 
  { 
    return this; 
  }

SpritePrimitive* SpritePrimitive::AsPrimitive() 
  { 
    return this; 
  }

HRESULT SpritePrimitive::CheckRange(long no,long count)
  {
    if ( no < 0 || count < 0 || unsigned(no+count) > vertices_.Count() )
      return GenSCWideError(L"primitive index out of range");
    return S_OK;
  }

void SpritePrimitive::TriangledImageType(SPRITE_IMAGE_TYPE* spritetype)
  {
    switch ( blendmode_ ) 
      {
      case BlendMoldeSolid:   *spritetype = SPRITE_SOLID;         break;
      case BlendMoldeBitmask: *spritetype = SPRITE_BITMASK;       break;
      case BlendMoldeAlpha:   *spritetype = SPRITE_BLEND;         break;
      case BlendMoldeAdditive:*spritetype = SPRITE_ADDITIVE;      break;
      case BlendModeDefault:
      default:
        *spritetype = SPRITE_INVALID_TYPE;  break;
      }
  }

bool SpritePrimitive::TriangledUpdate(
  SPRITE_IMAGE_TYPE* spritetype, bool* colorized, BufferT<VxB_VERTEX>& vxb )
  {
    bool updated = false;

    if ( needupdate_ ) 
      {
    
        float spr_height_1 = 1/spr_height_;
        float spr_width_1  = 1/spr_width_;

        if ( style_ == StyleTrilist ) 
          {
            vxb.Resize(vertices_.Count());
            for ( unsigned i = 0; i < vertices_.Count(); ++i ) 
              {
                VxB_VERTEX& to = vxb[i];
                PRIMITIVE_VERTEX& from = vertices_[i];
                to.color = mRGBA_t(
                  (from.color&0x00ff0000U) >> 16,
                  (from.color&0x0000ff00U) >> 8,
                  from.color&0x000000ffU,
                  from.color >> 24);
                to.vy = -from.y;
                to.vx = from.x;
                if ( uvnorm_ ) 
                  {
                    to.ts = from.u * spr_width_1;
                    to.tt = from.v * spr_height_1;
                  } 
                else 
                  {
                    to.ts = from.u;
                    to.tt = from.v;
                  }
              }
            if ( cw_ )
              for ( unsigned i = 0; i < vertices_.Count()/3; ++i ) 
                {
                  VxB_VERTEX tmp = vxb[i*3];
                  vxb[i*3] = vxb[i*3+2];
                  vxb[i*3+2] = tmp;
                }

            *colorized = colorized_;
          }
        else // StyleShape
          { 
            vxb.Resize(0);
            *colorized = false;
            teggo::PolygonT<float> poly;
            if ( cw_ ) 
              for ( int i = vertices_.Count()-1; i >= 0; --i )
                poly.AddPoint(teggo::Vect2T<float>(vertices_[i].x,-vertices_[i].y));
            else
              for ( unsigned i = 0; i < vertices_.Count(); ++i )
                poly.AddPoint(teggo::Vect2T<float>(vertices_[i].x,-vertices_[i].y));
            if ( clip_ ) 
              {
                BufferT< teggo::PolygonT<float> > poly2;
                BufferT< teggo::PolygonT<float> > waste;
                poly.VSlice(0,waste,poly2);
                (poly2.Empty() ? teggo::PolygonT<float>() : poly2.First()).Swap(poly);
                waste.Resize(0); poly2.Resize(0);
                poly.VSlice(spr_width_,poly2,waste); 
                (poly2.Empty() ? teggo::PolygonT<float>() : poly2.First()).Swap(poly);
                waste.Resize(0); poly2.Resize(0);
                poly.HSlice(0,waste,poly2);
                (poly2.Empty() ? teggo::PolygonT<float>() : poly2.First()).Swap(poly);
                waste.Resize(0); poly2.Resize(0);
                poly.HSlice(-spr_height_,poly2,waste); 
                (poly2.Empty() ? teggo::PolygonT<float>() : poly2.First()).Swap(poly);
                waste.Resize(0); poly2.Resize(0);
              }
            if ( poly.Count() ) 
              {
                BufferT< teggo::TriangleT<float> > triangles;
                poly.Triangulate(triangles);
                vxb.Resize(triangles.Count()*3);
                for ( unsigned i=0; i < triangles.Count(); ++i ) 
                  {
                    VxB_VERTEX* vx = &vxb[0] + i*3;
                    {
                      const float px = triangles[i].p0(0);
                      const float py = -triangles[i].p0(1);
                      vx[0].Xyz(px + xoffset_, -(py + yoffset_));
                      vx[0].Tex(px * spr_width_1, py * spr_height_1);
                    }
                    {
                      const float px = triangles[i].p1(0);
                      const float py = -triangles[i].p1(1);
                      vx[1].Xyz(px + xoffset_, -(py + yoffset_));
                      vx[1].Tex(px * spr_width_1, py * spr_height_1);
                    }
                    {
                      const float px = triangles[i].p2(0);
                      const float py = -triangles[i].p2(1);
                      vx[2].Xyz(px + xoffset_, -(py + yoffset_));
                      vx[2].Tex(px * spr_width_1, py * spr_height_1);
                    }
                  }
              }
          }
    
        needupdate_ = false;
        updated = true;
      }

      switch ( blendmode_ ) 
        {
        case BlendMoldeSolid:   *spritetype = SPRITE_SOLID;         break;
        case BlendMoldeBitmask: *spritetype = SPRITE_BITMASK;       break;
        case BlendMoldeAlpha:   *spritetype = SPRITE_BLEND;         break;
        case BlendMoldeAdditive:*spritetype = SPRITE_ADDITIVE;      break;
        case BlendModeDefault:
        default:
          *spritetype = SPRITE_INVALID_TYPE;  break;
        }

    return updated;
  }


HRESULT __stdcall SpritePrimitive::get_colorized(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = colorized_?VARIANT_TRUE:VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_colorized(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    colorized_ = newVal!=VARIANT_FALSE;
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::get_count(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = vertices_.Count();
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_count(long newVal)
  {
    _SYNCHRONIZE_;
    vertices_.Resize(newVal);
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::get_blendMode(enum tagBlendMode *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = blendmode_;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_blendMode(enum tagBlendMode newVal)
  {
    _SYNCHRONIZE_;
    blendmode_ = newVal;
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::SetColor(long no, long count, long color)
  {
    _SYNCHRONIZE_;
    if ( FAILED(CheckRange(no,count)) ) return E_FAIL;
    for ( int i = no; i < no+count; ++i ) vertices_[i].color = color;
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::SetXY(long no, float x, float y)
  {
    _SYNCHRONIZE_;
    if ( FAILED(CheckRange(no,1)) ) return E_FAIL;
    vertices_[no].x = x;
    vertices_[no].y = y;  
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::SetUV(long no, float u, float v)
  {
    _SYNCHRONIZE_;
    if ( FAILED(CheckRange(no,1)) ) return E_FAIL;
    vertices_[no].u = u;
    vertices_[no].v = v;  
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::SetVertexEx(long no, PRIMITIVE_VERTEX *vertex, long count)
  {
    _SYNCHRONIZE_;
    if ( FAILED(CheckRange(no,count)) ) return E_FAIL;
    for ( int i = 0; i < count; ++i ) vertices_[i+no] = vertex[i];
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::SetVertex(long no, long color, float x, float y, float u, float v)
  {
    _SYNCHRONIZE_;
    if ( FAILED(CheckRange(no,1)) ) return E_FAIL;
    PRIMITIVE_VERTEX& vx = vertices_[no];
    vx.color = color;
    vx.x = x;
    vx.y = y;
    vx.u = u;
    vx.v = v;
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::DiffVertex(long no, long count, float x, float y, float u, float v)
  {
    _SYNCHRONIZE_;

    if ( FAILED(CheckRange(no,count)) ) 
      return E_FAIL;
    
    for ( int i=no; i<no+count; ++i ) 
      {
        PRIMITIVE_VERTEX& vx = vertices_[i];
        vx.x += x;
        vx.y += y;
        vx.u += u;
        vx.v += v;
      }
    
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::GetVertexEx(long no, PRIMITIVE_VERTEX *vertex, long count)
  {
    _SYNCHRONIZE_;

    if ( FAILED(CheckRange(no,count)) ) 
      return E_FAIL;
    
    for ( int i = 0; i < count; ++i ) 
      vertex[i] = vertices_[i+no];
	  
    return S_OK;
  }

HRESULT __stdcall SpritePrimitive::GetX(long no, float *retVal)
  {
    _SYNCHRONIZE_;
    
    if ( FAILED(CheckRange(no,1)) ) 
      return E_FAIL;

    *retVal = vertices_[no].x;
	  
    return S_OK;
  }

HRESULT __stdcall SpritePrimitive::GetY(long no, float *retVal)
  {
    _SYNCHRONIZE_;
    
    if ( FAILED(CheckRange(no,1)) ) 
      return E_FAIL;
    
    *retVal = vertices_[no].y;
	  
    return S_OK;
  }

HRESULT __stdcall SpritePrimitive::GetU(long no, float *retVal)
  {
    _SYNCHRONIZE_;
    
    if ( FAILED(CheckRange(no,1)) ) 
      return E_FAIL;
    
    *retVal = vertices_[no].u;
	  
    return S_OK;
  }

HRESULT __stdcall SpritePrimitive::GetV(long no, float *retVal)
  {
    _SYNCHRONIZE_;

    if ( FAILED(CheckRange(no,1)) ) 
      return E_FAIL;
    
    *retVal = vertices_[no].v;
	  
    return S_OK;
  }

HRESULT __stdcall SpritePrimitive::GetColor(long no, long *retVal)
  {
    _SYNCHRONIZE_;
    
    if ( FAILED(CheckRange(no,1)) ) 
      return E_FAIL;
    
    *retVal = vertices_[no].color;

	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::get_xOffset(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = xoffset_;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_xOffset(float newVal)
  {
    _SYNCHRONIZE_;
    xoffset_ = newVal;
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::get_yOffset(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = yoffset_;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_yOffset(float newVal)
  {
    _SYNCHRONIZE_;
    yoffset_ = newVal;
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::get_clip(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = clip_?VARIANT_TRUE:VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_clip(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    clip_ = newVal != VARIANT_FALSE;
    needupdate_ = true;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::get_uvNorm(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = uvnorm_?VARIANT_TRUE:VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_uvNorm(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    uvnorm_ = newVal != VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall SpritePrimitive::get_cw(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = cw_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpritePrimitive::put_cw(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    cw_ = newVal!=VARIANT_FALSE;
	  return S_OK;
  }
