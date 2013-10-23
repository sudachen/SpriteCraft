/*

  there is simple & dirty LZ77 implementation

  (c)2008, Alexey Sudachen, alexey@sudachen.name

 */

#ifndef ___4d88f09e_0e1b_433b_9d95_d7984107b2bd___
#define ___4d88f09e_0e1b_433b_9d95_d7984107b2bd___

#include "_specific.h"

extern "C" _TEGGO_EXPORTABLE int lz77ss_compress(char *in_b, int in_b_len, unsigned char *out_b, int out_b_len);
extern "C" _TEGGO_EXPORTABLE int lz77ss_decompress(unsigned char *in_b, int in_b_len, char *out_b, int out_b_len);

#if defined _TEGGOINLINE
#include "./lz77ss.inl"
#endif

#endif /*___4d88f09e_0e1b_433b_9d95_d7984107b2bd___*/
