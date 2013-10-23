
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
#include "m_istream.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

NAMESPACE_MSKIN_BEGIN

MemInputStream::MemInputStream(const void* void_p,const void* void_pE)
{
  //REQUIRE ( p != 0 && pE >= p );
  cbyte_t* p  = (cbyte_t*)void_p;
  cbyte_t* pE = (cbyte_t*)void_pE;
  mem_ = memS_ = memE_= p;
  if ( p == 0 || pE < p  ) { SetECode(SCE_BAD_STREAM); return; } 
  memE_ = pE;
}

MemInputStream::MemInputStream(const BufferT<byte_t>& v)
{
  mem_ = memS_ = &const_cast<BufferT<byte_t>&>(v)[0];
  memE_ = memS_+v.size();
}

MemInputStream::~MemInputStream()
{
}

unsigned MemInputStream::Read(byte_t* dest,unsigned count)
{
  REQUIRE ( mem_ <= memE_ && mem_ >= memS_ );
  unsigned cnt = /*std::*/teggo_min(count,(unsigned)(memE_-mem_));
  memcpy(dest,mem_,cnt);
  mem_+=cnt;
  return cnt;
}

bool MemInputStream::EndOfStream() const
{
  STRICT_REQUIRE ( mem_ <= memE_ && mem_ >= memS_ );
  return ( memE_-mem_ == 0 );
}

bool MemInputStream::Reset()
{
  mem_ = memS_;
  return true;
}

unsigned MemInputStream::Skip(unsigned bytes)
{
  STRICT_REQUIRE ( mem_ <= memE_ && mem_ >= memS_ );
  bytes = /*std::*/teggo_min ( bytes , (unsigned)(memE_-mem_) );
  mem_+=bytes;
  return bytes;
}

unsigned MemInputStream::WasRead() const
{
  STRICT_REQUIRE ( mem_ <= memE_ && mem_ >= memS_ );
  return mem_-memS_;
}

BasicInputStream::BasicInputStream() : ecode_(SCE_OK) {}
BasicInputStream::~BasicInputStream() {}
SCERROR BasicInputStream::PeekError() const { return ecode_!=SCE_OK?ecode_:EndOfStream()?SCE_EOF:SCE_OK; }

void BasicInputStream::Release_() const { refbase::Release_(); }
void BasicInputStream::Grab_() const { refbase::Grab_(); }

pchar_t const InputStream::TypeID__ = "#TYPEID#mskin::InputStream";
pchar_t const InputStreamEx::TypeID__ = "#TYPEID#mskin::InputStreamEx";

void* InputStream::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return Adaptable::Adaptate_(id);
}

void* InputStreamEx::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  if ( void* y = InputStream::Adaptate_(id) ) return y;
  if ( void* x = IRefObject::Adaptate_(id) ) return x;
  return 0;
}

SCERROR InputStream::ReadVector(BufferT<byte_t>& v) {
  v.clear();
  for ( ;; ) {
    byte_t b[128];
    int k = Read(b,sizeof(b));
    fail_if_err( eof2ok(PeekError()) );
    if ( k == 0 ) return SCE_OK;
    v.insert(v.end(),b,b+k);
  }
}

SCERROR InputStream::ReadLine(BufferT<char>& line) {
  line.Clear();
  for ( ;; ) {
    char val = ReadByte();
    fail_if_err( eof2ok(PeekError()) );
    if ( val == '\n' || EndOfStream() ) break;
    if ( val != '\r' ) line.Push(val);
  }
  line.Push(0);
  return SCE_OK;
}

SCERROR InputStream::ReadBuffer(BufferT<byte_t>& v) {
  v.Clear();
  for ( ;; ) {
    byte_t b[128];
    int k = Read(b,sizeof(b));
    fail_if_err( eof2ok(PeekError()) );
    if ( k == 0 ) return SCE_OK;
    v.Insert(v.End(),b,b+k);
  }
}

NAMESPACE_MSKIN_END
