
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

#include "StdAfx.h"
#include "dsscfilter.h"

DataSourceScraftFilter_ *DataSourceScraftFilter_::instance_ = 0;

void DataSourceScraftFilter_::SetDefaultSearchPath(StringParam path)
  {
    if ( PATH_LIST_ *l = volumes_.Get(L"") )
      l->Clear();
    Mount(L"",path);
  }

void DataSourceScraftFilter_::Mount(StringParam vol,StringParam path)
  {
    SplitterT<wchar_t> splitter = SplitterT<wchar_t>(+path,L';');
    PATH_LIST_ plist;

    while ( splitter.Next() )
      if ( pwide_t p = splitter.Get() )
        {
          if ( !*p ) continue;
          StringW *fullpath = plist.Append();
          Prepare_(p,fullpath);
        }

    PATH_LIST_ &vol_plist = volumes_.Rec(+vol);
    for ( PATH_LIST_::Position p = plist.Begin(); p; ++p )
      vol_plist.Append(*p);
  }

void DataSourceScraftFilter_::Init()
  {
    teggo::DataSource->RegisterSourceFilter(
      rcc_ptr<DataSourceFilter>(new DataSourceScraftFilter_()));
  }

DataSourceScraftFilter_::DataSourceScraftFilter_()
  {
    STRICT_REQUIRE( !instance_  );
    instance_ = this;
  }

DataSourceScraftFilter_::~DataSourceScraftFilter_()
  {
    STRICT_REQUIRE(instance_);
    instance_ = 0;
  }

namespace teggo
  {

    template < class tTc >
      int BaseStringT<tTc>::Find( tTc const *s, int from ) const
        {
          int x = from;
          int l = Length();
          int l1 = StrLen(s);
          while ( x+l1 <= l )
            {
              if ( !memcmp(s,s_+x,l1*sizeof(tTc)) )
                return x;
              ++x;
            }
          return -1;            
        }

    template < class tTc >
      void BaseStringT<tTc>::RemoveReps( tTc pat )
        {
          tTc *s1 = s_+1;
          while ( *s1 && ( *s1 != pat || s1[1] != *s1 ) ) ++s1;
          if ( *s1 )
            {
              tTc *s = s1; 
              ++s1;
            l_s1:
              if ( *s1 )
                {
                  if ( *s1 == pat && *s == *s1 ) 
                    { ++s1; goto l_s1; }
                  ++s;
                  STRICT_REQUIRE(s-s_ < Length());
                  *s = *s1++;
                  goto l_s1;
                }
              Resize(s-s_+1);
            }
        }

    void NormalizePath(BaseStringT<wchar_t> &path)
      {
        path.Replace(L"\\",'/');
        path.RemoveReps('/');

        int p = 0;
        while ( (p = path.Find(L"/./",p)) >= 0 )
          path.Remove(p+1,2);

        p = 0;
        while ( (p = path.Find(L"/..",p)) >= 0 )
          {
            if ( int k = path.FindRChar('/',p-1) )
              {
                path.Remove(k+1,p+2-k);
                p = k;
              }
            else
              break;
          }
      }
  };

bool DataSourceScraftFilter_::Prepare_(pwide_t id, teggo::BaseStringT<wchar_t> *fullpath)
  {
    typedef BaseStringT<wchar_t> bst;
    int l = teggo::StrLen(id);
    if ( l > 0 )
      if ( id[0] == ':' )
        {
          if ( l > 3 && id[2] == ':' )
            return fullpath->Set(id), true;
          else
            {
              fullpath->Append(':');
              fullpath->Append(Hinstance->MetaModuleDir());
              fullpath->Append('/');
              fullpath->Append(id+1);
              teggo::NormalizePath(*fullpath);
              return true;
            }
        }

    fullpath->Set(id);
    return false;
  }

bool DataSourceScraftFilter_::Resolve_(pwide_t id, teggo::BaseStringT<wchar_t> *fullpath, pwide_t vol)
  {
    if ( !vol && *id == ':' )
      {
        Prepare_(id,fullpath);
        if ( teggo::FileExists(fullpath->Str()) )
          return true;
      }
    else
      {
        if ( vol )
          {
            if ( PATH_LIST_ *plist = volumes_.Get(vol) )
              {
                for ( PATH_LIST_::Position p = plist->Begin(); p ; ++p )
                  {
                    if ( *p->Begin() == ':' )
                      {
                        fullpath->Set(p->Str()+1);
                        fullpath->Append('/');
                        fullpath->Append(id);
                        teggo::NormalizePath(*fullpath);
                        if ( teggo::FileExists(*fullpath) );
                          return true;
                      }
                    else
                      {
                        if ( Resolve_(id,fullpath,+*p) )
                          return true;
                      }
                  }
              }
          }
        else
          {
            pwide_t pfxS = id;
            while ( *id && *id != ':' ) ++id;
            if ( *id == ':' )
              {
                StringW pfx(pfxS,id);
                return Resolve_(id+1,fullpath,+pfx);
              }
            else
              return Resolve_(pfxS,fullpath,L"");
          }
      }

    return false;
  }

rcc_ptr<DataStream> DataSourceScraftFilter_::Open( pwide_t identifier, unsigned flgs )
  {
    StringW fullpath;
    if ( Resolve_(identifier,&fullpath,0) )
      return teggo::OpenFile(fullpath,flgs);
    else
      return rcc_ptr<DataStream>(0);
  }

int DataSourceScraftFilter_::Exists( pwide_t identifier )
  {
    StringW fullpath;
    if ( Resolve_(identifier,&fullpath,0) )
      return teggo::FileExists(fullpath);
    else
      return 0;
  }

u32_t DataSourceScraftFilter_::LastMtimeOf( pwide_t identifier )
  {
    StringW fullpath;
    if ( Resolve_(identifier,&fullpath,0) )
      return teggo::LastMtimeOfFile(fullpath);
    else
      return 0;
  }
