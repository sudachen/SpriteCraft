
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
#include "../ScraftAPI/spritecraft.h"
#include "../ScraftAPI/guids_.h"
#include "traceback.h"
#include "frameobject.h"

struct PyDispatcher_ :
  IOnUpdate,
  IOnCollide,
  IOnMouseClick,
  IOnMouseUp,
  IOnMouseDown,
  IOnMouseMove,
  IOnMouseOver,
  IOnDispose,
  IOnLifetimeOut,
  IOnEmission,
  ICommand,
  IOnStopAnimation,
  IOnStopEmission,
  IOnStopSound,
  IComPyUnknown
  {
    typedef GuidT<0x1DB61EFD,0x7176,0x42D9,
                  0x9E, 0x5A, 0xBC, 0x1E, 0x5D, 0x93, 0x12, 0x08> Guid;

    virtual HRESULT InternalQueryInterface2(REFIID riid ,void ** pI) = 0;

    HRESULT InternalQueryInterface(REFIID riid ,void ** pI)
      {
        return InternalQueryInterface2(riid,pI);
      }

    u32_t __stdcall AddRef() = 0;
    u32_t __stdcall Release() = 0;
  };

struct PyDispatcher;
struct PyDispatcherWrapper
  {
    PyObject_HEAD
    PyDispatcher *dispatcher_;
  };

