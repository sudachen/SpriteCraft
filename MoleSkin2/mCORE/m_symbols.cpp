
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
#include "m_log.h"


#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 && \
    MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WPC 

#include <ImageHlp.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN
#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma warning(disable:4073)
#endif

static bool GetSection(void* ptr,const MEMORY_BASIC_INFORMATION& mbi,unsigned& seg,unsigned& offs)
{
  IMAGE_NT_HEADERS* nt_hdr = (IMAGE_NT_HEADERS*)((unsigned)mbi.AllocationBase+((PIMAGE_DOS_HEADER)mbi.AllocationBase)->e_lfanew);
  PIMAGE_SECTION_HEADER sec = IMAGE_FIRST_SECTION(nt_hdr);
  unsigned RVA = (unsigned)ptr-(unsigned)mbi.AllocationBase;
  for ( unsigned sec_no =0; 
    sec_no < (unsigned)nt_hdr->FileHeader.NumberOfSections; 
    ++sec_no)
  {
    unsigned start = sec[sec_no].VirtualAddress;
    unsigned end = start+teggo_max(sec[sec_no].SizeOfRawData,sec[sec_no].Misc.VirtualSize);
    if ( RVA >= start && RVA < end ){
      seg =  sec_no+1;
      offs = RVA-start;
      break;
    }
  }
  return false;
}

static const char* GetModuleName(const MEMORY_BASIC_INFORMATION& mbi)
{
  static char modulename[MAX_PATH];
  memset(modulename,0,MAX_PATH);
  if ( !GetModuleFileName((HMODULE)mbi.AllocationBase,modulename,MAX_PATH) )
  {
    return 0;
  }
  const char* _x = strrchr(modulename,'\\');
  if ( _x ) {
    return _x+1;
  }
  else if ( unsigned len = strlen(modulename) > 80 ) {
    modulename[len-80]='.';
    modulename[len-80+1]='.';
    modulename[len-80+2]='.';
    return modulename+len-80;
  }
  return modulename;
}

enum { SYMBUFFERSIZE = 1024 };


#if (_MSC_VER <= 1300 ) && !defined (DISABLE_IMAGEHLP) && defined (MSKIN_DEBUG)

#include <imagehlp.h>
#pragma comment(lib,"imagehlp.lib")

struct ImageHlpLib{
  HANDLE hProcess;
  IMAGEHLP_SYMBOL* syminfo;
  bool has_syminfo;
  IMAGEHLP_LINE li;
  bool has_lineinfo;
  unsigned long offs;
  char* trimSource;
  char* funName;
  ImageHlpLib()
  {
    syminfo = (IMAGEHLP_SYMBOL*)LocalAlloc(LPTR,sizeof(IMAGEHLP_SYMBOL)+256);
    hProcess = GetCurrentProcess();
    SymSetOptions(SYMOPT_UNDNAME|SYMOPT_DEFERRED_LOADS|SYMOPT_LOAD_LINES);
    SymInitialize(hProcess,NULL,TRUE);
  }
  ~ImageHlpLib()
  {
    LocalFree(syminfo);
    __try{SymCleanup(hProcess);}__except(1){}
  }
  void FindInfo(void* ptr)
  {
    __try{
      HANDLE _hProcess = GetCurrentProcess();
      if ( _hProcess != hProcess ) {
        __try{SymCleanup(hProcess);}__except(1){}
        hProcess = _hProcess;
        SymSetOptions(SYMOPT_UNDNAME|SYMOPT_DEFERRED_LOADS|SYMOPT_LOAD_LINES);
        SymInitialize(hProcess,NULL,TRUE);
      }

      memset(syminfo,0,sizeof(IMAGEHLP_SYMBOL)+256);
      syminfo->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
      syminfo->Address = (unsigned)ptr;
      syminfo->MaxNameLength = 255;
      has_syminfo = false;
      if(SymGetSymFromAddr(hProcess,(unsigned)ptr,&offs,syminfo))
      {
        li.Address=(unsigned)ptr;
        has_syminfo = true;
        has_lineinfo = false;
        if(SymGetLineFromAddr(hProcess,(unsigned)ptr,&(offs),&li))
        {
          trimSource = strrchr(li.FileName,'\\');
          if ( !trimSource ) trimSource = li.FileName;
          else trimSource+=1;
          has_lineinfo = true;
        }
        funName = syminfo->Name;
      }
    }
    __except(1)
    {
    }
  }
};

