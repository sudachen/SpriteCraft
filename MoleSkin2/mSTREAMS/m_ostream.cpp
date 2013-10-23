
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
#include "m_ostream.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

NAMESPACE_MSKIN_BEGIN

VectorOutputStream::VectorOutputStream() {}
VectorOutputStream::~VectorOutputStream() {}

SCERROR VectorOutputStream::Write(const byte_t* src,unsigned count)
{
  v_.insert(v_.end(),src,src+count);
  return SCE_OK;
}

SCERROR VectorOutputStream::Fill(byte_t val,unsigned count)
{
  v_.resize(v_.size()+count,val);
  return SCE_OK;
}

SCERROR VectorOutputStream::Flush()  { return SCE_OK; }
SCERROR VectorOutputStream::Finish() { return SCE_OK; }

unsigned VectorOutputStream::WasWritten() const
{
  return v_.size();
}

BasicOutputStream::BasicOutputStream() : ecode_(SCE_OK) {}
BasicOutputStream::~BasicOutputStream() {}
SCERROR BasicOutputStream::PeekError() const { return ecode_; }

void BasicOutputStream::Release_() const { refbase::Release_(); }
void BasicOutputStream::Grab_() const { refbase::Grab_(); }

pchar_t const OutputStream::TypeID__ = "#TYPEID#mskin::OutputStream";
pchar_t const OutputStreamEx::TypeID__ = "#TYPEID#mskin::OutputStreamEx";

void* OutputStream::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return Adaptable::Adaptate_(id);
}

void* OutputStreamEx::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  if ( void* y = OutputStream::Adaptate_(id) ) return y;
  if ( void* x = IRefObject::Adaptate_(id) ) return x;
  return 0;
}

SCERROR LogOutputStream::Write(const byte_t* src,unsigned count)
{
  for ( unsigned i=0; i < count; ++i ) {
    if ( src[i] == '\r' ) continue;
    if ( src[i] == '\n' ) {
      buffer_.push_back(0);
      Lout << &buffer_[0];
      buffer_.resize(0);
    } else {
      buffer_.push_back(src[i]);
    }
  }
  return SCE_OK;
}

SCERROR LogOutputStream::Fill(byte_t c,unsigned count)
{
  // nothing
  return SCE_OK;
}

SCERROR LogOutputStream::Flush()
{
  if ( buffer_.size() ) Write((byte_t*)"\n",1);
  return SCE_OK;
}

SCERROR LogOutputStream::Finish()
{
  // nothing
  if ( buffer_.size() ) Write((byte_t*)"\n",1);
  return SCE_OK;
}

unsigned LogOutputStream::WasWritten() const
{
  return 0;
}

LogOutputStream::LogOutputStream()
{
  buffer_.reserve(256);
}

LogOutputStream::~LogOutputStream()
{
}

NAMESPACE_MSKIN_END
