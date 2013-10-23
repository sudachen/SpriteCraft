
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
#include "m_dsource.h"

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

NAMESPACE_MSKIN_BEGIN

StreamedDataSource::StreamedDataSource(InputStreamEx* is,pwide_t name) 
  : aheadBytes_(0),is_(is),name_(name) {}
StreamedDataSource::StreamedDataSource(InputStreamEx* is,pchar_t name) 
  : aheadBytes_(0),is_(is),name_(name) {}
StreamedDataSource::StreamedDataSource(DataSource* ds) 
  : aheadBytes_(0),is_(ds),name_(ds->GetName()) {}

StreamedDataSource::~StreamedDataSource() {}

widestring StreamedDataSource::ToString_() const 
  { 
    return widestring(_S*L"strimmed datasource '%?'" %name_); 
  }

static const wchar_t DS_READ_ERROR[] = L":DataSource: '%?' read error";

u32_t StreamedDataSource::PeekLong() const 
{
  if ( aheadBytes_ < 4 ) {
    if ( (aheadBytes_ += is_->Read(ahead_+aheadBytes_,4-aheadBytes_)) != 4 ) {
      Lout <<  _S*DS_READ_ERROR %name_;
      return 0;
    }
  }
  return ReadLong_(ahead_); 
}

float StreamedDataSource::PeekFloat() const 
{
  if ( aheadBytes_ < 4 ) {
    if ( (aheadBytes_ += is_->Read(ahead_+aheadBytes_,4-aheadBytes_)) != 4 ) {
      Lout <<  _S*DS_READ_ERROR %name_;
      return 0;
    }
  }
  return ReadFloat_(ahead_); 
}

u16_t StreamedDataSource::PeekShort() const 
{
  if ( aheadBytes_ < 2 ) {
    if ( (aheadBytes_ += is_->Read(ahead_+aheadBytes_,2-aheadBytes_)) != 2 ) {
      Lout << _S*DS_READ_ERROR %name_;
      return 0;
    }
  }
  return ReadShort_(ahead_); 
}

byte_t StreamedDataSource::PeekByte() const 
{
  if ( aheadBytes_ == 0 ) {
    if ( (aheadBytes_ = is_->Read(ahead_,1)) != 1 ) {
      Lout << _S*DS_READ_ERROR %name_;
    }
  }
  return ahead_[0];
}

unsigned StreamedDataSource::Read(byte_t* dest,unsigned count)
{
  unsigned processed = 0;
  if ( aheadBytes_ ) {
    unsigned u = teggo_min(aheadBytes_,count);
    memcpy(dest,ahead_,u);
    processed = u;
    if ( u < aheadBytes_ ) memmove(ahead_,ahead_+u,aheadBytes_-u);
    aheadBytes_ -= u;
  }
  if ( count-processed ) {
    processed+=is_->Read(dest+processed,count-processed);
  }
  return processed;
}

unsigned StreamedDataSource::Skip(unsigned count) 
{
  unsigned processed = 0;
  if ( aheadBytes_ ) {
    processed = teggo_min( aheadBytes_, count );
    if ( processed < aheadBytes_ ) memmove(ahead_,ahead_+processed,aheadBytes_-processed);
    aheadBytes_-=processed;
    count -= processed;
  }
  if ( count ) processed += is_->Skip(count);
  return processed;
}

pwide_t  StreamedDataSource::GetName() const
{ return name_.Str(); }

//string StreamedDataSource::GetAsciiName() const
//{ return string( StringT<char,64>(name_.Str()) ); }

bool StreamedDataSource::Reset() 
{ aheadBytes_ = 0;  return is_->Reset(); }

unsigned StreamedDataSource::WasRead() const 
{ return is_->WasRead()-aheadBytes_; }

bool StreamedDataSource::EndOfStream() const
{ return aheadBytes_ == 0 && is_->EndOfStream(); }

SCERROR StreamedDataSource::PeekError() const 
{ 
  SCERROR e = is_->PeekError(); 
  return e!=SCE_OK ? e : EndOfStream() ? SCE_EOF : SCE_OK;
}

void StreamedDataSource::Release_() const { refbase::Release_(); }
void StreamedDataSource::Grab_() const { refbase::Grab_(); }

pchar_t const DataSource::TypeID__ = "#TYPEID#mskin::DataSource";

void* DataSource::Adaptate_(const void* id) {
  if ( id == TypeID__ ) return this;
  return InputStreamEx::Adaptate_(id);
}


//{ SCERROR ecode = SCE_OK; std::swap(ecode,ecode_); return ecode; }

NAMESPACE_MSKIN_END

