
#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "RBGFilter.h"
#include "Engine.h"
#include "Engine_DInput.h"


HRESULT SpriteCraftEngine::SetMousePos_(long x, long y)
  {
    HWND hwnd = 0;
    GfxControl *ctl = SCREEN::GetControl();
    if ( !ctl
         || ctl->GetWindowHandle(&hwnd,sizeof(hwnd)) != SCE_OK
         || hwnd == 0 )
      return GenSCWideError(L"SCREEN driver das not implements mouse control interface");
    POINT p = {x,y};
    ClientToScreen(hwnd,&p);
    SetCursorPos(p.x,p.y);
    return SCE_OK;
  }


HRESULT __stdcall SpriteCraftEngine::NextEvent(VARIANT_BOOL *bRet)
  {
    return NextEventEx(VARIANT_TRUE,bRet);
  }

HRESULT __stdcall SpriteCraftEngine::NextEventEx(VARIANT_BOOL processMsgQue,VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;

    if ( update_was_called_after_next_event_ )
      {
        mouse_DifX_  = 0;
        mouse_DifY_  = 0;
        update_was_called_after_next_event_ = false;
      }

    ProcessYield();

    event_ = SCREEN::NextEvent(processMsgQue != VARIANT_FALSE);

    if ( dinput_ )
      dinput_->RefreshInput();
    else
      ScRefreshInput();

  loop:

    if ( event_.type == EVENT::KEY_DOWN )
      {
        long key = event_.key & KEYVAL::EVK_KEYMASK;
        if ( key > 0 && key < 256 )
          {
            if ( keys_state_[key] )
              {
                event_.type = EVENT::NONE;
                event_.key = Key_NONE;
              }
            else
              keys_state_[key] = true;
          }
      }
    else if ( event_.type == EVENT::KEY_UP )
      {
        long key = event_.key & KEYVAL::EVK_KEYMASK;
        if ( key > 0 && key < 256 )
          keys_state_[key] = false;
      }

    if ( event_.type == EVENT::CONTEXT_RESET )
      {
        SCREEN::SetFillColor(fill_color_);

        if ( dinput_ )
          dinput_->ResetContext();
        else
          ScClearInput();
      }

    if ( event_.type == EVENT::POINTER_MOVE ||
         event_.type == EVENT::POINTER_DOWN ||
         event_.type == EVENT::POINTER_UP   ||
         event_.type == EVENT::POINTER_LEAVE  )
      {
        if ( mouse_mode_ == MouseDirectMode )
          {
            mouse_DifX_ = event_.point.X - mouse_X_;
            mouse_DifY_ = event_.point.Y - mouse_Y_;
            mouse_X_ = event_.point.X;
            mouse_Y_ = event_.point.Y;
            mouse_in_widow_ = (event_.type != EVENT::POINTER_LEAVE);
          }
        else
          {
            long screen_width2 = SCREEN::WndWidth()/2;
            long screen_height2 = SCREEN::WndHeight()/2;
            if ( event_.type == EVENT::POINTER_MOVE
              && event_.point.X == screen_width2 && event_.point.Y == screen_height2)
              {
                event_.type = EVENT::NONE;
                goto loop;
              }
            else
              {
                mouse_DifX_ = event_.point.X - screen_width2;
                mouse_DifY_ = event_.point.Y - screen_height2;
                mouse_X_ += mouse_DifX_;
                mouse_Y_ += mouse_DifY_;
                if ( FAILED(SetMousePos_(screen_width2,screen_height2)) )
                  return E_FAIL;
              }
          }

        mbleft_down_ = ((event_.key & KEYVAL::EVK_LBUTTONF) != 0 ) ? VARIANT_TRUE : VARIANT_FALSE;
        mbright_down_ = ((event_.key & KEYVAL::EVK_RBUTTONF) != 0 ) ? VARIANT_TRUE : VARIANT_FALSE;
        keys_state_[Key_LBUTTON] = mbleft_down_  != VARIANT_FALSE;
        keys_state_[Key_RBUTTON] = mbright_down_ != VARIANT_FALSE;
      }

    if ( event_.type == EVENT::POINTER_DOWN )
      if ( (event_.key & KEYVAL::EVK_KEYMASK ) == KEYVAL::EVK_LBUTTON )
        last_mbleft_down_ = GetMillis();
      else if ( (event_.key & KEYVAL::EVK_KEYMASK) == KEYVAL::EVK_RBUTTON )
        last_mbright_down_ = GetMillis();

    *bRet = event_.type!=EVENT::NONE?VARIANT_TRUE:VARIANT_FALSE;

    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtEvent(tagEventType *pVal)
  {
    _SYNCHRONIZE_;
    *pVal    = (tagEventType)event_.type;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtWindowBecomeActive(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal    = event_.type==EVENT::WINDOWACTIVE?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtWindowBecomeInactive(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal    = event_.type==EVENT::WINDOWINACTIVE?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::RefreshInput(long *state)
  {
    long st = 0;
    if ( dinput_ )
      {
        if ( dinput_->IsPressed(keyb_st_up_) )     st |= KeyStateUp;
        if ( dinput_->IsPressed(keyb_st_right_) )  st |= KeyStateRight;
        if ( dinput_->IsPressed(keyb_st_down_) )   st |= KeyStateDown;
        if ( dinput_->IsPressed(keyb_st_left_) )   st |= KeyStateLeft;
        if ( dinput_->IsPressed(keyb_st_fire_) )   st |= KeyStateFire;
        if ( dinput_->IsPressed(keyb_st_alter_) )  st |= KeyStateAlter;
        if ( dinput_->IsPressed(keyb_st_mode_) )   st |= KeyStateMode;
        if ( dinput_->IsPressed(keyb_st_esc_) )    st |= KeyStateESC;
      }
    else
      {
        if ( ScKeyIsPressed(keyb_st_up_) )     st |= KeyStateUp;
        if ( ScKeyIsPressed(keyb_st_right_) )  st |= KeyStateRight;
        if ( ScKeyIsPressed(keyb_st_down_) )   st |= KeyStateDown;
        if ( ScKeyIsPressed(keyb_st_left_) )   st |= KeyStateLeft;
        if ( ScKeyIsPressed(keyb_st_fire_) )   st |= KeyStateFire;
        if ( ScKeyIsPressed(keyb_st_alter_) )  st |= KeyStateAlter;
        if ( ScKeyIsPressed(keyb_st_mode_) )   st |= KeyStateMode;
        if ( ScKeyIsPressed(keyb_st_esc_) )    st |= KeyStateESC;
      }
    *state = st;
    return S_OK;
  }

void SpriteCraftEngine::TranslateKeyVal_(tagKeyVal *kv)
  {
    if ( *kv < 255 )
      return;

    switch ( *kv )
      {
      case Key_StAlter: *kv = keyb_st_alter_; break;
      case Key_StDown:  *kv = keyb_st_down_; break;
      case Key_StESC:   *kv = keyb_st_esc_; break;
      case Key_StFire:  *kv = keyb_st_fire_; break;
      case Key_StLeft:  *kv = keyb_st_left_; break;
      case Key_StMode:  *kv = keyb_st_mode_; break;
      case Key_StRight: *kv = keyb_st_right_; break;
      case Key_StUp:    *kv = keyb_st_up_; break;
      }
  }

HRESULT __stdcall SpriteCraftEngine::IsKeyPressed(tagKeyVal kvi,VARIANT_BOOL *pVal)
  {
    tagKeyVal kv = kvi;
    TranslateKeyVal_(&kv);
    if ( dinput_ )
      *pVal = dinput_->IsPressed(kv)?VARIANT_TRUE:VARIANT_FALSE;
    else
      *pVal = ScKeyIsPressed(kv)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::IsKeyReleasedNow(tagKeyVal kv,VARIANT_BOOL *pVal)
  {
    TranslateKeyVal_(&kv);
    if ( dinput_ )
      *pVal = dinput_->IsReleasedNow(kv)?VARIANT_TRUE:VARIANT_FALSE;
    else
      *pVal = ScKeyIsReleasedNow(kv)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::IsKeyPressedNow(tagKeyVal kv,VARIANT_BOOL *pVal)
  {
    TranslateKeyVal_(&kv);
    if ( dinput_ )
      *pVal = dinput_->IsPressedNow(kv)?VARIANT_TRUE:VARIANT_FALSE;
    else
      *pVal = ScKeyIsPressedNow(kv)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::MapKey(tagKeyVal kv,tagKeyVal st)
  {
    if ( st > Key_StESC || st < Key_StUp )
      return GenSCWideError(L"invalid st value",L":Map KEY -> ST:");

    switch ( st )
      {
      case Key_StAlter: keyb_st_alter_ = kv; break;
      case Key_StDown:  keyb_st_down_  = kv; break;
      case Key_StESC:   keyb_st_esc_   = kv; break;
      case Key_StFire:  keyb_st_fire_  = kv; break;
      case Key_StLeft:  keyb_st_left_  = kv; break;
      case Key_StMode:  keyb_st_mode_  = kv; break;
      case Key_StRight: keyb_st_right_ = kv; break;
      case Key_StUp:    keyb_st_up_    = kv; break;
      }

    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsESC(/*[out, retval]*/ VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal =
      (event_.key&KEYVAL::EVK_KEYMASK) == KEYVAL::EVK_ESC
      && event_.type == EVENT::KEY_DOWN
        ?VARIANT_TRUE
        :VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsKeyChar(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet = event_.type == EVENT::CHARKEY?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsKeyDown(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet = event_.type == EVENT::KEY_DOWN?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsKeyUp(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet = event_.type == EVENT::KEY_UP?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsQuit(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet = event_.type == EVENT::QUIT?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsMouseUp(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet = event_.type == EVENT::POINTER_UP?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsMouseDown(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet = event_.type == EVENT::POINTER_DOWN?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsMouseMove(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet = event_.type == EVENT::POINTER_MOVE?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsMouse(/*[out,retval]*/ VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    *bRet =
      event_.type == EVENT::POINTER_MOVE ||
      event_.type == EVENT::POINTER_UP   ||
      event_.type == EVENT::POINTER_DOWN ||
      event_.type == EVENT::POINTER_LEAVE
      ?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtKey(tagKeyVal *key)
  {
    *key = tagKeyVal(event_.key & Key_KEYMASK);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtKeyEx(long *keyval)
  {
    *keyval = event_.key;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtKeyChar_(wchar_t *charval)
  {
    *charval = event_.ckey;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtKeyChar(BSTR *charval)
  {
    *charval = SysAllocString(L"\x01");
    **charval = event_.ckey;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseX(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mouse_X_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_mouseX(long newVal)
  {
    _SYNCHRONIZE_;
    if ( mouse_mode_ == MouseDirectMode )
      if ( FAILED(SetMousePos_(newVal,mouse_Y_)) )
        return SCE_FAIL;
    mouse_X_ = newVal;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseY(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mouse_Y_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_mouseY(long newVal)
  {
    _SYNCHRONIZE_;
    if ( mouse_mode_ == MouseDirectMode )
      if ( FAILED(SetMousePos_(mouse_X_,newVal)) )
        return SCE_FAIL;
    mouse_Y_ = newVal;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseLDown(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mbleft_down_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseRDown(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mbright_down_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::MapEvent(tagEventType etype, tagKeyVal keyval, long uevent)
  {
    _SYNCHRONIZE_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::UnMapEvent(tagEventType etype, tagKeyVal keyval)
  {
    _SYNCHRONIZE_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::UnMapAllEvents()
  {
    _SYNCHRONIZE_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseMode(enum tagMouseMode *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mouse_mode_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_mouseMode(enum tagMouseMode newVal)
  {
    _SYNCHRONIZE_;
    mouse_mode_ = newVal;
    if ( mouse_mode_ == MouseIntervalMode )
      {
        long screen_width2 = SCREEN::WndWidth()/2;
        long screen_height2 = SCREEN::WndHeight()/2;
        mouse_X_ = screen_width2;
        mouse_Y_ = screen_height2;
        SetMousePos_(mouse_X_,mouse_Y_);
      }
    mouse_DifX_ = 0;
    mouse_DifY_ = 0;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseDifX(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mouse_DifX_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseDifY(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mouse_DifY_;
    return S_OK;
  }

void SpriteCraftEngine::ScRefreshInput()
  {
    _SYNCHRONIZE_;
    memcpy(keys_state_last_,keys_state_,sizeof(keys_state_last_));
  //  keys_state_[Key_CTRL]    = 0 != (GetKeyState(VK_CONTROL) & 0x08000);
  //  keys_state_[Key_ALT]     = 0 != (GetKeyState(VK_MENU)    & 0x08000);
  //  keys_state_[Key_SHIFT]   = 0 != (GetKeyState(VK_SHIFT)   & 0x08000);
    keys_state_[Key_RCTRL]   = 0 != (GetKeyState(VK_RCONTROL) & 0x08000);
    keys_state_[Key_RALT]    = 0 != (GetKeyState(VK_RMENU)    & 0x08000);
    keys_state_[Key_RSHIFT]  = 0 != (GetKeyState(VK_RSHIFT)   & 0x08000);
    keys_state_[Key_LCTRL]   = 0 != (GetKeyState(VK_LCONTROL) & 0x08000);
    keys_state_[Key_LALT]    = 0 != (GetKeyState(VK_LMENU)    & 0x08000);
    keys_state_[Key_LSHIFT]  = 0 != (GetKeyState(VK_LSHIFT)   & 0x08000);
    keys_state_[Key_CTRL]    = keys_state_[Key_RCTRL] || keys_state_[Key_LCTRL];
    keys_state_[Key_ALT]     = keys_state_[Key_RALT] || keys_state_[Key_LALT];
    keys_state_[Key_SHIFT]   = keys_state_[Key_RSHIFT] || keys_state_[Key_LSHIFT];
  }

void SpriteCraftEngine::ScClearInput()
  {
    _SYNCHRONIZE_;
    memset(keys_state_last_,0,sizeof(keys_state_last_));
    memset(keys_state_,0,sizeof(keys_state_));
  }

bool SpriteCraftEngine::ScKeyIsPressed(tagKeyVal kv)
  {
    if ( !kv || kv > 255 ) return false;
    return keys_state_[kv];
  }

bool SpriteCraftEngine::ScKeyIsPressedNow(tagKeyVal kv)
  {
    if ( !kv || kv > 255 ) return false;
    return keys_state_[kv]  && !keys_state_last_[kv];
  }

bool SpriteCraftEngine::ScKeyIsReleasedNow(tagKeyVal kv)
  {
    if ( !kv || kv > 255 ) return false;
    return !keys_state_[kv]  && keys_state_last_[kv];
  }
