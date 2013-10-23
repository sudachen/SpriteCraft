
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

#ifndef _nv_algebra_h_
#define _nv_algebra_h_

#include "../mcore/m_autoconf.h"
#include "../mcore/m_debug.h"

#include <math.h>
#include <float.h>

namespace nvmath {

typedef float nv_scalar;

#define nv_zero			    nv_scalar(0)
#define nv_zero_5       nv_scalar(0.5)
#define nv_one			    nv_scalar(1.0)
#define nv_two			    nv_scalar(2)
#define nv_half_pi      nv_scalar(3.14159265358979323846264338327950288419716939937510582 * 0.5)
#define nv_pi			      nv_scalar(3.14159265358979323846264338327950288419716939937510582)
#define nv_two_pi			  nv_scalar(3.14159265358979323846264338327950288419716939937510582 * 2.0)
#define nv_oo_pi			  (nv_one / nv_pi)
#define nv_oo_two_pi	  (nv_one / nv_two_pi)
#define nv_oo_255   	  (nv_one / nv_scalar(255))
#define nv_oo_128   	  (nv_one / nv_scalar(128))
#define nv_to_rad       (nv_pi / nv_scalar(180))
#define nv_to_deg       (nv_scalar(180) / nv_pi)
#define nv_eps		      nv_scalar(10e-6)
#define nv_double_eps	  (nv_scalar(10e-6) * nv_two)
#define nv_big_eps      nv_scalar(10e-6)
#define nv_small_eps    nv_scalar(10e-2)

struct vec2;
struct vec2t;
struct vec3;
struct vec3t;
struct vec4;
struct vec4t;

struct vec2
{
  vec2() { }
  vec2(nv_scalar x, nv_scalar y) : x(x), y(y) { }
  vec2(const nv_scalar* xy) : x(xy[0]), y(xy[1]) { }
  vec2(const vec2& u) : x(u.x), y(u.y) { }
  vec2(const vec2t&);
  vec2(const vec3&);
  vec2(const vec3t&);
  vec2t T() const;
  
  bool operator==(const vec2 & u)
  {
    return (u.x == x && u.y == y) ? true : false;
  }
  
  vec2 & operator*=(const nv_scalar & lambda)
  {
    x*= lambda;
    y*= lambda;
    return *this;
  }
  
  vec2 & operator-=(const vec2 & u)
  {
    x-= u.x;
    y-= u.y;
    return *this;
  }
  
  vec2 & operator+=(const vec2 & u)
  {
    x+= u.x;
    y+= u.y;
    return *this;
  }
  
  nv_scalar & operator[](int i)
  {
    return vec_array[i];
  }
  
  const nv_scalar operator[](int i) const
  {
    return vec_array[i];
  }
  
