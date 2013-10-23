
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

PyTypeObject *_PoC_Type_Type = 0;
PyTypeObject *_PoC_Int_Type = 0;
PyTypeObject *_PoC_Bool_Type = 0;
PyTypeObject *_PoC_String_Type = 0;
PyTypeObject *_PoC_Unicode_Type = 0;
PyTypeObject *_PoC_Float_Type = 0;
PyTypeObject *_PoC_Method_Type = 0;
PyTypeObject *_PoC_Function_Type = 0;
PyTypeObject *_PoC_Tuple_Type = 0;
PyTypeObject *_PoC_Gen_Type = 0;
PyObject *_PoC_True = 0;
PyObject *_PoC_False = 0;
PyObject *_PoC_None = 0;
PyObject *_PoC_Except = 0;
PyObject **_PoC_StdExcept = 0;
PyObject **_PoC_TypeExcept = 0;
PyObject **_PoC_StopIteration = 0;
PyObject *(__cdecl *_PoC_Int_FromLong)(long) = 0;
PyObject *(__cdecl *_PoC_Float_FromDouble)(double) = 0;
PyObject *(__cdecl *_PoC_Unicode_FromUnicode)(const Py_UNICODE *u, int sz) = 0;
PyObject *(__cdecl *_PoC_InitModule4)(pchar_t, PyMethodDef *,pchar_t, PyObject *,int)  = 0;
PyObject *(__cdecl *_PoC_Err_NewException)(pchar_t,PyObject *,PyObject *) = 0;
PyObject *(__cdecl *_PoC_Err_Occurred)() = 0;
void (__cdecl *_PoC_Err_Clear)() = 0;
void (__cdecl *_PoC_Err_SetString)(PyObject *, pchar_t) = 0;
void (__cdecl *_PoC_Err_SetNone)(PyObject *) = 0;
void (__cdecl *_PoC_Err_Fetch)(PyObject **,PyObject **,PyObject **) = 0;
void (__cdecl *_PoC_Err_NormalizeException)(PyObject **,PyObject **,PyObject **) = 0;
int (__cdecl *_PoC_Type_Ready)(PyTypeObject *) = 0;
int (__cdecl *_PoC_Module_AddObject)(PyObject *,pchar_t,PyObject *) = 0;
PyObject *(__cdecl *_PoC_Tuple_New)(int) = 0;
PyObject *(__cdecl *_PoC_Tuple_SetItem)(PyObject*,int,PyObject*) = 0;
PyObject *(__cdecl *_PoC_Object_GetAttrString)(PyObject *,pchar_t) = 0;
PyObject *(__cdecl *_PoC_Object_CallObject)(PyObject *, PyObject *) = 0;
PyObject *(__cdecl *_PoC_Object_Unicode)(PyObject *) = 0;
PyObject *(__cdecl *_PoC_Method_Function)(PyObject *) = 0;
PyObject *(__cdecl *_PoC_PyDict_GetItemString)(PyObject *,pchar_t) = 0;
int *(__cdecl *_PoC_Arg_ParseTuple)(PyObject *args, pchar_t format, ...) = 0;

char *_PoC_modulename = 0;
PyObject *_PoC_module = 0;
char *_PoC_exceptname = 0;
long *_PoC_RefTotal = 0;
void (__cdecl *_PoC_NegativeRefcount)(const char *,int, PyObject *) = 0;
void (__cdecl *_PoC_Dealloc)(PyObject *) = 0;

void TeggoRccTaits<PyObject>::Refcounter::
  Release(PyObject *p)
  {
    Py_XDECREF((PyObject*)p);
  }

void TeggoRccTaits<PyObject>::Refcounter::
  AddRef(PyObject *p)
  {
    Py_XINCREF((PyObject*)p);
  }

