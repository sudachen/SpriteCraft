
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
#include "m_xxconf.h"
#include "m_streams.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

NAMESPACE_MSKIN_BEGIN

static const char slotfromat[] = "SLOT%02d.slt";

#if defined pragma_has_filesystem

SCERROR MOLE_FUNCALL WriteSlot(int no,const BufferT<byte_t>& bits)
{
  retval_if ( no < 0 || no >= GetNumberOfSlots() , SCE_INVALIDARG );
  string fname(Format(slotfromat,no));
  FileOutputStream fos(fname.c_str());
  reterr_if ( fos.PeekError() );
  const byte_t* p = &const_cast< BufferT<byte_t>& >(bits)[0];
  reterr_if ( fos.Write(p,bits.size()) );
  reterr_if ( fos.Finish() );
  return SCE_OK;
}

SCERROR MOLE_FUNCALL ReadSlot(int no,vector<byte_t>& bits)
{
  retval_if ( no < 0 || no >= GetNumberOfSlots() , SCE_INVALIDARG );
  string fname(Format(slotfromat,no));
  FileInputStream fis(fname.c_str());
  reterr_if ( fis.PeekError() );
  bits.clear();
  reterr_if( ReadStream(fis,bits) );
  return SCE_OK;
}

int MOLE_FUNCALL GetNumberOfSlots()
{
  return 16;
}

#elif defined pragma_ptv_DOBL

SCERROR MOLE_FUNCALL WriteSlot(int no,const vector<byte_t>& bits)
{
  retval_if ( no < 0 || no >= GetNumberOfSlots() , SCE_INVALIDARG );
  return SCE_OK;
}

SCERROR MOLE_FUNCALL ReadSlot(int no,vector<byte_t>& bits)
{
  retval_if ( no < 0 || no >= GetNumberOfSlots() , SCE_INVALIDARG );
  return SCE_OK;
}

int MOLE_FUNCALL GetNumberOfSlots()
{
  return 16;
}

#endif

NAMESPACE_MSKIN_END