struct PyDispatcher : TeggoObject<PyDispatcher_>
  {
    PyDispatcherWrapper *wrapper_;
    u32_t flags_;
    pyobj_ptr real_obj_;
    pyobj_ptr on_execute_method_;
    pyobj_ptr on_update_method_;
    pyobj_ptr on_collide_method_;
    u32_t execute_generator_: 1;

    PyObject* Obj() { return +real_obj_ ? +real_obj_ : (PyObject*) wrapper_; }
    PyObject *__init__(PyObject *,PyObject *) { return Refe(_PoC_None); }

    enum { UNKNOWN = 0, SUPPORTED = 1, UNSUPPORTED = 2, SELF_SUPPORTED = 3 };

    enum
      {
        ON_UPDATE_SHIFT  = 0,
        ON_COLLIDE_SHIFT = 2,
        ON_MOUSECLICK_SHIFT = 4,
        ON_MOUSEUP_SHIFT =  6,
        ON_MOUSEDOWN_SHIFT = 8 ,
        ON_MOUSEMOVE_SHIFT = 10,
        ON_MOUSEOVER_SHIFT = 12,
        ON_DISPOSE_SHIFT = 14,
        ON_LIFETIMEOUT_SHIFT = 16,
        ON_EMISSION_SHIFT = 18,
        ON_EXECUTE_SHIFT = 20,
        ON_STOPANIMATION_SHIFT = 22,
        ON_STOPSOUND_SHIFT = 24,
        ON_STOPEMISSION_SHIFT = 26,
      };

    PyDispatcher() : wrapper_(0), flags_(0)
      {
        execute_generator_ = 0;
      }

    ~PyDispatcher()
      {
        Unrefe(real_obj_);
        Unrefe(on_execute_method_);
        Unrefe(on_update_method_);
        Unrefe(on_collide_method_);
      }

    virtual PyObjectWrapper_ *IComPyUnknown_Get(bool increfcnt);
    virtual void IComPyUnknown_Unlink()
      {
        wrapper_ = 0;
      };

    HRESULT DoDispatcherCall(PyObject *args, pchar_t method, unsigned shift);
    HRESULT InternalQueryInterface2_0(pchar_t,void **,unsigned ,IUnknown *, PyObject **);
    virtual HRESULT InternalQueryInterface2(REFIID riid ,void ** pI);
    virtual HRESULT __stdcall OnUpdate(ISprite* sprite,long tickdelta);
    virtual HRESULT __stdcall OnCollide(ISprite* sprite,ISprite* hit);
    virtual HRESULT __stdcall OnDispose(IDispatch* obj);
    virtual HRESULT __stdcall OnMouseDown(ISprite* sprite, float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseUp(ISprite* sprite, float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseClick(ISprite* sprite, float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseMove(ISprite* sprite, float x, float y);
    virtual HRESULT __stdcall OnMouseOver(ISprite* sprite, VARIANT_BOOL mouseIn);
    virtual HRESULT __stdcall OnLifetimeOut(IParticles* particles);
    virtual HRESULT __stdcall OnEmission(IParticles* particles, long tickdelta);
    virtual HRESULT __stdcall Execute(IQue *que,long *sleeptime);
    virtual HRESULT __stdcall OnStopAnimation(ISprite *sprite, long cookie, VARIANT_BOOL interrupted);
    virtual HRESULT __stdcall OnStopEmission(IParticles *sprite, long cookie, VARIANT_BOOL interrupted);
    virtual HRESULT __stdcall OnStopSound( BSTR sound, long channel, long cookie, VARIANT_BOOL interrupted);

    static void _PoC_CALL Py_Deallocate(PyObject *obj)
      {
        PyDispatcherWrapper *w = (PyDispatcherWrapper*)obj;
        if ( w->dispatcher_ )
          {
            w->dispatcher_->wrapper_ = 0;
            Unrefe(w->dispatcher_);
          }
        obj->ob_type->tp_free(obj);
      }

    static PyObject * _PoC_CALL Py_New(PyTypeObject *type, PyObject *args, PyObject *kwds);
    static int _PoC_CALL Py_Init(PyObject *self, PyObject *args, PyObject *kwds);
  };

static void PyDispatcher_Exceptiion_Trap(pwide_t prefix)
  {
    if ( PyObject *err = _PoC_Err_Occurred() )
      {
        PyObject *ptype;
        PyObject *pvalue;
        PyObject *ptraceback;

        _PoC_Err_Fetch(&ptype,&pvalue,&ptraceback);
        _PoC_Err_NormalizeException(&ptype,&pvalue,&ptraceback);

        PyObject *type_repr =
          ptype ? _PoC_Object_Unicode(ptype) :
                  _PoC_Unicode_FromUnicode1(L"?EXCEPTION?");
        PyObject *value_repr =
          pvalue ? _PoC_Object_Unicode(pvalue) :
                   _PoC_Unicode_FromUnicode1(L"?INFO?");

        Lout << _S*"<PYTHON/%?> %?\n   %?"
            %prefix
            %PyUnicode_AS_UNICODE(type_repr)
            %PyUnicode_AS_UNICODE(value_repr);

        if ( PyTracebackObject *tb = (PyTracebackObject *)ptraceback )
          {
            Lout << "  |---- traceback ----";
            while (tb)
              {
                Lout << _S*"   in %s, line %d, file %s"
                  %PyString_AS_STRING(tb->tb_frame->f_code->co_name)
                  %tb->tb_lineno
                  %PyString_AS_STRING(tb->tb_frame->f_code->co_filename)
                  ;
                tb = tb->tb_next;
              }
            Lout << "  |-------------------";
          }

        Py_DECREF(type_repr);
        Py_DECREF(value_repr);
        _PoC_Err_Clear();
      }
  }

PyObject *_Py_Find_Callable(pchar_t name, PyObject *o, bool *selfis)
  {

    if ( !o ) return 0;

    if ( PyObject *o0 = _PoC_Object_GetAttrString(o,(char*)name))
      {
        *selfis = false;
        return o0;
      }
    else
      {
        _PoC_Err_Clear();
        if ( o->ob_type == _PoC_Method_Type
          || o->ob_type == _PoC_Function_Type )
          {
            *selfis = true;
            return Refe(o);
          }
        else
          return 0;
      }
  }

HRESULT PyDispatcher::InternalQueryInterface2_0(pchar_t method,void **pI,unsigned shift,IUnknown *val, PyObject **method_obj)
  {
    unsigned selector = ( flags_ >> shift ) & 0x03;
    if ( selector == UNKNOWN )
      {
        bool selfis;
        PyObject *m = _Py_Find_Callable( method, Obj(), &selfis );
        if ( !m && shift == ON_EXECUTE_SHIFT && Obj()->ob_type == _PoC_Gen_Type )
          {
            m = _Py_Find_Callable( "next", Obj(), &selfis );
            if ( m ) execute_generator_ = 1;
          }
        if ( m )
          {
            if ( selfis || !method_obj )
              {
                Unrefe(m);
                if (method_obj) *method_obj = 0;
              }
            selector = SUPPORTED;
            if ( shift == ON_EXECUTE_SHIFT )
              {
                if (method_obj) *method_obj = m;
              }
            else if ( !selfis )
              {
                selector = SELF_SUPPORTED;
                if ( method_obj )
                  {
                    *method_obj = _PoC_Method_Function(m);
                    Py_INCREF(*method_obj);
                    Py_DECREF(m);
                  }
              }
          }
        else
          selector = UNSUPPORTED;
        flags_ |= ( selector << shift );
      }
    if ( selector & 0x01 )
      return (*pI = Refe(val), S_OK);
    else
      return E_NOINTERFACE;
  }

HRESULT PyDispatcher::InternalQueryInterface2(REFIID riid ,void **pI)
  {
    pyobj_ptr fake;

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IComPyUnknown)) )
      return *pI = Refe((IComPyUnknown*)this), S_OK;

    if ( IsEqualIID(riid,TEGGO_GUIDOF(ICommand)) )
      return InternalQueryInterface2_0(
        "_OnExecute",pI,ON_EXECUTE_SHIFT,(ICommand*)this,&+on_execute_method_);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnUpdate)) )
      return InternalQueryInterface2_0(
        "_OnUpdate",pI,ON_UPDATE_SHIFT,(IOnUpdate*)this,&+on_update_method_);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnCollide)) )
      return InternalQueryInterface2_0(
        "_OnCollide",pI,ON_COLLIDE_SHIFT,(IOnCollide*)this,&+on_collide_method_);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnLifetimeOut)) )
      return InternalQueryInterface2_0(
        "_OnLifetimeOut",pI,ON_LIFETIMEOUT_SHIFT,(IOnLifetimeOut*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseClick)) )
      return InternalQueryInterface2_0(
        "_OnMouseClick",pI,ON_MOUSECLICK_SHIFT,(IOnMouseClick*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseDown)) )
      return InternalQueryInterface2_0(
        "_OnMouseDown",pI,ON_MOUSEDOWN_SHIFT,(IOnMouseDown*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseUp)) )
      return InternalQueryInterface2_0(
        "_OnMouseUp",pI,ON_MOUSEUP_SHIFT,(IOnMouseUp*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseMove)) )
      return InternalQueryInterface2_0(
        "_OnMouseMove",pI,ON_MOUSEMOVE_SHIFT,(IOnMouseMove*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnMouseOver)) )
      return InternalQueryInterface2_0(
        "_OnMouseOver",pI,ON_MOUSEOVER_SHIFT,(IOnMouseOver*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnEmission)) )
      return InternalQueryInterface2_0(
       "_OnEmission",pI,ON_EMISSION_SHIFT,(IOnEmission*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnStopEmission)) )
      return InternalQueryInterface2_0(
       "_OnStopEmission",pI,ON_STOPEMISSION_SHIFT,(IOnStopEmission*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnStopAnimation)) )
      return InternalQueryInterface2_0(
       "_OnStopAnimation",pI,ON_STOPANIMATION_SHIFT,(IOnStopAnimation*)this,0);

    if ( IsEqualIID(riid,TEGGO_GUIDOF(IOnStopSound)) )
      return InternalQueryInterface2_0(
       "_OnStopSound",pI,ON_STOPSOUND_SHIFT,(IOnStopSound*)this,0);

    return E_NOINTERFACE;
  }

