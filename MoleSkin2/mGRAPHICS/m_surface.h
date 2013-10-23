
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


#if !defined (__m_surface_h__)
#define __m_surface_h__


#include "../mcore/m_autoconf.h"
#include "../mcore/m_debug.h"
#include "../mcore/m_irefobj.h"
#include "../mstreams/m_istream.h"
#include "../mstreams/m_dsource.h"
#include "m_types.h"
#include "m_color.h"
#include "../../Media/lib.ddraw/ddraw_inst.h"

NAMESPACE_MSKIN_BEGIN

class Surface;

/**
  \interface SurfaceRestorer
  Абстрактный класс востанавливающий содержимое поверхности по требыванию
*/
class MOLE_NOVTABLE SurfaceRestorer : public virtual IRefObject{
public:
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  /// Определяемая пользователем функция
  virtual SCERROR Restore(Surface*) = 0;
protected:
  SurfaceRestorer() {}
  virtual ~SurfaceRestorer() {}
private:
  SurfaceRestorer(const SurfaceRestorer&);
  SurfaceRestorer& operator=(const SurfaceRestorer&);
};

/**
  \interface Surface
  Абстракция графической поверхности
*/
class MOLE_NOVTABLE Surface : public virtual IRefObject
{
public:
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  enum RestoreBehaviour { DONOTRESTORECONTENT, RESTORECONTENT };
  //static const unsigned format_depth[5]; // всего форматов - 4 + SURFMT_DEFAULT
  enum {
    SURFMT_DEFAULT= 0,
    SURFMT_DEFAULT_DEPTH = 0,
    RGB5x5        = 1,
    RGB5x5_DEPTH  = 16,
    ALPHA         = 2,
    ALPHA_DEPTH   = 8,
    RGBx32        = 3,
    RGBx32_DEPTH  = 32,
    PAL           = 4,
    PAL_DEPTH     = 8,
    RGBx24        = 5,
    RGBx24_DEPTH  = 24,
    AUTODETECT    = 0x0ff,
    FORMATBITS    = 0x00ff,
    IMPLBITS      = 0x0300,
    SYSTEM        = 0x0000,
    ACCELERATE    = 0x0100,
    PREFERRED     = 0x0300,
    ALPHALAYER    = 0x1000,
    DOWNUP        = 0x2000,
    SHORTGREEN    = 0x4000,
    ALPHABITS     = 0x8000,
    SYS_RGB5x5    = SYSTEM|RGB5x5,
    SYS_RGBx32    = SYSTEM|RGBx32,
    SYS_RGBx24    = SYSTEM|RGBx24,
    SYS_PAL       = SYSTEM|PAL,
    SYS_ALPHA     = SYSTEM|ALPHA,
    ACCEL_RGB5x5  = ACCELERATE|RGB5x5,
    ACCEL_RGBx32  = ACCELERATE|RGBx32,
    ACCEL_PAL     = ACCELERATE|PAL,
    ACCEL_ALPHA   = ACCELERATE|ALPHA,
    PREF_RGB5x5   = PREFERRED|RGB5x5,
    PREF_RGBx32   = PREFERRED|RGBx32,
    PREF_PAL      = PREFERRED|PAL,
    PREF_ALPHA    = PREFERRED|ALPHA,
  };
  static inline unsigned GET_PIXFORMAT(unsigned fmt) { return fmt & FORMATBITS; }
  static inline unsigned SET_PIXFORMAT(unsigned fmt, unsigned pixfmt ) {
    return (fmt & (~FORMATBITS)) | (pixfmt&FORMATBITS);
  }
  /// ширина поверхности в пикселах
  virtual int     GetWidth() const = 0;
  /// высота поверхности в пикселах
  virtual int     GetHeight() const = 0;
  /// область в которой можно рисовать
  virtual rect_t  GetRect() const = 0;
  /// формат поверхности
  virtual unsigned GetFormat() const = 0;
  /// возвращает истину если ресурсы ассоциированные с поверхностью потеряны
  /// и требуется их восстановление
  virtual bool    IsLost() const = 0;
  /// блокирует поверхность в памяти и возвращает адрес
  virtual SCERROR Lock(void** p,int* stride, RestoreBehaviour = RESTORECONTENT) = 0;
  /// разблокирует поверхность
  virtual SCERROR Unlock() = 0;
  /// увеличивает количество ссылок на ресторер если будет его использовать
  virtual void SetRestorer(/*autoref*/SurfaceRestorer*) = 0;
  /// устанавливает значение цвета воспринимаемого как прозрачный
  virtual void SetTransparent(u32_t) = 0;
  /// возвращает значение цвета воспринимаемого как прозрачный
  virtual u32_t GetTransparent() = 0;
  /// возвращает значение пиксела
  virtual u32_t GetPixel(int x,int y) = 0;
  /// заставляет объект освободить занимаемые ресурсы
  virtual void Drop() = 0;
  /// если сурфейс "потерян" восстанавливает его
  virtual SCERROR Restore() = 0;
  /// копирует содержимое аргумента на сурфес
  /// \param from источник копирования
  /// \param x кордината по оси X по которой будет сделано копирование
  /// \param y кордината по оси Y по которой будет сделано копирование
  /// \param rect область которая будет скопированна
  virtual SCERROR BitBlt(Surface* from,int x = 0,int y = 0,const rect_t* rect = 0) = 0;
  /// копирует содержимое аргумента на сурфес c учетом "прозрачного" цвета
  /// \param from источник копирования
  /// \param transparent цвет который будет считатся прозрачным
  /// \param x кордината по оси X по которой будет сделано копирование
  /// \param y кордината по оси Y по которой будет сделано копирование
  /// \param rect область которая будет скопированна
  virtual SCERROR TransparentBlt(Surface* from,int x = 0, int y = 0,const rect_t* rect = 0) = 0;
  /// копирует содержимое аргумента на сурфес c учетом "прозрачного" цвета
  /// \param from источник копирования
  /// \param alpha поверхность выполняющая роль alpha-канала
  /// \param x кордината по оси X по которой будет сделано копирование
  /// \param y кордината по оси Y по которой будет сделано копирование
  /// \param rect область которая будет скопированна
  virtual SCERROR AlphaBlt(Surface* from,Surface* alpha,int x = 0, int y = 0,const rect_t* rect = 0) = 0;
  /// копирует содержимое аргумента на сурфес c учетом "прозрачного" цвета
  /// \param from источник копирования
  /// \param alpha значение прозраности
  /// \param x кордината по оси X по которой будет сделано копирование
  /// \param y кордината по оси Y по которой будет сделано копирование
  /// \param rect область которая будет скопированна
  virtual SCERROR AlphaValBlt(Surface* from,unsigned alpha,int x = 0, int y = 0,const rect_t* rect = 0) = 0;
  /// закрашивает заданную область указанным цветом
  virtual SCERROR FillRect(u32_t,const rect_t&) = 0;
  /// закрашивает всю поверхность указанным цветом
  virtual SCERROR Clear(u32_t) = 0;
  /// выполняет загрузку изображения из некоторого источника
  virtual SCERROR Load(DataSource* source) = 0;
  /// выполняет загрузку изображения по имени ресурса
  SCERROR Load(pwide_t source);
  /// устанавливает палитру (только для сурфесов с форматом PAL)
  virtual SCERROR SetPal(const BufferT<mrgba_t>& pal/*[256]*/) = 0;
  /// зачитывает палитру сурфейса (только для сурфесов с форматом PAL)
  virtual SCERROR GetPal(BufferT<mrgba_t>& pal/*[256]*/) const = 0;
  /// преобразует значение внутреннего формата цвета пиксела в mcolor_t
  virtual mrgba_t Pixel2Color(u32_t pixel) const = 0;
  /// преобразует значение mcolor_t во внутренний формат цвета пиксела
  virtual u32_t Color2Pixel(mrgba_t color) const = 0;
protected:
  Surface() {}
  virtual ~Surface() {}
private:
  Surface(const Surface&);
  Surface& operator=(const Surface&);
public:
  static SCERROR BitBlt_(Surface* to,Surface* from,int x = 0, int y = 0,const rect_t* rect = 0);
  static SCERROR TransparentBlt_(Surface* to,Surface* from,u32_t transparent,int x = 0, int y = 0,const rect_t* rect = 0);
  static SCERROR AlphaBlt_(Surface* to,Surface* from,Surface* alpha,int x = 0, int y = 0,const rect_t* rect = 0);
  static SCERROR AlphaBlt3a_(Surface* to,int x, int y,Surface* back,const rect_t* brect,Surface* fore,const rect_t* frect,Surface* alpha=0);
  static SCERROR AlphaValBlt_(Surface* to,Surface* from,unsigned alpha,int x = 0, int y = 0,const rect_t* rect = 0);
  static SCERROR FillRect_(Surface*,u32_t,const rect_t&);
  static SCERROR Clear_(Surface*,u32_t);
  static SCERROR Load_(Surface*,DataSource* source);
  static mrgba_t Pixel2Color_(const Surface*,u32_t pixel);
  static u32_t Color2Pixel_(const Surface*,mrgba_t color);
  static SCERROR CopyAlphaBits_(Surface* to,Surface* from);
  static SCERROR SetAlphaBitsByColorKey_(Surface*,mrgb_t colorkey);
  static SCERROR Surface::StretchBlt_(Surface* to,const rect_t& rin,Surface* from,const rect_t& rout);
};

