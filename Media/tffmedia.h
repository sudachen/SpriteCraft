
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

#if !defined ___e2d0d348_26ea_45e9_8813_44e79eb2ae14___
#define ___e2d0d348_26ea_45e9_8813_44e79eb2ae14___

#include "../Classes/classes.h"

#ifdef _MEDIA_DLL_EXPORT
#define MEDIA_EXPORTABLE __declspec(dllexport)
#define MEDIA_EXPORTABLE_DATA __declspec(dllexport)
#elif  _MEDIA_DLL_IMPORT
#define MEDIA_EXPORTABLE
#define MEDIA_EXPORTABLE_DATA __declspec(dllimport)
#else
#define MEDIA_EXPORTABLE
#define MEDIA_EXPORTABLE_DATA
#endif


extern char const MEDIA_ERROR[];

#pragma pack(push,1)

struct X_color_rgba8
  {
    enum { shift = 0, gshift = 0, ashift = 0 };
    byte_t b : 8;
    byte_t g : 8;
    byte_t r : 8;
    byte_t a : 8;
    void A(byte_t val) { a = val; }
    byte_t A() const { return a; }
    u32_t C() { return (u32_t const&)*this; }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_rgba8) == 4 )

struct X_color_bgra8
  {
    enum { shift = 0, gshift = 0, ashift = 0 };
    byte_t r : 8;
    byte_t g : 8;
    byte_t b : 8;
    byte_t a : 8;
    void A(byte_t val) { a = val; }
    byte_t A() const { return a; }
    u32_t C() { return (u32_t const&)*this; }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_bgra8) == 4 )

struct X_color_rgba4
  {
    enum { shift = 4, gshift = 4, ashift = 4 };
    byte_t b : 4;
    byte_t g : 4;
    byte_t r : 4;
    byte_t a : 4;
    void A(byte_t val) { a = val; }
    byte_t A() const { return a; }
    u32_t C() { return (u16_t const&)*this; }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_rgba4) == 2 )

struct X_color_rgb5a1
  {
    enum { shift = 3, gshift = 3, ashift = 7 };
    u16_t b : 5;
    u16_t g : 5;
    u16_t r : 5;
    u16_t a : 1;
    void A(byte_t val) { a = val; }
    byte_t A() const { return a; }
    u32_t C() { return (u16_t const&)*this; }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_rgb5a1) == 2 )

struct X_color_rgb5
  {
    enum { shift = 3, gshift = 3, ashift = 0 };
    u16_t b : 5;
    u16_t g : 5;
    u16_t r : 5;
    void A(byte_t val) {}
    byte_t A() const { return 255; }
    u32_t C() { return (u16_t const&)*this; }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_rgb5) == 2 )

struct X_color_r5g6b5
  {
    enum { shift = 3, gshift = 2, ashift = 0 };
    u16_t b : 5;
    u16_t g : 6;
    u16_t r : 5;
    void A(byte_t val) {}
    byte_t A() const { return 255; }
    u32_t C() { return (u16_t const&)*this; }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_r5g6b5) == 2 )

struct X_color_b5g6r5
  {
    enum { shift = 3, gshift = 2, ashift = 0 };
    u16_t r : 5;
    u16_t g : 6;
    u16_t b : 5;
    void A(byte_t val) {}
    byte_t A() const { return 255; }
    u32_t C() { return (u16_t const&)*this; }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_b5g6r5) == 2 )

struct X_color_rgb8
  {
    enum { shift = 0, gshift = 0, ashift = 0 };
    byte_t b : 8;
    byte_t g : 8;
    byte_t r : 8;
    void A(byte_t val) {}
    byte_t A() const { return 255; }
    u32_t C() { return 0UL|(u32_t(r)<<24)|(u32_t(g)<<16)|u32_t(b); }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_rgb8) == 3 )

