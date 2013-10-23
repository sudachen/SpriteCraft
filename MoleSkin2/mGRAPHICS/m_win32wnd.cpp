
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
#include "m_input.h"
#include "m_win32wnd.h"

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32

#include <zmouse.h>

#define mskin_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define mskin_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

bool Win32ScreenDevice::IsRubbish() const { return isRubbish_;}
void Win32ScreenDevice::MakeRubbish() { isRubbish_ = true;}
bool Win32ScreenDevice::HandleSpecific(const MSG&,EVENT&) { return false;}

LRESULT (CALLBACK *Win32ScreenDevice::defproc_)(HWND,UINT,WPARAM,LPARAM);

static Win32ScreenDevice* g_win32ScreenDev = 0;
static long wheel = 0;
static bool mouse_locked = false;

LRESULT CALLBACK Win32ScreenDevice::WindowProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  STRICT_REQUIRE ( g_win32ScreenDev != 0 );
  switch ( Msg )
  {
  case XEVENT:
    break;
  case WM_SIZE:
    if ( g_win32ScreenDev->attached_ ) g_win32ScreenDev->OnResize();
    break;
  case WM_CREATE:
    mouse_locked = false;
    ReleaseCapture();
    break;
  case WM_ACTIVATE:
    if ( LOWORD(wParam) != WA_INACTIVE )
      g_win32ScreenDev->isActive_ = true;
    else
      g_win32ScreenDev->isActive_ = false;
    g_win32ScreenDev->EnqueEvent(EVENT::MakeWindowActive(g_win32ScreenDev->isActive_));
    break;
  case WM_CLOSE:
    if (!g_win32ScreenDev->attached_)
    {
      PostMessage(hWnd,XCLOSE,0,0);
      return 0;
    }
    break;
  case WM_SETCURSOR:
    if(HTCLIENT == LOWORD(lParam))
    {
      if ( g_win32ScreenDev->enableCursor_ )
        SetCursor(g_win32ScreenDev->cursor_);
      else
        SetCursor(0);
      return TRUE;
    }
    break;
  case WM_PAINT:
    g_win32ScreenDev->MakeRubbish();
    if ( g_win32ScreenDev->attached_ )
      g_win32ScreenDev->OnPaint();
    break;
  case WM_GETDLGCODE:
    {
      if ( lParam && ((MSG*)lParam)->message == WM_KEYDOWN )
      {
        EVENT e = g_win32ScreenDev->ProcessMessage(*(MSG*)lParam);
        if ( e.type != EVENT::NONE )
          g_win32ScreenDev->EnqueEvent(e);
        return DLGC_WANTALLKEYS;
      }
      else
        return DLGC_WANTALLKEYS;
    }

  default:
    {
      MSG msg;
      msg.message = Msg;
      msg.lParam  = lParam;
      msg.wParam  = wParam;
      EVENT e = g_win32ScreenDev->ProcessMessage(msg);
      if ( e.type != EVENT::NONE )
        g_win32ScreenDev->EnqueEvent(e);
    }
  }
  return
    (defproc_ ? defproc_ : DefWindowProc)(hWnd,Msg,wParam,lParam);
}

static void LockMouseInWindow(HWND hWnd,bool locking, WPARAM wParam)
{
  if ( locking ) {
    if ( mouse_locked ) return;
    mouse_locked = true;
    SetCapture(hWnd);
  }else{
    if ( !mouse_locked ) return;
    if ( ( wParam & MK_LBUTTON ) || ( wParam & MK_RBUTTON ) ) return;
    mouse_locked = false;
    ReleaseCapture();
  }
}

static u32_t ConvertToMSkinKey(WPARAM wParam, u32_t message)
{
  u32_t keyval = 0;
  if ( wParam & MK_LBUTTON ) keyval |= KEYVAL::EVK_LBUTTONF;
  if ( wParam & MK_RBUTTON ) keyval |= KEYVAL::EVK_RBUTTONF;
  if ( wParam & MK_CONTROL ) keyval |= KEYVAL::EVK_CTRLF;
  if ( wParam & MK_SHIFT )   keyval |= KEYVAL::EVK_SHIFTF;

  switch ( message ) {
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
    keyval |= KEYVAL::EVK_MBUTTON; break;
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
    keyval |= KEYVAL::EVK_LBUTTON; break;
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
    keyval |= KEYVAL::EVK_RBUTTON; break;
  }

  return keyval;
}

