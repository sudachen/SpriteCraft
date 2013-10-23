
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
#include "flat_e.h"
#include "flat_e0.h"
//#include "../mUTIL/m_properties.h"
//#include "../mUTIL/m_sst.h"
#include "SST.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$flat$e")
#endif

//extern string WideToString(pwide_t val);

static pchar_t SPRITES_HAS_NOT_INITIALIZED = "[SPRITES] has not initialized";
static SCERROR SPRITE_LoadInfo(pwide_t kname,SPRITE_Info* info);
static SCERROR SPRITE_DirectInfo(pwide_t kname,SPRITE_Info* info);

SPRITES_Inst* SPRITES::instance_ = 0;

IRefObject* SPRITES::Acquire()
  {
    if ( !instance_ )
      {
        if ( SCREEN::Get3DDevice() )
          {
            Create3DSpritesInst(); // устанавливает реализацию спрайт-энжина
          }
        else ret0_because("only 3d devices supported by sprite-engine now");
        REQUIRE (instance_ != 0);
      }
    else
      Grab(instance_);
    return instance_;
  }

void SPRITES::Release()
  {
    return_if ( !instance_ );
    instance_->Release_(); // при нулевом количестве ссылок удалит реализацию
    //instance_ = 0;
  }

SCERROR SPRITES::Restore()
  {
    if ( instance_ )
      return instance_->Restore();
    else return SCE_OK;
  }

SCERROR SPRITES::Display()
  {
    if ( instance_ )
      return instance_->Display();
    else fail_because(SPRITES_HAS_NOT_INITIALIZED);
  }

pchar_t const ComplexSprite::TypeID__ = "#TYPEID#mskin::ComplexSprite";
pchar_t const TriangledSprite::TypeID__ = "#TYPEID#mskin::TriangledSprite";

pchar_t const Sprite::TypeID__ = "#TYPEID#mskin::Sprite";
void* Sprite::Adaptate_(const void* id)
  {
    if ( id == Sprite::TypeID__ )
      return this;
    return Adaptable::Adaptate_(id);
  }

Sprite::Sprite() {}
Sprite::~Sprite() {}

SCERROR SPRITES::Purge()
  {
    if ( instance_ )
      {
        return instance_->Purge();
      }
    else
      fail_because(SPRITES_HAS_NOT_INITIALIZED);
  }

SCERROR SPRITES::NewKlass(pwide_t klass)
  {
    if ( !instance_ )
      fail_because(SPRITES_HAS_NOT_INITIALIZED);
    SPRITE_Info info;
    if ( klass[0] != '!' )
      fail_if_err ( SPRITE_LoadInfo(klass,&info) );
    else
      fail_if_err ( SPRITE_DirectInfo(klass,&info) );
    return instance_->NewKlass(klass,info);
  }

SCERROR SPRITES::NewDynamicKlass(pwide_t klass,KlassProvider *provider)
  {
    if ( !instance_ )
      fail_because(SPRITES_HAS_NOT_INITIALIZED);
    return instance_->NewDynamicKlass(klass,provider);
  }

SCERROR SPRITES::NewDummyKlass(pwide_t klass,bool renderable)
  {
    if ( !instance_ ) fail_because(SPRITES_HAS_NOT_INITIALIZED);
    SPRITE_Info info;
    info.renderable_    = renderable;
    info.bitmap_height  = 1;
    info.bitmap_width   = 1;
    info.compress_it    = false;
    info.filtering      = false;
    info.hotspot_x      = 0;
    info.hotspot_y      = 0;
    info.is_font        = false;
    info.is_paletted    = false;
    info.spr_height     = 1;
    info.spr_width      = 1;
    info.spr_no         = 1;
    info.transparecy    = 0;
    info.type           = SPRITE_SOLID;
    info.dummy_         = true;
    info.additive_      = false;
    info.collide        = SPRITE_COLLIDE_RECTANGLE;
    info.is_region      = false;
    return instance_->NewKlass(klass,info);
  }

SCERROR SPRITES::UpdateKlass(pwide_t name)
  {
    if ( !instance_ )
      fail_because(SPRITES_HAS_NOT_INITIALIZED);
    return instance_->UpdateKlass(name);
  }

