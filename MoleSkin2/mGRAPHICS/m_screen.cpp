
/*

Copyright © 2003-2013, Alexéy Sudachén, alexey@sudachen.name

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
#include "../mstreams/m_istream.h"
#include "m_screen.h"
#include "m_font.h"
#include "m_texfont.h"

#include "../mGRAPHICS/m_color.h"
using namespace mskin;

#include "../../SpriteCraft.SRC/flat_e.h"
//#include ".h"
//#include "../msound/m_sound.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

GfxDevice*    SCREEN::gfx_ = 0;
Gfx3DDevice*  SCREEN::gfx_3d_ = 0;
GfxControl*   SCREEN::gfx_control_ = 0;
SCREEN_Inst*  SCREEN::instance_ = 0;
SCREEN_Font*  SCREEN::font_ = 0;

float         SCREEN::offsetx_ = 0;
float         SCREEN::offsety_ = 0;

static const char SCREEN_IS_NOT_INITIALIZED[] = "screen is not initialized";
static const char SCREEN_IS_NOT_INITIALIZED_OR_3D_UNSUPPORTED[] = "screen is not initialized or 3D unsupported";


struct SCREEN_Font {
  virtual SCERROR Print(int x, int y, pwide_t text) = 0;
  virtual int TextWidth(pwide_t text) = 0;
  virtual int TextHeight() = 0;
  SCREEN_Font() {}
  virtual ~SCREEN_Font() {}
};

struct SCREEN_TFont: public SCREEN_Font {
  AutoTexFont font_;
  BufferT<TexFont::SYMBOL> symbols_;
  virtual SCERROR Print(int x, int y, pwide_t text);
  virtual int TextWidth(pwide_t text) 
  { ret0_if(!font_); return font_->GetTextWidth(text); }
  virtual int TextHeight()
  { ret0_if(!font_); return font_->GetTextHeight(); }
  SCREEN_TFont();
  virtual ~SCREEN_TFont() {}
};

/*struct SCREEN_SFont: public SCREEN_Font {
  AutoSurfFont font_;
  virtual SCERROR Print(int x, int y, pchar_t text) {
    if ( Surface* fb = SCREEN::GetFrameBuffer() ) 
      return font_->Print(fb,x,y,text);
    else return SCE_FAIL;
  }
  virtual int TextWidth(pchar_t text) 
  { ret0_if(!font_); return font_->GetTextWidth(text); }
  virtual int TextHeight()
  { ret0_if(!font_); return font_->GetTextHeight(); }
  SCREEN_SFont();
  virtual ~SCREEN_SFont() {}
};*/

struct SCREEN_Inst : public IRefObject, private refbase {
public:
  virtual void Release_() const { refbase::Release_(); }
  virtual void Grab_() const    { refbase::Grab_(); }
  SCREEN_Inst() 
  {
    REQUIRE ( SCREEN::instance_ == 0 && SCREEN::gfx_ == 0 );
    SCREEN::instance_ = this;
  }
  virtual ~SCREEN_Inst() {
    REQUIRE ( SCREEN::instance_ == this );
    SCREEN::SetImpl(0);
    SCREEN::instance_ = 0;
  }
};


SCERROR SCREEN::GetWindowHandle(void* handle,u32_t sizeof_handle)
{
  if ( gfx_control_ ) 
    return gfx_control_->GetWindowHandle(handle,sizeof_handle);
  else
    fail_because(SCREEN_IS_NOT_INITIALIZED);
}

bool SCREEN::IsActive() { 
  if ( gfx_ ) return gfx_->IsActive();
  retval_because(SCREEN_IS_NOT_INITIALIZED,false);
}

bool SCREEN::IsRubbish() { 
  if ( gfx_ ) return gfx_->IsRubbish();
  retval_because(SCREEN_IS_NOT_INITIALIZED,false);
}

bool SCREEN::IsFullscreen() { 
  if ( gfx_ ) {
    GFX_VALUE mode = GFX_UNKNOWN;
    gfx_->GetMode(&mode);
    return mode == GFX_FULLSCREEN;
  }
  retval_because(SCREEN_IS_NOT_INITIALIZED,false);
}

void SCREEN::MakeRubbish() { 
  if ( gfx_ ) gfx_->MakeRubbish();
  else return_because(SCREEN_IS_NOT_INITIALIZED);
}

EVENT SCREEN::NextEvent(bool processMsgQue) { 
  if ( gfx_ ) return gfx_->NextEvent(processMsgQue);
  retval_because(SCREEN_IS_NOT_INITIALIZED,EVENT::MakeQuitEvent());
}

