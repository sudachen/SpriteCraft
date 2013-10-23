
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

#include "StdAfx.h"
#include "../ScraftAPI/spritecraft.h"
#include "console.h"
#include "../MoleSkin2/mGRAPHICS/m_gfx.h"
#include "../MoleSkin2/mGRAPHICS/m_gfx3d.h"
#include "../MoleSkin2/mGRAPHICS/m_screen.h"
#include "../MoleSkin2/mGRAPHICS/m_input.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$Engine")
#endif

Console::Console()
  {
    lines_ = 15;
    maxlines_ = 30;
    transparent_ = false;
    visible_ = false;
  }

extern ISprite* Engine_NewDummy();
extern IEngine* Engine_GetCurrent();

void Console::Reinit()
  {
    long symheight = 0;
    {
      rcc_ptr<ITextView> itv;
      con_content_ = rcc_ptr<ISprite> ( Engine_NewDummy() );
      con_content_->SetFrameOfKlass(0,L"$spritecraft$console$font$");
      con_content_->put_style(StyleTextView);
      con_content_->get_textview(&+itv);
      con_content_->put_layer(LONG_MIN);
      con_content_->put_sublayer(LONG_MIN+1);
      con_content_->get_symheight(&symheight);
      itv->put_width(SCREEN::WndWidth());
      itv->put_height(lines_*symheight);
    }
    {
      rcc_ptr<ITextView> itv;
      con_cmdline_ = rcc_ptr<ISprite> ( Engine_NewDummy() );
      con_cmdline_->SetFrameOfKlass(0,L"$spritecraft$console$font$");
      con_cmdline_->put_style(StyleTextView);
      con_cmdline_->put_y(lines_*symheight);
      con_cmdline_->put_layer(LONG_MIN);
      con_cmdline_->put_sublayer(LONG_MIN+1);
      con_cmdline_->get_textview(&+itv);
      itv->put_height(symheight);
      itv->put_width(SCREEN::WndWidth());
    }
    {
      rcc_ptr <IRGBFilter> cfilt;
      con_background_ = rcc_ptr<ISprite> ( Engine_NewDummy() );
      con_background_->SetFrameOfKlass(0,L"$spritecraft$blank$");
      con_background_->put_xSize(SCREEN::WndWidth());
      con_background_->put_ySize((lines_+1)*symheight);
      con_background_->get_cfilt(&+cfilt);
      con_content_->put_layer(LONG_MIN);
      con_content_->put_sublayer(LONG_MIN+2);
      cfilt->put_color(0x00a0a0a0);
    }
    con_content_->put_visible(visible_?VARIANT_TRUE:VARIANT_FALSE);
    con_cmdline_->put_visible(visible_?VARIANT_TRUE:VARIANT_FALSE);
    con_background_->put_visible(visible_?VARIANT_TRUE:VARIANT_FALSE);
    UpdateContent();
  }

Console::~Console()
  {
  }

void Console::Display()
  {
  }

STDMETHODIMP Console::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(IConsole),riid)?S_OK:S_FALSE;
  }

STDMETHODIMP Console::put_visible(/*[in]*/ VARIANT_BOOL on)
  {
    _SYNCHRONIZE_;
    visible_ = on != VARIANT_FALSE;
    con_content_->put_visible(on);
    con_cmdline_->put_visible(on);
    con_background_->put_visible(on);
    if ( visible_ )
      {
        rcc_ptr<ITextView> itv;
        long symheight;

        con_content_->get_symheight(&symheight);
        con_content_->get_textview(&+itv);
        itv->put_width(SCREEN::WndWidth());
        itv->put_height(lines_*symheight);
        Unrefe(itv);

        con_cmdline_->get_textview(&+itv);
        itv->put_width(SCREEN::GetWidth());
        con_cmdline_->put_y(lines_*symheight);

        con_background_->put_xSize(SCREEN::WndWidth());
        con_background_->put_ySize((lines_+1)*symheight);

        UpdateContent();
      }
    return S_OK;
  }

STDMETHODIMP Console::put_transparent(/*[in]*/ VARIANT_BOOL on)
  {
    _SYNCHRONIZE_;
    transparent_ = on != VARIANT_FALSE;
    con_background_->put_transparency(transparent_?50:0);
    return S_OK;
  }

