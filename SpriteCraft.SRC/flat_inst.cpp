
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
#include "flat_e3.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$flat$inst")
#endif

pchar_t const SPRITES_TextureProxy::TypeID__ = "##TypeID##mskin::SPRITES_TextureProxy";

/**
  Уничтожает все классы и спрайты
*/
SCERROR Impl_3DSPRITES::Purge()
  {
    order_.clear();

    // убиваем спрайты
    sprites_.Clear();
    // убиваем классы
    klasses_.Clear();

    for (int l=0,lE=layerObjs_.Count();l<lE;++l)
      delete layerObjs_[l];

    BufferT<Sprite3D_Layer*>().Swap(layerObjs_);
    need_restore_ = true;

    return SCE_OK;
  }

/**
  Восстанавливает(загружает) текстуры всех классов
*/
SCERROR Impl_3DSPRITES::Restore()
  {
    Lout << "[SPRITES] restore sprites ....";
    u32_t millis = GetMillis();
    need_refresh_ = true;
    order_.Clear();

    CollectionIteratorT<KlassOfSprite3D> it = klasses_.Iterate();

    while ( it.Next() )
      {
        if ( it->info_.dummy_ )
          continue;
        if ( !it->is_unloaded_ )
          {
            u32_t millis = GetMillis();
            fail_if_err ( RestoreKlass_(*it) );
            Lout << _S*"[SPRITES] klass '%?' restored in %d millis"
              %it->GetName() %(GetMillis()-millis);
          }
      }

    Lout << _S*"[SPRITES] was restored in %d millis" %(GetMillis()-millis);
    return SCE_OK;
  }

SCERROR Impl_3DSPRITES::UpdateKlass(pwide_t name)
  {
    if ( KlassOfSprite3D *klass = klasses_.Get(name) )
      klass->need_update_ = 1;
    return SCE_OK;
  }

SCERROR Impl_3DSPRITES::UnloadAll()
  {
    Lout << "[SPRITES] restore sprites ....";
    u32_t millis = GetMillis();
    need_refresh_ = true;
    order_.clear();

    CollectionIteratorT<KlassOfSprite3D> it = klasses_.Iterate();

    while ( it.Next() )
      {
        if ( it->info_.dummy_ ) continue;
        UnloadKlass(it->GetName(),false);
      }

    Lout << _S*"[SPRITES] was restored in %d millis" %(GetMillis()-millis);
    Xdbg( Ldbg << _S*"[SPRITES] restore sprites textures in %lu millis" %(GetMillis()-millis));
    return SCE_OK;
  }

struct KlassLessDepth
  {
    bool operator () (const KlassOfSprite3D* a,const KlassOfSprite3D* b)
      {
        if ( b->sprites_.empty() ) return false;
        if ( a->sprites_.empty() ) return true;
        Sprite3D* al;
        Sprite3D* bl;
        al = a->info_.type == SPRITE_BLEND ? a->sprites_.back() : a->sprites_.front();
        bl = b->info_.type == SPRITE_BLEND ? b->sprites_.back() : b->sprites_.front();
        if (  al->layered_.layer_ == bl->layered_.layer_ )
          return al->layered_.layer_cheat_ < bl->layered_.layer_cheat_;
        else return al->layered_.layer_ < bl->layered_.layer_;
      }
  };

