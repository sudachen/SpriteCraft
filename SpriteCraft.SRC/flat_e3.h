
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

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

#ifndef ___3dc8a767_c894_46ed_a5ab_d48a86c3a4f1___
#define ___3dc8a767_c894_46ed_a5ab_d48a86c3a4f1___

#include "flat_e.h"
#include "flat_e0.h"
#include "../moleskin2/mgraphics/m_screen.h"
#include "../moleskin2/mgraphics/m_loadsurf.h"
#include "../moleskin2/mgraphics/m_gfx3d.h"
#include "../moleskin2/mgraphics/m_texfont.h"
#include "../moleskin2/nvmath/algebra.h"
#include "../moleskin2/mUTIL/m_roundpow2.h"

using namespace nvmath;

static const float DEPTH_STEP_ON_SPRITE = 0.001f;

struct SPRITE_Rect  { rect_t rect; int x,y; };
struct SPRITE_fRect { float s,t,ws,wt; };

/// Класс текстуры и текстурной разметки
struct SPRITE_TexInfo
  {
    float                width_;     // ширина текстуры
    float                height_;    // высота текстуры
    AutoTexture          image_;     // текстра
    BufferT<SPRITE_Rect> layout_EX_; // области исходного имеджа
  };

struct KlassOfSprite3D;

struct Sprite3D_Layer
  {
    long no;
    float x;
    float y;
    struct
      {
        bool visible: 1;
      };
    Sprite3D_Layer(long _no,float _x,float _y,bool _vis)
      : no(_no),x(_x),y(_y),visible(_vis) {}
  };

struct Sprite3D;

struct Sprite3D_Layered
  {
    Sprite3D*           layered_sprite_;
    u32_t               unique_no_;     // уникальный ид обьекта
    long                layer_;         // слой на котором находится спрайт
    long                layer_cheat_;
    Sprite3D_Layered()
      : unique_no_(0),layer_(0),layer_cheat_(0),layered_sprite_(0) {}
  };

struct Sprite3D_Text
  {
    widestring text_; // текст для текстового спрайта
    long   height_;
    long   width_;
    //vector<VxB_VERTEX> vertices_;
    BufferT<TexFont::SYMBOL> symbols_;
    TexFont::SYMBOL *si_;

    bool SymbolReset()
      {
        si_ = symbols_.Begin();
        return si_ != symbols_.End();
      }

    TexFont::SYMBOL* SymbolNext()
      {
        if ( si_ == symbols_.End() ) return 0;
        TexFont::SYMBOL* ret = &*si_;
        ++si_;
        return ret;
      }

    void Prepare(TexFont* font_)
      {
        if ( font_ )
          {
            font_->FillSymbols(symbols_,+text_);
            Qsort(symbols_.Begin(),symbols_.End());
          }
        else symbols_.Resize(0);
      }
  };

