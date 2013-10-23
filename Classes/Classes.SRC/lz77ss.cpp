/*

  there is simple & dirty LZ77 implementation

  (c)2008, Alexey Sudachen, alexey@sudachen.name

 */

#include "detect_compiler.h"
#if CXX_COMPILER_IS_MSVC_COMPATIBLE
#pragma code_seg(".text$classes")
#endif

#define _TEGGO_LZ77SS_HERE
#include "lz77ss.h"
#include "lz77ss.inl"

extern _TEGGO_EXPORTABLE void __teggo_classes_lz77ss_module() {};
