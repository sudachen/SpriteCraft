
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
#include "MListenMgr.h" 
#include "Engine.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Managers$MListenMgr")
#endif

struct MListenMgr_Identifier
  {
    long layer;
    long sublayer;
    u32_t ident;
    friend bool operator < (const MListenMgr_Identifier& a, const MListenMgr_Identifier& b)
      {
        return a.layer < b.layer ||
          ( a.layer == b.layer && 
            ( a.sublayer < b.sublayer || 
              ( a.sublayer == b.sublayer && a.ident < b.ident ) ) );
      }
  };

MListenMgr::MListenMgr() 
  {
    nearLayer_ = LONG_MIN;
    farLayer_  = LONG_MAX;
  }

MListenMgr::~MListenMgr()
  {
  }

void MListenMgr::Add(IWeakSpriteEx* spr) 
  {
    SprLst::Position i = slist_.FindIf(s_t::Pred(spr));
    s_t& s = i ? *i : *slist_.Insert(slist_.Begin());
    if ( !i ) s.isprite.reset_(Refe(spr));
  }

void MListenMgr::Remove(IWeakSpriteEx* spr) 
  {
    if ( SprLst::Position i = slist_.FindIf(s_t::Pred(spr)) )
      slist_.Erase( i );
  }

void MListenMgr::GrabMouse(IWeakSpriteEx* spr)
  {
    locked_.reset_(Refe(spr));
  }

void MListenMgr::SetMouseNotifierRange(long nearLayer,long farLayer) 
  {
    nearLayer_ = nearLayer;
    farLayer_  = farLayer;
  }

void MListenMgr::Update() 
  {
    const float mouse_x = Engine_GetMouseX();
    const float mouse_y = Engine_GetMouseY();
    float min_x = mouse_x;
    float min_y = mouse_y;
    if ( +locked_ && locked_->Alive() ) 
      {
        if ( +selected_ != +locked_ ){
          if ( +selected_ && selected_->Alive() ) 
            Notify_MouseOver(+selected_,false);
          selected_ = locked_;
        }
        ISprite* sprite = locked_->GetSprite();
        if ( sprite ) sprite->ScreenToSprite(&min_x,&min_y);
      } 
    else 
      {
        if ( slist_.Empty() ) 
          return;
        MListenMgr_Identifier min_ident;
        rcc_ptr<IWeakSpriteEx> min_sprite;
        SprLst::Position i  = slist_.Begin();
        while ( i ) 
          {
            IWeakSpriteEx* sprite = +(*i).isprite;
            if ( sprite->Alive() ) 
              {
                if ( sprite->IsVisible() ) 
                  {
                    float x = mouse_x;
                    float y = mouse_y;
                    MListenMgr_Identifier ident;
                    if ( sprite->ContainsPoint(&x,&y,&ident.layer,&ident.sublayer,&ident.ident) ) 
                      if ( ident.layer >= nearLayer_ && ident.layer <= farLayer_ ) 
                        if ( !min_sprite || ident < min_ident ) 
                          {
                            min_sprite.reset_(Refe(sprite));
                            min_x = mouse_x;
                            min_y = mouse_y;
                            min_ident = ident;
                          }
                  }
                ++i;
              }
            else
              slist_.Erase(i);
          }

        if ( +selected_ && !selected_->Alive() ) 
          Unrefe(selected_);

        if ( +min_sprite != +selected_ ) 
          {
            Notify_MouseOver(+selected_,false);
            Notify_MouseOver(+min_sprite,true);
            selected_.reset_(min_sprite.forget_());
          }
      }
    
    if ( +selected_ ) 
      {
        EVENT event;
        Engine_EvtGet(&event);
        long keyval = event.key&KEYVAL::EVK_KEYMASK;
        
        if ( event.type == EVENT::POINTER_MOVE ) 
          Notify_MouseMove(min_x,min_y);
        
        if ( event.type == EVENT::POINTER_DOWN ) 
          {
            switch ( keyval ) 
              {
              case KEYVAL::EVK_LBUTTON: 
                Notify_MouseDown(min_x,min_y,1); 
                break;
              case KEYVAL::EVK_RBUTTON: 
                Notify_MouseDown(min_x,min_y,2); 
                break;
              }
          }
        
        if ( event.type == EVENT::POINTER_UP ) 
          {
            switch ( keyval ) 
              {
              case KEYVAL::EVK_LBUTTON: 
                Notify_MouseUp(min_x,min_y,1); 
                break;
              case KEYVAL::EVK_RBUTTON: 
                Notify_MouseUp(min_x,min_y,2); 
                break;
              }
          }
        
        if ( Engine_EvtIsRClick() ) 
          Notify_MouseClick(min_x,min_y,2);
        
        if ( Engine_EvtIsLClick() ) 
          Notify_MouseClick(min_x,min_y,1);
      }
  }

void MListenMgr::Notify_MouseOver(IWeakSpriteEx* iex,bool inArea) 
  {
    if ( !iex ) 
      return;
    
    rcc_ptr<IUnknown> iunk ( Refe(iex->GetSubImpl()) );

    if ( +iunk ) 
      {
        rcc_ptr<IOnMouseOver> iover;
        if ( SUCCEEDED(iunk->QueryInterface(TEGGO_GUIDOF(IOnMouseOver),(void**)&+iover)) ) 
            iover->OnMouseOver(iex->GetSprite(),inArea?VARIANT_TRUE:VARIANT_FALSE);
      }
  }

void MListenMgr::Notify_MouseMove(float x,float y) 
  {
    if ( !selected_ ) 
      return;
    
    rcc_ptr<IUnknown> iunk ( Refe(selected_->GetSubImpl()) );
    
    if ( +iunk ) 
      {
        rcc_ptr<IOnMouseMove> iom;
        if ( SUCCEEDED(iunk->QueryInterface(TEGGO_GUIDOF(IOnMouseMove),(void**)&+iom)) ) 
          iom->OnMouseMove(selected_->GetSprite(),x,y);
      }
  }

void MListenMgr::Notify_MouseDown(float x,float y,long button) 
  {
    if ( !selected_ ) 
      return;

    rcc_ptr<IUnknown> iunk ( Refe(selected_->GetSubImpl()) );

    if ( +iunk ) 
      {
        rcc_ptr<IOnMouseDown> iom;
        if ( SUCCEEDED(iunk->QueryInterface(TEGGO_GUIDOF(IOnMouseDown),(void**)&+iom)) ) 
          iom->OnMouseDown(selected_->GetSprite(),x,y,button);
      }
  }

void MListenMgr::Notify_MouseUp(float x,float y,long button) 
  {
    if ( !selected_ ) 
      return;

    rcc_ptr<IUnknown> iunk ( Refe(selected_->GetSubImpl()) );

    if ( +iunk ) 
      {
        rcc_ptr<IOnMouseUp> iom;
        if ( SUCCEEDED(iunk->QueryInterface(TEGGO_GUIDOF(IOnMouseUp),(void**)&+iom)) ) 
          iom->OnMouseUp(selected_->GetSprite(),x,y,button);
      }
  }

void MListenMgr::Notify_MouseClick(float x,float y,long button) 
  {
    if ( !selected_ ) 
      return;

    rcc_ptr<IUnknown> iunk ( Refe(selected_->GetSubImpl()) );

    if ( +iunk ) 
      {
        rcc_ptr<IOnMouseClick> iom;
        if ( SUCCEEDED(iunk->QueryInterface(TEGGO_GUIDOF(IOnMouseClick),(void**)&+iom)) ) 
          iom->OnMouseClick(selected_->GetSprite(),x,y,button);
      }
  }

