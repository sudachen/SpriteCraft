
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
#include "m_glyphsource.h"
#include "m_loadsurf.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

static bool GetSymbols(pwide_t symbols,pwide_t sourceName,widestring& ws)
{
  if ( !symbols ) {
    if ( !sourceName ) ret0_because("source unknown");
    BufferT<byte_t> symmap;
    widestring tex_name = sourceName;
    int i0 = tex_name.FindRChar('/');
    int i1 = tex_name.FindChar('.',teggo_max<int>(i0,0));
    if ( i1 > i0 ) tex_name.ReplaceRight(i1,L".txt");
    else tex_name.Append(L".txt");
    AutoDataSource ds ( RSCMAN::CreateSource(+tex_name) );
    ret0_if ( !ds || SCE_OK != ds->ReadBuffer(symmap) ); 
    symmap.Push(0);
    symmap.Push(0);
    if ( symmap[0] == 0xff && symmap[1] == 0xfe ) // UNICODE
    {
      ws.Resize(symmap.Count()/2);
      for ( int i = 0, iE = symmap.Count()/2; i != iE; ++i )
        ws[i] = ReadShort_(&symmap[i*2]);
    }
    else if ( symmap[0] == 0xfe && symmap[1] == 0xff ) // UNICODE
    {
      ws.Resize(symmap.Count()/2);
      for ( int i = 0, iE = symmap.Count()/2; i != iE; ++i )
        ws[i] = ReadNwShort_(&symmap[i*2]);
    }
    else if ( symmap[0] == 0xef && symmap[1] == 0xbb ) // UTF8
      ret0_because( _S*"glyphsource: font '%?' symbols description, utf-8 unsupported" %sourceName );
    else
      ws = (char*)&symmap[0];
  }
  else
    ws = symbols;
  return true;
}

struct GlyphSourceMarkedBMP : public GlyphSource, private refbase
{
  widestring         source_;
  widestring         symbols_;
  AutoSurface        surface_;
  float              char_height_;
  BufferT<rect_t>    symdefs_;
  SrcType            type_;
  mrgb_t             ckey_;

  virtual SCERROR Init(pwide_t source,pwide_t symbols,GlyphSource::SrcType type = GlyphSource::SRCBLEND,mrgb_t ckey=mRGB_t(0,0,0));
  virtual void        Relax();
  virtual SCERROR Ready();
  virtual long        GetCharCount();
  virtual float       GetCharHeight();
  virtual SCERROR GetChars(BufferT<CHARDEF>& width);
  virtual Surface*    GetCharSurface(long ident,rect_t&);

  virtual void Release_() const { refbase::Release_(); }
  virtual void Grab_() const { refbase::Grab_(); }

  GlyphSourceMarkedBMP();
  virtual ~GlyphSourceMarkedBMP();
};

GlyphSourceMarkedBMP::GlyphSourceMarkedBMP() {}
GlyphSourceMarkedBMP::~GlyphSourceMarkedBMP() {}

GlyphSource* CreateBMPGlyphSourceFrom(pwide_t source,pwide_t symbols,GlyphSource::SrcType type,mrgb_t ckey)
{
  auto_ref< GlyphSourceMarkedBMP > gsm ( new GlyphSourceMarkedBMP );
  ret0_if_fail( gsm->Init(source,symbols,type,ckey) );
  return Forget(gsm);
}

SCERROR GlyphSourceMarkedBMP::Init(pwide_t source,pwide_t symbols,GlyphSource::SrcType type,mrgb_t ckey)
{
  source_ = source;
  char_height_ = 0;
  type_ = type;
  ckey_ = ckey;
  fail_if ( !GetSymbols(symbols,source,symbols_) );
  return SCE_OK;
}

void GlyphSourceMarkedBMP::Relax()
{
  surface_.release__();
  symdefs_.Purge();
}

SCERROR GlyphSourceMarkedBMP::Ready()
{
  if ( !surface_ ) 
  {
    surface_ = AutoRef(  CreateSurfaceFrom(+source_,Surface::RGBx32) );
    fail_if ( !surface_ );
    if ( type_ == GlyphSource::SRCCOLORKEY )
      Surface::SetAlphaBitsByColorKey_(surface_,ckey_);
    fail_if_fail( ParseFontSurface(surface_,symdefs_) );
    char_height_ = float(symdefs_[0].Height());
  }
  return SCE_OK;
}

long GlyphSourceMarkedBMP::GetCharCount()
{
  return teggo_min<long>(symbols_.Length(),symdefs_.Count());
}

float GlyphSourceMarkedBMP::GetCharHeight()
{
  return char_height_;
}

SCERROR GlyphSourceMarkedBMP::GetChars(BufferT<CHARDEF>& width)
{
  u32_t count = GetCharCount();
  width.Resize(count);
  for ( unsigned i = 0; i < count; ++i ) {
    width[i].widechar = symbols_[i];
    width[i].width = float(symdefs_[i].Width());
  }
  return SCE_OK;
}

Surface* GlyphSourceMarkedBMP::GetCharSurface(long ident,rect_t& rect)
{
  if ( !surface_ ) 
    ret0_because("GlyphSource is not ready");
  if ( ident < 0 || unsigned(ident) >= symdefs_.Count() )
    ret0_because( _S*"glyphsource: invalid symbol %d identifier, range [0..%d)" %ident %symdefs_.Count() );
  rect = symdefs_[ident];
  return ~surface_;
}

NAMESPACE_MSKIN_END

