
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

#if !defined (__m_GraphicsDevice_renderer_h__)
#define __m_GraphicsDevice_renderer_h__

//#include "m_gfx.h"
#include "../mCORE/m_autoconf.h"
#include "../mCORE/m_irefobj.h"
#include "../nvmath/algebra.h"
#include "m_surface.h"
#include "m_gfx.h"


NAMESPACE_MSKIN_BEGIN

using nvmath::vec3;
using nvmath::mat4;
using nvmath::nv_scalar;

struct Texture;
struct VxB;

enum{
  GFX3D_TEX_DEFAULT        = 0,
  GFX3D_TEX_RGBA8          = 1,
  GFX3D_TEX_RGB5           = 2,
  GFX3D_TEX_PAL8           = 3,
  GFX3D_TEX_ALPHA          = 4,
  GFX3D_TEX_RGB5A1         = 5,
  GFX3D_TEX_RGBA4          = 6,
  GFX3D_TEX_AUTODETECT     = 0x0ff,
  GFX3D_VxB_VECTOR         = MSKIN_BIT(0),
  GFX3D_VxB_NORMAL         = MSKIN_BIT(1),
  GFX3D_VxB_TEXTURE        = MSKIN_BIT(2),
  GFX3D_VxB_COLOR          = MSKIN_BIT(3),
  GFX3D_VxB_STATIC         = MSKIN_BIT(4),
  GFX3D_VxB_DYNAMIC        = MSKIN_BIT(5),
  GFX3D_VxB_VNT            = GFX3D_VxB_VECTOR|GFX3D_VxB_NORMAL|GFX3D_VxB_TEXTURE,
  GFX3D_VxB_VCT            = GFX3D_VxB_VECTOR|GFX3D_VxB_COLOR|GFX3D_VxB_TEXTURE,
  GFX3D_VxB_VT             = GFX3D_VxB_VECTOR|GFX3D_VxB_TEXTURE,
  GFX3D_VxB_DEFAULT        = GFX3D_VxB_VNT
};

inline pwide_t GFX3D_TEXTYPE_TO_STRING(u32_t textype)
{
  switch(textype)
  {
  case GFX3D_TEX_DEFAULT: return L"default";
  case GFX3D_TEX_RGBA8:   return L"rgb8a8";
  case GFX3D_TEX_RGB5:    return L"rgb5";
  case GFX3D_TEX_PAL8:    return L"pal8";
  case GFX3D_TEX_ALPHA:   return L"alpha";
  case GFX3D_TEX_RGB5A1:  return L"rgb5a1";
  case GFX3D_TEX_RGBA4:   return L"rgb4a4";
  case GFX3D_TEX_AUTODETECT: return L"autodetect";
  }
  return L"unknown";
}

inline bool IS_TEX_PALETTED(unsigned fmt) {
  return fmt == GFX3D_TEX_PAL8;
}

enum GFX3D_STATE {
  GFX3D_STATE_BLENDING      ,
  GFX3D_STATE_DITHERING     ,
  GFX3D_STATE_MIPMAPPING    ,
//  GFX3D_STATE_TEXTURING    , для запрета текстурирования установить SetTеxture(0)
  GFX3D_STATE_LIGHTING      ,
// GFX3D_STATE_DEPTH_TEST    , 
// всергда вклчен, выключить невозможна , 
// так как в некоторых суперкривых OGL драйверах это приводит к зависанию системы
  GFX3D_STATE_ALPHA_TEST    ,
  GFX3D_STATE_DEPTH_FUNC    ,
  GFX3D_STATE_ALPHA_FUNC    ,
  GFX3D_STATE_BLEND_FUNC    ,
  GFX3D_STATE_WRAPST        ,
  GFX3D_STATE_MINMAGFILTER  ,
  GFX3D_STATE_ENVMODE       ,
  GFX3D_STATE_SHADINGMODE   ,
//  GFX3D_STATE_WRITE_BITS    ,
  GFX3D_STATE_WRITE_DEPTH   ,
//  GFX3D_STATE_CULLFACE      ,
  GFX3D_STATE_CULLFACE_MODE 
};

enum GFX3D_PRIMITIVE {
  GFX3D_QUADS        ,
  GFX3D_TRISTRIP     ,
  GFX3D_TRIFAN       ,
  GFX3D_TRILIST      
};

