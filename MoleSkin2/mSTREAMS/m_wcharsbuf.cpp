
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
#include "m_istream.h"
#include "../mcore/m_debug.h"
#include "../mcore/m_automem.h"
#include "m_wcharsbuf.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

NAMESPACE_MSKIN_BEGIN

SCERROR WcharStreamBuffer::NextChar(wchar_t* wc)
{
  STRICT_REQUIRE( p_ <= pE_ );
  if ( p_ == pE_ )  
    if ( !eos_ ) fail_if_fail( PumpBufferEx() );
    else *wc = -1;
  if ( p_ != pE_ )
  {
    *wc = *p_;
    if ( *p_ == L'\n' ) ++lineno_;
    ++p_;
  }
  return SCE_OK;
}

SCERROR WcharStreamBuffer::AheadChar(wchar_t* wc)
{
  STRICT_REQUIRE( p_ <= pE_ );
  if ( p_ == pE_ )  
    if ( !eos_ ) fail_if_fail( PumpBufferEx() );
    else return *wc = -1,SCE_OK;
  *wc = *p_;
  return SCE_OK;
}

SCERROR WcharStreamBuffer::NextWord(BaseStringT<wchar_t>* ws)
{
  STRICT_REQUIRE( p_ <= pE_ );
  ws->Resize(0);
  for (;;) {
    if ( p_ == pE_ )  fail_if_fail( PumpBufferEx() );
    if ( iswalnum(*p_) 
      || *p_ == L'#' 
      || *p_ == L':' 
      || *p_ == L'.'
      || *p_ == L'-'
      || *p_ == L'_' 
      || *p_ == L'$'
      || *p_ == L'*'
      )
    {
      ws->Append(*p_);
      ++p_;
    }
    else break;
  }
  return SCE_OK;
}

SCERROR WcharStreamBuffer::NextValue(BaseStringT<wchar_t>* ws)
{
  STRICT_REQUIRE( p_ <= pE_ );
  ws->Resize(0);
  for (;;) {
    if ( p_ == pE_ )  fail_if_fail( PumpBufferEx() );
    if ( !iswspace(*p_) ) break;
  }
  if ( *p_ == L'"' || *p_ == L'\'' ) {
    wchar_t quote = *p_;
    ++p_;
    for (;; ++p_) {
      if ( p_ == pE_ )  fail_if_fail( PumpBufferEx() );
      if ( *p_ == L'\\' ) 
      {
        ++p_;
        if ( p_ == pE_ )  fail_if_fail( PumpBufferEx() );
        if ( *p_ == L'\n' ) ++lineno_;
        ws->Append(*p_);
      }
      else 
      {
        if ( *p_ == quote ) { ++p_; break; }
        //if ( *p_ == L'\n' ) break;
        if (*p_ != L'\r') ws->Append(*p_!=L'\t'?*p_:L' ');
      }
    }
  }
  else return NextWord(ws);
  return SCE_OK;
}

SCERROR WcharStreamBuffer::SkipWS()
{
  STRICT_REQUIRE( p_ <= pE_ );
  for (;;)
  {
    if ( p_ == pE_ ) {
      if ( eos_ ) return SCE_OK;
      fail_if_fail( PumpBuffer() );
      if ( p_ == pE_ ) return SCE_OK;
    }
    //if ( *p_ == L'\r' ) break;
    //if ( *p_ == L'\t' ) break;
    if ( !iswspace(*p_) )  break;
    if ( *p_ == L'\n' ) ++lineno_;
    ++p_;
  }
  return SCE_OK;
}

SCERROR WcharStreamBuffer::SkipLine()
{
  STRICT_REQUIRE( p_ <= pE_ );
  for (;;)
  {
    if ( p_ == pE_ ) {
      if ( eos_  ) return SCE_OK;
      fail_if_fail( PumpBuffer() );
      if ( p_ == pE_  ) return SCE_OK;
    }
    if ( *p_ == L'\n' ) { ++lineno_; ++p_; break; }
    ++p_;
  }
  return SCE_OK;
}

