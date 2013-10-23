
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

#if !defined __m_glyphsource_h__
#define __m_glyphsource_h__

#include "m_gfx.h"
#include "m_gfx3d.h"
#include <stdarg.h>

NAMESPACE_MSKIN_BEGIN

struct GlyphSource : public IRefObject
{
  enum SrcType {
    SRCBLEND    = 0,
    SRCCOLORKEY = 1,
    SRCSOLID    = 2,
  };
  struct CHARDEF{
    wchar_t widechar;
    float   width;
  };
  virtual void        Relax() = 0;
  virtual SCERROR Ready() = 0;
  virtual long        GetCharCount() = 0;
  virtual float       GetCharHeight() = 0;
  virtual SCERROR GetChars(BufferT<CHARDEF>& width) = 0;
  virtual Surface*    GetCharSurface(long ident,rect_t&) = 0;
protected:
  GlyphSource();
  virtual ~GlyphSource();
};

typedef MSKINSPACE::auto_ref<GlyphSource> AutoGlyphSource;

GlyphSource* CreateGlyphSourceFrom(pwide_t font,pwide_t symbols,GlyphSource::SrcType type = GlyphSource::SRCBLEND,mrgb_t ckey=mRGB_t(0,0,0));
GlyphSource* CreateBMPGlyphSourceFrom(pwide_t source,pwide_t symbols,GlyphSource::SrcType type = GlyphSource::SRCBLEND,mrgb_t ckey=mRGB_t(0,0,0));
GlyphSource* CreateTTFGlyphSourceFrom(pwide_t source,pwide_t symbols,GlyphSource::SrcType type = GlyphSource::SRCBLEND,mrgb_t ckey=mRGB_t(0,0,0));

NAMESPACE_MSKIN_END

#endif // __m_glyphsource_h__
