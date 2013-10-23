
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

#include "stdafx.h"
#include "../ScraftAPI/spritecraft.h"
#include "RBGFilter.h"
#include "CollideMgr.h"
#include "SpriteHelper.h"
#include "flat_e3.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Sprites$Surface")
#endif

HRESULT __stdcall PyCreateSurface_(ISurface **pI,PyObject *args,PyObject *kwds)
  {
    wchar_t *source = 0;
    int width = 0;
    int height = 0;
    int pf = 0;
    *pI = 0;

    if ( _PoC_Args(args,kwds,"|iiiu{src}",&width,&height,&pf,&source) )
      {
//         if ( !source )
          if ( width <=0 || height <= 0 )
            return ScraftError("zero or negative surface size");

//         if ( source )
//           *pI = SpriteSurface::FromSource(source,width,height,pf);
//         else
          return (*pI=new SpriteSurface(width,height,pf)), S_OK;
      }
    _PoC_RaiseComExcept();
    return E_FAIL;
  }

HRESULT __stdcall Surface_RegisterPyType()
  {
    return SpriteSurface::RegisterPyType("Surface");
  }

SpriteSurface *SpriteSurface::FromSource(pwide_t source, int width, int height, int pf)
  {
    return 0;
  }

SpriteSurface::SpriteSurface(int width, int height, int pf)
  {
    if ( !pf )
      pf = TFF_PF_RGB5;
    else if ( pf != TFF_PF_RGB5 && pf != TFF_PF_RGBA8 && pf != TFF_PF_RGB8 && pf != TFF_PF_RGBA4 )
      pf = TFF_PF_RGB5;
    bitmap_ = RccPtr(new TFFt_BITMAP(width,height,(TFFt_PIXEL_FORMAT)pf));
    klass_name_ = _S*L"$spritecraft$dynamic$%08x$" %this;
    SPRITES::NewDynamicKlass(+klass_name_,this);
  }

SpriteSurface::~SpriteSurface()
  {
    SPRITES::UnloadKlass(+klass_name_,true);
  }

HRESULT __stdcall SpriteSurface::get_width(long *pVal)
  {
    *pVal = bitmap_->Width();
    return S_OK;
  }

HRESULT __stdcall SpriteSurface::get_height(long *pVal)
  {
    *pVal = bitmap_->Height();
    return S_OK;
  }

HRESULT __stdcall SpriteSurface::get_pf(long *pVal)
  {
    *pVal = bitmap_->PixelFormat();
    return S_OK;
  }

HRESULT __stdcall SpriteSurface::GetHDC(long *pVal)
  {
    *pVal = (long)bitmap_->GetDC();
    return S_OK;
  }

wchar_t * __stdcall SpriteSurface::__klass_name__()
  {
    return (wchar_t*)+klass_name_;
  }

HRESULT __stdcall SpriteSurface::Update()
  {
    SPRITES::UpdateKlass(+klass_name_);
    return S_OK;
  }

void SpriteSurface::FillInfo(SPRITE_Info *i)
  {
    i->bitmap = _S*L"*memory:%08x*" %+bitmap_;
    i->alpha = L"";
    i->symbols = L"";
    i->spr_width = i->bitmap_width  = bitmap_->Width();
    i->spr_height = i->bitmap_height = bitmap_->Height();
    i->spr_no = 1;
    i->spr_x = i->spr_y = 0;
    i->transparecy = 0;
    i->renderable_ = 1;
    i->hotspot_x = i->hotspot_y = 0;
    memset(&i->colorkey,0,sizeof(i->colorkey));
    i->collideRadius = 0;
    if ( (bitmap_->PixelFormat() & TFF_PF_BPPMASK) > 16 )
      i->bits_rgb32 = 1;
    else if ( bitmap_->PixelFormat() & TFF_PF_HALF )
      i->bits_lowcolor = 1;
    i->type = SPRITE_SOLID;
  }

TFFt_BITMAP *SpriteSurface::GetBitmap()
  {
    return +bitmap_;
  }
