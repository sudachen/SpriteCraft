
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

#if !defined (__m_autoref_h__)
#define __m_autoref_h__

NAMESPACE_MSKIN_BEGIN

/**
  простая реализация автоматического указателя на 
  обьект имеющий механизм подсчетом ссылок
*/
template<class T> class auto_ref
{
public: // из за проблем с темплетами френдами
  mutable T* ref_;
  void reassign__(T* t) { T* tmp = t; teggo_swap(tmp,ref_); if ( tmp ) tmp->Release_(); }
  void release__() { if ( ref_ ) { ref_->Release_(); ref_ = 0; } }
  T* forget__() { T* x = ref_; ref_ = 0; return x; }
//  struct ref0 { mutable T* ref_; explicit ref0(T* r) : ref_(r) {} };
public:
  explicit auto_ref(T* t=0) : ref_(t) {}
  auto_ref(const auto_ref& a) : ref_(a.ref_) { if ( ref_ ) ref_->Grab_(); }
//  auto_ref(const ref0& a) : ref_(a.ref_) {}
  ~auto_ref() { if ( ref_ ) { ref_->Release_(); ref_ = 0; } }
  operator T*() const { return ref_; }
  T*& operator ~() const { return ref_; }
  T*& operator +() const { return ref_; }
  T& operator *() const { return *ref_; }
  T* operator ->() const { return ref_; }
  template<class U>
  const auto_ref& operator=(const auto_ref<U>& a) { 
    if ( a.ref_ ) a.ref_->Grab_(); reassign__(a.ref_); return *this;
  }
  const auto_ref& operator=(const auto_ref<T>& a) { 
    if ( a.ref_ ) a.ref_->Grab_(); reassign__(a.ref_); return *this;
  }
  bool operator == ( const auto_ref& a ) { return ref_ == a.ref_; }
private:  
  // фикс для бага MSVC 6.0
  void operator=(const T*);
};

template<class T_> inline T_* Grab(const auto_ref<T_>& ref) { if ( ref.ref_ ) ref.ref_->Grab_(); return ref.ref_; }
template<class T_> inline T_* Get(const auto_ref<T_>& ref) { return ref.ref_; }
template<class T_> inline T_** GetRef(auto_ref<T_>& ref) { return &ref.ref_; }
template<class T_> inline T_* const * GetRef(const auto_ref<T_>& ref) { return &ref.ref_; }
template<class T_> inline bool IsNull(const auto_ref<T_>& ref) { return ref.ref_==0; }
template<class T_> inline T_* Forget(auto_ref<T_>& ref) { return ref.forget__(); }
template<class T_> inline void Reset(auto_ref<T_>& ref,T_* p) { ref.reassign__(p); }

template <class T> inline auto_ref<T> AutoRef(T* ref) { return auto_ref<T>(ref); }
template <class T> inline T* Grab(T* ref) { if ( ref ) ref->Grab_(); return ref; }
template <class T> inline void Release(T*& ref) { if ( ref ) ref->Release_(); ref = 0; }
template <class T> inline void Release(auto_ref<T>& ref) { ref.release__(); }

NAMESPACE_MSKIN_END

#endif // __mskin_refptr_h__