/**
  Создает новый класс спрайтов на основе информации о классе
*/
SCERROR Impl_3DSPRITES::NewKlass(pwide_t name,const SPRITE_Info& info,KlassOfSprite3D **klass_p)
  {
    // убедимся что такого класса еще нет
    if ( klasses_.Get(name) )
      fail_because( _S*"sprite klass '%?' already exists" %name );

    // создадим класс
    KlassOfSprite3D& klass = klasses_.Rec(name);

    klass.info_ = info;
    klass.mgr_  = this;
    klass.is_unloaded_ = true;
    klass.use_shared_texture_ = false;

    if ( !klass.info_.dummy_ ) LayoutKlass_(klass);
    else if ( klass.info_.renderable_ )
      {
        klass.unisprite_ = false;
        klass.textures_.resize(1);
        SPRITE_TexInfo& si = klass.textures_[0];
        SPRITE_Rect sr;
        sr.rect = rect_t(0,0,1,1);
        sr.x = 0;
        sr.y = 0;
        si.height_ = 1;
        si.width_  = 1;
        si.layout_EX_.push_back(sr);
        SPRITE_fRect sfr;
        sfr.s = 0;
        sfr.t = 0;
        sfr.ws = 0;
        sfr.wt = 0;
        klass.spr_layout_.push_back(sfr);
        klass.info_.spr_no = 1;
      }

    if ( klass.info_.is_font )
      fail_if_fail( RestoreKlass_(klass) );

    if ( klass_p ) *klass_p = &klass;

    return SCE_OK;
  }

SCERROR Impl_3DSPRITES::NewKlass(pwide_t name,const SPRITE_Info& info)
  {
    return NewKlass(name,info,0);
  }

SCERROR Impl_3DSPRITES::NewDynamicKlass(pwide_t klass,KlassProvider *provider)
  {
    KlassOfSprite3D *klass_object = 0;
    SPRITE_Info info;
    provider->FillInfo(&info);
    SCERROR e = NewKlass(klass,info,&klass_object);
    if ( e == SCE_OK )
      klass_object->provider_ = provider;
    return e;
  }

/**
  Выгружает ресурсы класса
*/
SCERROR Impl_3DSPRITES::UnloadKlass(pwide_t name,bool remove_klass)
  {
    // убедимся что такой класс есть
    KlassOfSprite3D* klass = klasses_.Get(name);
    if ( !klass )
      fail_because(_S*"sprite klass '%?' is not exists" %name);

    if ( !klass->info_.is_font )
      {
        // и освободим занимаемые им ресурсы
        klass->is_unloaded_ = true;
        for ( SPRITE_TexInfo *i = klass->textures_.Begin(), *iE= klass->textures_.End(); i != iE; ++i )
          {
            SPRITE_TexInfo& texinfo = *i;
            Release(texinfo.image_);
          }
      }

    if ( remove_klass )
      {
        BufferT<Sprite3D*> foo(klass->sprites_.Count());
        teggo::copy(klass->sprites_.Begin(),klass->sprites_.End(),foo.Begin());
        for ( Sprite3D **i = foo.Begin(); i != foo.End(); ++i )
          DisposeSprite_(*i);
        //LogF("[SPRITES] removing klass '%ls'",name);
        klasses_.Erase(name);
      }

    return SCE_OK;
  }

struct SpriteOrderLess
  {
    bool operator() ( const Sprite3D* a,const Sprite3D* b )
      {
        if ( a->layered_.layer_ < b->layered_.layer_ ) return true;
        if ( a->layered_.layer_cheat_ < b->layered_.layer_cheat_ ) return true;
        if ( a->layered_.layer_ == b->layered_.layer_ ) return a->layered_.unique_no_ < b->layered_.unique_no_;
        return false;
      }
  };

struct SpriteLess
  {
    bool operator() ( const Sprite3D* a,const Sprite3D* b ) const
      {
        return a->layered_.unique_no_ < b->layered_.unique_no_;
      }
  };

