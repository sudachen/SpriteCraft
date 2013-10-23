
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
#include "RBGFilter.h"
#include "CollideMgr.h"
#include "SpriteHelper.h"
#include "dispsubimpl.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Sprites$Helper")
#endif

pchar_t const SpriteHelper::TypeID__ = "#TYPEID#SpriteCraft::SpriteHelper";
teggo::randizer_t SpriteHelper::g_randomizer;

static pwide_t NOT_EXISTS_INFO = L"Sprite is not exists";
static pwide_t INVALID_STYLE_INFO = L"Invalid sprite style";

SpriteLwsArray* IStyleHelper::AsLwsArray()  { return 0; }
SpritePrimitive* IStyleHelper::AsPrimitive() { return 0; }
SpriteTextView* IStyleHelper::AsTextView()  { return 0; }
SpriteParticles* IStyleHelper::AsParticles() { return 0; }
void IStyleHelper::Displace() {}
void IStyleHelper::SetSpriteWH(float,float) {}

SpriteHelper::SpriteHelper()
  :  style_(StyleUnknown), styleHelper_(0), sprite_(0), noname_(false)
  {
    random_ = teggo::float_random(&g_randomizer);
  }

SpriteHelper::~SpriteHelper()
  {
    if ( styleHelper_ )
      {
        styleHelper_->Displace();
        Unrefe(styleHelper_);
      }
    if ( +weak_ )
      weak_->ForgetSprite();
  }

void SpriteHelper::Grab_() const
  {
    ((ISprite*)this)->AddRef();
  }

void SpriteHelper::Release_() const
  {
    ((ISprite*)this)->Release();
  }

Sprite* SpriteHelper::GetSprite_()
  {
    return sprite_;
  }

IUnknown* SpriteHelper::GetSubImpl_()
  {
    return +subimpl_;
  }

void* SpriteHelper::Adaptate_(const void* id)
  {
    if ( TypeID__ == id )
      return this;

    if ( id == ComplexSprite::TypeID__ && styleHelper_ )
      if ( SpriteLwsArray* lwsa = styleHelper_->AsLwsArray() )
        return lwsa->GetComplexSprite();

    if ( id == TriangledSprite::TypeID__ && styleHelper_ )
      if ( SpritePrimitive* prim = styleHelper_->AsPrimitive() )
        return prim->GetTriangled();

    return IRefObject::Adaptate_(id);
  }

bool SpriteHelper::Update_(u32_t tickdelta)
  {
    if ( +update_ )
      {
        rcc_ptr<IUnknown> guard0(Refe(this->__iunknown()));
        HRESULT hr = update_->OnUpdate(this,tickdelta);
        if ( FAILED(hr) )
          Lout << _S*"sprite: IOnUpdate_OnUpdate('%s','%d') failed, HRESUT = 0x%08x "
                    %(sprite_?sprite_->GetName():L"<displaced>") %tickdelta %hr;
        return true;
      }
    else
      return false;
  }

bool SpriteHelper::OnCollide(Sprite* s)
  {
    if ( s && +collide_ )
      {
        SpriteHelper* sh = Adaptate<SpriteHelper>(s->GetSubImpl());
        if ( sh )
          {
            rcc_ptr<IUnknown> guard0(Refe(this->__iunknown()));
            rcc_ptr<IUnknown> guard1(Refe(sh->__iunknown()));
            HRESULT hr = collide_->OnCollide(this,sh);
            if ( FAILED(hr) )
              Lout << _S*"sprite: IOnCollid_OnCollide('%s','%s') failed, HRESUT = 0x%08x "
                      %(sprite_?sprite_->GetName():L"<displaced>")
                      %(sh->sprite_?sh->sprite_->GetName():L"<displaced>") %hr;
          }
      }
    return true;
  }

HRESULT __stdcall SpriteHelper::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(ISprite),riid)?S_OK:S_FALSE;
  }

void SpriteHelper::IColorHelper_SetColor(mrgb_t c)
  {
    if ( !sprite_ )
      return;

    sprite_->SetRGB(c);
  }

mrgb_t SpriteHelper::IColorHelper_GetColor() const
  {
    if ( !sprite_ )
      return mRGB_t(0,0,0);

    return sprite_->GetRGB();
  }

