
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

#if !defined __0103EC7B_15B0_47B9_A506_4B4951D96D61__
#define __0103EC7B_15B0_47B9_A506_4B4951D96D61__

#include "_specific.h"
#include "string.h"
#include "array.h"
#include "format.h"
#include "hinstance.h"

#if defined _WIN32 && CXX_COMPILER_IS_MSVC_COMPATIBLE
# pragma comment(lib,"version.lib")
# pragma comment(lib,"winmm.lib")
#endif

namespace teggo {

  _TEGGO_EXPORTABLE StringT<char> CXX_STDCALL GetModuleVersion( pwide_t modname );
  _TEGGO_EXPORTABLE StringT<char> CXX_STDCALL SysFormatError( u32_t lerror = GetLastError());
  _TEGGO_EXPORTABLE StringT<char> CXX_STDCALL SysFormatErrorOem( u32_t lerror = GetLastError());
  _TEGGO_EXPORTABLE void *CXX_STDCALL SysMmapFile(StringParam fname, u32_t size = 0, u32_t offset = 0, bool writable = true, bool create_new = false);
  _TEGGO_EXPORTABLE void *CXX_STDCALL SysMmapFileByHandle(HANDLE handle, u32_t size = 0, u32_t offset = 0, bool writable = true);
  _TEGGO_EXPORTABLE void  CXX_STDCALL SysUnmmapFile(void *);

  inline StringT<char> CXX_STDCALL GetModuleVersion( pchar_t modname )
    {
      return GetModuleVersion(+StringT<wchar_t>(modname));
    }

  _TEGGO_EXPORTABLE void  CXX_STDCALL ProcessYield();
  _TEGGO_EXPORTABLE u32_t CXX_STDCALL GetMillis(void);
  _TEGGO_EXPORTABLE void  CXX_STDCALL ResetMillis(u32_t millis = 0);

#if defined _TEGGOINLINE || defined _TEGGO_SYSUTIL_HERE

#if !defined _TEGGO_SYSUTIL_HERE
# define _TEGGO_SYSUTIL_FAKE_INLINE  CXX_FAKE_INLINE
#else
# define _TEGGO_SYSUTIL_FAKE_INLINE
#endif

