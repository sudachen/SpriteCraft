
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
#pragma code_seg(".text$flat$e3")
#endif

//
//
//  Реализация спрайта
//
//

static u32_t Sprite3D_unique_no = 0;

rect_t Sprite3D::GetRect() const{
  const long x = x_-xhsp_;
  const long y = y_-yhsp_;
  const long w = x+Sprite3D::GetWidth()*xscale_;
  const long h = y+Sprite3D::GetHeight()*yscale_;
  return rect_t (x,y,w,h);
}

void Sprite3D::Rotate(float degree){ Sprite3D::SetAngle(Radians(degree)); }

void Sprite3D::SetAngle(float radians)
{
  if ( visible_ && EPSILON_TEST(radians,radians_) ) 
    SPRITES::SetNeedRefresh();
  radians_ = radians;
  cos_     = cosf(radians);
  sin_     = sinf(radians);
  rotated_ = !EPSILON_EQUAL0(radians);
}

SCERROR Sprite3D::Show()
{
  if ( !visible_ ) {
    SPRITES::SetNeedRefresh();
    visible_ = true;
    if ( klass_->info_.renderable_ ) klass_->mgr_->ShowSpr(this);
  }
  return SCE_OK;
}

SCERROR Sprite3D::Hide()
{
  if ( visible_ ) {
    SPRITES::SetNeedRefresh();
    visible_ = false;
    if ( klass_->info_.renderable_ ) klass_->mgr_->HideSpr(this);
  }
  return SCE_OK;
}

void Sprite3D::Move(float xX,float yY) { 
  if ( visible_ && ( EPSILON_TEST(xX,x_) || EPSILON_TEST(yY,y_) ) )
    SPRITES::SetNeedRefresh();
  x_ = xX; 
  y_ = yY;
}
void Sprite3D::MoveX(float xX) { 
  if ( visible_ && EPSILON_TEST(xX,x_) ) SPRITES::SetNeedRefresh();
  x_ = xX; 
}
void Sprite3D::MoveY(float yY) { 
  if ( visible_ && EPSILON_TEST(yY,y_) ) SPRITES::SetNeedRefresh();
  y_ = yY;
}

void Sprite3D::IncAngle(float val)
{
  SetAngle(radians_ + val);
}

void Sprite3D::IncX(float val)
{
  MoveY(y_ + val);
}

void Sprite3D::IncY(float val)
{
  MoveX(x_ + val);
}

float Sprite3D::GetDegee() const {return radians_*(180.0f/nv_pi);}
float Sprite3D::GetAngle() const {return radians_;}
float Sprite3D::GetX() const { return x_; }
float Sprite3D::GetY() const { return y_; }

float Sprite3D::GetTextWidth() const
{
  if ( !text_ ) return 0;
  Sprite3D_Text* ptex = ~text_;
  if ( ptex->width_ < 0 ) {
    if ( !klass_->font_ )  ret0_if_err ( klass_->mgr_->RestoreKlass_(*klass_) );
    if ( !ptex->text_.Length() ) ptex->width_ = 0;
    else {
      if ( text_multiline_ ) {
        pwide_t i  = ptex->text_.Str();
        pwide_t iE = i+ptex->text_.Length();
        ptex->width_ = 0;
        for(;;){
          pwide_t ii = teggo::linear_find(i,iE,L'\n');
          if ( i == ii ) break;
          ptex->width_ = 
            teggo_max<long>(
              klass_->font_->GetTextWidth(i,ii-i),ptex->width_);
          i = ii;
          if ( i == iE ) break;
          ++i;
        }
      }else
        ptex->width_ = klass_->font_->GetTextWidth(ptex->text_.Str());
    }
  }
  return ptex->width_;
}

float Sprite3D::GetWidth() const    { 
  if ( !klass_->info_.is_font ) return klass_->info_.spr_width; 
  else if ( Sprite3D::IsTextViewActive() )
    return Sprite3D::GetTextViewWidth();
  else 
    return GetTextWidth();
}

