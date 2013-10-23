
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

inline unsigned GfxFuncToD3D7(unsigned fn) {
  switch ( fn ) {
  case GFX3D_NEVER:   return D3DCMP_NEVER;
  case GFX3D_LESS:    return D3DCMP_LESS;
  case GFX3D_EQUAL:   return D3DCMP_EQUAL;
  case GFX3D_LEQUAL:  return D3DCMP_LESSEQUAL;
  case GFX3D_GREATER: return D3DCMP_GREATER;
  case GFX3D_NOTEQUAL:return D3DCMP_NOTEQUAL;
  case GFX3D_GEQUAL:  return D3DCMP_GREATEREQUAL;
  case GFX3D_ALWAYS:  return D3DCMP_ALWAYS;
  }
  return D3DCMP_FORCE_DWORD;
}


inline unsigned GfxWrapStToD3D7(unsigned val) {
  switch(val) {
  case GFX3D_TEXTURE_CLAMP:   return D3DTADDRESS_CLAMP;
  case GFX3D_TEXTURE_REPEAT:  return D3DTADDRESS_WRAP;
  }
  return D3DTADDRESS_FORCE_DWORD;
}

inline unsigned GfxFilterValToD3D7(unsigned val) {
  switch( val ) {
  case GFX3D_NEAREST: return D3DTFN_POINT;
  case GFX3D_LINEAR:  return D3DTFN_LINEAR;
  }
  return D3DTFN_FORCE_DWORD;
}

inline unsigned GfxBlendToD3D7(unsigned val)
{
  switch( val )
  {
  case GFX3D_ZERO:                return D3DBLEND_ZERO;
  case GFX3D_ONE:                 return D3DBLEND_ONE;
  case GFX3D_DST_COLOR:           return D3DBLEND_DESTCOLOR;
  case GFX3D_SRC_COLOR:           return D3DBLEND_SRCCOLOR;
  case GFX3D_ONE_MINUS_DST_COLOR: return D3DBLEND_INVDESTCOLOR;
  case GFX3D_ONE_MINUS_SRC_COLOR: return D3DBLEND_INVSRCCOLOR;
  case GFX3D_DST_ALPHA:           return D3DBLEND_DESTALPHA;
  case GFX3D_SRC_ALPHA:           return D3DBLEND_SRCALPHA;
  case GFX3D_ONE_MINUS_DST_ALPHA: return D3DBLEND_INVDESTALPHA;
  case GFX3D_ONE_MINUS_SRC_ALPHA: return D3DBLEND_INVSRCALPHA;
  }
  return D3DBLEND_FORCE_DWORD;
}

