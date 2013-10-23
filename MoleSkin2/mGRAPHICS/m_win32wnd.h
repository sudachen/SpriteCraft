
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

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

#if !defined __m_misc_win32_winbase_h__
#define __m_misc_win32_winbase_h__

#include "../mcore/m_autoconf.h"
#include "m_input.h"

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32

//#include <windows.h>

NAMESPACE_MSKIN_BEGIN

/**
  имплементационный класс для Win32
*/
class Win32ScreenDevice {
private:  
  HWND hwnd_;
  HDC  hdc_;
  bool enableCursor_;
  bool attached_;
  HCURSOR cursor_;
  widestring title_;
  bool leave_handled_;
  bool immediate_event_handler_;
  SlistT<EVENT> que_;

public:

  enum 
  {
    XCLOSE = WM_USER+0x32,
    XEVENT = WM_USER+0x33
  };

  bool isRubbish_;
  bool isActive_;
  
  static LRESULT (CALLBACK *defproc_)(HWND,UINT,WPARAM,LPARAM);
  static LRESULT CALLBACK WindowProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
  EVENT ProcessMessage(MSG& smg);
  
  enum { que_max_size_ = 8 }; 
  
  void EnqueEvent(EVENT& e) 
  { 
    if ( que_.Count() < que_max_size_ ) 
    {
      if ( !que_.Empty() && memcmp(&que_.Last(),&e,sizeof(EVENT)) == 0 ) 
        return;
      que_.Append(e);
    }
    
    //if ( !immediate_event_handler_ )
    //  SendMessage( hwnd_, XEVENT, 0, 0 );  
  }
 
  EVENT DequeEvent() 
  { 
    EVENT e = EVENT::MakeNoneEvent();
    if ( !que_.Empty() ) e = que_.Top(), que_.Pop(); 
    return e;
  }

  SCERROR AttachTo(HWND wnd);
  bool IsAttached() { return attached_; }

  EVENT NextEvent(bool processMsgQue); 
  bool  IsRubbish() const;
  void  MakeRubbish(); 
  bool  IsActive() const;
  void  EnableMouseCursor(bool);
  
  Win32ScreenDevice(unsigned width=320,unsigned height=240);
  virtual ~Win32ScreenDevice();
  virtual bool HandleSpecific(const MSG&,EVENT&);

  HWND GetWnd()  const { return hwnd_; }
  HDC  GetDc()   const { return hdc_; }
  SCERROR ReNewWindow(int width,int height);
  void SetWndTitle(pwide_t title);
  widestring GetWndTitle() const;

  virtual void OnResize() {}
  virtual void OnPaint() {}
};

NAMESPACE_MSKIN_END

#endif // platform

#endif // __m_misc_win32_winbase_h__

