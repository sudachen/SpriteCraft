
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
#include "m_properties.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mUTIL")
#endif

NAMESPACE_MSKIN_BEGIN

class GenericProperties : public Properties, private refbase{
  typedef CollectionT<widestring> CONTAINER;
  CONTAINER map_; 
public:
  virtual void Release_() const { refbase::Release_(); }
  virtual void Grab_()    const { refbase::Grab_(); }
  GenericProperties();
  virtual ~GenericProperties();
  virtual widestring  ToString_() const;
  virtual widestring  GetString(pwide_t key) const;
  virtual void    SetString(pwide_t key, pwide_t val);
  virtual void    SetString(pwide_t key, pchar_t val);
  virtual u32_t GetLong(pwide_t key) const;
  virtual void    SetLong(pwide_t key,u32_t val);
  virtual bool    GetBinary(pwide_t key,BufferT<byte_t>& val) const;
  virtual void    SetBinary(pwide_t key,const BufferT<byte_t>& val);
  virtual bool    HasProperty(pwide_t key) const;
  virtual void    RemoveProperty(pwide_t key);
  virtual void    RemoveAllProperties();
  virtual PropIterator* CreateIterator() const; 
};

class GenericPropertiesIter : public PropIterator, private refbase{
  BufferT<widestring> keys_;
  teggo::BufferT<widestring>::iterator keysI_;
  friend class GenericProperties;
public:
  virtual void Release_() const { refbase::Release_(); }
  virtual void Grab_()    const { refbase::Grab_(); }
  virtual pwide_t NextKey()  { 
    if ( keysI_ != keys_.end() ) 
      return (*keysI_++).Str();
    return L"";
  }
  virtual bool HasMore() const { return keysI_ != keys_.end(); }
  virtual void Reset() { keysI_ = keys_.begin(); }
};

pchar_t const Properties::TypeID__ = "#TYPEID#mskin::Properties";
void* Properties::Adaptate_(const void* tid)
{
  if ( tid == Properties::TypeID__ ) return this;
  else return IRefObject::Adaptate_(tid);
}
pchar_t const PropIterator::TypeID__ = "#TYPEID#mskin::PropIterator";
void* PropIterator::Adaptate_(const void* tid)
{
  if ( tid == PropIterator::TypeID__ ) return this;
  else return IRefObject::Adaptate_(tid);
}

widestring GenericProperties::GetString(pwide_t key) const
{
  if ( widestring *p = map_.Get(key) )
    return *p;
  else
    return widestring();
}

widestring Properties::GetLowerTrimString(pwide_t key) const
{
  widestring ws = GetString(key);
  ws.Trim().ToLower();
  return ws;
}

void GenericProperties::SetString(pwide_t key, pwide_t val)
{
  map_.Rec(key) = val;
}

u32_t GenericProperties::GetLong(pwide_t key) const
{
  if ( widestring *p = map_.Get(key) )
    return p->ToLong();
  else
    return 0;
}

bool Properties::GetBool(pwide_t key) const
{
  if ( widestring s = GetLowerTrimString(key) )
    return s==L"1" || s==L"yes" || s==L"true" || s==L"on" ;
  else
    return false;
}

void GenericProperties::SetLong(pwide_t key,u32_t val)
{
  map_.Rec(key).Set(teggo::Utow(val));
}

bool GenericProperties::HasProperty(pwide_t key) const
{
  return map_.Get(key) != 0;
}

void GenericProperties::RemoveProperty(pwide_t key)
{
  map_.Erase(key);
}

void GenericProperties::RemoveAllProperties()
{
  map_.Clear();
}

bool GenericProperties::GetBinary(pwide_t key,BufferT<byte_t>& val) const
{
  BufferT<byte_t>().swap(val);
  return false;
}

void GenericProperties::SetBinary(pwide_t key,const BufferT<byte_t>& val)
{
}

GenericProperties::GenericProperties()
{
}

GenericProperties::~GenericProperties()
{
}

widestring  GenericProperties::ToString_() const
{
  widestring text = L"{PROPERTIES}";
//  CONTAINER::const_iterator i  = map_.begin();
//  CONTAINER::const_iterator iE = map_.end();
//  for ( ; i != iE;  ) {
//    text += (*i).first.c_str();
//    text += "=";
//    text += (*i).second;
//    ++i;
//    if ( i != iE ) text += ",";
//  }
//  text += "}";
  return text;
}

PropIterator* GenericProperties::CreateIterator() const 
{
  auto_ref<GenericPropertiesIter> iter ( new GenericPropertiesIter );
  CONTAINER::Iterator i = map_.Iterate();
  iter->keys_.reserve(map_.Count());
  while ( i.Next() ) iter->keys_.push_back(i.Key());
  iter->keysI_ = iter->keys_.begin();
  return Forget(iter);
}

SCERROR LoadPropertiesFrom(InputStream* source,Properties* props)
{
  fail_if ( !source );
  fail_if ( !props );
  BufferT<char> line;
  while ( !source->EndOfStream() ) {
    fail_if_err ( source->ReadLine(line) );
    char *dilim = teggo::linear_find(line.Begin(),line.End(),'=');
    if ( dilim != line.End() ) {
      teggo::replace(line.Begin(),line.End(),'\t',' ');
      BaseStringT<wchar_t> const &key = widestring(line.Begin(),teggo::remove(line.End(),dilim,' ')).ToLower();      
      if ( key.Length() == 0 || key[0] == '#' ) continue;
      widestring val = widestring(dilim+1,line.End());
      props->SetString(key.Str(),val.Str());
    }
  }
  return SCE_OK;
}

SCERROR LoadPropertiesFrom(const BufferT<byte_t>& source,Properties* props)
{
  MemInputStream mis( source );
  return LoadPropertiesFrom(&mis,props);
}

Properties* CreatePropertiesFrom(const BufferT<byte_t>& source)
{
  MemInputStream mis( source );
  return CreatePropertiesFrom((InputStream*)&mis);
}

Properties* CreatePropertiesFrom(InputStream* source)
{
  ret0_if ( !source );
  AutoProperties props(new GenericProperties);
  ret0_if_err( LoadPropertiesFrom(source,props) );
  return Forget(props);
}

Properties* Properties::Create()
{
  return new GenericProperties();
}

NAMESPACE_MSKIN_END

