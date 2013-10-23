
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

#include "./python_over_com_intern.h"
#include "../synchronize.h"

void __stdcall TeggoRccTaits<PyObjectWrapper_>::Refcounter::
  Release(PyObjectWrapper_ *p)
  {
    Py_XDECREF((PyObject*)p);
  }

void __stdcall TeggoRccTaits<PyObjectWrapper_>::Refcounter::
  AddRef(PyObjectWrapper_ *p)
  {
    Py_XINCREF((PyObject*)p);
  }

PyObjectWrapper_ * CXX_STDCALL PyObjectWrapper_New(PyTypeInfo_ const &pytype, IUnknown *u)
  {
    PyObjectWrapper_ *pyobj = (PyObjectWrapper_*)pytype.pytype.tp_alloc((PyTypeObject*)&pytype.pytype,0);
    pyobj->this_ = Refe(u);
    return pyobj;
  }

void Py_Object_Release_(PyObject *self)
  {
    PyObjectWrapper_ *p = (PyObjectWrapper_*)self;
    if ( p->this_ )
      {
        rcc_ptr<IComPyUnknown> py_o;
        REQ_S_OK( p->this_->QueryInterface(TEGGO_GUIDOF(IComPyUnknown),(void**)&+py_o) );
        py_o->IComPyUnknown_Unlink();
        Unrefe(p->this_);
      }
  }

PyObject *Py_Object_Release(PyObject *self, PyObject *argv)
  {
    Py_Object_Release_(self);
    return Py_INCREF(_PoC_None),
                     _PoC_None;
  }

void _PoC_CALL Py_Object_Dealloc(PyObject *self)
  {
    Py_Object_Release_(self);
    self->ob_type->tp_free(self);
  }

struct PyInvoke_Frame
  {
    enum     {STACKSZ = 256};
    byte     data[256];
    byte     retval[8];
    unsigned rettype;
    struct   { unsigned no : 24; unsigned popcount : 8; };
    void     *retaddr;
    PyObject *self;
    PyObject *argv;
    void Push(unsigned *sp, void *val, unsigned n)
      {
        STRICT_REQUIRE(*sp >= n);
        *sp -= n;
        memcpy((data+*sp),val,n);
      }
    void PushP(unsigned *sp, void *val)
      {
        Push(sp,&val,sizeof(val));
      }
    void Push2i(unsigned *sp,short val)
      {
        memset(data+(*sp-=2),0,2);
        Push(sp,&val,2);
      }
    void Push4i(unsigned *sp,long val )
      {
        Push(sp,&val,sizeof(val));
      }
    void Push4f(unsigned *sp,float val )
      {
        Push(sp,&val,sizeof(val));
      }
    void Push8f(unsigned *sp,double val )
      {
        Push(sp,&val,sizeof(val));
      }
  };

struct PyInvoke_Frame_Rv
  {
    byte     retval[8];
    unsigned rettype;
    struct   { unsigned no : 24; unsigned popcount : 8; };
    void     *retaddr;
    PyObject *self;
    PyObject *argv;
  };

struct PyJamper_
  {
    enum { JAMPER_COUNT = 256 };
    byte_t data[16];
    void Init(long no)
      {
        memset(data,0x90,sizeof(data));
        data[0] = 0xb8;
        *(long*)(data+1) = no;
        //data[5] = 0xcc;
        data[8] = 0xe9;
        *(long*)(data+9) = (long)&PyJamper_::Invoker-((long)data+5+8);
      }
    static void __cdecl Invoker();
  };

IUnknown *_Py_Default_Object_Marshal(PyObject *o)
  {
    PyObjectWrapper_ *p = (PyObjectWrapper_*)o;
    return p->this_;
  }

Py_Object_Marshal _Py_Find_Object_Marshal( PyObject *o )
  {
    PyTypeObject *tp = o->ob_type;

    if ( tp->tp_dealloc == Py_Object_Dealloc )
      return _Py_Default_Object_Marshal;
    else
      return PyTypeInfoFactory().FindObjectMarshal(tp);
  }

