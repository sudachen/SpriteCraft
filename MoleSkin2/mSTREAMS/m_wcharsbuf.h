
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



#if !defined (__m_wcharsbuf_h__)
#define __m_wcharsbuf_h__

#include "../mcore/m_autoconf.h"
#include "m_istream.h"

NAMESPACE_MSKIN_BEGIN

struct WcharStreamBuffer {
  SCERROR NextChar(wchar_t* wc);
  SCERROR AheadChar(wchar_t* wc);
  SCERROR NextWord(BaseStringT<wchar_t>* ws);
  SCERROR NextValue(BaseStringT<wchar_t>* ws);
  SCERROR NextLine(BaseStringT<wchar_t>* ws);
  SCERROR SkipWS();
  SCERROR SkipChar();
  SCERROR SkipLine();
  SCERROR PeekError();
  bool        Eos();
  long        LineNo();
  pwide_t     GetSourceName() { return sourcename_.Str(); }

  WcharStreamBuffer(pwide_t sourcename,InputStream*);
  ~WcharStreamBuffer();

private:

  enum { BUFFER_CHARS_NO = 256 };
  enum STREAM_TYPE { UNKNOWN_STREAM, UTF16_STREAM, UTF16R_STREAM, ASCII_STREAM };
  SCERROR PumpBuffer();
  SCERROR PumpBufferEx();

  STREAM_TYPE   stream_type_;
  InputStream*  is_;
  long          lineno_;
  wchar_t       b_[BUFFER_CHARS_NO];
  wchar_t*      p_;
  wchar_t*      pE_;
  bool          eos_;
  widestring    sourcename_;
};

NAMESPACE_MSKIN_END

#endif // __m_wcharsbuf_h__