/// Реализация спрайта
struct Sprite3D : public Sprite
  {
    struct TextView
      {
        frect_t frect;
        int     border;
        mrgb_t  background;
        mrgb_t  bordcolor;
        bool    active:1;
        bool    colorized:1;
      };

    TextView* AllocateTextView()
      {
        if ( !textview_ )
          {
            textview_.reassign__(new TextView());
            memset(&textview_->frect,0,sizeof(textview_->frect));
            textview_->border = 0;
            textview_->background = mRGB_t(0,0,0);
            textview_->bordcolor = mRGB_t(0,0,0);
            textview_->active = false;
            textview_->colorized = false;
          }
        return ~textview_;
      }

    static  pchar_t const TypeID__;

    Sprite3D_Layered layered_;       /* 16 */
    Sprite3D_Layer*  layerObj_;      /* 4 */

    struct
      {                          /* 4 */
        unsigned  hsptag_:8;
        bool      text_multiline_:1;
        bool      update_text_:1;
        bool      visible_:1;
        bool      flip_x_:1;
        bool      flip_y_:1;
        bool      scaled_:1;
        bool      rotated_:1;
        bool      blended_:1;
        bool      complex_:1;
        bool      triangled_:1;
        bool      inter_paralaxed_:1;
        //bool      paralaxed_:1;
        //bool      dummy_:1;
      };

    float               z_;             /* 4 */ // глубина
    float               paralax_;       /* 4 */
    Sprite3D*           parent_;        /* 4 */
    u32_t             parent_deps_;   /* 4 */

    KlassOfSprite3D*    klass_;         /* 4 */ // класс спрайта

    float               x_;             /* 4 */ // координаты
    float               y_;             /* 4 */ // координаты
    float               xscale_;        /* 4 */ // масштаб спрайта
    float               yscale_;        /* 4 */ // масштаб спрайта
    float               transparency_;  /* 4 */ // степень просзрачности спрайта
    float               luminance_;     /* 4 */ // яркость спрайта
    mrgb_t              crgb_;          /* 4 */
    mrgba_t             color_;         /* 4 */
    unsigned            fr_no_;         /* 4 */ // текущий кадр анимации
    float               radians_;       /* 4 */ // угол поворота
    float               cos_;           /* 4 */ // предвычисленнй косинус угла поворота
    float               sin_;           /* 4 */ // предвычисленнй синус угла поворота
    float               xhsp_;          /* 4 */
    float               yhsp_;          /* 4 */

    auto_ref<SpriteSubImpl> subimpl_;   /* 4 */
    auto_mem<Sprite3D_Text> text_;      /* 4 */
    auto_mem<TextView>      textview_;  /* 4 */


    KeyListT<Sprite3D*>  children_;      /* 4 */

    auto_mem< BufferT<VxB_VERTEX> > triangled_vertices_;
    mrgba_t                        triangled_color_;

    virtual void*       Adaptate_(const void* id);
    virtual void        Move(float x,float y);
    virtual void        MoveX(float y);
    virtual void        MoveY(float y);
    virtual void        Rotate(float degree);
    virtual void        SetAngle(float radians);
    virtual void        Scale(float persent);
    virtual void        ScaleX(float persent);
    virtual void        ScaleY(float persent);
    virtual void        SetSizeX(float xsize);
    virtual void        SetSizeY(float ysize);
    virtual void        Rescale(float w,float h);
    virtual void        SetHotspotX(float xhsp);
    virtual void        SetHotspotY(float yhsp);
    virtual void        SetHotspot(unsigned hsptag);
    virtual float       GetHotspotX();
    virtual float       GetHotspotY();
    virtual float       GetDegee() const;
    virtual float       GetAngle() const;
    virtual float       GetScrAngle() const;
    virtual float       GetX() const;
    virtual float       GetY() const;
    virtual float       GetScrX() const;
    virtual float       GetScrY() const;
    virtual float       GetWidth() const;
    virtual float       GetHeight() const;
    virtual pwide_t     GetKlass() const;
    virtual float       GetTransparency() const;
    virtual void        SetTransparency(float percent);
    virtual float       GetLuminance() const;
    virtual void        SetLuminance(float percent);
    virtual float       GetScaleX() const;
    virtual float       GetScaleY() const;
    virtual SCERROR     Show();
    virtual SCERROR     Hide();
    virtual bool        IsVisible() const;
    virtual pwide_t     GetName() const;
    virtual void        SetLayer(int);
    virtual int         GetLayer() const;
    virtual void        SetFrameNo(int);
    virtual void        SetSubLayer(int);
    virtual int         GetSubLayer() const;
    virtual int         GetFrameNo() const;
    virtual rect_t      GetRect() const;
    virtual void        Dispose();
    virtual void        SetSubImpl(SpriteSubImpl*);
    virtual SpriteSubImpl* GetSubImpl() const;
    virtual void        SetText(pwide_t text);
    virtual pwide_t     GetText() const;
    virtual bool        Contains(float xX,float yY) const;
    virtual mrgb_t      GetRGB() const;
    virtual void        SetRGB(mrgb_t);
    virtual void        SetFlipX(bool);
    virtual void        SetFlipY(bool);
    virtual bool        IsFlipX() const;
    virtual bool        IsFlipY() const;
    virtual Sprite*     GetParent() const;
    virtual void        SetParent(Sprite*);
    virtual u32_t       GetParentDeps() const;
    virtual void        SetParentDeps(u32_t);
    virtual u32_t       GetFrCount() const;
    virtual SPRITE_COLLIDE_TYPE GetCollideType() const;
    virtual SPRITE_IMAGE_TYPE GetImageType() const;
    virtual void        GetRadius(float* r, float* x, float* y) const;
    virtual void        GetRect(float* x, float* y) const;
    virtual bool        Hit(const Sprite*,bool guard) const;
    virtual bool        OutOfScreen() const;
    virtual float       Distance(Sprite*) const;
    virtual float       AngleWith(Sprite*) const;
    virtual void        IncAngle(float);
    virtual void        IncX(float);
    virtual void        IncY(float);
    virtual void        SpriteToScreen(float* x,float* y);
    virtual void        ScreenToSprite(float* x,float* y);
    virtual bool        ContainsPoint(float* xX,float* yY) const;
    virtual void        GetFullIdent(long* layer,long* sublayer,u32_t* ident) const;
    virtual SCERROR     ChangeKlass(pwide_t klass);
    virtual Sprite*     GetChild(pwide_t key);
    virtual void        AttachChild(Sprite* child, pwide_t key);
    virtual bool        TryEnableComplex();
    virtual bool        TryEnableTriangled();
    virtual void        DisableComplex();
    virtual void        DisableTriangled();
    virtual bool        IsComplex() const;
    virtual bool        IsTriangled() const;

    virtual void        SetTextViewWidth(float width);
    virtual float       GetTextViewWidth() const;
    virtual void        SetTextViewHeight(float height);
    virtual float       GetTextViewHeight() const;
    virtual void        SetTextViewBackground(mrgb_t color);
    virtual mrgb_t      GetTextViewBackground() const;
    virtual void        SetTextViewBorder(long broder);
    virtual long        GetTextViewBorder() const;
    virtual void        SetTextViewBorderColor(mrgb_t color);
    virtual mrgb_t      GetTextViewBorderColor() const;
    virtual void        SetTextViewScrollX(float xOffset);
    virtual float       GetTextViewScrollX() const;
    virtual void        SetTextViewScrollY(float yOffset);
    virtual float       GetTextViewScrollY() const;
    virtual void        SetTextViewActive(bool active);
    virtual bool        IsTextViewActive() const;
    virtual float       GetTextViewMaxScrollX() const;
    virtual float       GetTextViewMaxScrollY() const;
    virtual void        SetTextViewColorized(bool active);
    virtual bool        IsTextViewColorized() const;

    virtual bool        IsText() const;
    virtual bool        IntersectLine(float x0,float y0, float x1,float y1) const;
    virtual long        GetSymheight() const;
    virtual long        GetParalax() const;
    virtual void        SetParalax( long );

    Sprite3D();
    virtual ~Sprite3D();

    void GetRadius_(float* r, float* x, float* y,const Sprite3D* targ) const;
    void GetOBR_(float* r, float* x, float* y) const;
    bool HitPoint(float xX,float yY) const;
  //  bool RecalcXYC(u32_t c_id);

  private:
    void RecalculateColor_()
      {
        color_ = mRGBA_t(
          byte_t(luminance_*float(crgb_.R)),
          byte_t(luminance_*float(crgb_.G)),
          byte_t(luminance_*float(crgb_.B)),
          (!EPSILON_EQUAL0(transparency_)?byte_t((1.0f-transparency_)*255):255)
          );
      }

    float GetTextWidth() const;
    float GetTextHeight() const;

  };

