
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
#include "m_igzstream.h"
#include "../mCORE/m_autoref.h"
#include "../mCORE/m_singletons.h"

#include <stdio.h>
#include <stdlib.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
# pragma warning(disable:4530)  //C++ exception handler used, but unwind semantics are not enabled.
# pragma warning(disable:4786)  //identifier was truncated to '255' characters in the debug information
#endif

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$mSTREAMS")
#endif

NAMESPACE_MSKIN_BEGIN

//extern string WideToString(pwide_t);

static pwide_t const INASTANCE_IS_NOT_REGISTERED = L"rscmanager: instance is not registered";

struct RSCMAN_Inst : public IRefObject , private refbase {
  typedef CollectionT<AutoProperties> SSTMAP;
  widestring sstname_;
  SSTMAP sstmap_;
  virtual void Release_ () const { refbase::Release_(); }
  virtual void Grab_() const { refbase::Grab_(); }
  RSCMAN_Inst() {
    REQUIRE ( RSCMAN::instance_ == 0 );
    RSCMAN::instance_ = this;
  }
  virtual ~RSCMAN_Inst() {
    REQUIRE ( RSCMAN::instance_ == this );
    RSCMAN::SetImpl(0);
    RSCMAN::instance_ = 0;
  }
  pwide_t GetFullNameSST(BaseStringT<wchar_t>&);
  AutoProperties GetRecordSST(pwide_t sst_record);
  SCERROR ReloadSST(pwide_t sst_resource);
  widestring GetFullNameSST() { return sstname_; }
};

//pwide_t RSCMAN_Inst::GetFullNameSST(BaseStringT<wchar_t>& ws) { return ws.Set(sstname_.c_str()); }
//
//AutoProperties RSCMAN_Inst::GetRecordSST(pwide_t sst_record)
//{
//  string sst_record_S = WideToString(sst_record);
//  transform(sst_record_S.begin(),sst_record_S.end(),sst_record_S.begin(),tolower);
//  SSTMAP::iterator i = sstmap_.find(sst_record_S.c_str());
//  if ( i == sstmap_.end() ) 
//    retval_because( WideFormat(L":RSCMAN: SST has no record '%s'",sst_record),AutoProperties(0));
//  else 
//    return (*i).second;
//}
//
//extern SCERROR RSCMAN_Inst_LoadSST(DataSource* ds,map<string_key,AutoProperties>&);
//
//SCERROR RSCMAN_Inst::ReloadSST(pwide_t sst_resource)
//{
//  SSTMAP().swap(sstmap_);
//  AutoDataSource ds ( RSCMAN::CreateSource(sst_resource) );
//  if ( SCE_OK != RSCMAN_Inst_LoadSST(ds,sstmap_) ) {
//    SSTMAP().swap(sstmap_);
//    return SCE_FAIL;
//  } 
//  return SCE_OK;
//}

pchar_t const RscManager::TypeID__ = "#TYPEID#mskin::RscManager";
void* RscManager::Adaptate_(const void* id) 
{
  if ( id == TypeID__ ) return this;
  return IRefObject::Adaptate_(id);
}

widestring GenericRscManager::ToString_() const { return widestring(L"generic resource manager"); }
void GenericRscManager::Release_() const { refbase::Release_(); }
void GenericRscManager::Grab_() const { refbase::Grab_(); }

StringT<wchar_t> GenericRscManager::MakePath( pwide_t name )
{
  StringT<wchar_t> path; 
  if ( !(name && teggo::StrLen(name) > 2 && name[1] == wchar_t(':') ) )
    path = rscpath_;
  return path+name;
}

DataSource* GenericRscManager::CreateDataSource(pwide_t name)
{
  DataSource* persistent = FindStatic(name);
  if ( persistent ) return persistent;
  else
  {
#if MSKIN_HAS_FILESYSTEM
    widestring path; 
    if ( name && wcslen(name) > 2 && name[1] == ':' )
      path = name;
    else
      path = rscpath_+name;
    if ( IsFileReadable(+path) )
      return new StreamedDataSource(new FileInputStream(+path),name);
#endif
    return 0;
  }
}

DataSource* GenericRscManager::CreateDataSourceGZ(pwide_t name)
{
  if ( InputStreamEx* persistent = FindStatic_(name) ) {
    return new StreamedDataSource(new GZipInputStream(persistent),name);
  }else{
#if MSKIN_HAS_FILESYSTEM
    widestring path; 
    if ( name && wcslen(name) > 2 && name[1] == ':' )
      path = name;
    else
      path = rscpath_+name;
    if ( IsFileReadable(+path) )
      return new StreamedDataSource(new GZipInputStream(new FileInputStream(+path)),name);
#endif
    return 0;
  }
}

void GenericRscManager::SetSubfolder(pwide_t subfolder)
{
  if ( wcslen(subfolder) > 2 && subfolder[1] == ':' )
  {
    rscpath_ = subfolder;
  }
  else if ( wcslen(subfolder) >= 5 && memcmp(subfolder,L"%CWD%",5*2) == 0 )
  {
    BufferT<char> temp(MAX_PATH+1,0);
    GetCurrentDirectory(MAX_PATH,&temp[0]);
    rscpath_ = &temp[0];
    rscpath_ += subfolder+5;
  }
  else
  {
    if ( modulepath_.Length() != 0 ) {
      //(( rscpath_ = modulepath_ ) += "/") += subfolder;
      rscpath_ = modulepath_;
      rscpath_ += "/";
      rscpath_ += subfolder;
    }else
      rscpath_ = subfolder;
  }
  if ( rscpath_.Length() != 0 && wcslen(subfolder) != 0 ) rscpath_ += L"/";
  rscpath_.Replace(L"\\",L'/');
  Lout << _S*"rscmanager: setting resources path to '%?'" %rscpath_;
}