typedef auto_ref<Surface> AutoSurface;

/**
  \class BasicSurface
*/
class BasicSurface: public Surface {
protected:
  u32_t transparent_;
  BasicSurface() {}
  virtual ~BasicSurface() {}
  virtual void* GetMemory(int& stride,RestoreBehaviour = RESTORECONTENT);
public:
  virtual void* Adaptate_(const void*);
  virtual widestring ToString_() const;
  //virtual void Grab_() const;
  //virtual void Release_() const;
  virtual bool IsLost() const;
  virtual void SetRestorer(/*autoref*/SurfaceRestorer*);
  virtual void SetTransparent(u32_t t);
  virtual u32_t GetTransparent();
  virtual u32_t GetPixel(int x,int y);
  virtual SCERROR Lock(void** p,int* stride, RestoreBehaviour = RESTORECONTENT);
  virtual SCERROR Unlock();
  virtual void Drop();
  virtual SCERROR Restore();
  virtual SCERROR BitBlt(Surface*,int,int,const rect_t*);
  virtual SCERROR TransparentBlt(Surface*,int,int,const rect_t*);
  virtual SCERROR AlphaBlt(Surface*,Surface*,int,int,const rect_t*);
  virtual SCERROR AlphaValBlt(Surface*,unsigned,int,int,const rect_t*);
  virtual SCERROR FillRect(u32_t,const rect_t&);
  virtual SCERROR Clear(u32_t);
  virtual SCERROR Load(DataSource* source);
  virtual SCERROR SetPal(const BufferT<mrgba_t>& pal/*[256]*/);
  virtual SCERROR GetPal(BufferT<mrgba_t>& pal/*[256]*/) const;
  virtual mrgba_t Pixel2Color(u32_t pixel) const;
  virtual u32_t Color2Pixel(mrgba_t color) const;
};

