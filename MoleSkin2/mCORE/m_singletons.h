
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

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

/** \file
  \brief механизм синглтонов
*/

#if !defined (__m_singletons_h__)
#define __m_singletons_h__

#include "m_autoconf.h"
#include "m_sync.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma warning(disable:4786)
#endif

NAMESPACE_MSKIN_BEGIN

struct AtExitHandler 
{
  virtual ~AtExitHandler() {}
};

/*
struct AtExitRelease : public AtExitHandler{
  IRefObject* object_;
  AtExitRelease(IRefObject* o) : object_(o) {}
  virtual ~AtExitRelease()  { object_->Release_(); }
};

template<class T>
struct AtExitDelete : public AtExitHandler{
  T* object_;
  AtExitDelete(T* o) : object_(o) {}
  virtual ~AtExitDelete()  { delete object_; }
};

template<class T>
struct AtExitDestroy : public AtExitHandler{
  T* object_;
  AtExitDestroy(T* o) : object_(o) {}
  virtual ~AtExitDestroy()  { if ( object_ ) object_->~T(); }
};
*/

/// Ставит синглтон на чет, в дальнейшем при завершении пограммы синглтон будет уничтожен
void MOLE_FUNCALL ManageObjectAtExit(AtExitHandler*, int);
/// Блокирует локер менеджера синглтонов
void MOLE_FUNCALL LockSingletonManager();
/// Разблокирует локер менеджера синглтонов
void MOLE_FUNCALL UnlockSingletonManager();
/// Внутренний класс, используется реализацией держателей сиглтонов
struct LockSingleTonManager{ 
  LockSingleTonManager(){LockSingletonManager();} 
  ~LockSingleTonManager(){UnlockSingletonManager();} 
};
/// Внутренняя функция, используется реализацией держателей синглтонов
template <class T>
inline T* ManageSingleton_(T* p,int level) { 
  ManageObjectAtExit((AtExitHandler*)p,level); return p; 
}

template<int destruction = 0,int sync = 1,int usingheap = 0>
struct SingletonTraits{
  enum { 
    DESTRUCTION_LEVEL = destruction,
    SYNCHRONIZED      = sync,
    USINGHEAP         = usingheap,
  };
};

typedef SingletonTraits<0,1,0> DefaultSingletonTraits;

/*

  Определяем классы управляющие жизненым циклом держателя синглтона

*/

/// Внутренний класс, создатель днопоточного держателя синглтона
template<class Holder,int level> 
struct SingletonCreator_unithread__
{
  typedef Holder* HolderPtr;
  static void MakeInstance(HolderPtr& holder,byte_t* bits){ 
    holder = ManageSingleton_( holder = new (bits) Holder, level );    
  }
};

/// Внутренний класс, создатель многопоточного держателя синглтона
template<class Holder,int level> 
struct SingletonCreator_multithread__
{
  typedef volatile Holder* HolderPtr;
  static void MakeInstance(HolderPtr& holder,byte_t* bits){ 
    /// локер всех сиглтонов !
    LockSingleTonManager _;
    if ( holder == 0 )
      holder = ManageSingleton_( holder = new (bits) Holder, level );    
  }
};

/// Держатель синглтона создающий синглтон в хипе
template<class STone>
struct SingletonHolder_dynamic__ : public AtExitHandler {
  STone* instance_;
  SingletonHolder_dynamic__() { instance_ = new STone; }
  STone* Instance() { return instance_; }
  virtual ~SingletonHolder_dynamic__() {delete instance_; instance_ = 0;}
};

/// Держатель синглтона создающий синглтон в статической памяти
template<class STone>
struct SingletonHolder_static__ : public AtExitHandler {
  byte_t bits_[sizeof(STone)];
  STone* instance_;
  SingletonHolder_static__() : instance_(0) { instance_ = new (bits_) STone; }
  STone* Instance() { return instance_; }
  virtual ~SingletonHolder_static__() { instance_->~STone(); instance_ = 0; }
};

/*
  А здесь начинается черная магия :)
  MSVC не поддерживает частичную специализацию шаблонов, но зато он поддерживает
  очень хитрый вариант специализации  внутри шаблонна структуры - фактически реализуя
  некоторое подобие частичной специализации.
  НО gcc/egcs этот код не поймут :( поэтому для них сделан нормальный 
  механизм с частичной специализацией
*/
#if CXX_COMPILER_IS_MSVC
template<class Holder,class Traits> struct SingletonCreator_helper__{
  enum{ SYNC = Traits::SYNCHRONIZED };
  enum{ LEVEL = Traits::DESTRUCTION_LEVEL };
  template<int> struct Helper__;
  template<> struct Helper__<0>: public SingletonCreator_unithread__<Holder,LEVEL> {};
  template<> struct Helper__<1> : public SingletonCreator_multithread__<Holder,LEVEL> {};
  typedef Helper__<SYNC> Helper;
};
template<class STone,class Traits>
struct SingletonHolder_helper__ {
  template<int> struct Helper__;
  template<> struct Helper__<0> : public SingletonHolder_static__<STone> {};
  template<> struct Helper__<1> : public SingletonHolder_dynamic__<STone> {};
  typedef Helper__<Traits::USINGHEAP> Helper;
};
template<class Holder,class Traits> 
struct SingletonCreator__ : public SingletonCreator_helper__<Holder,Traits>::Helper {};
template<class STone, class Traits>
struct SingletonHolder__ : public SingletonHolder_helper__<STone,Traits>::Helper{};
#else
template<class Holder,int level,int> struct SingletonCreator_helper__;
template<class Holder,int level> 
  struct SingletonCreator_helper__<Holder,level,0>:
    public SingletonCreator_unithread__<Holder,level> {};
template<class Holder,int level> 
  struct SingletonCreator_helper__<Holder,level,1>:
    public SingletonCreator_multithread__<Holder,level> {};
template<class Holder,class Traits> 
  struct SingletonCreator__ :
    public SingletonCreator_helper__<
       Holder,Traits::DESTRUCTION_LEVEL,Traits::SYNCHRONIZED> {};
template<class STone,int usingheap> struct SingletonHolder_helper__;
template<class STone> struct SingletonHolder_helper__<STone,0> :
  public SingletonHolder_static__<STone> {}; 
template<class STone> struct SingletonHolder_helper__<STone,1> :
  public SingletonHolder_dynamic__<STone> {}; 
template<class STone,class Traits> struct SingletonHolder__:
  public SingletonHolder_helper__<STone,Traits::USINGHEAP> {};
#endif

template<class STone, class Traits = DefaultSingletonTraits>
struct Singleton {
  typedef SingletonHolder__<STone,Traits> Holder;
  typedef SingletonCreator__<Holder,Traits> Creator;
  typedef typename SingletonCreator__<Holder,Traits>::HolderPtr HolderPtr;
  static STone* Instance();
};

template<class STone, class Traits>
STone* Singleton<STone,Traits>::Instance() {
  static byte_t bits[sizeof(Holder)];
  static HolderPtr holder = 0;
  if ( holder == 0 ) Creator::MakeInstance(holder,bits);
  return holder->instance_;
}

template<class T>
struct SingletonAccessor {
  static T* Instance_(T* x);
  static T* Instance() { return Instance_(0); }
  static T* SetInstance(T* x) { Instance_(x); return x; }
};

template<class T>
T* SingletonAccessor<T>::Instance_(T* x) {
    static T* instance = 0;
    if ( x ) instance = x;
    return instance;
}


NAMESPACE_MSKIN_END

#endif // __m_singletons_h__
