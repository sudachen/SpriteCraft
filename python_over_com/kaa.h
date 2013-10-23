
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

#if !defined ___bcb13d57_2f66_4e40_9609_cbb59db75cd8___
#define ___bcb13d57_2f66_4e40_9609_cbb59db75cd8___

// #include "../../Classes/classes.h"
// #include "../../Python.Teggo/Pythonic.SRC/_Python.h"
//
// struct _Py_External_Module_
//   {
//
//     struct Gate
//       {
//         _Py_External_Module_ *operator->()
//           {
//             static _Py_External_Module_ _static_external_module_;
//             return &_static_external_module_;
//           }
//       };
//
//     PyObject *Init(pchar_t, PyMethodDef *,pchar_t);
//     void RaiseTypeExcept(pchar_t text);
//     void RaiseExcept(pchar_t text);
//     void RaiseComExcept();
//     StringT<char> exceptname;
//     StringT<char> modulename;
//     PyObject *module;
// #   ifdef Py_REF_DEBUG
//     long *_RefTotal;
//     void (__cdecl *_NegativeRefcount)(const char *,int, PyObject *);
//     void (__cdecl *_Dealloc)(PyObject *);
// #   endif
//     PyTypeObject *Int_Type;
//     PyTypeObject *Bool_Type;
//     PyTypeObject *String_Type;
//     PyTypeObject *Unicode_Type;
//     PyTypeObject *Float_Type;
//     PyTypeObject *Method_Type;
//     PyTypeObject *Function_Type;
//     PyObject *True;
//     PyObject *False;
//     PyObject *None;
//     PyObject *Except;
//     PyObject **StdExcept;
//     PyObject **TypeExcept;
//     PyObject *(__cdecl *Int_FromLong)(long);
//     PyObject *(__cdecl *Float_FromDouble)(double);
//     PyObject *(__cdecl *Unicode_FromUnicode)(const Py_UNICODE *u, int sz);
//     PyObject *(__cdecl *InitModule4)(pchar_t, PyMethodDef *,pchar_t, PyObject *,int);
//     PyObject *(__cdecl *Err_NewException)(pchar_t,PyObject *,PyObject *);
//     PyObject *(__cdecl *Err_Occurred)();
//     void (__cdecl *Err_Clear)();
//     void (__cdecl *Err_SetString)(PyObject *, pchar_t);
//     void (__cdecl *Err_Fetch)(PyObject **,PyObject **,PyObject **);
//     void (__cdecl *Err_NormalizeException)(PyObject **,PyObject **,PyObject **);
//     int (__cdecl *Type_Ready)(PyTypeObject *);
//     int (__cdecl *Module_AddObject)(PyObject *,pchar_t,PyObject *);
//     PyObject *(__cdecl *Tuple_New)(int);
//     PyObject *(__cdecl *Object_GetAttrString)(PyObject *,pchar_t);
//     PyObject *(__cdecl *Object_CallObject)(PyObject *, PyObject *);
//     PyObject *(__cdecl *Object_Unicode)(PyObject *);
//     PyObject *(__cdecl *Method_Function)(PyObject *);
//     PyObject *Bool_FromVARBOOL(VARIANT_BOOL v);
//
//     PyObject *Unicode_FromUnicode1(wchar_t const *u)
//       {
//         return Unicode_FromUnicode((const Py_UNICODE *)u,wcslen(u));
//       }
//   };
//
// static _Py_External_Module_::Gate _Py_External_Module;
//
// # ifdef Py_REF_DEBUG
// inline long _Py_External_Module_Inc_RefTotal()
//   {
//     return (*_Py_External_Module->_RefTotal)++;
//   }
//
// inline long _Py_External_Module_Dec_RefTotal()
//   {
//     return (*_Py_External_Module->_RefTotal)--;
//   }
// #   undef _Py_INC_REFTOTAL
// #   define _Py_INC_REFTOTAL _Py_External_Module_Inc_RefTotal()
// #   undef _Py_DEC_REFTOTAL
// #   define _Py_DEC_REFTOTAL _Py_External_Module_Dec_RefTotal()
// #   undef _Py_Dealloc
// #   define _Py_Dealloc _Py_External_Module->_Dealloc
// #   undef _Py_NegativeRefcount
// #   define _Py_NegativeRefcount _Py_External_Module->_NegativeRefcount
// # endif
//
// inline PyObject *_Py_External_Module_::Bool_FromVARBOOL(VARIANT_BOOL v)
//   {
//     PyObject *val = (v != VARIANT_FALSE) ? True : False;
//     Py_INCREF(val);
//     return val;
//   }
//
// #define _PoC_CALL __cdecl
// #define _PoC_CALL_IS_STDCALL 0
//
// #define _PoC_Int_Type                 (_Py_External_Module->Int_Type)
// #define _PoC_Float_Type               (_Py_External_Module->Float_Type)
// #define _PoC_Bool_Type                (_Py_External_Module->Bool_Type)
// #define _PoC_Method_Type              (_Py_External_Module->Method_Type)
// #define _PoC_Unicode_Type             (_Py_External_Module->Unicode_Type)
// #define _PoC_Function_Type            (_Py_External_Module->Function_Type)
// #define _PoC_None                     _Py_External_Module->None
// #define _PoC_True                     _Py_External_Module->True
// #define _PoC_False                    _Py_External_Module->False
// #define _PoC_TypeExcept               _Py_External_Module->TypeExcept
// #define _PoC_StdExcept                _Py_External_Module->StdExcept
// #define _PoC_Int_FromLong             _Py_External_Module->Int_FromLong
// #define _PoC_Float_FromDouble         _Py_External_Module->Float_FromDouble
// #define _PoC_Unicode_FromUnicode      _Py_External_Module->Unicode_FromUnicode
// #define _PoC_Type_Ready               _Py_External_Module->Type_Ready
// #define _PoC_Module_AddObject         _Py_External_Module->Module_AddObject
// #define _PoC_Err_Occurred             _Py_External_Module->Err_Occurred
// #define _PoC_Err_Fetch                _Py_External_Module->Err_Fetch
// #define _PoC_Err_Clear                _Py_External_Module->Err_Clear
// #define _PoC_Object_Unicode           _Py_External_Module->Object_Unicode
// #define _PoC_Object_GetAttrString     _Py_External_Module->Object_GetAttrString
// #define _PoC_Method_Function          _Py_External_Module->Method_Function
// #define _PoC_Object_CallObject        _Py_External_Module->Object_CallObject
// #define _PoC_Tuple_New                _Py_External_Module->Tuple_New
//
//
// #define _PoC_Unicode_FromUnicode1     _Py_External_Module->Unicode_FromUnicode1
// #define _PoC_Init                     _Py_External_Module->Init
// #define _PoC_RaiseTypeExcept          _Py_External_Module->RaiseTypeExcept
// #define _PoC_RaiseExcept              _Py_External_Module->RaiseExcept
// #define _PoC_RaiseComExcept           _Py_External_Module->RaiseComExcept
//

