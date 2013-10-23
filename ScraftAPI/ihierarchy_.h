
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

struct CXX_NO_VTABLE ScraftIHierarchy
  {
    IHierarchy*    QueryIfs() { return (IHierarchy*)this; }
    u32_t          AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t          Release()  { return ((IUnknown*)this)->Release(); }

    void           SetX(bool);
    bool           GetX();
    void           SetY(bool);
    bool           GetY();
    void           SetAngle(bool);
    bool           GetAngle();
    void           SetLuminance(bool);
    bool           GetLuminance();
    void           SetTransparency(bool);
    bool           GetTransparency();
    void           SetColor(bool);
    bool           GetColor();
    void           SetScaleX(bool);
    bool           GetScaleX();
    void           SetScaleY(bool);
    bool           GetScaleY();
    void           SetVisibility(bool);
    bool           GetVisibility();
    void           SetParentDispose(bool);
    bool           GetParentDispose();

    struct Property_x         { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetX,GetX,x) } x;
    struct Property_y         { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetY,GetY,y) } y;
    struct Property_angle     { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetAngle,GetAngle,angle) } angle;
    struct Property_luminance { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetLuminance,GetLuminance,luminance) } luminance;
    struct Property_cfilt     { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetColor,GetColor,cfilt) } cfilt;
    struct Property_xScale    { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetScaleX,GetScaleX,xScale) } xScale;
    struct Property_yScale    { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetScaleY,GetScaleY,yScale) } yScale;
    struct Property_visibility { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetVisibility,GetVisibility,visibility) } visibility;
    struct Property_transparency { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetTransparency,GetTransparency,transparency) } transparency;
    struct Property_parentDispose { TEGGO_PROPERTY(ScraftIHierarchy,bool,SetParentDispose,GetParentDispose,parentDispose) } parentDispose;
  };

inline IHierarchy* scraft_query_ifs(ScraftIHierarchy* u)
  {
    return u->QueryIfs();
  }