float Sprite3D::GetTextHeight() const 
{
  if ( !text_ ) return 0;
  Sprite3D_Text* ptex = ~text_;
  if ( ptex->height_<0 ) {
    if ( !klass_->font_ ) ret0_if_err ( klass_->mgr_->RestoreKlass_(*klass_) );
    if ( !ptex->text_.Length() ) ptex->height_ = 0;
    else {
      if ( text_multiline_ ) {
        ptex->height_ = klass_->font_->GetTextHeight()*
          (teggo::enumerate(ptex->text_.Begin(),ptex->text_.End(),'\n')+1);
      }else
        ptex->height_ = klass_->font_->GetTextHeight();
    }
  }
  return ptex->height_;
}

float Sprite3D::GetHeight() const   { 
  if ( !klass_->info_.is_font )  return klass_->info_.spr_height; 
  else if ( Sprite3D::IsTextViewActive() )
    return Sprite3D::GetTextViewHeight();
  else 
    return GetTextHeight();
}

pwide_t Sprite3D::GetKlass() const { return klass_->GetName(); }
float Sprite3D::GetTransparency() const { return transparency_*100.0f; }
void Sprite3D::SetTransparency(float percent) {
  float foo = teggo_min<float>(percent,100.0f)*0.01f;
  if ( visible_ && EPSILON_TEST(foo,transparency_) ) SPRITES::SetNeedRefresh();
  transparency_ = foo;
  RecalculateColor_();
}
float Sprite3D::GetLuminance() const { return luminance_*100.0f; }
void Sprite3D::SetLuminance(float percent) {
  float foo = teggo_min<float>(percent,100.0f)*0.01f;
  if ( visible_ && EPSILON_TEST(foo,luminance_) ) SPRITES::SetNeedRefresh();
  luminance_ = foo;
  RecalculateColor_();
}
float Sprite3D::GetScaleX() const { return xscale_*100.0f; }
float Sprite3D::GetScaleY() const { return yscale_*100.0f; }
void Sprite3D::Scale(float percent) { 
  float newVal = percent*0.01f;
  if ( visible_ 
       && ( EPSILON_TEST(xscale_,newVal) || EPSILON_TEST(yscale_,newVal) ) )
    SPRITES::SetNeedRefresh();
  xscale_ = yscale_ = newVal; 
  scaled_ = EPSILON_TEST(xscale_,1.0f)||EPSILON_TEST(yscale_,1.0f);
}
void Sprite3D::ScaleX(float percent) { 
  float newVal = percent*0.01f;
  if ( visible_ && EPSILON_TEST(newVal,xscale_) ) SPRITES::SetNeedRefresh();
  xscale_ = newVal; 
  scaled_ = EPSILON_TEST(xscale_,1.0f)||EPSILON_TEST(yscale_,1.0f);
}
void Sprite3D::ScaleY(float percent) { 
  float newVal = percent*0.01f;
  if ( visible_ && EPSILON_TEST(newVal,yscale_) ) SPRITES::SetNeedRefresh();
  yscale_ = newVal; 
  scaled_ = EPSILON_TEST(xscale_,1.0f)||EPSILON_TEST(yscale_,1.0f);
}
void Sprite3D::SetSizeX(float xsize) 
{
  float width = Sprite3D::GetWidth();
  if ( EPSILON_EQUAL0(width) ) return;
  if ( visible_ ) SPRITES::SetNeedRefresh();
  xscale_ =xsize/width;
  scaled_ = EPSILON_TEST(xscale_,1.0f)||EPSILON_TEST(yscale_,1.0f);
}
void Sprite3D::SetSizeY(float ysize)
{
  float height = Sprite3D::GetHeight();
  if ( EPSILON_EQUAL0(height) ) return;
  if ( visible_ ) SPRITES::SetNeedRefresh();
  yscale_ = ysize/height;
  scaled_ = EPSILON_TEST(xscale_,1.0f)||EPSILON_TEST(yscale_,1.0f);
}
void Sprite3D::Rescale(float w,float h)
{
  if ( visible_ ) SPRITES::SetNeedRefresh();
  float width  = Sprite3D::GetWidth();
  float height = Sprite3D::GetHeight();
  if ( !EPSILON_EQUAL0(width) ) xscale_ = w/width;
  if ( !EPSILON_EQUAL0(height) ) yscale_ = h/height;
  scaled_ = EPSILON_TEST(xscale_,1.0f)||EPSILON_TEST(yscale_,1.0f);
}
bool Sprite3D::IsVisible() const  {return visible_;}
pwide_t Sprite3D::GetName() const {return ::teggo::CollectionT<Sprite3D>::KeyOf(this);}
int Sprite3D::GetLayer() const {return layered_.layer_;}
int Sprite3D::GetSubLayer() const {return layered_.layer_cheat_;}
void  Sprite3D::SetFrameNo(int fr_no) {
  u32_t foo = fr_no % klass_->info_.spr_no;
  if ( foo != fr_no_ ) {
    if ( visible_ ) SPRITES::SetNeedRefresh();
    fr_no_ = foo;
  }
}

