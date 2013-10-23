
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

#ifndef ___3771df95_7343_4f81_b13f_f3bd69bbd5f8___
#define ___3771df95_7343_4f81_b13f_f3bd69bbd5f8___

#include "../Classes.SRC/_specific.h"

namespace teggo {

typedef int   (CXX_STDCALL *XHash_Cmpfn)(u32_t,u32_t);
typedef u32_t (CXX_STDCALL *XHash_Hashfn)(u32_t,u32_t);

extern u32_t CXX_STDCALL XHash_StringAhash(u32_t key,u32_t cnt);
extern u32_t CXX_STDCALL XHash_TrivialHash(u32_t key,u32_t cnt);
extern int   CXX_STDCALL XHash_StringAcmp(u32_t keya,u32_t keyb);

struct XHash
  {
    struct ROW_
      {
        void  *val;
        u32_t key;
        ROW_  *next;
        ROW_() { memset(this,0,sizeof(*this)); }
      };

    u32_t rows_number;
    u32_t iterRow;
    ROW_  **rows;
    ROW_  *iterItem;
    CRITICAL_SECTION locker_;

    _TEGGO_EXPORTABLE void Displace(void (*displace)(void*));

    _TEGGO_EXPORTABLE XHash(u32_t numbers);
    _TEGGO_EXPORTABLE ~XHash();

    _TEGGO_EXPORTABLE void  Put(u32_t key,void  *val);
    _TEGGO_EXPORTABLE void  *Get(u32_t key);
    _TEGGO_EXPORTABLE void  *Erase(u32_t key);
    _TEGGO_EXPORTABLE void  Put2(u32_t key,void *val,XHash_Cmpfn,XHash_Hashfn);
    _TEGGO_EXPORTABLE void  *Get2(u32_t key,XHash_Cmpfn,XHash_Hashfn);
    _TEGGO_EXPORTABLE void  *Erase2(u32_t key,XHash_Cmpfn,XHash_Hashfn);
    _TEGGO_EXPORTABLE void  **Ensure(u32_t key);
    _TEGGO_EXPORTABLE void  Reset();
    _TEGGO_EXPORTABLE bool  Next(u32_t *key,void **val);

    inline void SafePut(u32_t key,void *val)
      {
        EnterCriticalSection(&locker_);
        __try { Put(key,val); }
        __finally { LeaveCriticalSection(&locker_); }
      }

    inline void *SafeErase(u32_t key)
      {
        EnterCriticalSection(&locker_);
        __try { return Erase(key); }
        __finally { LeaveCriticalSection(&locker_); }
        return 0;
      }

    inline void *SafeGet(u32_t key)
      {
        EnterCriticalSection(&locker_);
        __try { return Get(key); }
        __finally { LeaveCriticalSection(&locker_); }
        return 0;
      }

    inline bool _Lock()
      {
        if ( this )
          return EnterCriticalSection(&this->locker_),true;
        else
          return false;
      }

    inline void _Unlock()
      {
        if ( this )
          LeaveCriticalSection(&this->locker_);
      }

  };

} // namespace

#if defined _TEGGOINLINE
#include "./xhash.inl"
#endif

#endif // ___3771df95_7343_4f81_b13f_f3bd69bbd5f8___
