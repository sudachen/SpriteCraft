
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

#if !defined ___8f601429_033d_46af_b9e3_f2989df017d8___
#define ___8f601429_033d_46af_b9e3_f2989df017d8___

#include "collection.h"

#if defined _TEGGO_COLLECTION_HERE
# define _TEGGO_COLLECTION_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_COLLECTION_FAKE_INLINE CXX_FAKE_INLINE
#endif

namespace teggo {

  _TEGGO_COLLECTION_FAKE_INLINE
  void* CollectionBasic::Get(pwide_t key) const
    {
      if ( !key ) return 0;
      unsigned hascode = HashCode(key);
      CollectionT_Record *rec = table_[hascode%mod_];
      while ( rec ) {
        if ( hascode == rec->hascode && KeyEqual(pwide_t((char*)rec+sizeOfRec_),key) )
          return rec+1;
        rec = rec->next;
      }
      return 0;
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  void* CollectionBasic::Erase(pwide_t key)
    {
      if ( !key ) return 0;
      unsigned hascode = HashCode(key);
      CollectionT_Record **rec = &table_[hascode%mod_];
      while ( *rec )
        {
          if ( hascode == (*rec)->hascode && KeyEqual(pwide_t((char*)*rec+sizeOfRec_),key) )
            {
              CollectionT_Record* r = *rec;
              *rec = r->next;
              r->next = 0;
              --count_;
              return r+1;
            }
          rec = &(*rec)->next;
        }
      return 0;
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  void CollectionBasic::Erase(void *mem)
    {
      CollectionT_Record  *memrec = (CollectionT_Record*)mem-1;
      CollectionT_Record **rec = &table_[memrec->hascode%mod_];
      while ( *rec )
        {
          if ( *rec == memrec )
            {
              *rec = memrec->next;
              memrec->next = 0;
              --count_;
              return;
            }
          rec = &(*rec)->next;
        }
      REQUIRE(!"element is not in hashmap");
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  bool CollectionBasic::EnsurePresent(pwide_t key,void **mem)
    {
      unsigned hascode = HashCode(key);
      CollectionT_Record **nrec = &table_[hascode%mod_];
      CollectionT_Record  *rec = *nrec;

      while ( rec )
        {
          if ( hascode == rec->hascode && KeyEqual(pwide_t((char*)rec+sizeOfRec_),key) )
            {
              *mem = rec+1;
              return true;
            }
          rec = rec->next;
        }

      if ( count_*100 >= mod_*90 )
        {
          unsigned newmod = mod_*2 + 1;
          while ( newmod < count_*100/90 ) newmod = newmod*2 + 1;
          Rehash(newmod);
          nrec = &table_[hascode%mod_];
          rec = *nrec;
        }

      unsigned slen = wcslen(key);
      rec = (CollectionT_Record*)new byte_t[sizeOfRec_+(slen+1)*sizeof(wchar_t)];

      rec->hascode = hascode;
      rec->next = *nrec;
      memcpy((byte_t*)rec+sizeOfRec_,key,(slen+1)*sizeof(wchar_t));
      *nrec = rec;
      *mem = rec+1;
      ++count_;

      return false;
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  void CollectionBasic::DisposeValue(void *mem)
    {
      if ( mem )
        {
          delete[] (byte_t*)((CollectionT_Record*)mem - 1);
        }
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  void* CollectionBasic::NextValue(CollectionT_IRecord *iter) const
    {
      if ( !table_ ) return 0;

      if ( iter->row == table_ + mod_ ) return 0;

      if ( !iter->row )
        {
          iter->row = table_;
          iter->col = *iter->row;
      lb_find_row:
          while ( !iter->col )
            {
              ++iter->row;
              if ( iter->row == table_ + mod_ ) return 0;
              iter->col = *iter->row;
            }
        }
      else
        {
          if ( iter->col )
            iter->col = iter->col->next;
          else
            iter->col = *iter->row;
          goto lb_find_row; // find next row if !iter->col
        }

      return iter->col+1;
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  pwide_t CollectionBasic::NextKey(CollectionT_IRecord *iter) const
    {
      void *val = NextValue(iter);
      if ( val )
        return pwide_t((char*)((CollectionT_Record*)val-1)+sizeOfRec_);
      return 0;
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  void CollectionBasic::Clear()
    {
      for ( unsigned i = 0; i < mod_; ++i )
        {
          CollectionT_Record *rec = table_[i];
          while ( rec )
            {
              CollectionT_Record *r = rec->next;
              delete[] (byte_t*)rec;
              rec = r;
            }
        }
      memset(table_,0,sizeof(CollectionT_Record*)*mod_);
      count_ = 0;
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  void CollectionBasic::Rehash(unsigned newmod)
    {
      CollectionT_Record **table = new CollectionT_Record*[newmod];
      memset(table,0,newmod*sizeof(CollectionT_Record*));
      if ( table_ )
        {
          for ( unsigned i = 0; i < mod_; ++i )
            {
               CollectionT_Record *rec = table_[i];
               while ( rec )
                 {
                   CollectionT_Record* r= rec->next;
                   unsigned j = rec->hascode % newmod;
                   rec->next = table[j];
                   table[j] = rec;
                   rec = r;
                 }
            }
          delete[] table_;
        }
      table_ = table;
      mod_ = newmod;
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  CollectionBasic::CollectionBasic(unsigned preserve,unsigned sizeOfT)
    {
      sizeOfRec_ = sizeOfT+sizeof(CollectionT_Record);
      count_ = 0;
      table_ = 0;
      mod_   = 0;
      Rehash(preserve?preserve:13);
    }

  _TEGGO_COLLECTION_FAKE_INLINE
  CollectionBasic::~CollectionBasic()
    {
      delete[] table_;
    }

} // namespace

#undef _TEGGO_COLLECTION_FAKE_INLINE

#endif // ___8f601429_033d_46af_b9e3_f2989df017d8___
