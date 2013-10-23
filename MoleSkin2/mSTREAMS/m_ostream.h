
/*

Copyright ¬© 2003-2013, Alex√©y Sudach√©n, alexey@sudachen.name

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


#if !defined (__m_ostream_h__)
#define __m_ostream_h__


#include "m_xxconf.h"

#if MSKIN_HAS_FILESYSTEM
# include <stdio.h>
# if MSKIN_TARGET_PLATFORM == MSKIN_PLATFORM_DOBL
#  include "../mcore/m_DOBL.h"
# endif
#endif

//#include <string>
//using namespace std;

NAMESPACE_MSKIN_BEGIN

/*
  œ–≈ƒœŒÀŒ√¿≈“—ﬂ ◊“Œ œŒ“Œ  »Ã≈≈“ ‘Œ–Ã¿“  ¿  œ¿Ãﬂ“‹ œ–Œ÷≈——Œ–¿ INTEL  
*/

inline SCERROR WriteByte(byte_t*& p,const byte_t* pE, byte_t val) 
{ 
  retval_if_fail( p < pE , SCE_OUT_OF_STREAM );
  *p++ = val;
  return SCE_OK;
}

inline void WriteShort_(byte_t* p, u16_t val)
{
  p[0] = byte_t(val);
  p[1] = byte_t(val>>8);
}

inline SCERROR WriteShort(byte_t*& p,cbyte_t* pE, u16_t val) 
{ 
  retval_if_fail( p+2 <= pE , SCE_OUT_OF_STREAM );
  WriteShort_(p,val);
  p+=2;
  return SCE_OK;
}

inline void WriteLong_(byte_t* p,u32_t val) 
{ 
  p[0] = byte_t(val);
  p[1] = byte_t(val >> 8);
  p[2] = byte_t(val >> 16);
  p[3] = byte_t(val >> 24);
}

inline SCERROR WriteLong(byte_t*& p,cbyte_t* pE, u32_t val) 
{ 
  retval_if_fail( p+4 <= pE , SCE_OUT_OF_STREAM );
  WriteLong_(p,val);
  p+=4;
  return SCE_OK;
}

class OutputStream : public virtual Adaptable{
public:
  static pchar_t const TypeID__;
  void* Adaptate_(const void*);
  virtual SCERROR Write(const byte_t* src,unsigned count) = 0;
  virtual SCERROR Fill(byte_t,unsigned) = 0;
  virtual SCERROR Flush() = 0;
  virtual SCERROR Finish() = 0;
  virtual unsigned WasWritten() const = 0;
  virtual SCERROR PeekError() const = 0; 
  SCERROR WriteVector(const BufferT<byte_t>& v) { return Write(&v[0],v.size()); }
  SCERROR WritePChar(pchar_t src) { return Write((const byte_t*)src,strlen(src)+1); }
  SCERROR WriteString(BaseStringT<char> const &s) { return Write((const byte_t*)s.Str(),s.Length()+1); }
  SCERROR WriteS(pchar_t p) { return Write((const byte_t*)p,strlen(p)); }
  SCERROR WriteByte(byte_t);
  SCERROR WriteShort(u16_t);
  SCERROR WriteLong(u32_t);
protected:
  OutputStream() {}
  virtual ~OutputStream() {};
private:
  OutputStream& operator = (const OutputStream&);
  OutputStream(const OutputStream&);
};

class OutputStreamEx : public OutputStream, public virtual IRefObject{
public:
  static pchar_t const TypeID__;
  void* Adaptate_(const void*);
protected:
  virtual ~OutputStreamEx() {}
  OutputStreamEx() {}
private:
  OutputStreamEx& operator = (const OutputStreamEx&);
  OutputStreamEx(const OutputStreamEx&);
};

inline SCERROR OutputStream::WriteByte(byte_t val){
  return Write(&val,1);
}

inline SCERROR OutputStream::WriteShort(u16_t val){
  byte_t b[2];
  WriteShort_(b,val);
  return Write(b,2);
}

inline SCERROR OutputStream::WriteLong(u32_t val){
  byte_t b[4];
  WriteLong_(b,val);
  return Write(b,4);
}

class BasicOutputStream : public OutputStreamEx, private refbase{
protected:
  SCERROR ecode_;
  BasicOutputStream();
  virtual ~BasicOutputStream();
  SCERROR SetECode(SCERROR e) { ecode_ = e; return e; }
public:
  virtual void Grab_() const;
  virtual void Release_() const;
  virtual SCERROR PeekError() const; 
};

class VectorOutputStream : public BasicOutputStream{
  BufferT<byte_t> v_;
public:
  virtual SCERROR Write(const byte_t* src,unsigned count);
  virtual SCERROR Fill(byte_t,unsigned);
  virtual SCERROR Flush();
  virtual SCERROR Finish();
  virtual unsigned WasWritten() const;
  VectorOutputStream();
  virtual ~VectorOutputStream();
  void Swap(BufferT<byte_t>& v) { v.swap(v_); }
  const BufferT<byte_t>& GetVector() const { return v_; }
private:
  VectorOutputStream& operator = (const VectorOutputStream&);
  VectorOutputStream(const VectorOutputStream&);
};

class LogOutputStream : public BasicOutputStream{
  BufferT<char> buffer_;
public:
  virtual SCERROR Write(const byte_t* src,unsigned count);
  virtual SCERROR Fill(byte_t,unsigned);
  virtual SCERROR Flush();
  virtual SCERROR Finish();
  virtual unsigned WasWritten() const;
  LogOutputStream();
  virtual ~LogOutputStream();
private:
  LogOutputStream& operator = (const LogOutputStream&);
  LogOutputStream(const LogOutputStream&);
};

#if MSKIN_HAS_FILESYSTEM

class FileOutputStream: public BasicOutputStream{
  bool own_fd_;
  widestring path_;
  FILE* hf_;
  unsigned written_;
//  unsigned start_pos_;
public:
  virtual SCERROR Write(const byte_t* src,unsigned count);
  virtual SCERROR Fill(byte_t,unsigned);
  virtual SCERROR Flush();
  virtual SCERROR Finish();
  virtual SCERROR Skip(unsigned k);
  virtual unsigned WasWritten() const;
  FileOutputStream(pwide_t);
  FileOutputStream(FILE* fd);
  virtual ~FileOutputStream();
private:
  void Init( pwide_t );
  FileOutputStream& operator = (const FileOutputStream&);
  FileOutputStream(const FileOutputStream&);
};

#endif 

NAMESPACE_MSKIN_END

#endif //__m_ostream_h__

