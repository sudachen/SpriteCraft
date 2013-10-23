
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

#if !defined ___e8394b53_f4bc_4010_b2ec_918177dcde4d___
#define ___e8394b53_f4bc_4010_b2ec_918177dcde4d___

struct PyObjectWrapper_;
struct PyTypeInfo_Tlb_;
struct PyTypeInfo_Embedded_;
#ifndef _PYTEGGO_EMBEDDED_TYPEINFO
typedef PyTypeInfo_Tlb_ PyTypeInfo_;
#else
typedef PyTypeInfo_Embedded_ PyTypeInfo_;
#endif

struct CXX_NO_VTABLE IComPyUnknown : TeggoInterface<IUnknown>
  {
    virtual PyObjectWrapper_ *IComPyUnknown_Get(bool inrefcnt) = 0;
    virtual void IComPyUnknown_Unlink() = 0;
    virtual PyObject *__init__(PyObject *args, PyObject *kwds) = 0;
    //virtual HRESULT __del__() = 0;
  };
TEGGO_DECLARE_GUIDOF(IComPyUnknown, 0xC8418BDF,0xB477,0x4E20,0x9A,0x5A,0xBB,0xF3,0x6C,0x4B,0x21,0x9B);

template<> struct TeggoRccTaits<PyObjectWrapper_>
  {
    struct Refcounter
      {
        static void __stdcall Release(PyObjectWrapper_ *p);
        static void __stdcall AddRef(PyObjectWrapper_ *p);
      };
  };

PyObjectWrapper_ * CXX_STDCALL PyObjectWrapper_New(PyTypeInfo_ const&,IUnknown *u);
void PyInterlockedInitHolder(PyTypeInfo_ **holder,GUID const *guid);

typedef HRESULT(__stdcall *PythonOverCom_Make3)(IUnknown **,PyObject *,PyObject *);
typedef PyObject *(__cdecl *PythonOverCom_PyNew)(PyTypeObject *, PyObject *, PyObject *);
extern int __stdcall PythonOverCom_RegisterPyType(pchar_t name,PyTypeInfo_&,PythonOverCom_PyNew);

template< class tDifs, PythonOverCom_Make3 tMake, class tOther = TeggoIfsUnknown >
struct CXX_NO_VTABLE TeggoPyDispImpl :
  TeggoDispImpl<tDifs, TeggoIfs<IComPyUnknown,tOther> >
  {
    PyObjectWrapper_ *pyobj_;
    virtual u32_t __stdcall Release() = 0;
    virtual u32_t __stdcall AddRef() = 0;

    TeggoPyDispImpl() : pyobj_(0)
      {
      }

    virtual ~TeggoPyDispImpl()
      {
        DBG_REQUIRE(!pyobj_);
      }

    virtual PyObject *__init__(PyObject *, PyObject *) { return Refe(_PoC_None); }
    //virtual HRESULT __del__() { return S_OK; }

    virtual PyObjectWrapper_ *IComPyUnknown_Get(bool increfcnt)
      {
        if ( !pyobj_)
          {
            REQUIRE( increfcnt );
            PyTypeInfo_ &pytype = GetPyType();
            pyobj_ = PyObjectWrapper_New(pytype,(tDifs*)this);
          }
        else
          {
            if ( increfcnt )
              Refe(pyobj_);
          }
        return pyobj_;
      }

    virtual void IComPyUnknown_Unlink()
      {
        pyobj_ = 0;
      };

    static PyTypeInfo_& GetPyType()
      {
        static PyTypeInfo_* holder = 0;
        if ( !holder )
          PyInterlockedInitHolder(&holder,&TEGGO_GUIDOF(tDifs));
        return *holder;
      }

    static HRESULT RegisterPyType(pchar_t name)
      {
        return PythonOverCom_RegisterPyType(name,GetPyType(),tMake?&PyNew:0);
      }

    static PyObject * __cdecl PyNew(PyTypeObject *type, PyObject *args, PyObject *kwds)
      {
        teggo::rcc_ptr<tDifs> p;
        if ( tMake )
          if ( SUCCEEDED( tMake((IUnknown**)&+p,args,kwds) ) )
            {
              teggo::rcc_ptr<IComPyUnknown> icpu =
                teggo::ifs_cast<IComPyUnknown>(p);
              if ( icpu )
                return (PyObject*)icpu->IComPyUnknown_Get(true);
            }
        if ( !_PoC_Err_Occurred() )
          _PoC_RaiseExcept(0|_S*"object of type %s could not be created directly"%_PoC_TypeName(type));
        return 0;
      }
  };

PyObject * __cdecl PyInit(PyObject *self, PyObject *args, PyObject *kwds);

HRESULT __stdcall PythonOverCom_Init(pchar_t module_name, pchar_t module_doc);
HRESULT __stdcall PythonOverCom_ExportEnumNames();
HRESULT __stdcall PythonOverCom_AddObject(pchar_t name, IUnknown *obj);
HRESULT __stdcall PythonOverCom_GlobalizeMember(pchar_t name, IUnknown *obj);

#endif // ___e8394b53_f4bc_4010_b2ec_918177dcde4d___