static void SetLeaveHandler(HWND hwnd_,bool& leave_handled)
{
  if ( !leave_handled )
  {
    TRACKMOUSEEVENT track = {0,};
    leave_handled = true;
    track.cbSize = sizeof(track);
    track.dwFlags = TME_LEAVE;
    track.dwHoverTime = HOVER_DEFAULT;
    track.hwndTrack = hwnd_;
    warning_if(!TrackMouseEvent(&track));
  }
}

EVENT Win32ScreenDevice::NextEvent(bool processMsgQue)
{
  if ( !que_.Empty() ) return DequeEvent();
  if ( processMsgQue )
  {
    MSG msg;
    immediate_event_handler_ = true;
    while ( PeekMessage(&msg,0,0,0,PM_REMOVE))
    {
      if ( msg.message == WM_QUIT )
        return EVENT::MakeQuitEvent();
      else
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      if ( !que_.Empty() )
        return DequeEvent();
    }
  }
  else
    immediate_event_handler_ = false;
  return DequeEvent();
}

EVENT Win32ScreenDevice::ProcessMessage(MSG& msg) {
  //LogF("WND msg %x (%x), wParam %x (%x), lParam %x",msg.message,WM_KEYDOWN,msg.wParam,VK_RIGHT,msg.lParam);
  if ( msg.message == WM_QUIT )
    return EVENT::MakeQuitEvent();
  else
  {
    {
      EVENT e;
      if ( HandleSpecific(msg,e) ) return e;
    }

    if ( msg.message == XCLOSE )
      return EVENT::MakeQuitEvent();

    switch ( msg.message ) {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
      {
        u32_t key = KEYVAL::NONE;
        unsigned winkey = msg.wParam& 0x0ff;
        switch ( winkey ) {
        case VK_DOWN:   key = KEYVAL::EVK_DOWN; break;
        case VK_UP:     key = KEYVAL::EVK_UP;   break;
        case VK_LEFT:   key = KEYVAL::EVK_LEFT; break;
        case VK_RIGHT:  key = KEYVAL::EVK_RIGHT;break;
        case VK_ESCAPE: key = KEYVAL::EVK_ESC;  break;
        case VK_TAB :   key = KEYVAL::EVK_TAB;break;
        case VK_SPACE : key = KEYVAL::EVK_SPACE;break;
        case VK_RETURN: key = KEYVAL::EVK_ENTER;break;
        case VK_BACK:   key = KEYVAL::EVK_BACKSPACE;break;
        case VK_F1:     key = KEYVAL::EVK_F1; break;
        case VK_F2:     key = KEYVAL::EVK_F2; break;
        case VK_F3:     key = KEYVAL::EVK_F3; break;
        case VK_F4:     key = KEYVAL::EVK_F4; break;
        case VK_F5:     key = KEYVAL::EVK_F5; break;
        case VK_F6:     key = KEYVAL::EVK_F6; break;
        case VK_F7:     key = KEYVAL::EVK_F7; break;
        case VK_F8:     key = KEYVAL::EVK_F8; break;
        case VK_F9:     key = KEYVAL::EVK_F9; break;
        case VK_F10:    key = KEYVAL::EVK_F11; break;
        case VK_F11:    key = KEYVAL::EVK_F10; break;
        case VK_F12:    key = KEYVAL::EVK_F12;break;
        case '=':       key = KEYVAL::EVK_PLUS;break;
        case '-':       key = KEYVAL::EVK_MINUS;break;
        case ',':       key = KEYVAL::EVK_COMMA;break;
        case '.':       key = KEYVAL::EVK_PERIOD;break;
        default:
          if ( isalpha(winkey) || winkey == '.' || winkey == ',' ) {
            key = winkey;
          }
        }
        if ( key == KEYVAL::NONE && winkey <= '9' && winkey >= '0' ) {
          key = KEYVAL::Number(winkey-'0');
        }
        if ( key != KEYVAL::NONE ) {
          if ( msg.message==WM_KEYDOWN || msg.message==WM_SYSKEYDOWN )
            return EVENT::MakeKeyDownEvent(key);
          else
            return EVENT::MakeKeyUpEvent(key);
        }
      }
      break;
    case WM_MOUSELEAVE:
      {
        leave_handled_ = false;
        u32_t keyval = ConvertToMSkinKey(LOWORD(msg.wParam),msg.message);
        return EVENT::MakePointLeaveEvent(keyval,
          mskin_GET_X_LPARAM(msg.lParam),
          mskin_GET_Y_LPARAM(msg.lParam),
          wheel);
      }
//        case WM_MOUSEWHEEL:
//          {
//            SetLeaveHandler(hwnd_,leave_handled_);
//            wheel += long((short)HIWORD(msg.wParam))/120;
//            u32_t keyval = ConvertToMSkinKey(LOWORD(msg.wParam),msg.message);
//            return EVENT::MakePointMoveEvent(keyval,
//              mskin_GET_X_LPARAM(msg.lParam),
//              mskin_GET_Y_LPARAM(msg.lParam),
//              wheel);
//          }
    case WM_MOUSEMOVE:
      {
        SetLeaveHandler(hwnd_,leave_handled_);
        u32_t keyval = ConvertToMSkinKey(msg.wParam,msg.message);
        return EVENT::MakePointMoveEvent(keyval,
          mskin_GET_X_LPARAM(msg.lParam),
          mskin_GET_Y_LPARAM(msg.lParam),
          wheel);
      }
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONUP:
      {
        SetLeaveHandler(hwnd_,leave_handled_);
        LockMouseInWindow(hwnd_,false,msg.wParam);
        u32_t keyval = ConvertToMSkinKey(msg.wParam,msg.message);
        return EVENT::MakePointUpEvent(keyval,
          mskin_GET_X_LPARAM(msg.lParam),
          mskin_GET_Y_LPARAM(msg.lParam));
      }
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDOWN:
      {
        if ( GetFocus() != hwnd_ )
          SetFocus(hwnd_);
        SetLeaveHandler(hwnd_,leave_handled_);
        LockMouseInWindow(hwnd_,true,msg.wParam);
        u32_t keyval = ConvertToMSkinKey(msg.wParam,msg.message);
        return EVENT::MakePointDownEvent(keyval,
          mskin_GET_X_LPARAM(msg.lParam),
          mskin_GET_Y_LPARAM(msg.lParam));
      }
    case WM_CHAR:
      {
        if ( msg.wParam >= 32 && ( msg.wParam < 128 || msg.wParam > 256 ) )
          return EVENT::MakeCharKeyEvent(msg.wParam);
        else break;
      }
    }
  }
  return EVENT::MakeNoneEvent();
}

