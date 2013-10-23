
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

#ifndef ___63e88d75_0660_4f62_821f_e48a59d80947___
#define ___63e88d75_0660_4f62_821f_e48a59d80947___

extern CRITICAL_SECTION *global_critical_section;

struct GLOBAL_CRITICAL_SECTION_LOCKER
  {
    GLOBAL_CRITICAL_SECTION_LOCKER(int)
      { EnterCriticalSection(global_critical_section); }
    ~GLOBAL_CRITICAL_SECTION_LOCKER()
      { LeaveCriticalSection(global_critical_section); }
    operator bool() const { return false; }
  };

#define __synchronize__ \
  if ( GLOBAL_CRITICAL_SECTION_LOCKER __sync_locker_l = GLOBAL_CRITICAL_SECTION_LOCKER(0) ) \
    goto TEGGO_LABEL(__LINE__); else TEGGO_LABEL(__LINE__):

#define _SYNCHRONIZE_ GLOBAL_CRITICAL_SECTION_LOCKER ___locker(0)

#endif // ___63e88d75_0660_4f62_821f_e48a59d80947___
