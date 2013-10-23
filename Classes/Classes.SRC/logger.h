
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

#if !defined ___1a21d8d4_6b42_4208_b579_a39b603bc4f8___
#define ___1a21d8d4_6b42_4208_b579_a39b603bc4f8___

#include "_specific.h"
#include "string.h"
#include "format.h"

enum _T_logger_out_
  {
    Lout    = -1,
    Lerr    = 0,
    Ldbg1   = 1, Ldbg2, Ldbg3, Ldbg4, Ldbg5, Ldbg6,
    Ldbg    = Ldbg3,
    LdbgMax = Ldbg5
  };

_TEGGO_EXPORTABLE void CXX_STDCALL operator << ( _T_logger_out_, teggo::FormatT<char> const &fmt);
_TEGGO_EXPORTABLE void CXX_STDCALL operator << ( _T_logger_out_, teggo::FormatT<wchar_t> const &fmt);

namespace teggo {

  typedef HANDLE LOG_HANDLE;

#ifdef _TEGGOINLINE
  template < class tFake > struct logcontrol_
#else
  struct logcontrol
#endif
    {
      _TEGGO_EXPORTABLE static LOG_HANDLE handle;
      _TEGGO_EXPORTABLE static int  dbglevel;
      _TEGGO_EXPORTABLE static bool reopen;
    };
#ifdef _TEGGOINLINE
  typedef logcontrol_<int> logcontrol;
#endif

  inline bool LogLevelGeq(int lo) { return lo <= logcontrol::dbglevel; }

  _TEGGO_EXPORTABLE int  CXX_STDCALL OpenLog(pwide_t logname = 0,pwide_t suffix = 0);
  _TEGGO_EXPORTABLE void CXX_STDCALL CloseLog();
  _TEGGO_EXPORTABLE bool CXX_STDCALL LogIsOpen();

  _TEGGO_EXPORTABLE void CXX_STDCALL Logout( pwide_t text, int level );
  _TEGGO_EXPORTABLE void CXX_STDCALL Logout( pchar_t text, int level );
  _TEGGO_EXPORTABLE void CXX_STDCALL Logout_( pchar_t text, int count, int level );
  _TEGGO_EXPORTABLE void CXX_STDCALL LogoutBufferCRLF_( pchar_t buff, int count, int level );

} // namsepace

template <class tTc>
  inline void CXX_STDCALL operator << ( _T_logger_out_ lo, tTc const *text)
    { teggo::Logout( text, lo ); }

template <class tTc>
  inline void CXX_STDCALL operator << ( _T_logger_out_ lo, teggo::BaseStringT<tTc> const &text)
    { teggo::Logout( +text, lo ); }

#if defined _TEGGOINLINE
#include "Logger.inl"
#endif

#endif // ___1a21d8d4_6b42_4208_b579_a39b603bc4f8___
