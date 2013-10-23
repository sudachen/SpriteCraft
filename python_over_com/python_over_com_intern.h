
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

#include "../Classes/classes.h"
#include "Python.h"
#define _INCLUDE_FULL_PYTHON_
#include "./kaa.h"
#include "./python_over_com.h"

struct IComPyUnknown;

typedef IUnknown *(*Py_Object_Marshal)(PyObject *);

struct PyObjectWrapper_
  {
    PyObject_HEAD
    IUnknown *this_;
  };

enum TYPE_ANALOG
  {
    TP_VOID   = 0x00,
    TP_LONG   ,//= 0x04,
    TP_SHORT  ,//= 0x02,
    TP_BOOL   ,//= 0x0f2,
    TP_SINGLE ,//= 0x0f4,
    TP_DOUBLE ,//= 0x0f8,
    TP_BSTR   ,//= 0x0fd,
    TP_OBJECT ,//= 0x0ff,
    TP_SETRETVAL,
  };

struct PyDispFnDef_
  {
    struct ArgDef
      {
        TYPE_ANALOG type;
        unsigned short flags;
        unsigned short defultno;
        unsigned short isptr;
      };
    BufferT<ArgDef> argv;
    TYPE_ANALOG rettype;
    unsigned short vfto;
    unsigned short getset;
    pchar_t name;
    enum { IS_GETTER = 1, IS_SETTER = 2 };
  };

struct PyTypeInfo_Tlb_
  {
    PyTypeObject pytype;
    BufferT<PyGetSetDef> pymembers;
    BufferT<PyMethodDef> pymethods;
    ArrayT<PyDispFnDef_> funcd;
    BufferT<VARIANT> defaults;
    SlistT<teggo::asciistring> names;
    PySequenceMethods sq;
    int iterate_next_oVft;
    int iterate_oVft;
    int sq_len_oVft;
  };

struct PyTypeInfo_Factory_;
PyTypeInfo_Factory_& PyTypeInfoFactory();

struct PyJamper_;
struct PyTypeInfo_Factory_
  {
    typedef FlatmapT<GUID,PyTypeInfo_*,teggo::GUIDless> TLB_MAP;
    typedef FlatmapT<void const*,Py_Object_Marshal> MRS_MAP;
    TLB_MAP *map_;
    MRS_MAP *mrs_;
    PyJamper_ *jamper_;
    SlistT< StringT<char> > names_;
    PyTypeInfo_* GetPyTypeInfo(const GUID* iidGUID);
    Py_Object_Marshal FindObjectMarshal(PyTypeObject *tp);
    void AddObjectMarshal(PyTypeObject const *tp,Py_Object_Marshal mrs);
    int ExportEnumNames(PyObject *module, pchar_t modname);
    void Term();
    ~PyTypeInfo_Factory_();
  protected:
    void GenJamperTable();
    PyTypeInfo_Factory_();
    friend PyTypeInfo_Factory_& PyTypeInfoFactory();
  };

void PyInterlockedInitHolder(PyTypeInfo_ **holder,GUID const *guid);
extern int __stdcall TeggoDispImpl_PyAddTypeToModule(PyObject *module,pchar_t modulename,pchar_t name,PyTypeInfo_&);