struct X_color_bgr8
  {
    enum { shift = 0, gshift = 0, ashift = 0 };
    byte_t r : 8;
    byte_t g : 8;
    byte_t b : 8;
    void A(byte_t val) {}
    byte_t A() const { return 255; }
    u32_t C() { return 0UL|(u32_t(b)<<24)|(u32_t(g)<<16)|u32_t(r); }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_bgr8) == 3 )

struct X_color_rgba1
  {
    enum { shift = 0, gshift = 0, ashift = 0 };
    union {
      byte_t b;
      byte_t g;
      byte_t r;
      byte_t a;
    };
    void A(byte_t val) { a = val; }
    byte_t A() const { return a; }
    u32_t C() { return u32_t(a); }
  };
TEGGO_STATIC_CHECK( sizeof(X_color_rgba4) == 2 )

#pragma pack(pop)

template < class tTlayout >
  struct crgba_t : tTlayout
    {
      using tTlayout::r;
      using tTlayout::g;
      using tTlayout::b;
      using tTlayout::A;
      using tTlayout::C;

      crgba_t() {}

      crgba_t(int rgba)
        {
          this->r = byte_t(rgba >> 16) >> tTlayout::shift;
          this->g = byte_t(rgba >> 8) >> tTlayout::gshift;
          this->b = byte_t(rgba) >> tTlayout::shift;
          this->A(byte_t(rgba >> 24) >> tTlayout::ashift);
        }

      crgba_t(int r, int g, int b, int a)
        {
          this->r = byte_t(r) >> tTlayout::shift;
          this->g = byte_t(g) >> tTlayout::gshift;
          this->b = byte_t(b) >> tTlayout::shift;
          this->A(byte_t(a) >> tTlayout::ashift);
        };

      crgba_t(float r, float g, float b, float a)
        {
          this->r = byte_t(r*255) >> tTlayout::shift;
          this->g = byte_t(g*255) >> tTlayout::gshift;
          this->b = byte_t(b*255) >> tTlayout::shift;
          this->A(int(a*255) >> tTlayout::ashift);
        };

      template < class tTlayout0 > CXX_FORCEINLINE
        crgba_t(crgba_t<tTlayout0> const &c)
          {
            this->r = (int(c.r) << tTlayout0::shift) >> tTlayout::shift;
            this->g = (int(c.g) << tTlayout0::gshift) >> tTlayout::gshift;
            this->b = (int(c.b) << tTlayout0::shift) >> tTlayout::shift;
            this->A(c.A() << tTlayout0::ashift >> tTlayout::ashift);
          };

      crgba_t(crgba_t<tTlayout> const &c)
        {
          memcpy(this,&c,sizeof(c));
        };

      static crgba_t Make(byte_t r, byte_t g, byte_t b, byte_t a)
        {
          return crgba_t(r,g,b,a);
        }

      template < class tTlayout0 >
        static crgba_t Make( crgba_t<tTlayout0> c )
          {
            return crgba_t(c.r,c.g,c.b,c.A());
          }

      void Brswap()
        {
          unsigned c = r; r = b; b = c;
        }
    };

template <>
  struct crgba_t<X_color_rgba1> : X_color_rgba1
    {
      using X_color_rgba1::r;
      using X_color_rgba1::g;
      using X_color_rgba1::b;
      using X_color_rgba1::A;
      using X_color_rgba1::C;

      crgba_t() {}

      crgba_t(int r, int g, int b, int a)
        {
          this->A(byte_t(a));
        };

      crgba_t(int a)
        {
          this->A(byte_t(a));
        };

      crgba_t(float r, float g, float b, float a)
        {
          this->A(int(a*255));
        };

      template < class tTlayout0 > CXX_FORCEINLINE
        crgba_t(crgba_t<tTlayout0> const &c)
          {
            this->A(c.A() << tTlayout0::ashift);
          };

      crgba_t(crgba_t<X_color_rgba1> const &c)
        {
          memcpy(this,&c,sizeof(c));
        };

      static crgba_t Make(byte_t r, byte_t g, byte_t b, byte_t a) { return crgba_t(a); }

      template < class tTlayout0 >
        static crgba_t Make( crgba_t<tTlayout0> c ) { A(c.A() << tTlayout0::ashift); }

      void Brswap() {}
    };

