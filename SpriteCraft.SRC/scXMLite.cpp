
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

#include "../MoleSkin2/mUTIL/m_xml.h"
#include "scXMLite.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$XML/DEF")
#endif

#define NASCE_PRESERVE 64
#define PATH_PRESERVE 128

XMLNodeHelper::XMLNodeHelper(XdataROOT* root,XdataNODE* node)
  {
    root_.reassign__(Grab(root));
    node_ = node;
  }

XMLNodeHelper::~XMLNodeHelper()
  {
  }

HRESULT XMLNodeHelper::InternalQueryInterface1(REFIID riid ,void ** pI) 
  { 
    if ( IsEqualGUID(TEGGO_GUIDOF(XMLNodeHelper),riid) ) 
      return *pI = Refe(this), S_OK; 
    else
      return _SuperImpl::InternalQueryInterface(riid,pI);
  }

XMLNodeIterator::XMLNodeIterator(XdataROOT* root,XdataNODE* node,pwide_t tag)
  {
    iter_ = MemPtr( new XdataITER );
    root_.reassign__(Grab(root));
    node_ = node;
    *iter_ = node_->Iterate(tag);
    //Reset(0);
  }

XMLNodeIterator::~XMLNodeIterator()
  {
  }

HRESULT __stdcall XMLNodeHelper::GetTag(BSTR bstr_name, IXdata **pI)
  {
    _SYNCHRONIZE_;
    *pI = 0;
    XdataNODE* i = node_->GetTag( bstr_name );
    if ( i ) *pI = new XMLNodeHelper(root_,i);
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetStrAttr(BSTR bstr_name, BSTR *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SysAllocString( node_->GetAttr(bstr_name) );
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetIntAttr(BSTR bstr_name, long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = teggo::ToLong( node_->GetAttr(bstr_name) );
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetFltAttr(BSTR bstr_name, float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = teggo::ToFloat( node_->GetAttr(bstr_name) );
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetBoolAttr(BSTR bstr_name, VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = teggo::ToBool( node_->GetAttr(bstr_name) );
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::HasAttr(BSTR bstr_name, VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = node_->GetAttr(bstr_name) ? VARIANT_TRUE : VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetName(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SysAllocString( node_->GetName() );
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetContent(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SysAllocString( node_->GetContent() );
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetCount(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = node_->GetCountTag(0);
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetCountTag(BSTR tag, long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = node_->GetCountTag(tag);
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::Iterate(IXdataIterator **pI)
  {
    _SYNCHRONIZE_;
    *pI = new XMLNodeIterator(root_,node_);
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::IterateTag(BSTR bstr_tag, IXdataIterator **pI)
  {
    _SYNCHRONIZE_;
    *pI = new XMLNodeIterator(root_,node_,bstr_tag);
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::StoreToXML(BSTR filename,long encoding)
  {
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::StoreToDEF(BSTR filename,long encoding)
  {
    _SYNCHRONIZE_;

    ResetErrorLog();
    if ( SCE_OK != StoreDEFTo( filename, encoding, node_ ) )
      return GenSCWideError(0,_S*L":StoreToDEF: '%?'" %filename);

    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::SetBoolAttr(BSTR name,  VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(
                _S*L"could not change attribute '%?', document freezed" %name);
    node_->SetAttr(name,newVal!=VARIANT_FALSE?L"true":L"false");
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::SetFltAttr(BSTR name,  float newVal)
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(
                _S*L"could not change attribute '%?', document freezed" %name);
    node_->SetAttr(name,teggo::Ftow(newVal));
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::SetIntAttr(BSTR name,  long newVal)
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(
                _S*L"could not change attribute '%?', document freezed" %name);
    node_->SetAttr(name,teggo::Itow(newVal));
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::SetStrAttr(BSTR name,  BSTR newVal)
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(
                _S*L"could not change attribute '%?', document freezed" %name);
    node_->SetAttr(name,newVal);
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::SetContent(BSTR newVal)
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(L"could not change node content, document freezed");
    node_->SetContent(newVal);
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::Erase()
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(L"could not erase node, document freezed");
    node_->Erase();
    node_ = 0;
    MSKINSPACE::Release(root_);
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::Insert(BSTR name, IXdata** pI)
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(
                _S*L"could not insert subnode '%?', document freezed" %name);
    *pI = new XMLNodeHelper( root_, node_->Insert(name) );
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::InsertCopyOf(IXdata* node,IXdata** pI)
  {
    _SYNCHRONIZE_;
    if ( +root_ && root_->freezed_ ) 
      return GenSCWideError(L"could not insert subnode, document freezed");
    XMLNodeHelper* helper = 0;
    node->QueryInterface( TEGGO_GUIDOF(XMLNodeHelper), (void**)&helper );
    *pI = new XMLNodeHelper( root_, node_->InsertCopyOf(helper->GetRawNode()) );
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetNameAsPWIDE(wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    *pVal = (wchar_t*)node_->GetName();
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetContentAsPWIDE(wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    *pVal = (wchar_t*)node_->GetContent();
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetStrAttrAsPWIDE(BSTR name,wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    *pVal = (wchar_t*)node_->GetAttr(name);
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::Freeze()
  {
    _SYNCHRONIZE_;
    if ( +root_ ) root_->freezed_ = true;
	  return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::Clone(IXdata **pI)
  {
    _SYNCHRONIZE_;
    XdataROOT* root = new XdataROOT;
    *pI = new XMLNodeHelper( root, root->GetNode()->InsertCopyOf(node_,true) );
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetRoot(IXdata **pI)
  {
    _SYNCHRONIZE_;
    *pI = new XMLNodeHelper( root_, root_->GetNode() );
	  return S_OK;
  }


HRESULT __stdcall XMLNodeHelper::GetSrcName(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SysAllocString(root_->GetNode()->GetAttr(L"$/source"));
    return S_OK;
  }

HRESULT __stdcall XMLNodeHelper::GetSrcNameAsPWIDE(wchar_t **pVal)
  {
    _SYNCHRONIZE_;
    *pVal = (wchar_t*)root_->GetNode()->GetAttr(L"$/source");
	  return S_OK;
  }

HRESULT __stdcall XMLNodeIterator::Reset(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = iter_->Reset() ? VARIANT_TRUE : VARIANT_FALSE;
	  return S_OK;
  }

HRESULT __stdcall XMLNodeIterator::Next(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = iter_->Next() ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall XMLNodeIterator::Get(IXdata **pI)
  {
    _SYNCHRONIZE_;
    XdataNODE* p = iter_->Get();
    if ( p ) *pI = new XMLNodeHelper(root_,p);
    else *pI = 0;
	  return S_OK;
  }

HRESULT SpriteCraft_ParseDEF(pwide_t source,long flags,IXdata** node)
  {
    _SYNCHRONIZE_;

    ResetErrorLog();

    auto_ref<XdataROOT> xmlroot( ParseDEFFrom(source,flags) );
    if ( +xmlroot ) 
      {
        rcc_ptr<XMLNodeHelper> helper( new XMLNodeHelper(xmlroot,xmlroot->GetNode()) );
        *node = teggo::Forget(helper);
        return S_OK;
      }
    return GenSCWideError(0,_S*L":ParseDEF: '%?'" %source);
  }

extern "C" HRESULT __stdcall DLL_ParseDEF(BSTR source,long flags,IXdata** node)
  {
    return SpriteCraft_ParseDEF(source,flags,node);
  }

HRESULT SpriteCraft_NewXML(pwide_t tag, IXdata** pI)
  {
    _SYNCHRONIZE_;
    auto_ref<XdataROOT> root ( new XdataROOT );
    XdataNODE* base = root->GetNode();
    *pI = new XMLNodeHelper( +root, base->Insert(tag) );
	  return S_OK;
  }

extern "C" HRESULT __stdcall DLL_CreateXML(BSTR tag,IXdata **pI) 
  {
    return SpriteCraft_NewXML(tag,pI);
  }