/**
  Создает новый спрайт некоторого класса
*/
Sprite* Impl_3DSPRITES::NewSprite(pwide_t name,pwide_t klass_name,int layer)
  {
    static u32_t auto_name_no = 0; // генератор уникального имени
    ret0_if ( !klass_name );

    long namelen = name?wcslen(name):0;
    Sprite3D* sprite = 0;

    // проверим что такого спрайта еще нет
    if ( namelen && sprites_.Get(name))
      ret0_because( _S*"sprite '%?' already exists" %name );

    // найдем класс спрайтов
    KlassOfSprite3D* klass = FindKlass_(klass_name);
    if ( !klass )
      ret0_because( _S*"sprite klass '%?' is not exists" %klass_name );

    // создадим спрайт
    sprite = &sprites_.Rec(
      namelen
      ?name
      :+~(_S*L"{noname@%08x}" %++auto_name_no));

    //managed_heapcheck__();

    sprite->klass_ = klass;
    sprite->layered_.layer_ = layer;
    sprite->layerObj_ = RequestLayer(layer,false);
    sprite->visible_ = false;
    sprite->x_ = sprite->y_ = 0;
    sprite->xhsp_ = (float)klass->info_.hotspot_x;
    sprite->yhsp_ = (float)klass->info_.hotspot_y;
    sprite->hsptag_ = HOTSPOT_CUSTOM;
    klass->sprites_.insert(
      teggo::binary_upper(klass->sprites_.begin(),klass->sprites_.end(),sprite,SpriteLess()),
      sprite);

    order_.clear();

    return sprite;
  }

/**
  Изменяет класс спрайта
*/
SCERROR Impl_3DSPRITES::ChangeSpriteKlassOn(Sprite3D* sprite,pwide_t klass_name)
  {

    KlassOfSprite3D* klass = klasses_.Get(klass_name);

    if ( !klass )
      fail_because( _S*"sprite klass '%?' is not exists" %klass_name );

    KlassOfSprite3D* curr_klass = sprite->klass_;
    if ( curr_klass == klass ) return SCE_OK;
    bool is_visible = sprite->visible_;
    sprite->Hide();
    Sprite3D **i =
      teggo::binary_lower(curr_klass->sprites_.Begin(),curr_klass->sprites_.End(),sprite,SpriteLess());
    REQUIRE(i != curr_klass->sprites_.end() && *i == sprite );
    curr_klass->sprites_.erase(i);
    klass->sprites_.Insert(
      teggo::binary_upper(klass->sprites_.Begin(),klass->sprites_.End(),sprite,SpriteLess()),
      sprite);
    sprite->klass_ = klass;
    sprite->xhsp_ = (float)klass->info_.hotspot_x;
    sprite->yhsp_ = (float)klass->info_.hotspot_y;
    sprite->hsptag_ = HOTSPOT_CUSTOM;
    if ( is_visible ) sprite->Show();
    order_.clear();
    return SCE_OK;
  }

/**
  Уничтожает спрайт по имени
*/
SCERROR Impl_3DSPRITES::DisposeSprite(pwide_t name)
  {
    Sprite3D* sprite = sprites_.Get(name);
    if ( !sprite ) return SCE_OK; // already disposed
    return DisposeSprite_(sprite);
  }

SCERROR Impl_3DSPRITES::DisposeSprite_(Sprite3D* sprite)
  {
    fail_if ( !sprite );
  //  sprites_.erase(sprite->name_);
  //  XLog(_F(L"Dispose sprite %?")%sprite->GetName());
    DisposeChildren_(sprite);
    KlassOfSprite3D* klass = sprite->klass_;
    Sprite3D **i =
      teggo::binary_lower(
        klass->sprites_.begin(),klass->sprites_.end(),
        sprite,SpriteLess());
    STRICT_REQUIRE( i != klass->sprites_.end() && (*i) == sprite );
    klass->sprites_.erase(i);
    sprites_.Erase(sprite);
    return SCE_OK;
  }

SCERROR Impl_3DSPRITES::GetKlassInfo(pwide_t name, SPRITE_Info& info)
  {
    KlassOfSprite3D* k = klasses_.Get(name);
    if ( k ) { info = k->info_; return SCE_OK; }
    else
      fail_because(_S*":Impl_3DSPRITES::GetKlassInfo: klass %? not found" %name );
  }

