
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

#include "precompile.h"
#include "m_sprites.h"
#include "m_sprites0.h"
#include "m_loadsurf.h"
#include "m_screen.h"
#include "../mUTIL/m_collection.h"
#include "../mUTIL/m_widestring.h"

#include <limits.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

struct TileMap_Impl;

struct TileMapFactory : public BaseTileMapFactory {
  TileMapFactory(SPRITES_Inst*);
  ~TileMapFactory();
  void Purge();
  TileMap* NewTileMap(pwide_t name,unsigned rows,unsigned columns,unsigned tileSize,int layer);
  TileMap* FindTileMap(pwide_t name);
  SCERROR DisposeTileMap(pwide_t name);
  virtual SCERROR Enumerate(BufferT<TileMap*>&);
  SPRITES_Inst* eng_;
private:
  CollectionT<TileMap_Impl> tilmaps_;
};

BaseTileMapFactory* CreateTileMapFactory(SPRITES_Inst* eng){
  return new TileMapFactory(eng);
}

struct TileMap_Impl : public TileMap {
  virtual SCERROR Show();
  virtual SCERROR Hide();
  virtual void        Move(float x,float y);
  virtual void        Scale(float percent);
  virtual void        ScaleX(float percent);
  virtual void        ScaleY(float percent);
  virtual float       GetX() const;
  virtual float       GetY() const;
  virtual int         GetRows() const;
  virtual int         GetColumns() const;
  virtual void        SetTileSize(unsigned);
  virtual unsigned    GetTileSize() const;
  virtual void        SetTileSizeX(unsigned);
  virtual unsigned    GetTileSizeX() const;
  virtual void        SetTileSizeY(unsigned);
  virtual unsigned    GetTileSizeY() const;
  virtual bool        IsVisible() const;
  virtual long        GetLayer()  const;
  virtual void        SetLayer(long);
  virtual long        GetSubLayer() const;
  virtual void        SetSubLayer(long);
  virtual float       GetTransparency() const;
  virtual void        SetTransparency(float percent);
  virtual float       GetLuminance() const;
  virtual void        SetLuminance(float percent);
  virtual float       GetScaleX() const;
  virtual float       GetScaleY() const;
  virtual void        Dispose();
  virtual void        SetSubImpl(TileMapSubImpl*);
  virtual TileMapSubImpl* GetSubImpl() const;
  virtual mrgb_t      GetRGB() const;
  virtual void        SetRGB(mrgb_t);
	virtual SCERROR UseTilesFrom(pwide_t klass,int frno,int count,long* no);
	virtual void        Clear();
	virtual SCERROR SetTile(unsigned row,unsigned col,int tileno);
	virtual int         GetTile(unsigned row,unsigned col) const;
	virtual SCERROR SetTileByIdx(unsigned idx,int tileno);
	virtual int         GetTileByIdx(unsigned idx) const;
  virtual void        SetXCycled(bool cycled) { xcycled_ = cycled; }
  virtual void        SetYCycled(bool cycled) { ycycled_ = cycled; }
  virtual bool        IsXCycled() const { return xcycled_; }
  virtual bool        IsYCycled() const { return ycycled_; }
  virtual void        Purge();
  TileMap_Impl() {}
  virtual ~TileMap_Impl();
  void Init(TileMapFactory* fac,unsigned rows,unsigned cols,unsigned tilsize,long layer);
  virtual pwide_t GetName() const { return CollectionT<TileMap_Impl>::KeyOf(this); }
private:
  struct WrapTile: public SpriteSubImpl, public ComplexSprite, private refbase {
    Sprite* spr_;
    TileMap_Impl* impl_;
    float width_,height_;
    long row_,col_;
    long row_first_,row_last_;
    long col_first_,col_last_;
    float xfix_,yfix_;
    void Clear() { }
    bool NextTile(long* col,long* row,long* frno);
    bool ComplexSpriteReset(bool blendstate);
    bool ComplexSpriteNext(ComplexSprite::Info* info, bool blendstate);
    bool HasBlendState(bool blendstate);
    virtual void Release_() const { refbase::Release_(); }
    virtual void Grab_() const { refbase::Grab_(); }
    virtual void SetSpritePtr(Sprite* spr) { 
      spr_ = spr;
      if ( spr_ ) {
        width_ = spr->GetWidth();
        height_ = spr->GetHeight();
      }else{
        height_ = width_ = 0;
      }
    }
    virtual void* Adaptate_(const void* id) {
      if ( id == (const void*)ComplexSprite::TypeID__ ) return (ComplexSprite*)this;
      return IRefObject::Adaptate_(id);
    }
    WrapTile(TileMap_Impl* impl) : spr_(0),impl_(impl) {}
    ~WrapTile() { REQUIRE ( !spr_ ); }
  };
  friend struct WrapTile;
  struct TILE_DESC { string klass; long frno; WrapTile* wrap; };
  typedef BufferT<Sprite*>       SPRIVEC;
  typedef map<long,TILE_DESC>   TILESET;
  typedef map<string_key,auto_ref<WrapTile> > KLASSMAP;
  TileMapFactory*   fac_;     // фабрика карт
  unsigned          rows_;    // количество тайлов по вертикали
  unsigned          cols_;    // количество тайлов по горизонтали
  unsigned          x_tilsize_; // базовый размер тайла
  unsigned          y_tilsize_; // базовый размер тайла
  long              layer_;   // слой на котором находится карта
  long              sublayer_; 
  long              x_;       // позиция левого верхнего угла карты в пиксилах
  long              y_;
  float             transparency_;  // прозрачность спрайтов
  float             luminance_;     // мощнось свечения спрайтов
  float             scale_x_; // масштаб по горизонтали
  float             scale_y_; // масштаб по вернтикли
  mrgb_t            cfilt_;   // цветовой фильтер
  vector<u16_t>  tilmap_;  // карта тайлов
  SPRIVEC           sprites_; // спрайты видимой части
  TILESET           tilset_;  // набор тайлов
  KLASSMAP          klasses_;
  auto_ref<TileMapSubImpl> subimple_;
  struct {
    bool visible_              : 1; // флаг видимости карты
    bool xcycled_              : 1;
    bool ycycled_              : 1;
    bool has_blending_sprites_ : 1;
  };
};

