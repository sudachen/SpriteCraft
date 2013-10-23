
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

#if !defined __m_texfont_h__
#define __m_texfont_h__

#include "m_gfx.h"
#include "m_gfx3d.h"
#include <stdarg.h>
#include "m_glyphsource.h"

NAMESPACE_MSKIN_BEGIN

class TexFont : public IRefObject {
public:
  struct SYMBOL 
    {
      u32_t symbol;
      float width,x,y;
      bool operator < ( const SYMBOL& a ) const { return symbol < a.symbol; }
    };
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual int       GetTextHeight() const = 0;
  virtual int       GetTextWidth(pwide_t,int count=-1) = 0;
  virtual Texture*  GetSymbolTexture(u32_t symbol) = 0;
  virtual void      FillQuad(VxB_VERTEX* vx,u32_t symbol,float x, float y,const frect_t* clip=0) = 0;
  virtual void      FillSymbols(BufferT<SYMBOL>& syms,pwide_t text,int len=-1) = 0;
protected:
  TexFont() {}
  virtual ~TexFont(){}
private:
  TexFont(const TexFont&);
  TexFont& operator=(const TexFont&);
};
typedef MSKINSPACE::auto_ref<TexFont> AutoTexFont;

//TexFont* CreateTexFontFrom(pwide_t,pwide_t symbols=0,unsigned texfmt=GFX3D_TEX_ALPHA);
//TexFont* CreateTexFontFrom(DataSource*,pwide_t symbols=0,unsigned texfmt=GFX3D_TEX_ALPHA);
//TexFont* AcquireTexFontFromSet(pwide_t,pwide_t symname=0);  //format=GFX3D_TEX_ALPHA? or RGBA 

//SCERROR LoadTexFontSymbols(TexFont*,pwide_t);
//SCERROR LoadTexFontSymbols(TexFont*,DataSource*);

struct ProxyTexFont_OfTexFontSet;
class TexFontSet : public refbase {
public:
  static TexFontSet* AcquireSet();
  static SCERROR Restore();
  TexFont* Acquire_(pwide_t name,pwide_t symname=0,GlyphSource::SrcType type = GlyphSource::SRCBLEND,mrgb_t ckey=mRGB_t(0,0,0));
  static TexFont* Acquire(pwide_t name,pwide_t symname=0,GlyphSource::SrcType type = GlyphSource::SRCBLEND,mrgb_t ckey=mRGB_t(0,0,0)) {
    return AutoRef(AcquireSet())->Acquire_(name,symname,type,ckey);
  }
private:
  friend struct ProxyTexFont_OfTexFontSet;
  static TexFontSet* instance_;
  void UnAcquire(ProxyTexFont_OfTexFontSet*);
  typedef CollectionT<ProxyTexFont_OfTexFontSet*> FONTMAP;
  FONTMAP fonts_;
protected:
  TexFontSet();
  virtual ~TexFontSet();
};
typedef auto_ref<TexFontSet> AutoTexFontSet;

NAMESPACE_MSKIN_END

#endif // __m_texfont_h__
