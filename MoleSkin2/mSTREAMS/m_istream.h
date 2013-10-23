
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


#if !defined (__m_istream_h__)
#define __m_istream_h__


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

inline byte_t ReadByte(const byte_t*& p,const byte_t* pE) 
{ 
  retval_if_fail( p < pE , 0);
  return *p++;
}

inline u16_t ReadShort_(const byte_t* p)
{
  u16_t t = u16_t(*p);
  t |= u16_t(p[1])<<8;
  return t;
}

inline u16_t ReadNwShort_(const byte_t* p)
{
  u16_t t = u16_t(p[1]);
  t |= u16_t(p[0])<<8;
  return t;
}

inline u16_t ReadShort(const byte_t*& p,cbyte_t* pE) 
{ 
  retval_if_fail( p+2 <= pE , 0);
  u16_t r = ReadShort_(p);
  p+=2;
  return r;
}

inline u32_t ReadNwLong_(const byte_t* p) 
{ 
  u32_t t = u32_t(p[3]);
  t |= u32_t(p[2])<<8;
  t |= u32_t(p[1])<<16;
  t |= u32_t(p[0])<<24;
  return t;
}

inline u32_t ReadLong_(const byte_t* p) 
{ 
  u32_t t = u32_t(*p);
  t |= u32_t(p[1])<<8;
  t |= u32_t(p[2])<<16;
  t |= u32_t(p[3])<<24;
  return t;
}

inline u32_t ReadLong(const byte_t*& p,cbyte_t* pE) 
{ 
  retval_if_fail( p+4 <= pE , 0);
  u32_t r = ReadLong_(p);
  p+=4;
  return r;
}

inline float ReadFloat_(const byte_t* p) 
{ 
  u32_t t = u32_t(*p);
  t |= u32_t(p[1])<<8;
  t |= u32_t(p[2])<<16;
  t |= u32_t(p[3])<<24;
  return *(float*)&t;
}

inline float ReadFloat(const byte_t*& p,cbyte_t* pE) 
{ 
  retval_if_fail( p+4 <= pE , 0);
  float r = ReadFloat_(p);
  p+=4;
  return r;
}

class InputStream : public virtual Adaptable{
public:
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual unsigned Read(byte_t* dest,unsigned count) = 0;
  virtual bool EndOfStream() const = 0;
  virtual bool Reset() = 0;
  virtual unsigned Skip(unsigned) = 0;
  virtual unsigned WasRead() const = 0;
  virtual SCERROR PeekError() const = 0; 
  inline byte_t   ReadByte();
  inline u16_t ReadShort();
  inline u32_t  ReadLong();
  inline u16_t ReadNwShort();
  inline u32_t  ReadNwLong();
  inline float    ReadFloat();
  SCERROR ReadVector(BufferT<byte_t>& v);
  SCERROR ReadLine(BufferT<char>& v);
  SCERROR ReadBuffer(BufferT<byte_t>& v);
protected:
  InputStream() {}
  virtual ~InputStream() {};
private:
  InputStream& operator = (const InputStream&);
  InputStream(const InputStream&);
};

class InputStreamEx : public InputStream, public virtual IRefObject{
public:
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
protected:
  InputStreamEx() {}
  virtual ~InputStreamEx() {}
private:
  InputStreamEx& operator = (const InputStreamEx&);
  InputStreamEx(const InputStreamEx&);
};

inline byte_t InputStream::ReadByte(){
  byte_t val;
  //retval_if( Read(&val,1) != 1 , 0 );
  Read(&val,1);
  return val;
}

inline u16_t InputStream::ReadShort(){
  byte_t val[2];
  //retval_if( Read(val,2) != 2 , 0 );
  Read(val,2);
  return ReadShort_(val);
}

inline u32_t InputStream::ReadLong(){
  byte_t val[4];
  //retval_if( Read(val,4) != 4 , 0 );
  Read(val,4);
  return ReadLong_(val);
}

inline float InputStream::ReadFloat(){
  byte_t val[4];
  //retval_if( Read(val,4) != 4 , 0 );
  Read(val,4);
  return ReadFloat_(val);
}

inline u16_t InputStream::ReadNwShort(){
  byte_t val[2];
  //retval_if( Read(val,2) != 2 , 0 );
  Read(val,2);
  return ReadNwShort_(val);
}

inline u32_t InputStream::ReadNwLong(){
  byte_t val[4];
  //retval_if( Read(val,4) != 4 , 0 );
  Read(val,4);
  return ReadNwLong_(val);
}

class BasicInputStream : public InputStreamEx, private refbase{
protected:
  SCERROR ecode_;
  BasicInputStream();
  virtual ~BasicInputStream();
  SCERROR SetECode(SCERROR e) { ecode_ = e; return e; }
public:
  virtual void Grab_() const;
  virtual void Release_() const;
  virtual SCERROR PeekError() const; 
};

class MemInputStream : public BasicInputStream{
  cbyte_t* mem_;
  cbyte_t* memS_;
  cbyte_t* memE_;
public:
  MemInputStream(const void* p,const void* pE);
  MemInputStream(const BufferT<byte_t>& v);
  virtual ~MemInputStream();
  virtual unsigned Read(byte_t* dest,unsigned count);
  virtual bool EndOfStream() const;
  virtual bool Reset();
  virtual unsigned Skip(unsigned);
  virtual unsigned WasRead() const;
private:
  MemInputStream& operator = (const MemInputStream&);
  MemInputStream(const MemInputStream&);
};

#if MSKIN_HAS_FILESYSTEM

class FileInputStream: public BasicInputStream{
  widestring path_;
  FILE* hf_;
  bool eof_;
public:
  FileInputStream(pwide_t);
  virtual ~FileInputStream();
  virtual unsigned Read(byte_t* dest,unsigned count);
  virtual bool EndOfStream() const;
  virtual bool Reset();
  virtual unsigned Skip(unsigned);
  virtual unsigned WasRead() const;
private:
  FileInputStream& operator = (const FileInputStream&);
  FileInputStream(const FileInputStream&);
};

bool MOLE_FUNCALL IsFileExists(pwide_t);
bool MOLE_FUNCALL IsFileReadable(pwide_t);
bool MOLE_FUNCALL IsFileRewritable(pwide_t);
widestring MOLE_FUNCALL GetModulePath();

#endif 

NAMESPACE_MSKIN_END

#endif //__m_istream_h__

