
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

#ifndef ___11674425_dfd2_435f_9233_52e050838adf___
#define ___11674425_dfd2_435f_9233_52e050838adf___

#include "./xhash.h"

#if !defined _TEGGO_XHASH_HERE
# define _TEGGO_XHASH_FAKE_INLINE  CXX_FAKE_INLINE
#else
# define _TEGGO_XHASH_FAKE_INLINE  _TEGGO_EXPORTABLE
#endif

namespace teggo {

  _TEGGO_XHASH_FAKE_INLINE
  XHash::XHash(u32_t rows_number) : rows_number(rows_number)
    {
      this->iterItem = 0;
      rows = new ROW_*[rows_number];
      memset(rows,0,sizeof(ROW_*)*rows_number);
      InitializeCriticalSection(&locker_);
    }

  _TEGGO_XHASH_FAKE_INLINE
  void XHash::Displace(void (*displace)(void*))
    {
      for ( u32_t i = 0 ; i < rows_number; ++i )
        {
          ROW_* row = rows[i];
          while ( row != 0 )
            {
              if (displace) 
                displace(row->val);

              ROW_* r = row;
              row = row->next;
              delete r;
            }
        }
    }

  _TEGGO_XHASH_FAKE_INLINE
  XHash::~XHash()
    {
      Displace(0);
      delete rows;
      rows = 0;
      DeleteCriticalSection(&locker_);
    }

  _TEGGO_XHASH_FAKE_INLINE
  int __stdcall XXHash_eq__(u32_t a,u32_t b)
    {
      return a == b ? 0 : 1;
    }

  _TEGGO_XHASH_FAKE_INLINE
  void *XHash::Get(u32_t key)
    {
      return Get2(key,&XXHash_eq__,&XHash_TrivialHash);
    }

  _TEGGO_XHASH_FAKE_INLINE
  void *XHash::Get2(u32_t key,XHash_Cmpfn f,XHash_Hashfn hf)
    {
      ROW_ *row = rows[hf(key,rows_number)];
      while ( row != 0 )
        {
          if ( f(row->key,key) == 0 ) return row->val;
          row = row->next;
        }
      return 0;
    }

  _TEGGO_XHASH_FAKE_INLINE
  void *XHash::Erase(u32_t key)
    {
      return Erase2(key,&XXHash_eq__,&XHash_TrivialHash);
    }

  _TEGGO_XHASH_FAKE_INLINE
  void *XHash::Erase2(u32_t key,XHash_Cmpfn f,XHash_Hashfn hf)
    {
      u32_t hash = hf(key,rows_number);
      ROW_ *row = rows[hash];
      ROW_ **rowP = &rows[hash];
      while ( row != 0 )
        {
          if ( f(row->key,key) == 0 )
            {
              *rowP = row->next;
              void* v = row->val;
              delete row;
              return v;
            }
          rowP = &row->next;
          row = row->next;
        }
      return 0;
    }

  _TEGGO_XHASH_FAKE_INLINE
  void XHash::Put(u32_t key,void *val)
    {
      Put2(key,val,&XXHash_eq__,&XHash_TrivialHash);
    }

  _TEGGO_XHASH_FAKE_INLINE
  void XHash::Put2(u32_t key,void *val,XHash_Cmpfn f,XHash_Hashfn hf)
    {
      u32_t hash = hf(key,rows_number);
      ROW_ *row = rows[hash];
      while ( row != 0 )
        {
          if ( f(row->key,key) == 0 )
            {
              row->val = val;
              return;
            }
          row = row->next;
        }
      row = new ROW_();
      row->key = key;
      row->val = val;
      row->next = rows[hash];
      rows[hash] = row;
    }

void **XHash::Ensure(u32_t key)
  {
    ROW_ *row = rows[key%rows_number];
    while ( row != 0 )
      {
        if ( row->key == key )
          {
            return &row->val;
          }
        row = row->next;
      }
    row = new ROW_();
    row->key = key;
    row->val = 0;
    row->next = (ROW_*)rows[key%rows_number];
    rows[key%rows_number] = row;
    return &row->val;
  }

  _TEGGO_XHASH_FAKE_INLINE
  void XHash::Reset()
    {
      iterRow = 0;
      iterItem = 0;
    }

  _TEGGO_XHASH_FAKE_INLINE
  bool XHash::Next(u32_t *key,void **val)
    {
    lbl:    
      if ( iterRow >= rows_number ) return false;
      if ( iterItem == 0 ) 
        {
          iterItem = rows[iterRow];
        }
      else
        {
          iterItem = iterItem->next;
        }
      if ( iterItem == 0 ) 
        {
          ++iterRow;
          goto lbl;
        }
      *key = iterItem->key;
      *val = iterItem->val;
      return true;
    }

  _TEGGO_XHASH_FAKE_INLINE
  u32_t CXX_STDCALL XHash_TrivialHash(u32_t key,u32_t rows_number)
    {
      return key%rows_number;
    }

  _TEGGO_XHASH_FAKE_INLINE
  u32_t CXX_STDCALL XHash_StringAhash(u32_t key,u32_t rows_number)
    {
      u32_t h = Shash((char*)key);
      return h%rows_number;
    }

  _TEGGO_XHASH_FAKE_INLINE
  int CXX_STDCALL XHash_StringAcmp(u32_t keya,u32_t keyb)
    {
      return strcmp((char*)keya,(char*)keyb);
    }
}

#endif // ___11674425_dfd2_435f_9233_52e050838adf___