STDMETHODIMP Console::get_visible(/*[in]*/ VARIANT_BOOL* ret)
  {
    _SYNCHRONIZE_;
    *ret = visible_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

STDMETHODIMP Console::get_transparent(/*[in]*/ VARIANT_BOOL* ret)
  {
    _SYNCHRONIZE_;
    *ret = transparent_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

void Console::Print_(pwide_t text)
  {
    widesplitter wspl = widesplitter(text,L'\n');

    if ( content_.Empty() )
      content_.Append(L"");

    if ( wspl.Next() )
      content_.Last().Append(wspl.Get());

    while ( wspl.Next() )
      content_.Append(wspl.Get());
  }

STDMETHODIMP Console::Print(/*[in]*/ BSTR text)
  {
    _SYNCHRONIZE_;
    Print_(text);
    UpdateContent();
    return S_OK;
  }

STDMETHODIMP Console::Println(/*[in]*/ BSTR text)
  {
    _SYNCHRONIZE_;
    Print_(text);
    if ( content_.Last().Length() )
      content_.Append(L"");
    UpdateContent();
    return S_OK;
  }

STDMETHODIMP Console::put_cmdline(/*[in]*/ BSTR text)
  {
    _SYNCHRONIZE_;
    return con_cmdline_->put_text(text);
  }

STDMETHODIMP Console::get_cmdline(/*[out,retval]*/ BSTR* text)
  {
    _SYNCHRONIZE_;
    return con_cmdline_->get_text(text);
  }

STDMETHODIMP Console::Clear()
  {
    _SYNCHRONIZE_;
    con_content_->put_text((BSTR)L"");
    return S_OK;
  }

STDMETHODIMP Console::ProcessInput(/*[out,retval]*/ VARIANT_BOOL* cmdEnter)
  {
    _SYNCHRONIZE_;
    VARIANT_BOOL flag;
    IEngine* eng = Engine_GetCurrent();
    *cmdEnter = VARIANT_FALSE;
    eng->EvtIsKeyDown(&flag);
    if ( flag )
      {
        tagKeyVal key;
        eng->EvtKey(&key);
        if ( key == Key_BACKSPACE )
          {
            teggo::bstr_ptr mbstr;
            con_cmdline_->get_text(&+mbstr);
            widestring val = +mbstr;
            val.Chomp();
            con_cmdline_->put_text((BSTR)val.Str());
          }
        else if ( key == Key_ENTER )
          {
            *cmdEnter = VARIANT_TRUE;
          }
      }
    else
      {
        eng->EvtIsKeyChar(&flag);
        if ( flag )
          {
            teggo::bstr_ptr mbstr;
            wchar_t c;
            eng->EvtKeyChar_(&c);
            con_cmdline_->get_text(&+mbstr);
            con_cmdline_->put_text((BSTR)widestring(+mbstr).Append(c).Str());
          }
      }
    return S_OK;
  }

STDMETHODIMP Console::put_lines(/*[in]*/ long lines)
  {
    _SYNCHRONIZE_;
    if ( lines < 1 )
      lines = 1;
    lines_ = lines;
    if ( visible_ )
      put_visible(VARIANT_TRUE);
    return S_OK;
  }

STDMETHODIMP Console::get_lines(/*[in]*/ long* lines)
  {
    _SYNCHRONIZE_;
    *lines = lines_;
    return S_OK;
  }

STDMETHODIMP Console::put_maxlines(/*[in]*/ long lines)
  {
    _SYNCHRONIZE_;
    if ( lines < 1 )
      lines = 1;
    maxlines_ = lines;
    UpdateContent();
    return S_OK;
  }

STDMETHODIMP Console::get_maxlines(/*[in]*/ long* lines)
  {
    _SYNCHRONIZE_;
    *lines = maxlines_;
    return S_OK;
  }

void Console::UpdateContent()
  {
    unsigned sz = content_.Count();
    if ( sz > maxlines_ )
      while ( sz-- > maxlines_ )
        content_.Pop();

    widestring ws;
    for ( teggo::SlistT<widestring>::Position i = content_.Begin(); i ; ++i )
      ws.Append( (*i).Str() ), ws.Append(L'\n');

    con_content_->put_text((BSTR)ws.Str());

    rcc_ptr<ITextView> itv;
    long symheight = 0;
    con_content_->get_textview(&+itv);
    con_content_->get_symheight(&symheight);

    long l = (signed)content_.Count() - (signed)lines_;
    if ( !content_.Empty() && !content_.Last().Length() ) l -= 1;

    itv->put_yScroll(l*symheight);
  }