  union {
    struct {
      nv_scalar x,y;          // standard names for components
    };
    struct {
      nv_scalar s,t;          // standard names for components
    };
    nv_scalar vec_array[2];     // array access
  };
};

inline const vec2 operator+(const vec2& u, const vec2& v)
{
  return vec2(u.x + v.x, u.y + v.y);
}

inline const vec2 operator-(const vec2& u, const vec2& v)
{
  return vec2(u.x - v.x, u.y - v.y);
}

inline const vec2 operator*(const nv_scalar s, const vec2& u)
{
  return vec2(s * u.x, s * u.y);
}

inline const vec2 operator/(const vec2& u, const nv_scalar s)
{
  return vec2(u.x / s, u.y / s);
}

inline const vec2 operator*(const vec2&u, const vec2&v)
{
  return vec2(u.x * v.x, u.y * v.y);
}

struct vec3
{
  vec3() { }
  vec3(nv_scalar x, nv_scalar y, nv_scalar z) : x(x), y(y), z(z) { }
  vec3(const nv_scalar* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
  vec3(const vec2& u) : x(u.x), y(u.y), z(1.0f) { }
  vec3(const vec2t& u);
  vec3(const vec3& u) : x(u.x), y(u.y), z(u.z) { }
  vec3(const vec3t&);
  vec3(const vec4&);
  vec3(const vec4t&);
  vec3t T() const;
  
  bool operator==(const vec3 & u) const
  {
    return (u.x == x && u.y == y && u.z == z) ? true : false;
  }
  
  bool operator!=( const vec3& rhs ) const
  {
    return !(*this == rhs );
  }
  
  vec3 & operator*=(const nv_scalar & lambda)
  {
    x*= lambda;
    y*= lambda;
    z*= lambda;
    return *this;
  }
  
  vec3 operator - () const
  {
    return vec3(-x, -y, -z);
  }
  
  vec3 & operator-=(const vec3 & u)
  {
    x-= u.x;
    y-= u.y;
    z-= u.z;
    return *this;
  }
  
  vec3 & operator+=(const vec3 & u)
  {
    x+= u.x;
    y+= u.y;
    z+= u.z;
    return *this;
  }
  nv_scalar normalize();
  nv_scalar sq_norm() const { return x * x + y * y + z * z; }
  nv_scalar norm() const { return sqrtf(sq_norm()); }
  
  nv_scalar & operator[](int i)
  {
    return vec_array[i];
  }
  
  const nv_scalar operator[](int i) const
  {
    return vec_array[i];
  }
  
  union {
    struct {
      nv_scalar x,y,z;        // standard names for components
    };
    struct {
      nv_scalar s,t,r;        // standard names for components
    };
    nv_scalar vec_array[3];     // array access
  };
};

inline const vec3 operator+(const vec3& u, const vec3& v)
{
  return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline const vec3 operator-(const vec3& u, const vec3& v)
{
  return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline const vec3 operator^(const vec3& u, const vec3& v)
{
  return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline const vec3 operator*(const nv_scalar s, const vec3& u)
{
  return vec3(s * u.x, s * u.y, s * u.z);
}

inline const vec3 operator/(const vec3& u, const nv_scalar s)
{
  return vec3(u.x / s, u.y / s, u.z / s);
}

inline const vec3 operator*(const vec3& u, const vec3& v)
{
  return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec2::vec2(const vec3& u)
{
  nv_scalar k = 1 / u.z;
  x = k * u.x;
  y = k * u.y;
}

struct vec4
{
  vec4() { }
  vec4(nv_scalar x, nv_scalar y, nv_scalar z, nv_scalar w) : x(x), y(y), z(z), w(w) { }
  vec4(const nv_scalar* xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
  vec4(const vec3& u) : x(u.x), y(u.y), z(u.z), w(1.0f) { }
  vec4(const vec3t& u);
  vec4(const vec4& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }
  vec4(const vec4t& u);
  
  vec4t T() const;
  
  bool operator==(const vec4 & u) const
  {
    return (u.x == x && u.y == y && u.z == z && u.w == w) ? true : false;
  }
  
  bool operator!=( const vec4& rhs ) const
  {
    return !(*this == rhs );
  }
  
  
  vec4 & operator*=(const nv_scalar & lambda)
  {
    x*= lambda;
    y*= lambda;
    z*= lambda;
    w*= lambda;
    return *this;
  }
  
  vec4 & operator-=(const vec4 & u)
  {
    x-= u.x;
    y-= u.y;
    z-= u.z;
    w-= u.w;
    return *this;
  }
  
  vec4 & operator+=(const vec4 & u)
  {
    x+= u.x;
    y+= u.y;
    z+= u.z;
    w+= u.w;
    return *this;
  }
  
  vec4 operator - () const
  {
    return vec4(-x, -y, -z, -w);
  }
  
  nv_scalar & operator[](int i)
  {
    return vec_array[i];
  }
  
  const nv_scalar operator[](int i) const
  {
    return vec_array[i];
  }
  
  union {
    struct {
      nv_scalar x,y,z,w;          // standard names for components
    };
    struct {
      nv_scalar s,t,r,q;          // standard names for components
    };
    nv_scalar vec_array[4];     // array access
  };
};

inline const vec4 operator+(const vec4& u, const vec4& v)
{
  return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

inline const vec4 operator-(const vec4& u, const vec4& v)
{
  return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

inline const vec4 operator*(const nv_scalar s, const vec4& u)
{
  return vec4(s * u.x, s * u.y, s * u.z, s * u.w);
}

inline const vec4 operator/(const vec4& u, const nv_scalar s)
{
  return vec4(u.x / s, u.y / s, u.z / s, u.w / s);
}

inline const vec4 operator*(const vec4& u, const vec4& v)
{
  return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

inline vec3::vec3(const vec4& u)
{
  nv_scalar k = 1 / u.w;
  x = k * u.x;
  y = k * u.y;
  z = k * u.z;
}

/* transposed vectors (row vectors) */

struct vec2t {
  vec2t() { }
  vec2t(nv_scalar x, nv_scalar y) : x(x), y(y) { }
  vec2t(const nv_scalar* xy) : x(xy[0]), y(xy[1]) { }
  vec2t(const vec2t& u) : x(u.x), y(u.y) { }
  vec2t(const vec2& u) : x(u.x), y(u.y) { }
  vec2 T() const { return vec2(x, y); }
  nv_scalar x,y;
};

inline vec2::vec2(const vec2t& u) : x(u.x), y(u.y) { }

inline vec2t vec2::T() const
{
  return vec2t(x, y);
}

inline vec3::vec3(const vec2t& u) : x(u.x), y(u.y), z(1.0f) { }

inline float operator*(const vec2t& u, const vec2& v)
{
  return u.x * v.x + u.y * v.y;
}

struct vec3t {
  vec3t() { }
  vec3t(nv_scalar x, nv_scalar y, nv_scalar z) : x(x), y(y), z(z) { }
  vec3t(const nv_scalar* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
  vec3t(const vec3t& u) : x(u.x), y(u.y), z(u.z) { }
  vec3t(const vec2t& u) : x(u.x), y(u.y), z(1.0f) { }
  vec3t(const vec4t&);
  vec3 T() const { return vec3(x, y, z); }
  nv_scalar x,y,z;
};

inline vec2::vec2(const vec3t& u)
{
  nv_scalar k = 1 / u.z;
  x = k * u.x;
  y = k * u.y;
}

inline vec3::vec3(const vec3t& u) : x(u.x), y(u.y), z(u.z) { }

inline vec3t vec3::T() const
{
  return vec3t(x, y, z);
}

inline vec4::vec4(const vec3t& u) : x(u.x), y(u.y), z(u.z), w(1.0f) { }

inline float operator*(const vec3t& u, const vec3& v)
{
  return u.x * v.x + u.y * v.y + u.z * v.z;
}

struct vec4t {
  vec4t() { }
  vec4t(nv_scalar x, nv_scalar y, nv_scalar z, nv_scalar w) : x(x), y(y), z(z), w(w) { }
  vec4t(const nv_scalar* xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
  vec4t(const vec4t& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }
  vec4t(const vec3t& u) : x(u.x), y(u.y), z(u.z), w(1.0f) { }
  vec4 T() const { return vec4(x, y, z, w); }
  nv_scalar x,y,z,w;
};

inline vec3::vec3(const vec4t& u)
{
  nv_scalar k = 1 / u.w;
  x = k * u.x;
  y = k * u.y;
  z = k * u.z;
}

inline vec3t::vec3t(const vec4t& u)
{
  nv_scalar k = 1 / u.w;
  x = k * u.x;
  y = k * u.y;
  z = k * u.z;
}

inline vec4::vec4(const vec4t& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }

inline vec4t vec4::T() const
{
  return vec4t(x, y, z, w);
}

inline float operator*(const vec4t& u, const vec4& v)
{
  return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
}

// quaternion
struct quat;  

/*
for all the matrices...a<x><y> indicates the element at row x, col y

  For example:
  a01 <-> row 0, col 1 
*/

struct mat3
{
  mat3() {}
  mat3(const nv_scalar * array);
  mat3(const mat3 & M);
  
  const vec3 col(const int i) const
  {
    return vec3(&mat_array[i * 3]);
  }
  
  const vec3 operator[](int i) const
  {
    return vec3(mat_array[i], mat_array[i + 3], mat_array[i + 6]);
  }
  
  const nv_scalar& operator()(const int& i, const int& j) const
  {
    return mat_array[ j * 3 + i ];
  }
  
  nv_scalar& operator()(const int& i, const int& j)
  {
    return  mat_array[ j * 3 + i ];
  }
  
  void set_row(int i, const vec3 & v)
  {
    mat_array[i] = v.x;
    mat_array[i + 3] = v.y;
    mat_array[i + 6] = v.z;
  }
  
  void set_col(int i, const vec3 & v)
  {
    mat_array[i * 3] = v.x;
    mat_array[i * 3 + 1] = v.y;
    mat_array[i * 3 + 2] = v.z;
  }
  
  void set_rot(const nv_scalar & theta, const vec3 & v);
  void set_rot(const vec3 & u, const vec3 & v);
  
  union {
    struct {
      nv_scalar a00, a10, a20;        // standard names for components
      nv_scalar a01, a11, a21;        // standard names for components
      nv_scalar a02, a12, a22;        // standard names for components
    };
    nv_scalar mat_array[9];     // array access
  };
};

const vec3 operator*(const mat3&, const vec3&);

struct mat4
{
  mat4() {}
  mat4(const nv_scalar * array);
  mat4(const mat4 & M);
  
  mat4( const nv_scalar& f0,  const nv_scalar& f1,  const nv_scalar& f2,  const nv_scalar& f3,
    const nv_scalar& f4,  const nv_scalar& f5,  const nv_scalar& f6,  const nv_scalar& f7,
    const nv_scalar& f8,  const nv_scalar& f9,  const nv_scalar& f10, const nv_scalar& f11,
    const nv_scalar& f12, const nv_scalar& f13, const nv_scalar& f14, const nv_scalar& f15 )
    : a00( f0 ), a10( f4 ), a20( f8 ), a30( f12),
    a01( f1 ), a11( f5 ), a21( f9 ), a31( f13),
    a02( f2 ), a12( f6 ), a22( f10), a32( f14),
    a03( f3 ), a13( f7 ), a23( f11), a33( f15) { }
  
  const vec4 col(const int i) const
  {
    return vec4(&mat_array[i * 4]);
  }
  
  const vec4 operator[](const int& i) const
  {
    return vec4(mat_array[i], mat_array[i + 4], mat_array[i + 8], mat_array[i + 12]);
  }
  
  const nv_scalar& operator()(const int& i, const int& j) const
  {
    return mat_array[ j * 4 + i ];
  }
  
  nv_scalar& operator()(const int& i, const int& j)
  {
    return  mat_array[ j * 4 + i ];
  }
  
  void set_col(int i, const vec4 & v)
  {
    mat_array[i] = v.x;
    mat_array[i + 1] = v.y;
    mat_array[i + 2] = v.z;
    mat_array[i + 3] = v.w;
  }
  
  void set_row(int i, const vec4 & v)
  {
    mat_array[i] = v.x;
    mat_array[i + 4] = v.y;
    mat_array[i + 8] = v.z;
    mat_array[i + 12] = v.w;
  }
  
  mat3 & get_rot(mat3 & M) const;
  quat & get_rot(quat & q) const;
  void set_rot(const quat & q);
  void set_rot(const mat3 & M);
  void set_rot(const nv_scalar & theta, const vec3 & v);
  void set_rot(const vec3 & u, const vec3 & v);
  
  void set_translation(const vec3 & t);
  vec3 & get_translation(vec3 & t) const;
  
  mat4 operator*(const mat4&) const;
  
  union {
    struct {
      nv_scalar a00, a10, a20, a30;   // standard names for components
      nv_scalar a01, a11, a21, a31;   // standard names for components
      nv_scalar a02, a12, a22, a32;   // standard names for components
      nv_scalar a03, a13, a23, a33;   // standard names for components
    };
    struct {
      nv_scalar _11, _12, _13, _14;   // standard names for components
      nv_scalar _21, _22, _23, _24;   // standard names for components
      nv_scalar _31, _32, _33, _34;   // standard names for components
      nv_scalar _41, _42, _43, _44;   // standard names for components
    };
    union {
      struct {
        nv_scalar b00, b10, b20, p; // standard names for components
        nv_scalar b01, b11, b21, q; // standard names for components
        nv_scalar b02, b12, b22, r; // standard names for components
        nv_scalar x, y, z, w;       // standard names for components
      };
    };
    nv_scalar mat_array[16];     // array access
  };
};

const vec4 operator*(const mat4&, const vec4&);
const vec4t operator*(const vec4t&, const mat4&);

// quaternion
struct quat {
public:
  quat(nv_scalar x = 0, nv_scalar y = 0, nv_scalar z = 0, nv_scalar w = 1);
  quat(const quat& quat);
  quat(const vec3& axis, nv_scalar angle);
  quat(const mat3& rot);
  quat& operator=(const quat& quat);
  quat Inverse();
  void Normalize();
  void FromMatrix(const mat3& mat);
  void ToMatrix(mat3& mat) const;
  quat& operator*=(const quat& quat);
  static const quat Identity;
  nv_scalar& operator[](int i) { return comp[i]; }
  const nv_scalar operator[](int i) const { return comp[i]; }
  union {
    struct {
      nv_scalar x, y, z, w;
    };
    nv_scalar comp[4];
  };
};
const quat operator*(const quat&, const quat&);
quat & conj(quat & p, const quat & q);
quat & add_quats(quat & p, const quat & q1, const quat & q2);
quat & axis_to_quat(quat & q, const vec3 & a, const nv_scalar phi);
mat3 & quat_2_mat(mat3 &M, const quat &q );
quat & mat_2_quat(quat &q,const mat3 &M);

// constant algebraic values
const nv_scalar array16_id[] =        { nv_one, nv_zero, nv_zero, nv_zero,
nv_zero, nv_one, nv_zero, nv_zero,
nv_zero, nv_zero, nv_one, nv_zero,
nv_zero, nv_zero, nv_zero, nv_one};

const nv_scalar array16_null[] =      { nv_zero, nv_zero, nv_zero, nv_zero,
nv_zero, nv_zero, nv_zero, nv_zero,
nv_zero, nv_zero, nv_zero, nv_zero,
nv_zero, nv_zero, nv_zero, nv_zero};

const nv_scalar array16_scale_bias[] = { nv_zero_5, nv_zero,   nv_zero,   nv_zero,
nv_zero,   nv_zero_5, nv_zero,   nv_zero,
nv_zero,   nv_zero,   nv_zero_5, nv_zero,
nv_zero_5, nv_zero_5, nv_zero_5, nv_one};

const nv_scalar array9_id[] =         { nv_one, nv_zero, nv_zero,
nv_zero, nv_one, nv_zero,
nv_zero, nv_zero, nv_one};


const vec2      vec2_null(nv_zero,nv_zero);
const vec4      vec4_one(nv_one,nv_one,nv_one,nv_one);
const vec3      vec3_one(nv_one,nv_one,nv_one);
const vec3      vec3_null(nv_zero,nv_zero,nv_zero);
const vec3      vec3_x(nv_one,nv_zero,nv_zero);
const vec3      vec3_y(nv_zero,nv_one,nv_zero);
const vec3      vec3_z(nv_zero,nv_zero,nv_one);
const vec3      vec3_neg_x(-nv_one,nv_zero,nv_zero);
const vec3      vec3_neg_y(nv_zero,-nv_one,nv_zero);
const vec3      vec3_neg_z(nv_zero,nv_zero,-nv_one);
const vec4      vec4_null(nv_zero,nv_zero,nv_zero,nv_zero);
const vec4      vec4_x(nv_one,nv_zero,nv_zero,nv_zero);
const vec4      vec4_y(nv_zero,nv_one,nv_zero,nv_zero);
const vec4      vec4_z(nv_zero,nv_zero,nv_one,nv_zero);
const vec4      vec4_w(nv_zero,nv_zero,nv_zero,nv_one);
const quat      quat_id(nv_zero,nv_zero,nv_zero,nv_one);
const mat4      mat4_id(array16_id);
const mat3      mat3_id(array9_id);
const mat4      mat4_null(array16_null);
const mat4      mat4_scale_bias(array16_scale_bias);

// normalizes a vector and return a reference of itself
vec3 & normalize(vec3 & u);
vec4 & normalize(vec4 & u);

// Computes the squared magnitude
inline nv_scalar nv_sq_norm(const vec3 & n)
{ return n.x * n.x + n.y * n.y + n.z * n.z; }

inline nv_scalar nv_sq_norm(const vec4 & n)
{ return n.x * n.x + n.y * n.y + n.z * n.z + n.w * n.w; }

// Computes the magnitude
inline nv_scalar nv_norm(const vec3 & n)
{ return sqrtf(nv_sq_norm(n)); }

// Computes the magnitude
inline nv_scalar magnitude(const vec3 & n)
{ return sqrtf(nv_sq_norm(n)); }


inline nv_scalar nv_norm(const vec4 & n)
{ return sqrtf(nv_sq_norm(n)); }

// computes the cross product ( v cross w) and stores the result in u
// i.e.     u = v cross w
vec3 & cross(vec3 & u, const vec3 & v, const vec3 & w);

// computes the dot product ( v dot w) and stores the result in u
// i.e.     u = v dot w
nv_scalar & dot(nv_scalar & u, const vec3 & v, const vec3 & w);
nv_scalar & dot(nv_scalar & u, const vec4 & v, const vec4 & w);

// compute the reflected vector R of L w.r.t N - vectors need to be 
// normalized
//
//                R     N     L
//                  _       _
//                 |\   ^   /|
//                   \  |  /
//                    \ | /
//                     \|/
//                      +
vec3 & reflect(vec3 & r, const vec3 & n, const vec3 & l);

// Computes u = v * lambda + u
vec3 & madd(vec3 & u, const vec3 & v, const nv_scalar & lambda);
// Computes u = v * lambda
vec3 & mult(vec3 & u, const vec3 & v, const nv_scalar & lambda);
// Computes u = v * w
vec3 & mult(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v + w
vec3 & add(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v - w
vec3 & sub(vec3 & u, const vec3 & v, const vec3 & w);

// Computes u = u * s
vec3 & scale(vec3 & u, const nv_scalar s);
vec4 & scale(vec4 & u, const nv_scalar s);

// Computes u = M * v
vec3 & mult(vec3 & u, const mat3 & M, const vec3 & v);
vec4 & mult(vec4 & u, const mat4 & M, const vec4 & v);

// Computes u = M(3x3) * v and devides by w
vec3 & mult_pos(vec3 & u, const mat4 & M, const vec3 & v);
// Computes u = M(3x3) * v
vec3 & mult_dir(vec3 & u, const mat4 & M, const vec3 & v);


// Computes A += B
mat4 & add(mat4 & A, const mat4 & B);
mat3 & add(mat3 & A, const mat3 & B);

// Computes C = A * B
mat4 & mult(mat4 & C, const mat4 & A, const mat4 & B);
mat3 & mult(mat3 & C, const mat3 & A, const mat3 & B);

// Computes B = Transpose(A)
//       T
//  B = A
mat3 & transpose(mat3 & B, const mat3 & A);
mat4 & transpose(mat4 & B, const mat4 & A);
mat3 & transpose(mat3 & B);
mat4 & transpose(mat4 & B);

// Computes B = inverse(A)
//       -1
//  B = A
mat4 & invert(mat4 & B, const mat4 & A);
mat3 & invert(mat3 & B, const mat3 & A);

// Computes B = inverse(A)
//                                       T  T
//                   (R t)             (R -R t)
// assuming that A = (0 1) so that B = (0    1)
//  B = A
mat4 & invert_rot_trans(mat4 & B, const mat4 & A);

mat4 & look_at(mat4 & M, const vec3 & eye, const vec3 & center, const vec3 & up);
mat4 & frustum(mat4 & M, const nv_scalar l, const nv_scalar r, const nv_scalar b, 
                      const nv_scalar t, const nv_scalar n, const nv_scalar f);

mat4 & perspective(mat4 & M, const nv_scalar fovy, const nv_scalar aspect, const nv_scalar n, const nv_scalar f);

// quaternion
quat & normalize(quat & p);
quat & conj(quat & p);
quat & conj(quat & p, const quat & q);
quat & add_quats(quat & p, const quat & q1, const quat & q2);
quat & axis_to_quat(quat & q, const vec3 & a, const nv_scalar phi);
mat3 & quat_2_mat(mat3 &M, const quat &q );
quat & mat_2_quat(quat &q,const mat3 &M);
quat & mat_2_quat(quat &q,const mat4 &M);

// surface properties
mat3 & tangent_basis(mat3 & basis,const vec3 & v0,const vec3 & v1,const vec3 & v2,const vec2 & t0,const vec2 & t1,const vec2 & t2, const vec3 & n);

// linear interpolation
inline nv_scalar lerp(nv_scalar t, nv_scalar a, nv_scalar b)
{ return a * (nv_one - t) + t * b; }

inline vec3 & lerp(vec3 & w, const nv_scalar & t, const vec3 & u, const vec3 & v)
{ w.x = lerp(t, u.x, v.x); w.y = lerp(t, u.y, v.y); w.z = lerp(t, u.z, v.z); return w; }

// utilities
inline nv_scalar nv_min(const nv_scalar & lambda, const nv_scalar & n)
{ return (lambda < n ) ? lambda : n; }

inline nv_scalar nv_max(const nv_scalar & lambda, const nv_scalar & n)
{ return (lambda > n ) ? lambda : n; }

inline nv_scalar nv_clamp(nv_scalar u, const nv_scalar min, const nv_scalar max)
{ u = (u < min) ? min : u; u = (u > max) ? max : u; return u; }

nv_scalar nv_random();

quat & trackball(quat & q, vec2 & pt1, vec2 & pt2, nv_scalar trackballsize);

vec3 & cube_map_normal(int i, int x, int y, int cubesize, vec3 & v);

// geometry
// computes the area of a triangle
nv_scalar nv_area(const vec3 & v1, const vec3 & v2, const vec3 &v3);
// computes the perimeter of a triangle
nv_scalar nv_perimeter(const vec3 & v1, const vec3 & v2, const vec3 &v3);
// find the inscribed circle
nv_scalar nv_find_in_circle( vec3 & center, const vec3 & v1, const vec3 & v2, const vec3 &v3);
// find the circumscribed circle
nv_scalar nv_find_circ_circle( vec3 & center, const vec3 & v1, const vec3 & v2, const vec3 &v3);

// fast cosine functions
nv_scalar fast_cos(const nv_scalar x);
nv_scalar ffast_cos(const nv_scalar x);

// determinant
nv_scalar det(const mat3 & A);

void nv_is_valid(const vec3& v);
void nv_is_valid(nv_scalar lambda);

inline
mat3::mat3(const nv_scalar* array)
{
  memcpy(mat_array, array, sizeof(nv_scalar) * 9);
}

inline
mat3::mat3(const mat3 & M)
{
  memcpy(mat_array, M.mat_array, sizeof(nv_scalar) * 9);
}

inline
mat4::mat4(const nv_scalar* array)
{
  memcpy(mat_array, array, sizeof(nv_scalar) * 16);
}

inline
mat4::mat4(const mat4& M)
{
  memcpy(mat_array, M.mat_array, sizeof(nv_scalar) * 16);
}

inline
vec3 & cross(vec3 & u, const vec3 & v, const vec3 & w)
{
  u.x = v.y*w.z - v.z*w.y;
  u.y = v.z*w.x - v.x*w.z;
  u.z = v.x*w.y - v.y*w.x;
  return u;
}

inline
nv_scalar & dot(nv_scalar& u, const vec3& v, const vec3& w)
{
  u = v.x*w.x + v.y*w.y + v.z*w.z;
  return u;
}

inline
nv_scalar & dot(nv_scalar& u, const vec4& v, const vec4& w)
{
  u = v.x*w.x + v.y*w.y + v.z*w.z + v.w*w.w;
  return u;
}

inline
vec3 & reflect(vec3& r, const vec3& n, const vec3& l)
{
  nv_scalar n_dot_l;
  n_dot_l = nv_two * dot(n_dot_l,n,l);
  mult(r,l,-nv_one);
  madd(r,n,n_dot_l);
  return r;
}

inline
vec3 & madd(vec3 & u, const vec3& v, const nv_scalar& lambda)
{
  u.x += v.x*lambda;
  u.y += v.y*lambda;
  u.z += v.z*lambda;
  return u;
}

inline
vec3 & mult(vec3 & u, const vec3& v, const nv_scalar& lambda)
{
  u.x = v.x*lambda;
  u.y = v.y*lambda;
  u.z = v.z*lambda;
  return u;
}

inline
vec3 & mult(vec3 & u, const vec3& v, const vec3& w)
{
  u.x = v.x*w.x;
  u.y = v.y*w.y;
  u.z = v.z*w.z;
  return u;
}

inline
vec3 & sub(vec3 & u, const vec3& v, const vec3& w)
{
  u.x = v.x - w.x;
  u.y = v.y - w.y;
  u.z = v.z - w.z;
  return u;
}

inline
vec3 & add(vec3 & u, const vec3& v, const vec3& w)
{
  u.x = v.x + w.x;
  u.y = v.y + w.y;
  u.z = v.z + w.z;
  return u;
}

inline
vec3 & scale(vec3& u, const nv_scalar s)
{
  u.x *= s;
  u.y *= s;
  u.z *= s;
  return u;
}

inline
vec4 & scale(vec4& u, const nv_scalar s)
{
  u.x *= s;
  u.y *= s;
  u.z *= s;
  u.w *= s;
  return u;
}

inline
void mat4::set_rot(const mat3& M)
{
  // copy the 3x3 rotation block
  a00 = M.a00; a10 = M.a10; a20 = M.a20;
  a01 = M.a01; a11 = M.a11; a21 = M.a21;
  a02 = M.a02; a12 = M.a12; a22 = M.a22;
}

inline
void mat4::set_translation(const vec3& t)
{
  a03 = t.x;
  a13 = t.y;
  a23 = t.z;
}

inline
vec3 & mat4::get_translation(vec3& t) const
{
  t.x = a03;
  t.y = a13;
  t.z = a23;
  return t;
}

inline
quat & conj(quat & p)
{
  p.x = -p.x;
  p.y = -p.y;
  p.z = -p.z;
  return p;
}

inline
quat & conj(quat& p, const quat& q)
{
  p.x = -q.x;
  p.y = -q.y;
  p.z = -q.z;
  p.w = q.w;
  return p;
}

inline
quat& quat::operator*=(const quat& quat)
{
  *this = *this * quat;
  return *this;
}

inline
quat quat::Inverse()
{
  return quat(- x, - y, - z, w);
}

inline
quat& quat::operator=(const quat& quat)
{
  x = quat.x;
  y = quat.y;
  z = quat.z;
  w = quat.w;
  return *this;
}

inline
quat::quat(const mat3& rot)
{
  FromMatrix(rot);
}

inline
quat::quat(nv_scalar x, nv_scalar y, nv_scalar z, nv_scalar w) : x(x), y(y), z(z), w(w)
{}

inline
quat::quat(const quat& quat)
{
  x = quat.x;
  y = quat.y;
  z = quat.z;
  w = quat.w;
}

//#ifndef _WIN32
#define _isnan isnan
#define _finite finite
//#endif
/*
inline
void nv_is_valid(const vec3& v)
{
  REQUIRE(!_isnan(v.x) && !_isnan(v.y) && !_isnan(v.z) &&
    _finite(v.x) && _finite(v.y) && _finite(v.z));
}

inline
void nv_is_valid(nv_scalar lambda)
{
  REQUIRE(!_isnan(lambda) && _finite(lambda));
}

inline
bool nv_is_valid_bool(const vec3& v)
{
  return !_isnan(v.x) && !_isnan(v.y) && !_isnan(v.z) &&
    _finite(v.x) && _finite(v.y) && _finite(v.z);
}

inline
bool nv_is_valid_bool(nv_scalar lambda)
{
  return !_isnan(lambda) && _finite(lambda);
}
*/
} // namespace nv_sdk

#ifndef _WIN32
#undef _isnan
#undef _finite
#endif

#endif //_nv_algebra_h_
