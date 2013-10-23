
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

struct XmlDefParser {
  static SCERROR Parse(WcharStreamBuffer& wsb, XdataNODE* base, long flags);
};

SCERROR XmlDefParser::Parse(WcharStreamBuffer& wsb, XdataNODE* base, long flags)
{
  wchar_t wc;
  StringT<wchar_t,64> word;
  XdataNODE* node = base;
  for(;;) {
    fail_if_fail( wsb.SkipWS() );
    
    if ( wsb.Eos() )
      if ( node != base )
        fail_because(WideFormat(L"xmldefparesr: end of file reached, but tag %s is not closed, file '%s'",node->GetName(),wsb.GetSourceName()));
      else return SCE_OK;

    XdataNODE* xnode = 0;
    word.Set(L"");
    
    fail_if_fail( wsb.AheadChar(&wc) );

    if ( wc == L'{' ) // unnamed tag
    {
      fail_if_fail(wsb.SkipChar());
      xnode = node->Insert(L"");
      goto tag_block;
    }
    else if ( wc == L'}' ) // and tag
    {
      fail_if_fail(wsb.SkipChar());
      if ( node == base ) 
        fail_because(WideFormat(L"xmldefparesr: invalid '}' brace, file '%s', line %d",wsb.GetSourceName(),wsb.LineNo()));
      node = node->GetParent();
    }
    else if ( wc == '/' )
    {
      fail_if_fail(wsb.SkipChar());
      fail_if_fail(wsb.NextChar(&wc));
      if ( wc != L'/' ) 
        fail_because(WideFormat(L"xmldefparesr: invalid '%c' character, file '%s', line %d",wc,wsb.GetSourceName(),wsb.LineNo()));
      else
        if ( flags & XML_FLAGS_PARS_COMMENTS )
        {
          XdataNODE* com = node->Insert(L"$/comment");
          StringT<wchar_t> data;
          wsb.NextLine(&data);
          com->SetContent(data);
        }
        else
          wsb.SkipLine();
    }
    else
    {
      fail_if_fail( wsb.NextWord(&word) );
      if ( word.Length() == 0 ) fail_because(WideFormat(L"xmldefparesr: invalid tag/attribut, file '%s', line %d",wsb.GetSourceName(),wsb.LineNo()));
      fail_if_fail( wsb.SkipWS() );
      fail_if_fail( wsb.NextChar(&wc) );
      if ( wc == L'=' ) 
      {
        StringT<wchar_t,64> value;
        fail_if_fail( wsb.SkipWS() );
        fail_if_fail(wsb.NextValue(&value));
        node->SetAttr(word.Str(),value.Str());
        if ( flags & XML_FLAGS_PARS_LINENO ) {
          StringT<wchar_t,32> lineno = WideFormat(L"%d",wsb.LineNo());
          node->SetAttr(WideFormat(L"$/!%s",word.Str()),lineno.Str()); }
      }
      else
      {
        if ( wc == L'(' ) 
        {
          xnode = node->Insert(word.Str());
          if ( flags & XML_FLAGS_PARS_LINENO ) xnode->SetAttr(L"$/lineno",WideFormat(L"%d",wsb.LineNo()));
          StringT<wchar_t,64> content;
          bool rawdata = false;
          fail_if_fail(wsb.SkipWS());
          for (;;)
          {
            if ( wsb.Eos() ) fail_because(WideFormat(L"xmldefparesr: end of file reached, but tag content is not closed, file '%s'",wsb.GetSourceName()));
            fail_if_fail(wsb.NextChar(&wc));
            if ( wc == L'&') {
              wchar_t wc1;
              fail_if_fail(wsb.AheadChar(&wc1));
              if ( !rawdata && wc1 == L'[' ) rawdata = true, wsb.SkipChar(); 
              else if ( rawdata && wc1 == L']' ) rawdata = false, wsb.SkipChar(); 
              else content.CatC(wc);              
            } 
            else if ( wc == L')' && !rawdata ) break;
            else content.CatC(wc);
          }
          xnode->SetContent(content.Str());
          fail_if_fail( wsb.SkipWS() );
          fail_if_fail( wsb.AheadChar(&wc) );
          if ( wc == L'{' ) wsb.SkipChar();
          else if ( wc == L'=' )
          {
            wsb.SkipChar();
            StringT<wchar_t,64> value;
            fail_if_fail( wsb.SkipWS() );
            fail_if_fail(wsb.NextValue(&value));
            xnode->SetAttr(L"$/value",value.Str());
          }
        }
        if ( wc == L'{' )
        {
          if ( !xnode ) {
            xnode = node->Insert(word.Str());
            if ( flags & XML_FLAGS_PARS_LINENO ) xnode->SetAttr(L"$/lineno",WideFormat(L"%d",wsb.LineNo()));
          }
        tag_block:
          node = xnode;
        }
        else 
          if ( !xnode ) fail_because(WideFormat(L"xmldefparesr: should be '=' or '{', file '%s', line %d",wsb.GetSourceName(),wsb.LineNo()));
      }
    }
  }
}

