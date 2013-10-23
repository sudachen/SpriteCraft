
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

#if !defined __SpriteCraft_Impl_SpriteHelper_h__
#define __SpriteCraft_Impl_SpriteHelper_h__

#include "RBGFilter.h"
#include "flat_e0.h"

struct SpriteHelper;
struct SpriteLwsArray;
struct SpritePrimitive;
struct SpriteTextView;
struct SpriteParticles;
struct SpriteSurface;

struct IStyleHelper;
TEGGO_DECLARE_GUIDOF(IStyleHelper,0xA99CA7D1,0xB942,0x4d10,0x88,0xD4,0x16,0x13,0x89,0xEC,0xBE,0x52);

struct IStyleHelper : IUnknown
  {
    virtual SpriteLwsArray*   AsLwsArray();
    virtual SpritePrimitive*  AsPrimitive();
    virtual SpriteTextView*   AsTextView();
    virtual SpriteParticles*  AsParticles();
    virtual void Displace();
    virtual void SetSpriteWH(float,float);
  };

struct SpriteLwsArray :
  TeggoObject<
    TeggoPyDispImpl< ILwsArray, 0,
      TeggoIfs< IStyleHelper, TeggoISupportErrorInfo > > >,
  private LwsArrayBasic
  {

    SpriteLwsArray();
    ~SpriteLwsArray();
    using LwsArrayBasic::GetComplexSprite;

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
    virtual SpriteLwsArray* AsLwsArray();

    float sprite_width_;
    float sprite_height_;

    void SetSpriteWH(float w,float h);
    void SetSemitransparent( bool val );

    virtual HRESULT __stdcall Order(/*[in]*/ enum tagLwsaOrder order,enum tagHotspot hotspot);
    virtual HRESULT __stdcall get_isCentered(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_isCentered(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_count(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_count(/*[in]*/ long newVal);
    virtual HRESULT __stdcall CopyRange(/*[in]*/ long dst,/*[in]*/ long src,/*[in]*/ long count);
    virtual HRESULT __stdcall ShowRange(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ VARIANT_BOOL on);
    virtual HRESULT __stdcall SetRangeXY(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float x, /*[in]*/ float y);
    virtual HRESULT __stdcall SetRangeAngle(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float angle);
    virtual HRESULT __stdcall SetRangeScale(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float scale);
    virtual HRESULT __stdcall SetRangeTransparency(/*[in]*/ long no, /*[in]*/ long count,/*[in]*/ float transparency);
    virtual HRESULT __stdcall SetRangeFrno(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ long frno);
    virtual HRESULT __stdcall GetOneX(/*[in]*/ long no,/*[out,retval]*/ float* x);
    virtual HRESULT __stdcall GetOneY(/*[in]*/ long no,/*[out,retval]*/ float* y);
    virtual HRESULT __stdcall GetOneAngle(/*[in]*/ long no,/*[out,retval]*/ float* angle);
    virtual HRESULT __stdcall GetOneScale(/*[in]*/ long no,/*[out,retval]*/ float* scale);
    virtual HRESULT __stdcall GetOneTransparency(/*[in]*/ long no,/*[out,retval]*/ float* transparency);
    virtual HRESULT __stdcall GetOneFrno(/*[in]*/ long no,/*[out,retval]*/ long* frno);
    virtual HRESULT __stdcall SetOne(
        /*[in]*/ long no,
        /*[in]*/ float x,
        /*[in]*/ float y,
        /*[in]*/ float scale,
        /*[in]*/ float angle,
        /*[in]*/ float transparency,
        /*[in]*/ long frno);
    virtual HRESULT __stdcall DiffRangeScale(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float diff);
    virtual HRESULT __stdcall DiffRangeXY(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float xdiff,/*[in]*/ float ydiff);
    virtual HRESULT __stdcall DiffRangeTransparency(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float diff);
    virtual HRESULT __stdcall DiffRangeAngle(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ float diff);
    virtual HRESULT __stdcall DiffRangeFrno(/*[in]*/ long no,/*[in]*/ long count,/*[in]*/ long diff);
    virtual HRESULT __stdcall GetRange(/*[out,size_is(count)]*/ struct MICROSPRITE* s,/*[in]*/ long no,/*[in,defaultvalue(1)]*/ long count);
    virtual HRESULT __stdcall SetRange(/*[in,size_is(count)]*/ struct MICROSPRITE* s,/*[in]*/ long no,/*[in,defaultvalue(1)]*/ long count);
  };

struct SpritePrimitive :
  TeggoObject<
    TeggoPyDispImpl< ISpritePrimitive, 0,
        TeggoIfs< IStyleHelper, TeggoISupportErrorInfo > > >,
  TriangledSprite
  {
    virtual SpritePrimitive*  AsPrimitive();
    BufferT<PRIMITIVE_VERTEX> vertices_;
    tagBlendMode blendmode_;
    tagStyle     style_;
    float xoffset_;
    float yoffset_;
    float spr_width_;
    float spr_height_;

    struct
      {
        bool colorized_ : 1;
        bool clip_      : 1;
        bool needupdate_: 1;
        bool uvnorm_    : 1;
        bool cw_        : 1;
      };

    void SetSpriteWH(float w,float h);
    TriangledSprite* GetTriangled();

    HRESULT CheckRange(long no,long count);
    void TriangledImageType(SPRITE_IMAGE_TYPE* spritetype);
    bool TriangledUpdate(SPRITE_IMAGE_TYPE* spritetype, bool* colorized, BufferT<VxB_VERTEX>& vertices);
    SpritePrimitive(tagStyle);
    ~SpritePrimitive();

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);

    virtual HRESULT __stdcall get_cw(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_cw(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_uvNorm(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_uvNorm(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_colorized(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_colorized(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_clip(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_clip(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_yOffset(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_yOffset(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_xOffset(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_xOffset(/*[in]*/ float newVal);
    virtual HRESULT __stdcall GetColor(/*[in]*/ long no, /*[out,retval]*/ long* retVal);
    virtual HRESULT __stdcall GetV(/*[in]*/ long no, /*[out,retval]*/ float* retVal);
    virtual HRESULT __stdcall GetU(/*[in]*/ long no, /*[out,retval]*/ float* retVal);
    virtual HRESULT __stdcall GetY(/*[in]*/ long no, /*[out,retval]*/ float* retVal);
    virtual HRESULT __stdcall GetX(/*[in]*/ long no, /*[out,retval]*/ float* retVal);
    virtual HRESULT __stdcall GetVertexEx(/*[in]*/ long no,/*[in,out]*/ PRIMITIVE_VERTEX* vertices,/*[in,defaultvalue(1)]*/ long count);
    virtual HRESULT __stdcall DiffVertex(/*[in]*/ long no, /*[in]*/ long count, /*[in]*/ float x, /*[in]*/ float y, /*[in,defaultvalue(0)]*/ float u,/*[in,defaultvalue(0)]*/ float v);
    virtual HRESULT __stdcall SetVertex(/*[in]*/ long no,/*[in]*/ long color,/*[in]*/ float x,/*[in]*/ float y,/*[in]*/ float u,/*[in]*/ float v);
    virtual HRESULT __stdcall SetVertexEx(/*[in]*/ long no, /*[in]*/ PRIMITIVE_VERTEX* vertex, /*[in,defaultvalue(1)]*/ long count);
    virtual HRESULT __stdcall SetUV(/*[in]*/ long no, /*[in]*/ float u, /*[in]*/ float v);
    virtual HRESULT __stdcall SetXY(/*[in]*/ long no, /*[in]*/ float x, /*[in]*/ float y);
    virtual HRESULT __stdcall SetColor(/*[in]*/ long no,/*[in]*/ long count, /*[in]*/ long color);
    virtual HRESULT __stdcall get_blendMode(/*[out, retval]*/ enum tagBlendMode *pVal);
    virtual HRESULT __stdcall put_blendMode(/*[in]*/ enum tagBlendMode newVal);
    virtual HRESULT __stdcall get_count(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_count(/*[in]*/ long newVal);
  };

struct Hierarchy :
  TeggoObject <
    TeggoPyDispImpl < IHierarchy, 0,
                    TeggoISupportErrorInfo
    > >
  {
    rcc_ptr<ISpriteHelperEx> helper_;

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);

    Hierarchy(ISpriteHelperEx* helper);
    ~Hierarchy();

    virtual HRESULT __stdcall get_parentDispose(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_parentDispose(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_visibility(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_visibility(VARIANT_BOOL);
    virtual HRESULT __stdcall get_x(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_x(VARIANT_BOOL);
    virtual HRESULT __stdcall get_y(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_y(VARIANT_BOOL);
    virtual HRESULT __stdcall get_angle(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_angle(VARIANT_BOOL);
    virtual HRESULT __stdcall get_luminance(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_luminance(VARIANT_BOOL);
    virtual HRESULT __stdcall get_transparency(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_transparency(VARIANT_BOOL);
    virtual HRESULT __stdcall get_cfilt(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_cfilt(VARIANT_BOOL);
    virtual HRESULT __stdcall get_xScale(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_xScale(VARIANT_BOOL);
    virtual HRESULT __stdcall get_yScale(VARIANT_BOOL*);
    virtual HRESULT __stdcall put_yScale(VARIANT_BOOL);
  };

struct WeakSprite :
  TeggoObject<
      TeggoPyDispImpl< IWeakSprite, 0,
           TeggoIfs< IOnUpdateListener,
           TeggoIfs< ICollidableObject,
           TeggoIfs< IWeakSpriteEx,
                     TeggoISupportErrorInfo
  > > > > >
  {
    SpriteHelper* spr_;
    ICollideUnlink* collide_unlink_;

    void UnlinkCollide();
    void ForgetSprite();

    WeakSprite(SpriteHelper* s);
    virtual ~WeakSprite();

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);

    virtual HRESULT __stdcall get_value(ISprite** pI);
    virtual bool OnUpdateListener(u32_t tickdelta);
    virtual Sprite* ICollidableObject_GetSprite();
    virtual void ICollidableObject_OnCollide(Sprite* spr);
    virtual void ICollidableObject_Link(ICollideUnlink* ulink);
    virtual ISprite* ICollidableObject_GetISprite();
    virtual bool ContainsPoint(float* x,float* y,long* layer,long* sublayer,u32_t* ident);
    virtual bool Alive();
    virtual bool IsVisible();
    virtual ISprite* GetSprite();
    virtual IUnknown* GetSubImpl();
  };

struct SpriteTextView :
  TeggoObject<
    TeggoPyDispImpl< ITextView, 0,
         TeggoIfs< IStyleHelper,
                   TeggoISupportErrorInfo
  > > >
  {
    Sprite* sprite_;

    virtual SpriteTextView* AsTextView();
    virtual void Displace();

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);

    SpriteTextView(Sprite* sprite);
    ~SpriteTextView();

    virtual HRESULT __stdcall get_height(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_height(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_width(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_width(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_yScrollMax(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_xScrollMax(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_yScroll(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_yScroll(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_xScroll(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_xScroll(/*[in]*/ float newVal);
  };

extern HRESULT __stdcall PyCreateSprite_(ISprite** pI,PyObject *,PyObject *);

typedef
  TeggoPyDispImpl< ISprite, (PythonOverCom_Make3)PyCreateSprite_,
 TeggoCopntImpl< &DIID__ISpriteEvents,&CLSID__CoSpriteConnectionPoint,
       TeggoIfs< IColorHelper,
       TeggoIfs< ISpriteHelperEx,
       TeggoIfs< ISpriteHelperEx2,
                 TeggoISupportErrorInfo
  > > > > >
  SpriteHelper_Interfaces_List;

struct SpriteHelper :
  SpriteHelper_Interfaces_List,
  SpriteSubImpl
  {

    struct item_t
      {
        long id;
        long value;
        bool operator < (item_t const &a) const
          {
            return id < a.id;
          }
      };

    mem_ptr< BufferT<item_t> > items_;
    static teggo::randizer_t g_randomizer;

    tagStyle                style_;
    IStyleHelper*           styleHelper_;

    rcc_ptr<IUnknown>       subimpl_;
    rcc_ptr<IOnUpdate>      update_;
    rcc_ptr<IOnCollide>     collide_;
    rcc_ptr<WeakSprite>     weak_;
    rcc_ptr<ISurface>       surface_;

    Sprite*   sprite_;

    float     random_;
    u32_t   collide_type_;
    u32_t   cookie_;

    struct
      {
        bool noname_      : 1;
      };

    static pchar_t const TypeID__;
    virtual void* Adaptate_(const void* id);
    virtual void Grab_() const;
    virtual void Release_() const;

    Sprite* GetSprite_();
    IUnknown* GetSubImpl_();

    bool Update_(u32_t tickdelta);
    bool OnCollide(Sprite* s);
    void SetSpritePtr(Sprite* sprite);
    virtual u32_t  Ex_GetParentDeps() const;
    virtual void     Ex_SetParentDeps(u32_t);
    virtual void     XComObject__check() = 0;
    void CheckDispose();
    HRESULT FindHitNext_internal(Sprite* prev,long lnear,long lfar,ISprite** pI);
    virtual void   IColorHelper_SetColor(mrgb_t);
    virtual mrgb_t IColorHelper_GetColor() const;
    HRESULT InternalAdvise(IUnknown* dispifs,u32_t* cookie);
    HRESULT InternalUnadvise(u32_t cookie);
    HRESULT put_subimpl_(IUnknown* subimpl);

    SpriteHelper();
    virtual ~SpriteHelper();

    virtual HRESULT __stdcall SetFrameOfKlass(/*[in]*/ long frno,/*[in]*/ BSTR klass);
    virtual HRESULT __stdcall Clone(/*[in]*/ BSTR name,/*[out,retval]*/ ISprite** pI);
    virtual HRESULT __stdcall ChangeKlassTo(/*[in]*/ BSTR klass);
    virtual HRESULT __stdcall StopAnimation();
    virtual HRESULT __stdcall AnimateLoop(/*[in]*/ float fps,/*[in]*/ long startFrno,/*[in]*/ long stopFrno,/*[in,defaultvalue(0)]*/ IUnknown* listener,long cookie);
    virtual HRESULT __stdcall AnimateOnce(/*[in]*/ float fps,/*[in]*/ long startFrno,/*[in]*/ long stopFrno,/*[in,defaultvalue(0)]*/ IUnknown* listener,long cookie);
    virtual HRESULT __stdcall SpriteToScreen(float* x,float* y);
    virtual HRESULT __stdcall ScreenToSprite(float* x,float* y);
    virtual HRESULT __stdcall NameIs(/*[in]*/ BSTR name,/*[out,retval]*/ VARIANT_BOOL* pVal);
    virtual HRESULT __stdcall IncY(float val);
    virtual HRESULT __stdcall IncX(/*[in]*/ float val);
    virtual HRESULT __stdcall IncAngle(/*[in]*/ float val);
    virtual HRESULT __stdcall get_outOfScreen(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall AngleWith(/*[in]*/ ISprite* sprite,/*[out,retval]*/ float* pVal);
    virtual HRESULT __stdcall Distance(/*[in]*/ ISprite* sprite,/*[out,retval]*/ float*);
    virtual HRESULT __stdcall get_item(long no, /*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_item(long no, /*[in]*/ long newVal);
    virtual HRESULT __stdcall get_sublayer(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_sublayer(/*[in]*/ long newVal);
    virtual HRESULT __stdcall get_cookie(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_cookie(/*[in]*/ long newVal);
    virtual HRESULT __stdcall Randomize();
    virtual HRESULT __stdcall get_frcount(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall get_random(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_alive(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall get_collide(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_collide(/*[in]*/ long newVal);
    virtual HRESULT __stdcall get_scrAngle(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_scrY(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_scrX(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_subimpl(/*[out, retval]*/ IUnknown **pVal);
    virtual HRESULT __stdcall put_subimpl(/*[in]*/ IUnknown* newVal);
    virtual HRESULT __stdcall get_dispatcher(/*[out, retval]*/ IUnknown **pVal);
    virtual HRESULT __stdcall put_dispatcher(/*[in]*/ IUnknown* newVal);
    virtual HRESULT __stdcall get_weakref(/*[out, retval]*/ IWeakSprite **pVal);
    virtual HRESULT __stdcall Hit(/*[in]*/ ISprite* pI,/*[out,retval]*/ VARIANT_BOOL *bVal);
    virtual HRESULT __stdcall get_angle(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_angle(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_hierarchy(/*[in]*/IHierarchy** pI);
    virtual HRESULT __stdcall get_parent(/*[in]*/ISprite** pI);
    virtual HRESULT __stdcall put_parent(/*[in]*/ISprite* pI);
    virtual HRESULT __stdcall get_cfilt(/*[out, retval]*/ IRGBFilter* *pVal);
    virtual HRESULT __stdcall get_yFlip(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_yFlip(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_xFlip(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_xFlip(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall MoveTo(/*[in]*/ float x,/*[in]*/ float y);
    virtual HRESULT __stdcall put_hotspot(/*[in]*/ enum tagHotspot newVal);
    virtual HRESULT __stdcall get_yHotspot(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_yHotspot(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_xHotspot(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_xHotspot(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_ySize(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_ySize(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_xSize(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_xSize(/*[in]*/ float newVal);
    virtual HRESULT __stdcall Contains(/*[in]*/ float x,/*[in]*/ float y,/*[out,retval]*/ VARIANT_BOOL* bRet);
    virtual HRESULT __stdcall get_sprBottom(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_sprRight(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_sprTop(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_sprLeft(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_xyScale(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_yScale(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_yScale(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_xScale(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_xScale(/*[in]*/ float newVal);
    virtual HRESULT __stdcall Equal(/*[in]*/ IDispatch* spr, /*[out,retval]*/ VARIANT_BOOL* equ);
    virtual HRESULT __stdcall get_mouseOver(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall get_luminance(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_luminance(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_transparency(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_transparency(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_width(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_height(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall get_text(/*[out, retval]*/ BSTR *pVal);
    virtual HRESULT __stdcall put_text(/*[in]*/ BSTR newVal);
    virtual HRESULT __stdcall Dispose();
    virtual HRESULT __stdcall get_visible(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_visible(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_frno(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_frno(/*[in]*/ long newVal);
    virtual HRESULT __stdcall get_degree(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_degree(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_y(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_y(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_x(/*[out, retval]*/ float *pVal);
    virtual HRESULT __stdcall put_x(/*[in]*/ float newVal);
    virtual HRESULT __stdcall get_layer(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_layer(/*[in]*/ long newVal);
    virtual HRESULT __stdcall get_klass(/*[out, retval]*/ BSTR *pVal);
    virtual HRESULT __stdcall get_name(/*[out, retval]*/ BSTR *pVal);
    virtual HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
    virtual HRESULT __stdcall GetChild(/*[in]*/ BSTR key,/*[out,retval]*/ ISprite** pI);
    virtual HRESULT __stdcall AttachChild(BSTR key, ISprite* child);
    virtual HRESULT __stdcall GetChildEx(/*[in]*/ BSTR key,/*[out,retval]*/ IDispatch** pI);
    virtual HRESULT __stdcall AttachChildEx(BSTR key, IDispatch* child);
    virtual HRESULT __stdcall GetKlassAsPWIDE(/*[out,retval]*/ wchar_t** retVal);
    virtual HRESULT __stdcall GetNameAsPWIDE(/*[out,retval]*/ wchar_t** retVal);
    virtual HRESULT __stdcall get_primitive(/*[out, retval]*/ ISpritePrimitive* *pVal);
    virtual HRESULT __stdcall get_lwsArray(/*[out, retval]*/ ILwsArray **pVal);
    virtual HRESULT __stdcall get_textview(/*[out, retval]*/ ITextView* *pVal);
    virtual HRESULT __stdcall get_autodispose(/*[out, retval]*/ VARIANT_BOOL *pVal);
    virtual HRESULT __stdcall put_autodispose(/*[in]*/ VARIANT_BOOL newVal);
    virtual HRESULT __stdcall get_style(/*[out, retval]*/ tagStyle *pVal);
    virtual HRESULT __stdcall put_style(/*[in]*/ tagStyle newVal);
    virtual HRESULT __stdcall get_blendMode(tagBlendMode *pVal);
    virtual HRESULT __stdcall put_blendMode(tagBlendMode newVal);
    virtual HRESULT __stdcall DispatchSubImpl(/*[in]*/ IDispatch* subimpl);
    virtual HRESULT __stdcall get_symheight(/*[out, retval]*/ long *pVal);
    virtual HRESULT __stdcall put_surface(/*[in]*/ ISurface *);
    virtual HRESULT __stdcall get_surface(/*[out, retval]*/ ISurface **);
    virtual HRESULT __stdcall get_paralax(/*[out, retval]*/ long *);
    virtual HRESULT __stdcall put_paralax(/*[in]*/ long);

    virtual HRESULT __stdcall __comfunc_SprBorder(void **);
    virtual HRESULT __stdcall __comfunc_ScrBorder(void **);
  };

extern HRESULT __stdcall PyCreateSurface_(ISurface **pI,PyObject *,PyObject *);

struct SpriteSurface :
  TeggoObject <
    TeggoPyDispImpl < ISurface, (PythonOverCom_Make3)PyCreateSurface_,
      TeggoSupportEInfo <ISurface>
    > >, KlassProvider
  {
    rcc_ptr<TFFt_BITMAP> bitmap_;
    StringW klass_name_;

    static SpriteSurface *FromSource(pwide_t source, int width, int height, int pf);
    SpriteSurface(int width, int height, int pf);
    virtual ~SpriteSurface();

    virtual HRESULT __stdcall get_width(long*);
    virtual HRESULT __stdcall get_height(long*);
    virtual HRESULT __stdcall get_pf(long*);
    virtual HRESULT __stdcall GetHDC(long*);
    virtual HRESULT __stdcall Update();

    virtual wchar_t * __stdcall __klass_name__();
    virtual void FillInfo(SPRITE_Info *info);
    virtual TFFt_BITMAP *GetBitmap();
  };

#endif //  __SpriteCraft_Impl_SpriteHelper_h__
