
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


#if !defined (__m_sync_h__)
#define __m_sync_h__

#include "m_autoconf.h"
#include "m_debug.h"
#include "m_errors.h"

NAMESPACE_MSKIN_BEGIN

#if ( MSKIN_THREADS  )
#define VOLATILE volatile
#else
#define VOLATILE
#endif

#if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32
class ThreadLocker_
{
  CRITICAL_SECTION cs;
public:
  ThreadLocker_() {
    InitializeCriticalSection(&cs);
  }
  ~ThreadLocker_() {
    DeleteCriticalSection(&cs);
  }
  void Lock() {EnterCriticalSection(&cs);}
  void Unlock() {LeaveCriticalSection(&cs);}
private:
  ThreadLocker_(const ThreadLocker_&);
  ThreadLocker_& operator = (const ThreadLocker_&);
};

class Synchronizer_
{
  ThreadLocker_& lock_;
public:
  Synchronizer_(ThreadLocker_& lock) : lock_(lock) { lock_.Lock(); }
  ~Synchronizer_() { lock_.Unlock(); }
private:
  Synchronizer_(const Synchronizer_&);
  Synchronizer_& operator = (const Synchronizer_&);
};

#else
#error "threads unavailable"
#endif

#if ( MSKIN_THREADS ) && \
    ( MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_WIN32 )

typedef ThreadLocker_ ThreadLocker;

typedef void (*MSKIN_THREADPROC)(void*);
SCERROR MOLE_FUNCALL RunThread(MSKIN_THREADPROC);

#elif ( MSKIN_THREADS )
#error has no threadlocker implementation    

#else // no threads

class ThreadLocker{
public:
  ThreadLocker() {}
  ~ThreadLocker() {}
  void Lock() {}
  void Unlock() {}
private:
  ThreadLocker(const ThreadLocker&);
  ThreadLocker& operator = (const ThreadLocker&);
};

#endif

#if ( MSKIN_THREADS )

typedef Synchronizer_ Synchronizer; 

#else

class Synchronizer
{
public:
  Synchronizer(ThreadLocker&) {}
  ~Synchronizer() {}
private:
  Synchronizer(const Synchronizer&);
  Synchronizer& operator = (const Synchronizer&);
};

#endif

#define synchronized(cs) Synchronizer __mskin_synchronizer__(cs)

NAMESPACE_MSKIN_END

#endif // __m_sync_h__
