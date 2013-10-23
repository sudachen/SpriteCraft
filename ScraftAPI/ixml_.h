
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

struct ScraftIXdata
  {
    IXdata*        QueryIfs() { return (IXdata*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }
    IXdataPtr      GetTag(pwide_t);
    IXdataPtr      GetTagSafe(pwide_t);
    IXdataIterPtr  Iterate();
    IXdataIterPtr  IterateTag(pwide_t);
    long           GetIntAttr(pwide_t attr);
    float          GetFltAttr(pwide_t attr);
    bool           GetBoolAttr(pwide_t attr);
    __ScString     GetName();
    __ScString     GetContent();
    __ScString     GetStrAttr(pwide_t attr);
    __ScString     GetSrcName();
    unsigned       Count();
    unsigned       CountTag(pwide_t tag);
    pwide_t        GetNameAsPWIDE();
    pwide_t        GetContentAsPWIDE();
    pwide_t        GetStrAttrAsPWIDE(pwide_t attr);
    pwide_t        GetSrcNameAsPWIDE();
    long           GetLineNo();
    long           GetAttrLineNo(pwide_t attr);
    void           Freeze();
    IXdataPtr      Clone();
    IXdataPtr      GetRoot();
    void           SetContent(pwide_t cont);
    void           SetStrAttr(pwide_t attr,pwide_t val);
    void           SetIntAttr(pwide_t attr,long val);
    void           SetFltAttr(pwide_t attr,float val);
    void           SetBoolAttr(pwide_t attr,bool val);
    void           StoreToXML(pwide_t fname, long encoding = 0);
    void           StoreToDEF(pwide_t fname, long encoding = 0);
    IXdataPtr      Insert(pwide_t tag);
    IXdataPtr      InsertCopyOf(IXdataPtr const &);
    void           Erase();

  };

inline IXdata* scraft_query_ifs(ScraftIXdata* u)
  {
    return u->QueryIfs();
  }

struct ScraftIXdataIter
  {
//    typedef teggo::GuidT<0x7A914D61,0x5AEF,0x4fda,0xB7,0x97,0x11,0xCA,0xA4,0x02,0x4E,0x22> Guid;

    IXdataIterator* QueryIfs() { return (IXdataIterator*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }
    bool           Reset();
    bool           Next();
    IXdataPtr      Get();
  };

typedef ScraftIXdataIter IXdataIter;

inline IXdataIterator* scraft_query_ifs(ScraftIXdataIter* u)
  {
    return u->QueryIfs();
  }
