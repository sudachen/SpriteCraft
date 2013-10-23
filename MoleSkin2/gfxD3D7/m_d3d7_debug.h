
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


#if !defined __m_d3d7_debug_h__
#define __m_d3d7_debug_h__

#include "../mcore/m_debug.h"

namespace gfxD3D7 {

extern const pwide_t D3D7_x_s_AT_s_d;
extern const pwide_t D3D7_herr_x;

#define fail_if_dxerr(expr) \
do { HRESULT dxREQUIRE___hr = expr; \
  if ( dxREQUIRE___hr!=0) {\
    Lerr << _S*D3D7_x_s_AT_s_d %dxREQUIRE___hr %#expr %__FILE__ %__LINE__;\
    __debug_break__(); \
    return SCE_FAIL;}\
}while(0)

#define warn_if_dxerr(expr) \
do { HRESULT dxREQUIRE___hr = expr; \
  if ( dxREQUIRE___hr!=0) {\
    Lout << _S*D3D7_x_s_AT_s_d %dxREQUIRE___hr %#expr %__FILE__ %__LINE__;\
  }\
}while(0)

#define ret0_if_dxerr(expr) \
do { HRESULT dxREQUIRE___hr = expr; \
  if ( dxREQUIRE___hr!=0) {\
    Lerr << _S*D3D7_x_s_AT_s_d %dxREQUIRE___hr %#expr %__FILE__ %__LINE__;\
    __debug_break__(); \
    return 0;}\
}while(0)

#define retval_if_dxerr(expr,val) \
do { HRESULT dxREQUIRE___hr = expr; \
  if ( dxREQUIRE___hr!=0) {\
    Lerr << _S*D3D7_x_s_AT_s_d %dxREQUIRE___hr %#expr %__FILE__ %__LINE__;\
    __debug_break__(); \
    return val;}\
}while(0)

#define return_if_dxerr(expr) \
do { HRESULT dxREQUIRE___hr = expr; \
  if ( dxREQUIRE___hr!=0) {\
    Lerr << _S*D3D7_x_s_AT_s_d %dxREQUIRE___hr %#expr %__FILE__ %__LINE__;\
    __debug_break__(); \
    return;}\
}while(0)

} // namespace

#endif //__m_d3d7_debug_h__

