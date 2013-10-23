
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

#ifndef ___80ab0127_8cdb_4670_9e1d_b6e2016324e4___
#define ___80ab0127_8cdb_4670_9e1d_b6e2016324e4___

#include "_specific.h"

#define __lockon__(x) \
  if ( teggo::CRITICAL_SECTION_LOCKER __sync_locker_l = teggo::CRITICAL_SECTION_LOCKER(static_cast<CRITICAL_SECTION*>(x)) ) \
    goto TEGGO_LABEL(__LINE__); else TEGGO_LABEL(__LINE__):

#define __lock__ \
  if ( teggo::CRITICAL_SECTION_LOCKER __sync_locker_l = teggo::CRITICAL_SECTION_LOCKER(+teggo::GLOBAL_CRITICAL_SECTION()) ) \
    goto TEGGO_LABEL(__LINE__); else TEGGO_LABEL(__LINE__):

namespace teggo {

  _TEGGO_EXPORTABLE void SelectSyncObject(CRITICAL_SECTION **_cs);

  struct GLOBAL_CRITICAL_SECTION
    {
      _TEGGO_EXPORTABLE CRITICAL_SECTION *_Select() const;
      operator CRITICAL_SECTION *() const { return _Select(); }
      //CRITICAL_SECTION *operator&() const { return _Select(); }
      CRITICAL_SECTION *operator+() const { return _Select(); }
    };

  struct Tlocker
    {
      mutable CRITICAL_SECTION *cs_;
      Tlocker() : cs_ (0) {};
      ~Tlocker()
        {
          if ( cs_ )
            {
              DeleteCriticalSection(cs_);
              HeapFree(GetProcessHeap(),0,cs_);
            }
        }
      CRITICAL_SECTION *_Select() const
        {
          if (!cs_) SelectSyncObject(&cs_);
          return cs_;
        }
      operator CRITICAL_SECTION *() const
        {
          return _Select();
        }
      CRITICAL_SECTION *operator&() const
        {
          return _Select();
        }
      void Lock()
        {
          EnterCriticalSection(_Select());
        }
      void Unlock()
        {
          LeaveCriticalSection(_Select());
        }
    };

  struct CRITICAL_SECTION_LOCKER
    {
      CRITICAL_SECTION *locker_;
      CRITICAL_SECTION_LOCKER(CRITICAL_SECTION *l) : locker_(l)
        { EnterCriticalSection(locker_); }
      ~CRITICAL_SECTION_LOCKER()
        { LeaveCriticalSection(locker_); }
      operator bool() const { return false; }
    };

  template <class T>
    struct TsafeHolder
      {
        T *instance_;
        T *Get()
          {
            if ( !instance_ )
              __lock__
                {
                  if ( !(T * volatile&)instance_ )
                    instance_ = new T();
                }
            return instance_;
          }
        T *operator ->() { return Get(); }
      };

#if defined _TEGGOINLINE || defined _TEGGO_THREADS_HERE

#if defined _TEGGOINLINE
# define _TEGGO_THREADS_FAKE_INLINE CXX_FAKE_INLINE
#else
# define _TEGGO_THREADS_FAKE_INLINE _TEGGO_EXPORTABLE
#endif

  _TEGGO_THREADS_FAKE_INLINE
  void SelectSyncObject(CRITICAL_SECTION **_cs)
    {
      CRITICAL_SECTION *cs = (CRITICAL_SECTION *)HeapAlloc(GetProcessHeap(),0,sizeof(CRITICAL_SECTION));
      InitializeCriticalSection(cs);
      if ( 0 != InterlockedCompareExchange((long*)_cs,(long)cs,0) )
        {
          DeleteCriticalSection(cs);
          HeapFree(GetProcessHeap(),0,cs);
        }
    }

  _TEGGO_THREADS_FAKE_INLINE
  CRITICAL_SECTION *GLOBAL_CRITICAL_SECTION::_Select() const
    {
      static CRITICAL_SECTION *cs = 0;
      if ( !cs ) SelectSyncObject(&cs);
      return cs;
    }


#endif

}

#endif /*___80ab0127_8cdb_4670_9e1d_b6e2016324e4___*/
