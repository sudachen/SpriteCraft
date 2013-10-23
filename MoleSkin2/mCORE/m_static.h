
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

#if !defined __m_static_h__
#define __m_static_h__

#if !defined __m_autoconf_h__
#error "m_autoconf.h must be included before"
#endif

#if ( CXX_COMPILER_ == CXX_COMPILER__MSVC )
#endif

#if !defined MSKIN_DISABLE_STATIC_CHECK
template<bool> struct static_check;
template<> struct static_check<true> { typedef bool success; };
#endif

template<class T> inline int elnof(T& t) { 
  return sizeof(t)/sizeof(t[0]); 
}

template<class T> inline bool in_range(unsigned idx,T& t) { 
  return idx < sizeof(t)/sizeof(t[0]); 
}

//#if defined __STL_CLASS_PARTIAL_SPECIALIZATION || defined __NOSTLSGI
// template<class X>
// inline reverse_iterator<X*> preverse(X* it) 
// { return reverse_iterator<X*>(it); }
//#else
//template<class X>
//inline reverse_iterator<X*,X> preverse(X* it) 
//{ return reverse_iterator<X*,X>(it); }
//#endif
//
//template<class T>
//inline T teggo_min(T a,T b) { return a <= b ? a : b; }
//
//template<class T>
//inline T teggo_max(T a,T b) { return a >= b ? a : b; }

/*
inline string Lower(pchar_t c) {
  if ( !c ) return "";
  string cc = c;
  transform(cc.begin(),cc.end(),cc.begin(),tolower);
  return cc;
} 

inline string Lower(string cc) {
  transform(cc.begin(),cc.end(),cc.begin(),tolower);
  return cc;
} 
*/

#endif //__m_static_h__



