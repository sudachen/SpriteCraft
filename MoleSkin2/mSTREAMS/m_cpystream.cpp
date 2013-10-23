
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
#include "m_cpystream.h"

extern "C" u32_t crc32(u32_t crc, cbyte_t* buf, u32_t len);

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

NAMESPACE_MSKIN_BEGIN

int CopyStream(InputStream& i,OutputStream& o)
{
  BufferT<byte_t> buff(1024*64);
  int count = 0;
  for ( ;; ) {
    int k = i.Read(&buff[0],buff.size());
    retval_if( eof2ok(i.PeekError()), -1 );
    if ( k == 0 ) return count;
    retval_if( SCE_OK != o.Write(&buff[0],k) , -1 );
    count += k;
  }
}

SCERROR CopyStream(InputStream& i,OutputStream& o,int* count_res)
{
  return CopyStreamCRC(i,o,count_res,0);
}

SCERROR CopyStreamCRC(InputStream& i,OutputStream& o,int* count_res,u32_t* crc)
{
  BufferT<byte_t> buff(1000);
  int count = 0;
  if ( count_res ) *count_res = -1;
  for ( ;; ) {
    int k = i.Read(&buff[0],buff.size());
    fail_if_fail( eof2ok(i.PeekError()) );
    if ( k == 0 ) break;
    if ( crc ) *crc = crc32(*crc,&buff[0],k);
    fail_if_fail( o.Write(&buff[0],k) );
    count += k;
  }
  if ( count_res ) *count_res = count;
  return SCE_OK;
}

NAMESPACE_MSKIN_END