enum {
  GFX3D_CLEAR_ZBUFFER   = MSKIN_BIT(0),
  GFX3D_CLEAR_SCREEN    = MSKIN_BIT(1),
//  GFX3D_RED_BITS        = MSKIN_BIT(0),
//  GFX3D_GREEN_BITS      = MSKIN_BIT(1),
//  GFX3D_BLUE_BITS       = MSKIN_BIT(2),
//  GFX3D_ALPHA_BITS      = MSKIN_BIT(3),
//  GFX3D_COLOR_BITS      = GFX3D_RED_BITS|GFX3D_GREEN_BITS|GFX3D_BLUE_BITS,
//  GFX3D_NONE_BITS       = 0,
//  GFX3D_ALL_BITS        = GFX3D_COLOR_BITS|GFX3D_ALPHA_BITS,
};

enum {
  GFX3D_ENABLE            = GFX_ENABLE,
  GFX3D_DISABLE           = GFX_DISABLE,
  GFX3D_NONE_VALUE        = 1000,
  GFX3D_TEXTURE_DECAL     ,
  GFX3D_TEXTURE_MODULATE  ,
  GFX3D_TEXTURE_REPLACE   ,
  GFX3D_TEXTURE_BLEND     ,
  GFX3D_TEXTURE_CLAMP     ,
  GFX3D_TEXTURE_REPEAT    ,
  GFX3D_NEAREST           ,
  GFX3D_LINEAR            ,
  GFX3D_ANISOTROPIC       ,
  GFX3D_ZERO              ,
  GFX3D_ONE               ,
  GFX3D_DST_COLOR         ,
  GFX3D_SRC_COLOR         ,
  GFX3D_ONE_MINUS_DST_COLOR,
  GFX3D_ONE_MINUS_SRC_COLOR,
  GFX3D_DST_ALPHA         ,
  GFX3D_SRC_ALPHA         ,
  GFX3D_ONE_MINUS_DST_ALPHA,
  GFX3D_ONE_MINUS_SRC_ALPHA,
  GFX3D_NEVER             ,
  GFX3D_LESS              ,
  GFX3D_EQUAL             ,
  GFX3D_LEQUAL            ,
  GFX3D_GREATER           ,
  GFX3D_NOTEQUAL          ,
  GFX3D_GEQUAL            ,
  GFX3D_ALWAYS            ,
  GFX3D_FLAT              ,
  GFX3D_SMOOTH            ,
  GFX3D_NONE              ,
  GFX3D_BACK              ,
  GFX3D_FRONT             ,
  //GFX3D_FRONT_AND_BACK     
};

enum {
  GFX3D_LIGHT0        = 0,
  GFX3D_LIGHT1        ,
  GFX3D_LIGHT2        ,
  GFX3D_LIGHT3        ,
  GFX3D_LIGHT4        ,
  GFX3D_LIGHT5        ,
  GFX3D_LIGHT6        ,
  GFX3D_LIGHT7        ,
  GFX3D_LIGHTx        = 0x0fffffff,
};

static const float fRGBA_t_NORM = 1.0f/255.0f;
struct frgba_t { 
  float R,G,B,A; 
  float* Pfloat() const { return (float*)this; }
};
struct fRGBA_t : public frgba_t {
  inline float Norm(float c) { return c*fRGBA_t_NORM; }
  fRGBA_t(float r,float g,float b,float a=1) { R=r,G=g,B=b,A=a; }
  fRGBA_t(mrgb_t c) { R=Norm(c.R);G=Norm(c.G);B=Norm(c.B);A=1.0f; }    
  fRGBA_t(mrgba_t c) { R=Norm(c.R);G=Norm(c.G);B=Norm(c.B);A=Norm(c.A); }    
};

struct GFX3D_MATERIAL{
  frgba_t diffuse;
  frgba_t emission;
  frgba_t specular;
  float   shininess;
};

void FillBlackMaterial(GFX3D_MATERIAL* m);
void FillWhiteMaterial(GFX3D_MATERIAL* m);

enum GFX3D_LIGHT_TYPE {
  GFX3D_DIRECTION_LIGHT,
};

struct GFX3D_LIGHT{
  GFX3D_LIGHT_TYPE type;
  frgba_t diffuse;
  frgba_t specular;
  float   position[4];
};

