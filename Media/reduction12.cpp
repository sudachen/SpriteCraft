
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

#include "tffmedia.h"
#pragma code_seg(".text$tffmedia")

static inline void TFF__ForwardDistribute(int err, int *cerr, int &next)
  {
    next =  cerr[1] + err * 7 /16;
    cerr[-1] = cerr[-1] + err * 3 / 16;
    cerr[0]  = cerr[0]  + err * 5 / 16;
    cerr[1]  = cerr[1]  + err     / 16;
  }

static inline void TFF__BackwardDistribute(int err, int *cerr, int &next)
  {
    next =  cerr[-1] + err * 7 /16;
    cerr[1]  = cerr[1]  + err * 3 / 16;
    cerr[0]  = cerr[0]  + err * 5 / 16;
    cerr[-1] = cerr[-1] + err     / 16;
  }

static inline int TFF__c4( int c, int e )
  {
    c += e+8;

    if ( c  <= 0 )
      return 0;

    if ( c > 0x0ff )
      return 0x0f0;

    return c & 0x0f0;
  }

extern "C" void CXX_STDCALL TffmFrowardDiffuseDirection( rgba8_t *row, int *err, int width)
  {
    int *err_r = err + 1;
    int *err_g = err + width+2 + 1;
    int *err_b = err + (width+2)*2 + 1;

    int r_e = err_r[0];
    int g_e = err_g[0];
    int b_e = err_b[0];

    for ( int i = 0; i < width ; ++i )
      {
        int r = row[i].r;
        int g = row[i].g;
        int b = row[i].b;

        int rm = TFF__c4(r,r_e);
        int gm = TFF__c4(g,g_e);
        int bm = TFF__c4(b,b_e);

        TFF__ForwardDistribute( r - rm, err_r + i, r_e );
        TFF__ForwardDistribute( g - gm, err_g + i, g_e );
        TFF__ForwardDistribute( b - bm, err_b + i, b_e );

        row[i].r = rm;
        row[i].g = gm;
        row[i].b = bm;
      }
  }

extern "C"  void CXX_STDCALL TffmBackwardDiffuseDirection( rgba8_t *row, int *err, int width )
  {
    int *err_r = err + 1;
    int *err_g = err + width+2 + 1;
    int *err_b = err + (width+2)*2 + 1;

    int r_e = err_r[width-1];
    int g_e = err_g[width-1];
    int b_e = err_b[width-1];

    for ( int i = width-1; i >= 0 ; --i )
      {
        int r = row[i].r;
        int g = row[i].g;
        int b = row[i].b;

        int rm = TFF__c4(r,r_e);
        int gm = TFF__c4(g,g_e);
        int bm = TFF__c4(b,b_e);

        TFF__BackwardDistribute( r - rm, err_r + i, r_e );
        TFF__BackwardDistribute( g - gm, err_g + i, g_e );
        TFF__BackwardDistribute( b - bm, err_b + i, b_e );

        row[i].r = rm;
        row[i].g = gm;
        row[i].b = bm;
      }
  }


extern "C" void CXX_STDCALL TffmDiffusionColorReduction12( rgba8_t *row, int *err, int width, bool forward )
  {
    if ( forward )
      TffmFrowardDiffuseDirection(row,err,width);
    else
      TffmBackwardDiffuseDirection(row,err,width);
  }