int Sprite3D::GetFrameNo() const {return fr_no_;}
void Sprite3D::Dispose() { 
  if ( visible_ ) SPRITES::SetNeedRefresh();
  REQUIRE ( klass_ && klass_->mgr_ );
  klass_->mgr_->DisposeSprite_(this);
}

void Sprite3D::SetSubImpl(SpriteSubImpl* x) { 
  if ( !!subimpl_ ) {
    subimpl_->SetSpritePtr(0);
    Release(subimpl_);
  }
  subimpl_ = AutoRef(Grab(x)); 
  complex_ = Adaptate<ComplexSprite>(subimpl_) != 0;
  if ( !!subimpl_ ) subimpl_->SetSpritePtr(this);
}

mrgb_t Sprite3D::GetRGB() const { return crgb_; }
void Sprite3D::SetRGB(mrgb_t rgb)
{
  if ( visible_ && crgb_.C() != rgb.C() ) SPRITES::SetNeedRefresh();
  crgb_ = rgb;
  RecalculateColor_();
}

void Sprite3D::SetFlipX(bool val)
{
  if ( flip_x_ != val ) {
    if ( visible_ )  SPRITES::SetNeedRefresh();
    flip_x_ = val;
  }
}

void Sprite3D::SetFlipY(bool val)
{
  if ( flip_y_ != val ) {
    if ( visible_ )  SPRITES::SetNeedRefresh();
    flip_y_ = val;
  }
}

bool Sprite3D::IsFlipX() const { return flip_x_; }
bool Sprite3D::IsFlipY() const { return flip_y_; }

SpriteSubImpl* Sprite3D::GetSubImpl() const { return Get(subimpl_); }

void Sprite3D::SetText(pwide_t text)
{
  if ( text && ( !text_ || !(text_->text_.Equal(text)) ) ) {
    if ( visible_ ) SPRITES::SetNeedRefresh();
    if ( !text_ ) text_ = AutoMem(new Sprite3D_Text);
    Sprite3D_Text* ptex = ~text_;
    ptex->text_ = text;
    text_multiline_ = ptex->text_.FindChar(L'\n') != -1;
    ptex->width_ = ptex->height_ = -1;
    update_text_ = true;
    ptex->Prepare(~klass_->font_);
    switch ( hsptag_ ) {
    case HOTSPOT_LEFT_TOP:      xhsp_ = 0; yhsp_ = 0; break;
    case HOTSPOT_CENTER_TOP:    xhsp_ = Sprite3D::GetWidth()/2; yhsp_ = 0; break;
    case HOTSPOT_RIGHT_TOP:     xhsp_ = Sprite3D::GetWidth(); yhsp_ = 0; break;
    case HOTSPOT_RIGHT_CENTER:  xhsp_ = Sprite3D::GetWidth(); yhsp_ = Sprite3D::GetHeight()/2; break;
    case HOTSPOT_RIGHT_BOTTOM:  xhsp_ = Sprite3D::GetWidth(); yhsp_ = Sprite3D::GetHeight(); break;
    case HOTSPOT_CENTER_BOTTOM: xhsp_ = Sprite3D::GetWidth()/2; yhsp_ = Sprite3D::GetHeight(); break;
    case HOTSPOT_LEFT_BOTTOM:   xhsp_ = 0; yhsp_ = Sprite3D::GetHeight(); break;
    case HOTSPOT_LEFT_CENTER:   xhsp_ = 0; yhsp_ = Sprite3D::GetHeight()/2; break;
    case HOTSPOT_CENTER:        xhsp_ = Sprite3D::GetWidth()/2; yhsp_ = Sprite3D::GetHeight()/2; break;
    case HOTSPOT_CUSTOM:        break;
    }
  }
}

