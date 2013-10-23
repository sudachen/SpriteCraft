
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
                      
#include "../mcore/m_autoconf.h"
#include "../mcore/m_irefobj.h"
#include "../mstreams/m_streams.h"

#if !defined __m_util_properties_h__
#define __m_util_properties_h__

NAMESPACE_MSKIN_BEGIN

class PropIterator : public virtual IRefObject{
public:
  static pchar_t const TypeID__;
  virtual void*   Adaptate_(const void*);
  virtual bool    HasMore() const = 0;
  virtual pwide_t NextKey() = 0;
  virtual void    Reset() = 0;
protected:
  PropIterator() {}
  virtual ~PropIterator() {}
private:
  PropIterator(const PropIterator&);
  PropIterator& operator=(PropIterator&);
};

class Properties : public virtual IRefObject{
public:
  static pchar_t const TypeID__;
  virtual void*   Adaptate_(const void*);
  virtual widestring  GetString(pwide_t key) const = 0;
  virtual widestring  GetLowerTrimString(pwide_t key) const;
  virtual bool    GetBool(pwide_t key) const;
  virtual void    SetString(pwide_t key, pwide_t val) = 0;
  virtual u32_t GetLong(pwide_t key) const = 0;
  virtual void    SetLong(pwide_t key,u32_t val) = 0;
  virtual bool    GetBinary(pwide_t key,BufferT<byte_t>& val) const = 0;
  virtual void    SetBinary(pwide_t key,const BufferT<byte_t>& val) = 0;
  virtual bool    HasProperty(pwide_t key) const = 0;
  virtual void    RemoveProperty(pwide_t key) = 0;
  virtual void    RemoveAllProperties() = 0;
  virtual PropIterator* CreateIterator() const = 0; 
  static Properties* Create();
protected:
  Properties() {}
  virtual ~Properties() {}
private:
  Properties(const Properties&);
  Properties& operator=(const Properties&);
};

typedef auto_ref<Properties> AutoProperties;

SCERROR LoadPropertiesFrom(InputStream* source,Properties* props);
SCERROR LoadPropertiesFrom(const BufferT<byte_t>& source,Properties* props);
Properties* CreatePropertiesFrom(InputStream* source);
Properties* CreatePropertiesFrom(const BufferT<byte_t>& source);

NAMESPACE_MSKIN_END

#endif // __m_util_properties_h__

