
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

#if !defined (__m_nonstatic_h__)
#define __m_nonstatic_h__

#include "m_autoconf.h"

NAMESPACE_MSKIN_BEGIN

template<class T>
class NonStatic {
public: // в силу не понимания некоторыми компилерами темплейтных френдов
  byte_t _bits[sizeof(T)];
public:
  T& operator* () { return *(T*)_bits; }
  T* operator->() { return (T*)_bits; }
  operator T* () { return (T*)_bits; }  
private: 
// не получится в силу того что у этого класа ДОЛЖЕН быть тривиальный конструктор
  //NonStatic(NonStatic&);
  //NonStatic operator = (NonStatic&);
};

template <class T> 
inline void InitNonStatic(NonStatic<T>& nonstatic) {
  new (nonstatic._bits) T;
}

template <class T, class A> 
inline void InitNonStatic(NonStatic<T>& nonstatic, const A& arg) {
  new (nonstatic._bits) T(arg);
}

template <class T> 
inline void TermNonStatic(NonStatic<T>& nonstatic) {
  (*nonstatic).~T();
}

NAMESPACE_MSKIN_END

#endif // __m_nonstatic_h__
