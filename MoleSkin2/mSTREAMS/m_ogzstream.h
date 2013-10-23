
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



#if !defined (__m_ogzstream_h__)
#define __m_ogzstream_h__

#include "../mcore/m_autoconf.h"
#include "m_ostream.h"

struct z_stream_s;
typedef struct z_stream_s z_stream;

NAMESPACE_MSKIN_BEGIN

class GZipOutputStream : public BasicOutputStream{
  auto_ref<OutputStreamEx> osex_;
  OutputStream* os_;
  bool is_gzip_;
  byte_t* buff_;
  u32_t crc_;
  z_stream* zs_;
  unsigned written_;
  bool finished_;
  SCERROR Finish_(bool ziponly);
public:
  enum COMPRESSONLYSWITCHER {COMPRESSONLY};
  explicit GZipOutputStream(OutputStreamEx* os,pchar_t=0,u32_t level=6);
  explicit GZipOutputStream(OutputStreamEx* os,COMPRESSONLYSWITCHER,u32_t level=6);
  explicit GZipOutputStream(OutputStream& os,u32_t level=6);
  virtual ~GZipOutputStream();
  virtual SCERROR Write(const byte_t* src,unsigned count);
  virtual SCERROR Fill(byte_t,unsigned);
  virtual SCERROR Flush();
  virtual SCERROR Finish();
  SCERROR FinishZipOnly() { return Finish_(true); }
  virtual unsigned WasWritten() const;
  u32_t GetCRC() const { return crc_; }
private:
  void Init_(pchar_t,u32_t,bool);
  SCERROR WriteHeader(pchar_t);
  GZipOutputStream& operator = (const GZipOutputStream&);
  GZipOutputStream(const GZipOutputStream&);
};

NAMESPACE_MSKIN_END

#endif //__m_ogzstream_h__