struct VxB_VERTEX { /*32 байта*/
  static const VxB_VERTEX EMPTY;
  float vx,vy,vz; /*12*/
  union{          /*12*/
    struct{
      float nx,ny,nz;
    };
    struct{
      mrgba_t color;
      float ts,tt;
    };
  };
  float ts1,tt1;    /*8*/
  void Xyz(float x, float y, float z=0) { vx = x, vy = y, vz = z; }
  void Norm(float x, float y, float z=0) { nx = x, ny = y, nz = z; }
  void Tex(float s, float t)  { ts = s, tt = t; }
  void Tex1(float s, float t) { ts1 = s, tt1 = t; }
  void Colorf(float r,float g,float b,float a) { color = mRGBA_t(u32_t(r*255),u32_t(g*255),u32_t(b*255),u32_t(a*255)); }
  void Colori(int r,int g,int b,int a) { color = mRGBA_t(r,g,b,a); }
  void Color(mrgba_t c) { color = c; }
  void Transform(const mat4& m) { *(vec3*)&vx = m * *(vec3*)&vx; }
  void TransformVN(const mat4& m) { 
    *(vec3*)&vx = m * *(vec3*)&vx; 
    *(vec3*)&nx = m * *(vec3*)&nx; 
  }
};

extern const vec3 AXIS_X;
extern const vec3 AXIS_Y;
extern const vec3 AXIS_Z;

struct Gfx3DDevice : public virtual Adaptable {
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  
  virtual Texture*    CreateDirtyTexture(int width,int height,unsigned flags=GFX3D_TEX_DEFAULT,unsigned mipmap=0) = 0;
  virtual VxB*        CreateDirtyVxB(int count,unsigned flags=GFX3D_VxB_DEFAULT) = 0;
  virtual SCERROR SetState(GFX3D_STATE,u32_t val,float ref=0) = 0;
  virtual int         GetMaxTextureSize() const = 0;
  virtual SCERROR SetVxB(VxB* vxb) = 0;
  virtual SCERROR DrawVxB(GFX3D_PRIMITIVE,int first=0,int count=-1) = 0;
  virtual SCERROR DrawIndexedVxB(GFX3D_PRIMITIVE, const u16_t* idx, int ixcount,unsigned vxfirst=0,unsigned vxcount=0) = 0;
  virtual SCERROR DrawPrimitive(GFX3D_PRIMITIVE,const VxB_VERTEX*,int count,unsigned flags) = 0;
  virtual SCERROR DrawIndexedPrimitive(GFX3D_PRIMITIVE,const VxB_VERTEX*, unsigned vxcount,const u16_t* idx,int count,unsigned flags) = 0;
  virtual SCERROR SetFillerColor(float r, float g, float b) = 0;
          SCERROR SetFillerColor(const frgba_t& c) { return SetFillerColor(c.R,c.G,c.B); }
  virtual SCERROR SetFillerDepth(float depth) = 0;
  virtual SCERROR Clear(unsigned=GFX3D_CLEAR_SCREEN|GFX3D_CLEAR_ZBUFFER) = 0;
  virtual SCERROR SetTexture(Texture*) = 0;
  virtual SCERROR SetMaterial(const GFX3D_MATERIAL*) = 0;
  virtual SCERROR SetAmbientLight(float r,float g,float b,float a=1) = 0;
          SCERROR SetAmbientLight(const frgba_t& c) {return SetAmbientLight(c.R,c.G,c.B,c.A);}
  virtual SCERROR LightON(unsigned no, bool=true) = 0;
          SCERROR LightOFF(unsigned no) { return LightON(no,false); }
  virtual SCERROR SetLight(unsigned no,const GFX3D_LIGHT*) = 0;
  virtual SCERROR Ortho2D(float width=0,float height=0,float zNear=-1,float zFar=1) = 0;
  virtual SCERROR Perspective(float fovy=90,float scale=1,float zNear=-1,float zFar=-1,float aspect=-1) = 0;
  virtual SCERROR Translate(float x,float y, float z=0) = 0;
          SCERROR Translate(const vec3& v) {return Translate(v.x,v.y,v.z);}
  virtual SCERROR Scale(float x,float y, float z=0) = 0;
          SCERROR Scale(float x) { return Scale(x,x,x); }
  virtual SCERROR Rotate(float degree,float x=0,float y=0,float z=1) = 0;
  virtual SCERROR RotateRad(float readian,float x=0,float y=0,float z=1) = 0;
          SCERROR Rotate(float angle,const vec3& v) { return Rotate(angle,v.x,v.y,v.z); }
          SCERROR RotateRad(float angle,const vec3& v) { return RotateRad(angle,v.x,v.y,v.z); }
  virtual SCERROR PushTransform() = 0;
  virtual SCERROR PopTransform(int depth=0)  = 0;
  virtual SCERROR TopTransform()  = 0;
  virtual SCERROR FlushTransforms()  = 0;
  virtual SCERROR Transform(const mat4& m16) = 0;

