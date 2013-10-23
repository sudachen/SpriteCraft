
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

#if !defined __SpriteCraft_Impl_SpritesEnumerator_h__
#define __SpriteCraft_Impl_SpritesEnumerator_h__

struct SpritesEnumerator : 
  TeggoObject<
    TeggoPyDispImpl< ISpritesEnumerator, 0, TeggoISupportErrorInfo > >
{
  typedef ISprite* e_VALUE;
  typedef ArrayT<e_VALUE> e_VECTOR;
  e_VECTOR sprites_;
  e_VALUE *iter_;
  e_VALUE *value_;
public:
	HRESULT __stdcall Get(/*[out,retval]*/ ISprite** pI);

  SpritesEnumerator() 
    { 
      EnumerateSprites(LONG_MIN,LONG_MAX); 
    }
  
  SpritesEnumerator(long lnear,long lfar) 
    { 
      EnumerateSprites(lnear,lfar); 
    }
  
  ~SpritesEnumerator() 
    {
      for ( e_VALUE *i = sprites_.Begin(), *iE = sprites_.End(); i != iE; ++i ) 
        {
          (*i)->Release();
        }
    }

  void EnumerateSprites(long lnear,long lfar);

  HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
	HRESULT __stdcall Item(/*[in]*/ long,/*[out,retval]*/ ISprite** pI);
	HRESULT __stdcall get_count(/*[out, retval]*/ long *pVal);
	HRESULT __stdcall Next(/*[out,retval]*/ VARIANT_BOOL* bRet);
	HRESULT __stdcall Reset(/*[out,retval]*/ VARIANT_BOOL* bRet);
	HRESULT __stdcall get_value(/*[out, retval]*/ ISprite* *pVal);
};

#endif // __SpriteCraft_Impl_SpritesEnumerator_h__
