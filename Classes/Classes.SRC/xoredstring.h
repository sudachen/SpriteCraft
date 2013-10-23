
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

#ifndef ___4d259329_5ee0_475c_9566_f52f55850619___
#define ___4d259329_5ee0_475c_9566_f52f55850619___

namespace teggo {

  char* XoredStringA_Xor(unsigned char const *data, unsigned n, char *val)
    {
      int x = data[0];
      for ( unsigned i = 1; i < n; ++i )
        {
          val[i-1] = data[i] ^ x;
          x = ((x << 1) | (x >> 7))  & 0x0ff;
        }
      val[n-1] = 0;
      return val;
    }

  template < unsigned tN >
    struct XoredStringA_
      {
        mutable char value[tN];
      };


  template < unsigned tN >
    struct XoredStringA
      {
        unsigned char text[tN];
        inline char *Xor(XoredStringA_<tN> const &a = XoredStringA_<tN>())
          {
            return XoredStringA_Xor(text,tN,a.value);
          }
      };

} // namespace

#endif // ___4d259329_5ee0_475c_9566_f52f55850619___