struct Impl_3DSPRITES;

/// Реализаия класса спрайта
struct KlassOfSprite3D
  {
    SPRITE_Info info_;
    Impl_3DSPRITES* mgr_;
    AutoTexFont font_;
    BufferT<Sprite3D*>        sprites_;     // спрайты
    ArrayT<SPRITE_TexInfo>    textures_;    // текстуры класса спрайтов
    BufferT<SPRITE_fRect>     spr_layout_;  // разметка для каждого кадра
    BufferT<u32_t>            fr_mapping_;  // отображение кадров на текстуры
    BufferT<rect_t>           fr_bitmap_;   // отображение кадра на исходный имидж
    mem_arr<u32_t>            collide_bitmask_;
    KlassProvider            *provider_;
    struct
      {
        unsigned cbm_rows   : 8;
        unsigned cbm_cols   : 8;
        bool unisprite_     : 1;
        bool is_scaled_     : 1;
        bool is_unloaded_   : 1;
        bool use_shared_texture_ : 1;
        bool need_update_   : 1;
        //bool dummy_         : 1;
        //bool renderable_    : 1;
      };
                                          // (пусто если текстура одна)
    KlassOfSprite3D()
      {
        provider_ = 0;
      }

    virtual ~KlassOfSprite3D()
      {
        return;
      }

    pwide_t GetName()
      {
        return ::teggo::CollectionT<KlassOfSprite3D>::KeyOf(this);
      }
  };

