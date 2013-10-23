
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

#if !defined __SpriteCraft_Impl_scXMLite_h__
#define __SpriteCraft_Impl_scXMLite_h__

#include "xdata.h"

struct IXdata_DispImpl : TeggoPyDispImpl< IXdata, 0, TeggoSupportEInfo<IXdata>  >
  {
    typedef TeggoPyDispImpl< IXdata, 0, TeggoSupportEInfo<IXdata>  > _SuperImpl;
    HRESULT InternalQueryInterface(REFIID riid ,void ** pI)
      {
        return InternalQueryInterface1(riid,pI);
      }
    
    virtual HRESULT InternalQueryInterface1(REFIID riid ,void ** pI) = 0;
  };

struct XMLNodeHelper : TeggoObject<IXdata_DispImpl>
  {
    typedef GuidT<0xC39FC83A,0xFBDF,0x4BA6,
                  0xB4,0xB7,0xAB,0x67,0xAB,0xE3,0xCF,0x13> Guid;

    auto_ref<XdataROOT> root_;
    XdataNODE*          node_;

    HRESULT InternalQueryInterface1(REFIID riid ,void ** pI);

    XMLNodeHelper(XdataROOT*,XdataNODE*);
    virtual ~XMLNodeHelper();
    XdataNODE* GetRawNode() 
      { 
        return node_; 
      }

	  HRESULT __stdcall GetSrcNameAsPWIDE(/*[out,retval]*/ wchar_t** pVal);
	  HRESULT __stdcall GetSrcName(/*[out,retval]*/ BSTR* pVal);
	  HRESULT __stdcall GetRoot(/*[out,retval]*/ IXdata** pI);
	  HRESULT __stdcall Clone(/*[out,retval]*/ IXdata** pI);
	  HRESULT __stdcall Freeze();
	  HRESULT __stdcall GetStrAttrAsPWIDE(BSTR name,/*[out,retval]*/ wchar_t** pVal);
	  HRESULT __stdcall GetContentAsPWIDE(/*[out,retval]*/ wchar_t** pVal);
	  HRESULT __stdcall GetNameAsPWIDE(/*[out,retval]*/ wchar_t** pVal);
	  HRESULT __stdcall Erase();
	  HRESULT __stdcall Insert(/*[in]*/ BSTR name,/*[out,retval]*/ IXdata** pI);
	  HRESULT __stdcall InsertCopyOf(/*[in]*/ IXdata* node,/*[out,retval]*/ IXdata** pI);
  //	HRESULT __stdcall QueryValue(/*[in]*/ BSTR path,/*[in]*/ BSTR attr,/*[out,retval]*/ BSTR* pVal);
	  HRESULT __stdcall IterateTag(/*[in]*/ BSTR path,/*[out,retval]*/ IXdataIterator** pI);
	  HRESULT __stdcall Iterate(/*[out,retval]*/ IXdataIterator** pI);
	  HRESULT __stdcall GetContent(/*[out, retval]*/ BSTR *pVal);
	  HRESULT __stdcall SetContent(/*[in]*/ BSTR newVal);
	  HRESULT __stdcall GetName(/*[out, retval]*/ BSTR *pVal);
	  HRESULT __stdcall SetBoolAttr(BSTR name, /*[in]*/ VARIANT_BOOL newVal);
	  HRESULT __stdcall GetBoolAttr(BSTR name, /*[out, retval]*/ VARIANT_BOOL *pVal);
	  HRESULT __stdcall SetFltAttr(BSTR name, /*[in]*/ float newVal);
	  HRESULT __stdcall GetFltAttr(BSTR name, /*[out, retval]*/ float *pVal);
	  HRESULT __stdcall SetIntAttr(BSTR name, /*[in]*/ long newVal);
	  HRESULT __stdcall GetIntAttr(BSTR name, /*[out, retval]*/ long *pVal);
	  HRESULT __stdcall SetStrAttr(BSTR name, /*[in]*/ BSTR newVal);
	  HRESULT __stdcall GetStrAttr(BSTR name, /*[out, retval]*/ BSTR *pVal);
	  HRESULT __stdcall GetTag(/*[in]*/ BSTR name,/*[out,retval]*/ IXdata** pI);
	  HRESULT __stdcall GetCount(/*[out, retval]*/ long *pVal);
	  HRESULT __stdcall GetCountTag(/*[in]*/ BSTR tag,/*[out, retval]*/ long *pVal);
    HRESULT __stdcall StoreToXML(BSTR filename,long encoding);
    HRESULT __stdcall StoreToDEF(BSTR filename,long encoding);
	  HRESULT __stdcall HasAttr(BSTR name, /*[out, retval]*/ VARIANT_BOOL *pVal);
  };

struct XMLNodeIterator :
  TeggoObject< 
    TeggoPyDispImpl< IXdataIterator, 0, TeggoISupportErrorInfo > >
  {
    auto_ref<XdataROOT> root_;
    XdataNODE*          node_;
    mem_ptr<XdataITER>  iter_;

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid)
      {
        return IsEqualGUID(TEGGO_GUIDOF(IXdataIterator),riid)?S_OK:S_FALSE;
      }

    XMLNodeIterator(XdataROOT* root,XdataNODE* node,pwide_t tag=0);
    virtual ~XMLNodeIterator();

	  HRESULT __stdcall Get(/*[out,retval]*/ IXdata** pI);
	  HRESULT __stdcall Next(/*[out,retval]*/ VARIANT_BOOL* pVal);
	  HRESULT __stdcall Reset(/*[out,retval]*/ VARIANT_BOOL* pVal);
  };

#endif // __SpriteCraft_Impl_scXMLite_h__
