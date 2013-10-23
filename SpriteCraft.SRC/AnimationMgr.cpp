
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
#include "AnimationMgr.h"
  
#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Managers$AnimatinMgr")
#endif

void AnimationMgr::AddAnimatedSprite(IWeakSprite* ias, long bFrno, long eFrno, float fps, IUnknown* listener, long cookie, bool looping) 
  {
    rcc_ptr<ISprite> sprite; ias->get_value(&+sprite);
    if ( +sprite && SUCCEEDED(sprite->put_frno(bFrno)) ) 
      {
        aslist_t::Position i = teggo::linear_find_if(aslist_.Begin(),aslist_.End(),as_t::Pred(ias));
        as_t& as = i ? *i : *aslist_.Insert(aslist_.Begin(),as_t());
        if ( !i ) 
          as.isprite_.reset_(Refe(ias));
        as.fps = fps;
        as.looping = looping;
        as.startFrno = bFrno;
        as.stopFrno  = eFrno;
        as.step = 0;
        as.start_frame_millis = GetMillis_OnFrame();
        as.delta = 1000.0f/fps;
        as.__interrupted = false;
        as.cookie = cookie;
        as.listener_.reset_(Refe(listener));
      }
  }

void AnimationMgr::StopSpriteAnimation(aslist_t::Position iter,bool interrupted) 
  {
    as_t ast = *iter;
    aslist_.Erase(iter);
    rcc_ptr<IOnStopAnimation> ionstop;
    rcc_ptr<ISprite> spr; ast.isprite_->get_value(&+spr);
    if ( 
      !ast.listener_ 
      || FAILED(ast.listener_->
                    QueryInterface(TEGGO_GUIDOF(IOnStopAnimation),(void**)&+ionstop)) ) 
      {
        rcc_ptr<IUnknown> subimpl;
        if ( +spr && SUCCEEDED(spr->get_subimpl(&+subimpl)) && +subimpl ) 
          subimpl->QueryInterface(TEGGO_GUIDOF(IOnStopAnimation),(void**)&+ionstop);
      }
    if ( +ionstop ) ionstop->OnStopAnimation(+spr,ast.cookie,interrupted);
  }

void AnimationMgr::StopSpriteAnimation(IWeakSprite* ias,bool interrupted) 
  {
    if ( aslist_t::Position i = aslist_.FindIf(as_t::Pred(ias)) )
      StopSpriteAnimation(i,interrupted);
  }

void AnimationMgr::Update() 
  {
    aslist_t::Position i = aslist_.Begin();
    while ( i ) 
      {
        as_t& ast = *i;
        long timeleft = GetMillis_OnFrame()-ast.start_frame_millis;
        rcc_ptr<ISprite> spr; ast.isprite_->get_value(&+spr);
        if ( !spr ) 
          goto interrupt_it;
        if ( ast.delta*(ast.step+1) <= timeleft ) 
          {
            ast.step = timeleft/ast.delta;
            long traklength = ast.stopFrno-ast.startFrno+1;
            if ( ast.step >= traklength && !ast.looping) 
              {
                spr->put_frno(ast.stopFrno);
                goto remove_it;
              }
            if ( FAILED(spr->put_frno((ast.step % traklength) + ast.startFrno)) ) 
              goto interrupt_it;
          }
        ++i;
        continue;
    interrupt_it:
        ast.__interrupted = true;
    remove_it:
        StopSpriteAnimation(i,ast.__interrupted);
      }
  }