struct VISIBILITY_SET_Less
  {
    bool operator()(const Sprite3D_Layered* a,const Sprite3D_Layered* b) const
      {
        REQUIRE( a == b || a->unique_no_ != b->unique_no_ );
        if ( a->layer_ < b->layer_ ) return true;
        if ( a->layer_ == b->layer_ )
          {
            if ( a->layer_cheat_ < b->layer_cheat_)
              return true;
            if ( a->layer_cheat_ == b->layer_cheat_
                && a->unique_no_ < b->unique_no_ )
              return true;
          }
        return false;
      }
  };

typedef BufferT<Sprite3D_Layered*> VISIBILITY_SET;

static inline Sprite3D_Layered **VISIBILITY_SET_lower_bound(
  VISIBILITY_SET& vset,Sprite3D_Layered* layered)
  {
    return teggo::binary_lower(vset.Begin(),vset.End(),
            layered,VISIBILITY_SET_Less());
  }

static inline Sprite3D_Layered **VISIBILITY_SET_upper_bound(
  VISIBILITY_SET& vset,Sprite3D_Layered* layered)
  {
    return teggo::binary_upper(vset.Begin(),vset.End(),
            layered,VISIBILITY_SET_Less());
  }

static inline void VISIBILITY_SET_Insert(
  VISIBILITY_SET& vset,Sprite3D_Layered* layered)
  {
    Sprite3D_Layered **i = teggo::binary_lower(vset.begin(),vset.end(),
                            layered,VISIBILITY_SET_Less());
    vset.Insert(i,layered);
  }

static inline void VISIBILITY_SET_Erase(
  VISIBILITY_SET& vset,Sprite3D_Layered* layered)
  {
    Sprite3D_Layered **i = teggo::binary_lower(vset.begin(),vset.end(),
                            layered,VISIBILITY_SET_Less());
    vset.Erase(i);
  }

struct SPRITES_TextureSet;

