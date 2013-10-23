
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

#include "../Classes.SRC/_specific.h"

#if !defined __6F23C0B_148C_48F7_9C47_81E6697C8968__
#define __6F23C0B_148C_48F7_9C47_81E6697C8968__

namespace teggo
{

  template<class tTx> struct e_iterator_impl;

  template<class tTx>
    struct e_iterator
      {
        mutable tTx* val_;
        virtual bool Next() const = 0;
        tTx* operator-> () const { return val_; }
        tTx& operator*  () const { return *val_; }
        template<class U> bool Next(U& u) const
          {
            return reinterpret_cast<const e_iterator_impl<U>*>(this)->e_iterator_impl<U>::Next();
          }
        typedef e_iterator<tTx> const &_;
      protected:
        virtual ~e_iterator(){}
      };

  template<class tTx>
    struct e_iterator_impl : public e_iterator<typename tTx::value_type>
      {
        mutable typename tTx::iterator i_;
        mutable typename tTx::iterator iE_;

        virtual bool Next() const
          {
            return (i_ == iE_) ? false : (val_ = &*i_, ++i_, true);
          }

        e_iterator_impl(tTx& c) {
          val_ = 0;
          i_ = c.begin();
          iE_ = c.end();
        }
      };

  template<class tTx> e_iterator_impl<tTx> e_iterate(tTx& c)
    {
      return e_iterator_impl<tTx>(c);
    }

}

#endif // __6F23C0B_148C_48F7_9C47_81E6697C8968__