bool _Py_Push_TypedObject(
  unsigned /*TYPE_ANALOG*/ type,unsigned *sp, PyInvoke_Frame *fr, PyObject *o)
  {
    switch(type)
      {
      case TP_LONG:
      case TP_SHORT:
        {
         long val = 0;
          if ( o->ob_type == _PoC_Int_Type )
            val = PyInt_AS_LONG(o);
          else if ( o->ob_type == _PoC_Float_Type )
            val = (long)PyFloat_AS_DOUBLE(o);
          else
            return _PoC_RaiseTypeExcept("bad type, should be integer"), false;
          if ( type == TP_LONG )
            fr->Push4i(sp,val);
          else
            fr->Push2i(sp,(short)val);
        }
        break;
      case TP_SINGLE:
      case TP_DOUBLE:
        {
          double val = 0;
          if ( o->ob_type == _PoC_Int_Type )
            val = (double)PyInt_AS_LONG(o);
          else if ( o->ob_type == _PoC_Float_Type )
            val = PyFloat_AS_DOUBLE(o);
          else
            return _PoC_RaiseTypeExcept("bad type, should be float"), false;
          if ( type == TP_SINGLE )
            fr->Push4f(sp,val);
          else
            fr->Push8f(sp,val);
        }
        break;
      case TP_BOOL:
        {
          unsigned short val = 0;
          if ( o->ob_type == _PoC_Int_Type )
            val = PyInt_AS_LONG(o);
          else if ( o->ob_type == _PoC_Bool_Type )
            val = (o == _PoC_True) ? 1 : 0;
          else
            return _PoC_RaiseTypeExcept("bad type, should be boolean"), false;
          fr->Push2i(sp,val?VARIANT_TRUE:VARIANT_FALSE);
        }
        break;
      case TP_BSTR:
        {
          BSTR val = 0;
          if ( o->ob_type == _PoC_Unicode_Type )
            val = (wchar_t*)PyUnicode_AS_UNICODE(o);
          else if ( o->ob_type == _PoC_String_Type )
            val = _PoC_LocalString(PyString_AS_STRING(o));
          else
            return _PoC_RaiseExcept("bad type, should be unicode string"), false;
          fr->PushP(sp,val);
        }
        break;
      case TP_OBJECT:
        {
          if ( o == _PoC_None )
            {
              fr->PushP(sp,0);
            }
          else if ( Py_Object_Marshal marshal = _Py_Find_Object_Marshal(o) )
            {
              if ( IUnknown *o_this = marshal(o) )
                {
                  fr->PushP(sp,o_this);
                }
              else
                return _PoC_RaiseTypeExcept("dead object"), false;
            }
          else
            return _PoC_RaiseTypeExcept("bad type, should be wrapped object"), false;
        }
        break;
      }
    return true;
  }

unsigned __stdcall _Py_InitFrame(PyInvoke_Frame *fr)
  {
    unsigned sp = PyInvoke_Frame::STACKSZ;
    PyTupleObject *pto = (PyTupleObject *)(fr->argv);
    PyTypeInfo_  *pti = (PyTypeInfo_ *)fr->self->ob_type;
    PyDispFnDef_ *pdf = &pti->funcd[fr->no];
    fr->rettype = pdf->rettype;
    memset(fr->retval,0,sizeof(fr->retval));
    unsigned ptocnt = 0;
    unsigned i = pdf->argv.Count();

    if (!pdf->getset)
      {
        ptocnt = PyTuple_GET_SIZE(pto);
      }
    else if ( pdf->getset == PyDispFnDef_::IS_SETTER )
      {
        fr->popcount = 1;
        ptocnt = 1;
        REQUIRE(i==ptocnt);
      }

    for ( ; i > ptocnt ; )
      {
        --i;
        PyDispFnDef_::ArgDef &a = pdf->argv[i];
        if ( a.flags & PARAMFLAG_FHASDEFAULT )
          {
            VARIANT *val = &pti->defaults[a.defultno];
            switch ( a.type )
              {
              case TP_DOUBLE: // 8
                fr->Push8f(&sp,val->dblVal);
                break;
              case TP_SINGLE: // 4
              case TP_LONG:
              case TP_OBJECT:
              case TP_BSTR:
                fr->Push(&sp,&val->lVal,4);
                break;
              case TP_SHORT: // 2
              case TP_BOOL:
                fr->Push2i(&sp,val->lVal);
                break;
              default:
                EMERGENCY(L"invalid type");
              }
          }
        else
          {
            //STRICT_REQUIRE ( a.flags & PARAMFLAG_FRETVAL );
            if ( !(a.flags& PARAMFLAG_FRETVAL) )
              return _PoC_RaiseTypeExcept("not enough arguments"),
                      0x80000000;
            fr->PushP(&sp,&fr->retval[0]);
          }
      }

    if ( !pdf->getset )
      {
        for ( ; i > 0;  )
          {
            --ptocnt;
            --i;
            PyObject *o = PyTuple_GET_ITEM(pto,ptocnt);
            PyDispFnDef_::ArgDef &a = pdf->argv[i];
            if ( a.flags & PARAMFLAG_FRETVAL )
              {
                fr->PushP(&sp,&fr->retval[0]);
              }
            else
              if ( !_Py_Push_TypedObject(a.type,&sp,fr,o) )
                return (fr->no = (pdf->getset ? -1 : 0)), 0x80000000;
          }
      }
    else if ( pdf->getset == PyDispFnDef_::IS_SETTER )
      {
        STRICT_REQUIRE(pdf->argv.Count() == 1);
        PyDispFnDef_::ArgDef &a = pdf->argv[0];
        if ( !_Py_Push_TypedObject(a.type,&sp,fr,fr->argv) )
          return (fr->no = (pdf->getset ? -1 : 0)), 0x80000000;
        --i;
      }

    IUnknown *this_ = ((PyObjectWrapper_*)fr->self)->this_;
    fr->PushP(&sp,this_);
    fr->PushP(&sp,(*(void***)this_)[pdf->vfto>>2]);
    return sp;
  }