SCERROR SPRITES::UnloadKlass(pwide_t klass,bool flag)
  {
    if ( !instance_ )
      fail_because(SPRITES_HAS_NOT_INITIALIZED);
    return instance_->UnloadKlass(klass,flag);
  }

SCERROR SPRITES::RestoreKlass(pwide_t klass)
  {
    if ( !instance_ )
      fail_because(SPRITES_HAS_NOT_INITIALIZED);
    return instance_->RestoreKlass(klass);
  }

bool SPRITES::HasKlass(pwide_t klass)
  {
    if ( !instance_ )
      ret0_because(SPRITES_HAS_NOT_INITIALIZED);
    return instance_->HasKlass(klass);
  }

SCERROR SPRITES::NewFontKlassFrom(pwide_t fntklass,pwide_t resource,
  bool filtering,pwide_t symbols,SPRITE_IMAGE_TYPE t,long ckey)
  {
    if ( !instance_ ) fail_because(SPRITES_HAS_NOT_INITIALIZED);
    if ( instance_->HasKlass(fntklass) )
      fail_because(_S*":SPRITES_Inst3D::NewFontKlassFrom: klass %s already exists" %fntklass );
    else
      {
        SPRITE_Info info;
        fail_if_fail( SPRITE_FillInfo(
          fntklass,&info,resource,t,ckey,filtering,false,true,1,1,symbols) );
        fail_if_fail( instance_->NewKlass(fntklass,info) );
      }
    return SCE_OK;
  }

Sprite* SPRITES::NewSprite(pwide_t name,pwide_t klass,int layer)
  {
    if ( !instance_ ) ret0_because(SPRITES_HAS_NOT_INITIALIZED);
    Sprite* spr = instance_->NewSprite(name,klass,layer);
    if ( spr )
      {
        spr->Hide();
        spr->Move(0,0);
      }
    return spr;
  }