struct MultiLayerSurface : public virtual Adaptable{
  //static const typeid_t* TypeID__;
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual Surface* GetMainSurface() = 0;
  virtual Surface* GetAlphaSurface() = 0;
};

class GenericSurface : public BasicSurface, public MultiLayerSurface, private refbase {
  unsigned width_,height_;
  unsigned format_;
  auto_ref<SurfaceRestorer> restorer_;
  byte_t* memory_;
  int memsize_;
  auto_ref<Surface> alpha_;
  BufferT<mrgba_t> pal_;
public:
  static pchar_t const TypeID__;
  // Adaptable
  virtual void*   Adaptate_(const void*);
  virtual widestring  ToString_() const;
  // IRefObject
  virtual void    Release_() const;
  virtual void    Grab_() const;
  // Surface
  virtual int     GetWidth() const;
  virtual int     GetHeight() const;
  virtual rect_t  GetRect() const;
  virtual unsigned GetFormat() const;
  virtual void*   GetMemory(int& stride,Surface::RestoreBehaviour);
  virtual void    SetRestorer(SurfaceRestorer* rest);
  virtual bool    IsLost() const;
  virtual void    Drop();
  virtual SCERROR Restore();
  virtual SCERROR SetPal(const BufferT<mrgba_t>& pal/*[256]*/);
  virtual SCERROR GetPal(BufferT<mrgba_t>& pal/*[256]*/) const;
  // MultiLayerSurface
  virtual Surface* GetMainSurface();
  virtual Surface* GetAlphaSurface();
  // GenericSurface
  SCERROR  RestoreSurface(Surface::RestoreBehaviour bih);
  static GenericSurface* Create(int width,int height,unsigned format);
protected:
  GenericSurface(int width,int height,unsigned format);
  ~GenericSurface();
};

