
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "m_xml.h"
#include "../mSTREAMS/m_dsource.h"
#include "../mSTREAMS/m_wcharsbuf.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mUTIL")
#endif

NAMESPACE_MSKIN_BEGIN

struct WcharOutputStream {
  OutputStream* os_;

  SCERROR WriteText(pwide_t text);
  SCERROR WriteTabbedText(long tabs,pwide_t text);
  SCERROR WriteLine(long tabs,pwide_t text);
  SCERROR Flush();

  WcharOutputStream(OutputStream*);
  ~WcharOutputStream();
};

SCERROR WcharOutputStream::WriteText(pwide_t text)
{
  return SCE_OK;
}

SCERROR WcharOutputStream::WriteTabbedText(long tabs,pwide_t text)
{
  return SCE_OK;
}

SCERROR WcharOutputStream::WriteLine(long tabs,pwide_t text)
{
  return SCE_OK;
}

SCERROR WcharOutputStream::Flush()
{
  return SCE_OK;
}

WcharOutputStream::WcharOutputStream(OutputStream*)
{
}

WcharOutputStream::~WcharOutputStream()
{
}

SCERROR ParseTag(WcharStreamBuffer& wsb, XdataNODE* base, long flags)
{
  wchar_t wc;
  XdataNODE* node = base;
  for(;;) {
    fail_if_fail( wsb.SkipWS() );
    
    if ( wsb.Eos() )
      if ( node != base )
        fail_because(
          _S*"xmlparser: end of file reached, but tag %? is not closed, file '%?'"
            %node->GetName() %wsb.GetSourceName() );
      else return SCE_OK;

    fail_if_fail( wsb.NextChar(&wc) );
    
    if ( wc != L'<' ) {
      if ( node == base ) 
        fail_because(
          _S* "xmlparser: expected '<'  but '%c', file '%s', line %d"
          %wc %wsb.GetSourceName() %wsb.LineNo()));
      BufferT<wchar_t> content(1024);
      content.push_back(wc);
      for(;;) {
        if ( wsb.Eos() ) 
          fail_because(WideFormat(
            L"xmlparser: end of file reached, but tag %s is not closed, file '%s'",
            node->GetName(),wsb.GetSourceName()));
        fail_if_fail(wsb.NextChar(&wc));
        if ( wc == L'<' ) { content.push_back(wc); break; }
      }
    }

    fail_if_fail( wsb.AheadChar(&wc) );
    if ( wc == L'!' ) {
      wchar_t wc0,wc1;
      fail_if_fail( wsb.SkipChar() );
      fail_if_fail( wsb.NextChar(&wc0) );
      fail_if_fail( wsb.NextChar(&wc1) );
      if ( wc0 == wc1 || wc0 == L'-' )
      {
        fail_if_fail( wsb.NextChar(&wc0) );
        fail_if_fail( wsb.NextChar(&wc1) );
        widestring content;
        for(;;) {
          if ( wsb.Eos() ) fail_because(
            WideFormat(L"xmlparser: end of file reached, but comment is not closed, file %s",
              wsb.GetSourceName()));
          fail_if_fail( wsb.NextChar(&wc) );
          if ( wc == L'>' && wc0 == L'-' && wc1 == L'-' ) break;
          content.CatC(wc0);
          wc0 = wc1;
          wc1 = wc;
        }
        if ( flags & XML_FLAGS_PARS_COMMENTS ) {
          XdataNODE* xnode = node->Insert(L"!--");
          xnode->SetContent(content);
        }
      }
      else if ( wc0 == L'[' )
      {
        pwide_t xtag = 0;
        wchar_t buff[6] = {0};
        if ( wc1 == 'C' ) {
          { for ( int i = 0; i < 5; ++i ) fail_if_fail(wsb.NextChar(buff+i)); }
          if ( memcmp(buff,L"DATA[",5) == 0 ) xtag = L"![CDATA[";
        }
        else if ( wc1 == 'Q' ) {
          { for ( int i = 0; i < 5; ++i ) fail_if_fail(wsb.NextChar(buff+i)); }
          if ( memcmp(buff,L"UOTE[",5) == 0 ) xtag = L"![QUOTE[";
        }
        else if ( wc1 == 'S' ) {
          { for ( int i = 0; i < 6; ++i ) fail_if_fail(wsb.NextChar(buff+i)); }
          if ( memcmp(buff,L"CRIPT[",6) == 0 ) xtag = L"![SCRIPT[";
        }
        else 
          fail_because(
            WideFormat(
              L"xmlparser: invalid tag, file '%s', line %d",wsb.GetSourceName(),wsb.LineNo()));
        fail_if_fail( wsb.NextChar(&wc0) );
        fail_if_fail( wsb.NextChar(&wc1) );
        widestring content;
        for(;;) {
          if ( wsb.Eos() ) fail_because(
            WideFormat(L"xmlparser: end of file reached, but comment is not closed, file '%s'",
              wsb.GetSourceName()));
          fail_if_fail( wsb.NextChar(&wc) );
          if ( wc == L'>' && wc0 == L']' && wc1 == L']' ) break;
          content.CatC(wc0);
          wc0 = wc1;
          wc1 = wc;
        }
        XdataNODE* xnode = node->Insert(xtag);
        xnode->SetContent(content);
      }
      else
        fail_because(WideFormat(
          L"xmlparser: expected '<!--' but '<!%c%c', file '%s', line %d",
          wc0,wc1,wc0,wc1,wsb.GetSourceName(),wsb.LineNo()));
    }else if ( wc == L'/' ){
      fail_if_fail( wsb.SkipChar() );
      StringT<wchar_t,16> tag;
      fail_if_fail(wsb.NextWord(&tag));
      if ( wcsicmp(node->GetName(),tag) != 0 ) 
        fail_because(
          WideFormat(
            L"xmlparser: expected </%s> but </%s>, file '%s', line %d",
            node->GetName(),tag.Str(),wsb.GetSourceName(),wsb.LineNo()));
      //XMessageW((L"##colse tag %s",tag.Str()));
      while ( !wsb.Eos() ) {
        fail_if_fail(wsb.NextChar(&wc));
        if ( wc == L'>' ) break;
      }
      node = node->GetParent();
    }else{
      StringT<wchar_t,16> tag;
      fail_if_fail(wsb.NextWord(&tag));
     // XMessageW((L"##open tag %s",tag.Str()));
      node = node->Insert(tag);
      for(;;)
      {
        wchar_t wc;
        StringT<wchar_t,16> attrname;
        StringT<wchar_t,16> value;
        //if ( wsb.LineNo() == 66 ) __debug_break__();
        fail_if_fail(wsb.SkipWS());
        fail_if_fail(wsb.AheadChar(&wc));
        if ( wc == L'/' ) 
        {
          fail_if_fail(wsb.SkipChar());
          fail_if_fail(wsb.NextChar(&wc));
          if ( wc != L'>' )
            fail_because(WideFormat(
              L"xmlparser: expected '>' but %c, file '%s', line %d",
              wc,wsb.GetSourceName(),wsb.LineNo()));
          //XMessageW((L"##colse tag %s",tag.Str()));
          node = node->GetParent();
          break;
        }
        if ( wc == L'>' ) 
        {
          fail_if_fail(wsb.SkipChar());
          break;
        }
        else
        {
          wsb.NextWord(&attrname);
          fail_if_fail(wsb.SkipWS());
          fail_if_fail(wsb.NextChar(&wc));
          if ( wc != L'=' ) 
            fail_because(WideFormat(
              L"xmlparser: expected '=' but %c, file '%s', line %d",
              wc,wsb.GetSourceName(),wsb.LineNo()));
          fail_if_fail(wsb.SkipWS());
          wsb.NextValue(&value);
          node->SetAttr(attrname,value);
        }
      }
    }
  }
}

