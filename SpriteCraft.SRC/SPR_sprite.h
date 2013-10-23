
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

#if !defined ___5d3e664a_70b8_462d_8f76_9da11a6245f8___
#define ___5d3e664a_70b8_462d_8f76_9da11a6245f8___

#include "WD3D_window.h"

struct SCE_LAYERED;
struct SCE_LAYEROBJ;
struct SCE_SPRITE;
struct SCE_SKLASS;
struct SCE_TEXTSPRITE;

struct SCE_VERTEX1 /*24*/
  {
    float   vx,vy,vz; /*12*/
    mbgra_t color;    /*4*/
    float   ts,tt;    /*8*/
  };

struct ISceSubimpl : IUnknown 
  {
    typedef GuidT<0xf65c9a46,0xb21a,0x4937,0xa1,0xcd,0x8d,0xa9,0x56,0x57,0x24,0xcf> Guid;
    virtual void SetSpritePtr(SCE_SPRITE*) = 0;
  };

struct SCE_COMPLEX
  {
    struct INFO
      {
        enum 
          { 
            IS_ROTATED    = MSKIN_BIT(0),
            IS_SCALED     = MSKIN_BIT(1),
            IS_BLENDED    = MSKIN_BIT(2),
            IS_INDEPENDED = MSKIN_BIT(3),
            IS_CENTERED   = MSKIN_BIT(4),
            IS_UPDATED_R  = MSKIN_BIT(5),
            IS_VISIBLE_R  = MSKIN_BIT(6)
          };
        float x,y,xscale,yscale,cosv,sinv;
        u32_t alphaCf;
        u32_t fr_no;
        u32_t flags;
      };
    struct ELEMENT{ int x,y,frno; };
    virtual bool ComplexReset(bool blendstate) = 0;
    virtual bool ComplexNext(INFO *info, bool blendstate) = 0;
    virtual bool ComplexHasBlendState(bool) = 0;
  };

struct SCE_TRIANGLED 
  {
    virtual void TriangledImageType(SCE_SPRITE_IMAGE_TYPE *spritetype) = 0;
    virtual bool TriangledUpdate(
                    SCE_SPRITE_IMAGE_TYPE *spritetype, bool *colorized, 
                    BufferT<SCE_VERTEX1> &vertices) = 0;
  };

enum SCE_SPRITE_IMAGE_TYPE
  {
    SCE_SPRITE_INVALID_TYPE = 0,
    SCE_SPRITE_SOLID        = 1,
    SCE_SPRITE_BITMASK      = 2,
    SCE_SPRITE_COLORKEY     = 3,
    SCE_SPRITE_BLEND        = 4,
    SCE_SPRITE_ADDITIVE     = 5
  };

enum SCE_SPRITE_COLLIDE_TYPE
  {
    SCE_SPRITE_COLLIDE_RECTANGLE = 0,
    SCE_SPRITE_COLLIDE_RADIUS    = 1,
    SCE_SPRITE_COLLIDE_BITMASK   = 2
  };

struct SCE_SPRITE_Info__ 
  {
    bool     is_paletted  :1; // спрайт задан палитровым изображением
    bool     is_font      :1; // спрайт задаёт фонт
    bool     is_region    :1; // спрайт задан регионом
    bool     filtering    :1; // рисовать с фильтрацией
    bool     compress_it  :1; // сжимать спрайт если он выходит за максимальный размер текстуры
    bool     dummy_       :1; // фейковый спрайт (не рисуется)
    bool     additive_    :1; // рисовать с аддитивным блендингом
    bool     renderable_  :1; // спрайт лтображается
    bool     bits_rgb32   :1; // использовать полноцветную текстуру
    bool     bits_lowcolor:1; // использовать минимальную цветность текстуры
    unsigned mipmap       :4; // количество мипмап уровней
    unsigned round        :8; // ужимать картинку к минимальной приемлимой текстуре
  };

struct SCE_SPRITE_Info_ : SCE_SPRITE_Info__ 
  {
    unsigned spr_no;
    unsigned spr_width;
    unsigned spr_height;
    unsigned bitmap_width;
    unsigned bitmap_height;
    unsigned spr_x;
    unsigned spr_y;
    unsigned transparecy;
    int      hotspot_x;
    int      hotspot_y;
    mrgb_t   colorkey;
    float collideRadius;
    SCE_SPRITE_IMAGE_TYPE type;
    SCE_SPRITE_COLLIDE_TYPE collide;
  };