SCERROR WcharStreamBuffer::NextLine(BaseStringT<wchar_t>* ws)
{
  STRICT_REQUIRE( p_ <= pE_ );
  for (;;)
  {
    if ( p_ == pE_ ) {
      if ( eos_  ) return SCE_OK;
      fail_if_fail( PumpBuffer() );
      if ( p_ == pE_  ) return SCE_OK;
    }
    if ( *p_ == L'\n' ) { ++lineno_; ++p_; break; }
    else
      ws->Append(*p_);
    ++p_;
  }
  return SCE_OK;
}

SCERROR WcharStreamBuffer::SkipChar()
{
  STRICT_REQUIRE( p_ <= pE_ );
  if ( p_ == pE_ )  fail_if_fail( PumpBufferEx() );
  if ( *p_ == L'\n' ) ++lineno_;
  ++p_;
  return SCE_OK;
}

SCERROR WcharStreamBuffer::PeekError()
{
  return is_->PeekError();
}

bool WcharStreamBuffer::Eos()
{
  if ( p_ == pE_ ) {
    if ( eos_ ) return true;
    PumpBuffer();
    if (  p_ == pE_ && eos_ ) return true;
  }
  return false;
}

long WcharStreamBuffer::LineNo()
{
  return lineno_;
}

SCERROR WcharStreamBuffer::PumpBuffer()
{
  unsigned count;
  byte_t sb[BUFFER_CHARS_NO*2];

  REQUIRE ( p_ == pE_ );
  if ( eos_ ) fail_because( _S*"wcsbuff: end of stream reached, file '%s'" %sourcename_ );
  
  if ( stream_type_ == UNKNOWN_STREAM ) {
    count = is_->Read(sb,2);
    if ( count == 2 ) {
      if ( sb[0] == 0xff && sb[1] == 0xfe ) // UNICODE
      {
        stream_type_ = UTF16_STREAM;
        return PumpBuffer();
      }
      else if ( sb[0] == 0xfe && sb[1] == 0xff ) // UNICODE
      {
        stream_type_ = UTF16R_STREAM;
        return PumpBuffer();
      }
      else if ( sb[0] == 0xef && sb[1] == 0xbb ) // UTF8
      {
        fail_because( _S*"wcsbuffer: utf8 is not supported, file '%s'" %sourcename_ );
      }
      else
        stream_type_ = ASCII_STREAM;
    }
    {for ( unsigned i = 0; i < count; ++i ) b_[i] = teggo::ucs2_btowc(sb[i]); }
  }
  else if ( stream_type_ == ASCII_STREAM ) 
  {
    count = is_->Read(sb,BUFFER_CHARS_NO);
    if ( count < BUFFER_CHARS_NO ) eos_ = true;
    fail_if_fail ( eof2ok( is_->PeekError() ) );
    {for ( unsigned i = 0; i < count; ++i ) b_[i] = teggo::ucs2_btowc(sb[i]); }
  }
  else if ( stream_type_ == UTF16R_STREAM || stream_type_ == UTF16_STREAM) 
  {
    count = is_->Read(sb,BUFFER_CHARS_NO*2);
    if ( count < BUFFER_CHARS_NO*2 ) eos_ = true;
    fail_if_fail ( eof2ok( is_->PeekError() ) );
    count /= 2;
    if ( stream_type_ == UTF16R_STREAM )
      {for ( unsigned i = 0; i < count; ++i ) b_[i] = ReadNwShort_(sb+i*2); }
    else
      {for ( unsigned i = 0; i < count; ++i ) b_[i] = ReadShort_(sb+i*2); }
  }

  p_ = b_;
  pE_ = p_ + count;
  
  
  return SCE_OK;
}

SCERROR WcharStreamBuffer::PumpBufferEx()
{
  SCERROR e = PumpBuffer();
  if ( SCE_OK == e && p_ == pE_ ) {
    STRICT_REQUIRE ( eos_ ); 
    fail_because( _S*"wcsbuff: end of stream reached, file '%s'" %sourcename_);
  }
  return e;
}

WcharStreamBuffer::WcharStreamBuffer(pwide_t sourcename,InputStream* is)
{
  sourcename_ = sourcename;
  is_ = is;
  eos_ = false;
  lineno_ = 1;
  p_ =  b_ + BUFFER_CHARS_NO;
  pE_ = p_;
  stream_type_ = UNKNOWN_STREAM;
}

WcharStreamBuffer::~WcharStreamBuffer()
{
}

NAMESPACE_MSKIN_END