PyObject * __cdecl _Py_MakeRetVal(PyInvoke_Frame_Rv *fr)
  {
#     ifdef _DEBUG
    PyDispFnDef_ *pdf = &((PyTypeInfo_ *)fr->self->ob_type)->funcd[fr->no];
#     endif
    switch ( fr->rettype )
      {
      case TP_VOID:
        return Py_INCREF(_PoC_None), _PoC_None;
      case TP_BOOL:
        return *(VARIANT_BOOL*)fr->retval != 0
          ? (Py_INCREF(_PoC_True), _PoC_True) :
            (Py_INCREF(_PoC_False), _PoC_False);
      case TP_LONG:
        return _PoC_Int_FromLong(*(long*)fr->retval);
      case TP_SHORT:
        return _PoC_Int_FromLong(*(short*)fr->retval);
      case TP_SINGLE:
        return _PoC_Float_FromDouble(*(float*)fr->retval);
      case TP_DOUBLE:
        return _PoC_Float_FromDouble(*(double*)fr->retval);
      case TP_OBJECT:
        {
          rcc_ptr<IComPyUnknown> py_o;
          rcc_ptr<IUnknown> retval(*(IUnknown**)fr->retval);
          if ( !retval )
            return Py_INCREF(_PoC_None),
                             _PoC_None;
          else
            if ( SUCCEEDED( retval->QueryInterface(TEGGO_GUIDOF(IComPyUnknown),(void**)&+py_o) ) )
              return (PyObject*)py_o->IComPyUnknown_Get(true);
            else
              return
                _PoC_RaiseTypeExcept("unwrapped COM type"),
                0;
        }
      case TP_BSTR:
        {
          BSTR bstr_val = *(BSTR*)fr->retval;
          PyObject *ro;
          if ( bstr_val )
            {
              unsigned len = wcslen(bstr_val);
              ro = _PoC_Unicode_FromUnicode((Py_UNICODE*)bstr_val,len);
              SysFreeString(bstr_val);
            }
          else
            ro = _PoC_Unicode_FromUnicode(L"",0);
          return ro;
        }
      case TP_SETRETVAL:
        return 0;
      default:
        return
          _PoC_RaiseTypeExcept("unwrapped COM type"),
          0;
      }
    EMERGENCY(L"unreachable");
    return 0;
  }

unsigned __cdecl _Py_GenException(HRESULT hr,PyInvoke_Frame_Rv *fr)
  {
    PyDispFnDef_ *pdf = &((PyTypeInfo_ *)fr->self->ob_type)->funcd[fr->no];
    _PoC_RaiseComExcept();
    return pdf->getset ? -1 : 0;
  }

void __declspec(naked) __cdecl PyJamper_::Invoker()
  {
    __asm nop
    __asm push  eax
    __asm push  eax
    __asm push  eax
    __asm push  eax
    __asm sub   esp,256
    __asm push  esp
    __asm call  _Py_InitFrame
    __asm test  eax,0x80000000
    __asm jnz   lb_err1
    __asm add   esp,eax
    __asm pop   edx
    __asm call  edx
    __asm test  eax,0x80000000
    __asm jnz   lb_err
    __asm push  esp
    __asm call  _Py_MakeRetVal
    __asm add   esp,16
    __asm jmp   lb_ret
  lb_err:
    __asm push  esp
    __asm push  eax
    __asm call  _Py_GenException
    __asm add   esp,20
    __asm jmp   lb_ret
  lb_err1:
    __asm add   esp,256+4*3
    __asm mov   eax,0
  lb_ret:
    __asm add   esp, 4
    __asm ret
  }

