
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

/* --------------------------------------------------------------------------------------------- */
struct ScraftISpritePrimitive
{
  typedef teggo::GuidT<0x874095DD,0x87DA,0x48bd,0x85,0x2B,0xA7,0x05,0x13,0x12,0xE8,0xCC> Guid;

  ISpritePrimitive*  QueryIfs() { return (ISpritePrimitive*)this; }

  u32_t AddRef()   { return ((IUnknown*)this)->AddRef(); }
  u32_t Release()  { return ((IUnknown*)this)->Release(); }

  long    GetCount();
  void    SetCount(long);
  float   GetOffsetX();
  void    SetOffsetX(float);
  float   GetOffsetY();
  void    SetOffsetY(float);
  bool    IsClip();
  void    SetClip(bool);
  bool    IsColorized();
  void    SetColorized(bool);
  tagBlendMode  GetBlendMode();
  void    SetBlendMode(tagBlendMode);
  bool    IsNormUV();
  void    SetNormUV(bool);
  bool    IsCW();
  void    SetCW(bool);
  void    SetColor(long no,long count,long color);
  void    SetXY(long no,float x,float y);
  void    SetUV(long no,float u,float v);
  void    SetVertex(long no,long color,float x,float y,float u,float v);
  void    DiffVertex(long no,long count,float x,float y,float u=0,float v=0);
  float   GetX(long no);
  float   GetY(long no);
  float   GetU(long no);
  float   GetV(long no);
  long    GetColor(long no);
  void    SetVertexEx(long no,struct PRIMITIVE_VERTEX* vertex,long count=1);
  void    GetVertexEx(long no,struct PRIMITIVE_VERTEX* vertex,long count=1);
};
