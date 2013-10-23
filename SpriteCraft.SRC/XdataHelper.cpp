
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
#include "XdataHelper.h"

static wchar_t ERROR_XNODE_DISPOSED[] = L"Xdata node has been disposed";
static wchar_t ERROR_ATTRCHANGE_XDOC_FRIZED[] = L"could not change node attribute '%?', document freezed";
static wchar_t ERROR_CONTCHANGE_XDOC_FRIZED[] = L"could not change node content, document freezed";
static wchar_t ERROR_ERASENODE_XDOC_FRIZED[]  = L"could not erase node, document freezed";
static wchar_t ERROR_INSERTNODE_XDOC_FRIZED[] = L"could not insert node, document freezed";

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$scraft$xdata")
#endif

#define NASCE_PRESERVE 64
#define PATH_PRESERVE 128

int XdataDetectFormat(wchar_t *code)
  {
    while ( *code && iswspace(*code) ) ++code;
    if ( *code == '<' ) return XdataXMLformat;
    switch ( *code )
      { case '/': case '?': return XdataDEFformat; }
    if ( isalnum(*code) )
      return XdataDEFformat;
    return -1;
  }

HRESULT __stdcall PyCreateXdata_(IXdata **xdata,PyObject *args,PyObject *kwds)
  {
    wchar_t *code = 0;
    int format = -1;
    if ( _PoC_Args(args,kwds,"|u{code}i{format}",&code,&format) )
      {
        Xdocument doc;
        if ( !code )
          doc = Xdocument_New();
        else
          {
            rcc_ptr<DataStream> ds;
            if ( code[0] == '!' )
              {
                ds = teggo::DataSource->Open(code+1);
                if ( format < 0 && teggo::BaseStringT<wchar_t>::EndsWith_(code,L".def") )
                  format = XdataDEFformat;
                if ( format < 0 && teggo::BaseStringT<wchar_t>::EndsWith_(code,L".xml") )
                  format = XdataXMLformat;
                format = format < 0 ? 0 : format;
              }
            else
              format = (format >= 0 ? format : XdataDetectFormat(code)),
              ds.reset_(new MemDataStream(code,wcslen(code)*2,
                          FILEOPEN_UNICODE_MEMSTREAM|FILEOPEN_READ));
            if ( ds )
              if ( format == XdataXMLformat )
                doc = XopenXmlDocument(*ds);
              else if ( format == XdataDEFformat )
                doc = XopenDefDocument(*ds);
              else
                _PoC_RaiseExcept("unknown format");
            else
              _PoC_RaiseExcept(0|_S*"failed to open xdata source '%s'" %(code+1));
          }
        if ( doc )
          {
            *xdata = new XdataHelper(doc,doc->GetRoot());
            return S_OK;
          }
        _PoC_RaiseExcept("failed to parse");
      }
    return E_FAIL;
  }

HRESULT __stdcall Xdata_RegisterPyType()
  {
    return XdataHelper::RegisterPyType("Xdata");
  }

XdataHelper::XdataHelper(Xdocument const &doc,Xnode *node)
  {
    doc_ = doc;
    node->Link(&node_);
  }

XdataHelper::~XdataHelper()
  {
  }

HRESULT XdataHelper::InternalQueryInterface1(REFIID riid ,void ** pI)
  {
    if ( IsEqualGUID(TEGGO_GUIDOF(XdataHelper),riid) )
      return *pI = Refe(this), S_OK;
    else if ( IsEqualGUID(TEGGO_GUIDOF(Xnode),riid) )
      return *pI = +this->node_, S_OK;
    else
      return _SuperImpl::InternalQueryInterface(riid,pI);
  }