#if MSKIN_TARGET_PLATFORM_EX  == MSKIN_PLATFORM_EX_WPC
static const char WINDOW_CLASS[] = "{212F0356-4BFF-44a7-802F-8B6ED33034CF}";
static const char WINDOW_TITLE[] = "";
#else
static const wchar_t WINDOW_CLASS[] = L"{212F0356-4BFF-44a7-802F-8B6ED33034CF}";
static const wchar_t WINDOW_TITLE[] = L"";
#endif

static void DoRegisterClass()
{
  WNDCLASS wc = {0,};
#if MSKIN_TARGET_PLATFORM_EX != MSKIN_PLATFORM_EX_WCE
  wc.style = CS_OWNDC;
#endif
  wc.hInstance = GetModuleHandle(0);
  wc.hIcon = 0;
  {for ( int i=0 ; !wc.hIcon && i < 10 ; ++i )
    wc.hIcon = LoadIcon((HINSTANCE)wc.hInstance,MAKEINTRESOURCE(i));}
  {for ( int i=200 ; !wc.hIcon && i < 210 ; ++i )
    wc.hIcon = LoadIcon((HINSTANCE)wc.hInstance,MAKEINTRESOURCE(i));}
  wc.lpfnWndProc = (WNDPROC)Win32ScreenDevice::WindowProc;
  wc.lpszClassName = WINDOW_CLASS;
  wc.hbrBackground = 0;
  REQUIRE( RegisterClass(&wc) );
}

