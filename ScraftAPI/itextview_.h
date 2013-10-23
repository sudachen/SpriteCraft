
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

struct ScraftITextView
  {
    ITextView*     QueryIfs() { return (ITextView*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }
    float          GetScrollX();
    void           SetScrollX(float val);
    float          GetScrollY();
    void           SetScrollY(float val);
    float          ScrollMaxX();
    float          ScrollMaxY();
    float          GetWidth();
    void           SetWidth(float val);
    float          GetHeight();
    void           SetHeight(float val);

    struct Property_height   { TEGGO_PROPERTY(ScraftITextView,float,SetHeight,GetHeight,height) } height;
    struct Property_width    { TEGGO_PROPERTY(ScraftITextView,float,SetWidth,GetWidth,width) } width;
    struct Property_yScroll  { TEGGO_PROPERTY(ScraftITextView,float,SetScrollY,GetScrollY,yScroll) } yScroll;
    struct Property_xScroll  { TEGGO_PROPERTY(ScraftITextView,float,SetScrollX,GetScrollX,xScroll) } xScroll;
    struct Property_yMaxScroll { TEGGO_R_PROPERTY(ScraftITextView,float,ScrollMaxY,yScroll) } yMaxScroll;
    struct Property_xMaxScroll { TEGGO_R_PROPERTY(ScraftITextView,float,ScrollMaxX,xScroll) } xMaxScroll;

  };

inline ITextView* scraft_query_ifs(ScraftITextView* u)
  {
    return u->QueryIfs();
  }