HRESULT __stdcall  SpriteHelper::get_name(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = SysAllocString(sprite_->GetName());
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_klass(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = SysAllocString(sprite_->GetKlass());
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_layer(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetLayer();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_layer(long newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetLayer(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_x(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetX();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_x(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->MoveX(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_y(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetY();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_y(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->MoveY(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_degree(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetDegee();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_degree(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->Rotate(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_frno(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetFrameNo();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_frno(long newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetFrameNo(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_visible(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->IsVisible()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_visible(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( newVal!=VARIANT_FALSE )
      sprite_->Show();
    else
      sprite_->Hide();

    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::Dispose()
  {
    _SYNCHRONIZE_;
    if ( sprite_ )
      {
        sprite_->Dispose();
        sprite_ = 0;
      }
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_text(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = SysAllocString(sprite_->GetText());
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_text(BSTR newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetText(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_height(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetHeight();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_width(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetWidth();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_transparency(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetTransparency();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_transparency(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetTransparency(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_luminance(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetLuminance();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_luminance(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetLuminance(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_mouseOver(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->Contains(Engine_GetMouseX(),Engine_GetMouseY())
      ?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::Equal(IDispatch *Ip, VARIANT_BOOL *equ)
  {
    _SYNCHRONIZE_;
    *equ = (Ip == static_cast<IDispatch*>(this))
      ?VARIANT_TRUE
      :VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_xScale(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetScaleX();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_xScale(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->ScaleX(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_yScale(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetScaleY();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_yScale(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->ScaleY(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_xyScale(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->Scale(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_sprLeft(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rect_t rect = sprite_->GetRect();
    *pVal = float(rect.left);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_sprTop(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rect_t rect = sprite_->GetRect();
    *pVal = float(rect.top);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_sprRight(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rect_t rect = sprite_->GetRect();
    *pVal = float(rect.right);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_sprBottom(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rect_t rect = sprite_->GetRect();
    *pVal = float(rect.bottom);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::Contains(float x, float y, VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *bRet = sprite_->Contains(x,y)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_xSize(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rect_t rect = sprite_->GetRect();
    *pVal = float(rect.Width());
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_xSize(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetSizeX(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_ySize(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rect_t rect = sprite_->GetRect();
    *pVal = float(rect.Height());
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_ySize(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetHotspot(HOTSPOT_CUSTOM);
    sprite_->SetSizeY(newVal);
    return S_OK;
  }


HRESULT __stdcall  SpriteHelper::get_xHotspot(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetHotspotX();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_xHotspot(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetHotspot(HOTSPOT_CUSTOM);
    sprite_->SetHotspotX(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_yHotspot(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetHotspotY();
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_yHotspot(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetHotspotY(newVal);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_hotspot(enum tagHotspot newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetHotspot(newVal);
    return S_OK;
  }


HRESULT __stdcall  SpriteHelper::MoveTo(float x, float y)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->Move(x,y);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_xFlip(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->IsFlipX() ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_xFlip(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetFlipX(newVal!=VARIANT_FALSE);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_yFlip(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->IsFlipY() ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_yFlip(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetFlipY(newVal!=VARIANT_FALSE);
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_cfilt(IRGBFilter **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = new ColorObject(Refe((IColorHelper*)this));
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_parent(ISprite **pVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    *pVal = 0;
    if ( Sprite* spr = sprite_->GetParent() )
      {
        ISpriteHelperEx* helper = Adaptate<SpriteHelper>(spr->GetSubImpl());
        if ( helper )
          return helper->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pVal);
      }
    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::put_parent(ISprite *newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( newVal )
      {
        rcc_ptr<ISpriteHelperEx2> helper = teggo::ifs_cast<ISpriteHelperEx2>(newVal);
        if ( !helper )
          return GenSCWideError(L"invalid sprite",L":Set parent sprite:");
        Sprite* spr = helper->GetSprite_();
        if ( !spr )
          return GenSCWideError(NOT_EXISTS_INFO);
        sprite_->SetParent(spr);
      }
    else
      {
        sprite_->SetParent(0);
      }

    return S_OK;
  }

HRESULT __stdcall  SpriteHelper::get_hierarchy(IHierarchy** pI)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pI = new Hierarchy(this);
    return S_OK;
  }

u32_t  SpriteHelper::Ex_GetParentDeps() const
  {
    if ( !sprite_ )
      return 0xffffffff;
    else
      return sprite_->GetParentDeps();
  }

void SpriteHelper::Ex_SetParentDeps(u32_t val)
  {
    if ( !sprite_ )
      return;
    else
      sprite_->SetParentDeps(val);
  }

HRESULT __stdcall SpriteHelper::get_angle(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetAngle();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_angle(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetAngle(newVal);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_weakref(IWeakSprite **pVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( !weak_ )
      weak_.reset_(new WeakSprite(this));

    *pVal = Refe(+weak_);

    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_subimpl(IUnknown **pVal)
  {
    return get_dispatcher(pVal);
  }

HRESULT __stdcall SpriteHelper::get_dispatcher(IUnknown **pVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( !subimpl_ )
      *pVal = 0;
    else
      *pVal = Refe(+subimpl_);

    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_subimpl(IUnknown *newVal)
  {
    return put_dispatcher(newVal);
  }

HRESULT __stdcall SpriteHelper::put_dispatcher(IUnknown *newVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    rcc_ptr<Engine_IDispSubImpl> pI;

    if ( +subimpl_
      && SUCCEEDED(subimpl_->QueryInterface(TEGGO_GUIDOF(Engine_IDispSubImpl),(void**)&+pI)) )
      {
        Lout << _S*"engine: sprite '%?' already connected" %sprite_->GetName();
        return GenSCWideError(L"Could not change current logic subimplementation");
      }
    else
      return put_subimpl_(newVal);
  }

HRESULT SpriteHelper::put_subimpl_(IUnknown *newVal)
  {
    rcc_ptr<IUnknown> tmp_(subimpl_.forget_());
    rcc_ptr<IUnknown> tmpUpdate_ (update_.forget_());
    rcc_ptr<IUnknown> tmpCollide_(collide_.forget_());

    if ( newVal )
      subimpl_.reset_(Refe(newVal));

    if ( +subimpl_ )
      {
        subimpl_->QueryInterface(TEGGO_GUIDOF(IOnUpdate),(void**)&+update_);
        subimpl_->QueryInterface(TEGGO_GUIDOF(IOnCollide),(void**)&+collide_);
        if ( +update_ )
          {
            if ( !weak_ )
              weak_.reset_(new WeakSprite(this));
            Engine_ComAddOnUpdateListener(+weak_);
          }
      }

    if ( +weak_ )
      Engine_ComRemoveSpriteMouseListener(+weak_);

    if ( +subimpl_ )
      {
        rcc_ptr<IUnknown> tmp;
        if (
             SUCCEEDED(subimpl_->QueryInterface(TEGGO_GUIDOF(IOnMouseOver),(void**)&+tmp))
          || SUCCEEDED(subimpl_->QueryInterface(TEGGO_GUIDOF(IOnMouseClick),(void**)&+tmp))
          || SUCCEEDED(subimpl_->QueryInterface(TEGGO_GUIDOF(IOnMouseUp),(void**)&+tmp))
          || SUCCEEDED(subimpl_->QueryInterface(TEGGO_GUIDOF(IOnMouseDown),(void**)&+tmp))
          || SUCCEEDED(subimpl_->QueryInterface(TEGGO_GUIDOF(IOnMouseMove),(void**)&+tmp)) )
          {
            if ( !weak_ )
              weak_.reset_(new WeakSprite(this));
            Engine_ComAddSpriteMouseListener(+weak_);
          }
      }

    return S_OK;
  }

void SpriteHelper::CheckDispose()
  {
    // если есть спрайт,
    // он без имени и неконролируется другим спрайтом, то уничтожить спрайт
    if ( sprite_ && noname_ &&
      !(sprite_->GetParent() && (sprite_->GetParentDeps()&SPRITE_DEPS_DISPOSE)) )
      sprite_->Dispose();
  }

void SpriteHelper::SetSpritePtr(Sprite* sprite)
  {
    //XMessage(("SpriteHelper::SetSpritePtr(%08x) %08x",sprite,this));
    sprite_ = sprite;

    if ( styleHelper_ )
      styleHelper_->Displace(), Unrefe(styleHelper_);

    if ( !sprite_ )
      {

        if ( +subimpl_ )
          {
            rcc_ptr<IOnDispose> on_dispose;
            if ( SUCCEEDED(subimpl_->
                      QueryInterface(TEGGO_GUIDOF(IOnDispose),(void**)&+on_dispose)) )
              on_dispose->OnDispose(this);
          }

        if ( +weak_ )
          weak_->UnlinkCollide();

        Unrefe(update_);
        Unrefe(collide_);
        Unrefe(subimpl_);
      }
    else
      {
        style_ = sprite_->IsText() ? StyleText : StylePicture;
      }
  }

HRESULT __stdcall SpriteHelper::get_alive(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = !sprite_ ? VARIANT_FALSE : VARIANT_TRUE;
    return S_OK;
  }


HRESULT __stdcall SpriteHelper::get_random(float *pVal)
  {
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = random_;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_frcount(long *pVal)
  {
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetFrCount();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::Randomize()
  {
    _SYNCHRONIZE_;
    random_ = teggo::float_random(&g_randomizer);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_scrX(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetScrX();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_scrY(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetScrY();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_scrAngle(float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetScrAngle();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_collide(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = collide_type_;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_collide(long newVal)
  {
    _SYNCHRONIZE_;

    if ( +weak_ )
      weak_->UnlinkCollide();
    collide_type_ = 0;

    if ( newVal )
      {
        long group = newVal & 0x0fff;
        if ( group >= CollideMgr::MAX_GROUPS+1 || group <= 0 )
          return GenSCWideError(_S*L":Sprite.collide: invalide collision group %d" %group);
        if ( !(newVal & (CollideActive|CollidePassive)) )
          return GenSCWideError(L":Sprite.collide: invalide collision class");
        bool active = !!(CollideActive & newVal);
        bool trackable = !!(CollideTrack & newVal);
        collide_type_ = newVal;
        if ( !weak_ )
          weak_.reset_(new WeakSprite(this));
        Engine_ComAddCollidable(+weak_,group-1,active,trackable);
      }
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::Hit(ISprite *pI, VARIANT_BOOL *bVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    rcc_ptr<ISpriteHelperEx2> helper = teggo::ifs_cast<ISpriteHelperEx2>(pI);
    if ( !helper )
      return GenSCWideError(L"invalid argument",L":Set parent sprite:");

    Sprite* spr = helper->GetSprite_();
    if ( !spr )
      return GenSCWideError(NOT_EXISTS_INFO);

    *bVal = sprite_->Hit(spr)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }


HRESULT __stdcall SpriteHelper::get_cookie(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = cookie_;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_cookie(long newVal)
  {
    _SYNCHRONIZE_;
    cookie_ = newVal;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_item(long no, long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = 0;
    if ( +items_ )
      {
        item_t ii  = { no, 0 };
        if ( item_t *i = teggo::binary_lower(items_->Begin(),items_->End(),ii) )
          *pVal =  (*i).value;
      }
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_item(long no, long newVal)
  {
    _SYNCHRONIZE_;

    item_t val =  { no, newVal };

    if ( !items_ )
      items_.reset_( new BufferT<item_t> );

      items_->BinsertOne(val);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_sublayer(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->GetSubLayer();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_sublayer(long newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetSubLayer(newVal);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::Distance(ISprite *sprite, float *pVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    rcc_ptr<ISpriteHelperEx2> helper = teggo::ifs_cast<ISpriteHelperEx2>(sprite);
    if ( !helper )
      return GenSCWideError(L"invalid argument",L":Distance:");

    Sprite* spr = helper->GetSprite_();
    if ( !spr )
      return GenSCWideError(NOT_EXISTS_INFO);

    *pVal = sprite_->Distance(spr);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::AngleWith(ISprite *sprite, float *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    rcc_ptr<ISpriteHelperEx2> helper = teggo::ifs_cast<ISpriteHelperEx2>(sprite);
    if ( !helper )
      return GenSCWideError(L"invalid argument",L":AngleWith:");

    Sprite* spr = helper->GetSprite_();
    if ( !spr )
      return GenSCWideError(NOT_EXISTS_INFO);

    *pVal = sprite_->AngleWith(spr);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_outOfScreen(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = sprite_->OutOfScreen()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::IncAngle(float val)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->IncAngle(val);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::IncX(float val)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->IncX(val);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::IncY(float val)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->IncY(val);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::NameIs(BSTR name, VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = wcsicmp(sprite_->GetName(),name) == 0 ? VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::ScreenToSprite(float* x,float* y)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( !x || !y )
      return GenSCWideError(L"invalid arguments");

    sprite_->ScreenToSprite(x,y);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::SpriteToScreen(float* x,float* y)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( !x || !y )
      return GenSCWideError(L"invalid arguments");

    sprite_->SpriteToScreen(x,y);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::AnimateOnce(float fps, long startFrno, long stopFrno, IUnknown *listener, long cookie)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( !weak_ )
      weak_.reset_(new WeakSprite(this));

    if ( stopFrno < 0 )
      stopFrno = sprite_->GetFrCount()-1;

    if ( startFrno > stopFrno )
      startFrno = stopFrno;

    Engine_ComStartSpriteAnimation(+weak_,startFrno,stopFrno,fps,listener,cookie,false);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::AnimateLoop(float fps, long startFrno, long stopFrno, IUnknown *listener, long cookie)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( !weak_ )
      weak_.reset_(new WeakSprite(this));

    if ( stopFrno < 0 )
      stopFrno = sprite_->GetFrCount()-1;

    if ( startFrno > stopFrno )
      startFrno = stopFrno;

    Engine_ComStartSpriteAnimation(+weak_,startFrno,stopFrno,fps,listener,cookie,true);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::StopAnimation()
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    if ( weak_ )
      Engine_ComStopSpriteAnimation(+weak_);
    return S_OK;
  }

#define ___ChangeKlassTo_Reason FormatS(":ChangeKlass: to '%s'",sklass.c_str()).c_str()
HRESULT __stdcall SpriteHelper::ChangeKlassTo(BSTR klass)
  {
    _SYNCHRONIZE_;
    pwide_t e_info = 0;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    ResetErrorLog();

    if ( !SPRITES::HasKlass(klass) )
      if ( SCE_OK != SPRITES::NewKlass(klass) )
        goto lb_reason;

    if ( SCE_OK != sprite_->ChangeKlass(klass) )
      goto lb_reason;

    if ( styleHelper_ )
      switch ( style_ )
        {
        case StylePicture:
        case StyleLWSA:
        //case StyleShape:
        //case StyleTrilist:
          if ( sprite_->IsText() )
            {
              styleHelper_->Displace();
              Unrefe(styleHelper_);
              style_ = StyleText;
            }
          else
            styleHelper_->SetSpriteWH(sprite_->GetWidth(),sprite_->GetHeight());
          break;
        case StyleText:
        case StyleTextView:
          if ( !sprite_->IsText() )
            {
              styleHelper_->Displace();
              Unrefe(styleHelper_);
              style_ = StylePicture;
            }
          style_ = StyleText;
          break;
        }
    else
      if ( sprite_->IsText() )
        style_ = StyleText;
      else
        style_ = StylePicture;

    return S_OK;
  lb_reason:
    return GenSCWideError(e_info,_S*L":ChangeKlass: to '%?'" %klass);
  }

HRESULT __stdcall SpriteHelper::Clone(BSTR name, ISprite **pI)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    ResetErrorLog();

    Sprite* sprite = 0;//sprite_->Clone(sprite_name.c_str());

    if ( sprite )
      {
        SpriteHelper* helper = new XComObject__<SpriteHelper>;
        sprite->SetSubImpl(helper);
        helper->noname_ = !name || !*name;
        if ( helper->noname_ )
          sprite->SetParentDeps(sprite->GetParentDeps()|SPRITE_DEPS_DISPOSE);
        else
          sprite->SetParentDeps(sprite->GetParentDeps()&~SPRITE_DEPS_DISPOSE);
        *pI = helper;
        return S_OK;
      }
    else
      return GenSCWideError(0,
              _S*L":Clone: sprite '%?' -> '%?' " %sprite_->GetName() %name);
  }

HRESULT __stdcall SpriteHelper::SetFrameOfKlass(long frno, BSTR klass)
  {
    _SYNCHRONIZE_;

    if ( SUCCEEDED(ChangeKlassTo(klass)) )
      {
        sprite_->SetFrameNo(frno);
        return S_OK;
      }
    else
      return E_FAIL;
  }

HRESULT __stdcall SpriteHelper::AttachChild(BSTR bstr_key, ISprite *child)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    if ( child )
      {
        rcc_ptr<ISpriteHelperEx2> helper = teggo::ifs_cast<ISpriteHelperEx2>(child);
        if ( !helper )
          return GenSCWideError(L"invalid sprite",L":Attach child:");

        Sprite* spr = helper->GetSprite_();
        if ( !spr )
          return GenSCWideError(NOT_EXISTS_INFO);

        sprite_->AttachChild(spr,bstr_key);
      }
    else
      {
        return GenSCWideError(L"invalid sprite",L":Attach child:");
      }
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::AttachChildEx(BSTR bstr_key, IDispatch *child)
  {
    return E_UNEXPECTED;
  }

HRESULT __stdcall SpriteHelper::GetChild(BSTR bstr_key, ISprite **pI)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    *pI = 0;
    if ( Sprite* spr = sprite_->GetChild(bstr_key) )
      {
        ISpriteHelperEx* helper = Adaptate<SpriteHelper>(spr->GetSubImpl());
        if ( helper )
          return helper->QueryInterface(TEGGO_GUIDOF(ISprite),(void**)pI);
      }
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::GetChildEx(BSTR bstr_key, IDispatch **pI)
  {
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::GetNameAsPWIDE(wchar_t **retVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    *retVal = (wchar_t*)sprite_->GetName();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::GetKlassAsPWIDE(wchar_t **retVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    *retVal = (wchar_t*)sprite_->GetKlass();
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_style(tagStyle *pVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    *pVal = style_;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_style(tagStyle newVal)
  {
    _SYNCHRONIZE_;

    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);

    switch ( newVal )
      {
      case StylePicture:
      case StyleLWSA:
      case StyleShape:
      case StyleTrilist:
        if ( sprite_->IsText() )
          GenSCWideError(INVALID_STYLE_INFO);
        break;
      case StyleText:
      case StyleTextView:
        if ( !sprite_->IsText() )
          GenSCWideError(INVALID_STYLE_INFO);
        break;
      default: GenSCWideError(INVALID_STYLE_INFO);
      };

    if ( style_ != newVal )
      {
        if ( styleHelper_ )
          {
            styleHelper_->Displace();
            Unrefe(styleHelper_);
          }
        if ( style_ == StyleLWSA )      sprite_->DisableComplex();
        if ( style_ == StyleTrilist )   sprite_->DisableTriangled();
        if ( style_ == StyleShape )     sprite_->DisableTriangled();
        if ( style_ == StyleTextView )  sprite_->SetTextViewActive(false);
        switch ( newVal )
          {
          case StylePicture:
            style_ = StylePicture;
            break;
          case StyleLWSA:
            {
              SpriteLwsArray* lwsa = new SpriteLwsArray();
              styleHelper_ = lwsa;
              if ( sprite_->TryEnableComplex() )
                {
                  style_ = StyleLWSA;
                  if ( sprite_->GetImageType() == SPRITE_BLEND
                  ||   sprite_->GetImageType() == SPRITE_ADDITIVE )
                    lwsa->SetSemitransparent(true);
                }
              else
                style_ = StylePicture, styleHelper_->Displace(), Unrefe(styleHelper_);
              break;
            }
          case StyleTrilist:
            styleHelper_ = new SpritePrimitive(StyleTrilist);
            if ( sprite_->TryEnableTriangled() )
              style_ = StyleTrilist;
            else
              {
                style_ = StylePicture;
                styleHelper_->Displace();
                Unrefe(styleHelper_);
              }
            break;
          case StyleShape:
            styleHelper_ = new SpritePrimitive(StyleShape);
            if ( sprite_->TryEnableTriangled() )
              style_ = StyleShape;
            else
              {
                style_ = StylePicture;
                styleHelper_->Displace();
                Unrefe(styleHelper_);
              }
            break;
          case StyleText:
            if ( !sprite_->IsText() )
              GenSCWideError(INVALID_STYLE_INFO);
            style_ = StyleText;
            break;
          case StyleTextView:
            sprite_->SetTextViewActive(true);
            styleHelper_ = new SpriteTextView(sprite_);
            style_ = StyleTextView;
            break;
          }
      }

    if ( styleHelper_ )
      styleHelper_->SetSpriteWH(sprite_->GetWidth(),sprite_->GetHeight());

    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_lwsArray(ILwsArray **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    if ( style_ == StyleLWSA )
      return styleHelper_->QueryInterface(TEGGO_GUIDOF(ILwsArray),(void**)pVal);
    *pVal = 0;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_textview(ITextView **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    if ( style_ == StyleTextView )
      return styleHelper_->QueryInterface(TEGGO_GUIDOF(ITextView),(void**)pVal);
    *pVal = 0;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_primitive(ISpritePrimitive **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    if ( style_ == StyleShape || style_ == StyleTrilist )
      return styleHelper_->QueryInterface(TEGGO_GUIDOF(ISpritePrimitive),(void**)pVal);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_autodispose(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *pVal = noname_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_autodispose(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    noname_ = newVal!=VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_blendMode(tagBlendMode *newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::put_blendMode(tagBlendMode newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::get_symheight(long* ret)
  {
    if ( !sprite_->IsText() )
      return GenSCWideError(L"sprite klass is not font");
    *ret = sprite_->GetSymheight();
    return S_OK;
  }


HRESULT __stdcall SpriteTextView::put_height(float newVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetTextViewHeight(newVal);
    return S_OK;
  }

HRESULT __stdcall SpriteHelper::DispatchSubImpl(IDispatch *subimpl)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    return put_subimpl(
      +rcc_ptr<IUnknown>(
        subimpl
          ? ((IOnUpdate*) new Engine_DispSubImpl(subimpl,Engine_DispSubImpl::DISPATCH))
          : 0
          ));
  }

HRESULT SpriteHelper::InternalAdvise(IUnknown* dispifs,u32_t* cookie)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rcc_ptr<IUnknown> subimpl (
      (IOnUpdate*)(new Engine_DispSubImpl(
        (IDispatch*)dispifs,Engine_DispSubImpl::ADVISE)) );
    *cookie = (u32_t)+subimpl;
    return put_subimpl(+subimpl);
  }

HRESULT SpriteHelper::InternalUnadvise(u32_t cookie)
  {
    _SYNCHRONIZE_;
    if ( ((u32_t)+subimpl_) == cookie )
      put_subimpl_(0);
    return S_OK;
  }

HRESULT SpriteHelper::get_surface(ISurface **pVal)
  {
    __synchronize__
      {
        if ( !sprite_ )
          return ScraftError(NOT_EXISTS_INFO);
        *pVal = Refe(surface_);
      }
    return S_OK;
  }

HRESULT SpriteHelper::put_surface(ISurface *pSurf)
  {
    __synchronize__
      {
        if ( !sprite_ )
          return ScraftError(NOT_EXISTS_INFO);

        if ( pSurf )
          sprite_->ChangeKlass(pSurf->__klass_name__());
        else
          sprite_->ChangeKlass(L"$spritecraft$dummy$");

        surface_ = RccRefe(pSurf);

      }
    return S_OK;
  }

HRESULT SpriteHelper::get_paralax(long* val)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    *val = sprite_->GetParalax();
    return S_OK;
  }

HRESULT SpriteHelper::put_paralax(long val)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    sprite_->SetParalax(val);
    return S_OK;
  }

HRESULT SpriteHelper::__comfunc_SprBorder(void **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    rect_t rect = sprite_->GetRect();
    //*pVal = PyC_BuildFrect(rect.left,rect.top,rect.right.rect.bottom);
    return S_OK;
  }

HRESULT SpriteHelper::__comfunc_ScrBorder(void **pVal)
  {
    _SYNCHRONIZE_;
    if ( !sprite_ )
      return GenSCWideError(NOT_EXISTS_INFO);
    //*pVal = PyC_BuildPfrectTuple4(rect.left,rect.top,rect.right.rect.bottom);
    return E_FAIL;
  }

HRESULT __stdcall Sprite_RegisterPyType()
  {
    return SpriteHelper::RegisterPyType("Sprite");
  }
