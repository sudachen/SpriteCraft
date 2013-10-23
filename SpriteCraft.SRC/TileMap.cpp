
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

#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "TileMap.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$TileMap")
#endif

static pwide_t NOT_EXISTS_INFO = L"TileMap is not exists";

static HRESULT GenErrX()
  {
    return GenSCWideError(NOT_EXISTS_INFO);
  }

STDMETHODIMP TileMapHelper::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(ITileMap),riid)?S_OK:S_FALSE;
  }

/*

  Реализация интерфейса ComplexSprite для тайлов

*/

bool TileMapHelper_ComplexTile::NextTile(long* col,long* row,long* frno)
  {
    TileMapHelper::TileDesc* desc = 0;
    do
      {
        if ( row_ >= row_last_ )
          return false;
        if ( col_ >= col_last_ )
          {
            ++row_; col_ = col_first_;
            if ( row_ >= row_last_ )
              return false;
          }
        const long colzz = col_%long(impl_->cols_);
        const long tilecol = colzz >= 0 ? colzz : long(impl_->cols_)+colzz;
        const long rowzz = row_%long(impl_->rows_);
        const long tilerow = rowzz >= 0 ? rowzz : long(impl_->rows_)+rowzz;
        u32_t tileno = u32_t(impl_->map_[tilerow*impl_->cols_+tilecol]);
        desc = &impl_->tsetmapper_[tileno];
        *col = col_;
        *row = row_;
        ++col_;
      }
    while( desc->holder != this);
    *frno = desc->frno;
    return true;
  }

bool TileMapHelper_ComplexTile::ComplexSpriteReset(bool blendstate)
  {
    if ( blendstate != ( impl_->has_blending_sprites_ || impl_->transparency_ > 0.1 ) )
      return false;

    const long scr_x = long(-impl_->x_-SPRITES::GetLayerX(impl_->layer_)-SCREEN::OffsetX());
    const long scr_y = long(-impl_->y_-SPRITES::GetLayerY(impl_->layer_)-SCREEN::OffsetY());
    const long scrwidth = SCREEN::WndWidth();
    const long scrheight  = SCREEN::WndHeight();
    const long xtilesz = long(impl_->x_tilsize_*impl_->scale_x_);
    const long ytilesz = long(impl_->y_tilsize_*impl_->scale_y_);

    if ( impl_->xcycled_ )
      {
        col_first_ = scr_x / xtilesz;
        if ( scr_x < 0 )
          col_first_-=1;
        col_last_  = col_first_+( scrwidth + xtilesz - 1 ) / xtilesz + 1;
      }
    else
    {
      if ( scr_x >= long(impl_->cols_ * xtilesz) || scr_x < -scrwidth )
        return false;
      if ( scr_x >= 0 )
        {
          col_first_ = scr_x / xtilesz;
        }
      else
        {
          col_first_ = 0;
        }
      col_last_ = ( scr_x + scrwidth + xtilesz - 1 ) / xtilesz;
      col_last_ = teggo_min<long>(col_last_+1,impl_->cols_);
    }

    if ( impl_->ycycled_ )
      {
        row_first_ = scr_y / ytilesz;
        if ( scr_y < 0 )
          row_first_-=1;
        row_last_ = row_first_+( scrheight + ytilesz - 1 ) / ytilesz + 1;
      }
    else
      {
        if ( scr_y >= long(impl_->rows_ * ytilesz) || scr_y < -scrheight)
          return false;
        if ( scr_y >= 0 )
          {
            row_first_ = scr_y / ytilesz;
          }
        else
          {
            row_first_ = 0;
          }
        row_last_ = ( scr_y + scrheight + ytilesz - 1 ) / ytilesz;
        row_last_ = teggo_min<long>(row_last_+1,impl_->rows_);
      }

    row_ = row_first_;
    col_ = col_first_;
    return row_ < row_last_ && col_ < col_last_;
  }

bool TileMapHelper_ComplexTile::ComplexSpriteNext(ComplexSprite::Info* info,bool blendstate)
  {
    REQUIRE ( info != 0 );
    long row,col,frno;
    if ( NextTile(&col,&row,&frno) )
      {
        info->x = float(col)*width_+width_*0.5f;
        info->y = float(row)*height_+height_*0.5f;
        info->fr_no = frno;
        info->flags = 0|ComplexSprite::Info::IS_CENTERED;
        return true;
      }
    return false;
  }

