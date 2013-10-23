
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

#ifndef ___6a30e579_6eac_48d9_ac6d_4f9b2c7db0ec___
#define ___6a30e579_6eac_48d9_ac6d_4f9b2c7db0ec___

#include "lz+.h"

#if defined _TEGGOINLINE
# define _TEGGO_LZPLUS_ENCODER_FAKE_INLINE CXX_FAKE_INLINE
#elif defined _TEGGO_LZPLUS_ENCODER_HERE
# define _TEGGO_LZPLUS_ENCODER_FAKE_INLINE _TEGGO_EXPORTABLE CXX_NO_INLINE
#endif

namespace teggo { namespace lz_plus {

  CXX_FAKE_INLINE
  void CXX_STDCALL BitsOut(u32_t code,int count,BitsState *bs,...)
    {
      u32_t b = bs->bits |(code << bs->bitsNo);
      int n = bs->bitsNo + count;
      int p = bs->p;
      while ( n > 7 )
        {
          if ( p < bs->pE )
            bs->bytes[p] = (byte_t)b;
          ++p;
          b >>= 8;
          n -= 8;
        }
      bs->bits = b;
      bs->p = p;
      bs->bitsNo = n;
    }

  _TEGGO_LZPLUS_ENCODER_FAKE_INLINE
  int CXX_STDCALL Sort(LZ_ENCODE_MEMORY *mem, byte_t const *data, int dict_len)
    {
      u16_t *dict  = mem->dict;
      u16_t *repo  = mem->repo;
      int   *last  = (int*)repo;
      u16_t *first = dict+dict_len;
      u16_t *next  = dict;

      int i,k,j;

      for ( i = 0; i < dict_len-1; ++i )
        next[i] = i+1;

      next[dict_len-1] = 0xffff;
      j = 0;

      for ( k = MAX_MATCH_LEN; k >= 0; --k )
        {

          for ( i = 0; i < 256; ++i )
            {
              last[i] = i+dict_len, first[i] = 0xffff;
            }

          while ( j != 0xffff )
            {
              int c = data[j+k];
              next[last[c]] = j;
              last[c] = j;
              j = next[j];
            }

          for ( i = 0; i < 256; ++i )
            if ( first[i] != 0x0ffff )
              {
                j = first[i]; ++i;
                break;
              }

          for ( ; i < 256; ++i )
            {
              if ( first[i] != 0x0ffff )
                {
                  next[last[i-1]] = first[i];
                }
              else
                last[i] = last[i-1];
            }
          next[last[255]] = 0xffff;
        }

      return j;
    }

  _TEGGO_LZPLUS_ENCODER_FAKE_INLINE
  void CXX_STDCALL Compress(LZ_ENCODE_MEMORY *mem, byte_t const *data, int data_len, int R)
    {
      u16_t *dict  = mem->dict;
      u16_t *repo  = mem->repo;
      u16_t *next  = dict;

      byte_t *repo_L  = mem->repo_L;

      int i,k,j;

      for ( i = 0; i <data_len; ++i )
        repo[i] = i;

      j = R, repo_L[R] = 0;
      while ( next[j] != 0xffff )
        {
          k = next[j];
          for ( i = 0; i < MAX_MATCH_LEN; ++i )
            if ( *(data+j+i) != *(data+k+i) )
              break;
          repo_L[k] = i;
          j = k;
        }

      memset(&repo_L[data_len-MAX_MATCH_LEN],0,MAX_MATCH_LEN);

      next[MAX_BLOCK_LEN_64-1] = R;
      for ( k = MAX_MATCH_LEN; k >= MATCH_TRESHOLD; --k )
        {
          j = 0;

          int m,p = MAX_BLOCK_LEN_64-1;
          j = next[MAX_BLOCK_LEN_64-1];

          while ( j != 0xffff )
            {
              i = next[j];
              m = j;

              for (; i != 0xffff && repo_L[i] >= k; i = next[i] )
                if ( i < m )
                  m = i;

              repo_L[m] = repo_L[j];

              do
                {
                  if ( j != m )
                    {
                      int l = j-m;
                      repo[j] = m;
                      repo_L[j] = l < k ? l : k;
                    }
                  j = next[j];
                }
              while ( j != i );

              next[p] = m;
              next[m] = i;
              p = m;
            }
        }
    }

