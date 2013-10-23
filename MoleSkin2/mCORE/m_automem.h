
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


#if !defined (__m_automem_h__)
#define __m_automem_h__

#ifdef _MSC_VER
#pragma warning(disable:4284) 
// return type for 'a_ptr__<char>::operator ->' is 'char *' 
// (ie; not a UDT or reference to a UDT. 
// Will produce errors if applied using infix notation)
#endif

NAMESPACE_MSKIN_BEGIN

template<class T,class mem> 
struct a_ptr__{
	mutable T* ptr_;
  T* forget__() const { T* tmp = ptr_; ptr_=0; return tmp; }
  void reassign__(T* p) { 
    if ( ptr_==p ) return; 
    if (ptr_) mem::Delete(ptr_); 
    ptr_ = p;
  } 
	explicit a_ptr__(T* p=0) : ptr_(p) {}
	a_ptr__(const a_ptr__& p) {ptr_ = p.forget__();}
  ~a_ptr__() { if (ptr_) mem::Delete(ptr_); }
  T* operator->() const {return ptr_;}
  T* operator+(int i) const { return ptr_+i;}
  T& operator*() const { return *ptr_;}
  T& operator[](int i) const { return ptr_[i];}
  bool operator!() const { return !ptr_; }
  T*& operator ~() const { return ptr_; }
	const a_ptr__& operator = (const a_ptr__& p) {reassign__(p.forget__()); return *this;}
private:
  const a_ptr__& operator = (T*);
};

struct delete_one__
{ 
  template<class T> static void Delete(T* p) { delete p; } 
  template<class T> void operator ()(T*& p) const { delete p; p = 0; }
};

template<class T> 
struct auto_mem : a_ptr__<T,delete_one__> {
  auto_mem(T* p = 0) : a_ptr__<T,delete_one__> (p) {}
};
template<class T> inline auto_mem<T> AutoMem(T* p) { return auto_mem<T>(p); }

struct delete_array__
{ 
  template<class T> static void Delete(T* p) { if ( p != 0 ) delete[] p; }
  template<class T> void operator ()(T*& p) const { Delete(p); p = 0; }
};

template<class T> 
struct auto_arr : a_ptr__<T,delete_array__> {
  auto_arr(T* p = 0) : a_ptr__<T,delete_array__> (p) {}
};
template<class T> inline auto_arr<T> AutoArr(T* p) { return auto_arr<T>(p); }

template<class T,class mem> inline T** GetRef(a_ptr__<T,mem>& ap) { return &ap.ptr_; }
template<class T,class mem> inline T* const * GetRef(const a_ptr__<T,mem>& ap) { return &ap.ptr_; }
template<class T,class mem> inline T* Forget(a_ptr__<T,mem>& ap) { return ap.forget__(); }
template<class T,class mem> inline T* Reset(a_ptr__<T,mem>& ap,T* p) { ap.reassign__(p); return ap.ptr_; }
template<class T,class mem> inline T* Get(const a_ptr__<T,mem>& ap) { return ap.ptr_; }
template<class T,class mem> inline bool IsNull(const a_ptr__<T,mem>& ap) { return ap.ptr_ == 0; }
//template<class T,class mem> inline void Release(a_ptr__<T,mem>& ap) { ap.reassign__(0); }
template<class T,class mem> inline void Dispose(a_ptr__<T,mem>& ap) { ap.reassign__(0); }
template<class T> inline void Dispose(T*& ap) { delete ap; ap = 0; }

struct delete_self__
{ 
  template<class T> static void Delete(T* p) { if (p) p->DeleteSelf(); } 
  template<class T> void operator ()(T*& p) const { Delete(p); p = 0; }
};

NAMESPACE_MSKIN_END

#endif // __m_automem_h__
