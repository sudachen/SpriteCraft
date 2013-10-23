
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

#ifndef ___268e8c0f_7012_42cc_b1f4_e226a553c61b___
#define ___268e8c0f_7012_42cc_b1f4_e226a553c61b___

#if !defined _WITHOUT_TFFCLS
#include "./_specific.h"
#else
#include "./detect_compiler.h"
#endif

#if !defined _WITHOUT_TFFCLS && !defined _TEGGOINLINE
#define _TEGGO_LZPLUS_EXPORTABLE _TEGGO_EXPORTABLE
#else
#define _TEGGO_LZPLUS_EXPORTABLE
#endif

namespace teggo {

    enum /* encoder flags */
      {
        LZ_DO_HUFFMAN  = CXX_BIT(1),
        LZ_DO_CRCCHECK = CXX_BIT(2),
        LZ_CODE_FILTER = CXX_BIT(3),
      };

    enum /* errors */
      {
        LZ_E_OUT_OF_MEMORY    = -1,
        LZ_E_BAD_ARGUMENTS    = -2,
        LZ_E_OUTPUT_OVERFLOW  = -3,
        LZ_E_DATA_ERROR       = -4,
      };

    namespace lz_plus {

      enum /* internal constants */
        {
          MATCH_TRESHOLD  = 3,
          MAX_MATCH_LEN   = 15+MATCH_TRESHOLD+2,
          MATCH_LENGTH_BITS = 4,
          MAX_BLOCK_LEN_64 = 64*1024,
          MAX_BLOCK_LEN   = MAX_BLOCK_LEN_64 - 256,
          MAX_SDICT_LEN   = MAX_BLOCK_LEN - MAX_MATCH_LEN,
          LOCAL_DICT_SIZE = 4096,
          HF_LENGTH       = 256+MAX_MATCH_LEN-MATCH_TRESHOLD+1
        };

      struct BitsState
        {
          byte_t *bytes;
          int p;
          int pE;
          u32_t bits;
          int bitsNo;
        };

      void CXX_STDCALL BitsOut(u32_t code,int count,BitsState *bs,...);
      u32_t CXX_STDCALL BitsIn(int count,BitsState *bs,...);

      struct LZ_ENCODE_MEMORY
        {
          u16_t   dict[MAX_BLOCK_LEN_64];
          u16_t   repo[MAX_BLOCK_LEN_64];
          byte_t  repo_L[MAX_BLOCK_LEN_64];
          union {
            byte_t  shadow[MAX_BLOCK_LEN_64];
            u16_t   local_dict[LOCAL_DICT_SIZE];
          };
          //u16_t   hf_fq[HF_LENGTH];
        };

      _TEGGO_LZPLUS_EXPORTABLE
      int CXX_STDCALL RawEncode(void const *from, int insize, void *to, int outsize, u32_t flags, LZ_ENCODE_MEMORY *mem);

      struct LZ_DECODE_MEMORY
        {
          u16_t local_dict[LOCAL_DICT_SIZE];
          //u16_t hf_left[HF_LENGTH];
          //u16_t hf_right[HF_LENGTH];
        };

      _TEGGO_LZPLUS_EXPORTABLE
      int CXX_STDCALL RawDecode(void const *from, int insize, void *to, int outsize, LZ_DECODE_MEMORY *mem);

    } // namespace


  typedef teggo::lz_plus::LZ_DECODE_MEMORY LZ_DECODE_MEMORY;
  typedef teggo::lz_plus::LZ_ENCODE_MEMORY LZ_ENCODE_MEMORY;

  _TEGGO_LZPLUS_EXPORTABLE
  int CXX_STDCALL Lz_Encode(
    void const *from, int insize, void *to, int outsize,
    u32_t flags = LZ_DO_HUFFMAN, LZ_ENCODE_MEMORY *mem = 0);

  _TEGGO_LZPLUS_EXPORTABLE
  int CXX_STDCALL Lz_Decode(
    void const *from, int insize, void *to, int outsize,
    LZ_DECODE_MEMORY *mem = 0);

} // namespace

#if defined _TEGGOINLINE || defined _TEGGO_LZPLUS_ENCODER_HERE
#include "lz+.inl"
#endif

#if defined _TEGGOINLINE || defined _TEGGO_LZPLUS_DECODER_HERE
#include "lz+decoder.inl"
#endif

#endif //___268e8c0f_7012_42cc_b1f4_e226a553c61b___
