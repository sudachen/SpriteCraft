
/*

Copyright Â© 2003-2013, AlexÃ©y SudachÃ©n, alexey@sudachen.name

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
#include "logout.h"

void Logout_LogOsVer()
  {
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(info);
    GetVersionEx(&info);
    const char* platf;
    switch ( info.dwPlatformId ){
    case VER_PLATFORM_WIN32s: /*úðú ¤ªþ, üv õ•õ ¨ðñþªðõü ?*/
      platf = "win32s";break;
    case VER_PLATFORM_WIN32_WINDOWS:
      if ( info.dwMinorVersion == 0 ) platf = "Windows95";
      else if ( info.dwMinorVersion == 10 ) platf = "Windows98";
      else if ( info.dwMinorVersion == 90 ) platf = "WindowsMe";
      else platf = "Windows9x(unknown)";
      break;
    case VER_PLATFORM_WIN32_NT:
      if ( info.dwMajorVersion == 3 ) platf = "WindowsNT(3.51)";
      else if ( info.dwMajorVersion == 4 ) platf = "WindowsNT(4.0)";
      else if ( info.dwMajorVersion == 5 )
      {
        if ( info.dwMinorVersion == 0 ) platf = "Windows2000";
        else if ( info.dwMinorVersion == 1 ) platf = "WindowsXP";
        else if ( info.dwMinorVersion == 2 ) platf = "Windows.NET";
        else platf = "WindowsNT(unknown)";
      }
      else platf = "WindowsNT(unknown)";
    }
    Lout << _S*"%? %? [%?.%?.%?]"
      %platf
      %info.szCSDVersion
      %info.dwMajorVersion
      %info.dwMinorVersion
      %info.dwBuildNumber;
  }

void Logout_LogMemoryInfo()
  {
    MEMORYSTATUS memstat;
    memstat.dwLength = sizeof(memstat);
    GlobalMemoryStatus(&memstat);
    Lout << _S*"pagefile free: %4dM of %4dM"
      %(memstat.dwAvailPageFile/(1024*1024))
      %(memstat.dwTotalPageFile/(1024*1024));
    Lout << _S*"memory   free: %4dM of %4dM"
      %(memstat.dwAvailPhys/(1024*1024))
      %(memstat.dwTotalPhys/(1024*1024));
  }

void Logout_LogCPUInfo()
  {
    u32_t result[3];
    u32_t ext_count;
    unsigned char tinfo[64] = {0};
    widestring cpuinfo;

    {

    __asm push edx
    __asm push ecx
    __asm push ebx
    __asm push eax
    __asm mov eax, 1
    __asm _emit 0x0f
    __asm _emit 0xa2
    __asm mov [result],eax
    __asm mov [result+4],edx
    __asm pop edx
    __asm pop ecx
    __asm pop ebx
    __asm pop eax

    }

    {

    __asm push edx
    __asm push ecx
    __asm push ebx
    __asm push eax
    __asm mov eax, 0x80000000
    __asm _emit 0x0f
    __asm _emit 0xa2
    __asm mov [ext_count],eax
    __asm pop edx
    __asm pop ecx
    __asm pop ebx
    __asm pop eax

    }

    if ( ext_count >= 0x80000004 )
    {
      __asm push edx
      __asm push ecx
      __asm push ebx
      __asm push eax
      __asm push edi
      __asm lea edi,tinfo
      __asm mov ecx,3

  lb_loop:
      __asm push ecx
      __asm mov eax, 0x80000005
      __asm sub eax,ecx
      __asm _emit 0x0f
      __asm _emit 0xa2
      __asm mov dword ptr [edi],   eax
      __asm mov dword ptr [edi+4], ebx
      __asm mov dword ptr [edi+8], ecx
      __asm mov dword ptr [edi+12],edx
      __asm add edi,16
      __asm pop ecx
      __asm dec ecx
      __asm jnz lb_loop

      __asm pop edi
      __asm pop edx
      __asm pop ecx
      __asm pop ebx
      __asm pop eax

      for ( int i = 1; i < sizeof(tinfo) && tinfo[i] ; )
        if ( tinfo[i] == tinfo[i-1] && tinfo[i] == ' ' || tinfo[i] < 32 || tinfo[i] > 128 )
          memmove(tinfo+i-1,tinfo+i,sizeof(tinfo)-i);
        else
          ++i;
    }
    else
    {
      __asm push edx
      __asm push ecx
      __asm push ebx
      __asm push eax
      __asm mov eax,0
      __asm _emit 0x0f
      __asm _emit 0xa2
      __asm mov dword ptr [tinfo],  ebx
      __asm mov dword ptr [tinfo+4],edx
      __asm mov dword ptr [tinfo+8],ecx
      __asm pop edx
      __asm pop ecx
      __asm pop ebx
      __asm pop eax
    }

    cpuinfo << (char*)tinfo;
    if ( result[1] & (MSKIN_BIT(23)|MSKIN_BIT(24)|MSKIN_BIT(25)) )
      {
        bool slh = false;
        cpuinfo << ('(');
        if ( result[1] & MSKIN_BIT(23) )
          {
            cpuinfo << L"MMX"; slh = true;
          }
        if ( result[1] & MSKIN_BIT(24) )
          {
            if ( slh ) cpuinfo << '/';
            cpuinfo << L"MMX2"; slh = true;
          }
        if ( result[1] & MSKIN_BIT(25) )
          {
            if ( slh ) cpuinfo << '/';
            cpuinfo << L"SSE";
          }
        cpuinfo << ')';
      }
    cpuinfo.RemoveExtraWS().Trim();
    Lout << cpuinfo;
  }