const char* MOLE_FUNCALL GetSymbol(void* ptr)
{
  static ImageHlpLib ihl; 
  static char symBuffer[SYMBUFFERSIZE];
  if ( symBuffer == 0 ) return "!broken!";
  memset(symBuffer,0,SYMBUFFERSIZE);
  MEMORY_BASIC_INFORMATION mbi;
#if CXX_COMPILER__IS_MSVC_COMPATIBLE
  __try
#endif
  {
    if(!VirtualQuery(ptr,&mbi,sizeof(mbi)))
    {
      wsprintf(symBuffer,"0x%08x:[unknown]:unknown");
      return symBuffer;
    }
    const char* module = GetModuleName(mbi);
    unsigned seg=0,offs=0;
    GetSection(ptr,mbi,seg,offs);
    if ( module == 0 ) module = "unknown";
    ihl.FindInfo(ptr);
    if ( ihl.has_lineinfo ){
      wsprintf(symBuffer,"0x%08x:[%s]!%s:{%s:%d}",ptr,module,ihl.funName,ihl.trimSource,ihl.li.LineNumber);
    }else if ( ihl.has_syminfo ){
      wsprintf(symBuffer,"0x%08x:[%s]!%s+%03x",ptr,module,ihl.funName,ihl.offs);
    }else {
      wsprintf(symBuffer,"0x%08x:[%s]:(%03x:%08x)",ptr,module,seg,offs);
    }
    return symBuffer;
  }
#if CXX_COMPILER__IS_MSVC_COMPATIBLE
  __except(1){}
#endif
  wsprintf(symBuffer,"!broken!0x%08x:",ptr);
  return symBuffer;
}

#else

const char* MOLE_FUNCALL GetSymbol(void* ptr)
{
  static char symBuffer[SYMBUFFERSIZE];
  if ( symBuffer == 0 ) return "!broken!";
  memset(symBuffer,0,SYMBUFFERSIZE);
#if CXX_COMPILER__IS_MSVC_COMPATIBLE
  __try
#endif
  {
    MEMORY_BASIC_INFORMATION mbi;
    if(!VirtualQuery(ptr,&mbi,sizeof(mbi)))
    {
      wsprintf(symBuffer,"0x%08x:[unknown]:unknown");
      return symBuffer;
    }
    const char* module = GetModuleName(mbi);
    unsigned seg=0,offs=0;
    GetSection(ptr,mbi,seg,offs);
    if ( module == 0 ) module = "unknown";
    wsprintf(symBuffer,"0x%08x:[%s]:(%03x:%08x)",ptr,module,seg,offs);
    return symBuffer;
  }
#if CXX_COMPILER__IS_MSVC_COMPATIBLE
  __except(1){}
#endif
  wsprintf(symBuffer,"!broken!0x%08x:",ptr);
  return symBuffer;
}

#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE //&& defined(MSKIN_DEBUG)
void MOLE_FUNCALL DumpStackTrace()
{
  unsigned* stack;
  __asm mov stack,ebp;
  __try
  {
    for ( unsigned bt=0; 
      bt < 32 && stack !=0 && *(stack+1) != 0; 
      ++bt,stack = (unsigned*)*stack )
    {
      const char* symbol;
      symbol = GetSymbol((void*)*(stack+1));
      LogF("  %s",symbol);
    }
  }
  __except(1)
  {
    LogF("  ...broken...");
  }
}
#else
void MOLE_FUNCALL DumpStackTrace()
{
  Lout << "-- no stack trace --";
}
#endif

/*
#include <imagehlp.h>
#pragma comment(lib,"imagehlp.lib")

const char* MOLE_FUNCALL GetSymbol(void* ptr) { return "-- ??? --";}
void MOLE_FUNCALL DumpStackTrace()
{
  LogF("-- no stack trace --");
}
*/

NAMESPACE_MSKIN_END

#elif MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL

NAMESPACE_MSKIN_BEGIN
const char* MOLE_FUNCALL GetSymbol(void* ptr) { return "-- ??? --";}
void MOLE_FUNCALL DumpStackTrace()
{
  LogF("-- no stack trace --");
}
NAMESPACE_MSKIN_END

#else // platform


NAMESPACE_MSKIN_BEGIN
const char* MOLE_FUNCALL GetSymbol(void* ptr) { return "-- ??? --";}
void MOLE_FUNCALL DumpStackTrace()
{
  LogF("-- no stack trace --");
}
NAMESPACE_MSKIN_END


#endif // platform