  _TEGGO_LZPLUS_ENCODER_FAKE_INLINE
  int CXX_STDCALL RawEncode(void const *from, int insize, void *to, int outsize, u32_t flags, LZ_ENCODE_MEMORY *mem)
    {
      if ( insize < 0 || outsize < 0 || !mem || !from || !to )
        return LZ_E_BAD_ARGUMENTS;

      int i,j;
      int input_p = 0;
      byte_t const *input = (byte_t const*)from;
      byte_t *output = (byte_t *)to;

      BitsState bs = { output, 0, outsize, 0, 0 };
      BitsOut(flags,8,&bs);
      BitsOut(insize,32,&bs);

      while ( input_p < insize-MAX_MATCH_LEN )
        {
          int ppp = bs.p;

          byte_t const *data = input+input_p;
          int data_len = cxx_min<int>(insize-input_p,MAX_BLOCK_LEN);
          int dict_len = cxx_min<int>(insize-input_p-MAX_MATCH_LEN,MAX_SDICT_LEN);

          u16_t  *dict   = mem->dict;
          u16_t  *repo   = mem->repo;
          byte_t *repo_L = mem->repo_L;

          j = Sort(mem,data,dict_len);
          Compress(mem,data,data_len,j);

          for ( i = 0; i < data_len; )
            {
              if ( repo_L[i] < MATCH_TRESHOLD ||
                   repo_L[i] < repo_L[i+1] ||
                   (repo_L[i] == repo_L[i+1] && repo[i] > repo[i+1]) )
                {
                  repo_L[i] = 0;
                  ++i;
                }
              else
                {
                  i += repo_L[i];
                }
            }

          int code_bits_count = 1;
          int local_dict_bits = 1;
          int local_dict_f = 1;

          memset(dict,0xff,data_len*2);
          memset(mem->local_dict,0xff,sizeof(mem->local_dict));

          int last_L;
          int last_I;
          int last_N = 0;

          // encoding

          for ( i = 0; i < data_len;  )
            {
              if ( last_N )
                if ( last_I != repo[i] || last_L != repo_L[i] )
                  {
                    --last_N;
                    while ( last_N >= 1 )
                      {
                        BitsOut(1,1,&bs);
                        BitsOut(0x3f,6,&bs);
                        int r = cxx_min<int>(last_N,256);
                        BitsOut(r-1,8,&bs);
                        last_N -= r;
                      }
                  }
                else
                  {
                    ++last_N;
                    i += repo_L[i];
                    continue;
                  }

              if ( !repo_L[i] )
                {
                  BitsOut((u32_t)data[i]<<1,9,&bs);
                  ++i;
                  last_N = 0;
                  last_I = 0;
                }
              else
                {
                  REQUIRE( memcmp(data+repo[i],data+i,repo_L[i]) == 0 );
                  last_I = repo[i], last_N = 1, last_L = repo_L[i];

                  BitsOut(1,1,&bs);

                  if ( repo_L[i] > MATCH_TRESHOLD+3 )
                    BitsOut(((repo_L[i]-MATCH_TRESHOLD-3)<<1)|1,1+MATCH_LENGTH_BITS,&bs);
                  else
                    BitsOut((repo_L[i]-MATCH_TRESHOLD)<<1,3,&bs);

                  if ( dict[repo[i]] != 0x0ffff )
                    {
                      REQUIRE(local_dict_f > dict[repo[i]]);
                      BitsOut(dict[repo[i]]<<1,local_dict_bits+1,&bs);
                    }
                  else
                    {
                      while ( i >= (1 << code_bits_count) ) ++code_bits_count;
                      while ( (local_dict_f%LOCAL_DICT_SIZE) >= (1 << local_dict_bits) ) ++local_dict_bits;

                      BitsOut((repo[i]<<1)|1,code_bits_count+1,&bs);
                      if ( mem->local_dict[local_dict_f%LOCAL_DICT_SIZE] != 0x0ffff )
                        dict[mem->local_dict[local_dict_f%LOCAL_DICT_SIZE]] = 0x0ffff;
                      mem->local_dict[local_dict_f%LOCAL_DICT_SIZE] = repo[i];
                      dict[repo[i]] = (local_dict_f++)%LOCAL_DICT_SIZE;
                    }

                  i += repo_L[i];
                }
            }
          input_p += i;
        }

      while ( input_p < insize )
        BitsOut((u32_t)input[input_p++]<<1,9,&bs);

      if ( bs.bitsNo && bs.p < outsize )
        output[bs.p++] = byte_t(bs.bits);

      if ( bs.p >= outsize )
        return LZ_E_OUTPUT_OVERFLOW;

      return bs.p;
    }

}} // namespace

#if !defined _WITHOUT_TFFCLS

namespace teggo {

  _TEGGO_LZPLUS_ENCODER_FAKE_INLINE
  int CXX_STDCALL Lz_Encode(void const *from, int insize, void *to, int outsize, u32_t flags, LZ_ENCODE_MEMORY *_m)
    {
      LZ_ENCODE_MEMORY *m = _m;
      if ( !m ) m = (LZ_ENCODE_MEMORY*)malloc(sizeof(LZ_ENCODE_MEMORY));
      int e = 0;
      if ( m )
        {
          e = ::teggo::lz_plus::RawEncode(from,insize,to,outsize,flags,m);
          if ( !_m ) free(m);
        }
      else
        e = LZ_E_OUT_OF_MEMORY;
      return e;
    }

} // namespace

#endif

#undef _TEGGO_LZPLUS_ENCODER_FAKE_INLINE
#endif // ___6a30e579_6eac_48d9_ac6d_4f9b2c7db0ec___