pwide_t Sprite3D::GetText() const 
{ 
  return !!text_ ? text_->text_.Str() : L"";
}

void Sprite3D::SetHotspotX(float xhsp)
{
  if ( visible_ && EPSILON_TEST(xhsp,xhsp_) ) SPRITES::SetNeedRefresh();
  xhsp_ = xhsp;
}

void Sprite3D::SetHotspotY(float yhsp)
{
  if ( visible_ && EPSILON_TEST(yhsp_,yhsp) ) SPRITES::SetNeedRefresh();
  yhsp_ = yhsp;
}

void Sprite3D::SetHotspot(unsigned hsptag)
{
  if ( visible_ && hsptag_ != hsptag ) SPRITES::SetNeedRefresh();
  hsptag_ = hsptag;
  switch ( hsptag_ ) {
  case HOTSPOT_LEFT_TOP:      xhsp_ = 0; yhsp_ = 0; break;
  case HOTSPOT_CENTER_TOP:    xhsp_ = Sprite3D::GetWidth()/2; yhsp_ = 0; break;
  case HOTSPOT_RIGHT_TOP:     xhsp_ = Sprite3D::GetWidth(); yhsp_ = 0; break;
  case HOTSPOT_RIGHT_CENTER:  xhsp_ = Sprite3D::GetWidth(); yhsp_ = Sprite3D::GetHeight()/2; break;
  case HOTSPOT_RIGHT_BOTTOM:  xhsp_ = Sprite3D::GetWidth(); yhsp_ = Sprite3D::GetHeight(); break;
  case HOTSPOT_CENTER_BOTTOM: xhsp_ = Sprite3D::GetWidth()/2; yhsp_ = Sprite3D::GetHeight(); break;
  case HOTSPOT_LEFT_BOTTOM:   xhsp_ = 0; yhsp_ = Sprite3D::GetHeight(); break;
  case HOTSPOT_LEFT_CENTER:   xhsp_ = 0; yhsp_ = Sprite3D::GetHeight()/2; break;
  case HOTSPOT_CENTER:        xhsp_ = Sprite3D::GetWidth()/2; yhsp_ = Sprite3D::GetHeight()/2; break;
  case HOTSPOT_CUSTOM:        break;
  }
}

float  Sprite3D::GetHotspotX() { return xhsp_; }
float  Sprite3D::GetHotspotY() { return yhsp_; }

Sprite3D::Sprite3D() 
{
  layered_.layered_sprite_ = this;
  fr_no_        = 0;
  radians_      = 0;
  xscale_       = 1.0f;
  yscale_       = 1.0f;
  transparency_ = 0;
  luminance_    = 1.0f;
  crgb_         = mRGB_t(255,255,255);
  z_            = 100;
  layered_.unique_no_    = ++Sprite3D_unique_no;
  // layer, layer_chat уже инициализированны
  x_ = y_       = 0;
  parent_       = 0;
  paralax_      = 1;
  parent_deps_  = 
    SPRITE_DEPS_X|SPRITE_DEPS_Y|
    SPRITE_DEPS_XSCALE|SPRITE_DEPS_YSCALE|
    SPRITE_DEPS_DEGREE|
    SPRITE_DEPS_VISIBILITY|
    SPRITE_DEPS_DISPOSE;
    //SPRITE_DEPS_TRANSPARENCY|
    //SPRITE_DEPS_LUMINANCE;
  flip_x_       = false;
  flip_y_       = false;
  scaled_       = false;
  rotated_      = false;
  update_text_  = false;
  visible_      = false;
  complex_      = false;
  triangled_    = false;
  inter_paralaxed_ = false;
  //dummy_        = false;
  layerObj_     = 0;
  RecalculateColor_();
}

Sprite3D::~Sprite3D(){ 
  //XMessageW((L"~Sprite3D: %s",GetName()));
  Hide();  
  SetSubImpl(0); 
  SetParent(0);
}

