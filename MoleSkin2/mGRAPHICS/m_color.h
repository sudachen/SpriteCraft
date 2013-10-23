
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


#if !defined __m_graphics_color_h__
#define __m_graphics_color_h__

#include "../mcore/m_autoconf.h"

# define MSKIN_XRGB_MASK_555      0x7fffU
# define MSKIN_R_MASK_555         0x00007c00U
# define MSKIN_G_MASK_555         0x000003e0U
# define MSKIN_B_MASK_555         0x0000001fU
# define MSKIN_A_MASK_555         0x00008000U
# define MSKIN_RED_555(c)         (((c)&MSKIN_R_MASK_555)>>(10-3))
# define MSKIN_GREEN_555(c)       (((c)&MSKIN_G_MASK_555)>>(5-3))
# define MSKIN_BLUE_555(c)        (((c)&MSKIN_B_MASK_555)<<3)
# define MSKIN_ALPHA_555(c)       ((((c)&MSKIN_A_MASK_555)>>15)*255)
# define MSKIN_CC_555(x,n)        (((((unsigned short)x)>>3)&0x01f)<<(n*5))
# define MSKIN_RGB_555(r,g,b)     (MSKIN_CC_555(r,2)|MSKIN_CC_555(g,1)|MSKIN_CC_555(b,0))
# define MSKIN_RGB_555_A(r,g,b,a) (MSKIN_RGB_555(r,g,b)|((a&0x080)<<8))
# define MSKIN_SETRED_555(c,r)    (((c)&+MSKIN_R_MASK_555)|MSKIN_CC_555(r,2))
# define MSKIN_SETGREEN_555(c,g)  (((c)&+MSKIN_G_MASK_555)|MSKIN_CC_555(g,1))
# define MSKIN_SETBLUE_555(c,b)   (((c)&+MSKIN_B_MASK_555)|MSKIN_CC_555(b,0))

# define MSKIN_XRGB_MASK_565      0xffffU
# define MSKIN_R_MASK_565         0x0000f800U
# define MSKIN_G_MASK_565         0x000007e0U
# define MSKIN_B_MASK_565         0x0000001fU
# define MSKIN_RED_565(c)         (((c)&MSKIN_R_MASK_565)>>(5+6-3))
# define MSKIN_GREEN_565(c)       (((c)&MSKIN_G_MASK_565)>>(5-2))
# define MSKIN_BLUE_565(c)        (((c)&MSKIN_B_MASK_565)<<3)
# define MSKIN_CC_565(x,m,n)      (((((unsigned short)x)>>3)&m)<<(n))
# define MSKIN_CC2_565(x,m,n)     (((((unsigned short)x)>>2)&m)<<(n))
# define MSKIN_RGB_565(r,g,b)     (MSKIN_CC_565(r,0x1f,5+6)|MSKIN_CC2_565(g,0x3f,5)|MSKIN_CC_565(b,0x1f,0))
# define MSKIN_SETRED_565(c,r)    (((c)&+MSKIN_R_MASK_565)|MSKIN_CC_565(r,0x1f,5+6))
# define MSKIN_SETGREEN_565(c,g)  (((c)&+MSKIN_G_MASK_565)|MSKIN_CC2_565(g,0x3f,5))
# define MSKIN_SETBLUE_565(c,b)   (((c)&+MSKIN_B_MASK_565)|MSKIN_CC_565(b,0x1f,0))

# define MSKIN_XRGB_MASK_444      0x0fffU
# define MSKIN_R_MASK_444         0x0000f00U
# define MSKIN_G_MASK_444         0x00000f0U
# define MSKIN_B_MASK_444         0x000000fU
# define MSKIN_RED_444(c)         (((c)&MSKIN_R_MASK_444)>>4)
# define MSKIN_GREEN_444(c)       (((c)&MSKIN_G_MASK_444))
# define MSKIN_BLUE_444(c)        (((c)&MSKIN_B_MASK_444)<<4)
# define MSKIN_RGB_444(r,g,b)     (((r)&0x0f0)<<4) | ((g)&0x0f0) | (((b)&0x0f0)>>4)
# define MSKIN_RGB_4444(r,g,b,a)  (((r)&0x0f0)<<4) | ((g)&0x0f0) | (((b)&0x0f0)>>4 | (((a)&0x0f0)<<8) )

