
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
#include "Engine_DInput.h"

#define DIRECTINPUT_VERSION 0x0300
#include <dinput.h>

#pragma comment(lib,"dinput.lib")

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Engine")
#endif

#define K(x)  {Key_##x,DIK_##x}
static struct{BYTE idx;BYTE val;} vkdi_config[] =
  {
    K(F1),K(F2),K(F3),K(F4),K(F5),K(F6),K(F7),K(F8),K(F9),K(F10),K(F11),K(F12),
    {Key_ENTER,DIK_RETURN},
    {Key_ESC,DIK_ESCAPE},
    {'`',DIK_GRAVE},
    {'1',DIK_1},{'2',DIK_2},{'3',DIK_3},{'4',DIK_4},{'5',DIK_5},
      {'6',DIK_6},{'7',DIK_7},{'8',DIK_8},{'9',DIK_9},{'0',DIK_0},
    {'Q',DIK_Q},{'W',DIK_W},{'E',DIK_E},{'R',DIK_R},{'T',DIK_T},
      {'Y',DIK_Y},{'U',DIK_U},{'I',DIK_I},{'O',DIK_O},{'P',DIK_P},
    {'A',DIK_A},{'S',DIK_S},{'D',DIK_D},{'F',DIK_F},{'G',DIK_G},
      {'H',DIK_H},{'J',DIK_J},{'K',DIK_K},{'L',DIK_L},
    {'Z',DIK_Z},{'X',DIK_X},{'C',DIK_C},{'V',DIK_V},{'B',DIK_B},
      {'N',DIK_N},{'M',DIK_M},//{',',DIK_},{'.',DIK_},{'/',DIK_},
    {Key_TAB,DIK_TAB},
    {Key_BACKSPACE,DIK_BACK},
    {Key_CTRL,DIK_LCONTROL},
    {Key_RCTRL,DIK_RCONTROL},
    {Key_SHIFT,DIK_LSHIFT},
    {Key_RSHIFT,DIK_RSHIFT},
    {Key_ALT,DIK_LWIN},
    {Key_RALT,DIK_RWIN},
    {Key_LEFT,DIK_LEFT},
    {Key_RIGHT,DIK_RIGHT},
    {Key_UP,DIK_UP},
    {Key_DOWN,DIK_DOWN},
    {Key_SPACE,DIK_SPACE},
  };
#undef K

static BYTE Key_to_DI[256];
static void vktodiInit()
  {
    memset(Key_to_DI,0,sizeof(Key_to_DI));
    for ( unsigned i = 0 ; i < sizeof(vkdi_config)/2; ++i )
      Key_to_DI[vkdi_config[i].idx] = vkdi_config[i].val;
  }

struct Engine_DInput_Impl
  {
  //  HWND hwnd;
    bool initialized;
    LPDIRECTINPUT lpDI;
    LPDIRECTINPUTDEVICE lpDIDevice_keyb;
    //LPDIRECTINPUTDEVICE lpDIDevice_mouse;

    //int mlX;
    //int mlY;
    byte_t last_keyb_state[256];
    byte_t curr_keyb_state[256];
    byte_t last_mouse_state[3];
    byte_t curr_mouse_state[3];
    //DIMOUSESTATE last_mouse_state;
    //DIMOUSESTATE curr_mouse_state;
  };

static inline
  bool KEYDOWN(BYTE* bf, unsigned key)
    {
      REQUIRE(key<256);
      return (bf[Key_to_DI[key]] & 0x80)!=0;
    }


Engine_DInput::Engine_DInput()
  {
    impl_ = new Engine_DInput_Impl;
    memset(impl_,0,sizeof(Engine_DInput_Impl));
    vktodiInit();
    ResetContext(true);
  }

Engine_DInput::~Engine_DInput()
  {
    Terminate();
    delete impl_;
  }

void Engine_DInput::RefreshInput()
  {
    if ( !impl_->initialized ) return;
    HRESULT hr;
    memcpy(impl_->last_keyb_state,impl_->curr_keyb_state,sizeof(impl_->curr_keyb_state));

  refetch_keyb:
    hr = impl_->lpDIDevice_keyb->GetDeviceState(sizeof(impl_->curr_keyb_state),(LPVOID)&impl_->curr_keyb_state);
    if ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
      {
        Xdbg( Ldbg << ":Engine_DInput:keyboard input was lost, reAcuire" );
        hr = 0;
        if ( impl_->lpDIDevice_keyb->Acquire() != DI_OK )
          {
            Sleep(100);
            return;
          }
        goto refetch_keyb;
      }
    else
      return;

    memcpy(&impl_->last_mouse_state,&impl_->curr_mouse_state,sizeof(impl_->curr_mouse_state));
    impl_->curr_mouse_state[0] = (byte_t)GetAsyncKeyState(VK_LBUTTON);
    impl_->curr_mouse_state[1] = (byte_t)GetAsyncKeyState(VK_RBUTTON);
  }