  virtual SCERROR LookAt(const vec3& eye, const vec3& point, const vec3& up );
  virtual SCERROR LookTo(const vec3& point, float distance, float longitude, float lantitude, float tetha = 0 );
  virtual SCERROR LookFrom(const vec3& eye, float longitude, float lantitude, float tetha = 0 );

protected:
  Gfx3DDevice();
  virtual ~Gfx3DDevice();
private:
  Gfx3DDevice(const Gfx3DDevice&);
  Gfx3DDevice& operator=(const Gfx3DDevice&);
};

struct VxB : public virtual IRefObject {
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual int Count() const = 0;
  virtual SCERROR  Fill(const VxB_VERTEX* vertices, int first, int count) = 0;
protected:
  VxB();
  virtual ~VxB();
private:
  VxB(const VxB&);
  VxB& operator=(VxB&);
};
typedef auto_ref<VxB> AutoVxB;

struct Texture : public virtual IRefObject{
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual SCERROR Load(Surface*,int level=0) = 0;
  virtual SCERROR LoadSubpicture(Surface*,int x,int y,const rect_t&,int level = 0) = 0;
  virtual SCERROR LoadPalette(const BufferT<mrgba_t>& pal) = 0;
  virtual SCERROR StoreSubpicture(Surface*,int x,int y,const rect_t&) const = 0;
  virtual int         GetWidth() const = 0;
  virtual int         GetHeight() const = 0;
  virtual unsigned    GetFormat() const = 0;
  virtual bool        IsLost() const = 0;
  virtual SCERROR Restore() = 0;
protected:
  Texture();
  virtual ~Texture();
private:
  Texture(const Texture&);
  Texture& operator=(const Texture&);
};
typedef auto_ref<Texture> AutoTexture;

struct ProxyTexture_OfTextureSet;
struct TextureSet : public refbase {
  static TextureSet* AcquireSet();
  static void SetFormat(unsigned texfmt);
  static SCERROR Restore();
  Texture* Acquire_(pwide_t name);
  static Texture* Acquire(pwide_t name) {
    return AutoRef(AcquireSet())->Acquire_(name);
  }
private:
  typedef CollectionT<ProxyTexture_OfTextureSet*> TEXTURES;
  TEXTURES texmap_;
  static unsigned texfmtdef_; /*GFX3D_TEX_AUTODETECT by default*/
protected:
  static TextureSet* instance_;
  friend struct ProxyTexture_OfTextureSet;
  void UnAcquireTexture(ProxyTexture_OfTextureSet*);
  TextureSet();
  virtual ~TextureSet();
};
typedef auto_ref<TextureSet> AutoTextureSet;

Texture* CreateTextureFrom(pwide_t resname,unsigned=GFX3D_TEX_AUTODETECT,int width=0,int height=0);
Texture* CreateTextureFrom(DataSource*,unsigned=GFX3D_TEX_AUTODETECT,int width=0,int height=0);
Texture* AcquireTextureFromSet(pwide_t name);
unsigned RecognizeSurfaceFormat(unsigned& texfmt);

struct MaterialInfo;
struct Model3D;
struct Object3D;
typedef auto_ref<MaterialInfo>  AutoMaterialInfo;
typedef auto_ref<Model3D>       AutoModel3D;
typedef auto_ref<Object3D>      AutoObject3D;

struct MaterialInfo : public virtual IRefObject, private refbase {
  widestring      name_;            // имя материала (уникально для модели)
  AutoTexture     texture_;         // текстура материала (диффузионная)
  float           texture_percent_; // влияние текстуры на материал (непрозрачность) [0..1]
  float           transparency_;    // непрозрачность материала [0..1]
  GFX3D_MATERIAL  colors_;          // цветовые характеристики материала
  virtual void Release_() const;
  virtual void Grab_() const;
  MaterialInfo();
  virtual ~MaterialInfo();
};
typedef auto_ref<MaterialInfo> AutoMaterialInfo;

