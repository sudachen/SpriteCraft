
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

#include "precompile.h"
#include "../mcore/m_time.h"
#include "../mstreams/m_igzstream.h"
#include "m_loadmodel.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mGRAPHICS")
#endif

NAMESPACE_MSKIN_BEGIN

Model3D* MOLE_FUNCALL CreateModelFrom(pchar_t name)
{
  AutoDataSource data_source ( RSCMAN::CreateSource(+widestring(name)) );
  ret0_if ( IsNull(data_source) );
  return CreateModelFrom(data_source);
}

Model3D* MOLE_FUNCALL CreateModelFrom(DataSource* source)
{
  ret0_if ( !source );
//  { // ASE file format
//    if ( source->PeekLong() == ReadShort_("*3DS") )
//      return CreateASEModelFrom(source);
//  }
  return CreateModelFrom_3DS(source);
  //ret0_because(":CreateModelFrom: unknown model format");
}

NAMESPACE_MSKIN_END