SCERROR StoreXML(XdataROOT* xml,OutputStream* output);

XdataROOT* ParseXML(pwide_t sourcename,InputStream* source, long flags)
{
  WcharStreamBuffer wsb(sourcename,source);

  XdataROOT* root = new XdataROOT;
  XdataNODE* base = root->GetNode();
  base->SetAttr(L"$/source",sourcename);
  
  while ( !wsb.Eos() ) {
    if ( SCE_OK != ParseTag(wsb,base,flags) ) goto lb_err;
  }

#if defined MSKIN_DEBUG
  {
    LogOutputStream los;
    StoreXML(root,&los);
  }
#endif

  return root;

lb_err:
  delete root;
  return 0;
}

XdataROOT* ParseXMLFrom(pwide_t source, long flags)
{
  AutoDataSource ds ( RSCMAN::CreateSource(source) );
  ret0_if(!ds);
  return ParseXML( source, ~ds, flags );
}

SCERROR StoreXmlNODE(XdataNODE* node, WcharOutputStream* wos, BaseStringT<wchar_t>* textbuf, long tabs )
{
  pwide_t tag         = node->GetName();
  bool    hasChildren = node->HasChildren();
  pwide_t content     = node->GetContent();
  bool    onelinetag  = !content && !hasChildren;

  if ( tag ) {
    if ( tag[0] == L'!' ) {
      textbuf->SetF(L"<%s",tag);
      fail_if_fail( wos->WriteTabbedText(tabs,textbuf->Str()) );
      fail_if_fail( wos->WriteText(content) );
      if ( wcslen(tag) > 1 ) {
        if ( tag[1] == L'[' ) { fail_if_fail( wos->WriteText(L"]]>") ); }
        else if ( tag[1] == L'-' ) { fail_if_fail( wos->WriteText(L"-->") ); }
      }
      onelinetag = true;
    }else{
      textbuf->SetF(L"<%s",tag);
      if ( onelinetag ) textbuf->CatF(L"/>");
      else textbuf->CatF(L">");
      fail_if_fail( wos->WriteLine(tabs,textbuf->Str()) );
    }
  }
  
  if ( !onelinetag ) {
   
    XdataITER xit = node->Iterate();
    while ( xit.Next() ) {
      StoreXmlNODE(xit.Get(),wos,textbuf,tag?tabs+1:0);
    }

    if ( tag ) fail_if_fail( wos->WriteLine(tabs,WideFormat(L"</%s>",tag)) );
  }
  return SCE_OK;
}

SCERROR StoreXML(XdataROOT* xml, OutputStream* output)
{
  widestring text;
  WcharOutputStream wos(output);
  return StoreXmlNODE(xml->GetNode(),&wos,&text,0);
}

NAMESPACE_MSKIN_END
