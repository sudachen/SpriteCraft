
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

#if !defined __50DFA54F_0876_4F8C_9226_0BD2D899B7D0__
#define __50DFA54F_0876_4F8C_9226_0BD2D899B7D0__

#include "_specific.h"
#include "guid.h"

namespace teggo
{

  struct CXX_NO_VTABLE Ireferred
    {
      virtual u32_t Release() = 0;
      virtual u32_t AddRef() = 0;
      virtual void *QueryInterface(guid_t const &guid) { return 0; }
      virtual ~Ireferred() {}
      //_TEGGO_EXPORTABLE virtual void Finalize();
//       template <class tTx>
//         tTx *Cast(Ireferred *p, tTx *_=0)
//           { return (tTx*)QueryInterface(TEGGO_GUIDOF(tTx)); }
    };

  template < class tTbase >
    struct RefcountedT : tTbase
      {
        u32_t Release()
          {
            if ( long refcount = InterlockedDecrement(&refcount_) )
              return refcount;
            else
              return (delete this), 0;
          }

        u32_t AddRef()
          {
            return InterlockedIncrement(&refcount_);
          }

        RefcountedT() : refcount_(1) {}
        u32_t _Refcount() { return refcount_; }

      protected:
        virtual ~RefcountedT() {}

      private:
        RefcountedT( const RefcountedT& );
        RefcountedT& operator =( const RefcountedT& );
        long refcount_;
      };

  typedef RefcountedT<Ireferred> Refcounted;

} // namespace

#endif // __50DFA54F_0876_4F8C_9226_0BD2D899B7D0__