void _Py_InitParamDef(PyTypeInfo_ *val,ELEMDESC const *el,PyDispFnDef_::ArgDef &a,teggo::TypeInfo_ *ti, bool retval)
  {
    TYPEDESC const *tdsc = &el->tdesc;
    a.flags = el->paramdesc.wParamFlags;
    a.isptr = 0;

    while ( tdsc->vt == VT_PTR )
      {
        ++a.isptr;
        tdsc = tdsc->lptdesc;
      }

    if ( tdsc->vt == VT_USERDEFINED )
      {
        rcc_ptr<ITypeInfo> tti;
        TYPEATTR *tta = 0;
        REQUIRE(S_OK == ti->info_->GetRefTypeInfo(tdsc->hreftype,&+tti));
        REQUIRE(S_OK == tti->GetTypeAttr(&tta));
        if ( tta->typekind == TKIND_ENUM )
          {
            a.type = TP_LONG;
          }
        else if ( tta->typekind == TKIND_RECORD )
          {
            a.type = TP_LONG;
          }
        else
          {
            a.type = TP_OBJECT;
          }
        tti->ReleaseTypeAttr(tta);
      }
    else if ( tdsc->vt == VT_I4 || tdsc->vt == VT_UI4 )
      a.type = TP_LONG;
    else if ( tdsc->vt == VT_INT || tdsc->vt == VT_UINT )
      a.type = TP_LONG;
    else if ( tdsc->vt == VT_I2 || tdsc->vt == VT_UI2 )
      a.type = TP_SHORT;
    else if ( tdsc->vt == VT_R4 )
      a.type = TP_SINGLE;
    else if ( tdsc->vt == VT_R8 )
      a.type = TP_DOUBLE;
    else if ( tdsc->vt == VT_BSTR )
      a.type = TP_BSTR;
    else if ( tdsc->vt == VT_BOOL )
      a.type = TP_BOOL;
    else if ( tdsc->vt == VT_DISPATCH || tdsc->vt == VT_UNKNOWN )
      a.type = TP_OBJECT;
    else
      EMERGENCY(L"invalid type");

    if ( (a.flags & PARAMFLAG_FHASDEFAULT) && !retval )
      {
        a.defultno = val->defaults.Count();
        VARIANT *x = val->defaults.Push();
        VariantInit(x);
        VariantCopy(x,&el->paramdesc.pparamdescex->varDefaultValue);
      }
    else
      {
        if ( retval )
          a.flags = PARAMFLAG_FRETVAL;
        a.defultno = 0xffff;
      }
  }

typedef PyObject *(_PoC_CALL *_Py_getter)(PyObject *, void *);
typedef int (_PoC_CALL *_Py_setter)(PyObject *, PyObject *, void *);

struct PyGetSetDef_Less
  {
    bool operator() ( PyGetSetDef const &a, PyGetSetDef const &b )
      {
        return strcmp(a.name,b.name) < 0;
      }
  };

PyObject * __cdecl PyIterateNext(PyObject *_self)
  {
    typedef PyObject * (__stdcall *__python_iterate_next_f)(IUnknown *);
    PyObjectWrapper_ *self = (PyObjectWrapper_*)_self;
    PyTypeInfo_ *pti = (PyTypeInfo_ *)self->ob_type;
    return ((__python_iterate_next_f)(*(void***)self->this_)[pti->iterate_next_oVft])(self->this_);
  }

PyObject * __cdecl PyIterate(PyObject *_self)
  {
    typedef PyObject * (__stdcall *__python_iterate_f)(IUnknown *);
    PyObjectWrapper_ *self = (PyObjectWrapper_*)_self;
    PyTypeInfo_ *pti = (PyTypeInfo_ *)self->ob_type;
    return ((__python_iterate_f)(*(void***)self->this_)[pti->iterate_oVft])(self->this_);
  }

PyObject * __cdecl PySelfIter(PyObject *_self)
  {
    return Refe(_self);
  }