SCERROR GfxD3D7Device::SetState(GFX3D_STATE state,u32_t val,float ref)
{
  unsigned denum;
  switch (state) {
  case GFX3D_STATE_ALPHA_TEST:   
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,val==GFX3D_ENABLE?TRUE:FALSE) );
    break;
  case GFX3D_STATE_BLENDING:     
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_BLENDENABLE,val==GFX3D_ENABLE?TRUE:FALSE) );
    break;
  case GFX3D_STATE_LIGHTING:
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_LIGHTING,val==GFX3D_ENABLE?TRUE:FALSE) );
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_COLORVERTEX,val==GFX3D_ENABLE?TRUE:FALSE) );
    break;
  case GFX3D_STATE_SHADINGMODE:
    if ( val == GFX3D_FLAT ) 
      fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_FLAT) );
    else if ( val == GFX3D_SMOOTH ) 
      fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD) );
    else 
      fail_because("gfxd3d7: invalid shading model");
    break;
  case GFX3D_STATE_ALPHA_FUNC:
    if ( (denum = GfxFuncToD3D7(val) ) == D3DCMP_FORCE_DWORD ) fail_because("gfxd3d7: unknown alpha function");
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,denum) );
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_ALPHAREF,u32_t(ref*255)) );
    break;
  case GFX3D_STATE_DEPTH_FUNC:
    if ( (denum = GfxFuncToD3D7(val) ) == D3DCMP_FORCE_DWORD ) fail_because("gfxd3d7: unknown depth function");
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_ZFUNC,denum) );
    break;
  case GFX3D_STATE_BLEND_FUNC:
    {
      unsigned dst_func,src_func;
      if ( (src_func = GfxBlendToD3D7(val)) == D3DBLEND_FORCE_DWORD ) fail_because("gfxd3d7: unknown blend function");
      if ( (dst_func = GfxBlendToD3D7(unsigned(ref))) == D3DBLEND_FORCE_DWORD ) fail_because("gfxd3d7: unknown blend function");
      fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_SRCBLEND,src_func) );
      fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_DESTBLEND,dst_func) );
      break;
    }
    break;
  case GFX3D_STATE_ENVMODE:
    switch(val) {
    case GFX3D_TEXTURE_REPLACE:   
      fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1) );
      fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1) );
      break;
    case GFX3D_TEXTURE_MODULATE:  
      fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE) );
      fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE) );
      //fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE) );
      //fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1) );
      break;
    case GFX3D_TEXTURE_DECAL:
      fail_because("gfxd3d7: GFX3D_TEXTURE_DECAL is not implemented");
      break;
    case GFX3D_TEXTURE_BLEND: 
      fail_because("gfxd3d7: GFX3D_TEXTURE_BLEND is not implemented");
      break;
    default:
      fail_because("gfxd3d7: unknown texturing mode");
    }
    break;
  case GFX3D_STATE_WRAPST:
    if ( (denum = GfxWrapStToD3D7(val)) == D3DTADDRESS_FORCE_DWORD ) fail_because("gfxd3d7: invalid wrapping mode");
    if (ref != 0) {
      fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_ADDRESSU,denum) );
      if ( (denum = GfxWrapStToD3D7(unsigned(ref))) == D3DTADDRESS_FORCE_DWORD ) fail_because("gfxd3d7: invalid wrapping mode");
      fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_ADDRESSV,denum) );
    }else
      fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_ADDRESS,denum) );
    break;
  case GFX3D_STATE_MINMAGFILTER:
    if ( (denum = GfxFilterValToD3D7(val)) == D3DTFN_FORCE_DWORD ) fail_because("gfxd3d7: invalid texture filter"); 
    fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_MINFILTER,denum) );
    if ( ref != 0 )
      if ( (denum = GfxFilterValToD3D7(unsigned(ref))) == D3DTFN_FORCE_DWORD ) fail_because("gfxd3d7: invalid texture filter"); 
    fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_MAGFILTER,denum) );
    break;
//  case GFX3D_STATE_WRITE_BITS:
//    {
//      bool red    = (val & GFX3D_RED_BITS ) != 0 ;
//      bool green  = (val & GFX3D_GREEN_BITS ) != 0;
//      bool blue   = (val & GFX3D_BLUE_BITS ) != 0;
//      bool alpha   = (val & GFX3D_ALPHA_BITS ) != 0;
//      break;
//    }
  case GFX3D_STATE_WRITE_DEPTH:
    fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,val==GFX3D_ENABLE?TRUE:FALSE) );
    break;
  case GFX3D_STATE_CULLFACE_MODE:
    if ( val == GFX3D_NONE ) {
      fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE) );
    }else{
      switch( val ) {
      case GFX3D_BACK:  val = D3DCULL_CW; break;
      case GFX3D_FRONT: val = D3DCULL_CCW; break;
      //case GFX3D_FRONT_AND_BACK: val = GL_FRONT_AND_BACK ; break;
      default:
        fail_because ("gfxd3d7: state value must be GFX3D_FRONT/GFX3D_BACK/GFX3D_NONE");
      }
      fail_if_dxerr( idd_->SetRenderState(D3DRENDERSTATE_CULLMODE,val) );
    }
    break;
  case GFX3D_STATE_MIPMAPPING:
    fail_if_dxerr( idd_->SetTextureStageState(0,D3DTSS_MIPFILTER,val==GFX3D_ENABLE?D3DTFP_LINEAR:D3DTFP_NONE) );
    break;
  default:
      fail_because ("gfxd3d7: unknown state");
  }
  return SCE_OK;
}

} // namespace
