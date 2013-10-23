
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

#ifndef ___5ca32535_5cee_47f1_afb3_07988a7b1672___
#define ___5ca32535_5cee_47f1_afb3_07988a7b1672___

//
//  Реализация спрайтового энжина
//

#include "flat_e.h"
#include "../moleskin2/mgraphics/m_gfx3d.h"

/// флаги спрайта
struct SPRITE_Info__
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

struct SPRITE_Info_ : SPRITE_Info__
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
    SPRITE_IMAGE_TYPE type;
    SPRITE_COLLIDE_TYPE collide;
  };

struct SPRITE_Info : SPRITE_Info_
  {
    widestring bitmap;
    widestring alpha;
    widestring symbols;
    SPRITE_Info()
      {
        memset((SPRITE_Info_*)this,0,sizeof(SPRITE_Info_));
      }
  };

SCERROR SPRITE_FillInfo(
  pwide_t kname,SPRITE_Info* info,pwide_t bitmap_resource,
  SPRITE_IMAGE_TYPE imgt=SPRITE_SOLID,u32_t crgb=0,
  bool filtering=false,bool cmprs=true,bool isfnt=false,
  int hor_layout=1, int ver_layout=1, pwide_t symbols=0);

struct ComplexSprite
  {
    struct Info
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
    static pchar_t const TypeID__;
    struct SpriteEl{int x,y,frno;};
    virtual bool ComplexSpriteReset(bool blendstate) = 0;
    virtual bool ComplexSpriteNext(Info* info, bool blendstate) = 0;
    virtual bool HasBlendState(bool) = 0;
  };

struct TriangledSprite
  {
    static pchar_t const TypeID__;
    virtual void TriangledImageType(SPRITE_IMAGE_TYPE* spritetype) = 0;
    virtual bool TriangledUpdate(
                    SPRITE_IMAGE_TYPE* spritetype, bool* colorized,
                    BufferT<VxB_VERTEX>& vertices) = 0;
  };

struct LWSA_MICROSPRITE
  {    /* 32 bytes */
    float   x,y,angle;    /*12*/
    float   sinv;         /*4*/
    float   cosv;         /*4*/
    float   scale;        /*4*/
    float   yscale;       /*4*/
    struct
      {
        unsigned flags      :8;  /*1*/
        unsigned fr_no      :8;  /*1*/
        unsigned alphaCf    :16; /*2*/
      };
  };

struct LwsArrayBasic : /*private*/ ComplexSprite
  {
    struct
      {
        bool is_centerd_          : 1;
        bool blended_sprites_     : 1;
        bool regular_sprites_     : 1;
        bool need_check_blending_ : 1;
        bool semitransparent_     : 1;
      };

    BufferT<LWSA_MICROSPRITE> range_;
    LWSA_MICROSPRITE filler_;
    LWSA_MICROSPRITE *iter_;
    LWSA_MICROSPRITE *iterE_;
    LwsArrayBasic()
      {
        is_centerd_ = true;
        memset(&filler_,0,sizeof(filler_));
        filler_.scale = 1;
        filler_.yscale = 1;
        filler_.alphaCf = 1000;
        blended_sprites_ = 0;
        regular_sprites_ = 0;
        need_check_blending_ = false;
        semitransparent_ = false;
      }
    virtual ~LwsArrayBasic() {}

    SCERROR LwsResize(long count);
    SCERROR LwsShow(long noB,long count,bool on);
    SCERROR LwsGetOneXY(long no,float* x,float* y);
    SCERROR LwsGetOneAngle(long no,float* angle);
    SCERROR LwsGetOneScale(long no,float* scale);
    SCERROR LwsGetOneTransparency(long no,float* transparency);
    SCERROR LwsGetOneFrno(long no,long* frno);
    SCERROR LwsScaleRangeDif(long no,long count,float dif);
    SCERROR LwsBlendRangeDif(long no,long count,float dif);
    SCERROR LwsMoveRangeDif(long no,long count,float difX,float difY);
    SCERROR LwsAngleRangeDif(long no,long count,float dif);
    SCERROR LwsFrnoRangeDif(long no,long count,long diff);
    SCERROR LwsSetRangeXY(long no,long count,float x,float y);
    SCERROR LwsSetRangeAngle(long no,long count,float angle);
    SCERROR LwsSetRangeScale(long no,long count,float scale);
    SCERROR LwsSetRangeTransparency(long no,long count,float transparency);
    SCERROR LwsSetRangeFrno(long no,long count,long frno);
    SCERROR LwsSetRangeIndep(long no,long count,bool indep);
    SCERROR LwsCopyRange(long dst,long src,long count);
    long        LwsCount() { return range_.Count(); }

    SCERROR LwsSetOne(long no,float x,float y,float scale,float angle,float transparency,long frno);
    SCERROR LwsGetOne(long no,float* x,float* y,float* scale,float* angle,float* transparency,long* frno);

    LWSA_MICROSPRITE* LwsGetRange(unsigned* count)
      {
        *count = range_.Count();
        if ( *count )
          return &range_[0];
        else
          return 0;
      }

    void NeedCheckBlendingState() { need_check_blending_ = true; }
    void Semitransparent(bool val) { semitransparent_ = true; }

    ComplexSprite* GetComplexSprite() { return this; }
  private:
    virtual bool  ComplexSpriteReset(bool);
    virtual bool  ComplexSpriteNext(ComplexSprite::Info* info,bool);
    virtual bool  HasBlendState(bool state);
  };

