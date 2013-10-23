
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

#if !defined ___CD4E5B55_0248_456a_A23F_F3B50A68DEFE___
#define ___CD4E5B55_0248_456a_A23F_F3B50A68DEFE___

#include "_specific.h"
#include "_crc32.h"
#include "string.h"

namespace teggo
{

  struct CollectionT_Record
    {
      unsigned hascode;
      CollectionT_Record *next;
    };

  struct CollectionT_IRecord
    {
      CollectionT_Record **row;
      CollectionT_Record *col;
    };

  struct CollectionBasic
    {
      enum { PRECLEANED = 0x7fffffff };
      unsigned sizeOfRec_;
      unsigned count_;
      unsigned mod_;
      CollectionT_Record** table_;
      _TEGGO_EXPORTABLE void* Get(pwide_t) const;
      _TEGGO_EXPORTABLE void* Erase(pwide_t);
      _TEGGO_EXPORTABLE void  Erase(void*);
      _TEGGO_EXPORTABLE bool EnsurePresent(pwide_t key,void** mem);
      _TEGGO_EXPORTABLE void DisposeValue(void*);
      _TEGGO_EXPORTABLE void* NextValue(CollectionT_IRecord*) const;
      _TEGGO_EXPORTABLE pwide_t NextKey(CollectionT_IRecord*) const;
      _TEGGO_EXPORTABLE bool IncrementIter(CollectionT_IRecord*) const;
      _TEGGO_EXPORTABLE void Clear();
      _TEGGO_EXPORTABLE void Rehash(unsigned newmod);
      _TEGGO_EXPORTABLE CollectionBasic(unsigned preserve,unsigned sizeOfT);
      _TEGGO_EXPORTABLE ~CollectionBasic();
      unsigned Count() const { return count_; }

      static bool CXX_STDCALL KeyEqual(pwide_t keyA,pwide_t keyB) { return wcscmp(keyA,keyB) == 0; }
      static unsigned CXX_STDCALL HashCode(pwide_t key) { return Crc32(0,(const byte_t*)key,wcslen(key)*sizeof(wchar_t)); }

      void Swap(CollectionBasic &c)
        {
          teggo_swap(sizeOfRec_,c.sizeOfRec_);
          teggo_swap(count_,c.count_);
          teggo_swap(mod_,c.mod_);
          teggo_swap(table_,c.table_);
        }

    };

  template <class tTx>
    struct CollectionIteratorT
      {
        CollectionBasic const *db_;
        CollectionT_IRecord iter;
        pwide_t Key() const;
        tTx* val_;

        tTx& Value()
          {
            return *val_;
          }

        tTx* Next()
          {
            return db_ ? (val_ = (tTx*)db_->NextValue(&iter)) : 0;
          }

        bool Reset()
          {
            iter.row = 0;
            val_ = 0;
            return db_ ? db_->Count() != 0 : false;
          }

        CollectionIteratorT(CollectionBasic* db) : db_(db)
          {
            iter.row = 0;
            val_ = 0;
          }

        tTx& operator*()
          {
            return *val_;
          }

        tTx* operator->()
          {
            return val_;
          }

        operator bool () const
          {
            return db_ ? db_->Count() != 0 : false;
          }
      };

  template<class tTx>
    struct CollectionT : private CollectionBasic
      {
        typedef CollectionIteratorT<tTx> Iterator;
        typedef tTx Value;

        CollectionT(int preserve=13);
        ~CollectionT();

        void Swap(CollectionT &c)
          {
            CollectionBasic::Swap(c);
          }

        tTx* Get(pwide_t key) const
          {
            return (tTx*)CollectionBasic::Get(key);
          }

        tTx* GetLower(pwide_t key) const
          {
            return Get(StringT<wchar_t,32>(key).ToLower().Str());
          }

        tTx& Put(pwide_t key,const tTx& value)
          {
            REQUIRE(count_ != PRECLEANED);
            tTx* t;
            if ( CollectionBasic::EnsurePresent(key,(void**)&t) )
              {
                if ( t == &value ) return *t;
                t->~tTx();
              }
            return *(new(t) tTx(value));
          }

        tTx& PutLower(pwide_t key,const tTx& value)
          {
            return Put(StringT<wchar_t,32>(key).ToLower().Str(),value);
          }

        tTx& Rec(pwide_t key)
          {
            REQUIRE(count_ != PRECLEANED);
            tTx* t;
            if ( !CollectionBasic::EnsurePresent(key,(void**)&t) )
              new(t) tTx;
            return *t;
          }

        void Erase(pwide_t key)
          {
            if ( count_ == PRECLEANED ) return;

            tTx* t = (tTx*) CollectionBasic::Erase(key);
            if ( t )
              {
                t->~tTx();
                CollectionBasic::DisposeValue(t);
              }
          }

        void EraseLower(pwide_t key)
          {
            Erase(StringT<wchar_t,32>(key).ToLower().Str());
          }

        void Erase(tTx* t)
          {
            if ( count_ == PRECLEANED ) return;

            CollectionBasic::Erase((void*)t);
            t->~tTx();
            CollectionBasic::DisposeValue(t);
          }

        tTx& operator[] ( pwide_t key )
          {
            return Rec(key);
          }

        void Clear() ;

        Iterator Iterate() const
          {
            return Iterator((CollectionBasic*)this);
          }

        static pwide_t CXX_STDCALL KeyOf(const tTx* t)
          {
            return pwide_t((char*)((CollectionT_Record*)t-1)+sizeof(CollectionT_Record)+sizeof(tTx));
          }

        using CollectionBasic::Count;

      private:

        CollectionT(CollectionT const&);
        CollectionT& operator =(CollectionT const&);

      };

  template < class tTx >
    CollectionT<tTx>::CollectionT(int preserve) : CollectionBasic(preserve,sizeof(tTx))
      {
      }

  template < class tTx >
    CollectionT<tTx>::~CollectionT()
      {
        Clear();
      }

  template < class tTx >
    void CollectionT<tTx>::Clear()
      {
        CollectionBasic::count_ = PRECLEANED;
        tTx* t;
        CollectionT_IRecord iter = { 0,0 };
        while ( (t=(tTx*)CollectionBasic::NextValue(&iter)) != 0 )
          t->~tTx();
        CollectionBasic::count_ = 0;
        CollectionBasic::Clear();
      }

  template<class tTx>
    pwide_t CollectionIteratorT<tTx>::Key() const
      {
        return CollectionT<tTx>::KeyOf(val_);
      }

} // namespace teggo

template < class tTx > inline
  void CXX_STDCALL teggo_swap(teggo::CollectionT<tTx> &s0, teggo::CollectionT<tTx> &s1)
    {
      s0.Swap(s1);
    }

#if defined _TEGGOINLINE
#include "collection.inl"
#endif

#endif // ___CD4E5B55_0248_456a_A23F_F3B50A68DEFE___
