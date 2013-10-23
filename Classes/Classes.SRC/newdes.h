
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

#ifndef ___6ea1d37e_eace_4986_b99a_fcd335057356___
#define ___6ea1d37e_eace_4986_b99a_fcd335057356___

#if !defined _WITHOUT_TFFCLS
#include "./_specific.h"
#else
#include "./detect_compiler.h"
#define _TEGGO_EXPORTABLE
#endif

namespace teggo {

  struct NEWDES_Cipher // trivial 'aka C' structure
    {
      enum { BLOCK_BYTES = 8 };
      enum { KEY_BYTES   = 15 };
      enum { UNRAV_BYTES = 60 };
      enum { UNRAV_BYTES_1 = 68 };
      enum _CONFIGURE_FLAG { ENCRYPTION, DECRYPTION };

      _TEGGO_EXPORTABLE void SetupEncipher(void const *key);
      _TEGGO_EXPORTABLE void SetupDecipher(void const *key);
      _TEGGO_EXPORTABLE void DoCipher(void *data, u32_t count_of_blocks);
      _TEGGO_EXPORTABLE void DoCipherBlock(void *b);
      _TEGGO_EXPORTABLE void Normalize_(byte_t const *kunrav);

      union
        {
          unsigned _;
          unsigned char key_unrav[UNRAV_BYTES_1];
        };
    };
}

//#if defined _X86_ASSEMBLER
extern "C" void NEWDES_DoCipher_S(void const *key,void *b,int count);
//#endif

#include "./newdes.inl"

#endif // ___6ea1d37e_eace_4986_b99a_fcd335057356___