typedef crgba_t<X_color_rgba8>  rgba8_t;
typedef crgba_t<X_color_rgba4>  rgba4_t;
typedef crgba_t<X_color_rgba1>  rgba1_t;
typedef crgba_t<X_color_rgb5a1> rgb5a1_t;
typedef crgba_t<X_color_rgb5>   rgb5_t;
typedef crgba_t<X_color_rgb8>   rgb8_t;
typedef crgba_t<X_color_bgr8>   bgr8_t;
typedef crgba_t<X_color_bgra8>  bgra8_t;
typedef crgba_t<X_color_r5g6b5> r5g6b5_t;
typedef crgba_t<X_color_b5g6r5> b5g6r5_t;

enum TFFMt_E { TFFM_OK, TFFM_FAIL, TFFM_BADARGUMENT, TFFM_NOFILE };

struct TFFt_PICTURE
  {
    TFFt_PICTURE();
    virtual ~TFFt_PICTURE();

    struct INFO
      {
        int width;
        int height;
        int bpp;
        int palsize;
        int frames;
        bool updown;
        bool alphabits;
      };

    virtual TFFMt_E GetInfo(INFO *) = 0;
    virtual TFFMt_E NextRow8( rgba8_t *row, int shift, int cnt ) = 0;
  };

enum TFFt_PIXEL_FORMAT
  {
    TFF_PF_NOALPHA         = 0x000100,
    TFF_PF_ALPHAONLY       = 0x000200,
    TFF_PF_HALF            = 0x000400,
    TFF_PF_BGR             = 0x000800,
    TFF_PF_GREEN           = 0x001000,
    TFF_PF_RGB8            = 24,
    TFF_PF_RGBA8           = 32,
    TFF_PF_RGB5            = 16 | TFF_PF_NOALPHA,
    TFF_PF_PAL8            = 8,
    TFF_PF_ALPHA           = 8 | TFF_PF_ALPHAONLY,
    TFF_PF_RGB5A1          = 16,
    TFF_PF_RGBA4           = 16 | TFF_PF_HALF,
    TFF_PF_R5G6B5          = 16 | TFF_PF_GREEN,
    TFF_PF_BPPMASK         = 0x0000ff
  };

typedef void (CXX_STDCALL *TFFt_CopyRowXtoX)(void const *, void *, int);
extern "C" MEDIA_EXPORTABLE TFFt_CopyRowXtoX CXX_STDCALL TffmCopyRowXtoX( int from_bpp, int to_bpp );
extern "C" MEDIA_EXPORTABLE void CXX_STDCALL TffmDiffusionColorReduction12( rgba8_t *row, int *err, int width, bool forward );
extern "C" MEDIA_EXPORTABLE TFFMt_E CXX_STDCALL TffmWriteBmpBits8(pwide_t fname, rgba8_t *img, int width, int height, bool invert);
extern "C" MEDIA_EXPORTABLE TFFMt_E CXX_STDCALL TffmWriteTgaBits8(pwide_t fname, rgba8_t *img, int width, int height);
MEDIA_EXPORTABLE extern mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmPngCreatePicFrom( rcc_ptr<DataStream> const &ds );
MEDIA_EXPORTABLE extern mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmJpgCreatePicFrom( rcc_ptr<DataStream> const &ds );
MEDIA_EXPORTABLE extern mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmBmpCreatePicFrom( rcc_ptr<DataStream> const &ds );
MEDIA_EXPORTABLE extern mem_ptr<TFFt_PICTURE> CXX_STDCALL TffmTgaCreatePicFrom( rcc_ptr<DataStream> const &ds );
MEDIA_EXPORTABLE extern mem_ptr<TFFt_PICTURE> CXX_STDCALL OpenPicture(StringParam name);

#endif // ___e2d0d348_26ea_45e9_8813_44e79eb2ae14___
