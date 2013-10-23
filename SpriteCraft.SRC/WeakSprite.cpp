
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
#pragma code_seg(".text$AAA$Sprites$Weak")
#endif

WeakSprite::WeakSprite(SpriteHelper* s) : spr_(s),collide_unlink_(0) 
  {
  }

WeakSprite::~WeakSprite() 
  {
  }

HRESULT __stdcall WeakSprite::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(IWeakSprite),riid)?S_OK:S_FALSE;
  }


void WeakSprite::UnlinkCollide() 
  { 
    if ( collide_unlink_ ) 
      collide_unlink_->Unlink(); 
  }

void WeakSprite::ForgetSprite()  
  { 
    UnlinkCollide(); 
    spr_ = 0; 
  }

bool WeakSprite::OnUpdateListener(u32_t tickdelta) 
  { 
    return !spr_ ? false : spr_->Update_(tickdelta); 
  }

Sprite* WeakSprite::ICollidableObject_GetSprite() 
  { 
    return !spr_ ? 0 : spr_->GetSprite_(); 
  }

void WeakSprite::ICollidableObject_OnCollide(Sprite* spr) 
  { 
    if ( spr_  && !spr_->OnCollide(spr) ) 
      UnlinkCollide(); 
  }

void WeakSprite::ICollidableObject_Link(ICollideUnlink* ulink) 
  { 
    DBG_REQUIRE ( !ulink || !collide_unlink_ ); 
    collide_unlink_ = ulink; 
  }

ISprite* WeakSprite::ICollidableObject_GetISprite() 
  { 
    return spr_; 
  }

bool WeakSprite::Alive() 
  { 
    return !spr_ ? false : spr_->GetSprite_() != 0; 
  }

ISprite* WeakSprite::GetSprite() 
  { 
    return spr_; 
  }

IUnknown* WeakSprite::GetSubImpl() 
  { 
    return spr_->GetSubImpl_(); 
  }

HRESULT __stdcall WeakSprite::get_value(ISprite** pI) 
  { 
    _SYNCHRONIZE_;
    *pI = Refe(spr_);
    return S_OK;
  }

bool WeakSprite::ContainsPoint(float* x,float* y,long* layer,long* sublayer,u32_t* ident)
  {
    if ( !spr_ ) 
      return false;
    
    Sprite* sprite = spr_->GetSprite_();

    if ( sprite && sprite->ContainsPoint(x,y) ) 
      {
        sprite->GetFullIdent(layer,sublayer,ident);
        return true;
      }

    return false;
  }

bool WeakSprite::IsVisible()
  {
    if ( !spr_ ) 
      return false;
  
    if ( Sprite* spr = spr_->GetSprite_() )
      return spr->IsVisible();
    else 
      return false;
  }