bool TileMap_Impl::WrapTile::NextTile(long* col,long* row,long* frno)
{
  TileMap_Impl::TILE_DESC* desc;
  do {
    if ( row_ >= row_last_ ) return false;
    if ( col_ >= col_last_ ) { 
      ++row_; col_ = col_first_; 
      if ( row_ >= row_last_ ) return false;
    }
    const long colzz = col_%long(impl_->cols_);
    const long tilecol = colzz >= 0 ? colzz : long(impl_->cols_)+colzz;
    const long rowzz = row_%long(impl_->rows_);
    const long tilerow = rowzz >= 0 ? rowzz : long(impl_->rows_)+rowzz;
    long tileno = impl_->tilmap_[tilerow*impl_->cols_+tilecol];
    desc = &impl_->tilset_[tileno];
    *col = col_;
    *row = row_;
    ++col_;
  }while( desc->wrap != this);
  *frno = desc->frno;
  return true;
}

bool TileMap_Impl::WrapTile::ComplexSpriteReset(bool blendstate) 
{
  if ( blendstate != ( impl_->has_blending_sprites_ || impl_->transparency_ > 0.1 ) ) return false;

  const long scr_x = long(-impl_->x_-SPRITES::GetLayerX(impl_->layer_)-SCREEN::OffsetX());
  const long scr_y = long(-impl_->y_-SPRITES::GetLayerY(impl_->layer_)-SCREEN::OffsetY());
  const long scrwidth  = SCREEN::WndWidth(); //SCREEN::GetWidth();
  const long scrheight = SCREEN::WndHeight(); //SCREEN::GetHeight();
  const long xtilesz = long(impl_->x_tilsize_*impl_->scale_x_);
  const long ytilesz = long(impl_->y_tilsize_*impl_->scale_y_);

  if ( impl_->xcycled_ ) {
    col_first_ = scr_x / xtilesz; if ( scr_x < 0 ) col_first_-=1;
    col_last_  = col_first_+( scrwidth + xtilesz - 1 ) / xtilesz + 1;
  }else{
    if ( scr_x >= long(impl_->cols_ * xtilesz) || scr_x < -scrwidth ) return false;
    if ( scr_x >= 0 ) {
      col_first_ = scr_x / xtilesz;
    }else{
      col_first_ = 0;
    }
    col_last_ = ( scr_x + scrwidth + xtilesz - 1 ) / xtilesz;
    col_last_ = teggo_min<long>(col_last_+1,impl_->cols_);
  }

  if ( impl_->ycycled_ ) {
    row_first_ = scr_y / ytilesz; if ( scr_y < 0 ) row_first_-=1;
    row_last_ = row_first_+( scrheight + ytilesz - 1 ) / ytilesz + 1;
  }else{
    if ( scr_y >= long(impl_->rows_ * ytilesz) || scr_y < -scrheight) return false;
    if ( scr_y >= 0 ) {
      row_first_ = scr_y / ytilesz;
    }else{
      row_first_ = 0;
    }
    row_last_ = ( scr_y + scrheight + ytilesz - 1 ) / ytilesz;
    row_last_ = teggo_min<long>(row_last_+1,impl_->rows_);
  }

  row_ = row_first_;
  col_ = col_first_;
  return row_ < row_last_ && col_ < col_last_;
}