Sprite* SPRITES::NewSprite(pwide_t name,pwide_t klass,int layer,int x, int y)
  {
    if ( instance_ )
      {
        Sprite* spr = instance_->NewSprite(name,klass,layer);
        if ( spr )
          {
            spr->Move((float)x,(float)y);
            spr->Show();
          }
        return spr;
      }
    else ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

Sprite* SPRITES::NewSpriteFrom(pwide_t name,pwide_t resource,
  int layer,int hlayout,int vlayout,
  bool filtering,bool cmprs,SPRITE_IMAGE_TYPE t,long ckey)
  {
    if ( instance_ )
      {
        Sprite* spr = 0;
        StringW klass = _S*L"$${%s}:%dx%d-%d%d-%d-%06d"
            %resource %hlayout %vlayout %filtering %cmprs %t %ckey;

        if ( instance_->HasKlass(+klass) )
          spr = instance_->NewSprite(name,+klass,layer);
        else
          {
            SPRITE_Info info;
            ret0_if_fail( SPRITE_FillInfo(+klass,&info,resource,t,ckey,filtering,cmprs,false,hlayout,vlayout,0) );
            ret0_if_fail( instance_->NewKlass(+klass,info) );
            spr = instance_->NewSprite(name,+klass,layer);
          }
        if ( spr )
          {
            spr->Move(0,0);
            spr->Show();
          }
        return spr;
      }
    else
      ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

SCERROR SPRITES::DisposeSprite(pwide_t name)
  {
    if ( instance_ )
      return instance_->DisposeSprite(name);
    else fail_because(SPRITES_HAS_NOT_INITIALIZED);
  }

SCERROR SPRITES::UnloadAll()
  {
    if ( instance_ )
      return instance_->UnloadAll();
    else fail_because(SPRITES_HAS_NOT_INITIALIZED);
  }

Sprite* SPRITES::FindSprite(pwide_t name)
  {
    if ( instance_ )
      return instance_->FindSprite(name);
    else ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

Sprite* SPRITES::FindSpriteAt(long x,long y,long nearlayer,long farlayer)
  {
    if ( instance_ )
      return instance_->FindSpriteAt(x,y,nearlayer,farlayer);
    else ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

void SPRITES::SetNeedRefresh(bool val)
  {
    if ( instance_ ) instance_->SetNeedRefresh(val);
    else return_because(SPRITES_HAS_NOT_INITIALIZED);
  }

bool SPRITES::NeedRefresh()
  {
    if ( instance_ )
      return instance_->NeedRefresh();
    else ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

SCERROR SPRITES::EnumerateSprites(ArrayT<Sprite*>& v,long lnear,long lfar)
  {
    if ( instance_ ) return instance_->EnumerateSprites(v,lnear,lfar);
    else fail_because(SPRITES_HAS_NOT_INITIALIZED);
  }


void SPRITES::SetLayerX(long no,float val)
  {
    if ( instance_ )
      instance_->SetLayerX(no,val);
    else return_because(SPRITES_HAS_NOT_INITIALIZED);
  }

void SPRITES::SetLayerY(long no,float val)
  {
    if ( instance_ )
      instance_->SetLayerY(no,val);
    else return_because(SPRITES_HAS_NOT_INITIALIZED);
  }

void SPRITES::SetLayerVisibility(long no,bool val)
  {
    if ( instance_ )
      instance_->SetLayerVisibility(no,val);
    else return_because(SPRITES_HAS_NOT_INITIALIZED);
  }

float SPRITES::GetLayerX(long no)
  {
    if ( instance_ )
      return instance_->GetLayerX(no);
    else ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

float SPRITES::GetLayerY(long no)
  {
    if ( instance_ )
      return instance_->GetLayerY(no);
    else ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

bool SPRITES::IsLayerVisible(long no)
  {
    if ( instance_ )
      return instance_->IsLayerVisible(no);
    else ret0_because(SPRITES_HAS_NOT_INITIALIZED);
  }

static SPRITE_IMAGE_TYPE ToImageType( pwide_t val )
  {
    StringT<char,32> sval(val);
    sval.Trim().ToLower();
    if ( sval == "solid")     return SPRITE_SOLID;
    if ( sval == "bitmask")   return SPRITE_BITMASK;
    if ( sval == "colorkey")  return SPRITE_COLORKEY;
    if ( sval == "blend")     return SPRITE_BLEND;
    return SPRITE_INVALID_TYPE;
  }

SCERROR SPRITE_FillInfo(
  pwide_t kname,SPRITE_Info* info,pwide_t bitmap_resource,
  SPRITE_IMAGE_TYPE imgt,u32_t crgb,
  bool filtering,bool cmprs,bool isfnt,
  int hor_layout, int ver_layout, pwide_t symbols)
  {
    fail_if ( !info );
    fail_if ( !bitmap_resource );
    info->bitmap = bitmap_resource;
    info->type = imgt;
    info->transparecy = 0;
    info->colorkey = mRGB_t(byte_t(crgb>>16),byte_t(crgb>>8),byte_t(crgb));
    if ( hor_layout <= 0 || ver_layout <= 0 ) fail_because("invalid sprite layout");
    AutoImageDataSource ids ( MakeImageDataSourceFrom( AutoRef(RSCMAN::CreateSource(+info->bitmap)) ) );
    if ( !ids ) fail_because("spiteinfo: invalid sprite bitmap");
    ImageDataSource::ImageInfo img_info;
    fail_if_err ( ids->ReadPicInfo(img_info) );
    info->bitmap_width  = img_info.width;
    info->bitmap_height = img_info.height;
    info->spr_no = ver_layout*hor_layout;
    info->is_paletted = ( img_info.bpp == 8 );
    info->spr_height = info->bitmap_height/ver_layout;
    info->spr_width  = info->bitmap_width/hor_layout;
    info->hotspot_x = 0;
    info->hotspot_y = 0;
    info->collide = SPRITE_COLLIDE_RECTANGLE;
    if ( isfnt )
      {
        info->is_font = true;
        if ( symbols ) {
          info->symbols = symbols;
          // удаляем завершающие пробелы
          info->symbols.RightTrim();
          //string& s = info->symbols;
          //s.erase(find_if(s.rbegin(),s.rend(),bind2nd(not_equal_to<char>(),' ')).base(),s.end());
        }
      }
    else
      info->is_font = false;

    info->filtering     = filtering;
    info->compress_it   = cmprs;
    info->additive_     = false;
    info->renderable_   = true;
    info->is_region      = false;
    return SCE_OK;
  }

static SCERROR SPRITE_LoadInfo(pwide_t kname,SPRITE_Info* info)
  {
    fail_if ( !info );
    Xnode *xmln = SST->GetKlass(kname);
    if ( !xmln ) fail_because( _S*"spiteinfo: klass %? is not defined" %kname );
    bool is_sprite = !wcsicmp(xmln->GetName(),L"sprite");
    bool is_font   = !wcsicmp(xmln->GetName(),L"font");
    if ( !is_sprite && !is_font )
      fail_because( _S*"spiteinfo: invalid record type of klass %?" %kname );

    if ( pwide_t bitmap = xmln->GetParam(L"bitmap") ) info->bitmap = bitmap, info->bitmap.Trim();
    else
      fail_because("spiteinfo: sprite bitmap is not specified");

    if ( pwide_t alpha  = xmln->GetParam(L"alpha") ) info->alpha = alpha, info->alpha.Trim();

    if ( pwide_t type = xmln->GetParam(L"type") ) {
      info->type   = ToImageType( type );
      if ( info->type == SPRITE_INVALID_TYPE )
        fail_because( _S*"spiteinfo: invalid sprite type of klass %?" %kname );
    }
    else info->type = SPRITE_SOLID;

    info->bits_rgb32 = teggo::ToBool(xmln->GetParam(L"truecolor"));
    info->bits_lowcolor  = teggo::ToBool(xmln->GetParam(L"lowcolor"));

    if ( pwide_t transp = xmln->GetParam(L"transparency") ) info->transparecy = teggo::ToLong(transp);
    else info->transparecy = 0;

    if ( pwide_t keyrgb = xmln->GetParam(L"keyrgb") ) {
      u32_t r = 0,g = 0,b = 0;
      swscanf(StringT<wchar_t,32>(keyrgb).ToLower().Trim().Str(),L"%2lx%2lx%2lx",&r,&g,&b);
      info->colorkey = mRGB_t(r,g,b);
    }else info->transparecy = 0;

    AutoImageDataSource ids ( MakeImageDataSourceFrom( AutoRef(RSCMAN::CreateSource(+info->bitmap)) ) );
    if ( !ids )
      fail_because("spiteinfo: invalid sprite bitmap resource name");
    ImageDataSource::ImageInfo img_info;
    img_info.frames = 1;
    fail_if_err ( ids->ReadPicInfo(img_info) );

    int hor_layout = 1;
    int ver_layout = 1;

    if ( pwide_t layout = xmln->GetParam(L"layout") ) {
        swscanf(layout,L"%dx%d",&hor_layout,&ver_layout);
      if ( hor_layout <= 0 || ver_layout <= 0 )
        fail_because(_S*"spiteinfo: invalid sprite layout '%?' of klass %?" %layout %kname );
    }else{
      if ( img_info.frames > 1 ) {
        ver_layout = img_info.frames;
      }
    }

    info->bitmap_width  = img_info.width;
    info->bitmap_height = img_info.height;
    info->spr_no = ver_layout*hor_layout;
    info->is_paletted = ( img_info.bpp == 8 );
    info->spr_height = info->bitmap_height/ver_layout;
    info->spr_width  = info->bitmap_width/hor_layout;
    info->hotspot_x = 0;
    info->hotspot_y = 0;
    if ( pwide_t xhotspot = xmln->GetParam(L"hotspot") ) {
      StringT<wchar_t,32> hotspot(xhotspot); hotspot.Trim().ToLower();
      if ( hotspot == L"center" ) {
        info->hotspot_x = info->spr_width/2;
        info->hotspot_y = info->spr_height/2;
      }else{
        swscanf(+hotspot,L"%d,%d",&info->hotspot_x,&info->hotspot_y);
      }
    }
    if ( is_font )
    {
      info->is_font = true;
      info->symbols = xmln->GetParam(L"symbols");
      // удаляем завершающие пробелы
      info->symbols.RightTrim();
      //string& s = info->symbols;
      //s.erase(find_if(s.rbegin(),s.rend(),bind2nd(not_equal_to<char>(),' ')).base(),s.end());
    }else{
      info->is_font = false;
    }
    //XMessage(("SPRITE_Info: hotspot %dx%d",info->hotspot_x,info->hotspot_y));
    info->filtering = teggo::ToBool(xmln->GetParam(L"filtering"));
    info->additive_  = teggo::ToBool(xmln->GetParam(L"additive"));
    info->compress_it = teggo::ToBool(xmln->GetParam(L"compress"));

    if ( pwide_t round = xmln->GetParam(L"round") )
      info->round = (teggo::ToLong(round)&0x0ff);

    if ( pwide_t mipmap = xmln->GetParam(L"mipmap") )
      info->mipmap = (teggo::ToLong(mipmap)&0x0f);

    info->dummy_ = false;
    info->renderable_ = true;
    if ( pwide_t collide = xmln->GetParam(L"collide") )
    {
      StringT<wchar_t,32> collidtype = collide;
      collidtype.Trim().ToLower();
      if ( collidtype == L"rect" )
        info->collide = SPRITE_COLLIDE_RECTANGLE;
      else if ( collidtype == L"point" ) {
        info->collide = SPRITE_COLLIDE_RADIUS;
        info->collideRadius = 0;
      }
      else if ( collidtype == L"bitmask" )
        info->collide = SPRITE_COLLIDE_BITMASK;
      else {
        long radius;
        if ( swscanf(collidtype.Str(),L"%ld",&radius) ) {
          info->collide = SPRITE_COLLIDE_RADIUS;
          info->collideRadius = (float)radius;
        }else
          fail_because(_S*"spiteinfo: invalid sprite collide type '%?' of klass %?"
                        %collide %kname );
      }
    }
    else info->collide = SPRITE_COLLIDE_RECTANGLE;

    if ( pwide_t region = xmln->GetParam(L"region") )
    {
      info->is_region = true;
      long right = 0,bottom = 0;
      if ( info->bitmap_height <= 256 && info->bitmap_width <= 256 ) {
        if ( 4 != swscanf(region,L"%d,%d,%d,%d",&info->spr_x,&info->spr_y,&right,&bottom) )
          fail_because( _S*"spiteinfo: invalid region '%?' of klass %?" %region %kname );
        info->spr_width  = right - info->spr_x;
        info->spr_height = bottom - info->spr_y;
        info->spr_no = 1;
      }
      else
        fail_because(L"spiteinfo: region can applayed only to 256x256 images or less");
    }
    else
    {
      info->is_region = false;
    }

    return SCE_OK;
  }

static SCERROR SPRITE_DirectInfo(pwide_t kname,SPRITE_Info* info)
  {
    REQUIRE ( kname[0] == L'!' );
    fail_if ( !info );
    info->bitmap = kname+1;
    info->transparecy = 0;
    memset(&info->colorkey,0,sizeof(info->colorkey));

    AutoImageDataSource ids (
      MakeImageDataSourceFrom( AutoRef(RSCMAN::CreateSource(+info->bitmap)) ) );
    if ( !ids )
      fail_because("spiteinfo: invalid sprite bitmap resource name");
    ImageDataSource::ImageInfo img_info;
    img_info.frames = 1;
    fail_if_err ( ids->ReadPicInfo(img_info) );
    info->bitmap_width  = img_info.width;
    info->bitmap_height = img_info.height;
    info->spr_no = img_info.frames;
    info->is_paletted = ( img_info.bpp == 8 );
    info->spr_height = info->bitmap_height/img_info.frames;
    info->spr_width  = info->bitmap_width;
    info->hotspot_x = 0;
    info->hotspot_y = 0;
    info->is_font = false;
    info->filtering = true;
    info->additive_  = false;
    info->compress_it = false;
    info->dummy_ = false;
    info->renderable_ = true;
    info->is_region = false;
    if ( img_info.bpp == 32 )
      {
        info->type = SPRITE_BLEND;
        info->collide = SPRITE_COLLIDE_BITMASK;
      }
    else
      {
        info->type = SPRITE_SOLID;
        info->collide = SPRITE_COLLIDE_RECTANGLE;
      }
    return SCE_OK;
  }