struct SCE_SINFO : SPRITE_Info_ 
  {
    widestring bitmap;
    widestring alpha;
    widestring symbols;
    SCE_SINFO() 
      { 
        memset((SCE_SPRITE_Info_*)this,0,sizeof(SCE_SPRITE_Info_));  
      }
  };

SCERROR FillSINFO(
  pwide_t kname,SCE_SPRITE *info,pwide_t bitmap_resource,
  SCE_SPRITE_IMAGE_TYPE imgt=SCE_SPRITE_SOLID,u32_t crgb=0,
  bool filtering=false,bool cmprs=true,bool isfnt=false,
  int hor_layout=1, int ver_layout=1, pwide_t symbols=0);

struct SCE_RECT  { rect_t rect; int x,y; };
struct SCE_fRECT { float s,t,ws,wt; };

/// Класс текстуры и текстурной разметки
struct SCE_TEXTINFO 
  {
    float               width;     // ширина текстуры
    float               height;    // высота текстуры
    AutoTexture         image;     // текстра
    BufferT<SCE_RECT>   layout_EX; // области исходного имиджа
  };

struct SCE_LAYER 
  {
    long no;
    float x;
    float y;
    struct 
      {
        bool visible: 1;
      };
    SCE_LAYER(long _no,float _x,float _y,bool _vis) 
      : no(_no),x(_x),y(_y),visible(_vis) 
      {
      }
  };

struct SCE_SPRITE;
struct SCE_LAYERED 
  {
    SCE_SPRITE  *layered_sprite_;
    u32_t       unique_no_;     // уникальный ид обьекта
    long        layer_;         // слой на котором находится спрайт
    long        layer_cheat_;
    SCE_LAYERED() 
      : unique_no_(0),layer_(0),layer_cheat_(0),layered_sprite_(0) 
      {
      }
  };

//struct TEXT_SPRITE 
//  {
//    long   height_;
//    long   width_;
//    StringT<wchar_t> text_; // текст для текстового спрайта
//    BufferT<TexFont::SYMBOL> symbols_;
//    TexFont::SYMBOL *si_;
//
//    bool SymbolReset()
//      {
//        si_ = symbols_.Begin();
//        return si_ != symbols_.End();
//      }
//
//    TexFont::SYMBOL* SymbolNext()
//      {
//        if ( si_ == symbols_.End() ) return 0;
//        TexFont::SYMBOL* ret = &*si_;
//        ++si_;
//        return ret;
//      }
//
//    void Prepare(TexFont* font_)
//      {
//        if ( font_ ) {
//          font_->FillSymbols(symbols_,+text_);
//          teggo::quicksort(symbols_.Begin(),symbols_.End());
//        }
//        else symbols_.Resize(0);
//      }
//  };


