
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
#include "m_graphics.h"
#include "../mutil/m_roundpow2.h"
#include "../mcore/m_vsnprintf.h"

#include "m_glyphsource.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

static const float MAX_TEXTURE_SIZE = 256;

struct SYMDEF 
  {
    float x,y,w,h;
  };

pchar_t const TexFont::TypeID__ = "#TYPEID#mskin::TexFont";

void* TexFont::Adaptate_(const void* id)
  {
    if ( id == TypeID__ ) return this;
    return IRefObject::Adaptate_(id);
  }

struct GenericTexFont2 : public TexFont, private refbase 
{
  struct SYMBOLRECORD
    {
      wchar_t       widechar;
      Texture*      texture;
      float         width;
      float         tx,ty,tw,th;
      float         wcf,hcf;
    };
  
  struct SYMBOLALIAS
    {
      wchar_t widechar;
      u32_t ident;
      bool operator < ( const SYMBOLALIAS& a ) const 
        { 
          return widechar < a.widechar; 
        }
    };
  
  float                 font_height_;   // высота строки символов
  float                 width_cf_;      // коэфициэнт для ширины символа
  AutoGlyphSource       glyph_source_;  // описание глифов шрифта
  SlistT<AutoTexture>   tex_;           // список текстур содержащих символы
  // описания символов шрифта, первый элемент - пустой символ
  BufferT<SYMBOLRECORD>  symdef_;
  // отсортированная по уникод-кодам карта символов, для двоичного поиска
  BufferT<SYMBOLALIAS>   symbols_;
  
  //оптимизация для латинских символов
  //enum { DIRECTMAP_MIN = 32, DIRECTMAP_MAX = 128 };
  //u32_t               directmap_[DIRECTMAP_MAX-DIRECTMAP_MIN];
  
  virtual void      Release_() const { refbase::Release_(); }
  virtual void      Grab_() const { refbase::Grab_(); }
  virtual int       GetTextHeight() const;
  virtual int       GetTextWidth(pwide_t,int count);
  virtual Texture*  GetSymbolTexture(u32_t symbol);
  virtual void      FillQuad(VxB_VERTEX* vx,u32_t symbol,float x, float y,const frect_t* clip);
  virtual void      FillSymbols(BufferT<TexFont::SYMBOL> &syms,pwide_t text,int len);

  GenericTexFont2();
  virtual ~GenericTexFont2();

  SCERROR       Init(GlyphSource*);
  SCERROR       Restore();

  SYMBOLRECORD*     GetRecordOf(wchar_t widechar);
  u32_t           GetSymbolOf(wchar_t widechar);
};

enum { MAX_SYMBOLS = 128 };

u32_t GenericTexFont2::GetSymbolOf(wchar_t widechar)
  {
    SYMBOLALIAS a;
    a.widechar = widechar;
    if ( SYMBOLALIAS *i = symbols_.Bfind(a) )
      if ( i->widechar == widechar ) 
        return i->ident;
    return 0;
  }

GenericTexFont2::SYMBOLRECORD* GenericTexFont2::GetRecordOf(wchar_t widechar)
  {
    if ( u32_t si = GetSymbolOf(widechar) )
      return &symdef_[si];
    else
      return 0;
  }

int GenericTexFont2::GetTextHeight() const
  {
    return int(font_height_);
  }

int GenericTexFont2::GetTextWidth(pwide_t text,int count)
  {
    unsigned width = 0;
    
    if ( count < 0 ) 
      count = wcslen(text);
    
    for ( int i = 0; i < count; ++i ) 
    {
      SYMBOLRECORD* sr = GetRecordOf(text[i]);
      if ( sr ) 
        width += int(sr->width * width_cf_);
    }
    
    return width;
  }

Texture* GenericTexFont2::GetSymbolTexture(u32_t symbol)
  {
    if ( symbol < symdef_.Count() )
      return symdef_[symbol].texture;
    return 0;
  }