Sprite* Sprite3D::GetParent() const { 
  return parent_;
}

void Sprite3D::SetParent(Sprite* parent) {
  if ( parent_ != parent ) {
    Sprite3D* spr3d = Adaptate<Sprite3D>(parent);
    if ( spr3d ) {
      {for( Sprite3D* p = spr3d; p!= 0 ; p= p->parent_ )
        if ( p == this ) return_because("found cycle sprite parent relation");}
      klass_->mgr_->ChangeParentOf(this,spr3d,0);
      //parent_ = spr3d;
    }else if ( parent_ != 0 ) {
      klass_->mgr_->ChangeParentOf(this,0,0);
      //parent_ = 0;
    }
    SPRITES::NeedRefresh();
  }
}

Sprite* Sprite3D::GetChild(pwide_t key)
{
  Sprite3D** val = children_.FindValueOfI(key);
  if ( val ) return *val;
  return 0;
}

void Sprite3D::AttachChild(Sprite* child, pwide_t key)
{
  Sprite3D* spr3d = Adaptate<Sprite3D>(child);
  klass_->mgr_->ChangeParentOf(spr3d,this,key);
  SPRITES::NeedRefresh();
}

u32_t Sprite3D::GetParentDeps() const {
  return parent_deps_;
}

void Sprite3D::SetParentDeps(u32_t deps) {
  if ( parent_deps_ != deps ) SPRITES::NeedRefresh();
  parent_deps_ = deps;
}

void Sprite3D::SetLayer(int layer) {
  if ( layer == layered_.layer_ ) return;
  layerObj_ = klass_->mgr_->RequestLayer(layer,false);
  if ( visible_ ) {
    Hide();
    layered_.layer_ = layer;
    Show(); // NeedRefresh
  }else{
    layered_.layer_ = layer;
  }
}

void Sprite3D::SetSubLayer(int sublayer) {
  if ( sublayer == layered_.layer_cheat_ ) return;
  if ( visible_ ) {
    Hide();
    layered_.layer_cheat_ = sublayer;
    Show(); // NeedRefresh
  }else{
    layered_.layer_cheat_ = sublayer;
  }
}

pchar_t const Sprite3D::TypeID__ = "#TYPEID#mskin::Sprite3D";
void* Sprite3D::Adaptate_(const void* id){
  if ( id == Sprite3D::TypeID__ ) return this;
  return Sprite::Adaptate_(id);
}

u32_t Sprite3D::GetFrCount() const
{
  return klass_->info_.spr_no;
}

void Sprite3D::GetFullIdent(long* layer,long* sublayer,u32_t* ident) const
{
  *layer = layered_.layer_;
  *sublayer = layered_.layer_cheat_;
  *ident = layered_.unique_no_;
}

SCERROR Sprite3D::ChangeKlass(pwide_t klass)
{
  Impl_3DSPRITES* mgr = klass_->mgr_;
  SCERROR ecode = mgr->ChangeSpriteKlassOn(this,klass);
  if ( ecode == SCE_OK ) {
    if ( klass_->info_.is_font && ~text_ ) text_->Prepare(klass_->font_);
  }
  //SPRITES::NeedRefresh();
  return ecode;
}

SPRITE_IMAGE_TYPE Sprite3D::GetImageType() const
{
  return klass_->info_.type;
}

bool Sprite3D::TryEnableComplex()
{
  if ( !complex_ ) {
    complex_ = Adaptate<ComplexSprite>(subimpl_) != 0;
    if ( complex_ ) triangled_ = false, triangled_vertices_.reassign__(0);
    if ( visible_ ) SPRITES::SetNeedRefresh();
  }
  return complex_;
}

