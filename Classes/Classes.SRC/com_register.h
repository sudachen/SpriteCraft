
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

#if !defined __506DA0C7_943D_4E7C_A382_00B907BB5CD8__
#define __506DA0C7_943D_4E7C_A382_00B907BB5CD8__

#if !defined CXX_NO_MSCOM_SUPPORT

#include "_specific.h"
#include "hinstance.h"
#include "string.h"
#include "com_generic.h"

#define ComRegIfSok(hr,expr) FAILED(hr) ? hr : (expr)

namespace teggo
{

  _TEGGO_EXPORTABLE HRESULT CXX_STDCALL ComRegWriteStr(pchar_t root,pchar_t key,pchar_t subkey,pchar_t value);
  _TEGGO_EXPORTABLE HRESULT CXX_STDCALL ComRegDeleteKey(pchar_t key);
  _TEGGO_EXPORTABLE HRESULT CXX_STDCALL ComRegLoadTypeLib(ITypeLib** pI,pwide_t p = 0,HinstanceType &hinstance = Hinstance);
  _TEGGO_EXPORTABLE HRESULT CXX_STDCALL ComRegRegisterTypeLib(HinstanceType &hinstance = Hinstance);
  _TEGGO_EXPORTABLE HRESULT CXX_STDCALL ComRegUnregisterTypeLib(HinstanceType &hinstance = Hinstance);
  _TEGGO_EXPORTABLE HRESULT CXX_STDCALL ComRegRegisterCoClass(const GUID& clsid,pchar_t progID,pchar_t desc,long ver,HinstanceType &hinstance = Hinstance);
  _TEGGO_EXPORTABLE HRESULT CXX_STDCALL ComRegUnregisterCoClass(const GUID& clsid,pchar_t progID,long ver);

  _TEGGO_EXPORTABLE const wchar_t * CXX_STDCALL GUIDToRegString(const GUID &guid,BaseStringT<wchar_t> &s);
  _TEGGO_EXPORTABLE const char * CXX_STDCALL GUIDToRegString(const GUID &guid,BaseStringT<char> &s);

  typedef char _GUID_char48_t[48];
  _TEGGO_EXPORTABLE pchar_t CXX_STDCALL GUIDToRegStringA(const GUID &guid,mutable_t<_GUID_char48_t> const &val = mutable_t<_GUID_char48_t>());
  typedef wchar_t _GUID_wchar48_t[48];
  _TEGGO_EXPORTABLE pwide_t CXX_STDCALL GUIDToRegStringW(const GUID &guid,mutable_t<_GUID_wchar48_t> const &val = mutable_t<_GUID_wchar48_t>());
}

#endif // !defined CXX_NO_MSCOM_SUPPORT

#endif // __506DA0C7_943D_4E7C_A382_00B907BB5CD8__
