
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

struct CXX_NO_VTABLE ScraftISpritesList
  {
    ISpritesList*  QueryIfs() { return (ISpritesList*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }

    bool           Reset();
    bool           Next();
    ISpritePtr     Get();
    void           Remove();
    void           Add(const ISpritePtr&);
    bool           Find(const ISpritePtr&);
    void           Purge();
  };

inline ISpritesList* scraft_query_ifs(ScraftISpritesList* u)
  {
    return u->QueryIfs();
  }

struct CXX_NO_VTABLE ScraftIRandomizer
  {
    IRandomizer*   QueryIfs() { return (IRandomizer*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }

    void           Reset(u32_t cookie);
    float          Next();
  };

inline IRandomizer* scraft_query_ifs(ScraftIRandomizer* u)
  {
    return u->QueryIfs();
  }

struct CXX_NO_VTABLE ScraftISpriteEnum
  {
    ISpritesEnumerator* QueryIfs() { return (ISpritesEnumerator*)this; }
    u32_t        AddRef()   { return ((IUnknown*)this)->AddRef(); }
    u32_t        Release()  { return ((IUnknown*)this)->Release(); }
  };

inline ISpritesEnumerator* scraft_query_ifs(ScraftISpriteEnum* u)
  {
    return u->QueryIfs();
  }
