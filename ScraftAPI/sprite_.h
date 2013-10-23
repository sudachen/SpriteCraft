
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

struct CXX_NO_VTABLE IScraftHandler : public IUnknown
  {
    virtual ScraftSprite* GetScraftSprite() = 0;
  };

enum
  {
    SCRAFT_ONUPDATE     = 1,
    SCRAFT_ONMOUSEKEY   = 2,
    SCRAFT_ONMOUSEMOVE  = 4,
    SCRAFT_ONCOLLIDE    = 8,
    SCRAFT_ONDISPOSE    = 16
  };

struct _Scraft_Sprite_Handler :
  public IOnUpdate,
  public IOnMouseClick,
  public IOnMouseDown,
  public IOnMouseMove,
  public IOnMouseOver,
  public IOnMouseUp,
  public IOnCollide,
  public IOnDispose,
  public IScraftHandler
  {};

struct ScraftSpriteHandler : TeggoObject < TeggoCppIfs< _Scraft_Sprite_Handler > >
  {
    u32_t flags_;
    ScraftSprite* impl_;
    virtual HRESULT __stdcall QueryInterface(REFIID riid,void ** pI);
    IUnknown* __iunknown() { return (IScraftHandler*)this; }
    void Detach() { impl_ = 0; }

    ScraftSpriteHandler(ScraftSprite* s,u32_t flags);
    virtual ~ScraftSpriteHandler();

    virtual HRESULT __stdcall OnUpdate(ISprite* sprite,long tickdelta);
    virtual HRESULT __stdcall OnCollide(ISprite* sprite,ISprite* hit);
    virtual HRESULT __stdcall OnDispose(IDispatch*);
    virtual HRESULT __stdcall OnMouseDown(ISprite* sprite,float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseUp(ISprite* sprite,float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseClick(ISprite* sprite,float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseMove(ISprite* sprite,float x, float y);
    virtual HRESULT __stdcall OnMouseOver(ISprite* sprite,VARIANT_BOOL mouseIn);

    virtual ScraftSprite* GetScraftSprite();
  };

TEGGO_DECLARE_GUIDOF(IScraftHandler,0x3ae85855,0x901a,0x4c0d,0x98,0xcd,0x9d,0xad,0xbe,0xfe,0x4d,0xde);


struct ScraftSprite
  {

    ISpritePtr GetSprite() const { return sprite_; }

    virtual void OnUpdate(long tickdelta);
    virtual void OnCollide(ScraftSprite*);
    virtual void OnCollideRaw(ScraftISprite*);
    virtual void OnDispose();
    virtual void OnMouseDown(float x,float y,long btn);
    virtual void OnMouseUp(float x,float y,long btn);
    virtual void OnMouseClick(float x,float y,long btn);
    virtual void OnMouseMove(float x,float y);
    virtual void OnMouseOver(bool mouseIn);

    ScraftSprite(pwide_t name=0,u32_t flags=SCRAFT_ONUPDATE);
    virtual ~ScraftSprite();

  protected:

    struct __Property_sprite_ : teggo::rcc_ptr<ScraftISprite>
      {
      private:
        const teggo::rcc_ptr<ScraftISprite>& operator=(const teggo::rcc_ptr<ScraftISprite>& a);
      } sprite_;

  private:
    teggo::rcc_ptr<ScraftSpriteHandler>  handler_;
  };

template < unsigned flags >
  struct ScraftSpriteEx : ScraftSprite
    {
      ScraftSpriteEx() : ScraftSprite(0,flags) {}
      virtual ~ScraftSpriteEx() {}
    };

template < unsigned flags = SCRAFT_ONUPDATE, class U = ScraftIfsUnknown >
  struct ScraftSpriteObject : TeggoObject< TeggoCppIfs< ScraftSpriteEx<flags> , U > >
    {
      ScraftSpriteObject() {}
      virtual ~ScraftSpriteObject() {}
    };

//template <class T>
//  T* sprite_cast( ScraftISprite* p, T* = 0 )
//    {
//      return dynamic_cast<T*>(p->QueryScraftSprite());
//    }
//
//template <class T>
//  T* sprite_cast( const ISpritePtr& p, T* = 0 )
//    {
//      return sprite_cast(+p);
//    }
//
//template <class T>
//  teggo::rcc_ptr<T> sprite_ref_cast( ScraftISprite* p, T* = 0 )
//    {
//      return teggo::rcc_ptr<T>(Refe(sprite_cast<T>(p)));
//    }
//
//template <class T>
//  teggo::rcc_ptr<T> sprite_ref_cast( const ISpritePtr& p, T* = 0 )
//    {
//      return sprite_ref_cast(~p);
//    }