HRESULT __stdcall XdataHelper::GetTag(BSTR name, IXdata **pI)
  {
    _SYNCHRONIZE_;
    *pI = 0;
    if ( node_ )
      {
        if ( !name || !*name )
          {
            Xiterator i = node_->Iterate();
            while ( i.Next() )
              if ( !teggo::StrStartWith(i->GetName(),L"<?") )
                { *pI = new XdataHelper(doc_,+i); break; }
          }
        else if ( Xnode* n = node_->GetNode( name ) )
          *pI = new XdataHelper(doc_,n);
      }
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetStrAttr(BSTR name, BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pVal = SysAllocString( node_->GetParam(name) ), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetIntAttr(BSTR name, long *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pVal = teggo::ToLong( node_->GetParam(name) ), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetFltAttr(BSTR bstr_name, float *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pVal = teggo::ToFloat( node_->GetParam(bstr_name) ), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetBoolAttr(BSTR bstr_name, VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pVal = teggo::ToBool( node_->GetParam(bstr_name) ), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::HasAttr(BSTR bstr_name, VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return ((*pVal = node_->GetParam(bstr_name) ? VARIANT_TRUE : VARIANT_FALSE), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetName(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pVal = SysAllocString( node_->GetName() ), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetContent(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      {
        pwide_t cont = node_->GetContent();
        return (*pVal = SysAllocString(cont?cont:L""), S_OK);
      }
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetCount(long *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pVal = node_->GetCountNodes(0), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetCountTag(BSTR tag, long *pVal)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pVal = node_->GetCountNodes(tag), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::Attributes(IXdataAttributes **pI)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pI = new XdataAttributes(doc_,+node_), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::Tags(BSTR tag, IXdataEnumerator **pI)
  {
    _SYNCHRONIZE_;
    if ( node_ )
      return (*pI = new XdataEnumerator(doc_,+node_,tag), S_OK);
    else
      return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::SetBoolAttr(BSTR name,  VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(_S*ERROR_ATTRCHANGE_XDOC_FRIZED %name);
    node_->SetParam(name,newVal!=VARIANT_FALSE?L"true":L"false");
    return S_OK;
  }

HRESULT __stdcall XdataHelper::SetFltAttr(BSTR name,  float newVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(_S*ERROR_ATTRCHANGE_XDOC_FRIZED %name);
    node_->SetParam(name,teggo::Ftow(newVal));
    return S_OK;
  }

HRESULT __stdcall XdataHelper::SetIntAttr(BSTR name,  long newVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(_S*ERROR_ATTRCHANGE_XDOC_FRIZED %name);
    node_->SetParam(name,teggo::Itow(newVal));
    return S_OK;
  }

HRESULT __stdcall XdataHelper::SetStrAttr(BSTR name,  BSTR newVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(_S*ERROR_ATTRCHANGE_XDOC_FRIZED %name);
    node_->SetParam(name,newVal);
    return S_OK;
  }

HRESULT __stdcall XdataHelper::SetContent(BSTR newVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(ERROR_CONTCHANGE_XDOC_FRIZED);
    node_->SetContent(newVal);
    return S_OK;
  }

HRESULT __stdcall XdataHelper::Erase()
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(ERROR_ERASENODE_XDOC_FRIZED);
    node_->Erase();
    STRICT_ENSURE( +node_ == 0 );
    Unrefe(doc_);
    return S_OK;
  }

HRESULT __stdcall XdataHelper::Insert(BSTR name, IXdata** pI)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(ERROR_INSERTNODE_XDOC_FRIZED);
    *pI = new XdataHelper( doc_, node_->Insert(name) );
    return S_OK;
  }

HRESULT __stdcall XdataHelper::InsertCopyOf(IXdata* node, VARIANT_BOOL req, IXdata** pI)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( doc_->freezed_ )
      return ScraftError(ERROR_INSERTNODE_XDOC_FRIZED);
    XdataHelper* helper = 0;
    node->QueryInterface( TEGGO_GUIDOF(XdataHelper), (void**)&helper );
    Xnode *n = helper->GetRawNode();
    if ( n->GetRoot() == n )
      {
        Xiterator i = n->Iterate();
        while ( i.Next() )
          if ( !teggo::StrStartWith(i->GetName(), L"<?") )
            { n = +i; break; }
      }
    if ( n )
      *pI = new XdataHelper( doc_, node_->InsertCopyOf(n,!!req) );
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetNameAsPWIDE(wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    *pVal = (wchar_t*)node_->GetName();
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetContentAsPWIDE(wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    *pVal = (wchar_t*)node_->GetContent();
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetStrAttrAsPWIDE(BSTR name,wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    *pVal = (wchar_t*)node_->GetParam(name);
    return S_OK;
  }

HRESULT __stdcall XdataHelper::Freeze()
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    if ( +doc_ ) doc_->freezed_ = true;
    return S_OK;
  }

HRESULT __stdcall XdataHelper::Clone(IXdata **pI)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    Xdocument doc = Xdocument_New();
    *pI = new XdataHelper( doc, doc->GetRoot()->InsertCopyOf(+node_,true) );
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetRoot(IXdata **pI)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    *pI = new XdataHelper( doc_, doc_->GetRoot() );
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetParent(IXdata **pI)
  {
    _SYNCHRONIZE_;
    if ( !node_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    *pI = new XdataHelper( doc_, node_->GetParent() );
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetSrcName(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( !doc_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    *pVal = SysAllocString(doc_->GetSource());
    return S_OK;
  }

HRESULT __stdcall XdataHelper::GetSrcNameAsPWIDE(wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    if ( !doc_ )
      return ScraftError(ERROR_XNODE_DISPOSED);
    *pVal = (wchar_t*)doc_->GetSource();
    return S_OK;
  }

HRESULT __stdcall XdataHelper::Next(BSTR tag,IXdata **pI)
  {
    __synchronize__
      {
        if ( !node_ )
          return ScraftError(ERROR_XNODE_DISPOSED);
        if (!tag || !*tag) tag = 0;
        if ( Xnode *node = node_->GetNext(tag) )
          *pI = new XdataHelper(doc_,node);
        else
          *pI = 0;
      }
    return S_OK;
  }

XdataEnumerator::XdataEnumerator(Xdocument const &doc,Xnode *node,BSTR tag)
  {
    //__synchronize__
      {
        doc_ = doc;
        i_ = -1;
        if ( tag && *tag == 0 ) tag = 0;
        int count = node->GetCountNodes(tag);
        nodes_.Resize(count);
        int j = 0;
        Xiterator i = node->Iterate(tag);
        for ( int no = 0; i.Next(); ++no )
          {
            if ( !tag && teggo::StrStartWith(i->GetName(),L"<?") ) continue;
            new (&nodes_[j]) WeakLink<Xnode*>();
            i.Get()->Link(&nodes_[j]);
            ++j;
          }
        nodes_.Resize(j);
      }
  }

XdataEnumerator::XdataEnumerator(Xdocument const &doc,Xnode *node,pwide_t cont, pwide_t tag)
  {
    //__synchronize__
      {
        doc_ = doc;
        i_ = -1;
        if ( tag && *tag == 0 ) tag = 0;
        int count = node->GetCountNodes(tag);
        nodes_.Resize(count);
        Xiterator i = node->Iterate(tag);
        int j = 0;
        for ( int no = 0; i.Next(); ++no )
          {
            if ( tag || !teggo::StrStartWith(i->GetName(),L"<?") )
              if ( pwide_t c = i->GetContent() )
                if ( cont && !wcscmp(cont,c) )
                  {
                    new (&nodes_[j]) WeakLink<Xnode*>();
                    i.Get()->Link(&nodes_[j]);
                    ++j;
                  }
          }
        nodes_.Resize(j);
      }
  }

XdataEnumerator::~XdataEnumerator()
  {
    __synchronize__
      for ( int no = 0, nE = nodes_.Count(); no != nE; ++no )
        nodes_[no].~WeakLink<Xnode*>();
  }

HRESULT __stdcall XdataEnumerator::Reset(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    i_ = -1;
    *pVal = nodes_.Count() ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall XdataEnumerator::Next(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( i_ + 1 >= nodes_.Count() )
      *pVal = VARIANT_FALSE;
    else
      ++i_, *pVal = VARIANT_TRUE;
    return S_OK;
  }

HRESULT __stdcall XdataEnumerator::Get(IXdata **pI)
  {
    _SYNCHRONIZE_;
    *pI = 0;
    if ( i_ < 0 ) { VARIANT_BOOL has; Next(&has); }
    if ( i_ >= 0 && i_ < nodes_.Count() )
      *pI = new XdataHelper( doc_, +nodes_[i_] );
    return S_OK;
  }

HRESULT SpriteCraft_NewXML(BSTR tag, IXdata **pI)
  {
    _SYNCHRONIZE_;
    Xdocument doc = Xdocument_New();
    *pI = new XdataHelper( doc, doc->GetRoot()->Insert(tag) );
    return S_OK;
  }

HRESULT SpriteCraft_ParseDEF(pwide_t source,long flags,IXdata** node)
  {
    _SYNCHRONIZE_;

    ResetErrorLog();

    Xdocument doc( XopenDefDocument(source) );
    if ( +doc )
      {
        rcc_ptr<XdataHelper> helper( new XdataHelper(doc,doc->GetRoot()) );
        *node = teggo::Forget(helper);
        return S_OK;
      }
    return GenSCWideError(0,_S*L":ParseDEF: '%?'" %source);
  }

extern "C" HRESULT __stdcall DLL_ParseDEF(BSTR source,long flags,IXdata** node)
  {
    __synchronize__
      return SpriteCraft_ParseDEF(source,flags,node);
  }

extern "C" HRESULT __stdcall DLL_CreateXML(BSTR tag,IXdata **pI)
  {
    __synchronize__
      return SpriteCraft_NewXML(tag,pI);
  }

XdataAttributes::XdataAttributes(Xdocument const &doc,Xnode *node,BSTR tag)
  {
    //__synchronize__
      {
        doc_ = doc;
        node->Link(&node_);
        i_ = -1;
        names_.Reserve(16);
        Xparam const *p = node->GetFirstParam();
        for ( int n = 0; p; p = p->next )
          {
            names_.Append(p);
          }
      }
  }

XdataAttributes::~XdataAttributes() {}

HRESULT __stdcall XdataAttributes::Reset(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    i_ = -1;
    *pVal = names_.Count() ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall XdataAttributes::Next(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( i_ + 1 >= names_.Count() )
      *pVal = VARIANT_FALSE;
    else
      ++i_, *pVal = VARIANT_TRUE;
    return S_OK;
  }

HRESULT __stdcall XdataAttributes::get_name(BSTR *pI)
  {
    _SYNCHRONIZE_;
    *pI = 0;
    if ( i_ < 0 ) { VARIANT_BOOL has; Next(&has); }
    if ( i_ >= 0 && node_ && i_ < names_.Count() )
      *pI = SysAllocString(names_[i_]->name);
    return S_OK;
  }

HRESULT __stdcall XdataAttributes::get_value(BSTR *pI)
  {
    _SYNCHRONIZE_;
    *pI = 0;
    if ( i_ < 0 ) { VARIANT_BOOL has; Next(&has); }
    if ( i_ >= 0 && node_ && i_ < names_.Count() )
      *pI = SysAllocString(+names_[i_]->value);
    return S_OK;
  }

void * __stdcall XdataAttributes::__python_iterate_next()
  {
    VARIANT_BOOL b;
    __synchronize__
      if ( !Next(&b) && b )
        {
          PyObject *o = _PoC_Tuple_New(2);
          _PoC_Tuple_SetItem(o,0,_PoC_Unicode_FromUnicode1(+names_[i_]->name));
          _PoC_Tuple_SetItem(o,1,_PoC_Unicode_FromUnicode1(+names_[i_]->value));
          return o;
        }
      else
        return _PoC_Err_StopIteration(), 0;
  }

int __stdcall XdataAttributes::__python_len()
  {
    __synchronize__
      return names_.Count();
  }

HRESULT __stdcall XdataAttributes::get_count(long *pVal)
  {
    __synchronize__
      return *pVal = names_.Count(), S_OK;
  }

void * __stdcall XdataEnumerator::__python_iterate_next()
  {
    VARIANT_BOOL b;
    __synchronize__
      if ( !Next(&b) && b )
        {
          rcc_ptr<IComPyUnknown> p (new XdataHelper( doc_, +nodes_[i_] ));
          return p->IComPyUnknown_Get(true);
        }
      else
        return _PoC_Err_StopIteration(), 0;
  }

int __stdcall XdataEnumerator::__python_len()
  {
    __synchronize__
      return nodes_.Count();
  }

HRESULT __stdcall XdataEnumerator::get_count(long *pVal)
  {
    __synchronize__
      return *pVal = nodes_.Count(), S_OK;
  }

HRESULT __stdcall XdataHelper::StoreTo(BSTR filename,long fmt)
  {
    __synchronize__
      {
        if ( !node_ )
          return ScraftError(ERROR_XNODE_DISPOSED);
        if ( fmt == XdataDEFformat )
          {
            if ( TEGGO_OK != teggo::XwriteDefDocument(+node_,filename,false) )
              return ScraftError("failed to write xdata");
          }
        else
          return ScraftError("unknown xdata format");
      }
    return S_OK;
  }

HRESULT __stdcall XdataHelper::Empty(VARIANT_BOOL *pVal)
  {
    __synchronize__
      if ( node_ )
        {
          *pVal = node_->Empty()?VARIANT_TRUE:VARIANT_FALSE;
          return S_OK;
        }
      else
        return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::Subtags(BSTR content,BSTR tag,IXdataEnumerator** pI)
  {
    __synchronize__
      if ( node_ )
        return (*pI = new XdataEnumerator(doc_,+node_,content,tag), S_OK);
      else
        return ScraftError(ERROR_XNODE_DISPOSED);
  }

HRESULT __stdcall XdataHelper::GetSubtag(BSTR name,BSTR tag,IXdata** pI)
  {
    return _GetSubtag(name,tag,pI,false);
  }

HRESULT __stdcall XdataHelper::GetSubtagNocase(BSTR name,BSTR tag,IXdata** pI)
  {
    return _GetSubtag(name,tag,pI,true);
  }

HRESULT __stdcall XdataHelper::_GetSubtag(BSTR name,BSTR tag,IXdata** pI,bool nocase)
  {
    __synchronize__
      {
        *pI = 0;
        if ( node_ )
          {
            if ( !tag || !*tag )
              tag = 0;
            else
              if ( !(tag = (BSTR)node_->GetDocument()->LookupLiteral(tag)) )
                return S_OK;
            Xiterator i = node_->Iterate();
            while ( i.Next() )
              {
                if ( tag && i->GetName() != tag ) continue;
                if ( !tag && teggo::StrStartWith(i->GetName(),L"<?") ) continue;
                if ( pwide_t cont = i->GetContent() )
                  if ( nocase )
                    {
                      if ( !wcsicmp(name,cont) )
                        return *pI = new XdataHelper(doc_,+i), S_OK;
                    }
                  else
                    {
                      if ( !wcscmp(name,cont) )
                        return *pI = new XdataHelper(doc_,+i), S_OK;
                    }
              }
          }
        else
          return ScraftError(ERROR_XNODE_DISPOSED);
      }
    return S_OK;
  }

void *  __stdcall XdataHelper::__python_iterate()
  {
    __synchronize__
      {
        rcc_ptr<IXdataEnumerator> pI;

        if ( SUCCEEDED( this->Tags(0,&+pI)) )
          {
            rcc_ptr<IComPyUnknown> py_o;
            pI->QueryInterface(TEGGO_GUIDOF(IComPyUnknown),(void**)&+py_o);
            return (PyObject*)py_o->IComPyUnknown_Get(true);
          }
        else
          return _PoC_RaiseComExcept(),0;
      }
  }
