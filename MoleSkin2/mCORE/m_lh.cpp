
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
#include "m_process.h"
#include "m_sync.h"
#include "m_singletons.h"

#if \
    MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 && \
    MSKIN_TARGET_PLATFORM_EX == MSKIN_PLATFORM_EX_WPC && \
    CXX_COMPILER__IS_MSVC_COMPATIBLE

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mCORE")
#endif

NAMESPACE_MSKIN_BEGIN

#pragma warning(disable:4073)
//#pragma init_seg(lib)

#define DISABLE_LEAKHUNTER

#if (defined MSKIN_DEBUG && !defined DISABLE_LEAKHUNTER) || (defined FORCE_LEAKHUNTER) 
#define USE_LEAKHUNTER
#else
#undef USE_LEAKHUNTER
#endif

static void WriteLeakLog(const char* text);
static void ClearLeakLog();
static void PrintfLeakLog(const char* format,...);

#if defined USE_LEAKHUNTER

static const unsigned STACK_DEPTH = 32;
static const unsigned MEMHASH_TABLE_SIZE = 2019;
static const unsigned LEAKTRACER_RESERVE_POOL_SIZE = (1024*1024);
static const unsigned LEAKTRACER_PRESERVE_SIZE = 4*1024;

struct MemLink{
  bool broken;
  unsigned mem_ptr;
  unsigned size;
  unsigned back_trace[STACK_DEPTH];
  MemLink* next;
  MemLink* sort;
};

static inline unsigned ROUND4K(unsigned v) { 
  if ( v%4096 != 0 ) v+=4096-v%4096;
  return v;
}

static void DumpCurrentStackTrace()
{
  unsigned* stack;
  __asm mov stack,ebp;
  __try
  {
    for ( unsigned bt=0; 
      bt < STACK_DEPTH && stack !=0 && *(stack+1) != 0; 
      ++bt,stack = (unsigned*)*stack )
    {
      const char* symbol;
      symbol = GetSymbol((void*)*(stack+1));
      PrintfLeakLog("  %s",symbol);
    }
  }
  __except(1)
  {
    PrintfLeakLog("  ...broken...");
  }
}

class LeackHunterSingleton
{
  static bool isDead;
  
  MemLink* mem_table[MEMHASH_TABLE_SIZE];
  MemLink* mem_pool;
  MemLink* free_link;
  unsigned commited_elements;

public:
  ThreadLocker_ thl;
  LeackHunterSingleton()
  {
    isDead = false;
    mem_pool = 0;
    free_link = 0;
    commited_elements = 0;
    // initializing
    memset(mem_table,0,sizeof(mem_table));
    if ( 0 == (mem_pool = (MemLink*)VirtualAlloc(0,ROUND4K(LEAKTRACER_RESERVE_POOL_SIZE*sizeof(MemLink)),MEM_RESERVE,PAGE_NOACCESS) ))
      FatalError("LeakTrace can't reserve memory");
    if ( !VirtualAlloc(mem_pool,ROUND4K(LEAKTRACER_PRESERVE_SIZE*sizeof(MemLink)),MEM_COMMIT,PAGE_READWRITE) )
      FatalError("LeakTrace can't commit memory");
    memset(mem_pool,0,ROUND4K(LEAKTRACER_PRESERVE_SIZE*sizeof(MemLink)));
    commited_elements = LEAKTRACER_PRESERVE_SIZE;
    for ( unsigned i = 1; i< commited_elements ; ++i ){
      mem_pool[i].next = mem_pool+(i-1);
    }
    free_link = mem_pool+(commited_elements-1);
  }
  
  ~LeackHunterSingleton()
  {
    ClearLeakLog();
    DumpLeaks();
    isDead = true;
  }

  void Reallocate()
  {
    FatalError("LeakTrace can't commit memory");
  }

  void Manage(void* ptr,unsigned size)
  {
    if ( !ptr ) {
      PrintfLeakLog(":Manage: can't manage zero pointer\n");
      DumpCurrentStackTrace();
      DebugBreak();
      return;
    }
    
    if ( !free_link ) Reallocate();

    MemLink* link = free_link;
    free_link = free_link->next;
    link->mem_ptr = (unsigned)ptr;
    link->size = size;
    {
      int cnt = 0;
      unsigned* stack;
      __asm mov stack,ebp;
      memset(link->back_trace,0,STACK_DEPTH*4);
      __try
      {
        while(stack != 0 && *(stack+1) != 0 && cnt < STACK_DEPTH+2){
          if ( cnt > 1 )
            link->back_trace[cnt-2] = (*(stack+1));
          ++cnt;
          stack = (unsigned*)*stack;
        }
        link->broken = false;
      }
      __except(1)
      {
        link->broken = true;
      }
    }

    link->next = mem_table[(unsigned)ptr%MEMHASH_TABLE_SIZE];
    mem_table[(unsigned)ptr%MEMHASH_TABLE_SIZE] = link;
  }