struct Object3D : public virtual IRefObject, private refbase {
  struct SLICE {
    u16_t slice_;        // индекс слайса
    u16_t ixfirst_;      // первый индекс вертекса в слайсе
    u16_t ixcount_;      // количество индексов вертексов (кратно трем, задает треугольники)
  };

  widestring      name_;    // имя обьекта
  mat4            oord_;    // матрица перевода в локальную систему координат обьекта
  mat4            absT_;    // абсолютная матрица трансформации для кадра 0
  mat4            relT_;    // относительная матрица трансформации для кадра 0
  vec3            pivot_;
  u16_t        vxfirst_; // первый элемент в массиве вертексов
  u16_t        vxcount_; // количество вертексов
  Model3D*        model_;   // модель которой пренадлежит обьект
  BufferT<SLICE>   faces_;   // индексы вертексов (треугольники) для обьекта
  BufferT<mat4>    track_;   // анимационные матрицы трансформации вертексов обьекта от кадра 1
  Object3D*       parent_;  // родительсктий обьект 
  Object3D*       refer_;   // ссылка на описаие обьекта (для обьектов инстансов)
  BufferT<Object3D*>     hierarchy_;  // иерархия подобьектов
  virtual void Release_() const;
  virtual void Grab_() const;
  
  Object3D();
  virtual ~Object3D();
  /** генерирует вертексы для указанного кадра */
  void GenVertices(VxB_VERTEX*, u16_t frame) const;
  /** вычисляет нормали вертексов для model->vertices_ */
  void ComputeNormals();
  /** вычисляет нормали вертексов */
  void ComputeNormals(VxB_VERTEX* vertices) const;
  /** рисует обьект в каде frame используюя массив вертексов модели */
  void Draw() const;
  /// рисует обьект в каде frame используюя текущий вертекс буффер 
  /// вертекс буффер строится через model->FillVxB и ТОЛЬКО так
  void DrawVxB(unsigned vxfirst = 0 ) const;
  /** возвращает матрицу преобразований для указанного кадра */
  const mat4& GetAbsTransform(u16_t frame) const;
  /** изменяет матрицу преобразований рендерера для указанного кадра */
  void Transform(u16_t frame=0) const;
  /** изменяет матрицу преобразований рендерера для кадра 0*/
  void RelTransform() const;
  
  u16_t  GetVerticesCount() const { return  vxcount_; }
  void Draw_(bool = false, unsigned = 0) const;
};

typedef BufferT<u16_t>    INDICES;
typedef BufferT<VxB_VERTEX>  VERTICES;

struct Model3D : public virtual IRefObject, private refbase {
  struct SLICE {
    INDICES           indices_;
    AutoMaterialInfo  material_; 
    int Cmp ( pwide_t name) const {
      if ( !material_ ) return !name?0:-1; // для фейсов у которых материал отсутствует
      if ( name == 0 )  return 1;
      return material_->name_.Cmp(name);
    }
    bool operator < ( const SLICE& slice) const
      {return Cmp(!slice.material_?0:slice.material_->name_.Str()) < 0;}
    widestring GetMaterialName() const {
      if ( !material_ ) return widestring("<NOMATERIAL>");
      return material_->name_;
    }
  };
/*  struct SLICE_Less : public binary_function<bool,SLICE,SLICE> {
    bool operator()(pwide_t name,const SLICE& slice1) const{ return slice1.Cmp(name) > 0; }
    bool operator()(const SLICE& slice0,pwide_t name) const{ return slice0.Cmp(name) < 0; }
    bool operator()(const SLICE& slice0,const SLICE& slice1) const{ return slice0 < slice1; }
  }; */
  widestring                  source_;    // имя модели
  float                       scale_;     // масштаб
  u16_t                    frames_;    // общее количество кадров анимации
  VERTICES                    vertices_;  // список вертексов модели
  mutable VERTICES            cache_vertices_;
  mutable u16_t            cache_frame_;
  BufferT<AutoObject3D>        objects_;   // список обьектов модели
  BufferT<SLICE>               slice_;     // расслоение модели по материалам
  BufferT<Object3D*>           hierarchy_; // иерархия обьектов модели
  virtual void Release_() const;
  virtual void Grab_() const;
  Model3D();
  virtual ~Model3D();
  /** находит слайс по имени материала */
  SLICE* FindSliceByMaterial(pwide_t name) const;
  /** находит обьект по имени */
  Object3D* FindObjectByName(pwide_t name) const;
  /** генерирует вертексы для указанного кадра */
  void GenVertices(VxB_VERTEX*, u16_t frame) const;
  /** генерирует вертексы для указанного кадра */
  void GenVertices(VERTICES& vertices,u16_t frame) const;
  /** генерирует вертексы для указанного кадра */
  void GenVxB(u16_t frame,VxB* vxb,unsigned first=0) const;
  /** вычисляет нормали для vertices_ */
  void ComputeNormals();
  /** вычисляет нормали для массива вертексов сгенерированного через GenVertices */
  void ComputeNormals(VxB_VERTEX* vertices) const;
  /** вычисляет нормали для массива вертексов сгенерированного через GenVertices */
  void ComputeNormals(VERTICES& vertices) const;
  /** отрисовывает модель в указанном кадре */
  void Draw(u16_t frame = 0) const;
  /// отрисовывает модель в указанном кадре используя текущий вертекс буффер 
  /// эта функция рисует ТОЛЬКО вертекс буфферы сгенерированные для кадров анимации
  void DrawVxB( unsigned first = 0 ) const;
  /** заполняет вертекс буффер вертексами модели */
  void FillVxB(VxB* vxb, unsigned first = 0 ) const;

