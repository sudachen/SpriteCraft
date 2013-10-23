
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

#if !defined (__m_irefobj_h__)
#define __m_irefobj_h__

#include "m_autoconf.h"
#include "m_debug.h"
#include "m_autoref.h"

NAMESPACE_MSKIN_BEGIN

/**
  простой класс для реализации обьектов со счечиком ссылок
*/
class refbase {
  mutable unsigned refcount_;
public:
  void Release_() const {
    {
      REQUIRE ( refcount_ > 0 );
      if ( --refcount_ != 0 ) return;
    }
    delete this;
  }
  void Grab_() const { 
    REQUIRE ( refcount_ > 0 );
    ++refcount_; 
  }
  unsigned GetRefCounter() const {
    return refcount_;
  }
protected:
  /// не может быть явно создан
  refbase() : refcount_(1) {}
  /// не может быть явно удален 
  virtual ~refbase(){
    //REQUIRE(refcount_ == 0);
  }
private:
  /// не может быть скопирован
  void operator = (const refbase&);
  /// не может быть скопирован
  refbase(const refbase&);
};

struct Adaptable {
  virtual void* Adaptate_(const void*);
  virtual widestring ToString_() const;
};

template<class T>
inline T* Adaptate(Adaptable* u) {
  return u?(T*)u->Adaptate_(T::TypeID__):0;
}

/// абстрактный базовый класс для обьектов с подсчетом ссылок
struct IRefObject: public virtual Adaptable {
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual void Release_() const = 0;
  virtual void Grab_() const = 0;
};
typedef auto_ref<IRefObject> AutoObject;


NAMESPACE_MSKIN_END

#endif // __mskin_irefobj_h__