bool Engine_DInput::IsPressed(tagKeyVal vk)
  {
    if ( !impl_->initialized )
      return false;

    switch ( vk )
      {
      case Key_LBUTTON:
        return ( impl_->curr_mouse_state[0]&0x80)!=0;
      case Key_RBUTTON:
        return ( impl_->curr_mouse_state[1]&0x80)!=0;
      default:
        return KEYDOWN(impl_->curr_keyb_state,vk);
      }
  }

bool Engine_DInput::IsPressedNow(tagKeyVal vk)
  {
    if ( !impl_->initialized )
      return false;
    switch ( vk )
      {
      case Key_LBUTTON:
        return ( impl_->curr_mouse_state[0]&0x80)!=0 && ( impl_->last_mouse_state[0]&0x80)==0;
      case Key_RBUTTON:
        return ( impl_->curr_mouse_state[1]&0x80)!=0 && ( impl_->last_mouse_state[1]&0x80)==0;
      default:
        return KEYDOWN(impl_->curr_keyb_state,vk) && !KEYDOWN(impl_->last_keyb_state,vk);
      }
  }

bool Engine_DInput::IsReleasedNow(tagKeyVal vk)
  {
    if ( !impl_->initialized )
      return false;

    switch ( vk )
      {
      case Key_LBUTTON:
        return ( impl_->curr_mouse_state[0]&0x80)==0 && ( impl_->last_mouse_state[0]&0x80)!=0;
      case Key_RBUTTON:
        return ( impl_->curr_mouse_state[1]&0x80)==0 && ( impl_->last_mouse_state[1]&0x80)!=0;
      default:
        return !KEYDOWN(impl_->curr_keyb_state,vk) && KEYDOWN(impl_->last_keyb_state,vk);
      }
  }

void Engine_DInput::Terminate()
  {
    if (!impl_)
      return;
    impl_->initialized = false;

    if ( impl_->lpDIDevice_keyb )
      impl_->lpDIDevice_keyb->Unacquire();
    if ( impl_->lpDIDevice_keyb )
      impl_->lpDIDevice_keyb->Release();
    if ( impl_->lpDI )
      impl_->lpDI->Release();
  }


static bool dxerr_impl(HRESULT hr,const char* file,int lineno)
  {
    if ( FAILED(hr) )
      {
        Lerr << _S*":Engine_DInput: failed (0x%08x) at %s:%d" %hr %file %lineno;
        __debug_break__();
      }
    return FAILED(hr);
  }
#define dxerr(ex) dxerr_impl((ex),__FILE__,__LINE__)

void Engine_DInput::ResetContext(bool do_init)
  {
    HWND hwnd = 0;
    SCREEN::GetWindowHandle(&hwnd,sizeof(hwnd));
    bool ok = false;
    //impl_->initialized = false;
    if ( do_init )
      {
        Lout << _S*":Engine_DInput: DINPUT dinput.dll [%?]" %teggo::GetModuleVersion(L"dinput.dll");
        if (
          dxerr ( DirectInputCreate(GetModuleHandle(0),DIRECTINPUT_VERSION, &impl_->lpDI, 0) ) ||
          dxerr ( impl_->lpDI->CreateDevice(GUID_SysKeyboard, &impl_->lpDIDevice_keyb, 0) ) ||
          dxerr ( impl_->lpDIDevice_keyb->SetDataFormat(&c_dfDIKeyboard) ) ||
          dxerr ( impl_->lpDIDevice_keyb->SetCooperativeLevel(hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE) )
          )
          {
            Terminate();
            return;
          }
      }
    else
      {
        impl_->lpDIDevice_keyb->Unacquire();
        if (
          dxerr ( impl_->lpDIDevice_keyb->SetCooperativeLevel(
                    hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE) )
          )
          {
            Terminate();
            return;
          }
      }

    impl_->initialized = true;
  }

