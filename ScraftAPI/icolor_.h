
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

struct CXX_NO_VTABLE ScraftIColor
  {
    IRGBFilter*    QueryIfs() { return (IRGBFilter*)this; }

    u32_t          AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t          Release()  { return ((IUnknown*)this)->Release(); }

    void           SetRed(long);
    long           GetRed();
    void           SetGreen(long);
    long           GetGreen();
    void           SetBlue(long);
    long           GetBlue();
    void           SetColor(u32_t);
    u32_t          GetColor();

    struct Property_r {TEGGO_PROPERTY(ScraftIColor,long,SetRed,GetRed,r)} r;
    struct Property_g {TEGGO_PROPERTY(ScraftIColor,long,SetGreen,GetGreen,g)} g;
    struct Property_b {TEGGO_PROPERTY(ScraftIColor,long,SetBlue,GetBlue,b)} b;
    struct Property_color {TEGGO_PROPERTY(ScraftIColor,u32_t,SetColor,GetColor,color)} color;
  };

inline IRGBFilter* scraft_query_ifs(ScraftIColor* u)
  {
    return u->QueryIfs();
  }
