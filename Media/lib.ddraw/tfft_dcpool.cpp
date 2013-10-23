
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

#include "../tffmedia.h"

#pragma code_seg(".text$tffmedia")
#include "./ddraw_inst.h"

TFFt_DCPOOL g_bitmap_dc_pool = TFFt_DCPOOL();

TFFt_DCPOOL::TFFt_DCPOOL()
  {
    memset(dc_pool2,0,sizeof(dc_pool2));
    memset(dc_pool1,0,sizeof(dc_pool1));
    marker = 0;
  }

TFFt_DCPOOL::~TFFt_DCPOOL()
  {
    UnpoolAll();
  }

HDC TFFt_DCPOOL::PoolDC(TFFt_DCPOOL_ELEMENT *_p)
  {
    if ( !_p->dcpool_element_hdc )
      {
        TFFt_DCPOOL_ELEMENT **pf = 0;
        for ( TFFt_DCPOOL_ELEMENT **p = dc_pool2, **pE = dc_pool2+POOL_SIZE; p != pE; ++p )
          {
            if ( !*p ) { pf = p; break; }
            else if ( !pf || (**p) < (**pf) ) 
              pf = p;
          }
        if ( *pf )
          (**pf).dcpool_element_hdc = 0;
        *pf = _p;
        _p->dcpool_element_hdc = dc_pool1 + (pf - dc_pool2);
        *_p->dcpool_element_hdc = _p->_aqcu_dc();
      }
    ++_p->dcpool_element_marker;
    return *_p->dcpool_element_hdc;
  }

void TFFt_DCPOOL::UnpoolDC(TFFt_DCPOOL_ELEMENT *_p)
  {
    if ( _p->dcpool_element_hdc )
      {
        int i = _p->dcpool_element_hdc - dc_pool1;
        REQUIRE( i >=0 && i < POOL_SIZE );
        REQUIRE( dc_pool2[i] == _p );
        dc_pool2[i]->_uget_dc(dc_pool1[i]);
        _p->dcpool_element_hdc = 0;
        dc_pool1[i] = 0;
        dc_pool2[i] = 0;        
      }
  }

void TFFt_DCPOOL::UpdateMarker(TFFt_DCPOOL_ELEMENT *p)
  {
    REQUIRE(p->dcpool_element_hdc);
    p->dcpool_element_marker = ++marker; 
  }

void TFFt_DCPOOL::UnpoolAll()
  {
    for ( int i = 0; i < POOL_SIZE; ++i )
      {
        if ( dc_pool2[i] ) 
          {
            REQUIRE(dc_pool2[i]->dcpool_element_hdc == dc_pool1+i);
            dc_pool2[i]->_uget_dc(dc_pool1[i]);
            dc_pool2[i]->dcpool_element_hdc = 0;
            dc_pool1[i] = 0;          
          }
        REQUIRE( dc_pool1[i] == 0 );
      }
    memset(dc_pool2,0,sizeof(dc_pool2));
    memset(dc_pool1,0,sizeof(dc_pool1));
    marker = 0;
  }