// в системе координат Ortho3D, y направлен с низу в верх, заданы координаты левого верхнего угла символа.
void GenericTexFont2::FillQuad(VxB_VERTEX* vx,u32_t symbol,float x, float y,const frect_t* clip)
  {
    DBG_REQUIRE( symbol < symdef_.Count() );

    SYMBOLRECORD* sr = &symdef_[symbol];
    float width = sr->width * width_cf_;
    memset(vx,0,sizeof(VxB_VERTEX)*4);
    bool needclipping  = false;
    float mrgleft=0,mrgright=0,mrgtop=0,mrgbottom=0;

    if ( clip ) 
      {
        if ( x < clip->left )        
          needclipping = true, mrgleft = clip->left - x;

        if ( x+width > clip->right ) 
          needclipping = true, mrgright = x + width - clip->right;

        if ( y > -clip->top )         
          needclipping = true, mrgtop = y + clip->top;

        if ( y-font_height_ < -clip->bottom ) 
          needclipping = true, mrgbottom = -clip->bottom - (y-font_height_);
      }
    if ( needclipping )
      {
        const float txleft    = mrgleft/MAX_TEXTURE_SIZE;
        const float txright   = mrgright/MAX_TEXTURE_SIZE;
        const float txtop     = mrgtop/MAX_TEXTURE_SIZE;
        const float txbottom  = mrgbottom/MAX_TEXTURE_SIZE;
        vx[0].Xyz(x+mrgleft,y-mrgtop);
        vx[0].Tex(sr->tx+txleft,sr->ty+txtop);
        vx[1].Xyz(x+mrgleft,y-font_height_+mrgbottom);
        vx[1].Tex(sr->tx+txleft,sr->th-txbottom);
        vx[2].Xyz(x+width-mrgright,y-font_height_+mrgbottom);
        vx[2].Tex(sr->tw-txright,sr->th-txbottom);
        vx[3].Xyz(x+width-mrgright,y-mrgtop);
        vx[3].Tex(sr->tw-txright,sr->ty+txtop);
      }
    else
      {
        vx[0].Xyz(x,y);
        vx[0].Tex(sr->tx,sr->ty);
        vx[1].Xyz(x,y-font_height_);
        vx[1].Tex(sr->tx,sr->th);
        vx[2].Xyz(x+width,y-font_height_);
        vx[2].Tex(sr->tw,sr->th);
        vx[3].Xyz(x+width,y);
        vx[3].Tex(sr->tw,sr->ty);
      }
  }

// x,y  в системе координат текста, y направлен сверху вниз, начало отсчета в левом верхнем углу.
void GenericTexFont2::FillSymbols(BufferT<TexFont::SYMBOL>& syms,pwide_t text,int len)
  {
    if ( len <0 ) 
      len = wcslen(text);
    syms.Resize(0);
    syms.Reserve(len);
    float px = 0;
    float py = 0;
    float fontheight = font_height_; //glyph_source_->GetCharHeight();
    for ( int i = 0; i < len; ++i )
    {
      if ( text[i] == '\n' ) py += fontheight, px = 0;
      u32_t si = GetSymbolOf(text[i]);
      if ( !si ) continue;
      TexFont::SYMBOL foo;
      foo.symbol = si;
      foo.x = px;
      foo.y = py;
      foo.width = symdef_[si].width * width_cf_;
      syms.Push(foo);
      px += foo.width;
    }
  }

SCERROR GenericTexFont2::Init(GlyphSource* gs)
  {
    glyph_source_ =  AutoRef(Grab(gs));
    fail_if( !glyph_source_ );
    fail_if_fail( glyph_source_->Ready() );
    font_height_ = glyph_source_->GetCharHeight();
    width_cf_ = 1;
    return  Restore();
  }

SCERROR GenericTexFont2::Restore()
  {
    fail_if_fail( glyph_source_->Ready() );

    // ресторим текстуры
    for ( teggo::SlistT<AutoTexture>::Position i = tex_.Begin(); i ; ++i )
      {
        if ( (*i)->Restore() != SCE_OK ) 
          (*i).reassign__(
            SCREEN::CreateDirtyTexture(
              MAX_TEXTURE_SIZE,MAX_TEXTURE_SIZE,GFX3D_TEX_RGBA8));
        fail_if ( !*i );
      }

    // перестраиваем лейоут
    if ( !symdef_.Count() ) 
      {
        REQUIRE ( tex_.Count() == 0 );
        float symheight = glyph_source_->GetCharHeight();
        if ( symheight > MAX_TEXTURE_SIZE-2 ) 
          fail_because ( 
            _S*"texfont: font symbols height must be less then %.0f"
              %(MAX_TEXTURE_SIZE-2) );
        long count = glyph_source_->GetCharCount();
        SYMBOLRECORD symrec = {0};
        symdef_.Resize(count+1,symrec);
        symbols_.Resize(count);
        BufferT<GlyphSource::CHARDEF> symwidth;
        fail_if_fail( glyph_source_->GetChars(symwidth) );
        fail_if ( symwidth.Count() != count );
        float px = 0;
        float py = 0;
        for ( int i = 0; i < count ; ++i ) 
        {
          SYMBOLRECORD& sr = symdef_[i+1];
          sr.width = symwidth[i].width;
          if ( sr.width > MAX_TEXTURE_SIZE-2 ) 
            fail_because ( 
              _S*"texfont: font symbols width must be less then %.0f"
                %(MAX_TEXTURE_SIZE-2) );
          sr.widechar = symwidth[i].widechar;
          if ( px + sr.width > MAX_TEXTURE_SIZE ) py += symheight + 2,px = 0;
          if ( py + symheight + 2 >= MAX_TEXTURE_SIZE || tex_.Empty() ) 
            py=0, px=0, 
            tex_.Append(AutoRef(
              SCREEN::CreateDirtyTexture(MAX_TEXTURE_SIZE,MAX_TEXTURE_SIZE,GFX3D_TEX_RGBA8)));
          sr.tx = (px+1)/MAX_TEXTURE_SIZE;
          sr.ty = (py+1)/MAX_TEXTURE_SIZE;
          sr.tw = (px+sr.width+1)/MAX_TEXTURE_SIZE;
          sr.th = (py+symheight+1)/MAX_TEXTURE_SIZE;
          //sr.wcf = (sr.tw-sr.tx)/sr.width;
          //sr.hcf = (sr.th-sr.ty)/symheight;
          px += sr.width + 2;
          sr.texture = tex_.Last();
          symbols_[i].ident = i+1;
          symbols_[i].widechar = sr.widechar;
        }
        Qsort(symbols_.Begin(),symbols_.End());
      }
    // перезагружаем текстуры
    {
      long count = symdef_.Count()-1;
      for ( int i = 0; i < count ; ++i ) 
        {
          SYMBOLRECORD& sr = symdef_[i+1];
          rect_t   rect;
          Surface* surf = glyph_source_->GetCharSurface(i,rect);
          fail_if ( !surf );
          sr.texture->LoadSubpicture(surf,sr.tx*MAX_TEXTURE_SIZE,sr.ty*MAX_TEXTURE_SIZE,rect);
        }
    }
    glyph_source_->Relax();
    return SCE_OK;
  }