XdataROOT* ParseDEF(pwide_t sourcename,InputStream* source, long flags)
{
  WcharStreamBuffer wsb(sourcename,source);

  XdataROOT* root = new XdataROOT;
  XdataNODE* base = root->GetNode();
  base->SetAttr(L"$/source",sourcename);
  
  while ( !wsb.Eos() ) {
    if ( SCE_OK != XmlDefParser::Parse(wsb,base,flags) ) goto lb_err;
  }

  return root;

lb_err:
  delete root;
  return 0;
}

XdataROOT* ParseDEFFrom(pwide_t source, long flags)
{
  AutoDataSource ds ( RSCMAN::CreateSource(source) );
  ret0_if(!ds);
  return ParseDEF( source, ~ds, flags );
}

static void StoreATTR(OutputStream *o, const XdataATTR *a, BaseStringT<char> &prefix )
{
  StringT<char,128> buf;
  while ( a )
  {
    if ( a->name && a->name[0] && !widestring::StartWith_( a->name, L"$/" ) )
    {
      buf = prefix;
      buf.Cat(a->name);
      buf.Cat(" = '");
      buf.Cat(a->value.Str() );
      buf.Cat("'\n");
      o->WriteS(buf.Str());
    }
    a = a->next;
  }
}

static void ContentConvertToDef( BaseStringT<char> &buf )
{
  buf.Insert( 0, "(" );
  buf.Insert( -1, ")" );
}

static void StoreNODE(OutputStream *o, XdataNODE *n, BaseStringT<char> &prefix )
{  

  if ( 0 == wcscmp( n->GetName(), L"$/comment" ) )
  {
    o->WriteS( prefix );
    o->WriteS( "//" );
    o->WriteS( StringT<char>(n->GetContent()).Str() );
    o->WriteS( "\n" );
    return;
  }

  bool isroot = !wcscmp( n->GetName(), L"$/root" );
  if ( !isroot )
  {
    o->WriteS( prefix );
    o->WriteS( StringT<char,32>(n->GetName()).Str() );
  
    if ( pwide_t content = n->GetContent() )
    {
      StringT<char> buf = n->GetContent();
      ContentConvertToDef( buf ); 
      o->WriteS( buf.Str() );
    }
  
    o->WriteS( prefix );
    o->WriteS("\n");
  }

  const XdataATTR *a = n->GetFirstAttr();
  XdataITER i = n->Iterate();

  if ( a || i.Reset() || !n->GetContent() ) 
  {
    if ( !isroot )
    {
      o->WriteS( prefix );
      o->WriteS("{\n");
      prefix.Cat( "  " );
    }
    StoreATTR(o, a , prefix);
    while ( i.Next() ) StoreNODE( o, i.Get(), prefix );
    if ( !isroot )
    {
      prefix.Chomp( 2 );
      o->WriteS( prefix );
      o->WriteS("}\n");
    }
  }
  
}

SCERROR StoreDEFTo(pwide_t filename, long, XdataNODE *node)
{
  widestring path = RSCMAN::MakePath(filename);
  if ( auto_ref<OutputStreamEx> ose = auto_ref<OutputStreamEx>( new FileOutputStream(path) ) )
  {
    StringT<char,128> prefix;
    StoreNODE( ose, node, prefix );
    return SCE_OK;
  }
  else
    return SCE_FAIL;
}

NAMESPACE_MSKIN_END
