
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

#include "flat_e3.h"
#include "SpriteHelper.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Sprites$Lwsa")
#endif

HRESULT __stdcall SpriteLwsArray::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(ILwsArray),riid)?S_OK:S_FALSE;
  }

SpriteLwsArray* SpriteLwsArray::AsLwsArray() 
  { 
    return this; 
  }

void SpriteLwsArray::SetSpriteWH(float w,float h) 
  { 
    sprite_width_ = w; 
    sprite_height_ = h; 
  }

void SpriteLwsArray::SetSemitransparent( bool val ) 
  { 
    LwsArrayBasic::Semitransparent(val); 
  }

SpriteLwsArray::SpriteLwsArray()
  {
  }

SpriteLwsArray::~SpriteLwsArray() 
  {}

#define failcheck(x) if ( SCE_OK != (x) ) return GenSCWideError(0); else;

HRESULT __stdcall SpriteLwsArray::get_count(/*[out, retval]*/ long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = LwsArrayBasic::LwsCount();
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::put_count(/*[in]*/ long newVal)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsResize(newVal));
    return S_OK;
  }


HRESULT __stdcall SpriteLwsArray::CopyRange(/*[in]*/ long dst,/*[in]*/ long src,/*[in]*/ long count)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsCopyRange(dst,src,count));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::ShowRange(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ VARIANT_BOOL on)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsShow(no,count,on != VARIANT_FALSE));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::SetRangeXY(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float x, /*[in]*/ float y)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsSetRangeXY(no,count,x,y));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::SetRangeAngle(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float angle)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsSetRangeAngle(no,count,angle));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::SetRangeScale(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float scale)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsSetRangeScale(no,count,scale));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::SetRangeTransparency(/*[in]*/ long no, /*[in]*/ long count,/*[in]*/ float transparency)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsSetRangeTransparency(no,count,transparency));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::SetRangeFrno(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ long frno)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsSetRangeFrno(no,count,frno));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::GetOneX(/*[in]*/ long no,/*[out,retval]*/ float* x)
  {
    _SYNCHRONIZE_;
    float y;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsGetOneXY(no,x,&y));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::GetOneY(/*[in]*/ long no,/*[out,retval]*/ float* y)
  {
    _SYNCHRONIZE_;
    float x;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsGetOneXY(no,&x,y));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::GetOneAngle(/*[in]*/ long no,/*[out,retval]*/ float* angle)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsGetOneAngle(no,angle));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::GetOneScale(/*[in]*/ long no,/*[out,retval]*/ float* scale)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsGetOneScale(no,scale));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::GetOneTransparency(/*[in]*/ long no,/*[out,retval]*/ float* transparency)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsGetOneTransparency(no,transparency));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::GetOneFrno(/*[in]*/ long no,/*[out,retval]*/ long* frno)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsGetOneFrno(no,frno));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::SetOne(
      /*[in]*/ long no,
      /*[in]*/ float x,
      /*[in]*/ float y,
      /*[in]*/ float scale,
      /*[in]*/ float angle,
      /*[in]*/ float transparency,
      /*[in]*/ long frno)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsSetOne(no,x,y,scale,angle,transparency,frno));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::DiffRangeScale(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float diff)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsScaleRangeDif(no,count,diff));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::DiffRangeXY(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float xdiff,/*[in]*/ float ydiff)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsMoveRangeDif(no,count,xdiff,ydiff));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::DiffRangeTransparency(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float diff)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsBlendRangeDif(no,count,diff));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::DiffRangeAngle(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float diff)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsAngleRangeDif(no,count,diff));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::DiffRangeFrno(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ long diff)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    failcheck(LwsArrayBasic::LwsFrnoRangeDif(no,count,diff));
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::GetRange(/*[out,size_is(count)]*/ struct MICROSPRITE* s,/*[in]*/ long no,/*[in,defaultvalue(1)]*/ long count)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    for ( int i = 0; i < count; ++i ) 
      {
        failcheck(LwsArrayBasic::LwsGetOne(
                    i+no,&s[i].x,&s[i].y,
                    &s[i].scale,&s[i].angle,
                    &s[i].transparency,&s[i].frno));
      }
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::SetRange(/*[in,size_is(count)]*/ struct MICROSPRITE* s,/*[in]*/ long no,/*[in,defaultvalue(1)]*/ long count)
  {
    _SYNCHRONIZE_;
    ResetErrorLog();
    for ( int i = 0; i < count; ++i ) 
      {
        failcheck(LwsArrayBasic::LwsSetOne(
                    no+i,s[i].x,s[i].y,
                    s[i].scale,s[i].angle,
                    s[i].transparency,s[i].frno));
      }
    return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::get_isCentered(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = is_centerd_ ? VARIANT_TRUE : VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::put_isCentered(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    is_centerd_ = newVal!=VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall SpriteLwsArray::Order(tagLwsaOrder order,tagHotspot hotspot)
  {
    _SYNCHRONIZE_;
    long count = LwsArrayBasic::LwsCount();
    bool topbottom = order == LwsaOrderTopBottom;
    float swidth  = sprite_width_;
    float sheight = sprite_height_;
    float xcount  = float(topbottom?1:count);
    float ycount  = float(topbottom?count:1);
    float dx = topbottom?0:swidth;
    float dy = topbottom?sheight:0;
    float xoffs = is_centerd_?swidth*0.5f:0;
    float yoffs = is_centerd_?sheight*0.5f:0;
    switch ( hotspot ) 
      {
      case HotspotLeftTop:                                                                   break;
      case HotspotLeftCenter:                                 yoffs += -sheight*ycount*0.5f; break;
      case HotspotLeftBottom:                                 yoffs += -sheight*ycount;      break;
      case HotspotCenterTop:    xoffs += -swidth*xcount*0.5f;                                break;
      case HotspotCenterBottom: xoffs += -swidth*xcount*0.5f; yoffs += -sheight*ycount;      break;  
      case HotspotRightTop:     xoffs += -swidth*xcount;                                     break;
      case HotspotRightCenter:  xoffs += -swidth*xcount;      yoffs += -sheight*ycount*0.5f; break;
      case HotspotRightBottom:  xoffs += -swidth*xcount;      yoffs += -sheight*ycount;      break;
      case HotspotCenter:       xoffs += -swidth*count*0.5f;  yoffs += -sheight*ycount*0.5f; break;
      }

    for ( int i = 0; i < count; ++i, xoffs += dx, yoffs += dy  ) 
      LwsArrayBasic::LwsSetRangeXY(i,1,xoffs,yoffs);
  
    LwsArrayBasic::LwsShow(0,count,true);

    return S_OK;
  }