bool TileMapHelper_ComplexTile::HasBlendState(bool blendstate)
  {
    return blendstate == ( impl_->has_blending_sprites_ || impl_->transparency_ > 0.1 );
  }

void TileMapHelper_ComplexTile::Release_() const
  {
    refbase::Release_();
  }

void TileMapHelper_ComplexTile::Grab_() const
  {
    refbase::Grab_();
  }

void TileMapHelper_ComplexTile::Clear()
  {
  }

void TileMapHelper_ComplexTile::SetSpritePtr(Sprite* spr)
  {
    spr_ = spr;
    if ( spr_ )
      {
        width_ = spr->GetWidth();
        height_ = spr->GetHeight();
      }
    else
      {
        height_ = width_ = 0;
      }
  }

void* TileMapHelper_ComplexTile::Adaptate_(const void* id)
  {
    if ( id == (const void*)ComplexSprite::TypeID__ )
      return (ComplexSprite*)this;
    if ( id == (const void*)SpriteSubImpl::TypeID__ )
      return (SpriteSubImpl*)this;
    return IRefObject::Adaptate_(id);
  }

TileMapHelper_ComplexTile::TileMapHelper_ComplexTile(TileMapHelper* impl)
  : spr_(0),impl_(impl)
  {
  }

TileMapHelper_ComplexTile::~TileMapHelper_ComplexTile()
  {
    REQUIRE ( !spr_ );
  }

/*

  Реализация менеджера тайлмапов

*/

TileMapManager* TileMapManager::manager_ = 0;

TileMapManager::TileMapManager()
  {
    REQUIRE ( !manager_ );
    manager_ = this;
  }

TileMapManager::~TileMapManager()
  {
    REQUIRE ( manager_ == this || !manager_ );
    manager_ = 0;
  }

HRESULT TileMapManager::DoMake(pwide_t name,unsigned rows,unsigned columns,unsigned tileSize,int layer,rcc_ptr<TileMapHelper>* pVal)
  {
    auto_ref<TileMapManager> manager;
    widestring nameS;
    static long unique_id = 0;

    long namelen = name?wcslen(name):0;

    if ( namelen && Find(name) )
      return GenSCWideError(_S*L"tilemap %? already exists" %name);

    if ( !manager_ )
      manager.reassign__(new TileMapManager());
    else
      manager.reassign__(Grab(manager_));

    if ( namelen )
      nameS = name;
    else
      nameS.Set(_S*L"{tilemap@%08x}" %++unique_id);

    rcc_ptr<TileMapHelper> helper ( new XComObject__<TileMapHelper>() );
    helper->Init_(manager,nameS.Str(),namelen==0,rows,columns,tileSize,layer);
    manager->tmaps_.Put(nameS.Str(),Refe(helper));
    *pVal = helper;
    return S_OK;
  }

TileMapHelper* TileMapManager::Find(pwide_t name)
  {
    if ( !manager_ )
      return 0;
    TileMapHelper** phelper = manager_->tmaps_.Get(name);
    return phelper?*phelper:0;
  }

void TileMapManager::Purge()
  {
    if ( manager_ )
      {
        auto_ref<TileMapManager> guard(Grab(manager_));
        CollectionIteratorT<TileMapHelper*> i = manager_->tmaps_.Iterate();
        bool cont = !!i.Next();
        while ( cont )
          {
            TileMapHelper* ii = *i;
            cont = !!i.Next();
            ii->Dispose_();
          }
      }
  }

void TileMapManager::Forget( pwide_t name )
  {
    TileMapHelper** h = tmaps_.Get(name);
    if ( h )
      {
        (*h)->Release();
        tmaps_.Erase(h);
      }
  }

HRESULT TileMapManager::Enumerate(ArrayT< rcc_ptr<TileMapHelper> >& e)
  {
    e.Resize(0);
    if ( manager_ )
      {
        CollectionIteratorT<TileMapHelper*> i = manager_->tmaps_.Iterate();
        e.Reserve(manager_->tmaps_.Count());
        while(i.Next())
          e.Push(rcc_ptr<TileMapHelper>(Refe(*i)));
      }
    return S_OK;
  }

