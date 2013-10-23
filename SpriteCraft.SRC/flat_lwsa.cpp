
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
#include "flat_e.h"
#include "flat_e0.h"
#include <limits.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

#include "flat_lwsa.inl"

SCERROR LwsArrayBasic::LwsResize(long count)
  {
    range_.Reserve(count+1);
    range_.Resize(count,filler_);
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsShow(long no,long count,bool on)
  {
    if ( no < 0 || no + count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));
    
    for ( int i = no; i < no+count; ++i ) 
      {
        if ( on ) 
          range_[i].flags |= ComplexSprite::Info::IS_VISIBLE_R;
        else 
          range_[i].flags &= ~ComplexSprite::Info::IS_VISIBLE_R;
      }

    NeedCheckBlendingState();
    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsSetRangeIndep(long no,long count,bool on)
  {
    if ( no < 0 || no + count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    for ( int i = no; i < no+count; ++i ) 
      {
        if ( on ) 
          range_[i].flags |= ComplexSprite::Info::IS_INDEPENDED;
        else 
          range_[i].flags &= ~ComplexSprite::Info::IS_INDEPENDED;
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsGetOne(long no,float* x,float* y,float* scale,float* angle,float* transparency,long* frno)
  {
    if ( no < 0 || no >= signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws number %d" %no);
  
    *transparency = float(1000-range_[no].alphaCf)/10.0f;
    *x = range_[no].x;  
    *y = range_[no].y;  
    *scale = range_[no].scale*100;
    *angle = range_[no].angle;  
    *frno = range_[no].fr_no;

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsSetOne(long no,float x,float y,float scale,float angle,float transparency,long frno)
  {
    if ( no < 0 || no >= signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws number %d" %no);
  
    range_[no].flags &= 
      ComplexSprite::Info::IS_UPDATED_R 
      | ~(ComplexSprite::Info::IS_BLENDED|ComplexSprite::Info::IS_ROTATED|ComplexSprite::Info::IS_SCALED);

    range_[no].alphaCf = 1000-teggo_min<long>(long(transparency*10.0f),1000);
    range_[no].x = x;  
    range_[no].y = y;  
    range_[no].scale = scale*0.01f;  
    range_[no].angle = angle;  
    range_[no].fr_no = frno;

    if (range_[no].alphaCf<999)               range_[no].flags |= ComplexSprite::Info::IS_BLENDED;
    if ( !EPSILON_EQUAL0(range_[no].angle) )  range_[no].flags |= ComplexSprite::Info::IS_ROTATED;
    if ( EPSILON_TEST(scale,1.0f) )           range_[no].flags |= ComplexSprite::Info::IS_SCALED;
  
    NeedCheckBlendingState();
    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsSetRangeXY(long no,long count,float x,float y) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].x = x;
        range_[i].y = y;
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsSetRangeAngle(long no,long count,float angle) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    u32_t flags = ComplexSprite::Info::IS_UPDATED_R;
    if ( !EPSILON_EQUAL0(angle) ) 
      flags |= ComplexSprite::Info::IS_ROTATED;

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].angle = angle; 
        range_[i].flags = (range_[i].flags & ~ComplexSprite::Info::IS_ROTATED) | flags;
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsAngleRangeDif(long no,long count,float angle) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].angle += angle; 
        range_[i].flags |= ComplexSprite::Info::IS_ROTATED;
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsSetRangeScale(long no,long count,float scale) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    scale *= 0.01f;
    u32_t flags = 0;
    if ( EPSILON_TEST(scale,1.0f) ) 
      flags |= ComplexSprite::Info::IS_SCALED;

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].scale = scale; 
        range_[i].flags = (range_[i].flags & ~ComplexSprite::Info::IS_SCALED) | flags;
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsSetRangeFrno(long no,long count,long frno) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].fr_no = abs(frno); /*% spr_->GetFrCount();*/
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsFrnoRangeDif(long no,long count,long frno) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].fr_no += abs(long(range_[i].fr_no+frno)); /*% spr_->GetFrCount();*/
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsSetRangeTransparency(long no,long count,float transparency) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    long alphaCf = 1000-teggo_min<long>(long(transparency*10.0f),1000);
    u32_t flags = 0;

    if (alphaCf<999) 
      flags |= ComplexSprite::Info::IS_BLENDED;
    
    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].alphaCf = alphaCf; 
        range_[i].flags = 
          ( range_[i].flags & ~ComplexSprite::Info::IS_BLENDED ) | flags;  
      }

    if ( flags & ComplexSprite::Info::IS_BLENDED ) 
      NeedCheckBlendingState();

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsGetOneXY(long no,float* x,float* y) 
  {
    if ( no < 0 || no >= signed(range_.Count()) )  
      fail_because(_S*"lwsarray: invalid particle number %d" %no);

    STRICT_REQUIRE ( x && y );  
    LWSA_MICROSPRITE* p; 
    
    if ( no < 0 ) 
      p = &filler_;  
    else 
      p = &range_[no];
    
    if ( x ) *x = range_[no].x;
    if ( y ) *y = range_[no].y;
    
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsGetOneAngle(long no,float* angle) 
  {
    if ( no < 0 || no >= signed(range_.Count()) ) 
      fail_because(_S*"lwsarray: invalid particle number %d" %no);

    STRICT_REQUIRE ( angle );  
    *angle = range_[no].angle;
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsGetOneScale(long no,float* scale) 
  {
    if ( no < 0 || no >= signed(range_.Count()) ) 
      fail_because(_S*"lwsarray: invalid particle number %d" %no);

    STRICT_REQUIRE ( scale );  
    *scale = range_[no].scale*100;
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsGetOneTransparency(long no,float* transparency) 
  {
    if ( no < 0 || no >= signed(range_.Count()) ) 
      fail_because(_S*"lwsarray: invalid particle number %d" %no);

    STRICT_REQUIRE ( transparency );  
    *transparency = float(1000-range_[no].alphaCf)/10.0f;
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsGetOneFrno(long no,long* frno) 
  {
    if ( no < 0 || no >= signed(range_.Count()) ) 
      fail_because(_S*"lwsarray: invalid particle number %d" %no);

    STRICT_REQUIRE ( frno );  
    *frno = range_[no].fr_no;
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsCopyRange(long dst,long src,long count) 
  {

    if ( dst < 0 || dst+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid dest lws range [%d..%d]" %dst %(dst+count));

    if ( src < 0 || src+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid source lws range [%d..%d]" %src %(src+count));

    memmove(&range_[dst],&range_[src],count*sizeof(LWSA_MICROSPRITE));
    NeedCheckBlendingState();
    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsScaleRangeDif(long no,long count,float dif) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].scale += dif;
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsBlendRangeDif(long no,long count,float dif) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    long difCf = long(dif*10);

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].alphaCf = teggo_max<long>(range_[i].alphaCf - difCf,0);
        bool is_blended = range_[i].alphaCf < 999;
        if ( is_blended ) range_[i].flags |= ComplexSprite::Info::IS_BLENDED;
        else              range_[i].flags &= ~ComplexSprite::Info::IS_BLENDED;
      }

    NeedCheckBlendingState();
    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

SCERROR LwsArrayBasic::LwsMoveRangeDif(long no,long count,float difX, float difY) 
  {
    if ( no < 0 || no+count > signed(range_.Count()) )
      fail_because(_S*"lwsarray: invalid lws range [%d..%d]" %no %(no+count));

    for ( int i = no; i < no+count; ++i ) 
      {
        range_[i].x += difX;
        range_[i].y += difY;
      }

    SPRITES::SetNeedRefresh();
    return SCE_OK;
  }

bool LwsArrayBasic::HasBlendState(bool blendstate) 
  {
    if ( need_check_blending_ ) 
      {
        blended_sprites_ = false;
        regular_sprites_ = false;
    
        LWSA_MICROSPRITE *i   = range_.Begin();
        LWSA_MICROSPRITE *iE  = range_.End();
        for ( ; i != iE && ( (blended_sprites_ && regular_sprites_) == false ) ; ++i )
          if ( (*i).flags & ComplexSprite::Info::IS_VISIBLE_R )
            if ( (*i).flags & ComplexSprite::Info::IS_BLENDED ) blended_sprites_ = true;
            else regular_sprites_ = true;

        need_check_blending_ = false;
      }

    if ( semitransparent_ ) return blendstate;
    if ( blendstate && !blended_sprites_ ) return false;
    if ( !blendstate && !regular_sprites_ ) return false;

    return true;
  }

bool LwsArrayBasic::ComplexSpriteReset(bool blendstate) 
  {
    iter_   = range_.Begin();
    iterE_  = range_.End();
  
    if ( !semitransparent_ )
      {
        const u32_t CHECKER = 
          ComplexSprite::Info::IS_VISIBLE_R 
          | (blendstate?ComplexSprite::Info::IS_BLENDED:0);
        
        while ( iter_ != iterE_ 
              && (iter_->flags 
                  & (ComplexSprite::Info::IS_VISIBLE_R|ComplexSprite::Info::IS_BLENDED)) 
                      != CHECKER ) 
          {
            ++iter_;
          }
        return iter_ != iterE_;
      }
    else
      return blendstate;
  }

bool LwsArrayBasic::ComplexSpriteNext(ComplexSprite::Info* info,bool blendstate) 
  {
    if ( !semitransparent_ )
      {
        const u32_t CHECKER = 
          ComplexSprite::Info::IS_VISIBLE_R 
          | (blendstate?ComplexSprite::Info::IS_BLENDED:0);

        while ( iter_ != iterE_ 
              && (iter_->flags 
                  & (ComplexSprite::Info::IS_VISIBLE_R|ComplexSprite::Info::IS_BLENDED)) 
                    != CHECKER)
          {
            ++iter_;
          }
      }
    else
      if ( !blendstate ) 
        return false;

    if ( iter_ != iterE_ ) 
      {
        LWSA_MICROSPRITE& p = (*iter_);
        info->x = p.x;
        info->y = p.y;
        info->alphaCf = p.alphaCf;
        info->xscale = info->yscale = p.scale;
        info->fr_no = p.fr_no;
        info->flags = p.flags;

        if ( is_centerd_ ) 
          info->flags |= ComplexSprite::Info::IS_CENTERED;

        if ( info->flags & ComplexSprite::Info::IS_ROTATED ) 
          {
            if ( info->flags & ComplexSprite::Info::IS_UPDATED_R ) 
              {
                p.sinv = sinf(p.angle);
                p.cosv = cosf(p.angle);
                p.flags &= ~ComplexSprite::Info::IS_UPDATED_R;
              }
            info->sinv = p.sinv;
            info->cosv = p.cosv;
          }

        ++iter_;
        return true;
      }
    return false;
  }
