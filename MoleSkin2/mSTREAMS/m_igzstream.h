
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



#if !defined (__m_igzstream_h__)
#define __m_igzstream_h__


#include "../mcore/m_autoconf.h"
#include "m_istream.h"

struct z_stream_s;
typedef struct z_stream_s z_stream;

NAMESPACE_MSKIN_BEGIN

class GZipInputStream : public BasicInputStream{
  auto_ref<InputStreamEx> is_;
  bool in_gzip_;
  byte_t* inbuff_;
  u32_t crc_;
  u32_t Xcrc_;
  u32_t start_;
  z_stream* zs_;
  bool eof_;
  unsigned wasread_;
public:
  enum UNCOMPRESSONLYSWITCHER{ UNCOMPRESSONLY };
  explicit GZipInputStream(InputStreamEx* is);
  explicit GZipInputStream(InputStreamEx* is,UNCOMPRESSONLYSWITCHER,u32_t crc=0);
  virtual ~GZipInputStream();
  virtual unsigned Read(byte_t* dest,unsigned count);
  virtual bool EndOfStream() const;
  virtual bool Reset();
  virtual unsigned Skip(unsigned);
  virtual unsigned WasRead() const;
private:
  void Init_();
  void CheckHeader();
  u32_t GetByte();
  GZipInputStream& operator = (const GZipInputStream&);
  GZipInputStream(const GZipInputStream&);
};

NAMESPACE_MSKIN_END

#endif //__m_igzstream_h__