Surface* SCREEN::GetFrameBuffer() {
  if ( gfx_ ) return gfx_->FrameBuffer(); 
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

SCERROR SCREEN::BeginDraw(unsigned flags) {
  if ( gfx_ ) return gfx_->BeginDraw(flags); 
  retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
}

SCERROR SCREEN::EndDraw(unsigned flags) {
  if ( gfx_ ) return gfx_->EndDraw(flags); 
  retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
}

int SCREEN::GetWidth() {
  if ( gfx_ ) return gfx_->GetWidth(); 
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

int SCREEN::GetHeight() {
  if ( gfx_ ) return gfx_->GetHeight(); 
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

int SCREEN::WndWidth() {
  long val;
  if ( gfx_control_ ) return gfx_control_->GetWindowSize(&val,0), val; 
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

int SCREEN::WndHeight() {
  long val;
  if ( gfx_control_ ) return gfx_control_->GetWindowSize(0,&val), val; 
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

GfxControl* SCREEN::GetControl() {
  if ( gfx_ ) return gfx_control_;
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

u32_t SCREEN::GetCaps() {
  if ( gfx_ ) return gfx_->GetCaps();
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

Gfx3DDevice* SCREEN::Get3DDevice() {
  if ( gfx_ ) return gfx_3d_;
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

GfxDevice* SCREEN::GetGfxDevice() {
  if ( gfx_ ) return gfx_;
  retval_because(SCREEN_IS_NOT_INITIALIZED,0);
}

SCERROR SCREEN::SetMode(GFX_VALUE wndmode,GFX_VALUE mode,GFX_VALUE bpp,GFX_VALUE refrate,u32_t flags){
  if ( gfx_ ) return gfx_->SetMode(wndmode,mode,bpp,refrate,flags);
  retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
}

SCERROR SCREEN::GetMode(GFX_VALUE* wndmode,GFX_VALUE* mode,GFX_VALUE* bpp,GFX_VALUE* refrate,u32_t* flags){
  if ( gfx_ ) return gfx_->GetMode(wndmode,mode,bpp,refrate,flags);
  retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
}

void SCREEN::SetImpl(GfxDevice* gfx) 
{ 
  if ( !instance_ ) return_because(SCREEN_IS_NOT_INITIALIZED);
  delete font_; font_ = 0;
  Release(gfx_);
  gfx_         = gfx;
  gfx_3d_      = Adaptate<Gfx3DDevice>(gfx);
  gfx_control_ = Adaptate<GfxControl>(gfx);
}

Surface* SCREEN::CreateDirtySurface(int w,int h,unsigned fmt)
{
  if ( gfx_ ) return gfx_->CreateDirtySurface(w,h,fmt);
  return GenericSurface::Create(w,h,fmt);
}

Texture* SCREEN::CreateDirtyTexture(int width,int height,unsigned flags)
{
  if ( gfx_3d_ ) return gfx_3d_->CreateDirtyTexture(width,height,flags);
  retval_because(SCREEN_IS_NOT_INITIALIZED_OR_3D_UNSUPPORTED,0);
}

VxB* SCREEN::CreateDirtyVxB(int count,unsigned flags)
{
  if ( gfx_3d_ ) return gfx_3d_->CreateDirtyVxB(count,flags);
  retval_because(SCREEN_IS_NOT_INITIALIZED_OR_3D_UNSUPPORTED,0);
}

void SCREEN::SetFillColor(mrgb_t color)
{
  if ( gfx_3d_ ) gfx_3d_->SetFillerColor(fRGBA_t(color));
  else return_because(SCREEN_IS_NOT_INITIALIZED_OR_3D_UNSUPPORTED);
}

IRefObject*  SCREEN::Acquire(GFX_DEVICE devtype,GFX_VALUE winmod,GFX_VALUE vmode,GFX_VALUE bpp) 
{
  if ( !instance_ ) {
    new SCREEN_Inst(); 
    GfxDevice* gfx = CreateDynamicDevice(devtype,winmod,vmode,bpp);
    if ( !gfx ) {
      instance_->Release_();
      return 0;
    }else SetImpl(gfx);
  } else Grab ( instance_ );
  return instance_;
}

IRefObject*  SCREEN::Acquire(GfxDevice* dev) 
{
  if ( !instance_ ) new SCREEN_Inst(); else Grab ( instance_ );
  SetImpl(dev);
  return instance_;
}

SCERROR SCREEN::RestoreContext()
{
  SCERROR res = SCE_OK;
  //delete font_; font_ = 0;
  //XMessage(("(*)RESTORE TEXTURES"));
  if ( TextureSet::Restore() != SCE_OK ) res = SCE_FAIL;
  //XMessage(("(*)RESTORE FONTS"));
  if ( TexFontSet::Restore() != SCE_OK ) res = SCE_FAIL;
  //XMessage(("(*)RESTORE SPRITES"));
  if ( SPRITES::Restore()    != SCE_OK ) res = SCE_FAIL;
  //XMessage(("(*)RESTORE SOUNDS"));
//  if ( SOUNDS::Restore()     != SCE_OK ) res = SCE_FAIL;
//  XMessage(("(*)CONTEXT RESTORE %s",res==SCE_OK?"SUCCESS":"FAIL"));
  return res;
}

SCERROR  SCREEN::Clear()
{
  if ( !gfx_ ) retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
  if ( gfx_3d_ ) return gfx_3d_->Clear();
  else return gfx_->FrameBuffer()->Clear(0);
}

SCERROR  SCREEN::ClearDepth()
{
  if ( !gfx_ ) retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
  if ( gfx_3d_ ) return gfx_3d_->Clear(GFX3D_CLEAR_ZBUFFER);
  else return SCE_UNSUPPORTED;//return gfx_->FrameBuffer()->Clear(0);
}

SCERROR SCREEN::Print(int x,int y,pwide_t text)
{
  if ( instance_ ) 
    return GetFont_()->Print(x,y,text);
  retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
}

int SCREEN::TextWidth(pwide_t text)
{
  if ( instance_ ) 
    return GetFont_()->TextWidth(text);
  retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
}

int SCREEN::TextHeight()
{
  if ( instance_ ) 
    return GetFont_()->TextHeight();
  retval_because(SCREEN_IS_NOT_INITIALIZED,SCE_FAIL);
}

SCREEN_Font* SCREEN::GetFont_()
{
  if ( !font_ ) {
    if ( gfx_3d_ ) font_ = new SCREEN_TFont;
    else REQUIRE(!"surface font is unimplemented");
    //else font_ = new SCREEN_SFont;
  }
  return font_;
}

// SCREEN_TFont 3D impl

SCERROR SCREEN_TFont::Print(int x, int y, pwide_t text)
{
  Gfx3DDevice* gfx = SCREEN::Get3DDevice();
  if ( !gfx ) fail_because(SCREEN_IS_NOT_INITIALIZED_OR_3D_UNSUPPORTED);
  gfx->Ortho2D();
  //gfx->Translate(x,SCREEN::GetHeight()-font_->GetTextHeight()-y);
  gfx->SetState(GFX3D_STATE_MINMAGFILTER,GFX3D_NEAREST);
  gfx->SetState(GFX3D_STATE_ENVMODE,GFX3D_TEXTURE_MODULATE);
  gfx->SetState(GFX3D_STATE_BLENDING,GFX3D_DISABLE);
  gfx->SetState(GFX3D_STATE_ALPHA_TEST,GFX3D_DISABLE);
  gfx->SetState(GFX3D_STATE_DEPTH_FUNC,GFX3D_ALWAYS);
//  u16_t indices[] = {0,1,2,2,3,0};
  u16_t indices[] = {0,1,2,2,3,0};
  mrgba_t color = mRGBA_t(255,255,255,255);
  font_->FillSymbols(symbols_,text);
  float screen_height = (float)SCREEN::WndHeight();
  for ( unsigned i = 0; i < symbols_.Count(); ++i ) {
    gfx->SetTexture(font_->GetSymbolTexture(symbols_[i].symbol));
    VxB_VERTEX vertices[4];
    font_->FillQuad(vertices,symbols_[i].symbol,symbols_[i].x + x,screen_height - symbols_[i].y - y);
    vertices[0].color = color;
    vertices[1].color = color;
    vertices[2].color = color;
    vertices[3].color = color;
    gfx->DrawIndexedPrimitive(GFX3D_TRILIST,vertices,4,indices,6,GFX3D_VxB_VCT);
  }
  return SCE_OK;
}

extern "C" const byte_t BGZ_arial14a[];
extern "C" const byte_t TXT_arial14a[];
extern "C" const unsigned long BGZ_arial14a_size;
extern "C" const unsigned long TXT_arial14a_size;

SCREEN_TFont::SCREEN_TFont()
{
  font_ = AutoRef ( TexFontSet::Acquire(L"@arial14a.bmp.gz",0) );
  if ( !font_ ) 
    Lout << "screen: system font is not loaded, fps/stats and SCREEN::Print is disabled";
}

// SCREEN_SFont 2D impl
//SCREEN_SFont::SCREEN_SFont()
//{
//  font_ = AutoRef ( CreateSurfFontFrom("@arial14a.bmp.gz") );
//}

NAMESPACE_MSKIN_END
