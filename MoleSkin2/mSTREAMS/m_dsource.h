
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


#if !defined (__m_dsource_h__)
#define __m_dsource_h__

#include "m_xxconf.h"
#include "m_istream.h"
#include "../mcore/m_singletons.h"
#include "../mUTIL/m_properties.h"


NAMESPACE_MSKIN_BEGIN

class DataSource;

class DataSource : public InputStreamEx {
public:
  static pchar_t const TypeID__;
  virtual void*    Adaptate_(const void*);
  virtual u32_t  PeekLong() const = 0;
  virtual u16_t PeekShort() const = 0;
  virtual byte_t   PeekByte() const = 0;
  virtual float    PeekFloat() const = 0;
  virtual pwide_t  GetName() const = 0;
//  virtual string   GetAsciiName() const = 0;
protected:
  DataSource() {}
  virtual ~DataSource() {}
private:
  DataSource(const DataSource&);
  DataSource& operator=(const DataSource&);
};
typedef auto_ref<DataSource> AutoDataSource;

class StreamedDataSource : public DataSource, private refbase {
  auto_ref<InputStreamEx> is_;
  mutable byte_t    ahead_[4];
  mutable unsigned  aheadBytes_;
  widestring        name_;
  bool              badFlag_;
public:
  virtual widestring  ToString_() const;
  virtual void        Grab_() const;
  virtual void        Release_() const;
  virtual u32_t     PeekLong() const;
  virtual float       PeekFloat() const;
  virtual u16_t    PeekShort() const;
  virtual byte_t      PeekByte() const;
  virtual unsigned    Read(byte_t* dest,unsigned count);
  virtual unsigned    Skip(unsigned);
  virtual bool        Reset();
  virtual pwide_t     GetName() const;
  virtual bool        EndOfStream() const;
  virtual unsigned    WasRead() const;
  virtual SCERROR PeekError() const; 
  StreamedDataSource(InputStreamEx*,pwide_t);
  StreamedDataSource(InputStreamEx*,pchar_t);
  StreamedDataSource(DataSource*);
  virtual ~StreamedDataSource();
private:
  StreamedDataSource(const StreamedDataSource&);
  StreamedDataSource& operator=(const StreamedDataSource&);
};

class RscManager;

class RscManager : public IRefObject{
public:
  static pchar_t const TypeID__;
  virtual void* Adaptate_(const void*);
  virtual DataSource* CreateDataSource(pwide_t name) = 0;
  virtual DataSource* CreateDataSourceGZ(pwide_t name) = 0;
  virtual void ManageStatic(pwide_t name,const byte_t* mem,unsigned size) = 0;
  virtual void SetSubfolder(pwide_t path) = 0;
  virtual bool IsRscExists(pwide_t name) = 0;
  virtual widestring MakePath( pwide_t p ) = 0;
protected:
  RscManager() {}
  virtual ~RscManager() {}
private:
  RscManager(const RscManager&);
  RscManager& operator=(const RscManager&);
};

struct RSCMAN_Inst;
struct RSCMAN {
  static void SetImpl(RscManager* rm);
  static IRefObject* Acquire(RscManager* = 0);
//  static string GetFullNameSST();
//  static pwide_t GetFullNameSST(WideStringBasic&);
//  static AutoProperties GetRecordSST(pwide_t sst_record);
//  static SCERROR ReloadSST(pwide_t sst_resource);

  static DataSource* CreateSource(pwide_t name);
  static DataSource* CreateSourceGZ(pwide_t name);
  static void ManageStatic(pwide_t name,const byte_t* mem,unsigned size);
  static void SetSubfolder(pwide_t subfolder);
  static bool IsRscExists(pwide_t name);
  static widestring MakePath( pwide_t p );
  /* depricated */
//  static AutoProperties GetRecordSST(pchar_t sst_record) 
//  {
//    return GetRecordSST(widestring( sst_record ));
//  }

private:
  friend struct RSCMAN_Inst;
  static RscManager* impl_;
  static RscManager* GetImpl() { return impl_; }
  static RSCMAN_Inst*  instance_;
  RSCMAN();
  ~RSCMAN();
  RSCMAN(const RSCMAN&);
  RSCMAN& operator=(RSCMAN&);
};

class GenericRscManager : public RscManager, private refbase
{
  struct ResourceDef { const byte_t* mem; unsigned size; };
  typedef CollectionT<ResourceDef> CONTAINER;

  widestring rscpath_;
  widestring modulepath_;
  CONTAINER resources_;
  DataSource* FindStatic(pwide_t name);
  InputStreamEx* FindStatic_(pwide_t name);

public:
  virtual widestring ToString_() const;
  virtual void Release_() const;
  virtual void Grab_() const;
  DataSource* CreateDataSource(pwide_t name);
  DataSource* CreateDataSourceGZ(pwide_t name);
  void ManageStatic(pwide_t name,const byte_t* mem,unsigned size);
  void SetSubfolder(pwide_t path);
  virtual bool IsRscExists(pwide_t name);
  widestring MakePath( pwide_t p );
  static GenericRscManager* Create();
protected:
  GenericRscManager();
  virtual ~GenericRscManager();
};

SCERROR MOLE_FUNCALL WriteSlot(int no,const BufferT<byte_t>&);
SCERROR MOLE_FUNCALL ReadSlot(int no,BufferT<byte_t>&);
int MOLE_FUNCALL GetNumberOfSlots();

NAMESPACE_MSKIN_END

#endif //__m_dsource_h__