HRESULT PyDispatcher::DoDispatcherCall(PyObject *args, pchar_t method, unsigned shift)
  {
    if ( (( flags_ >> shift ) & 0x01) == SUPPORTED )
      {
        if ( pyobj_ptr obj = RccRefe(Obj()) )
          {
            bool selfis;
            pyobj_ptr collable ( _Py_Find_Callable(method, +obj, &selfis) );
            STRICT_REQUIRE(+collable != 0);
            pyobj_ptr retval ( _PoC_Object_CallObject(+collable,args) );
            if ( +retval )
              return S_OK;
            else
              return PyDispatcher_Exceptiion_Trap(
                      (_S*L"scraft.Dispatcher::%?" %method).Str()),
                     E_FAIL;
          }
        else
          {
            return E_FAIL;
          }
      }
    else
      return E_NOTIMPL;
  }

HRESULT __stdcall PyDispatcher::Execute(IQue *que,long *sleeptime)
  {
    if ( ( flags_ >> ON_EXECUTE_SHIFT ) & 0x01 )
      {
        if ( pyobj_ptr obj = RccRefe(Obj()) )
          {
            pyobj_ptr retval;
            if ( !execute_generator_ )
              {
                pyobj_ptr args ( _PoC_Tuple_New(1) );
                rcc_ptr<IComPyUnknown> icpu = teggo::ifs_cast<IComPyUnknown>(que);
                PyTuple_SET_ITEM(+args,0,(PyObject*)icpu->IComPyUnknown_Get(true));
                retval = RccPtr( on_execute_method_
                  ? _PoC_Object_CallObject(+on_execute_method_,+args)
                  : _PoC_Object_CallObject(+obj,+args) );
              }
            else
              {
                retval =
                  RccPtr(_PoC_Object_CallObject(+on_execute_method_,
                                                +pyobj_ptr(_PoC_Tuple_New(0))));
              }

            if ( +retval )
              {
                HRESULT hr = S_OK;
                if ( retval->ob_type == _PoC_Int_Type )
                  *sleeptime = PyInt_AS_LONG(+retval);
                else if ( retval->ob_type == _PoC_Float_Type )
                  *sleeptime = (long)(PyFloat_AS_DOUBLE(+retval));
                else if ( retval->ob_type == _PoC_Gen_Type )
                  {
                    execute_generator_ = 1;
                    on_execute_method_ = RccPtr(_PoC_Object_GetAttrString(+retval,"next"));
                    return PyDispatcher::Execute(que,sleeptime);
                  }
                else if ( +retval == _PoC_None )
                  *sleeptime = CommandStateEnd;
                else
                  {
                    Lout << _S*"<PYTHON/%?> %?\n   %?"
                      %L"scraft.Dispatcher::_OnExecute"
                      %"-"
                      %"invalid return vlaue";
                    hr = E_FAIL;
                  }
                return hr;
              }
            else if ( execute_generator_ && _PoC_Err_Occurred() == *_PoC_StopIteration )
              {
                _PoC_Err_Clear();
                *sleeptime = CommandStateEnd;
                return S_OK;
              }
            else
              {
                PyDispatcher_Exceptiion_Trap(L"scraft.Dispatcher::_OnExecute");
                return E_FAIL;
              }
          }
        else
          {
            return E_FAIL;
          }
      }
    else
      return E_NOTIMPL;
  }

