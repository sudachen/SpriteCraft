
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


#if !defined ___41F3F0C3_8C9B_4A61_8846_83C5692309C0___
#define ___41F3F0C3_8C9B_4A61_8846_83C5692309C0___

#include "_specific.h"

#if defined _TEGGO_CODECOP_HERE
# define _TEGGO_CODECOP_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_CODECOP_FAKE_INLINE CXX_FAKE_INLINE
#endif

extern "C" int __stdcall IsDebuggerPresent(void);
//extern "C" void __stdcall DebugBreak(void);

#if defined _DEBUG
_TEGGO_CODECOP_FAKE_INLINE
void CXX_STDCALL _Teggo_Debug_Code_Here_()
  {
    if ( IsDebuggerPresent() )
      DebugBreak();
  }
#endif

namespace teggo {

  _TEGGO_CODECOP_FAKE_INLINE
  void CXX_STDCALL CodePolice__::Message(pwide_t msg,pwide_t prefix, int lineno, pchar_t fname)
    {
      HANDLE errh = GetStdHandle(STD_ERROR_HANDLE);
      if ( errh && errh != INVALID_HANDLE_VALUE  )
        fprintf(stderr,"%ls: %ls, at '%s':%d \n",prefix,msg,fname,lineno);
      else
        {
        // ВНИМАНИЕ!!!
        // важно для приложений не линкующих user32.lib
        // при установленном McAffe Virus Scan будет пустое окно
        // антивирус нарушает работу подсистемы ресурсов если она не загружается
        // во время старта приложения
        #if defined _WIN32
          int (__stdcall *MessageBoxW_fn)(HWND,pwide_t,pwide_t,int);
          HMODULE usr32dll = LoadLibraryA("user32.dll");
          *((FARPROC*)&MessageBoxW_fn)
            = GetProcAddress(usr32dll,"MessageBoxW");
          MessageBoxW_fn(0,msg,prefix,0);
          FreeLibrary(usr32dll);
        #endif
        }
    }

  _TEGGO_CODECOP_FAKE_INLINE
  void CXX_STDCALL CodePolice__::Terminator(pwide_t msg, int lineno, pchar_t fname)
    {
      char mem[96] = {0};
      Message(msg,L"CodPolice::Termination",lineno,fname);
      _Teggo_Debug_Code_Here_();
      abort();
    }

  _TEGGO_CODECOP_FAKE_INLINE
  void CXX_STDCALL CodePolice__::Notifier(pwide_t msg, int lineno, pchar_t fname)
    {
      static pwide_t prefix = L"CodPolice::Notification";
      Message(msg,L"CodPolice::Notification",lineno,fname);
    }


  template <> _TEGGO_CODECOP_FAKE_INLINE
    void CodePolice_<0>::Analyse( ILLEGAL_SITUATION sit, pwide_t descr, int lineno, pchar_t fname )
      {
        if ( sit == SIT_VIOLATION || sit == SIT_ASSERTION || sit == SIT_EMERGENCY )
          {
            if ( terminate_ )
              (*terminate_)(descr,lineno,fname);
          }
        else if ( sit == SIT_WARNING )
          {
            if ( notify_ )
              (*notify_)(descr,lineno,fname);
          }
      }

#if !defined _TEGGOINLINE
  template <> _TEGGO_EXPORTABLE_DATA
    CodePolice_<0> CodePolice_Office<0>::police_
      = {
          &CodePolice__::Notifier,
          &CodePolice__::Terminator
        };
#endif

} // namespace teggo

#undef _TEGGO_CODECOP_FAKE_INLINE

#endif // ___41F3F0C3_8C9B_4A61_8846_83C5692309C0___
