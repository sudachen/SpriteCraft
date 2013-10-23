
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

#if !defined ___0bfc9217_d962_4796_af8e_5e66d37bc11e___
#define ___0bfc9217_d962_4796_af8e_5e66d37bc11e___

#include "com_register.h"

#include "sysutil.h"

#if defined _TEGGO_COMREGISTER_HERE
# define _TEGGO_COMREGISTER_FAKE_INLINE _TEGGO_EXPORTABLE
#else
# define _TEGGO_COMREGISTER_FAKE_INLINE CXX_FAKE_INLINE
#endif

namespace teggo {

  _TEGGO_COMREGISTER_FAKE_INLINE
  pchar_t CXX_STDCALL GUIDToRegStringA(const GUID &guid,mutable_t<_GUID_char48_t> const &val)
    {
      SprintF(val.value,
          48,
          "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
          guid.Data1,guid.Data2,guid.Data3,
          guid.Data4[0],guid.Data4[1],guid.Data4[2],guid.Data4[3],
          guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
      return val.value;
    }

  _TEGGO_COMREGISTER_FAKE_INLINE
  pwide_t CXX_STDCALL GUIDToRegStringW(const GUID &guid,mutable_t<_GUID_wchar48_t> const &val)
    {
      SprintF(val.value,
          48,
          L"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
          guid.Data1,guid.Data2,guid.Data3,
          guid.Data4[0],guid.Data4[1],guid.Data4[2],guid.Data4[3],
          guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
      return val.value;
    }

  _TEGGO_COMREGISTER_FAKE_INLINE
  const char * CXX_STDCALL GUIDToRegString(const GUID &guid,BaseStringT<char> &s)
    {
      return s.Set(GUIDToRegStringA(guid));
    }

  _TEGGO_COMREGISTER_FAKE_INLINE
  const wchar_t * CXX_STDCALL GUIDToRegString(const GUID &guid,BaseStringT<wchar_t> &s)
    {
      return s.Set(GUIDToRegStringW(guid));
    }

  _TEGGO_COMREGISTER_FAKE_INLINE
  HRESULT CXX_STDCALL ComRegWriteStr(pchar_t root,pchar_t key,pchar_t subkey,pchar_t value)
    {
      HKEY hKey = 0;
      StringT<char,64> xkey;

      if (key)
        xkey.Set(_S*"%s\\%s" %root %key);
      else
        xkey = root;

      LONG hr = RegCreateKeyExA(
        HKEY_CLASSES_ROOT,
        +xkey,
        0,REG_OPTION_NON_VOLATILE,
        0,KEY_ALL_ACCESS,0, &hKey, 0 );

      if ( SUCCEEDED(hr) )
        {
          hr = RegSetValueExA(hKey,subkey,0,REG_SZ,(BYTE*)value,strlen(value)+1);
          if ( FAILED(hr) )
            {
              widestring syserr = SysFormatError();
              WARNING(
                (_S*L"RegSetValueEx: Could not set key value %?\\%?\nreason: %?"
                  %xkey %subkey %syserr).Str() );
            }
          RegCloseKey(hKey);
        }
      else
        {
          widestring syserr = SysFormatError();
          WARNING(
            (_S*L"Could not open/create key %?\nwith KEY_ALL_ACCESS permissions\nreason: %?"
                %xkey %syserr).Str() );
        }

      return hr;
    }

  _TEGGO_COMREGISTER_FAKE_INLINE
  HRESULT CXX_STDCALL ComRegDeleteKey(pchar_t key)
    {
      HKEY hKey = 0;

      if ( ERROR_SUCCESS == RegOpenKeyExA(HKEY_CLASSES_ROOT,key,0,KEY_ALL_ACCESS,&hKey ) )
        {
          FILETIME ftime;
          BufferT<char> buff(MAX_PATH);
          for ( DWORD sz = MAX_PATH ;
            ERROR_SUCCESS == RegEnumKeyExA(hKey,0,&buff[0],&sz,0,0,0,&ftime);
            sz = MAX_PATH )
            {
              StringT<char> temp;
              temp.Set(_S*"%s\\%s" %key %&buff[0]);
              if ( S_OK != ComRegDeleteKey(temp.Str()) )
                {
                  RegCloseKey(hKey);
                  return E_FAIL;
                }
            }

          RegCloseKey(hKey);

          if ( ERROR_SUCCESS != RegDeleteKeyA(HKEY_CLASSES_ROOT,key) )
            {
              widestring syserr = SysFormatError();
              WARNING(
                (_S*L"Could not dlete key %?\nreason: %?"
                  %key %syserr).Str() );
              return E_FAIL;
            }
        }
      else if ( u32_t lerror = GetLastError() )
        {
          widestring syserr = SysFormatError(lerror);
          WARNING(
            (_S*L"Could not open key %?\nwith KEY_ALL_ACCESS permissions\nreason: %?"
              %key %syserr).Str() );
          return E_FAIL;
        }
      return S_OK;
    }


  _TEGGO_COMREGISTER_FAKE_INLINE
  HRESULT CXX_STDCALL ComRegLoadTypeLib(ITypeLib** pI,pwide_t p,HinstanceType &hinstance)
    {
      if ( !p )
        return ::LoadTypeLib(hinstance->GetModulePath(),pI);
      else
        return ::LoadTypeLib(p,pI);
    }


  _TEGGO_COMREGISTER_FAKE_INLINE
  HRESULT CXX_STDCALL ComRegRegisterTypeLib(HinstanceType &hinstance)
    {
      HRESULT hr = S_OK;
      rcc_ptr<ITypeLib> typelib;

      if ( SUCCEEDED((hr = ComRegLoadTypeLib(&+typelib,0,hinstance))) )
        {
          hr = ::RegisterTypeLib(
                    +typelib,
                    (BSTR)hinstance->GetModulePath(),
                    (BSTR)hinstance->GetModuleDir());
        }

      return hr;
    }

  _TEGGO_COMREGISTER_FAKE_INLINE
  HRESULT CXX_STDCALL ComRegUnregisterTypeLib(HinstanceType &hinstance)
    {
      HRESULT hr = S_OK;
      rcc_ptr<ITypeLib> typelib;

      if ( SUCCEEDED((hr = ComRegLoadTypeLib(&+typelib,0,hinstance))) )
        {
          TLIBATTR* tla;
          if ( SUCCEEDED((hr = typelib->GetLibAttr(&tla))) )
            {
              hr = ::UnRegisterTypeLib(
                        tla->guid,
                        tla->wMajorVerNum, tla->wMinorVerNum,
                        tla->lcid,
                        tla->syskind);
              typelib->ReleaseTLibAttr(tla);
            }
        }

      return hr;
    }

  _TEGGO_COMREGISTER_FAKE_INLINE
  HRESULT CXX_STDCALL ComRegRegisterCoClass(const GUID& clsid,pchar_t progID,pchar_t desc,long ver,HinstanceType &hinstance)
    {
      StringT<char,46> clsid_s;
      StringT<char,8+46> keyroot;

      HRESULT hr = S_OK;

      teggo::GUIDToRegString(clsid,clsid_s);
      keyroot.Set(_S*"CLSID\\%s" %clsid_s);

      if ( desc )
        hr = ComRegIfSok(hr,ComRegWriteStr(+keyroot,0,0,desc));

      hr = ComRegIfSok(hr,
              ComRegWriteStr(+keyroot,"InprocServer32",0,
                +asciistring(hinstance->GetModulePath()) ));

      hr = ComRegIfSok(hr,
              ComRegWriteStr(+keyroot,"InprocServer32","ThreadingModel","both"));

      if ( progID )
        {
          StringT<char,128> progidV (_S*"%s.%d" %progID %ver);

          hr = ComRegIfSok(hr,ComRegWriteStr(+keyroot,"VersionIndependentProgID",0,progID));
          hr = ComRegIfSok(hr,ComRegWriteStr(+keyroot,"ProgID",0,+progidV));

          if ( desc )
            {
              hr = ComRegIfSok(hr,ComRegWriteStr(progID,0,0, desc));
              hr = ComRegIfSok(hr,ComRegWriteStr(progidV.Str(),0,0, desc));
            }

          hr = ComRegIfSok(hr,ComRegWriteStr(progID,"CLSID",0, +clsid_s));
          hr = ComRegIfSok(hr,ComRegWriteStr(progID,"CurVer",0, +progidV));
          hr = ComRegIfSok(hr,ComRegWriteStr(+progidV,"CLSID",0, +clsid_s));
        }

      return hr;
    }


  _TEGGO_COMREGISTER_FAKE_INLINE
  HRESULT CXX_STDCALL ComRegUnregisterCoClass(const GUID& clsid,pchar_t progID,long ver)
    {
      StringT<char,46> clsid_s;
      StringT<char,8+46> keyroot;
      HRESULT hr = S_OK;

      teggo::GUIDToRegString(clsid,clsid_s);
      keyroot.Set(_S*"CLSID\\%s" %clsid_s);

      hr = ComRegIfSok(hr,ComRegDeleteKey(+keyroot));

      if ( progID )
        {
          StringT<char,128> progidV(_S*"%s.%d" %progID %ver);
          hr = ComRegIfSok(hr,ComRegDeleteKey(+progidV));
          hr = ComRegIfSok(hr,ComRegDeleteKey(progID));
        }

      return hr;
    }

} // namespace

#endif // ___0bfc9217_d962_4796_af8e_5e66d37bc11e___