  void Unmaname(void* ptr)
  {
    if ( ptr == 0 ) return;
    MemLink* prev = 0;
    MemLink* link = mem_table[(unsigned)ptr%MEMHASH_TABLE_SIZE];
    while ( link ){
      if ( link->mem_ptr == (unsigned)ptr ){
        link->mem_ptr = 0;
        if ( prev ) {
          prev->next = link->next;
        }else{
          mem_table[(unsigned)ptr%MEMHASH_TABLE_SIZE] = link->next;
        }
        break;
      }else{
        prev = link;
        link = link->next;
      }
    }
    if ( !link ) FatalError("Opps, delete on unallocated memory!");
    link->next = free_link;
    free_link = link;
  }

  void DumpLeaks()
  {
    unsigned u = 0;
    for ( u=0; u < commited_elements; ++u )
    {
      if ( mem_pool[u].mem_ptr != 0 )
      {
        PrintfLeakLog("leak size:%d, memory:0x%x\n",mem_pool[u].size,mem_pool[u].mem_ptr);
        for ( unsigned bt=0; bt < STACK_DEPTH; ++bt )
        {
          const char* symbol;
          if ( mem_pool[u].back_trace[bt] == 0 ) break;
          symbol = GetSymbol((void*)mem_pool[u].back_trace[bt]);
          PrintfLeakLog("  %s\n",symbol);
        }
        if ( mem_pool[u].broken )
          PrintfLeakLog("  ...broken...\n");
      }
    }
  }

public:
  enum Op{MANAGE,UNMANAGE,NONE};
  static void Manager(Op op,void* ptr,unsigned size=0);

};

typedef Singleton< LeackHunterSingleton,SingletonTraits<INT_MAX> > LHS;

void LeackHunterSingleton::Manager(Op op,void* ptr,unsigned size)
{
  Synchronizer_ _(LHS::Instance()->thl);
  switch ( op ) {
  case MANAGE:  LHS::Instance()->Manage(ptr,size); break;
  case UNMANAGE: LHS::Instance()->Unmaname(ptr); break;
  case NONE: break;
  default:
    FatalError("unknown leakhunter command");
  }        
}

struct FakeInitializer { FakeInitializer() {LeackHunterSingleton::Manager(LeackHunterSingleton::NONE,0);}};
static FakeInitializer __fake_initializer__;

//CRITICAL_SECTION_lock LeackHunterSingleton::cs_;
bool LeackHunterSingleton::isDead = false;

static const char* MakeFileName()
{
  static bool isok = false;
  static char path[MAX_PATH+1] = {0};
  if ( !isok )
  {
    memset(path,0,sizeof(path));
    //GetModuleFileName(GetModuleHandle(0),path,MAX_PATH-10);
    wsprintf(path,"%s/%s",MCORE::GetModulePath(),MCORE::GetModuleName());
    wsprintf(path+strlen(path),"-leak.txt");
    isok = true;
  }
  return path;
}

static void WriteLeakLog(const char* text)
{
  static bool first_pass = true;
  const char* path = MakeFileName();
  ClearLeakLog();
  HANDLE file = CreateFile(path,GENERIC_WRITE,FILE_SHARE_READ,0,first_pass?CREATE_ALWAYS:OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  first_pass = false;
  if ( !file ) {
    MessageBox(0,"Can't open file for dump memleaks","",MB_OK|MB_ICONSTOP);
    TerminateProcess(GetCurrentProcess(),0);
  }
  SetFilePointer(file,0,0,FILE_END);
  unsigned long l;
  WriteFile(file,text,lstrlen(text),&l,0);
  CloseHandle(file);
}

static void ClearLeakLog()
{
  static bool cleared = false;
  if ( cleared ) return;
  const char* modulename = MakeFileName();
  DeleteFile(modulename);
  cleared = true;
}

static void PrintfLeakLog(const char* format,...)
{
  va_list va;
  va_start(va,format);
  WriteLeakLog(Format_va(format,va));
  va_end(va);
}

#endif

NAMESPACE_MSKIN_END

void* LHunterManage(void* ptr,unsigned size)
{
#ifdef USE_LEAKHUNTER
  MSKINSPACE::LeackHunterSingleton::Manager(MSKINSPACE::LeackHunterSingleton::MANAGE,ptr,size);
#endif
  return ptr;
}

void* LHunterUnmanage(void* ptr) 
{
#ifdef USE_LEAKHUNTER
  MSKINSPACE::LeackHunterSingleton::Manager(MSKINSPACE::LeackHunterSingleton::UNMANAGE,ptr);
#endif
  return ptr;
}

#else

void* LHunterManage(void* p,unsigned) { return p; }
void* LHunterUnmanage(void* p) { return p; }

#endif // platform