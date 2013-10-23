
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

#if !defined __8A8D5E5E_F001_4D51_BA50_BC47530AB702__
#define __8A8D5E5E_F001_4D51_BA50_BC47530AB702__

struct SNDinfo
  {
    widestring klass;
    widestring rsc;
    bool streamed;
    u32_t vol;

    void Fill_SDN_INFO(SOUND_INFO *info)
      {
        info->klass = klass.Str();
        info->resource = rsc.Str();
        info->streamed = streamed;
        info->vol = vol;
      }

    void operator =( SOUND_INFO const *info )
      {
        klass = info->klass;
        rsc = info->resource;
        streamed = info->streamed;
        vol = info->vol;
      }

    SNDinfo( SOUND_INFO const *info )
      {
        operator = ( info );
      }

    SNDinfo() 
      {
      }
  };

#endif // __8A8D5E5E_F001_4D51_BA50_BC47530AB702__