struct SPRITES_Inst : public IRefObject
  {
    SPRITES_Inst()
      {
        REQUIRE ( SPRITES::instance_ == 0 );
        SPRITES::instance_ = this;
      }

    virtual ~SPRITES_Inst()
      {
        REQUIRE ( SPRITES::instance_ == this );
        SPRITES::instance_ = 0;
      }
    virtual SCERROR Restore() = 0;
    virtual SCERROR RestoreKlass(pwide_t klass) = 0;
    virtual SCERROR Purge()   = 0;
    virtual SCERROR Display() = 0;
    virtual SCERROR NewKlass(pwide_t name,const SPRITE_Info&) = 0;
    virtual SCERROR NewDynamicKlass(pwide_t klass,KlassProvider*) = 0;
    virtual SCERROR UnloadKlass(pwide_t name,bool) = 0;
    virtual SCERROR UpdateKlass(pwide_t name) = 0;
    virtual bool        HasKlass(pwide_t klass) = 0;
    virtual Sprite*     NewSprite(pwide_t name,pwide_t klass,int layer) = 0;
    virtual SCERROR DisposeSprite(pwide_t name) = 0;
    virtual Sprite*     FindSprite(pwide_t name) = 0;
    virtual Sprite*     FindSpriteAt(long x,long y,long nearlayer,long farlayer) = 0;
    virtual void        SetNeedRefresh(bool val = true)= 0;
    virtual bool        NeedRefresh() const = 0;
    virtual SCERROR EnumerateSprites(ArrayT<Sprite*>&,long lnear = LONG_MIN,long lfar = LONG_MAX) = 0;
    virtual SCERROR GetKlassInfo(pwide_t name,SPRITE_Info& info) = 0;
    virtual void        SetLayerX(long no,float val) = 0;
    virtual void        SetLayerY(long no,float val) = 0;
    virtual void        SetLayerVisibility(long no,bool val) = 0;
    virtual float       GetLayerX(long no) const = 0;
    virtual float       GetLayerY(long no) const = 0;
    virtual bool        IsLayerVisible(long no) const = 0;
    virtual SCERROR UnloadAll() = 0;
  protected:
    void AfterConstract()
      {
      }
    void BeforeDestract()
      {
      }
  };

extern SPRITES_Inst* Create3DSpritesInst();

struct SPRITE_Info;
struct KlassProvider
  {
    virtual void FillInfo(SPRITE_Info *info) = 0;
    virtual TFFt_BITMAP *GetBitmap() = 0;
  };


#endif // ___5ca32535_5cee_47f1_afb3_07988a7b1672___
