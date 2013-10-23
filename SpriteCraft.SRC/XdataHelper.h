
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

#if !defined ___7a765092_4126_41a9_9ca4_7e8085c82679___
#define ___7a765092_4126_41a9_9ca4_7e8085c82679___

HRESULT __stdcall PyCreateXdata_(IXdata **xdata,PyObject*,PyObject*);

struct IXdata_DispImpl : TeggoPyDispImpl< IXdata, (PythonOverCom_Make3)PyCreateXdata_, TeggoSupportEInfo<IXdata>  >
  {
    typedef TeggoPyDispImpl< IXdata, (PythonOverCom_Make3)PyCreateXdata_, TeggoSupportEInfo<IXdata>  > _SuperImpl;
    HRESULT InternalQueryInterface(REFIID riid ,void ** pI)
      {
        return InternalQueryInterface1(riid,pI);
      }

    virtual HRESULT InternalQueryInterface1(REFIID riid ,void ** pI) = 0;
  };

struct XdataHelper : TeggoObject<IXdata_DispImpl>
  {
    typedef GuidT<0xC39FC83A,0xFBDF,0x4BA6,0xB4,0xB7,0xAB,0x67,0xAB,0xE3,0xCF,0x13> Guid;

    Xdocument        doc_;
    WeakLink<Xnode*> node_;

    HRESULT InternalQueryInterface1(REFIID riid ,void ** pI);

    XdataHelper(Xdocument const &, Xnode *);
    virtual ~XdataHelper();
    Xnode *GetRawNode() { return +node_; }

    HRESULT __stdcall GetSrcNameAsPWIDE(wchar_t **pVal);
    HRESULT __stdcall GetSrcName(BSTR *pVal);
    HRESULT __stdcall GetRoot(IXdata **pI);
    HRESULT __stdcall GetParent(IXdata **pI);
    HRESULT __stdcall Clone(IXdata **pI);
    HRESULT __stdcall Freeze();
    HRESULT __stdcall GetStrAttrAsPWIDE(BSTR name,wchar_t **pVal);
    HRESULT __stdcall GetContentAsPWIDE(wchar_t **pVal);
    HRESULT __stdcall GetNameAsPWIDE(wchar_t **pVal);
    HRESULT __stdcall Erase();
    HRESULT __stdcall Insert(BSTR name, IXdata **pI);
    HRESULT __stdcall InsertCopyOf(IXdata *node, VARIANT_BOOL req, IXdata **pI);
    HRESULT __stdcall Tags(BSTR tag, IXdataEnumerator **pI);
    HRESULT __stdcall GetContent(BSTR *pVal);
    HRESULT __stdcall SetContent(BSTR newVal);
    HRESULT __stdcall GetName(BSTR *pVal);
    HRESULT __stdcall SetBoolAttr(BSTR name, VARIANT_BOOL newVal);
    HRESULT __stdcall GetBoolAttr(BSTR name, VARIANT_BOOL *pVal);
    HRESULT __stdcall SetFltAttr(BSTR name, float newVal);
    HRESULT __stdcall GetFltAttr(BSTR name, float *pVal);
    HRESULT __stdcall SetIntAttr(BSTR name, long newVal);
    HRESULT __stdcall GetIntAttr(BSTR name, long *pVal);
    HRESULT __stdcall SetStrAttr(BSTR name, BSTR newVal);
    HRESULT __stdcall GetStrAttr(BSTR name, BSTR *pVal);
    HRESULT __stdcall GetTag(BSTR name, IXdata **pI);
    HRESULT __stdcall GetCount(long *pVal);
    HRESULT __stdcall GetCountTag(BSTR tag, long *pVal);
    HRESULT __stdcall StoreTo(BSTR filename,long fmt);
    HRESULT __stdcall HasAttr(BSTR name, VARIANT_BOOL *pVal);
    HRESULT __stdcall Attributes(IXdataAttributes **pI);
    HRESULT __stdcall Next(BSTR tag,IXdata **pI);

// compatibility
    HRESULT __stdcall StoreToXML(BSTR filename,long) { return StoreTo(filename,XdataXMLformat); }
    HRESULT __stdcall StoreToDEF(BSTR filename,long) { return StoreTo(filename,XdataDEFformat); }
    HRESULT __stdcall IterateTag(BSTR tag, IXdataEnumerator **pI) { return Tags(tag,pI); }
    HRESULT __stdcall Iterate(IXdataEnumerator **pI) { return Tags(0,pI); }

    HRESULT __stdcall Subtags(BSTR content,BSTR tag,IXdataEnumerator** pI);
    HRESULT __stdcall Empty(VARIANT_BOOL *pVal);
    HRESULT __stdcall GetSubtag(BSTR name,BSTR tag,IXdata** pI);
    HRESULT __stdcall GetSubtagNocase(BSTR name,BSTR tag,IXdata** pI);
    HRESULT __stdcall _GetSubtag(BSTR name,BSTR tag,IXdata** pI,bool);

    void *  __stdcall __python_iterate();
  };

struct XdataEnumerator :
  TeggoObject<
    TeggoPyDispImpl< IXdataEnumerator, 0, TeggoISupportErrorInfo > >
  {
    Xdocument doc_;
    BufferT< WeakLink<Xnode*> > nodes_;
    int i_;

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid)
      {
        return IsEqualGUID(TEGGO_GUIDOF(IXdataEnumerator),riid)?S_OK:S_FALSE;
      }

    XdataEnumerator(Xdocument const &root, Xnode *node, BSTR tag=0);
    XdataEnumerator(Xdocument const &root, Xnode *node, pwide_t cont, pwide_t tag);
    virtual ~XdataEnumerator();

    HRESULT __stdcall get_count(long *pVal);
    HRESULT __stdcall Get(IXdata **pI);
    HRESULT __stdcall Next(VARIANT_BOOL *pVal);
    HRESULT __stdcall Reset(VARIANT_BOOL *pVal);
    void *  __stdcall __python_iterate_next();
    int     __stdcall __python_len();
  };

struct XdataAttributes :
  TeggoObject<
    TeggoPyDispImpl< IXdataAttributes, 0, TeggoISupportErrorInfo > >
  {
    Xdocument doc_;
    WeakLink<Xnode *> node_;
    BufferT<Xparam const*> names_;
    int i_;

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid)
      {
        return IsEqualGUID(TEGGO_GUIDOF(IXdataAttributes),riid)?S_OK:S_FALSE;
      }

    XdataAttributes(Xdocument const &root, Xnode *node, BSTR tag=0);
    virtual ~XdataAttributes();

    HRESULT __stdcall get_value(BSTR *retVal);
    HRESULT __stdcall get_name(BSTR *retVal);
    HRESULT __stdcall get_count(long *pVal);
    HRESULT __stdcall __comfunc_Value(BSTR *retVal) { return get_value(retVal); }
    HRESULT __stdcall __comfunc_Name(BSTR *retVal)  { return get_name(retVal); }
    //HRESULT __stdcall _python_Get(void*,void*,void**);
    HRESULT __stdcall Next(VARIANT_BOOL *pVal);
    HRESULT __stdcall Reset(VARIANT_BOOL *pVal);
    void *  __stdcall __python_iterate_next();
    int     __stdcall __python_len();
  };

#endif // ___7a765092_4126_41a9_9ca4_7e8085c82679___
