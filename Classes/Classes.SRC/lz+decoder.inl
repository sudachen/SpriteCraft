
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

#ifndef ___a123a609_8f33_4789_af6e_aa921766ab72___
#define ___a123a609_8f33_4789_af6e_aa921766ab72___

#include "lz+.h"

#if defined _TEGGO_LZPLUS_DECODER_HERE
# define _TEGGO_LZPLUS_DECODER_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_LZPLUS_DECODER_FAKE_INLINE CXX_FAKE_INLINE
#endif

namespace teggo { namespace lz_plus {

  CXX_FAKE_INLINE
  void CXX_STDCALL PumpBits(BitsState *bs, ...)
    {
      while ( bs->bitsNo <= 24 )
        {
          if ( bs->p < bs->pE )
            bs->bits |= u32_t(bs->bytes[bs->p]) << bs->bitsNo;
          ++bs->p;
          bs->bitsNo += 8;
        }
    }

  _TEGGO_LZPLUS_DECODER_FAKE_INLINE
  int CXX_STDCALL RawDecode(void const *from, int insize, void *to, int outsize, LZ_DECODE_MEMORY *mem)
    {
      if ( insize < 0 || outsize < 0 || !mem || !from || !to )
        return LZ_E_BAD_ARGUMENTS;

      int    output_p = 0;
      byte_t *output = (byte_t *)to;

      BitsState bs = { (byte_t*)from, 0, insize, 0, 0 };
      PumpBits(&bs);
      u32_t flags  = bs.bits & 0xff; bs.bits >>= 8, bs.bitsNo -= 8;
      PumpBits(&bs);
      int data_len = bs.bits; bs.bits = 0, bs.bitsNo = 0;

      if ( data_len > outsize )
        return LZ_E_OUTPUT_OVERFLOW;

      while ( output_p < data_len )
        {
          int portial_E = output_p
            + (data_len-output_p < MAX_BLOCK_LEN
                ? data_len-output_p : MAX_BLOCK_LEN );
          int dict_base = output_p;

          int code_bits_count = 1;
          int local_dict_bits = 1;
          int local_dict_f = 1;

          while ( output_p < portial_E )
            {
              if ( bs.bitsNo < 9 ) PumpBits(&bs);
              int flag = bs.bits & 0x01; bs.bits >>= 1, --bs.bitsNo;
              if ( !flag )
                {
                  output[output_p] = byte_t(bs.bits);
                  ++output_p;
                  bs.bits >>= 8, bs.bitsNo -= 8;
                }
              else
                {
                  int l, offs, is_big, repeat = 1;

                  is_big = bs.bits & 0x01; bs.bits >>=1;

                  if ( !is_big )
                    l = (bs.bits & 0x3) + MATCH_TRESHOLD,
                        bs.bits >>=2, bs.bitsNo -= 3;
                  else
                    l = (bs.bits & ((1<<MATCH_LENGTH_BITS)-1)) + MATCH_TRESHOLD +3,
                        bs.bits >>=MATCH_LENGTH_BITS, bs.bitsNo -= MATCH_LENGTH_BITS+1;

                  is_big = bs.bits & 0x01;
                  bs.bits >>=1, --bs.bitsNo;

                  if ( !is_big )
                    {
                      if ( bs.bitsNo < local_dict_bits ) PumpBits(&bs);
                      offs = bs.bits & ((1<<local_dict_bits)-1);
                      bs.bits >>= local_dict_bits, bs.bitsNo -= local_dict_bits;
                      if ( offs >= local_dict_f || offs > LOCAL_DICT_SIZE )
                        return LZ_E_DATA_ERROR;
                      offs = mem->local_dict[offs] + dict_base;
                    }
                  else
                    {
                      while ( output_p-dict_base >= (1 << code_bits_count) ) ++code_bits_count;
                      while ( (local_dict_f%LOCAL_DICT_SIZE) >= (1 << local_dict_bits) ) ++local_dict_bits;
                      if ( bs.bitsNo < code_bits_count ) PumpBits(&bs);
                      offs = bs.bits & ((1<<code_bits_count)-1);
                      bs.bits >>= code_bits_count, bs.bitsNo -= code_bits_count;
                      mem->local_dict[(local_dict_f%LOCAL_DICT_SIZE)] = offs;
                      ++local_dict_f;
                      offs += dict_base;
                    }

                featch_repeat_loop:
                  if ( bs.bitsNo < 1+6 ) PumpBits(&bs);
                    {
                      if ( (bs.bits & 0x7f) == 0x7f )
                        {
                          bs.bits >>= 7; bs.bitsNo -= 7; PumpBits(&bs);
                          repeat += (bs.bits & 0xff) + 1;
                          bs.bits >>= 8; bs.bitsNo -= 8;
                          goto featch_repeat_loop;
                        }
                    }

                  int r = repeat;

                  while ( repeat-- )
                    {
                      int i;
                      for ( i = 0; i < l; ++i )
                        if ( output_p+i < outsize )
                          output[output_p+i] = output[offs+i];
                        else
                          return LZ_E_OUTPUT_OVERFLOW;

                      output_p += i;
                    }
                }
            }
        }

      return output_p;
    }

}} // namespace

#if !defined _WITHOUT_TFFCLS

namespace teggo {

  _TEGGO_LZPLUS_DECODER_FAKE_INLINE
  int CXX_STDCALL Lz_Decode(void const *from, int insize, void *to, int outsize, LZ_DECODE_MEMORY *_m)
    {
      LZ_DECODE_MEMORY *m = _m;
      if ( !m ) m = (LZ_DECODE_MEMORY*)malloc(sizeof(LZ_DECODE_MEMORY));
      int e = 0;
      if ( m )
        {
          e = ::teggo::lz_plus::RawDecode(from,insize,to,outsize,m);
          if ( !_m ) free(m);
        }
      else
        e = LZ_E_OUT_OF_MEMORY;
      return e;
    }

} // namespace

#endif

#undef _TEGGO_LZPLUS_DECODER_FAKE_INLINE
#endif // ___a123a609_8f33_4789_af6e_aa921766ab72___
