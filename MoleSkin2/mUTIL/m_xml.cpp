
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

#if CXX_COMPILER__IS_MSVC_COMPATIBLE 
# pragma code_seg(".text$Xdata$parser")
#endif

pwide_t XdataLiteral::FindLiteral(pwide_t val)
  {
    if ( !val ) return 0;
    PWide pw;
    pw.text = val;
    pw.len  = wcslen(val);
    if ( PWide *ppw = lset_.Get(pw) ) 
      return ppw->text;
    return 0;
  }

pwide_t XdataLiteral::GetLiteral(pwide_t val)
  {
    PWide pw;
    pw.text = val;
    pw.len  = wcslen(val);
    if ( PWide *ppw = lset_.Get(pw) ) 
      return ppw->text;
    else 
      {
        pw.text = new wchar_t[pw.len+1];
        memcpy((void*)pw.text,val,(pw.len+1)*sizeof(wchar_t));
        lset_.Put(pw);
        return pw.text;
      }
  }

XdataLiteral::XdataLiteral()
  {
  }

XdataLiteral::~XdataLiteral()
  {
    for ( lset_t::Iterator i = lset_.Iterate(); i.Next(); ) 
      delete[] (wchar_t*)i->text;
  }

XdataROOT::XdataROOT()
  {
    literal_ = new XdataLiteral();
    base_ = 0;
    freezed_ = false;
    base_ = new XdataNODE(L"$/root",0,this,&base_);
  }

XdataROOT::~XdataROOT()
  {
    base_->Erase();
    delete literal_;
  }


void XdataNODE::Erase()
  {
    if ( next_ ) 
      next_->pnext_ = pnext_;
    else if (parent_) 
      {
        REQUIRE ( parent_->psubnode_ == &next_ );
        parent_->psubnode_ = pnext_;
      }
    *pnext_ = next_;
    delete this;
  }

XdataNODE* XdataNODE::Insert(pwide_t tag)
  {
    StringT<wchar_t,64> mytag(tag);
    mytag.ToLower();
    tag = root_->literal_->GetLiteral(mytag.Str());
    return Insert_(tag);
  }

XdataNODE* XdataNODE::GetTag(pwide_t tag)
  {
    pwide_t mytag = TranslateName_(tag);
    if ( mytag ) return FindTag_(mytag);
    return 0;
  }

pwide_t XdataNODE::GetAttr(pwide_t attr)
  {
    if ( attr && wcscmp(attr,L"$/content") == 0 )
      {
        return GetContent();
      }
    else
      {
        if ( pwide_t myattr = TranslateName_(attr) )
          {
            XdataATTR* attrval = FindAttr_(myattr); 
            if ( attrval ) 
              return attrval->GetValue();
          }
        return 0;
      }
  }

void XdataNODE::SetAttr(pwide_t attr,pwide_t value)
  {
    if ( attr && wcscmp(attr,L"$/content") == 0 )
        SetContent( value );
    else
      {
        if ( pwide_t myattr = root_->literal_->GetLiteral(StringT<wchar_t,64>(attr).ToLower().Str()) )
          {
            XdataATTR* attrval = FindAttr_(myattr); 
            if ( !attrval )
              attr_ = new XdataATTR(myattr,attr_,value);
            else
              attrval->SetValue(value);
          }
      }
  }

long XdataNODE::GetCountTag(pwide_t tag)
  {
    if ( tag ) 
      {
        tag = TranslateName_(tag);
        if ( !tag ) return 0;
      }
    long count = 0;
    XdataNODE* node = subnode_;
    while ( node ) 
      {
        if ( !tag || node->name_ == tag ) ++count;
        node = node->next_;
      }
    return count;
  }

XdataITER XdataNODE::Iterate(pwide_t tag)
  {
    if ( tag ) 
      {
        tag = TranslateName_(tag);
        if ( !tag ) return XdataITER(0,0);
      }
    return XdataITER(this,tag);
  }

pwide_t XdataNODE::TranslateName_(pwide_t tag)
  {
    pwide_t mytag = root_->literal_->FindLiteral(tag);
    if ( !mytag ) 
      mytag = root_->literal_->FindLiteral(StringT<wchar_t,64>(tag).ToLower().Str());
    return mytag;
  }

XdataNODE* XdataNODE::FindTag_(pwide_t tag)
  {
    XdataNODE* node = subnode_;
    while ( node ) 
      {
        if ( node->name_ == tag ) return node;
        node = node->next_;
      }
    return 0;
  }

XdataATTR* XdataNODE::FindAttr_(pwide_t name)
  {
    XdataATTR* attr = attr_;
    while ( attr ) 
      {
        if ( attr->name == name ) return attr;
        attr = attr->next;
      }
    return 0;
  }

XdataNODE* XdataNODE::Insert_(pwide_t tag)
  {
    XdataNODE* node = new XdataNODE(tag,this,root_,psubnode_);
    psubnode_ = &node->next_;
    return node;
  }

XdataNODE::XdataNODE(pwide_t name,XdataNODE* parent,XdataROOT* root,XdataNODE** pnext)
  {
    name_ = name;
    parent_ = parent;
    root_   = root;
    if ( pnext ) *pnext = this;
    pnext_ = pnext;

    attr_ = 0;
    subnode_ = 0;
    psubnode_ = &subnode_;
    next_ = 0;
  }

XdataNODE::~XdataNODE()
  {
    while ( subnode_ ) subnode_->Erase();
    while ( attr_ ) 
      {
        XdataATTR* a = attr_->next;
        delete attr_;
        attr_ = a;
      }
  }

XdataNODE* XdataITER::FindNextWithTag(XdataNODE* node,pwide_t tag)
  {
    while ( node ) 
      {
        if ( !tag || node->name_ == tag ) return node;
        node = node->next_;
      }
    return 0;
  }

bool XdataITER::Next()
  {
    curr_ = next_;
    if ( next_ ) next_ = FindNextWithTag(next_->next_,tag_);
    return !!curr_;
  }

bool XdataITER::Reset()
  {
    if ( base_ ) 
      next_ = FindNextWithTag(base_->subnode_,tag_);
    else
      next_ = 0;
    curr_ = 0;
    return !!next_;
  }

XdataNODE* XdataNODE::InsertCopyOf(XdataNODE* n, bool requce)
  {
    XdataNODE* cpy = Insert( n->GetName() );
    for ( XdataATTR* a = n->attr_; a; a = a->next )
      cpy->SetAttr( a->name, +a->value );
    if ( requce )
      {
        XdataITER i = n->Iterate();
        while ( i.Next() )
          cpy->InsertCopyOf( i.Get(), true );
      }
    return cpy;
  }