GenericTexFont2::GenericTexFont2() 
  {
  }

GenericTexFont2::~GenericTexFont2() 
  {
  }

struct ProxyTexFont_OfTexFontSet : public GenericTexFont2
  {
    AutoTexFontSet tfset_;
    widestring font_name_;
    ProxyTexFont_OfTexFontSet() 
      {
      }

    virtual ~ProxyTexFont_OfTexFontSet() 
      { 
        tfset_->UnAcquire(this); 
      }

    const pwide_t GetName() 
      { 
        return +font_name_; 
      }
  };

TexFontSet::TexFontSet() 
  {
    REQUIRE (instance_ == 0);
    instance_ = this;
  }

TexFontSet::~TexFontSet() 
  {
    REQUIRE(instance_ == this);
    instance_ = 0;
  }

TexFontSet* TexFontSet::instance_ = 0;

TexFontSet* TexFontSet::AcquireSet() 
  { 
    if ( !instance_ ) 
      return new TexFontSet();
    return Grab(instance_);
  }

TexFont* TexFontSet::Acquire_(pwide_t xname,pwide_t symname,GlyphSource::SrcType type,mrgb_t ckey)
  {
    StringT<wchar_t,64> name(xname);
    if ( name.Length() > 3 && wcsicmp(name.Str()+name.Length()-3,L".gz") == 0 ) name.Resize(name.Length()-3);
    Xdbg( Ldbg << _S*"texfontset: acquire font '%?'" %name );
    StringT<wchar_t,64> name2(_S*L"{%?}-%x-%x" %name %type %ckey.C());  
    ProxyTexFont_OfTexFontSet** font = fonts_.Get(+name2);

    if ( font ) 
      {
        Xdbg( Ldbg << _S*"texfontset: using font '%?'" %name );
        return Grab(*font);
      } 
    else 
      {
        Xdbg( Ldbg << _S*"texfontset: creating font '%?'" %name );
        auto_ref<ProxyTexFont_OfTexFontSet> pf ( new ProxyTexFont_OfTexFontSet() );
        pf->font_name_ = name2;
        auto_ref<GlyphSource> gs ( CreateGlyphSourceFrom(+name,symname,type,ckey) );
        ret0_if_fail(pf->Init(~gs));
        Xdbg( Ldbg << _S*"texfontset: acquire font '%?', %d textures (%dx%d) used"
          %name.Str()
          %pf->tex_.Count()
          %long(MAX_TEXTURE_SIZE) %long(MAX_TEXTURE_SIZE) );
        pf->tfset_ = AutoRef(Grab(this));
        fonts_.Put(+name2,Get(pf));
        return Forget(pf);
      }
  }

void TexFontSet::UnAcquire(ProxyTexFont_OfTexFontSet* fnt)
  { 
    fonts_.Erase(fnt->GetName());
  }

SCERROR TexFontSet::Restore()
  {
    if ( !instance_ ) return SCE_OK;
    SCERROR result = SCE_OK;
    FONTMAP::Iterator i = instance_->fonts_.Iterate();
    while ( i.Next() ) 
      {
        Xdbg( Ldbg << _S*"texfontset: restore '%?'" %FONTMAP::KeyOf(&*i) );
        if ( (*i)->Restore() != SCE_OK ) 
          result = SCE_FAIL;
      }
    return result;
  }

NAMESPACE_MSKIN_END
