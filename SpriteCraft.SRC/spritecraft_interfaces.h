
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

/**

  интерфейсы для внутреннего использования

*/

#if !defined ___68D29F09_1809_49ab_9FD2_07F90AF0DBAC___
#define ___68D29F09_1809_49ab_9FD2_07F90AF0DBAC___

struct IEngine;
struct IWeakSpriteEx;
struct IWeakSprite;
struct ICommand;
struct IOnUpdateListener;
struct ICollidableObject;
struct ISprite;

struct IOnUpdateListener : IUnknown 
  {
    typedef GuidT<0x6D555C59,0xBF00,0x4304,0x91,0xCF,0x49,0xCC,0x9E,0x01,0x57,0x83> Guid;
  
    virtual bool OnUpdateListener(u32_t tickdelta) = 0;
  };

struct ICollideUnlink 
  { 
    virtual void Unlink() = 0; 
  };

struct ICollidableObject : IUnknown
  {
    typedef GuidT<0xFEBA07B3,0x66D9,0x4225,0xB6,0x30,0x71,0x5E,0x1A,0xF7,0x79,0x4E> Guid;

    virtual void ICollidableObject_Link(ICollideUnlink*) = 0;
    virtual void ICollidableObject_OnCollide(Sprite* spr) = 0;
    virtual Sprite* ICollidableObject_GetSprite() = 0;
    virtual ISprite* ICollidableObject_GetISprite() = 0;
  };

struct IWeakSpriteEx : IUnknown 
  {
    typedef GuidT<0x8100E838,0x80C6,0x4bd5,0xB7,0x77,0x46,0x39,0x7E,0x2C,0xFC,0x9A> Guid;

    virtual bool ContainsPoint(float* x,float* y,long* layer,long* sublayer,u32_t* ident) = 0;
    virtual bool Alive() = 0;
    virtual bool IsVisible() = 0;
    virtual ISprite* GetSprite() = 0;
    virtual IUnknown* GetSubImpl() = 0;
  };

struct ISpriteHelperEx : IUnknown 
  {
    typedef GuidT<0x4079E7B4,0x108A,0x4383,0x9E,0x6C,0xF8,0x3C,0xA9,0x29,0x8C,0xB5> Guid;
  
    virtual u32_t  Ex_GetParentDeps() const = 0;
    virtual void   Ex_SetParentDeps(u32_t) = 0;
  };

struct ISpriteHelperEx2 : IUnknown 
  {
    typedef GuidT<0x57732262,0x2091,0x4e52,0xB1,0xC3,0x96,0x1E,0x6C,0x85,0x6F,0xFB> Guid;

    virtual Sprite* GetSprite_()  = 0;
  };

struct IColorHelper : IUnknown 
  {
    typedef GuidT<0x1FCC49A8,0x8419,0x4c0a,0x82,0x64,0x5F,0x4C,0xCE,0xA3,0x39,0x32> Guid;
  
    virtual void    IColorHelper_SetColor(mrgb_t)  = 0;
    virtual mrgb_t  IColorHelper_GetColor() const  = 0;
  };

#endif // ___68D29F09_1809_49ab_9FD2_07F90AF0DBAC___