int __cdecl PySqLen(PyObject *_self)
  {
    typedef int (__stdcall *__python_len_f)(IUnknown *);
    PyObjectWrapper_ *self = (PyObjectWrapper_*)_self;
    PyTypeInfo_ *pti = (PyTypeInfo_ *)self->ob_type;
    return ((__python_len_f)(*(void***)self->this_)[pti->sq_len_oVft])(self->this_);
  }

PyGetSetDef &PyGetSetDef_FindOrAdd(PyTypeInfo_ *val,pwide_t name)
  {
    asciistring ass(name);
    PyGetSetDef gsd ;
    gsd.name = (char*)ass.Str();

    if ( PyGetSetDef *g = val->pymembers.Bfind(gsd,PyGetSetDef_Less()) )
      return *g;
    else
      {
        gsd.name = (char*)val->names.Push(ass)->Str();
        gsd.doc = 0;
        gsd.closure = 0;
        gsd.set = 0;
        gsd.get = 0;
        return *val->pymembers.Binsert(gsd,PyGetSetDef_Less());
      }
  }

pchar_t MkPyMethod(PyTypeInfo_ *val,PyJamper_ *jamper, pchar_t prefix, pwide_t fname,unsigned defno)
  {
    teggo::StringT<char,64> name (fname);
    if ( prefix )
      {
        name[0] = teggo::ChrUpper( name[0] );
        name = prefix + name;
      }
    PyMethodDef &md = *val->pymethods.Push();
    md.ml_doc = 0;
    md.ml_flags = METH_VARARGS;
    md.ml_meth = (PyCFunction)(jamper+defno);
    md.ml_name = (char*)val->names.Push(name)->Str();
    return md.ml_name;
  }