struct SCE_SPRITE 
  {
    //struct TEXTVIEW 
    //  {
    //    frect_t frect;
    //    int     border;
    //    mrgb_t  background;
    //    mrgb_t  bordcolor;
    //    struct 
    //      {
    //        bool  active:1;
    //        bool  colorized:1;
    //      };
    //  };
    
    struct TRIANGULATED
      {
        BufferT<SCE_VERTEX1>  vertices;
        mrgba_t               color;
      };
    

    SCE_LAYERED layered_; /* 16 bytes */
    SCE_LAYER  *layerObj_;
    
    struct /* 4 bytes */
      {                          
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
      };

    float        z_;             /* 4 */ // глубина
    float        paralax_;       /* 4 */ 
    SCE_SPRITE  *parent_;        /* 4 */ 
    u32_t        parent_deps_;   /* 4 */ 
    SCE_SKLASS  *klass_;         /* 4 */ // класс спрайта

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

    rcc_ptr<ISceSubimpl>    subimpl_;   /* 4 */
    //mem_ptr<SCE_TEXTSPRITE> text_;      /* 4 */
    //mem_ptr<TEXTVIEW>       textview_;  /* 4 */
    KeyListT<SCE_SPRITE*>   children_;  /* 4 */
    mem_ptr<TRIANGULATED>   triangled_; /* 4 */

    //TEXTVIEW   *AllocateTextView() ;
    void        Move(float x,float y);
    void        MoveX(float y);
    void        MoveY(float y);
    void        Rotate(float degree);
    void        SetAngle(float radians);
    void        Scale(float persent);
    void        ScaleX(float persent);
    void        ScaleY(float persent);
    void        SetSizeX(float xsize);
    void        SetSizeY(float ysize);
    void        Rescale(float w,float h);
    void        SetHotspotX(float xhsp);
    void        SetHotspotY(float yhsp);
    void        SetHotspot(unsigned hsptag);
    float       GetHotspotX();
    float       GetHotspotY();
    float       GetDegee() const;
    float       GetAngle() const;
    float       GetScrAngle() const;
    float       GetX() const;
    float       GetY() const;
    float       GetScrX() const;
    float       GetScrY() const;
    float       GetWidth() const;
    float       GetHeight() const;
    pwide_t     GetKlass() const;
    float       GetTransparency() const;
    void        SetTransparency(float percent);
    float       GetLuminance() const;
    void        SetLuminance(float percent);
    float       GetScaleX() const;
    float       GetScaleY() const;
    SCERROR     Show();
    SCERROR     Hide();
    bool        IsVisible() const;
    pwide_t     GetName() const;
    void        SetLayer(int);
    int         GetLayer() const;
    void        SetFrameNo(int);
    void        SetSubLayer(int);
    int         GetSubLayer() const;
    int         GetFrameNo() const;
    rect_t      GetRect() const;
    void        Dispose();
    void        SetSubImpl(ISceSubimpl*);
    ISceSubimpl *GetSubImpl() const;
    void        SetText(pwide_t text);
    pwide_t     GetText() const;
    bool        Contains(float xX,float yY) const;
    mrgb_t      GetRGB() const;
    void        SetRGB(mrgb_t);
    void        SetFlipX(bool);
    void        SetFlipY(bool);
    bool        IsFlipX() const;
    bool        IsFlipY() const;
    SCE_SPRITE *GetParent() const;
    void        SetParent(Sprite*);
    u32_t       GetParentDeps() const;
    void        SetParentDeps(u32_t);
    u32_t       GetFrCount() const;
    SCE_SPRITE_COLLIDE_TYPE GetCollideType() const;
    SCE_SPRITE_IMAGE_TYPE GetImageType() const;
    void        GetRadius(float* r, float* x, float* y) const;
    void        GetRect(float* x, float* y) const;
    bool        Hit(const Sprite*,bool guard) const;
    bool        OutOfScreen() const;
    float       Distance(Sprite*) const;
    float       AngleWith(Sprite*) const;
    void        IncAngle(float);
    void        IncX(float);
    void        IncY(float);
    void        SpriteToScreen(float* x,float* y);
    void        ScreenToSprite(float* x,float* y);
    bool        ContainsPoint(float* xX,float* yY) const;
    void        GetFullIdent(long* layer,long* sublayer,u32_t* ident) const;
    SCERROR     ChangeKlass(pwide_t klass);
    SCE_SPRITE *GetChild(pwide_t key);
    void        AttachChild(Sprite* child, pwide_t key);
    bool        TryEnableComplex();
    bool        TryEnableTriangled();
    void        DisableComplex();
    void        DisableTriangled();
    bool        IsComplex() const;
    bool        IsTriangled() const;
    void        SetTextViewWidth(float width);
    float       GetTextViewWidth() const;
    void        SetTextViewHeight(float height);
    float       GetTextViewHeight() const;
    void        SetTextViewBackground(mrgb_t color);
    mrgb_t      GetTextViewBackground() const;
    void        SetTextViewBorder(long broder);
    long        GetTextViewBorder() const;
    void        SetTextViewBorderColor(mrgb_t color);
    mrgb_t      GetTextViewBorderColor() const;
    void        SetTextViewScrollX(float xOffset);
    float       GetTextViewScrollX() const;
    void        SetTextViewScrollY(float yOffset);
    float       GetTextViewScrollY() const;
    void        SetTextViewActive(bool active);
    bool        IsTextViewActive() const;
    float       GetTextViewMaxScrollX() const;
    float       GetTextViewMaxScrollY() const;
    void        SetTextViewColorized(bool active);
    bool        IsTextViewColorized() const;
    bool        IsText() const; 
    bool        IntersectLine(float x0,float y0, float x1,float y1) const;
    long        GetSymheight() const;
    long        GetParalax() const;
    void        SetParalax( long );

    SCE_SPRITE();
    ~SCE_SPRITE();
    
    void GetRadius_(float* r, float* x, float* y,const Sprite3D* targ) const;
    void GetOBR_(float* r, float* x, float* y) const;
    bool HitPoint(float xX,float yY) const;

  private:
    void RecalculateColor_() 
      {
        color_ = mRGBA_t(
          byte_t(luminance_*float(crgb_.R)),
          byte_t(luminance_*float(crgb_.G)),
          byte_t(luminance_*float(crgb_.B)),
          (!teggo::in_epsilon(transparency_)?byte_t((1.0f-transparency_)*255):255)
          );
      }

    float GetTextWidth() const;
    float GetTextHeight() const;
  };

