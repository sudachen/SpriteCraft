
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
     
#include <(aux)/scraft1.h>
#include "./xdata_cow.h"

XDATA_COW_HOLDER_::~XDATA_COW_HOLDER_() 
  { 
    // root node is destroing
    // all subnodes are destroyed now
    //UnlinkAll(false); // do not modify disposed memory
    REQUIRE( l_ == 0 );
    REQUIRE( *holder_ = this );
    *holder_ = 0; 
  }

void XDATA_COW_HOLDER_::UnlinkAll(bool reset_holders)
  {
    while (l_)
      {
        UNKNOWN_XDATA_COW_ *l = l_;
        l->LinkOut();
        if ( reset_holders )
          {
            REQUIRE( *l->holder_ == l );
            *l->holder_ = 0;
          }
        l->holder_ = 0;
        Unrefe(l);
      }
  }

void XDATA_COW_HOLDER_::Link(UNKNOWN_XDATA_COW_ *p)
  {
    REQUIRE( p->holder_ != 0 );
    p->LinkIn(&l_);
  }

void XDATA_COW_HOLDER_::OnModify(Xnode *, u32_t, pwide_t)
  {
    UnlinkAll();
  }