bool TileMapManager::NameIsLess::operator()(const TileMapHelper* a,const TileMapHelper* b) const
  {
    return wcscmp(a->name_.Str(),b->name_.Str()) < 0;
  }

void TileMapManager::LogTileMaps()
  {
    if ( manager_ )
      {
        CollectionIteratorT<TileMapHelper*> i = manager_->tmaps_.Iterate();
        BufferT<TileMapHelper*> e;
        e.Reserve(manager_->tmaps_.Count());
        while(i.Next())
          e.Push(*i);
        Qsort(e.Begin(),e.End(),NameIsLess());
        if ( e.Empty() )
          Lout << "engine: has no tilemaps";
        else
          for ( TileMapHelper **i = e.Begin(), **iE = e.End(); i != iE; ++i )
            {
              TileMapHelper* tm = *i;
              bool xCycled = tm->xcycled_;
              bool yCycled = tm->ycycled_;
              Lout << _S*"engine: [%?] '%?' %dx%d %? at %d,%d  tile %dx%d"
                %(tm->visible_?"+":"-")
                %tm->name_
                %tm->cols_ %tm->rows_
                %((xCycled&&yCycled)?"(xyCycled)":xCycled?"(xCycled)":yCycled?"(yCycled)":"")
                %long(tm->x_) %long(tm->y_)
                %tm->x_tilsize_ %tm->y_tilsize_;
            }
      }
  }

/*

  Реализация тайлмапа

*/

TileMapHelper::TileMapHelper()
  {
    tsetcount_ = 0;
    memset(tsetmapper_,0,sizeof(tsetmapper_));
    noname_ = true;
  }

TileMapHelper::~TileMapHelper()
  {
    Dispose_();
  }

void TileMapHelper::Init_(const auto_ref<TileMapManager>& manager,pwide_t name,bool noname,unsigned rows,unsigned columns,unsigned tileSize,int layer)
  {
    REQUIRE ( ~manager != 0 );
    REQUIRE ( name != 0 );
    noname_ = noname;
    manager_ = manager;
    name_ = name;
    rows_ = rows;
    cols_ = columns;
    layer_ = layer;
    x_tilsize_ = tileSize;
    y_tilsize_ = tileSize;
    sublayer_ = 0;
    x_ = y_ = 0;
    transparency_ = 0;
    luminance_ = 100;
    cfilt_ = mRGB_t(255,255,255);
    scale_x_ = scale_y_ = 1;
    visible_ = false;
    xcycled_ = ycycled_ = false;
    has_blending_sprites_ = false;
    map_.Resize(cols_*rows_,HEREIS_NO_TILE);
    Show_();
  }

void TileMapHelper::Show_()
  {
    if (!visible_)
      {
        visible_ = true;
        DoIt_(&Sprite::Show);
      }
  }

void TileMapHelper::Hide_()
  {
    if (visible_)
    {
      visible_ = false;
      DoIt_(&Sprite::Hide);
    }
  }

void TileMapHelper::Purge_()
  {
    teggo::fill(map_.Begin(),map_.End(),HEREIS_NO_TILE);
    DoIt_(&Sprite::Dispose);
    tiles_.Clear();
    tsetcount_ = 0;
    memset(tsetmapper_,0,sizeof(tsetmapper_));
  }

void TileMapHelper::SetSubImpl_(IUnknown*)
  {
  }

void TileMapHelper::Dispose_()
  {
    if ( ~manager_ )
      {
        auto_ref<TileMapManager> manager(manager_.forget__());
        Hide_();
        Purge_();
        SetSubImpl_(0);
        manager->Forget(name_.Str());
      }
  }

mrgb_t TileMapHelper::IColorHelper_GetColor() const
  {
    return cfilt_;
  }

void   TileMapHelper::IColorHelper_SetColor(mrgb_t c)
  {
    cfilt_ = c; DoIt_(&Sprite::SetRGB,c);
  }

void TileMapHelper::Grab_() const
  {
    ((ITileMap*)this)->AddRef();
  }

void TileMapHelper::Release_() const
  {
    ((ITileMap*)this)->Release();
  }