void Sprite3D::DisableComplex()
{
  complex_ = false;
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

bool Sprite3D::IsComplex() const
{
  return complex_;
}

bool Sprite3D::TryEnableTriangled()
{
  if ( !triangled_ ) {
    triangled_ = Adaptate<TriangledSprite>(subimpl_) != 0;
    if ( triangled_ ) complex_ = false;
    if ( visible_ ) SPRITES::SetNeedRefresh();
  }
  return triangled_;
}

void Sprite3D::DisableTriangled()
{
  triangled_ = false, triangled_vertices_.reassign__(0);
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

bool Sprite3D::IsTriangled() const
{
  return triangled_;
}

void Sprite3D::SetTextViewWidth(float width)
{
  TextView* tview = AllocateTextView();
  tview->frect.right = tview->frect.left + fabs(width);
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

float Sprite3D::GetTextViewWidth() const
{
  return ~textview_?textview_->frect.Width():0;
}

void Sprite3D::SetTextViewHeight(float height)
{
  TextView* tview = AllocateTextView();
  tview->frect.bottom = tview->frect.top + fabs(height);
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

float Sprite3D::GetTextViewHeight() const
{
  return ~textview_?textview_->frect.Height():0;
}

void Sprite3D::SetTextViewBackground(mrgb_t color)
{
  TextView* tview = AllocateTextView();
  tview->background = color;
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

mrgb_t Sprite3D::GetTextViewBackground() const
{
  return ~textview_?textview_->background:mRGB_t(0,0,0);
}

void Sprite3D::SetTextViewBorder(long border)
{
  TextView* tview = AllocateTextView();
  tview->border = border;
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

long Sprite3D::GetTextViewBorder() const
{
  return ~textview_?textview_->border:0;
}

void Sprite3D::SetTextViewBorderColor(mrgb_t color)
{
  TextView* tview = AllocateTextView();
  tview->bordcolor = color;
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

mrgb_t Sprite3D::GetTextViewBorderColor() const
{
  return ~textview_?textview_->bordcolor:mRGB_t(0,0,0);
}

void Sprite3D::SetTextViewScrollX(float xOffset)
{
  TextView* tview = AllocateTextView();
  tview->frect.MoveSelf(xOffset,tview->frect.top);
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

float Sprite3D::GetTextViewScrollX() const
{
  return ~textview_?textview_->frect.left:0;
}

void Sprite3D::SetTextViewScrollY(float yOffset)
{
  TextView* tview = AllocateTextView();
  tview->frect.MoveSelf(tview->frect.left,yOffset);
  if ( visible_ ) SPRITES::SetNeedRefresh();
}

float Sprite3D::GetTextViewScrollY() const
{
  return ~textview_?textview_->frect.top:0;
}

void Sprite3D::SetTextViewActive(bool active)
{
  TextView* tview = AllocateTextView();
  if ( visible_ && tview->active != active ) SPRITES::SetNeedRefresh();
  tview->active = active;
}

bool Sprite3D::IsTextViewActive() const
{
  return ~textview_?textview_->active:false;
}

void Sprite3D::SetTextViewColorized(bool colorized)
{
  TextView* tview = AllocateTextView();
  if ( visible_ && tview->colorized != colorized ) SPRITES::SetNeedRefresh();
  tview->colorized = colorized;
}

bool Sprite3D::IsTextViewColorized() const
{
  return ~textview_?textview_->colorized:false;
}

float Sprite3D::GetTextViewMaxScrollX() const
{
  return GetTextWidth();
}

float Sprite3D::GetTextViewMaxScrollY() const
{
  return GetTextHeight();
}

long Sprite3D::GetSymheight() const
{
  if ( !klass_->font_ ) ret0_if_err ( klass_->mgr_->RestoreKlass_(*klass_) );
  return klass_->font_->GetTextHeight();
}

bool Sprite3D::IsText() const
{
  return klass_->info_.is_font;
}

void Sprite3D::SetParalax( long par )
{
  if ( par > 0 ) 
  {
    if ( par >= 100 ) paralax_ = 0;
    else paralax_ = float(100-par)/100.0f;
  }
  else if ( par < -0 )
  {
    if ( par < -100 ) par = 2.0f;
    else paralax_ = 1.0f + float(100+par)/100.0f;
  }
  else
    paralax_ =  1.0f;
}

long Sprite3D::GetParalax() const
{
  if ( paralax_ > 0.9998 && paralax_ < 1.0001 ) 
    return 0;
  if ( paralax_ < 1 ) 
    return 100-long(paralax_ * 100);
  else
    return -long((paralax_-1.f) * 100);
}