void InitPyTypeInfoFromTypeInfo(PyTypeInfo_ *val,GUID const *iidGUID,PyJamper_ *jamper)
  {
    static PyTypeObject TYPE_FILLER =
      {
        PyObject_HEAD_INIT(NULL)
        0,                         /*ob_size*/
        "-",                       /*tp_name*/
        sizeof(PyObjectWrapper_),  /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        Py_Object_Dealloc,         /*tp_dealloc*/
        0,                         /*tp_print*/
        0,                         /*tp_getattr*/
        0,                         /*tp_setattr*/
        0,                         /*tp_compare*/
        0,                         /*tp_repr*/
        0,                         /*tp_as_number*/
        0,                         /*tp_as_sequence*/
        0,                         /*tp_as_mapping*/
        0,                         /*tp_hash */
        0,                         /*tp_call*/
        0,                         /*tp_str*/
        0,                         /*tp_getattro*/
        0,                         /*tp_setattro*/
        0,                         /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT /*| Py_TPFLAGS_BASETYPE*/, /*tp_flags*/
        "- external type -",       /* tp_doc */
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        (getiterfunc)0,            /* tp_iter */
        (iternextfunc)0,           /* tp_iternext */
        0,                         /* tp_methods */
        0,                         /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        0,                         /* tp_init */
        0,                         /* tp_alloc */
        0,                         /* tp_new */
      };

    val->pytype = TYPE_FILLER;

    teggo::TypeInfo_ *ti = teggo::TypeInfoFactory().GetTypeinfo(iidGUID);

    REQUIRE( SUCCEEDED(ti->EnsureTI(0,Hinstance)) );

    TYPEATTR *ta = 0;
    if ( SUCCEEDED( ti->info_->GetTypeAttr(&ta) ) )
      {
        unsigned meth_count = 0;
        unsigned memb_count = 0;
        {for ( int mno = 7; mno < ta->cFuncs; ++mno )
          {
            FUNCDESC *fdsc = 0;
            ti->info_->GetFuncDesc(mno,&fdsc);
            if ( fdsc->invkind == DISPATCH_PROPERTYPUT
            || fdsc->invkind == DISPATCH_PROPERTYGET )
              {
                ++memb_count;
              }
            else if ( fdsc->invkind == DISPATCH_METHOD )
              {
                ++meth_count;
              }
            ti->info_->ReleaseFuncDesc(fdsc);
          }}

        val->pymembers.Reserve(memb_count+1);
        val->pymethods.Reserve(meth_count+1+1);


        {for ( int mno = 7; mno < ta->cFuncs; ++mno )
          {
            FUNCDESC *fdsc = 0;
            teggo::bstr_ptr fname;
            ti->info_->GetFuncDesc(mno,&fdsc);
            ti->info_->GetDocumentation(fdsc->memid,&+fname,0,0,0);

            if ( fname[0] == L'_' || (fdsc->wFuncFlags & (FUNCFLAG_FHIDDEN|FUNCFLAG_FRESTRICTED)) )
              {
                if ( teggo::BaseStringT<wchar_t>::StartWith_(+fname,L"__comfunc_") )
                  {
                    int i = 0, j = 10;
                    for ( ; fname[j] != 0; ++i, ++j) fname[i] = fname[j];
                    fname[i] = 0;
                  }
                else
                  {
                    if ( teggo::BaseStringT<wchar_t>::StartWith_(+fname,L"__python_") )
                      {
                        if ( !wcscmp(fname+9,L"iterate_next") )
                          {
                            val->iterate_next_oVft = fdsc->oVft>>2;
                            val->pytype.tp_iternext = &PyIterateNext;
                            val->pytype.tp_iter = &PySelfIter;
                          }
                        else if ( !wcscmp(fname+9,L"iterate") )
                          {
                            val->iterate_oVft = fdsc->oVft>>2;
                            val->pytype.tp_iter = &PyIterate;
                          }
                        else if ( !wcscmp(fname+9,L"len") )
                          {
                            val->sq_len_oVft = fdsc->oVft>>2;
                            val->sq.sq_length = &PySqLen;
                            val->pytype.tp_as_sequence = &val->sq;
                          }
                      }
                    ti->info_->ReleaseFuncDesc(fdsc);
                    continue;
                  }
              }

            unsigned defno = val->funcd.Count();

            PyDispFnDef_ &mfnd = *val->funcd.Push();
            mfnd.vfto = fdsc->oVft;
            mfnd.argv.Reserve(fdsc->cParams);
            mfnd.getset = 0;
            mfnd.rettype = TP_VOID;

            if ( fdsc->invkind == DISPATCH_METHOD )
              {
                *+fname = towupper(*+fname);
                mfnd.name = MkPyMethod(val,jamper,0,+fname,defno);
              }
            else if ( fdsc->invkind == DISPATCH_PROPERTYGET )
              {
                if ( fdsc->cParams > 0 )
                  mfnd.name = MkPyMethod(val,jamper,"Get",+fname,defno);
                else
                  {
                    PyGetSetDef &gsd = PyGetSetDef_FindOrAdd(val,+fname);
                    gsd.get = (_Py_getter)(jamper+defno);
                    mfnd.getset = PyDispFnDef_::IS_GETTER;
                    mfnd.name = gsd.name;
                  }
              }
            else if ( fdsc->invkind == DISPATCH_PROPERTYPUT )
              {
                if ( fdsc->cParams > 1 )
                  mfnd.name = MkPyMethod(val,jamper,"Set",+fname,defno);
                else
                  {
                    PyGetSetDef &gsd = PyGetSetDef_FindOrAdd(val,+fname);
                    gsd.set = (_Py_setter)(jamper+defno);
                    mfnd.getset = PyDispFnDef_::IS_SETTER;
                    mfnd.rettype = TP_SETRETVAL;
                    mfnd.name = gsd.name;
                  }
              }
            else
              EMERGENCY(L"invalid invkind");

            for ( int ano = 0; ano < fdsc->cParams; ++ano )
              {
                PyDispFnDef_::ArgDef &a = *mfnd.argv.Push();
                ELEMDESC const *el = fdsc->lprgelemdescParam+ano;
                _Py_InitParamDef(val,el,a,ti,false);
              }
            if ( fdsc->elemdescFunc.tdesc.vt != VT_VOID )
              {
                PyDispFnDef_::ArgDef &a = *mfnd.argv.Push();
                ELEMDESC const *el = &fdsc->elemdescFunc;
                _Py_InitParamDef(val,el,a,ti,true);
                mfnd.rettype = a.type;
                a.flags |= PARAMFLAG_FRETVAL;
              }
            ti->info_->ReleaseFuncDesc(fdsc);
          }}

        {
          PyMethodDef &md = *val->pymethods.Push();
          md.ml_doc = 0;
          md.ml_flags = METH_NOARGS;
          md.ml_meth = (PyCFunction)Py_Object_Release;
          md.ml_name = "_Release";//(char*)val->names.Push("_Release")->Str();
        }

        val->pymembers.Push();
        memset(&val->pymembers.Last(),0,sizeof(val->pymembers.Last()));
        val->pymethods.Push();
        memset(&val->pymethods.Last(),0,sizeof(val->pymethods.Last()));

        val->pytype.tp_getset  = &val->pymembers[0];
        val->pytype.tp_methods = &val->pymethods[0];
        //val->pytype.tp_name = (char*)val->names.Push("-")->Str();
        ti->info_->ReleaseTypeAttr(ta);
      }
  }

