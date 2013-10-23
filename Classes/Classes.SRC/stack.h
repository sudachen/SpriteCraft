
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

#if !defined __8A26ECE9_C113_4110_9252_CACCAD8E56AB__
#define __8A26ECE9_C113_4110_9252_CACCAD8E56AB__

#include "_specific.h"

namespace teggo
{

  template < class tTx, unsigned tCount, bool tCheckit = true >
    struct StackT
      {
        byte_t buffer_[sizeof(tTx)*tCount];
        unsigned p_;
        StackT() : p_(0) {}

        ~StackT()
          {
            while ( !Empty() ) Pop();
          }

        StackT& Push( const tTx& val )
          {
            if ( tCheckit )
              VIOLATION( ( p_ >= sizeof(buffer_) ),
                         L"stack overflow, size limit reached" );
            new (&buffer_[p_]) tTx(val);
            p_ += sizeof(tTx);
            return *this;
          }

        void Pop()
          {
            if ( tCheckit )
              VIOLATION( ( p_ == 0 ),
                         L"stack underflow" );
            p_ -= sizeof(tTx);
            ((tTx*)&buffer_[p_])->~tTx();
          }

        tTx& Top()
          {
            return *(tTx*)&buffer_[p_ - sizeof(tTx)];
          }

        tTx& operator *()
          {
            return Top();
          }

        tTx* operator ->()
          {
            return &Top();
          }

        bool Empty()
          {
            return p_ == 0;
          }

        unsigned Size()
          {
            return p_ / sizeof(tTx);
          }

        tTx& operator [](unsigned i)
          {
            return *(tTx*)&buffer_[sizeof(tTx)*i];
          }
      };

} // namespace

#endif