NAMESPACE_MSKIN_BEGIN

#pragma pack(1)
struct mbgr_t {
  byte_t B;
  byte_t G;
  byte_t R;
//  u32_t C() { u32_t u = 0; memcpy(&u,this,3); return u; }
  u32_t C() const { return *(const u32_t*)this; }
};
struct mrgb_t {
  byte_t R;
  byte_t G;
  byte_t B;
  //u32_t C() { u32_t u = 0; memcpy(&u,this,3); return u; }
  u32_t C() const { return *(const u32_t*)this; }
};
struct mrgba_t {
  byte_t R;
  byte_t G;
  byte_t B;
  byte_t A;
  //u32_t C() { u32_t u; memcpy(&u,this,sizeof(this)); return u; }
  u32_t C() const { return *(const u32_t*)this; }
};
struct mbgra_t {
  byte_t B;
  byte_t G;
  byte_t R;
  byte_t A;
  //u32_t C() { u32_t u; memcpy(&u,this,sizeof(this)); return u; }
  u32_t C() const { return *(const u32_t*)this; }
};

inline bool operator == (const mrgba_t& a,const mrgba_t& b) {
  return !memcmp(&a,&b,sizeof(mrgba_t));
}
inline bool operator != (const mrgba_t& a,const mrgba_t& b) {
  return !!memcmp(&a,&b,sizeof(mrgba_t));
}
inline bool operator == (const mbgra_t& a,const mbgra_t& b) {
  return !memcmp(&a,&b,sizeof(mbgra_t));
}
inline bool operator != (const mbgra_t& a,const mbgra_t& b) {
  return !!memcmp(&a,&b,sizeof(mbgra_t));
}

struct mRGBA_t : public mrgba_t {
  mRGBA_t(unsigned r,unsigned g,unsigned b, unsigned a = 255) 
  { A = a; R = r; G = g; B = b; }
  mRGBA_t(mbgra_t c)
  { A = c.A; R = c.R; G = c.G; B = c.B; }
  mRGBA_t(mbgr_t c)
  { A = 255; R = c.R; G = c.G; B = c.B; }
  mRGBA_t(mrgb_t c)
  { A = 255; R = c.R; G = c.G; B = c.B; }
};
struct mBGRA_t : public mbgra_t {
  mBGRA_t(unsigned r,unsigned g,unsigned b, unsigned a = 255)
  { A = a; R = r; G = g; B = b; }
  mBGRA_t(mrgba_t c)
  { A = c.A; R = c.R; G = c.G; B = c.B; }
  mBGRA_t(mbgr_t c)
  { A = 255; R = c.R; G = c.G; B = c.B; }
  mBGRA_t(mrgb_t c)
  { A = 255; R = c.R; G = c.G; B = c.B; }
};
struct mBGR_t : public mbgr_t {
  mBGR_t(unsigned r,unsigned g,unsigned b)
  { R = r; G = g; B = b; }
  mBGR_t(mrgba_t c)
  { R = c.R; G = c.G; B = c.B; }
  mBGR_t(mbgra_t c)
  { R = c.R; G = c.G; B = c.B; }
  mBGR_t(mbgr_t c)
  { R = c.R; G = c.G; B = c.B; }
};
struct mRGB_t : public mrgb_t {
  mRGB_t(unsigned r,unsigned g,unsigned b)
  { R = r; G = g; B = b; }
  mRGB_t(mrgba_t c)
  { R = c.R; G = c.G; B = c.B; }
  mRGB_t(mbgra_t c)
  { R = c.R; G = c.G; B = c.B; }
  mRGB_t(mbgr_t c)
  { R = c.R; G = c.G; B = c.B; }
};
#pragma pack()

#if !defined MSKIN_DISABLE_STATIC_CHECK
typedef static_check<bool(sizeof(mrgba_t)==4)>::success margb_sizeof_check_0;
typedef static_check<bool(sizeof(mbgra_t)==4)>::success mbgra_sizeof_check_0;
#endif

template<class XXX> 
inline mrgba_t xxx_to_mrgba(XXX x) { return mRGBA_t(x); }
inline mrgba_t xxx_to_mrgba(mrgba_t x) { return x; }