void PyTypeInfo_Factory_::GenJamperTable()
  {
    SYSTEM_INFO sSysInfo;
    GetSystemInfo(&sSysInfo);
    unsigned pgSz = sSysInfo.dwPageSize;

    byte_t *vp =
      (byte_t *)VirtualAlloc(0,
        (((sizeof(PyJamper_)*PyJamper_::JAMPER_COUNT) + pgSz - 1 ) / pgSz + 2) * pgSz,
        MEM_RESERVE,PAGE_NOACCESS);

    jamper_ = (PyJamper_ *)(vp + pgSz);
    VirtualAlloc(jamper_,
        (((sizeof(PyJamper_)*PyJamper_::JAMPER_COUNT) + pgSz - 1 ) / pgSz) * pgSz,
        MEM_COMMIT,PAGE_READWRITE);

    for ( int i = 0; i < PyJamper_::JAMPER_COUNT; ++i )
      jamper_[i].Init(i);

    DWORD old;
    VirtualProtect(jamper_,(((sizeof(PyJamper_)*PyJamper_::JAMPER_COUNT) + pgSz - 1 ) / pgSz) * pgSz,PAGE_EXECUTE_READ,&old);
  }

PyTypeInfo_* PyTypeInfo_Factory_::GetPyTypeInfo(GUID const *iidGUID)
  {
    if ( !map_ )
      map_ = new TLB_MAP;
    if ( PyTypeInfo_ **i = map_->Get(*iidGUID) )
      {
        return *i;
      }
    else
      {
        PyTypeInfo_ *val = new PyTypeInfo_;
        InitPyTypeInfoFromTypeInfo(val,iidGUID,jamper_);
        _PoC_Type_Ready(&val->pytype);
        Py_INCREF(&val->pytype);
        map_->Put(*iidGUID,val);
        return val;
      }
  }

Py_Object_Marshal PyTypeInfo_Factory_::FindObjectMarshal(PyTypeObject *tp)
  {
    if ( Py_Object_Marshal *mrs = mrs_->Get(tp) )
      return *mrs;
    else
      {
        if ( tp->tp_base )
          {
            return FindObjectMarshal(tp->tp_base);
          }
        else
          return 0;
      }
  }

void PyTypeInfo_Factory_::AddObjectMarshal(PyTypeObject const *tp,Py_Object_Marshal mrs)
  {
    if ( !mrs_ )
      mrs_ = new MRS_MAP;
    mrs_->Put(tp,mrs);
  }

int PyTypeInfo_Factory_::ExportEnumNames(PyObject *module, pchar_t modname)
  {
    rcc_ptr<ITypeLib> ptl;
    HRESULT hr = ::LoadTypeLib(Hinstance->GetModulePath(),&+ptl);
    if (SUCCEEDED(hr))
      {
        unsigned ti_count = ptl->GetTypeInfoCount();
        for ( int ti_i = 0; ti_i < ti_count; ++ti_i )
          {
            rcc_ptr<ITypeInfo> pti;
            TYPEATTR *pta = 0;
            TYPEKIND tka;

            if ( FAILED(ptl->GetTypeInfoType(ti_i,&tka)) || tka != TKIND_ENUM )
              continue;

            REQ_S_OK(ptl->GetTypeInfo(ti_i,&+pti));
            REQ_S_OK(pti->GetTypeAttr(&pta));

            for ( int i = 0; i < pta->cVars; ++i )
              {
                VARDESC *vdesc = 0;
                if ( SUCCEEDED(pti->GetVarDesc(i,&vdesc)) )
                  {
                    teggo::bstr_ptr name;

                    REQ_S_OK(
                      pti->GetDocumentation( vdesc->memid, &+name,0,0,0 ));

                    asciistring &aname = *names_.Push(+name);

                    REQUIRE(
                      0 ==
                      _PoC_Module_AddObject(
                        module,
                        (char*)aname.Str(),
                        _PoC_Int_FromLong(vdesc->lpvarValue->lVal)
                      ));
                    pti->ReleaseVarDesc(vdesc);
                  }
              }
            pti->ReleaseTypeAttr(pta);
          }
        return 0;
      }
    return -1;
  }