static HWND DoCreateWindow(unsigned width,unsigned height)
{
  HWND hwnd = 0;
#if MSKIN_TARGET_PLATFORM_EX  == MSKIN_PLATFORM_EX_WPC
  DWORD ex_style = WS_EX_APPWINDOW;
#else
  DWORD ex_style = 0;
#endif
  DWORD style = (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX);
  RECT rect = {0,0,width,height};
  int screen_width = GetSystemMetrics(SM_CXSCREEN);
  int screen_height = GetSystemMetrics(SM_CYSCREEN);

  rect.left = (screen_width-rect.right)/2;
  rect.top = (screen_height-rect.bottom)/2;
  rect.right+=rect.left;
  rect.bottom+=rect.top;
  AdjustWindowRectEx(&rect,style,0,ex_style);
  hwnd = CreateWindowEx(
            ex_style,
            WINDOW_CLASS,
            WINDOW_TITLE,
            style,
            rect.left,rect.top,
            rect.right-rect.left,rect.bottom-rect.top,
            0,0,0,0);
  REQUIRE ( hwnd != 0 );
  return hwnd;
}

Win32ScreenDevice::Win32ScreenDevice(unsigned width,unsigned height)
{
  cursor_ = 0;
  enableCursor_ = false;
  attached_ = false;
  defproc_ = 0;
  g_win32ScreenDev = this;
  hdc_ = 0;
  DoRegisterClass();
  hwnd_ = DoCreateWindow(width,height);
  if ( hwnd_ ) {
    SetWindowTextW(hwnd_,title_.Str());
    hdc_  = GetDC(hwnd_);
  }
  leave_handled_ = false;
  immediate_event_handler_ = false;
}

Win32ScreenDevice::~Win32ScreenDevice()
{
  if ( hwnd_ && !attached_ ) {
    if ( hdc_ ) ReleaseDC( hwnd_,hdc_ );
    DestroyWindow(hwnd_);
  }
  UnregisterClass(WINDOW_CLASS,GetModuleHandle(0));
  g_win32ScreenDev = 0;
}

SCERROR Win32ScreenDevice::AttachTo(HWND wnd)
{
 // if ( wnd == hwnd_ ) return SCE_OK;
  if ( hwnd_ ) {
    if ( hdc_ && !attached_ )  ReleaseDC( hwnd_,hdc_ );
    if (!attached_)
      DestroyWindow(hwnd_);
    else
      if (defproc_) SetWindowLong(hwnd_,GWL_WNDPROC,(LONG)defproc_);
    hdc_ = 0;
  }
  if ( wnd )
  {
    hwnd_ = wnd;
    //*(LONG*)&defproc_ = GetWindowLong(hwnd_,GWL_WNDPROC);
    defproc_ = 0;
    *(LONG*)&defproc_ = SetWindowLong(hwnd_,GWL_WNDPROC,(LONG)WindowProc);
    if ( (LONG)defproc_ & 0x80000000 ) defproc_ = 0;
    attached_ = true;
  }
  else
  {
    attached_ = false;
    hwnd_ = 0;
    defproc_ = 0;
  }

  return SCE_OK;
}

SCERROR Win32ScreenDevice::ReNewWindow(int width,int height)
{
  if ( !attached_ || !hwnd_ )
  {
    if ( hwnd_ && !attached_ ) {
      if ( hdc_ )  ReleaseDC( hwnd_,hdc_ );
      DestroyWindow(hwnd_);
    }
    hdc_ = 0;
    fail_if_fail ( hwnd_ = DoCreateWindow(width,height) );
    hdc_ = GetDC(hwnd_);
    SetWindowTextW(hwnd_,title_.Str());
    leave_handled_ = false;
  }
  return SCE_OK;
}

bool Win32ScreenDevice::IsActive() const
{
  return isActive_;
}

void Win32ScreenDevice::EnableMouseCursor(bool flag) {
  enableCursor_ = flag;
  if ( flag && !cursor_ ) cursor_ = LoadCursor(0,IDC_ARROW);
  if ( flag ) SetCursor(cursor_); else SetCursor(0);
}

void Win32ScreenDevice::SetWndTitle(pwide_t title)
{
  if ( hwnd_ ) {
    title_ = title;
    SetWindowTextW(hwnd_,title);
  }
}

widestring Win32ScreenDevice::GetWndTitle() const
{
  return title_;
}

NAMESPACE_MSKIN_END

#endif