HRESULT __stdcall PyDispatcher::OnUpdate(ISprite* sprite,long tickdelta)
  {
    if ( ( flags_ >> ON_UPDATE_SHIFT ) & 0x01 )
      {
        if ( pyobj_ptr obj = RccRefe(Obj()) )
          {
            int args_pos = 0;
            bool need_self = ( ((flags_ >> ON_UPDATE_SHIFT ) & 0x02) != 0 );
            pyobj_ptr args( _PoC_Tuple_New(need_self ? 3 : 2) );
            rcc_ptr<IComPyUnknown> icpu = teggo::ifs_cast<IComPyUnknown>(sprite);

            if ( need_self )
              {
                Py_INCREF(+obj);
                PyTuple_SET_ITEM(+args,args_pos++,+obj);
              }

            PyTuple_SET_ITEM(+args,args_pos++,(PyObject*)icpu->IComPyUnknown_Get(true));
            PyTuple_SET_ITEM(+args,args_pos++,_PoC_Int_FromLong(tickdelta));

            pyobj_ptr retval (
              on_update_method_
                ? _PoC_Object_CallObject(+on_update_method_,+args)
                : _PoC_Object_CallObject(+obj,+args) );

            if ( +retval )
              return S_OK;
            else
              return PyDispatcher_Exceptiion_Trap(L"scraft.Dispatcher::_OnUpdate"), E_FAIL;
          }
        else
          {
            return E_FAIL;
          }
      }
    else
      return E_NOTIMPL;
  }

HRESULT __stdcall PyDispatcher::OnCollide(ISprite* sprite,ISprite* hit)
  {
    if ( ( flags_ >> ON_COLLIDE_SHIFT ) & 0x01 )
      {
        if ( pyobj_ptr obj = RccRefe(Obj()) )
          {
            int args_pos = 0;
            bool need_self = ( ((flags_ >> ON_UPDATE_SHIFT ) & 0x02) != 0 );
            pyobj_ptr args( _PoC_Tuple_New(need_self ? 3 : 2) );
            rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(sprite);
            rcc_ptr<IComPyUnknown> icpu1 = teggo::ifs_cast<IComPyUnknown>(hit);

            if ( need_self )
              {
                Py_INCREF(+obj);
                PyTuple_SET_ITEM(+args,args_pos++,+obj);
              }

            PyTuple_SET_ITEM(+args,args_pos++,(PyObject*)icpu0->IComPyUnknown_Get(true));
            PyTuple_SET_ITEM(+args,args_pos++,(PyObject*)icpu1->IComPyUnknown_Get(true));

            pyobj_ptr retval (
              on_collide_method_
                ? _PoC_Object_CallObject(+on_collide_method_,+args)
                : _PoC_Object_CallObject(+obj,+args) );

            if ( +retval )
              return S_OK;
            else
              return PyDispatcher_Exceptiion_Trap(L"scraft.Dispatcher::_OnCollide"), E_FAIL;
          }
        else
          {
            return E_FAIL;
          }
      }
    else
      return E_NOTIMPL;
  }