void PyTypeInfo_Factory_::Term()
  {
    if ( mrs_ )
      {
        delete mrs_; mrs_ = 0;
      }

    if ( map_ )
      {
        TLB_MAP::Iterator i = map_->Iterate();
        while ( i.Next()  ) delete *i;
        delete map_; map_ = 0;
      }
    if ( jamper_ )
      {
        VirtualFree( jamper_, 0, MEM_RELEASE );
        jamper_ = 0;
      }
  }

PyTypeInfo_Factory_::~PyTypeInfo_Factory_()
  {
    Term();
  }

PyTypeInfo_Factory_::PyTypeInfo_Factory_() : map_(0)
  {
    GenJamperTable();
  }

PyTypeInfo_Factory_& PyTypeInfoFactory()
  {
    static PyTypeInfo_Factory_ factory_;
    return factory_;
  }

void PyInterlockedInitHolder(PyTypeInfo_ **holder,GUID const *guid)
  {
    *holder = PyTypeInfoFactory().GetPyTypeInfo(guid);
  }

typedef PyObject *(__cdecl *PythonOverCom_PyNew)(PyTypeObject *type, PyObject *args, PyObject *kwds);
extern int __stdcall PythonOverCom_RegisterPyType(pchar_t name,PyTypeInfo_ &ti,PythonOverCom_PyNew fNew)
  {
    ti.pytype.tp_name = (char*)
      ti.names.Push()->Set(_S*"%s.%s" %_PoC_modulename %name);
    if ( fNew ) //PyTypeObject_EnableConstructor(ti);
      ti.pytype.tp_new = fNew;
    Py_INCREF(&ti.pytype);
    return _PoC_Module_AddObject(
      _PoC_module, (char*)name,(PyObject*)&ti.pytype);
  }

HRESULT __stdcall PythonOverCom_Init(pchar_t module_name, pchar_t module_doc)
  {
    static PyMethodDef module_methods[] =
      {
          {NULL}  /* Sentinel */
      };

    if ( 0 != _PoC_Init(module_name,module_methods,module_doc) )
      return S_OK;
    else
      return _E.Fail(E_FAIL,_S*L"failed to initialize python runtime");
  }

HRESULT __stdcall PythonOverCom_ExportEnumNames()
  {
    PyTypeInfoFactory().ExportEnumNames(
                          _PoC_module,
                          _PoC_modulename);
    return S_OK;
  }

HRESULT __stdcall PythonOverCom_AddObject(pchar_t name, IUnknown *obj)
  {
    rcc_ptr<IComPyUnknown> i;
    if ( SUCCEEDED( obj->QueryInterface(TEGGO_GUIDOF(IComPyUnknown),(void**)&+i)) )
      {
        PyObject *po = (PyObject*)i->IComPyUnknown_Get(true);
        STRICT_ENSURE( 0 == _PoC_Module_AddObject(_PoC_module, (char*)name, po) );
        return S_OK;
      }
    else
      return _E.Fail(E_FAIL,
        _S*"failed to adding object '%?' into module '%?', object is not wrapped through 'python over com' api"
            %name
            %_PoC_modulename);
  }

HRESULT __stdcall PythonOverCom_GlobalizeMember(pchar_t name, IUnknown *obj)
  {
    rcc_ptr<IComPyUnknown> i;
    if ( SUCCEEDED( obj->QueryInterface(TEGGO_GUIDOF(IComPyUnknown),(void**)&+i)) )
      {
        pyobj_ptr po( (PyObject*)i->IComPyUnknown_Get(true) );
        if ( PyObject *o = _PoC_Object_GetAttrString(+po,name) )
          {
            STRICT_ENSURE( 0 == _PoC_Module_AddObject(_PoC_module, (char*)name, o) );
            return S_OK;
          }
      }

    return _E.Fail(E_FAIL,
        _S*"failed to adding object member '%?' into module '%?', object is not wrapped through 'python over com' api"
            %name
            %_PoC_modulename);
  }

PyObject * __cdecl PyInit(PyObject *self, PyObject *args, PyObject *kwds)
  {
    PyObjectWrapper_ *pw = (PyObjectWrapper_ *)self;
    rcc_ptr<IComPyUnknown> icpu;
    if ( SUCCEEDED( pw->this_->QueryInterface(TEGGO_GUIDOF(IComPyUnknown),(void**)&+icpu)))
      return icpu->__init__(args,kwds);
    else
      return
        _PoC_RaiseExcept(0|_S*"object of type %s could not be initialized directly"%pw->ob_type->tp_name),
        0;
  }