  _TEGGO_SYSUTIL_FAKE_INLINE
  StringT<char> CXX_STDCALL GetModuleVersion( pwide_t modname )
    {
      unsigned long foo = 0;
      unsigned size = GetFileVersionInfoSizeW((wchar_t*)modname,&foo);
      if ( size != 0 )
      {
        BufferT<byte_t> buffer(size);
        if ( GetFileVersionInfoW((wchar_t*)modname,0,size,&buffer[0]) )
        {
          VS_FIXEDFILEINFO* vs = 0;
          unsigned len = 0;
          if ( VerQueryValueW(&buffer[0],L"\\",(void**)&vs,&len)
            && (0xfeef04bd == vs->dwSignature) )
          {
            return StringT<char>(
              _S*"%d.%d.%d.%d"
              %(vs->dwFileVersionMS >> 16)
              %(vs->dwFileVersionMS & 0xffff)
              %(vs->dwFileVersionLS >> 16)
              %(vs->dwFileVersionLS & 0xffff));
          }
        }
      }
      return StringT<char>("x.x.x.x");
    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  StringT<char> CXX_STDCALL SysFormatError( u32_t lerror )
    {
      BufferT<char> buff(1024,0);
      u32_t len =
        FormatMessageA(
          FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
          NULL,
          lerror,
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
          &buff[0],
          buff.Count()-1,
          NULL
        );
      char* pE = buff.Begin()+len;
      pE = teggo::remove(buff.Begin(),pE,'\n');
      pE = teggo::remove(buff.Begin(),pE,'\r');
      *pE = 0;
      return StringT<char>(+buff);
    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  StringT<char> CXX_STDCALL SysFormatErrorOem( u32_t lerror )
    {
      StringT<char> se = SysFormatError( lerror );
      BufferT<char> oem;
      oem.Resize(se.Length()+1);
      CharToOemA(+se,+oem);
      return StringT<char>(+oem);
    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  void CXX_STDCALL ProcessYield()
    {
      typedef BOOL (__stdcall *SwitchToThread_t)(void);
      static SwitchToThread_t SwitchToThread_f  =
        (SwitchToThread_t)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"SwitchToThread");
      if ( SwitchToThread_f )
        SwitchToThread_f();
      else
        Sleep(1);
    }

  template <unsigned tFake>
    struct Millis_Static_
      {
        static u32_t millis;
        static u32_t millis_pf;
        static u32_t millis_pf_interval;
      };
  template <unsigned tFake> u32_t Millis_Static_<tFake>::millis = 0;
  template <unsigned tFake> u32_t Millis_Static_<tFake>::millis_pf = 0;
  template <unsigned tFake> u32_t Millis_Static_<tFake>::millis_pf_interval = 0;

  _TEGGO_SYSUTIL_FAKE_INLINE
  u32_t CXX_STDCALL __get_millis_pf()
    {
      static __int64 perffreq = 0;
      static BOOL _fake = QueryPerformanceFrequency((LARGE_INTEGER*)&perffreq);
      if ( perffreq > 1000 )
        {
          __int64 val;
          QueryPerformanceCounter((LARGE_INTEGER*)&val);
          return u32_t(val / (perffreq/1000));
        }
      else
        return timeGetTime();
    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  u32_t CXX_STDCALL GetMillis(void)
    {
      typedef Millis_Static_<0> g;
      static u32_t last_ms = timeGetTime();
      u32_t ms  = timeGetTime();
      u32_t mpf = __get_millis_pf();
      if ( ms - last_ms > (mpf-g::millis_pf+20) )
        {
          g::millis_pf = mpf;
          g::millis_pf_interval = ms - g::millis;
        }
      last_ms = ms;
      return mpf-g::millis_pf + g::millis_pf_interval;
    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  void CXX_STDCALL ResetMillis(u32_t millis)
    {
      typedef Millis_Static_<0> g;
      g::millis_pf = __get_millis_pf();
      g::millis    = timeGetTime();
      if ( !millis )
        g::millis_pf_interval = 0;
      else
        g::millis_pf_interval = g::millis - millis;
    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  void *CXX_STDCALL SysMmapFile(StringParam fname, u32_t size, u32_t offset, bool writable, bool create_new)
    {
      HANDLE file_handle = INVALID_HANDLE_VALUE;
      if ( Hinstance->IsWi9x() )
        file_handle = CreateFileA(
                        +StringA(+fname),
                        GENERIC_READ|(writable?GENERIC_WRITE:0),
                        FILE_SHARE_READ,
                        0,
                        (create_new?CREATE_ALWAYS:OPEN_EXISTING),
                        0,0);
      else
        file_handle = CreateFileW(
                        +fname,
                        GENERIC_READ|(writable?GENERIC_WRITE:0),
                        FILE_SHARE_READ,
                        0,
                        (create_new?CREATE_ALWAYS:OPEN_EXISTING),
                        0,0);

      if ( file_handle != INVALID_HANDLE_VALUE )
        {
          void *p = SysMmapFileByHandle(file_handle, size, offset, writable);
          CloseHandle(file_handle);
          return p;
        }

      return 0;

    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  void *CXX_STDCALL SysMmapFileByHandle(HANDLE handle, u32_t size, u32_t offset, bool writable)
    {
      byte_t *result = 0;
      HANDLE file_mapping = 0;
      if ( handle != INVALID_HANDLE_VALUE )
        file_mapping = CreateFileMappingA(
                      handle,0,
                      (writable?PAGE_READWRITE:PAGE_READONLY),
                      0,size,0);

      if ( file_mapping )
        {if ( !(result = (byte_t*)MapViewOfFile(
                                  file_mapping,(writable?FILE_MAP_WRITE:0)|FILE_MAP_READ,0,offset,size)) )
            printf("mapping_file:%s\n",+teggo::SysFormatErrorOem()); }
      else
        printf("create_mapping:%s\n",+teggo::SysFormatErrorOem())
        ;

      if ( file_mapping ) CloseHandle( file_mapping );
      return result;
    }

  _TEGGO_SYSUTIL_FAKE_INLINE
  void CXX_STDCALL SysUnmmapFile(void *p)
    {
      UnmapViewOfFile(p);
    }

#endif

} // namespace

#endif // __0103EC7B_15B0_47B9_A506_4B4951D96D61__