Sprite* Impl_3DSPRITES::FindSpriteAt(long xX,long yY,long nearlayer,long farlayer)
  {
    Sprite3D_Layered spr;
    spr.layer_       = nearlayer;
    spr.layer_cheat_ = LONG_MIN;
  //  VISIBILITY_SET::iterator n = layers_.binary_lower(&spr);
    VISIBILITY_SET::iterator n = VISIBILITY_SET_lower_bound(layers_,&spr);
    spr.layer_ = farlayer;
    spr.unique_no_   = ULONG_MAX;
    spr.layer_cheat_ = LONG_MAX;
  //  VISIBILITY_SET::iterator f = layers_.binary_upper(&spr);
    VISIBILITY_SET::iterator f = VISIBILITY_SET_upper_bound(layers_,&spr);
    for ( ; n != f ; ++n )
      if ( (*n)->layered_sprite_->Contains((float)xX,(float)yY) )
        return (*n)->layered_sprite_;
    return 0;
  }

SCERROR Impl_3DSPRITES::EnumerateSprites(ArrayT<Sprite*>& v,long lnear,long lfar)
  {
    v.Clear();
    v.Reserve(sprites_.Count());
    CollectionIteratorT<Sprite3D> i = sprites_.Iterate();
    while ( i.Next() )
      if ( i->layered_.layer_ >= lnear && i->layered_.layer_ <= lfar ) v.Push(&*i);
    return SCE_OK;
  }

/**
  Отображает все видимые спрайты
*/
SCERROR Impl_3DSPRITES::Display()
  {
    return Display2();
  }

void Impl_3DSPRITES::ChangeParentOf(Sprite3D* child,Sprite3D* parent,pwide_t newkey)
  {
    return_if ( !child );
    if ( child->parent_ )
      // удаляем из списка чайлдов
      child->parent_->children_.Erase(child);
    if ( parent )
      parent->children_.Insert(newkey,child);
    child->parent_ = parent;
  }

void Impl_3DSPRITES::DisposeChildren_(Sprite3D* parent)
  {
    Sprite3D** i;
    while ( ( i = parent->children_.First() ) != 0 ) {
      if ( (*i)->parent_deps_ & SPRITE_DEPS_DISPOSE )
        DisposeSprite_(*i);
      //parent->children_.EraseFirst();
    }
  }

void Impl_3DSPRITES::SetLayerX(long no,float val)
  {
    Sprite3D_Layer* layer = RequestLayer(no,true);
    layer->x = val;
    SPRITES::SetNeedRefresh();
  }

void Impl_3DSPRITES::SetLayerY(long no,float val)
  {
    Sprite3D_Layer* layer = RequestLayer(no,true);
    layer->y = val;
    SPRITES::SetNeedRefresh();
  }

void Impl_3DSPRITES::SetLayerVisibility(long no,bool val)
  {
    Sprite3D_Layer* layer = RequestLayer(no,true);
    if ( layer->visible != val )
      {
        layer->visible = val;
        visiblity_changed_ = true;
        SPRITES::SetNeedRefresh();
      }
  }

float Impl_3DSPRITES::GetLayerX(long no) const
  {
    Sprite3D_Layer* layer = RequestLayer(no);
    if ( !layer ) return 0;
    return layer->x;
  }

float Impl_3DSPRITES::GetLayerY(long no) const
  {
    Sprite3D_Layer* layer = RequestLayer(no);
    if ( !layer ) return 0;
    return layer->y;
  }

bool Impl_3DSPRITES::IsLayerVisible(long no) const
  {
    Sprite3D_Layer* layer = RequestLayer(no);
    if ( !layer ) return true;
    return layer->visible;
  }

void Impl_3DSPRITES::AssignLayerWithSprites(long layerno,Sprite3D_Layer* layer)
  {
    CollectionIteratorT<Sprite3D> i = sprites_.Iterate();
    while ( i.Next() )
      if ( i->layered_.layer_ == layerno )
        {
          REQUIRE ( !i->layerObj_ );
          i->layerObj_ = layer;
        }
  }

SPRITES_Inst* Create3DSpritesInst()
  {
    return new Impl_3DSPRITES;
  }