PyObject *_PoC_Init(pchar_t name, PyMethodDef *m0, pchar_t m1)
  {
    __synchronize__
      {

        _PoC_modulename = strdup(name);
        HMODULE hpm;
        hpm = GetModuleHandle("python27"
#       ifdef Py_REF_DEBUG
          "_d"
#       endif //Py_REF_DEBUG
          );
        if ( !hpm )
          hpm = GetModuleHandle("python"
#       ifdef Py_REF_DEBUG
          "_d"
#       endif //Py_REF_DEBUG
          );
        if ( !hpm )
          hpm = GetModuleHandle("pythonic");
        if ( !hpm )
          hpm = GetModuleHandle("pyteggo2");
        if ( !hpm )
          hpm = GetModuleHandle(0);

        if ( *(void**)(&_PoC_InitModule4) =
#       ifdef Py_REF_DEBUG
          GetProcAddress(hpm,"Py_InitModule4TraceRefs")
#       else
          GetProcAddress(hpm,"Py_InitModule4")
#       endif //Py_REF_DEBUG
          )
          {
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Type_Type) = GetProcAddress(hpm,"PyType_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Tuple_Type) = GetProcAddress(hpm,"PyTuple_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Int_Type) = GetProcAddress(hpm,"PyInt_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Float_Type) = GetProcAddress(hpm,"PyFloat_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Bool_Type) = GetProcAddress(hpm,"PyBool_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_String_Type) = GetProcAddress(hpm,"PyString_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Unicode_Type) = GetProcAddress(hpm,"PyUnicode_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Method_Type) = GetProcAddress(hpm,"PyMethod_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Function_Type) = GetProcAddress(hpm,"PyFunction_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Gen_Type) = GetProcAddress(hpm,"PyGen_Type")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_True) = GetProcAddress(hpm,"_Py_TrueStruct")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_False) = GetProcAddress(hpm,"_Py_ZeroStruct")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_None) = GetProcAddress(hpm,"_Py_NoneStruct")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Int_FromLong) = GetProcAddress(hpm,"PyInt_FromLong")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Float_FromDouble) = GetProcAddress(hpm,"PyFloat_FromDouble")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Type_Ready) = GetProcAddress(hpm,"PyType_Ready")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Module_AddObject) = GetProcAddress(hpm,"PyModule_AddObject")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_StdExcept) = GetProcAddress(hpm,"PyExc_StandardError")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_StopIteration) = GetProcAddress(hpm,"PyExc_StopIteration")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_TypeExcept) = GetProcAddress(hpm,"PyExc_TypeError")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Err_NewException) = GetProcAddress(hpm,"PyErr_NewException")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Err_SetString) = GetProcAddress(hpm,"PyErr_SetString")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Err_SetNone) = GetProcAddress(hpm,"PyErr_SetNone")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Err_Clear) = GetProcAddress(hpm,"PyErr_Clear")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Err_Fetch) = GetProcAddress(hpm,"PyErr_Fetch")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Err_NormalizeException) = GetProcAddress(hpm,"PyErr_NormalizeException")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Err_Occurred) = GetProcAddress(hpm,"PyErr_Occurred")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Unicode_FromUnicode) = GetProcAddress(hpm,"PyUnicodeUCS2_FromUnicode")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Object_GetAttrString) = GetProcAddress(hpm,"PyObject_GetAttrString")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Object_CallObject) = GetProcAddress(hpm,"PyObject_CallObject")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Object_Unicode) = GetProcAddress(hpm,"PyObject_Unicode")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Tuple_New) = GetProcAddress(hpm,"PyTuple_New")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Tuple_SetItem) = GetProcAddress(hpm,"PyTuple_SetItem")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Method_Function) = GetProcAddress(hpm,"PyMethod_Function")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_PyDict_GetItemString) = GetProcAddress(hpm,"PyDict_GetItemString")) );
            STRICT_ENSURE( 0 != (*(void**)(&_PoC_Arg_ParseTuple) = GetProcAddress(hpm,"PyArg_ParseTuple")) );
#           ifdef Py_REF_DEBUG
              STRICT_ENSURE( 0 != (*(void**)(&_PoC_RefTotal) = GetProcAddress(hpm,"_Py_RefTotal")) );
              STRICT_ENSURE( 0 != (*(void**)(&_PoC_Dealloc) = GetProcAddress(hpm,"_Py_Dealloc")) );
              STRICT_ENSURE( 0 != (*(void**)(&_PoC_NegativeRefcount) = GetProcAddress(hpm,"_Py_NegativeRefcount")) );
#           endif //Py_REF_DEBUG
            if ( PyObject *mRet = _PoC_InitModule4(_PoC_modulename,m0,m1,(PyObject *)NULL, PYTHON_API_VERSION) )
              {
                _PoC_module = mRet;
                static char SpecificError[] = "SpecificError";
                StringA exceptname;
                ((exceptname = _PoC_modulename) += "." ) += SpecificError;
                _PoC_exceptname = strdup(+exceptname);
                STRICT_ENSURE(
                  0 != (
                    _PoC_Except = _PoC_Err_NewException(_PoC_exceptname,*_PoC_StdExcept,0)) );
                Py_INCREF( _PoC_Except );
                STRICT_ENSURE(
                  0 ==
                    _PoC_Module_AddObject(_PoC_module,SpecificError,_PoC_Except) );
                return _PoC_module;
              }
          }
        return 0;
      } // __synchronize__
  }

void _PoC_RaiseExcept(pchar_t msg)
  {
    _PoC_Err_SetString(_PoC_Except,msg);
  }

void _PoC_RaiseTypeExcept(pchar_t msg)
  {
    _PoC_Err_SetString(*_PoC_TypeExcept,msg);
  }

void _PoC_RaiseComExcept()
  {
    widestring msg;
    TeggoComException::GetComError(&msg);
    _PoC_RaiseExcept(asciistring(msg).Str());
  }

pchar_t _PoC_TypeName(PyTypeObject *type)
  {
    return type->tp_name;
  }