#define _PoC_CALL __cdecl
#define _PoC_CALL_IS_STDCALL 0

struct _object;
struct _typeobject;
typedef struct _object PyObject;
typedef struct _typeobject PyTypeObject;
typedef wchar_t Py_UNICODE;

# ifdef Py_REF_DEBUG
extern long *_PoC_RefTotal;
extern void (__cdecl *_PoC_NegativeRefcount)(const char *,int, PyObject *);
extern void (__cdecl *_PoC_Dealloc)(PyObject *);

inline long _Py_External_Module_Inc_RefTotal()
  {
    return (*_PoC_RefTotal)++;
  }

inline long _Py_External_Module_Dec_RefTotal()
  {
    return (*_PoC_RefTotal)--;
  }
#   undef _Py_INC_REFTOTAL
#   define _Py_INC_REFTOTAL _Py_External_Module_Inc_RefTotal()
#   undef _Py_DEC_REFTOTAL
#   define _Py_DEC_REFTOTAL _Py_External_Module_Dec_RefTotal()
#   undef _Py_Dealloc
#   define _Py_Dealloc _PoC_Dealloc
#   undef _Py_NegativeRefcount
#   define _Py_NegativeRefcount _PoC_NegativeRefcount
# endif

extern PyTypeObject *_PoC_Type_Type;
extern PyTypeObject *_PoC_Int_Type;
extern PyTypeObject *_PoC_Bool_Type;
extern PyTypeObject *_PoC_String_Type;
extern PyTypeObject *_PoC_Unicode_Type;
extern PyTypeObject *_PoC_Float_Type;
extern PyTypeObject *_PoC_Method_Type;
extern PyTypeObject *_PoC_Function_Type;
extern PyTypeObject *_PoC_Tuple_Type;
extern PyTypeObject *_PoC_Gen_Type;
extern PyObject *_PoC_True;
extern PyObject *_PoC_False;
extern PyObject *_PoC_None;
extern PyObject *_PoC_Except;
extern PyObject **_PoC_StdExcept;
extern PyObject **_PoC_StopIteration;
extern PyObject **_PoC_TypeExcept;
extern PyObject *(__cdecl *_PoC_Int_FromLong)(long);
extern PyObject *(__cdecl *_PoC_Float_FromDouble)(double);
extern PyObject *(__cdecl *_PoC_Unicode_FromUnicode)(const Py_UNICODE *u, int sz);
#if defined _INCLUDE_FULL_PYTHON_
extern PyObject *(__cdecl *_PoC_InitModule4)(pchar_t, PyMethodDef *,pchar_t, PyObject *,int);
#endif
extern PyObject *(__cdecl *_PoC_Err_NewException)(pchar_t,PyObject *,PyObject *);
extern PyObject *(__cdecl *_PoC_Err_Occurred)();
extern void (__cdecl *_PoC_Err_Clear)();
extern void (__cdecl *_PoC_Err_SetString)(PyObject *, pchar_t);
extern void (__cdecl *_PoC_Err_Fetch)(PyObject **,PyObject **,PyObject **);
extern void (__cdecl *_PoC_Err_NormalizeException)(PyObject **,PyObject **,PyObject **);
extern int (__cdecl *_PoC_Type_Ready)(PyTypeObject *);
extern int (__cdecl *_PoC_Module_AddObject)(PyObject *,pchar_t,PyObject *);
extern PyObject *(__cdecl *_PoC_Tuple_New)(int);
extern PyObject *(__cdecl *_PoC_Tuple_SetItem)(PyObject*,int,PyObject*);
extern PyObject *(__cdecl *_PoC_Object_GetAttrString)(PyObject *,pchar_t);
extern PyObject *(__cdecl *_PoC_Object_CallObject)(PyObject *, PyObject *);
extern PyObject *(__cdecl *_PoC_Object_Unicode)(PyObject *);
extern PyObject *(__cdecl *_PoC_Method_Function)(PyObject *);
extern PyObject *(__cdecl *_PoC_Dict_GetItemString)(PyObject *,pchar_t);
extern int *(__cdecl *_PoC_Arg_ParseTuple)(PyObject *args, pchar_t format, ...);