struct SPRITES_TextureProxy : public Texture , private refbase
  {
    static pchar_t const TypeID__;
    widestring name_;
    unsigned fmt_;
    AutoTexture texture_;
    SPRITES_TextureSet* set_;

    SPRITES_TextureProxy(
      pwide_t name,unsigned fmt,SPRITES_TextureSet* pset,Texture* tex)
      {
        name_ = name;
        fmt_ = fmt;
        texture_.reassign__(tex);
        set_ = pset;
      }

    virtual ~SPRITES_TextureProxy() { Forget_(); }

    virtual void* Adaptate_(const void* id)
      {
        if ( id == SPRITES_TextureProxy::TypeID__ ) return this;
        if ( void* x = IRefObject::Adaptate_(id) ) return x;
        if ( void* x = texture_->Adaptate_(id) ) return  x;
        return 0;
      }

    virtual void Grab_() const { refbase::Grab_(); }
    virtual void Release_() const { refbase::Release_(); }
    virtual int GetWidth() const  { return texture_->GetWidth(); }
    virtual int GetHeight() const { return texture_->GetHeight(); }
    virtual unsigned GetFormat() const { return texture_->GetFormat(); }
    virtual bool IsLost() const { return texture_->IsLost(); }

    virtual SCERROR Load(Surface* surf,int level)
      { return texture_->Load(surf,level); }
    virtual SCERROR LoadSubpicture(Surface* surf,int x,int y,const rect_t& rect,int level)
      { return texture_->LoadSubpicture(surf,x,y,rect,level);}
    virtual SCERROR LoadPalette(const BufferT<mrgba_t>& pal)
      { return texture_->LoadPalette(pal); }
    virtual SCERROR StoreSubpicture(Surface* surf,int x,int y,const rect_t& rec) const
      { return texture_->StoreSubpicture(surf,x,y,rec); }
    virtual SCERROR Restore() { return texture_->Restore(); }
    void Forget_();
  };

struct SPRITES_TextureSet
  {
    struct TexKey
      {
        pwide_t name;
        unsigned fmt;
        SPRITES_TextureProxy* texture;
        bool operator < ( const TexKey& a ) const
          {
            return fmt < a.fmt
               || ( fmt == a.fmt && wcscmp(name,a.name) < 0 );
          }
      };

    typedef FlatsetT<TexKey> set_t;
    set_t set_;

    ~SPRITES_TextureSet()
      {
        for ( set_t::Iterator i = set_.Iterate(); i.Next(); )
          i->texture->set_ = 0;
      }
    Texture* Get(pwide_t name,unsigned fmt)
      {
        TexKey key = { name,fmt,0 };
        if ( TexKey *ti = set_.Get(key) )
          return Grab(ti->texture);
        else
          return 0;
      }
    Texture* Put(pwide_t name,unsigned fmt,Texture* tex)
      {
        auto_ref<SPRITES_TextureProxy> proxy(new SPRITES_TextureProxy(name,fmt,this,tex));
        TexKey key = { proxy->name_.Str(),proxy->fmt_,~proxy };
        set_.Put(key);
        return proxy.forget__();
      }
    void Forget(Texture* tex)
      {
        SPRITES_TextureProxy* proxy = Adaptate<SPRITES_TextureProxy>(tex);
        return_if( !proxy );
        TexKey key = { proxy->name_.Str(),proxy->fmt_,0 };
        set_.Erase(key);
      }
  };

inline void SPRITES_TextureProxy::Forget_()
  {
    if ( set_ )
    set_->Forget(this);
    set_ = 0;
  }