int __PoC_Convert(PyObject *a,int need_type,void *to)
  {
    if ( need_type == 'u' )
      {
        wchar_t *u;
        if ( a->ob_type == _PoC_Unicode_Type )
          {
            u = (wchar_t*)PyUnicode_AS_UNICODE(a);
          }
        else if ( a->ob_type == _PoC_String_Type )
          {
            u = _PoC_LocalString(PyString_AS_STRING(a));
          }
        else if ( a == _PoC_None )
          {
            u = 0;
          }
        else
          return _PoC_RaiseExcept(0|_S*"failed to convert '%s' to unicode" %a->ob_type->tp_name),
            0;
        *(wchar_t**)to = u;
      }
    else if ( need_type == 'i' )
      {
        int l;
        if ( a->ob_type == _PoC_Int_Type )
          l = (int)PyInt_AS_LONG(a);
        else if ( a->ob_type == _PoC_Float_Type )
          l = (int)PyFloat_AS_DOUBLE(a);
        else
          return _PoC_RaiseExcept(0|_S*"failed to convert '%s' to integer" %a->ob_type->tp_name),
            0;
        *(int*)to = l;
      }
    else if ( need_type == 'f' )
      {
        float l;
        if ( a->ob_type == _PoC_Int_Type )
          l = (int)PyInt_AS_LONG(a);
        else if ( a->ob_type == _PoC_Float_Type )
          l = (int)PyFloat_AS_DOUBLE(a);
        else
          return _PoC_RaiseExcept(0|_S*"failed to convert '%s' to integer" %a->ob_type->tp_name),
            0;
        *(float*)to = l;
      }
    else if ( need_type == 'F' )
      {
        double l;
        if ( a->ob_type == _PoC_Int_Type )
          l = (int)PyInt_AS_LONG(a);
        else if ( a->ob_type == _PoC_Float_Type )
          l = (int)PyFloat_AS_DOUBLE(a);
        else
          return _PoC_RaiseExcept(0|_S*"failed to convert '%s' to integer" %a->ob_type->tp_name),
            0;
        *(double*)to = l;
      }
    else
      return _PoC_RaiseExcept(0|_S*"unknown required type '%c'" %need_type),
        0;
    return 1;
  }

int _PoC_Args(PyObject *args, PyObject *kwds, pchar_t format, ...)
  {
    int result = 0;
    int part = 0; // 0 - manadory, 1 - optional, 2 - keywords
    int arg_count = args ? PyTuple_GET_SIZE(args) : 0;
    int arg_i = 0;
    va_list va;
    va_start(va,format);
    char const *fmtI = format;
    StringT<char,32> keyname;
    while ( *fmtI )
      {
        int need_type = 0;
        keyname.Resize(0);

        if ( *fmtI == 'u' )
          {
            need_type = 'u';
          }
        else if ( *fmtI == 'i' )
          {
            need_type = 'i';
          }
        else if ( *fmtI == '|' )
          {
            ++part;
          }
        else
          {
            _PoC_RaiseExcept(0|_S*"bad character in format string at %d" %(fmtI-format));
            goto end;
          }
        ++fmtI;
        if ( need_type )
          {
            if ( *fmtI == '{' )
              {
                for ( ++fmtI; *fmtI && *fmtI != '}'; ++fmtI )
                  keyname.Append(*fmtI);
                if ( *fmtI  == '}' ) ++fmtI;
              }
            if ( arg_i < arg_count && part < 2 )
              {
                PyObject *a = PyTuple_GET_ITEM(args,arg_i);
                ++arg_i;
                void *to = va_arg(va,void*);
                if ( !__PoC_Convert(a,need_type,to) )
                  goto end;
              }
            else if ( keyname && kwds )
              {
                void *to = va_arg(va,void*);
                PyObject *a = _PoC_PyDict_GetItemString(kwds,+keyname);
                if ( a )
                  if ( !__PoC_Convert(a,need_type,to) )
                    goto end;
              }
            else if ( part == 0 )
              {
                _PoC_RaiseExcept("not enough arguments");
                goto end;
              }
            else
              (void)va_arg(va,void*); // skip
          }
      }
    result = 1;
  end:
    va_end(va);
    return result;
  }

wchar_t *_PoC_LocalString_(int len)
  {
    enum { LOCAL_BUFFERCOUNT = 128 };
    static wchar_t *buffer[LOCAL_BUFFERCOUNT] = {0};
    static int counter = 0;

    __synchronize__
      {
        wchar_t **l = buffer+counter;
        if ( *l )
          SysFreeString(*l);
        *l = SysAllocStringLen(0,len+1);
        counter = (counter + 1) % LOCAL_BUFFERCOUNT;
        return *l;
      }
  }

wchar_t *_PoC_LocalString(char *s)
  {
    if ( s )
      {
        int s_len = strlen(s);
        wchar_t *w = _PoC_LocalString_(s_len);
        wchar_t *ww = w;
        while ( *s ) *ww++ = teggo::ucs2_btowc(*s++);
        *ww = 0;
        return w;
      }
    else
      return 0;
  }

void _PoC_Err_StopIteration()
  {
    _PoC_Err_SetNone(*_PoC_StopIteration);
  }