GenericRscManager::GenericRscManager()
{
#if MSKIN_HAS_FILESYSTEM
  modulepath_ = Hinstance->MetaModuleDir();
#else
  modulepath_ = L"";
#endif
  SetSubfolder(L"");
}

GenericRscManager::~GenericRscManager() {}

InputStreamEx* GenericRscManager::FindStatic_(pwide_t name)
{
  if ( ResourceDef *def = resources_.Get(name) )
  {
    Xdbg( Ldbg << _S*"rscmanager: found datasource '%?' [%p..%p]"
                  %resources_.KeyOf(def) 
                  %def->mem 
                  %(def->mem+def->size) );
    return new MemInputStream(def->mem,def->mem+def->size);
  }
  else 
    return 0;
}

DataSource* GenericRscManager::FindStatic(pwide_t name) 
{
  if ( InputStreamEx* isx = FindStatic_(name) ) 
    return new StreamedDataSource(isx,name);
  else
    return 0;
}

bool GenericRscManager::IsRscExists(pwide_t name) 
{
  if ( ResourceDef *r = resources_.Get(name) )
    return true;
  else
    return IsFileReadable(+(rscpath_+name));
}

void GenericRscManager::ManageStatic(pwide_t name,const byte_t* mem,unsigned size)
{
  Xdbg( Ldbg << _S*"rscmanager: manage datasource '%?' [%p..%p]" %name %mem %(mem+size) );
  ResourceDef r = { mem, size };
  resources_.Put( name, r );
}

GenericRscManager* GenericRscManager::Create() 
{
  return new GenericRscManager();
}

RscManager* RSCMAN::impl_ = 0;
DataSource* RSCMAN::CreateSource(pwide_t name) 
{
  if ( RscManager* manager = RSCMAN::impl_ ) 
  {
    DataSource* ds = manager->CreateDataSource(name);
    if ( !ds ) ds = manager->CreateDataSourceGZ(+(widestring(name)+=L".gz"));
    if ( !ds )
      Lerr << _S*"rscmanager: datasource '%?' is not found" %name;
    return ds;
  }
  else ret0_because ( INASTANCE_IS_NOT_REGISTERED );
}

DataSource* RSCMAN::CreateSourceGZ(pwide_t name) 
{
  if ( RscManager* manager = RSCMAN::impl_ ) 
    return manager->CreateDataSourceGZ(name);
  else ret0_because ( INASTANCE_IS_NOT_REGISTERED );
}

widestring RSCMAN::MakePath(pwide_t name) 
{
  if ( RscManager* manager = RSCMAN::impl_ ) 
    return manager->MakePath(name);
  else 
    retval_because ( INASTANCE_IS_NOT_REGISTERED, L"" );
}

bool RSCMAN::IsRscExists(pwide_t name) 
{
  if ( RscManager* manager = RSCMAN::impl_ ) {
    if ( manager->IsRscExists(name) ) return true;
    if ( manager->IsRscExists(+(widestring(name)+=L".gz")) ) return true;
    return false;
  }
  else 
    ret0_because ( INASTANCE_IS_NOT_REGISTERED );
}

void RSCMAN::ManageStatic(pwide_t name,const byte_t* mem,unsigned size) 
{
  if ( RscManager* manager = RSCMAN::impl_ ) 
    manager->ManageStatic(name,mem,size);
  else return_because ( INASTANCE_IS_NOT_REGISTERED );
}

void RSCMAN::SetSubfolder(pwide_t subfolder) 
{
  if ( RscManager* manager = RSCMAN::impl_ ) 
    manager->SetSubfolder(subfolder);
  else return_because ( INASTANCE_IS_NOT_REGISTERED );
}

//string RSCMAN::GetFullNameSST() 
//{
//  if ( RSCMAN::instance_ ) return instance_->GetFullNameSST();
//  else retval_because ( INASTANCE_IS_NOT_REGISTERED,"");
//}
//
//pwide_t RSCMAN::GetFullNameSST(BaseStringT<wchar_t>& ws) 
//{
//  return ws.Set(GetFullNameSST().c_str());
//}
//
//AutoProperties RSCMAN::GetRecordSST(pwide_t sst_record)
//{
//  if ( RSCMAN::instance_ ) return instance_->GetRecordSST(sst_record);
//  else retval_because ( INASTANCE_IS_NOT_REGISTERED,AutoProperties(0));
//}
//
//SCERROR RSCMAN::ReloadSST(pwide_t sst_resource)
//{
//  if ( RSCMAN::instance_ ) return instance_->ReloadSST(sst_resource);
//  else fail_because ( INASTANCE_IS_NOT_REGISTERED );
//}

RSCMAN_Inst* RSCMAN::instance_ = 0;

extern "C" const byte_t BGZ_arial14a[];
extern "C" const byte_t TXT_arial14a[];
extern "C" const unsigned long BGZ_arial14a_size;
extern "C" const unsigned long TXT_arial14a_size;

void RSCMAN::SetImpl(RscManager* manager) 
{ 
  Release(impl_);
  impl_ = manager;
  if ( !!impl_ ) 
  {
    impl_->ManageStatic(L"@arial14a.bmp.gz",BGZ_arial14a,BGZ_arial14a_size);
    impl_->ManageStatic(L"@arial14a.txt",TXT_arial14a,TXT_arial14a_size);
  }
}

IRefObject* RSCMAN::Acquire(RscManager* mgr) 
{
  if ( !instance_ ) new RSCMAN_Inst;
  else Grab(instance_);
  SetImpl(!mgr?GenericRscManager::Create():mgr);
  return instance_;
}
NAMESPACE_MSKIN_END
