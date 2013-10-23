
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


#if !defined __m_graphics_types_h__
#define __m_graphics_types_h__

#include "../mcore/m_autoconf.h"

NAMESPACE_MSKIN_BEGIN

//template<class T>
//struct rect_T
//{
//  T left;
//  T top;
//  T right;
//  T bottom;
//  rect_T() {}
//  rect_T(T l,T t,T r,T b) : left(l),top(t),right(r),bottom(b) {}
//  
//  rect_T shift(T dx,T dy) const
//  { 
//    return rect_T(left+dx,top+dy,right+dx,bottom+dy);
//  }
//  
//  rect_T& shift_self(T dx,T dy)
//  { 
//    left    +=dx;
//    top     +=dy;
//    right   +=dx;
//    bottom  +=dy;
//    return *this;
//  }
//
//  rect_T move(T x,T y) const
//  { 
//    return rect_T(x,y,x+right-left,y+bottom-top);
//  }
//  
//  rect_T& move_self(T x,T y)
//  { 
//    right = x+right-left;
//    bottom = y+bottom-top;
//    left = x;
//    top = y;
//    return *this;
//  }
//
//  rect_T& clip_self(T x, T y)
//  {
//    if ( teggo_max(right-left,0) > x ) right = teggo_max(left+x,0);
//    if ( teggo_max(bottom-top,0) > y ) bottom = teggo_max(top+y,0);
//    return *this;
//  }
//
//  rect_T intersection(const rect_T& r) const
//  {
//    T x_right   = /*std::*/teggo_min(r.right,right);
//    T x_bottom  = /*std::*/teggo_min(r.bottom,bottom);
//    T x_left    = /*std::*/teggo_max(r.left,left);
//    T x_top     = /*std::*/teggo_max(r.top,top);
//    if ( x_right  < x_left ) return rect_T(0,0,0,0);
//    if ( x_bottom < x_top  ) return rect_T(0,0,0,0);
//    return rect_T(x_left,x_top,x_right,x_bottom);
//  }
//
//  rect_T& intersection_self(const rect_T& r)
//  {
//    right   = /*std::*/teggo_min(r.right,right);
//    bottom  = /*std::*/teggo_min(r.bottom,bottom);
//    left    = /*std::*/teggo_max(r.left,left);
//    top     = /*std::*/teggo_max(r.top,top);
//    if ( right  < left ) left = right = 0;
//    if ( bottom < top  ) bottom = top = 0;
//    return *this;
//  }
//
//  bool intersect(const rect_T& r) const 
//  {
//    T x_right   = /*std::*/teggo_min(r.right,right);
//    T x_left    = /*std::*/teggo_max(r.left,left);
//    if ( x_right  < x_left ) return false;
//    T x_bottom  = /*std::*/teggo_min(r.bottom,bottom);
//    T x_top     = /*std::*/teggo_max(r.top,top);
//    if ( x_bottom < x_top  ) return false;
//    return true;
//  }
//
//  bool contains(const rect_T& r) const
//  {
//    return  r.left >= left && r.top >= top && 
//            r.right <= right && r.bottom <= bottom &&
//            r.left <= r.right && r.top <= r.bottom;
//  }
//
//  bool operator ==(const rect_T& r) const
//  {
//    return  memcmp(this,&r,sizeof(rect_T)) == 0;
//  }
//
//  bool is_zero() const
//  {
//    return left == right || top == bottom;
//  }
//
//  T width() const 
//  {
//    return right-left;
//  }
//
//  T height() const 
//  {
//    return bottom-top;
//  }
//};
//
//typedef rect_T<int>   rect_t;
//typedef rect_T<float> frect_t;

NAMESPACE_MSKIN_END

#endif // __m_graphics_types_h__
