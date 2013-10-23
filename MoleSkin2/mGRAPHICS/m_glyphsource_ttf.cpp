
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

#include "../../mFONTS/freetype.h"
#include "../../mFONTS/ftglyph.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

struct GlyphSourceTTF : public GlyphSource, private refbase
{
  widestring         source_;
  AutoSurface         surface_;
  float               char_height_;
  BufferT<rect_t>      symdefs_;
  FT_Face             face_;

  virtual SCERROR Init(pwide_t source,pwide_t symbols);
  virtual void        Relax();
  virtual SCERROR Ready();
  virtual long        GetCharCount();
  virtual float       GetCharHeight();
  virtual SCERROR GetChars(BufferT<CHARDEF>& width);
  virtual Surface*    GetCharSurface(long ident,rect_t&);

  virtual void Release_() const { refbase::Release_(); }
  virtual void Grab_() const { refbase::Grab_(); }

  GlyphSourceTTF();
  virtual ~GlyphSourceTTF();

  static FT_Library  ft_library_;
  static long        ft_library_count_;

  static SCERROR Init_FreeType_Library()
  {
    if ( !ft_library_ ) {
      if ( !FT_Init_FreeType( &ft_library_ ) ) {
        ft_library_ = 0;
        fail_because("initialization of FreeType library is failed");
      }
    }
    ++ft_library_count_;
    return SCE_OK;
  }

  static void Done_FreeType_Library()
  {
    if ( ft_library_count_ > 0 ) {
      if ( --ft_library_count_ == 0 ) {
        FT_Done_FreeType(ft_library_);
        ft_library_ = 0;
      }
    }
  }
};


FT_Library  GlyphSourceTTF::ft_library_ = 0;
long        GlyphSourceTTF::ft_library_count_ = 0;

GlyphSourceTTF::GlyphSourceTTF() {}
GlyphSourceTTF::~GlyphSourceTTF() {}

GlyphSource* CreateTTFGlyphSourceFrom(pwide_t source,pwide_t symbols)
{
  auto_ref< GlyphSourceTTF > gsm ( new GlyphSourceTTF );
  ret0_if_fail( gsm->Init(source,symbols) );
  return Forget(gsm);
}

SCERROR GlyphSourceTTF::Init(pwide_t source,pwide_t symbols)
{
  face_ = 0;
  return SCE_OK;
}

void GlyphSourceTTF::Relax()
{
  if ( face_ )  { FT_Done_Face(face_); face_ = 0; }
  Done_FreeType_Library();
}

SCERROR GlyphSourceTTF::Ready()
{
  pwide_t errstring = L"";
  fail_if_fail ( Init_FreeType_Library() );
  if ( !face_ ) 
    switch ( FT_New_Face( ft_library_,WideToString(source_).c_str(),0,&face_ ) ) {
    case 0:  break;// ok
    case FT_Err_Unknown_File_Format: errstring = L"invalid font format"; goto broken;
    default: errstring = L"unknown freetype error when load font"; goto broken;
    }
  
  return SCE_OK;
broken:
  GlyphSourceTTF::Relax();
  fail_because(errstring);
}

long GlyphSourceTTF::GetCharCount()
{
  return symdefs_.size();
}

float GlyphSourceTTF::GetCharHeight()
{
  return char_height_;
}

SCERROR GlyphSourceTTF::GetChars(BufferT<CHARDEF>& width)
{
  return SCE_OK;
}

Surface* GlyphSourceTTF::GetCharSurface(long ident,rect_t& rect)
{
  return 0;
}

NAMESPACE_MSKIN_END