struct SCE_SKLASS 
  {
    SCE_SINFO info_;
    AutoTexFont font_;
    BufferT<SCE_SPRITE>       sprites_;     // спрайты
    BufferT<SCE_TEXTINFO>     textures_;    // текстуры класса спрайтов
    BufferT<SCE_fRECT>        spr_layout_;  // разметка для каждого кадра 
    BufferT<u32_t>            fr_mapping_;  // отображение кадров на текстуры
    BufferT<rect_t>           fr_bitmap_;   // отображение кадра на исходный имидж
    mem_arr<u32_t>            collide_bitmask_;
    struct 
      {
        unsigned cbm_rows   : 8;
        unsigned cbm_cols   : 8;
        bool unisprite_     : 1;
        bool is_scaled_     : 1;
        bool is_unloaded_   : 1;
        bool use_shared_texture_ : 1;
      };
                                          // (пусто если текстура одна)
    ~SCE_SKLASS() {}

    pwide_t GetName() 
      {
        return ::teggo::CollectionT<SCE_SKLASS>::KeyOf(this);
      }
  };

struct SCE_SPRITES_
  {
  
    static SCE_SPRITES_ *instance_;
    static void Init();
    static void Term();
    
    struct Gate 
      {
        SCE_SPRITES_ *operator->() { return SCE_SPRITES_::instance_; }
      };
  
    CollectionT<SCE_SKLASS> klasses_;
    CollectionT<SCE_SPRITE> sprites_;
    BufferT<SCE_LAYER>      layers_;
  
    struct {
      bool need_restore_ : 1;
      bool need_refresh_ : 1;
      bool visiblity_changed_ : 1;
    };
  
    void ShowSpr(SCE_SPRITE *s);
    void HideSpr(SCE_SPRITE *s);
    
    SCE_SPRITES_();
    ~SCE_SPRITES_() {}
    void AssignLayerWithSprites(long layerno,SCE_LAYER *layer);
    SCE_LAYER  *RequestLayer(long no) const;
    SCE_LAYER  *RequestLayer(long no,bool createIfNotExists);
    SCERROR     Restore();
    SCERROR     RestoreKlass(pwide_t klass);
    SCERROR     Purge();
    SCERROR     Display();
    SCERROR     Display2();
    SCERROR     NewKlass(pwide_t name,const SCE_SINFO&);
    SCERROR     UnloadKlass(pwide_t name,bool);
    bool        HasKlass(pwide_t klass) {return FindKlass_(klass)!=0; };
    SCE_SPRITE *NewSprite(pwide_t name,pwide_t klass,int layer);
    SCERROR     DisposeSprite(pwide_t name);
    SCE_SPRITE *FindSprite(pwide_t name) { return FindSprite_(name); }
    SCE_SPRITE *FindSpriteAt(long x,long y,long nearlayer,long farlayer);
    void        SetNeedRefresh(bool val = true) { need_refresh_ = val; }
    bool        NeedRefresh() const { return need_refresh_; }
    SCERROR     EnumerateSprites(BufferT<SCE_SPRITE*>& v,long lnear,long lfar);
    SCERROR     GetKlassInfo(pwide_t name,SPRITE_Info& info);

    void        ChangeParentOf(SCE_SPRITE *child,SCE_SPRITE *parent,pwide_t newkey);
    void        DisposeChildren_(SCE_SPRITE *parent);
    SCERROR     DisposeSprite_(SCE_SPRITE *sprite);
    SCE_SPRITE *FindSprite_(pwide_t name) { return sprites_.Get(name); }
    void        DisplayKlass_(SCE_SKLASS&,bool);
    SCERROR     LayoutKlass_(SCE_SKLASS&);
    SCERROR     RestoreKlass_(SCE_SKLASS&);
    SCE_SKLASS *FindKlass_(pwide_t name) { return klasses_.Get(name); }
    void        SetLayerX(long no,float val);
    void        SetLayerY(long no,float val);
    void        SetLayerVisibility(long no,bool val);
    float       GetLayerX(long no) const;
    float       GetLayerY(long no) const;
    bool        IsLayerVisible(long no) const;

    SCERROR     ChangeSpriteKlassOn(SCE_SPRITE *sprite,pwide_t klass);
    void        RenderSprite(SCE_SPRITE *sprite,SCE_COMPLEX*);
    void        RenderSprite2(SCE_SPRITE *sprite,SCE_TRIANGLED*);
    SCERROR     UnloadAll();
  };

static SCE_SPRITES_::Gate FLATFX;

#endif // ___5d3e664a_70b8_462d_8f76_9da11a6245f8___
