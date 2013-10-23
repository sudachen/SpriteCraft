
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
#if !defined __1655743D_1A4A_4600_8306_F4745CB2F83B__
#define __1655743D_1A4A_4600_8306_F4745CB2F83B__

struct Engine_IDispSubImpl : 
  IOnUpdate, 
  IOnCollide, 
  IOnMouseClick, 
  IOnMouseUp, 
  IOnMouseDown, 
  IOnMouseMove, 
  IOnMouseOver, 
  IOnDispose, 
  IOnLifetimeOut,
  IOnEmission
  {
    typedef GuidT<0x322e2920, 0x7b27, 0x4fdf,
                  0xbe, 0x2f, 0x18, 0x8c, 0x6f, 0x9b, 0x6c, 0xf7> Guid;

    virtual HRESULT InternalQueryInterface2(REFIID riid ,void ** pI) = 0;
    
    HRESULT InternalQueryInterface(REFIID riid ,void ** pI)
      {
        return InternalQueryInterface2(riid,pI);
      }

    u32_t __stdcall AddRef() = 0;
    u32_t __stdcall Release() = 0;
  };

struct Engine_DispSubImpl : TeggoObject<Engine_IDispSubImpl>
  {

    enum MODE { ADVISE, DISPATCH };
    rcc_ptr<IDispatch> impl_;
    MODE mode_;
  
    long 
      disp_IOnUpdate_OnUpdate,
      disp_IOnCollide_OnCollide,
      disp_IOnMouseClick_OnMouseClick, 
      disp_IOnMouseUp_OnMouseUp, 
      disp_IOnMouseDown_OnMouseDown, 
      disp_IOnMouseMove_OnMouseMove, 
      disp_IOnMouseOver_OnMouseOver, 
      disp_IOnDispose_OnDispose, 
      disp_IOnLifetimeOut_OnLifetimeOut,
      disp_IOnEmission_OnEmission,
      disp_QI
      ;

    virtual HRESULT InternalQueryInterface2(REFIID riid ,void ** pI);

    void GetDispId(pwide_t method,long* dispid,long def); 
    Engine_DispSubImpl(IDispatch* impl,MODE mode);
    virtual HRESULT __stdcall OnUpdate(ISprite* sprite,long tickdelta);
    virtual HRESULT __stdcall OnCollide(ISprite* sprite,ISprite* hit);
    virtual HRESULT __stdcall OnDispose(IDispatch* obj);
    virtual HRESULT __stdcall OnMouseDown(ISprite* sprite, float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseUp(ISprite* sprite, float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseClick(ISprite* sprite, float x, float y,long btn);
    virtual HRESULT __stdcall OnMouseMove(ISprite* sprite, float x, float y);
    virtual HRESULT __stdcall OnMouseOver(ISprite* sprite, VARIANT_BOOL mouseIn);
    virtual HRESULT __stdcall OnLifetimeOut(IParticles* particles);
    virtual HRESULT __stdcall OnEmission(IParticles* particles, long tickdelta);
  };

#endif // __1655743D_1A4A_4600_8306_F4745CB2F83B__