extern char *_PoC_modulename;
extern PyObject *_PoC_module;
extern char *_PoC_exceptname;

#if defined _INCLUDE_FULL_PYTHON_
PyObject *_PoC_Init(pchar_t, PyMethodDef *,pchar_t);
#endif

extern void _PoC_RaiseTypeExcept(pchar_t text);
extern void _PoC_RaiseExcept(pchar_t text);
extern void _PoC_RaiseComExcept();
extern pchar_t _PoC_TypeName(PyTypeObject*);
extern int _PoC_Args(PyObject *args, PyObject *kwds, pchar_t format, ...);
extern wchar_t *_PoC_LocalString(char *s);
extern void _PoC_Err_StopIteration();
extern void _PoC_CALL Py_Object_Dealloc(PyObject *self);

inline PyObject *_PoC_Bool_FromVARBOOL(VARIANT_BOOL b)
  {
    return Refe(b?_PoC_True:_PoC_False);
  }

inline PyObject *_PoC_RefNone()
  {
    return Refe(_PoC_None);
  }

inline PyObject *_PoC_Unicode_FromUnicode1(wchar_t const *u)
  {
    return _PoC_Unicode_FromUnicode((const Py_UNICODE *)u,wcslen(u));
  }

template<> struct TeggoRccTaits<PyObject>
  {
    struct Refcounter
      {
        static void Release(PyObject *p);
        static void AddRef(PyObject *p);
      };
  };

typedef rcc_ptr<PyObject> pyobj_ptr;

#endif //___bcb13d57_2f66_4e40_9609_cbb59db75cd8___
