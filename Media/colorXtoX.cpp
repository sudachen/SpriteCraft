
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

template < class tC0, class tC1 >
  void CXX_STDCALL TFF__CopyRowXX( tC0 const *from, tC1 *to, int cnt )
    {
      for ( int i = 0; i < cnt; ++i )
        *to++ = *from++;
    }

static inline void CXX_STDCALL TFF__CopyRowXX( rgba8_t const *from, rgba8_t *to, int cnt )
  {
    memcpy(to,from,cnt*4);
  }

static inline void CXX_STDCALL TFF__CopyRowXX( rgb5_t const *from, rgb5_t *to, int cnt )
  {
    memcpy(to,from,cnt*2);
  }

static inline void CXX_STDCALL TFF__CopyRowXX( rgb5a1_t const *from, rgb5a1_t *to, int cnt )
  {
    memcpy(to,from,cnt*2);
  }

template < class tT0, class tT1 >
  struct TFFt_CopyRowXX
    {
      static void CXX_STDCALL _( void const *from, void *to, int cnt )
        {
          TFF__CopyRowXX((tT0 const*)from,(tT1*)to,cnt);
        }
    };


template < class tT0 >
  struct TFFt_CopyRowXX_
    {
      static TFFt_CopyRowXtoX CXX_STDCALL _( int to_pf )
        {
          switch ( to_pf )
            {
              case TFF_PF_ALPHA:
                return &TFFt_CopyRowXX<tT0,rgba1_t>::_;
              case TFF_PF_RGB5A1:
                return &TFFt_CopyRowXX<tT0,rgb5a1_t>::_;
              case TFF_PF_R5G6B5:
                return &TFFt_CopyRowXX<tT0,r5g6b5_t>::_;
              case TFF_PF_RGB5:
                return &TFFt_CopyRowXX<tT0,rgb5_t>::_;
              case TFF_PF_RGB8:
                return &TFFt_CopyRowXX<tT0,rgb8_t>::_;
              case TFF_PF_RGBA8:
                return &TFFt_CopyRowXX<tT0,rgba8_t>::_;
              case TFF_PF_RGBA4:
                return &TFFt_CopyRowXX<tT0,rgba4_t>::_;
              case TFF_PF_RGB8 | TFF_PF_BGR:
                return &TFFt_CopyRowXX<tT0,bgr8_t>::_;
              case TFF_PF_RGBA8 | TFF_PF_BGR:
                return &TFFt_CopyRowXX<tT0,bgra8_t>::_;
              default:
                EMERGENCY(L"unsupported pixel format");
                return 0;
            }
        }
    };

extern "C" TFFt_CopyRowXtoX CXX_STDCALL TffmCopyRowXtoX(int from_pf, int to_pf)
  {
    switch ( from_pf )
      {
        case TFF_PF_ALPHA:
          switch ( to_pf )
            {
              case TFF_PF_ALPHA:
                return &TFFt_CopyRowXX<rgba1_t,rgba1_t>::_;
              case TFF_PF_RGB5A1:
                return &TFFt_CopyRowXX<rgba1_t,rgb5a1_t>::_;
              case TFF_PF_RGBA4:
                return &TFFt_CopyRowXX<rgba1_t,rgba4_t>::_;
              case TFF_PF_RGBA8 | TFF_PF_BGR:
                return &TFFt_CopyRowXX<rgba1_t,bgra8_t>::_;
            }
           break;
        case TFF_PF_RGB5A1:
          return TFFt_CopyRowXX_<rgb5a1_t>::_(to_pf);
        case TFF_PF_RGB8:
          return TFFt_CopyRowXX_<rgb8_t>::_(to_pf);
        case TFF_PF_RGB8 | TFF_PF_BGR:
          return TFFt_CopyRowXX_<bgr8_t>::_(to_pf);
        case TFF_PF_RGBA8 | TFF_PF_BGR:
          return TFFt_CopyRowXX_<bgra8_t>::_(to_pf);
        case TFF_PF_RGBA8:
          return TFFt_CopyRowXX_<rgba8_t>::_(to_pf);
        case TFF_PF_RGBA4:
          return TFFt_CopyRowXX_<rgba4_t>::_(to_pf);
        case TFF_PF_R5G6B5:
          return TFFt_CopyRowXX_<r5g6b5_t>::_(to_pf);
        case TFF_PF_R5G6B5 | TFF_PF_BGR:
          return TFFt_CopyRowXX_<b5g6r5_t>::_(to_pf);
        default:
          EMERGENCY(L"unsupported pixel format");
      }
    return 0;
  }
