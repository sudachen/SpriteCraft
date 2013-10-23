
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

struct CXX_NO_VTABLE ScraftISprite
  {

    ISprite*       QueryIfs() { return (ISprite*)this; }
    ScraftSprite*  QueryScraftSprite();

    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }

    void           Dispose();
    bool           Equal(const ISpritePtr&);
    bool           Contains(float x,float y);
    void           MoveTo(float x,float y);
    long           GetAttr(long no);
    void           SetAttr(long no,long val);
    __ScString     GetName();
    __ScString     GetKlass();
    pwide_t        GetNameAsPWIDE();
    pwide_t        GetKlassAsPWIDE();
    long           GetLayer();
    void           SetLayer(long);
    long           GetParalax();
    void           SetParalax(long);
    long           GetSubLayer();
    void           SetSubLayer(long);
    float          GetX();
    void           SetX(float);
    float          GetY();
    void           SetY(float);
    float          GetDegree();
    void           SetDegree(float);
    float          GetAngle();
    void           SetAngle(float);
    void           SetFrame(long frame,pwide_t klass = 0);
    long           GetFrame();
    bool           IsVisible();
    void           SetVisible(bool);
    float          GetHeight();
    float          GetWidth();
    float          GetSizeX();
    float          GetSizeY();
    float          GetScaleX();
    float          GetScaleY();
    void           SetSizeX(float);
    void           SetSizeY(float);
    void           SetScaleX(float);
    void           SetScaleY(float);
    void           SetScale(float);
    float          GetTransparency();
    void           SetTransparency(float);
    float          GetLuminance();
    void           SetLuminance(float);
    bool           IsMouseOver();
    void           SetHotSpot(tagHotspot);
    float          GetHotSpotX();
    float          GetHotSpotY();
    void           SetHotSpotX(float);
    void           SetHotSpotY(float);
    bool           IsFlippedByX();
    bool           IsFlippedByY();
    void           SetFlippedByX(bool);
    void           SetFlippedByY(bool);
    void           SetText(pwide_t);
    IColorPtr      Color();
    void           SetParent(const ISpritePtr&);
    void           SetParentNothing();
    ISpritePtr     GetParent();
    IHierarchyPtr  GetHierarchy();
    float          GetScrX();
    float          GetScrY();
    float          GetScrAngle();
    long           GetCollideGroup();
    void           SetCollideGroup(long);
    bool           IsAlive();
    float          GetRandom();
    long           FrCount();
    long           GetCookie();
    void           SetCookie(long);
    bool           IsOutOfScreen();
    bool           Hit(const ISpritePtr&);
    void           Randomize();
    float          Distance(const ISpritePtr&);
    float          AngleWith(const ISpritePtr&);
    void           IncAngle(float);
    void           IncX(float);
    void           IncY(float);
    void           SpriteToScreen(float* x,float* y);
    void           ScreenToSprite(float* x,float* y);
    void           AnimateOnce(float fps,long startFrno = 0,long stopFrno = -1,IUnknown* listener = 0,long cookie = 0);
    void           AnimateLoop(float fps,long startFrno = 0,long stopFrno = -1,IUnknown* listener = 0,long cookie = 0);
    void           StopAnimation();
    void           AttachChild(pwide_t key,const ISpritePtr&);
    ISpritePtr     GetChild(pwide_t key);
    __ScString     GetText();
    void           SetSubImpl(IUnknown* iunk) { SetSubImpl(teggo::RccRefe(iunk)); }
    void           SetSubImpl(const IUnknownPtr&);
    IUnknownPtr    GetSubImpl();
    void           SetStyle(tagStyle atyle);
    tagStyle       GetStyle();
    ILwsArrayPtr   GetLwsArray();
    ITextViewPtr   GetTextView();
    ISpritePrimitivePtr GetPrimitive();

    void GetRect( float *l, float *t, float *r, float *b )
    {
      *l = GetX();
      *t = GetY();
      *r = *l + GetSizeX();
      *b = *t + GetSizeY();
    }

    struct Property_name {TEGGO_R_PROPERTY(ScraftISprite,__ScString,GetName,name)};
    struct Property_klass {TEGGO_R_PROPERTY(ScraftISprite,__ScString,GetKlass,klass)};
    struct Property_layer {TEGGO_PROPERTY(ScraftISprite,long,SetLayer,GetLayer,layer)};
    struct Property_paralax {TEGGO_PROPERTY(ScraftISprite,long,SetParalax,GetParalax,paralax)};
    struct Property_x {TEGGO_PROPERTY(ScraftISprite,float,SetX,GetX,x)};
    struct Property_y  {TEGGO_PROPERTY(ScraftISprite,float,SetY,GetY,y)};
    struct Property_degree {TEGGO_PROPERTY(ScraftISprite,float,SetDegree,GetDegree,degree)};
    struct Property_angle {TEGGO_PROPERTY(ScraftISprite,float,SetAngle,GetAngle,angle)};
    struct Property_frno {TEGGO_PROPERTY(ScraftISprite,long,SetFrame,GetFrame,frno)};
    struct Property_visible  {TEGGO_PROPERTY(ScraftISprite,bool,SetVisible,IsVisible,visible)};
    struct Property_height {TEGGO_R_PROPERTY(ScraftISprite,float,GetHeight,height)};
    struct Property_width {TEGGO_R_PROPERTY(ScraftISprite,float,GetWidth,width)};
    struct Property_transparency {TEGGO_PROPERTY(ScraftISprite,float,SetTransparency,GetTransparency,transparency)};
    struct Property_luminance {TEGGO_PROPERTY(ScraftISprite,float,SetLuminance,GetLuminance,luminance)};
    struct Property_mouseOver {TEGGO_R_PROPERTY(ScraftISprite,bool,IsMouseOver,mouseOver)};
    struct Property_xScale {TEGGO_PROPERTY(ScraftISprite,float,SetScaleX,GetScaleX,xScale)};
    struct Property_yScale {TEGGO_PROPERTY(ScraftISprite,float,SetScaleY,GetScaleY,yScale)};
    struct Property_xyScale {TEGGO_W_PROPERTY(ScraftISprite,float,SetScale,xyScale)};
    struct Property_xSize {TEGGO_PROPERTY(ScraftISprite,float,SetSizeX,GetSizeX,xSize)};
    struct Property_ySize {TEGGO_PROPERTY(ScraftISprite,float,SetSizeY,GetSizeY,ySize)};
    struct Property_xHotspot {TEGGO_PROPERTY(ScraftISprite,float,SetHotSpotX,GetHotSpotX,xHotspot)};
    struct Property_yHotspot {TEGGO_PROPERTY(ScraftISprite,float,SetHotSpotY,GetHotSpotY,yHotspot)};
    struct Property_hotspot  {TEGGO_W_PROPERTY(ScraftISprite,tagHotspot,SetHotSpot,hotspot)};
    struct Property_cfilt {TEGGO_R_PROPERTY_ARROW(ScraftISprite,IColorPtr,Color,cfilt)};
    struct Property_xFlip {TEGGO_PROPERTY(ScraftISprite,bool,SetFlippedByX,IsFlippedByX,xFlip)};
    struct Property_yFlip {TEGGO_PROPERTY(ScraftISprite,bool,SetFlippedByY,IsFlippedByY,yFlip)};
    struct Property_text {TEGGO_S_PROPERTY(ScraftISprite,SetText,GetText,text)};
    struct Property_parent {TEGGO_PROPERTY_ARROW(ScraftISprite,ISpritePtr,SetParent,GetParent,parent)};
    struct Property_hierarchy {TEGGO_R_PROPERTY_ARROW(ScraftISprite,IHierarchyPtr,GetHierarchy,hierarchy)};
    struct Property_scrX  {TEGGO_R_PROPERTY(ScraftISprite,float,GetScrX,scrX)};
    struct Property_scrY {TEGGO_R_PROPERTY(ScraftISprite,float,GetScrY,scrY)};
    struct Property_scrAngle {TEGGO_R_PROPERTY(ScraftISprite,float,GetScrAngle,scrAngle)};
    struct Property_collide {TEGGO_PROPERTY(ScraftISprite,long,SetCollideGroup,GetCollideGroup,collide)};
    struct Property_alive {TEGGO_R_PROPERTY(ScraftISprite,bool,IsAlive,alive)};
    struct Property_random {TEGGO_R_PROPERTY(ScraftISprite,float,GetRandom,random)};
    struct Property_frcount {TEGGO_R_PROPERTY(ScraftISprite,long,FrCount,frcount)};
    struct Property_cookie {TEGGO_PROPERTY(ScraftISprite,long,SetCookie,GetCookie,cookie)};
    struct Property_sublayer {TEGGO_PROPERTY(ScraftISprite,long,SetSubLayer,GetSubLayer,sublayer)};
    struct Property_outOfScreen {TEGGO_R_PROPERTY(ScraftISprite,bool,IsOutOfScreen,outOfScreen)};
    struct Property_subimpl {TEGGO_PROPERTY(ScraftISprite,IUnknownPtr,SetSubImpl,GetSubImpl,subimpl)};
    struct Property_style {TEGGO_PROPERTY(ScraftISprite,tagStyle,SetStyle,GetStyle,style)};
    struct Property_lwsArray {TEGGO_R_PROPERTY_ARROW(ScraftISprite,ILwsArrayPtr,GetLwsArray,lwsArray)};
    struct Property_textView {TEGGO_R_PROPERTY_ARROW(ScraftISprite,ITextViewPtr,GetTextView,textView)};
    struct Property_primitive {TEGGO_R_PROPERTY_ARROW(ScraftISprite,ISpritePrimitivePtr,GetPrimitive,primitive)};

    Property_name         name;
    Property_klass        klass;
    Property_layer        layer;
    Property_x            x;
    Property_y            y;
    Property_degree       degree;
    Property_angle        angle;
    Property_frno         frno;
    Property_visible      visible;
    Property_height       height;
    Property_width        width;
    Property_transparency transparency;
    Property_luminance    luminance;
    Property_mouseOver    mouseOver;
    Property_xScale       xScale;
    Property_yScale       yScale;
    Property_xyScale      xyScale;
    Property_xSize        xSize;
    Property_ySize        ySize;
    Property_xHotspot     xHotspot;
    Property_yHotspot     yHotspot;
    Property_hotspot      hotspot;
    Property_cfilt        cfilt;
    Property_xFlip        xFlip;
    Property_yFlip        yFlip;
    Property_text         text;
    Property_parent       parent;
    Property_hierarchy    hierarchy;
    Property_scrX         scrX;
    Property_scrY         scrY;
    Property_scrAngle     scrAngle;
    Property_collide      collide;
    Property_alive        alive;
    Property_random       random;
    Property_frcount      frcount;
    Property_cookie       cookie;
    Property_sublayer     sublayer;
    Property_outOfScreen  outOfScreen;
    Property_subimpl      subimpl;
    Property_style        style;
    Property_lwsArray     lwsArray;
    Property_textView     textView;
    Property_primitive    primitive;
    Property_paralax      paralax;

  };

inline ISprite* scraft_query_ifs(ScraftISprite* u)
  {
    return u->QueryIfs();
  }