STDMETHODIMP TileMapHelper::Dispose()
  {
    _SYNCHRONIZE_;
    Dispose_();
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_name(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SysAllocString(name_.Str());
    return S_OK;
  }

STDMETHODIMP TileMapHelper::GetNameAsPWIDE(wchar_t** pVal)
  {
    _SYNCHRONIZE_;
    *pVal = (wchar_t*)name_.Str();
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_layer(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = layer_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_layer(long val)
  {
    _SYNCHRONIZE_;
    layer_ = val;
    DoIt_(&Sprite::SetLayer,val);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_luminance(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = luminance_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_luminance(float newVal)
  {
    _SYNCHRONIZE_;
    luminance_ = newVal;
    DoIt_(&Sprite::SetLuminance,newVal);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_transparency(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = transparency_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_transparency(float newVal)
  {
    _SYNCHRONIZE_;
    transparency_ = newVal;
    DoIt_(&Sprite::SetTransparency,newVal);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_cfilt(IRGBFilter **pVal)
  {
    _SYNCHRONIZE_;
    *pVal = new ColorObject(Refe((IColorHelper*)this));
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_x(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = float(x_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_x(float newVal)
  {
    _SYNCHRONIZE_;
    x_ = long(newVal);
    DoIt_(&Sprite::MoveX,x_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_y(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = float(y_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_y(float newVal)
  {
    _SYNCHRONIZE_;
    y_ = long(newVal);
    DoIt_(&Sprite::MoveY,y_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_visible(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = visible_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_visible(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( newVal != VARIANT_FALSE )
      Show_();
    else
      Hide_();
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_tileSize(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = x_tilsize_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_tileSize(long newVal)
  {
    _SYNCHRONIZE_;
    x_tilsize_ = newVal;
    y_tilsize_ = newVal;
    DoIt_(&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_rows(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = rows_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_cols(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = cols_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_xyScale(float newVal)
  {
    _SYNCHRONIZE_;
    scale_y_ = scale_x_ = newVal*0.01f;
    DoIt_(&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_xScale(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = scale_x_ * 100.f;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_xScale(float newVal)
  {
    _SYNCHRONIZE_;
    scale_x_ = newVal*0.01f;
    DoIt_(&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_yScale(float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = scale_y_ * 100.f;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_yScale(float newVal)
  {
    _SYNCHRONIZE_;
    scale_y_ = newVal*0.01f;
    DoIt_(&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
    return S_OK;
  }

HRESULT TileMapHelper::UseTilesFrom_(pwide_t klass,long frno,long count,long* no)
  {
    ResetErrorLog();

    if ( !SPRITES::HasKlass(klass) && SCE_OK != SPRITES::NewKlass(klass) )
      return GenSCWideError(
        _S*L"could not use tiles from '%?', reason %?"
          %klass %GetErrorLog() );

    ComplexTilePtr* ctp = tiles_.Get(klass);
    if ( !ctp )
      {
        Sprite* s= SPRITES::NewSprite(0,klass,layer_);
        if ( !s )
          return GenSCWideError(
            _S*L"could not use tiles from '%?', reason %?"
              %klass %GetErrorLog() );
        s->SetSubLayer(sublayer_);
        s->Rescale(x_tilsize_*scale_x_,y_tilsize_*scale_y_);
        s->Move(float(x_),float(y_));
        if ( visible_ ) s->Show();
        ComplexTilePtr complex_tile (new ComplexTile(this));
        s->SetSubImpl((SpriteSubImpl*)+complex_tile);
        ctp = &tiles_.Put(klass,complex_tile);
        unsigned img_type = s->GetImageType();
        has_blending_sprites_ |= ( img_type == SPRITE_BLEND || img_type == SPRITE_ADDITIVE );
      }

    if ( !count ) count = (*ctp)->spr_->GetFrCount();
    if ( tsetcount_ + count > 255 ) count = 255-tsetcount_;

    for ( int i = 0; i < count; ++i )
      {
        tsetmapper_[tsetcount_].frno = frno + i;
        tsetmapper_[tsetcount_].holder = ~*ctp;
        ++tsetcount_;
      }

    *no = count;

    return S_OK;
  }

STDMETHODIMP TileMapHelper::AddTilesFrom(BSTR klass, long frno, long count, long* no)
  {
    _SYNCHRONIZE_;
    if ( !SPRITES::HasKlass(klass) )
      {
        ResetErrorLog();
        if ( SCE_OK != SPRITES::NewKlass(klass) )
          return GenSCWideError(0,_S*L":UseTiesFrom '%?':" %klass);
      }
    if ( !manager_ )
      return GenErrX();
    return UseTilesFrom_(klass,frno,count,no);
  }

STDMETHODIMP TileMapHelper::Clear()
  {
    _SYNCHRONIZE_;
    teggo::fill(map_.Begin(),map_.End(),HEREIS_NO_TILE);
    return S_OK;
  }

HRESULT TileMapHelper::SetTile_(long idx,long tilno)
  {
    map_[idx] = tilno < 0 ? byte_t(HEREIS_NO_TILE) : byte_t(tilno);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::SetTile(long col, long row, long tileno)
  {
    _SYNCHRONIZE_;
    if ( !manager_ )
      return GenErrX();
    if ( col < 0 || row < 0 || unsigned(row) >= rows_ || unsigned(col) >= cols_ )
      return GenSCWideError(_S*L"invalid tilemap row/col position %d/%d" %row %col);
    if ( tileno > tsetcount_ )
      return GenSCWideError(_S*L"invalid tileno %d" %tileno);
    return SetTile_(row*cols_+col,tileno);
  }

STDMETHODIMP TileMapHelper::GetTile(long col, long row, long *tileno)
  {
    _SYNCHRONIZE_;
    if ( !manager_ ) return GenErrX();
    if ( col < 0 || row < 0 || unsigned(row) >= rows_ || unsigned(col) >= cols_ )
      return GenSCWideError(_S*L"invalid tilemap row/col position %d/%d" %row %col);
    byte_t no = map_[row*cols_+col];
    *tileno = (no==HEREIS_NO_TILE)?-1:u32_t(no);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::SetTileByIdx(long idx, long tileno)
  {
    _SYNCHRONIZE_;
    if ( !manager_ )
      return GenErrX();
    if ( idx < 0 || unsigned(idx) >= rows_*cols_ )
      return GenSCWideError(_S*L"invalid tilemap index %d" %idx);
    if ( tileno > tsetcount_ )
      return GenSCWideError(_S*L"invalid tileno %d" %tileno);
    return SetTile_(idx,tileno);
  }

STDMETHODIMP TileMapHelper::GetTileByIdx(long idx, long *tileno)
  {
    _SYNCHRONIZE_;
    if ( !manager_ )
      return GenErrX();
    if ( idx < 0 || unsigned(idx) >= rows_*cols_ )
      return GenSCWideError(_S*L"invalid tilemap index %d" %idx);
    byte_t no = map_[idx];
    *tileno = (no==HEREIS_NO_TILE)?-1:u32_t(no);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_xCycled(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = xcycled_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_xCycled(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    xcycled_ = newVal!=VARIANT_FALSE;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_yCycled(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = ycycled_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_yCycled(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    ycycled_ = newVal!=VARIANT_FALSE;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::Purge()
  {
    _SYNCHRONIZE_;
    if ( !manager_ ) return GenErrX();
    Purge_();
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_tileWidth(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = x_tilsize_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_tileWidth(long newVal)
  {
    _SYNCHRONIZE_;
    x_tilsize_ = newVal;
    DoIt_(&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_tileHeight(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = y_tilsize_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_tileHeight(long newVal)
  {
    _SYNCHRONIZE_;
    y_tilsize_ = newVal;
    DoIt_(&Sprite::Rescale,x_tilsize_*scale_x_,y_tilsize_*scale_y_);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_alive(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = !manager_?VARIANT_FALSE:VARIANT_TRUE;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_sublayer(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = sublayer_;
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_sublayer(long val)
  {
    _SYNCHRONIZE_;
    sublayer_ = val;
    DoIt_(&Sprite::SetSubLayer,val);
    return S_OK;
  }

STDMETHODIMP TileMapHelper::get_style(enum tagStyle *pVal)
  {
    _SYNCHRONIZE_;
    if ( !manager_ )
      return GenErrX();
    return S_OK;
  }

STDMETHODIMP TileMapHelper::put_style(enum tagStyle newVal)
  {
    _SYNCHRONIZE_;
    if ( !manager_ )
      return GenErrX();
    return S_OK;
  }
