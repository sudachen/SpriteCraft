
/*

Copyright Â© 2003-2013, AlexÃ©y SudachÃ©n, alexey@sudachen.name

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


#if !defined (__m_graphics_font_h__)
#define __m_graphics_font_h__

#include "../mcore/m_autoconf.h"
#include "../mcore/m_irefobj.h"
#include "../mstreams/m_istream.h"
#include "../mstreams/m_dsource.h"
#include "m_types.h"
#include "m_color.h"
#include "m_surface.h"

NAMESPACE_MSKIN_BEGIN

class SurfFont;

/**
  \interface Surface
  Àáñòðàêöèÿ øðèôòà 
*/
class MOLE_NOVTABLE SurfFont : public virtual IRefObject{
public:
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void* id);
  virtual int GetTextHeight() const = 0;
  virtual int GetTextWidth(pchar_t) const = 0;
  virtual void  SetSymbols(pchar_t) = 0;
  virtual SCERROR Print(Surface*,int x,int y, pchar_t text) const = 0;
  virtual SCERROR Printf(Surface*,int x,int y, pchar_t fmt,...) const = 0;
  virtual SCERROR Load(DataSource*,unsigned format=Surface::SYS_RGB5x5|Surface::ALPHALAYER) = 0;
  virtual void EnableAlpha(bool) = 0;
protected:
	SurfFont() {}
	virtual ~SurfFont() {}
private:
  SurfFont(const SurfFont&);
  SurfFont& operator=(const SurfFont&);
};

class GenericFont : public SurfFont, public virtual MultiLayerSurface, private refbase{
  widestring symbols_;
  unsigned charHeight_;
  byte_t mapsym_[256];
  BufferT<rect_t> rectsym_;
  auto_ref<Surface> imgSurf_;
  bool enableAlpha_;
  int charInterval_;
public:
  // Adaptable
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void* id);
  virtual widestring ToString_() const;
  // IRefObject
  virtual void Grab_() const;
  virtual void Release_() const;
  // SurfFont
  virtual int GetTextHeight() const;
  virtual int GetTextWidth(pchar_t) const;
  virtual void SetSymbols(pchar_t);
  virtual SCERROR Print(Surface*,int x,int y, pchar_t text) const;
  virtual SCERROR Printf(Surface*,int x,int y, pchar_t fmt,...) const;
  virtual void EnableAlpha(bool v);
  virtual SCERROR Load(DataSource*,unsigned format);
  // MultiLayerSurface
  virtual Surface* GetMainSurface();
  virtual Surface* GetAlphaSurface();
  static GenericFont* Create();
protected:
  int GetCharWidth(char) const;
	GenericFont();
	virtual ~GenericFont();
private:
  GenericFont(const SurfFont&);
  GenericFont& operator=(const GenericFont&);
};

typedef auto_ref<SurfFont> AutoSurfFont;
SCERROR ParseFontSurface(Surface* surf,BufferT<rect_t>& rects);

SurfFont*  MOLE_FUNCALL CreateSurfFontFrom(DataSource* source);
SurfFont*  MOLE_FUNCALL CreateSurfFontFrom(pchar_t resname);
SCERROR LoadSurfFontSymbols(SurfFont*,pchar_t);
SCERROR LoadSurfFontSymbols(SurfFont*,DataSource*);

NAMESPACE_MSKIN_END

#endif // __m_graphics_font_h__