template<class XXX> 
inline mbgra_t xxx_to_mbgra(XXX x) { return mBGRA_t(x); }
inline mbgra_t xxx_to_mbgra(mbgra_t x) { return x; }

template<class XXX> 
inline mbgr_t xxx_to_mbgr(XXX x) { return mBGR_t(x); }
inline mbgr_t xxx_to_mbgr(mbgr_t x) { return x; }

template<class XXX> 
inline mrgb_t xxx_to_mrgb(XXX x) { return mRGB_t(x); }
inline mrgb_t xxx_to_mrgb(mrgb_t x) { return x; }

inline mbgra_t c565_to_mbgra( u16_t color ) {
  return mBGRA_t( MSKIN_RED_565(color), MSKIN_GREEN_565(color), MSKIN_BLUE_565(color) );
}

inline mrgba_t c565_to_mrgba( u16_t color ) {
  return mRGBA_t( MSKIN_RED_565(color), MSKIN_GREEN_565(color), MSKIN_BLUE_565(color) );
}

inline mbgr_t c565_to_mbgr( u16_t color ) {
  return mBGR_t( MSKIN_RED_565(color), MSKIN_GREEN_565(color), MSKIN_BLUE_565(color) );
}

inline mrgb_t c565_to_mrgb( u16_t color ) {
  return mRGB_t( MSKIN_RED_565(color), MSKIN_GREEN_565(color), MSKIN_BLUE_565(color) );
}

template<class T>
inline u16_t xxx_to_c565( T rgb ) {
  return MSKIN_RGB_565(rgb.R,rgb.G,rgb.B);
}

inline mbgra_t c555_to_mbgra( u16_t color ) {
  return mBGRA_t( MSKIN_RED_555(color), MSKIN_GREEN_555(color), MSKIN_BLUE_555(color), MSKIN_ALPHA_555(color) );
}

inline mrgba_t c555_to_mrgba( u16_t color ) {
  return mRGBA_t( MSKIN_RED_555(color), MSKIN_GREEN_555(color), MSKIN_BLUE_555(color), MSKIN_ALPHA_555(color) );
}

inline mbgr_t c555_to_mbgr( u16_t color ) {
  return mBGR_t( MSKIN_RED_555(color), MSKIN_GREEN_555(color), MSKIN_BLUE_555(color) );
}

inline mrgb_t c555_to_mrgb( u16_t color ) {
  return mRGB_t( MSKIN_RED_555(color), MSKIN_GREEN_555(color), MSKIN_BLUE_555(color) );
}

template<class T>
inline u16_t xxx_to_c555( T rgb ) {
  return MSKIN_RGB_555(rgb.R,rgb.G,rgb.B);
}
inline u16_t xxx_to_c555(mrgba_t rgb ) {
  return MSKIN_RGB_555(rgb.R,rgb.G,rgb.B);
}
inline u16_t xxx_to_c555(mbgra_t rgb ) {
  return MSKIN_RGB_555(rgb.R,rgb.G,rgb.B);
}

inline u16_t xxx_to_c5551(mrgba_t rgb ) {
  return MSKIN_RGB_555_A(rgb.R,rgb.G,rgb.B,rgb.A);
}
inline u16_t xxx_to_c5551(mbgra_t rgb ) {
  return MSKIN_RGB_555_A(rgb.R,rgb.G,rgb.B,rgb.A);
}

inline u16_t xxx_to_c4444(mrgba_t rgb ) {
  return MSKIN_RGB_4444(rgb.R,rgb.G,rgb.B,rgb.A);
}
inline u16_t xxx_to_c4444(mbgra_t rgb ) {
  return MSKIN_RGB_4444(rgb.R,rgb.G,rgb.B,rgb.A);
}

inline u16_t c555_to_c565( u16_t color ) {
  return MSKIN_RGB_565( MSKIN_RED_555(color), MSKIN_GREEN_555(color), MSKIN_BLUE_555(color) );
}

inline u16_t c565_to_c555( u16_t color ) {
  return MSKIN_RGB_555( MSKIN_RED_565(color), MSKIN_GREEN_565(color), MSKIN_BLUE_565(color) );
}

NAMESPACE_MSKIN_END

#endif 
