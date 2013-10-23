
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

struct CXX_NO_VTABLE ScraftIParticles
  {
    IParticles*    QueryIfs() { return (IParticles*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }
    void           Dispose();

    long           GetCount();
    void           SetCount(long count);

    __ScString     GetName();
    __ScString     GetKlass();
    pwide_t        GetNameAsPWIDE();
    pwide_t        GetKlassAsPWIDE();
    long           GetLayer();
    void           SetLayer(long);
    long           GetSubLayer();
    void           SetSubLayer(long);
    float          GetX();
    void           SetX(float);
    float          GetY();
    void           SetY(float);
    float          GetAngle();
    void           SetAngle(float);
    bool           IsVisible();
    void           SetVisible(bool);
    float          GetScaleX();
    float          GetScaleY();
    void           SetScaleX(float);
    void           SetScaleY(float);
    void           SetScale(float);
    float          GetTransparency();
    void           SetTransparency(float);
    float          GetLuminance();
    void           SetLuminance(float);
    IColorPtr      Color();
    void           SetParent(const ISpritePtr&);
    void           SetParentNothing();
    ISpritePtr     GetParent();
    IHierarchyPtr  GetHierarchy();

    bool           IsAlive();

    void           Reset();
    void           StartEmission(IUnknown* listener = 0,long cookie = 0);
    void           StopEmission(long timeout = 0);
    void           SetSubImpl(IUnknown*);
    void           SetEmCount(long emcount);
    long           GetEmCount();
    void           SetLifeTime(long lifetime);
    long           GetLifeTime();

    void           SetEmitterCf(long cfno, float val0,float val1,long emitter=-1);
    void           SetEmissionQuantity(long minval,long maxval,long emitter=-1);
    void           SetEmissionPeriod(long minval,long maxval,long emitter=-1);
    void           SetEmissionTime(long minval,long maxval,long emitter=-1);
    void           SetProgram(pwide_t program, long emitter = -1);
    void           SetDefaultProgram(long emitter = -1);
    void           DumpProgramMicrops(long emitter = -1);
    void           SetCycled(bool val);
    bool           IsCycled();
    void           SetAutodispose(bool val);
    bool           IsAutodispose();


    struct Property_name      {TEGGO_R_PROPERTY(ScraftIParticles,__ScString,GetName,name)} name;
    struct Property_klass     {TEGGO_R_PROPERTY(ScraftIParticles,__ScString,GetKlass,klass)} klass;
    struct Property_layer     {TEGGO_PROPERTY(ScraftIParticles,long,SetLayer,GetLayer,layer)} layer;
    struct Property_x         {TEGGO_PROPERTY(ScraftIParticles,float,SetX,GetX,x)} x;
    struct Property_y         {TEGGO_PROPERTY(ScraftIParticles,float,SetY,GetY,y)} y;
    struct Property_angle     {TEGGO_PROPERTY(ScraftIParticles,float,SetAngle,GetAngle,angle)} angle;
    struct Property_visible   {TEGGO_PROPERTY(ScraftIParticles,bool,SetVisible,IsVisible,visible)} visible;
    struct Property_luminance {TEGGO_PROPERTY(ScraftIParticles,float,SetLuminance,GetLuminance,luminance)} luminance;
    struct Property_xScale    {TEGGO_PROPERTY(ScraftIParticles,float,SetScaleX,GetScaleX,xScale)} xScale;
    struct Property_yScale    {TEGGO_PROPERTY(ScraftIParticles,float,SetScaleY,GetScaleY,yScale)} yScale;
    struct Property_xyScale   {TEGGO_W_PROPERTY(ScraftIParticles,float,SetScale,xyScale)} xyScale;
    struct Property_cfilt     {TEGGO_R_PROPERTY_ARROW(ScraftIParticles,IColorPtr,Color,cfilt)} cfilt;
    struct Property_parent    {TEGGO_PROPERTY_ARROW(ScraftIParticles,ISpritePtr,SetParent,GetParent,parent)} parent;
    struct Property_hierarchy {TEGGO_R_PROPERTY_ARROW(ScraftIParticles,IHierarchyPtr,GetHierarchy,hierarchy)} hierarchy;
    struct Property_alive     {TEGGO_R_PROPERTY(ScraftIParticles,bool,IsAlive,alive)} alive;
    struct Property_sublayer  {TEGGO_PROPERTY(ScraftIParticles,long,SetSubLayer,GetSubLayer,sublayer)} sublayer;
    struct Property_subimpl   {TEGGO_W_PROPERTY(ScraftIParticles,IUnknown*,SetSubImpl,subimpl)} subimpl;
    struct Property_count     { TEGGO_PROPERTY(ScraftIParticles,long,SetCount,GetCount,count) } count;
    struct Property_cycled    { TEGGO_PROPERTY(ScraftIParticles,bool,SetCycled,IsCycled,cycled) } cycled;
    struct Property_transparency {TEGGO_PROPERTY(ScraftIParticles,float,SetTransparency,GetTransparency,transparency)} transparency;
    struct Property_autodispose {TEGGO_PROPERTY(ScraftIParticles,bool,SetAutodispose,IsAutodispose,autodispose)} autodispose;
  };

inline IParticles* scraft_query_ifs(ScraftIParticles* u)
  {
    return u->QueryIfs();
  }