struct Impl_3DSPRITES : public virtual SPRITES_Inst , private refbase
  {
    CollectionT<KlassOfSprite3D> klasses_;
    CollectionT<Sprite3D>        sprites_;
    BufferT<KlassOfSprite3D*>    order_;
    VISIBILITY_SET              layers_;
    BufferT<VxB_VERTEX>          vertices_;
    BufferT<u16_t>            indices_;
    BufferT<Sprite3D_Layer*>     layerObjs_;
    SPRITES_TextureSet          texset_;

    struct
      {
        bool need_restore_ : 1;
        bool need_refresh_ : 1;
        bool visiblity_changed_ : 1;
      };

    void ShowSpr(Sprite3D* s)
      {
        //layers_.insert(s);
        VISIBILITY_SET_Insert(layers_,&s->layered_);
        visiblity_changed_ = true;
      }
    void HideSpr(Sprite3D* s)
      {
        //layers_.erase(s);
        VISIBILITY_SET_Erase(layers_,&s->layered_);
        visiblity_changed_ = true;
      }
    virtual void Release_() const { refbase::Release_(); }
    virtual void Grab_()    const { refbase::Grab_(); }

    Impl_3DSPRITES()
      {
        need_restore_ = true;
        visiblity_changed_ = false;
        // резервируем место для 4000 полигонов
        vertices_.reserve(1024*2*4);    //256K
        indices_.reserve(1024*2*4/4*6); //96K
        SPRITES_Inst::AfterConstract();
      }

    virtual ~Impl_3DSPRITES()
      {
        SPRITES_Inst::BeforeDestract();
        Purge();
      }

    void AssignLayerWithSprites(long layerno,Sprite3D_Layer* layer);

    struct Layer_Less
      {
        bool operator() (const Sprite3D_Layer* a,const Sprite3D_Layer* b) const
          { return a->no < b->no; }
      };

    Sprite3D_Layer* RequestLayer(long no) const
      {
        return const_cast<Impl_3DSPRITES*>(this)->RequestLayer(no,false);
      }

    Sprite3D_Layer* RequestLayer(long no,bool createIfNotExists)
      {
        Sprite3D_Layer layer(no,0,0,true);
        Sprite3D_Layer **i =
          teggo::binary_lower(layerObjs_.Begin(),layerObjs_.End(),&layer,Layer_Less());
        if ( i != layerObjs_.End() && (*i)->no == no ) return *i;
        else if ( createIfNotExists ) {
          Sprite3D_Layer* pLayer = new Sprite3D_Layer(no,0,0,true);
          layerObjs_.Insert(i,pLayer);
          AssignLayerWithSprites(no,pLayer);
          return pLayer;
        }
        else return 0;
      }

    SCERROR   Restore();
    SCERROR   RestoreKlass(pwide_t klass);
    SCERROR   Purge();
    SCERROR   Display();
    SCERROR   Display2();
    SCERROR   NewKlass(pwide_t name,const SPRITE_Info&);
    SCERROR   NewKlass(pwide_t name,const SPRITE_Info&,KlassOfSprite3D **);
    SCERROR   NewDynamicKlass(pwide_t klass,KlassProvider*);
    SCERROR   UnloadKlass(pwide_t name,bool);
    SCERROR   UpdateKlass(pwide_t name);
    bool      HasKlass(pwide_t klass) {return FindKlass_(klass)!=0; };
    Sprite*   NewSprite(pwide_t name,pwide_t klass,int layer);
    SCERROR   DisposeSprite(pwide_t name);
    Sprite*   FindSprite(pwide_t name) { return FindSprite_(name); }
    Sprite*   FindSpriteAt(long x,long y,long nearlayer,long farlayer);
    void      SetNeedRefresh(bool val = true) { need_refresh_ = val; }
    bool      NeedRefresh() const { return need_refresh_; }
    SCERROR   EnumerateSprites(ArrayT<Sprite*>& v,long lnear,long lfar);
    SCERROR   GetKlassInfo(pwide_t name,SPRITE_Info& info);

    void      ChangeParentOf(Sprite3D* child,Sprite3D* parent,pwide_t newkey);
    void      DisposeChildren_(Sprite3D* parent);
    SCERROR   DisposeSprite_(Sprite3D* sprite);
    Sprite3D* FindSprite_(pwide_t name) { return sprites_.Get(name); }
    void      DisplayKlass_(KlassOfSprite3D&,bool);
    SCERROR   LayoutKlass_(KlassOfSprite3D&);
    SCERROR   RestoreKlass_(KlassOfSprite3D&);
    KlassOfSprite3D* FindKlass_(pwide_t name) { return klasses_.Get(name); }
    void      SetLayerX(long no,float val);
    void      SetLayerY(long no,float val);
    void      SetLayerVisibility(long no,bool val);
    float     GetLayerX(long no) const;
    float     GetLayerY(long no) const;
    bool      IsLayerVisible(long no) const;

    SCERROR   ChangeSpriteKlassOn(Sprite3D* sprite,pwide_t klass);
    void      RenderSprite(Sprite3D* sprite,ComplexSprite*);
    void      RenderSprite2(Sprite3D* sprite,TriangledSprite*);
    SCERROR   UnloadAll();
  };

#endif // ___3dc8a767_c894_46ed_a5ab_d48a86c3a4f1___