bool TileMap_Impl::WrapTile::ComplexSpriteNext(ComplexSprite::Info* info,bool blendstate)
{
  //if ( blendstate != ( impl_->has_blending_sprites_ || impl_->transparency_ > 0.1 ) ) return false;

  REQUIRE ( info != 0 );
  long row,col,frno;
  if ( NextTile(&col,&row,&frno) ) {
    info->x = float(col)*width_+width_*0.5f;
    info->y = float(row)*height_+height_*0.5f;
    info->fr_no = frno;
    info->flags = 0|ComplexSprite::Info::IS_CENTERED;
    return true;
  }
  return false;
}

bool TileMap_Impl::WrapTile::HasBlendState(bool blendstate)
{
  return blendstate == ( impl_->has_blending_sprites_ || impl_->transparency_ > 0.1 );
}

TileMapFactory::TileMapFactory(SPRITES_Inst* sinst) : eng_(sinst) {}

TileMapFactory::~TileMapFactory()
{
  Purge();
}

void TileMapFactory::Purge()
{
  tilmaps_.Clear();
}

SCERROR TileMapFactory::Enumerate(vector<TileMap*>& ve) {
  vector<TileMap*>().swap(ve);
  ve.resize(tilmaps_.Count(),0);
  CollectionT<TileMap_Impl>::iterator it = tilmaps_.Iterate();
  vector<TileMap*>::iterator ii = ve.begin();
  while ( it.Next() ) *ii++ = &*it;
  return SCE_OK;
}

TileMap* TileMapFactory::NewTileMap(pwide_t name,unsigned rows,unsigned columns,unsigned tileSize,int layer)
{
  static u32_t unique = 0;
  widestring autoname;
  
  pwide_t nameS = name && wcslen(name) ? name : autoname.Expand(128).SetF(L"{tilemap@%08x}",++unique);
  
  if ( tilmaps_.Get(nameS) ) 
    ret0_because(WideFormat(L"sprites: tilemap %s alredy exists",nameS));

  TileMap_Impl* tilmap = &tilmaps_.Rec(nameS);
  tilmap->Init(this,rows,columns,tileSize,layer);
  return tilmap;
}

TileMap* TileMapFactory::FindTileMap(pwide_t name)
{
  TileMap* tm = tilmaps_.Get(name);
  if ( !tm ) 
    ret0_because(WideFormat(L"sprites: tilemap %s is not exists",name));
  return tm;
}

SCERROR TileMapFactory::DisposeTileMap(pwide_t name)
{
  tilmaps_.Erase(name);
  return SCE_OK;
}

template<class V,class F1,class A1,class A2>
void DoIt(V& v,F1 f,A1 a1,A2 a2){
  for ( typename V::iterator i = v.begin(),iE = v.end(); i != iE; ++i )
    ((*i)->*f)(a1,a2);}
template<class V,class F1,class A1>
void DoIt(V& v,F1 f,A1 a1){
  for ( typename V::iterator i = v.begin(),iE = v.end(); i != iE; ++i )
    ((*i)->*f)(a1);}
template<class V,class F1>
void DoIt(V& v,F1 f){
  for ( typename V::iterator i = v.begin(),iE = v.end(); i != iE; ++i )
    ((*i)->*f)();}

SCERROR TileMap_Impl::Show()
{
  if ( !visible_ ) { visible_ = true; DoIt(sprites_,&Sprite::Show); }
  return SCE_OK;
}

SCERROR TileMap_Impl::Hide()
{
  if ( visible_ ) { DoIt(sprites_,&Sprite::Hide); visible_ = false; }
  return SCE_OK;
}

void TileMap_Impl::Move(float xf,float yf)
{
  long x = long(xf);
  long y = long(yf);
  if ( x != x_ || y != y_ ) { 
    x_ = x; y_ = y; 
    DoIt(sprites_,&Sprite::Move,x,y); 
  }
}