class TffSurface : public BasicSurface, private refbase
  {
  public:
    static pchar_t const TypeID__;
    // Adaptable
    virtual void*       Adaptate_(const void*);
    virtual widestring  ToString_() const;
    //
    TffSurface(TFFt_BITMAP *b) : bitmap_(Refe(b)) {}
    virtual void    Release_() const { refbase::Release_(); }
    virtual void    Grab_() const { refbase::Grab_(); }
    // Surface
    virtual int     GetWidth()  const { return bitmap_->Width(); }
    virtual int     GetHeight() const { return bitmap_->Height(); }
    virtual rect_t  GetRect()   const { return rect_t(0,0,bitmap_->Width(),bitmap_->Height()); }
    virtual void*   GetMemory(int& stride,Surface::RestoreBehaviour) { TFFt_PIXEL_FORMAT pf; return bitmap_->Bits(&stride,&pf); }
    virtual void    SetRestorer(SurfaceRestorer* rest) {}
    virtual bool    IsLost() const { return false; }
    virtual void    Drop() {}
    virtual SCERROR Restore() { return SCE_OK; }
    virtual SCERROR SetPal(const BufferT<mrgba_t>& pal/*[256]*/) { return SCE_OK; }
    virtual SCERROR GetPal(BufferT<mrgba_t>& pal/*[256]*/) const { return SCE_OK; }
    virtual unsigned GetFormat() const
      {
        TFFt_PIXEL_FORMAT pf = bitmap_->PixelFormat();
        if ( (pf&TFF_PF_BPPMASK) == 32 ) return SYS_RGBx32;
        if ( (pf&TFF_PF_BPPMASK) == 24 ) return SYS_RGBx24;
        if ( (pf&TFF_PF_BPPMASK) == 16 ) return SYS_RGB5x5|SHORTGREEN;
        return 0;
      }
    unsigned TffPixelFormat() { return bitmap_->PixelFormat(); }
  protected:
    ~TffSurface() {}
  private:
    rcc_ptr<TFFt_BITMAP> bitmap_;
  };

/**
  класс автоматической блокировки сурфейса
*/
struct SurfLock
{
  void* mem_;
  int stride_;
  Surface* surf_;
  SCERROR ecode_;
public:
  SurfLock(Surface* surf, Surface::RestoreBehaviour bih = Surface::DONOTRESTORECONTENT ) : mem_(0), stride_(0)
  { Lock_(surf,bih); }
  SCERROR Lock(Surface* surf, Surface::RestoreBehaviour bih = Surface::DONOTRESTORECONTENT )
  { REQUIRE ( surf_ == 0 ); return Lock_(surf,bih); }
  ~SurfLock() {
    if ( surf_ ) {
      surf_->Unlock();
      Release(surf_);
    }
  }
  SurfLock() : mem_(0),surf_(0),ecode_(SCE_OK) {}
  SCERROR PeekError() { return ecode_; }
  void* GetMemory(int& stride) { stride = stride_; return mem_; }
  SurfLock* operator->() { return this; }
private:
  SurfLock(const SurfLock&);
  SurfLock& operator =(const SurfLock&);
  static void* operator new(unsigned size);
  SCERROR Lock_(Surface* surf, Surface::RestoreBehaviour bih = Surface::DONOTRESTORECONTENT )
  {
    if ( surf ) {
      surf_ = Grab(surf);
      ecode_ = surf->Lock(&mem_,&stride_,bih);
    }else{
      surf_ = 0;
      ecode_ = SCE_FAIL;
    }
    return ecode_;
  }
//  static void  operator delete(void*);
};

NAMESPACE_MSKIN_END

#endif // __m_surface_h__
