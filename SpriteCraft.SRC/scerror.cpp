
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

#include "stdafx.h"
#include "scerror.h"
#include "../ScraftAPI/spritecraft.h"

#define ERROR_TO_PCHAR(err) case err: return #err

pchar_t TranslateECode(SCERROR e)
  {
    static char errtext[40];
    switch ( e )
      {
        ERROR_TO_PCHAR(SCE_OK);
        ERROR_TO_PCHAR(SCE_INVALIDARG);
        ERROR_TO_PCHAR(SCE_OUT_OF_MEMORY);
        ERROR_TO_PCHAR(SCE_OUT_OF_RANGE);
        ERROR_TO_PCHAR(SCE_OUT_OF_STREAM);
        ERROR_TO_PCHAR(SCE_BAD_FORMAT);
        ERROR_TO_PCHAR(SCE_BAD_STREAM);
        ERROR_TO_PCHAR(SCE_BAD_IMAGE);
        ERROR_TO_PCHAR(SCE_BAD_STATE);
        ERROR_TO_PCHAR(SCE_EOF);
        ERROR_TO_PCHAR(SCE_UNSUPPORTED);
        ERROR_TO_PCHAR(SCE_UNKNOWN);
        ERROR_TO_PCHAR(SCE_SURFLOST);
        ERROR_TO_PCHAR(SCE_INVALIDRECT);
        ERROR_TO_PCHAR(SCE_GZIPFAIL);
        ERROR_TO_PCHAR(SCE_FINISHED);
        ERROR_TO_PCHAR(SCE_FAIL);
      }
    teggo::SprintF(errtext,sizeof(errtext),"unknown: 0x%x",e);
    return errtext;
  }

HRESULT GenError(BSTR source,BSTR info)
  {
    ICreateErrorInfo* pICE = 0;
    if ( FAILED( ::CreateErrorInfo(&pICE) ) ) return E_FAIL;
    pICE->SetSource(source);
    pICE->SetDescription(info);
    IErrorInfo* pIEI = 0;
    if ( SUCCEEDED( pICE->QueryInterface(TEGGO_GUIDOF(IErrorInfo),(void**)&pIEI) ) ) 
      {
        ::SetErrorInfo(0L,pIEI);
        pIEI->Release();
      }
    pICE->Release();
    return E_FAIL;
  }

inline pwide_t undefined_if_empty(pwide_t val) 
  {
    if ( !val || *val == 0 ) return L"undefined";
    return val;
  }

HRESULT GenSCWideError(FormatT<wchar_t> const &info, pwide_t reason_prefix)
  {
    return GenSCWideError(info.Str(),reason_prefix);
  }

HRESULT GenSCWideError(pwide_t info, FormatT<wchar_t> const &reason_prefix)
  {
    return GenSCWideError(info,reason_prefix.Str());
  }

HRESULT GenSCWideError(pwide_t info,pwide_t reason_prefix)
  {
    widestring source;
    widestring description;
    source << _S*"SpriteCraft Engine %?.%?" %SCVerMajor %SCVerMinor;
    if ( reason_prefix )
      description.Set(_S*L"{%s}" %reason_prefix);
    description << _S*L"%s" %(info?info:undefined_if_empty(GetErrorLog()));
    Lout << _S*"SCERR: %?"%description;
    return GenError((BSTR)source.Str(),(BSTR)description.Str());
  }
  