void TileMap_Impl::Scale(float percent)
{
  scale_y_ = scale_x_ = percent/100;
  DoIt(sprites_,&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
}

void TileMap_Impl::ScaleX(float percent)
{
  scale_x_ = percent/100;
  DoIt(sprites_,&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
}

void TileMap_Impl::ScaleY(float percent)
{
  scale_y_ = percent/100; 
  DoIt(sprites_,&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
}

void TileMap_Impl::SetTileSize(unsigned ts)
{
  x_tilsize_ = ts;
  y_tilsize_ = ts;
  DoIt(sprites_,&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
}

void TileMap_Impl::SetTileSizeX(unsigned ts)
{
  x_tilsize_ = ts;
  DoIt(sprites_,&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
}

void TileMap_Impl::SetTileSizeY(unsigned ts)
{
  y_tilsize_ = ts;
  DoIt(sprites_,&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
}

float TileMap_Impl::GetX() const { return x_; }
float TileMap_Impl::GetY() const { return y_; }
int TileMap_Impl::GetRows() const { return rows_; }
int TileMap_Impl::GetColumns() const { return cols_; }
unsigned TileMap_Impl::GetTileSize() const { return x_tilsize_; }
unsigned TileMap_Impl::GetTileSizeX() const { return x_tilsize_; }
unsigned TileMap_Impl::GetTileSizeY() const { return y_tilsize_; }
bool TileMap_Impl::IsVisible() const { return visible_;}
//pwide_t TileMap_Impl::GetName() const { return name_.c_str(); }
long TileMap_Impl::GetLayer()  const { return layer_; }
long TileMap_Impl::GetSubLayer() const { return sublayer_; }
float TileMap_Impl::GetTransparency() const { return transparency_; }
float TileMap_Impl::GetLuminance() const { return luminance_; }
float TileMap_Impl::GetScaleX() const { return scale_x_*100; }
float TileMap_Impl::GetScaleY() const { return scale_y_*100; }
TileMapSubImpl* TileMap_Impl::GetSubImpl() const { return subimple_; }
mrgb_t TileMap_Impl::GetRGB() const { return cfilt_; }

void  TileMap_Impl::SetLayer(long val) 
{
  layer_ = val;
  DoIt(sprites_,&Sprite::SetLayer,val);
}

void  TileMap_Impl::SetSubLayer(long val)
{
  sublayer_ = val;
  DoIt(sprites_,&Sprite::SetSubLayer,val);
}

void TileMap_Impl::SetTransparency(float percent) 
{
  transparency_ = percent;
  DoIt(sprites_,&Sprite::SetTransparency,percent);
}

void TileMap_Impl::SetLuminance(float percent)
{
  luminance_ = percent;
  DoIt(sprites_,&Sprite::SetLuminance,percent);
}

void TileMap_Impl::Dispose()
{
  fac_->DisposeTileMap(GetName());
}

void TileMap_Impl::SetSubImpl(TileMapSubImpl* sbi)
{
  auto_ref<TileMapSubImpl> tmp = subimple_;
  if ( !!subimple_ ) subimple_->SetTileMapPtr(0);
  subimple_ = AutoRef(Grab(sbi));
  if ( !!subimple_ ) subimple_->SetTileMapPtr(this);
}

void TileMap_Impl::SetRGB(mrgb_t cfilt)
{
  if ( cfilt_.C() != cfilt_.C() ) {
    cfilt_ = cfilt;
    DoIt(sprites_,&Sprite::SetRGB,cfilt);
  }
}

SCERROR TileMap_Impl::UseTilesFrom(pwide_t klass,int frno,int count,long* no)
{
  SPRITE_Info info;
  fail_if_fail ( fac_->eng_->GetKlassInfo(klass,info) );
  KLASSMAP::iterator ki = klasses_.find(WideToString(klass).c_str());
  WrapTile* wrap = 0;
  int tileno = tilset_.size();
  if ( ki == klasses_.end() ) {
    KLASSMAP::value_type v(WideToString(klass),AutoRef(new WrapTile(this)));
    Sprite* spr = fac_->eng_->NewSprite(0,klass,layer_);
    fail_if ( !spr );
    has_blending_sprites_ = has_blending_sprites_ || ( spr->GetImageType() != SPRITE_SOLID );
    if (sublayer_) spr->SetSubLayer(sublayer_);
    spr->Rescale(x_tilsize_*scale_x_,y_tilsize_*scale_y_);
    spr->Move(x_,y_);
    if ( visible_ ) spr->Show();
    spr->SetSubImpl(v.second);
    klasses_.insert(v);
    sprites_.push_back(spr);
    wrap = v.second;
  }else{
    wrap = (*ki).second;
  }
  if ( no ) *no = 0;
  if ( frno < 0 || frno >= signed(info.spr_no) ) fail_because("invalid start frame no");
  if ( count <0 || frno+count > signed(info.spr_no) ) fail_because("invalid count of tiles");
  if ( count == 0 ) count = info.spr_no - frno;
  TILESET::value_type tilset_value;
  tilset_value.second.klass = WideToString(klass);
  tilset_value.second.wrap = wrap;
  for ( int i = 0; i < count; ++i ) {
    tilset_value.second.frno = frno+i;
    const_cast<long&>(tilset_value.first) = tileno+i;
    XMessage((":TileMap_Impl::UseTilesFrom: %d -> %s:%d",
      tilset_value.first,
      tilset_value.second.klass.c_str(),
      tilset_value.second.frno));
    tilset_.insert(tilset_value);
  }
  if ( no ) *no = count;
  return SCE_OK;
}

void TileMap_Impl::Clear()
{
  TILESET::iterator i = tilset_.begin();
  TILESET::iterator iE = tilset_.end();
  for (; i != iE; ++i ) (*i).second.wrap->Clear();
  fill( tilmap_.begin(),tilmap_.end(), USHRT_MAX );
}

SCERROR TileMap_Impl::SetTile(unsigned row,unsigned col,int tileno)
{
  if ( row >= rows_ || col >= cols_ )
    fail_because(Format(":TileMap_Impl::SetTile: invalid row/col position %d/%d",row,col));
  return TileMap_Impl::SetTileByIdx(row*cols_+col,tileno);
}

SCERROR TileMap_Impl::SetTileByIdx(unsigned idx,int tileno)
{
  if ( idx >= rows_*cols_ )
    fail_because(Format(":TileMap_Impl::SetTileByIdx: invalid index %d",idx));
  if ( tileno >= 0 && tileno < USHRT_MAX ) {
    TILESET::iterator i = tilset_.find(tileno);
    if ( i == tilset_.end() ) {
      tilmap_[idx] = USHRT_MAX;
      fail_because(Format(":TileMap_Impl::SetTile: has no tile %d",tileno));
    }
    else {
      //XMessage((":TileMap_Impl::SetTile: %d,%d -> %d",row,col,tileno));
      tilmap_[idx] = tileno;
      //  (*i).second.wrap->SetTile(row,col,(*i).second.frno);
    }
  }
  else {
    tileno = tilmap_[idx];
    tilmap_[idx] = USHRT_MAX;
    //TILESET::iterator i = tilset_.find(tileno);
    //if ( i != tilset_.end() ) (*i).second.wrap->ClearTile(row,col);
  }
  return SCE_OK;
}

int TileMap_Impl::GetTile(unsigned row,unsigned col) const
{
  if ( row >= rows_ || col >= cols_ )
    ret0_because(Format("sprites: invalid tilemap row/col position %d/%d",row,col));
  u16_t tileno = tilmap_[row*cols_+col];
  if ( tileno == USHRT_MAX ) return -1; else return tileno;
}

int TileMap_Impl::GetTileByIdx(unsigned idx) const
{
  if ( idx >= rows_*cols_ )
    fail_because(Format("sprites: invalid tilemap index %d",idx));
  u16_t tileno = tilmap_[idx];
  if ( tileno == USHRT_MAX ) return -1; else return tileno;
}

void TileMap_Impl::Init(TileMapFactory* fac,unsigned rows,unsigned cols,unsigned tilsize,long layer)
{
  fac_ = fac;
  rows_ = rows;
  cols_ = cols;
  layer_ = layer;
  x_tilsize_ = tilsize;
  y_tilsize_ = tilsize;
  sublayer_ = 0;
  x_ = y_ = 0;
  transparency_ = 0;
  luminance_ = 100;
  cfilt_ = mRGB_t(255,255,255);
  scale_x_ = scale_y_ = 1;
  visible_ = false;
  xcycled_ = ycycled_ = false;
  has_blending_sprites_ = false;
  if ( cols_*rows_ > 1024*1024 ) {
    cols_ = 0; rows_ = 0;
    LogF("sprites: tilemap row(%d)*col(%d) limit excided",cols_,rows_);
  }
  tilmap_.resize(cols_*rows_,USHRT_MAX);
  Show();
}

TileMap_Impl::~TileMap_Impl()
{
  Purge();
  SetSubImpl(0);
}

void TileMap_Impl::Purge()
{
  Clear();
  DoIt(sprites_,&Sprite::Dispose);
  tilset_.clear();
  klasses_.clear();
  sprites_.clear();
  has_blending_sprites_ = false;
}

NAMESPACE_MSKIN_END
