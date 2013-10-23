
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
#include "m_autoconf.h"
#include "m_debug.h"
#include <stdio.h>
#include <stdarg.h>

//#if ( MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 ) 
//#include <windows.h>
//#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN


#define ERROR_TO_PCHAR(err) case err: return #err

pchar_t TranslateECode(SCERROR e)
  {
    static char errtext[40];
    switch ( e )
      {
        ERROR_TO_PCHAR(SCE_OK);
        ERROR_TO_PCHAR(SCE_INVALIDARG);
        ERROR_TO_PCHAR(SCE_OUT_OF_MEMORY);
        ERROR_TO_PCHAR(SCE_OUT_OF_RANGE);
        ERROR_TO_PCHAR(SCE_OUT_OF_STREAM);
        ERROR_TO_PCHAR(SCE_BAD_FORMAT);
        ERROR_TO_PCHAR(SCE_BAD_STREAM);
        ERROR_TO_PCHAR(SCE_BAD_IMAGE);
        ERROR_TO_PCHAR(SCE_BAD_STATE);
        ERROR_TO_PCHAR(SCE_EOF);
        ERROR_TO_PCHAR(SCE_UNSUPPORTED);
        ERROR_TO_PCHAR(SCE_UNKNOWN);
        ERROR_TO_PCHAR(SCE_SURFLOST);
        ERROR_TO_PCHAR(SCE_INVALIDRECT);
        ERROR_TO_PCHAR(SCE_GZIPFAIL);
        ERROR_TO_PCHAR(SCE_FINISHED);
        ERROR_TO_PCHAR(SCE_FAIL);
      }
    sprintf(errtext,"unknown: 0x%x",e);
    return errtext;
  }

#pragma comment(lib,"version.lib")

widestring GetModuleVersion(pwide_t name)
{
  unsigned long foo = 0;
  unsigned size = GetFileVersionInfoSizeW((LPWSTR)name,&foo);
  if ( size != 0 )
  {
    BufferT<byte_t> buffer(size);
    if ( GetFileVersionInfoW((LPWSTR)name,0,size,&buffer[0]) )
    {
      VS_FIXEDFILEINFO* vs = 0;
      unsigned len = 0;
      if ( VerQueryValueW(&buffer[0],L"\\",(void**)&vs,&len) 
        && (0xfeef04bd == vs->dwSignature) )
      {
        return widestring(_S*L"%d.%d.%d.%d"
          %(vs->dwFileVersionMS >> 16)
          %(vs->dwFileVersionMS & 0xffff)
          %(vs->dwFileVersionLS >> 16)
          %(vs->dwFileVersionLS & 0xffff)
        );
      }
    }
  }
  return widestring(L"x.x.x.x");
}

NAMESPACE_MSKIN_END