  u16_t  GetVerticesCount() const { return vertices_.size(); }

  void Draw_(bool = false, unsigned = 0) const;
};

/*

  !!! ВАЖНО !!!

  отличия между Object3D::DrawVxB и Model3D::DrawVxB
  в том что первая рисует вертекс буффер построенный по сырому массиву вертексов (FillVxB)
  а вторая по сгенерированному для некоторого кадра анимации (GenVxB)

*/

void MOLE_FUNCALL ComputeNormals(
  VxB_VERTEX* vertices,int vcount,const u16_t* indices,int icount);
void MOLE_FUNCALL ComputeNormals(
  VERTICES& vertices, const INDICES& indices);

static const float epsilon__ = float(1e-5);
//static const u32_t lepsilon_ = 0;

inline bool EPSILON_EQUAL(float a,float b) {
  const float ab = fabs(a-b);
  return ab < epsilon__;
}

inline bool EPSILON_TEST(float a,float b) {
  const float ab = fabs(a-b);
  return ab > epsilon__;
}

inline bool EPSILON_EQUAL0(float a) {
  return fabs(a) < epsilon__;
}

inline float Radians(float degree) { return degree*(nv_pi/180.0f); }
inline float Degree(float radians) { return radians*(180.0f/nv_pi); }

struct IGfxTexture;
struct IGfxVxB;

//struct IGfxTexture : TeggoInterface<IUnknwon>
//  {
//    typedef GuidT<0x3e9b0363,0x407e,0x4a7b,
//                  0xbd, 0x56, 0xaf, 0x51, 0xdd, 0x38, 0x28, 0xc6> Guid;
//
//    virtual HRESULT __stdcall Load(Surface*,int level=0) = 0;
//    virtual HRESULT __stdcall LoadSubpicture(Surface*,int x,int y,rect_t const*,int level = 0) = 0;
//    virtual HRESULT __stdcall LoadPalette(mrgba_t const *pal, unsigned count) = 0;
//    virtual HRESULT __stdcall StoreSubpicture(Surface*,int x,int y,const rect_t&) = 0;
//    virtual HRESULT __stdcall GetWidth(u32_t *width) = 0;
//    virtual HRESULT __stdcall GetHeight(u32_t *height) = 0;
//    virtual HRESULT __stdcall GetFormat(u32_t *format) = 0;
//    virtual HRESULT __stdcall IsLost() const = 0;
//    virtual HRESULT __stdcall Restore() = 0;
//  };
//
//struct IGfxVxB : TeggoInterface<IUnknown> 
//  {
//    typedef GuidT<0x545d5da1,0x425c,0x431a,
//                  0x97, 0x2e, 0x81, 0x06, 0x8a, 0x9c, 0x7c, 0x2b> Guid;
//
//    virtual HRESULT __stdcall Count(u32_t count) = 0;
//    virtual HRESULT __stdcall Fill(const VxB_VERTEX* vertices, int first, int count) = 0;
//  };

NAMESPACE_MSKIN_END

#endif //__m_GraphicsDevice_renderer_h__