HRESULT __stdcall PyDispatcher::OnDispose(IDispatch* obj)
  {
    pyobj_ptr args ( _PoC_Tuple_New(1) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(obj);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    return DoDispatcherCall(+args,"_OnDispose",ON_DISPOSE_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnMouseDown(ISprite* sprite, float x, float y,long btn)
  {
    pyobj_ptr args ( _PoC_Tuple_New(4) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(sprite);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Float_FromDouble(x));
    PyTuple_SET_ITEM(+args,2,_PoC_Float_FromDouble(y));
    PyTuple_SET_ITEM(+args,3,_PoC_Int_FromLong(btn));
    return DoDispatcherCall(+args,"_OnMouseDown",ON_MOUSEDOWN_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnMouseUp(ISprite* sprite, float x, float y,long btn)
  {
    pyobj_ptr args ( _PoC_Tuple_New(4) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(sprite);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Float_FromDouble(x));
    PyTuple_SET_ITEM(+args,2,_PoC_Float_FromDouble(y));
    PyTuple_SET_ITEM(+args,3,_PoC_Int_FromLong(btn));
    return DoDispatcherCall(+args,"_OnMouseUp",ON_MOUSEUP_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnMouseClick(ISprite* sprite, float x, float y,long btn)
  {
    pyobj_ptr args ( _PoC_Tuple_New(4) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(sprite);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Float_FromDouble(x));
    PyTuple_SET_ITEM(+args,2,_PoC_Float_FromDouble(y));
    PyTuple_SET_ITEM(+args,3,_PoC_Int_FromLong(btn));
    return DoDispatcherCall(+args,"_OnMouseClick",ON_MOUSECLICK_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnMouseMove(ISprite* sprite, float x, float y)
  {
    pyobj_ptr args ( _PoC_Tuple_New(3) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(sprite);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Float_FromDouble(x));
    PyTuple_SET_ITEM(+args,2,_PoC_Float_FromDouble(y));
    return DoDispatcherCall(+args,"_OnMouseMove",ON_MOUSEMOVE_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnMouseOver(ISprite* sprite, VARIANT_BOOL mouseIn)
  {
    pyobj_ptr args ( _PoC_Tuple_New(2) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(sprite);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Bool_FromVARBOOL(mouseIn));
    return DoDispatcherCall(+args,"_OnMouseOver",ON_MOUSEOVER_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnLifetimeOut(IParticles* particles)
  {
    pyobj_ptr args ( _PoC_Tuple_New(1) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(particles);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    return DoDispatcherCall(+args,"_OnLifetimeOut",ON_LIFETIMEOUT_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnEmission(IParticles* particles, long tickdelta)
  {
    pyobj_ptr args ( _PoC_Tuple_New(2) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(particles);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Int_FromLong(tickdelta));
    return DoDispatcherCall(+args,"_OnEmission",ON_EMISSION_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnStopAnimation(ISprite *sprite, long cookie, VARIANT_BOOL interrupted)
  {
    pyobj_ptr args ( _PoC_Tuple_New(3) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(sprite);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Int_FromLong(cookie));
    PyTuple_SET_ITEM(+args,2,_PoC_Bool_FromVARBOOL(interrupted));
    return DoDispatcherCall(+args,"_OnStopAnimation",ON_STOPANIMATION_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnStopEmission(IParticles *particles, long cookie, VARIANT_BOOL interrupted)
  {
    pyobj_ptr args ( _PoC_Tuple_New(3) );
    rcc_ptr<IComPyUnknown> icpu0 = teggo::ifs_cast<IComPyUnknown>(particles);
    PyTuple_SET_ITEM(+args,0,(PyObject*)icpu0->IComPyUnknown_Get(true));
    PyTuple_SET_ITEM(+args,1,_PoC_Int_FromLong(cookie));
    PyTuple_SET_ITEM(+args,2,_PoC_Bool_FromVARBOOL(interrupted));
    return DoDispatcherCall(+args,"_OnStopEmission",ON_STOPEMISSION_SHIFT);
  }

HRESULT __stdcall PyDispatcher::OnStopSound( BSTR sound, long channel, long cookie, VARIANT_BOOL interrupted)
  {
    pyobj_ptr args ( _PoC_Tuple_New(4) );
    PyTuple_SET_ITEM(+args,0,_PoC_Unicode_FromUnicode1(sound));
    PyTuple_SET_ITEM(+args,1,_PoC_Int_FromLong(channel));
    PyTuple_SET_ITEM(+args,2,_PoC_Int_FromLong(cookie));
    PyTuple_SET_ITEM(+args,3,_PoC_Bool_FromVARBOOL(interrupted));
    return DoDispatcherCall(+args,"_OnStopSound",ON_STOPSOUND_SHIFT);
  }

#define Dispatcher_NAME "Dispatcher"

static PyTypeObject PyDispatcherWrapper_Type =
  {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "scraft."Dispatcher_NAME,  /*tp_name*/
    sizeof(PyDispatcherWrapper),  /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    PyDispatcher::Py_Deallocate,  /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "spritecraft object dispatcher", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,                         /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    &PyDispatcher::Py_Init,     /* tp_init */
    0,                         /* tp_alloc */
    &PyDispatcher::Py_New,      /* tp_new */
  };

int _PoC_CALL PyDispatcher::Py_Init(PyObject *self, PyObject *args, PyObject *kwds)
  {
    PyDispatcherWrapper *w = (PyDispatcherWrapper*)self;
    PyTupleObject *tpl = (PyTupleObject*)args;
    if ( w->dispatcher_->real_obj_ == 0 && self->ob_type == &PyDispatcherWrapper_Type )
      {
        REQUIRE ( PyTuple_GET_SIZE(tpl) == 1 && PyTuple_GET_ITEM(tpl,0) != 0 );
        w->dispatcher_->real_obj_.reset_(PyTuple_GET_ITEM(tpl,0));
        Py_INCREF(+w->dispatcher_->real_obj_);
      }
    return 0;
  }

PyObject *PyDispatcher::Py_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    PyTupleObject *tpl = (PyTupleObject*)args;
    if ( type == &PyDispatcherWrapper_Type )
      {
        if ( PyTuple_GET_SIZE(tpl) != 1 || !PyTuple_GET_ITEM(tpl,0) )
             return _PoC_RaiseExcept("failed to wrap, need object"), 0;

        PyTypeObject *tp = PyTuple_GET_ITEM(tpl,0)->ob_type;

        if ( tp == _PoC_Type_Type )
          return _PoC_RaiseTypeExcept("failed to wrap type object, need instance"), 0;

        while (tp)
          if ( tp->tp_dealloc == Py_Object_Dealloc )
            return Refe(PyTuple_GET_ITEM(tpl,0));
          else
            tp = tp->tp_base;
      }

    //printf("type %s, (%s)\n",type->tp_name,PyTuple_GET_SIZE(tpl)?PyTuple_GET_ITEM(tpl,0)->ob_type->tp_name:"<0>");

    rcc_ptr<PyDispatcher> p(new PyDispatcher());
    p->wrapper_ = (PyDispatcherWrapper*)type->tp_alloc(type, 0);
    p->wrapper_->dispatcher_ = Refe(p);
    return (PyObject *)p->wrapper_;
  }

PyObjectWrapper_ *PyDispatcher::IComPyUnknown_Get(bool increfcnt)
  {
    REQUIRE (wrapper_);
    if ( increfcnt )
      {
        Py_INCREF(wrapper_);
      }
    return (PyObjectWrapper_*)wrapper_;
  }

IUnknown *PyDispatcher_Marshal(PyObject *o)
  {
    PyDispatcherWrapper *w = (PyDispatcherWrapper*)o;
    return (IComPyUnknown*)w->dispatcher_;
  }

HRESULT __stdcall PyDispatch_RegisterPyType()
  {
    _PoC_Type_Ready(&PyDispatcherWrapper_Type);
    Py_INCREF(&PyDispatcherWrapper_Type);
    _PoC_Module_AddObject(_PoC_module,Dispatcher_NAME,(PyObject*)&PyDispatcherWrapper_Type);
    PyTypeInfoFactory().AddObjectMarshal(&PyDispatcherWrapper_Type,PyDispatcher_Marshal);
    return S_OK;
  }
