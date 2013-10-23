
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
#include "dsscfilter.h"
#include "resource.h"
#include <initguid.h>
#include "../ScraftAPI/spritecraft.h"
#include "logout.h"
#include "RBGFilter.h"
#include "Engine.h"
#include <mmsystem.h>
#include <locale.h>

#if CXX_COMPILER__IS_MSVC_COMPATIBLE
#pragma code_seg(".text$AAA$DLL")
#endif

CRITICAL_SECTION global_critical_section_;
CRITICAL_SECTION *global_critical_section = &global_critical_section_;
extern const IID LIBID_SpriteCraft_Old;

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
  {
    if (dwReason == DLL_PROCESS_ATTACH)
      {
        InitializeCriticalSection(global_critical_section);
        setlocale(LC_ALL,"");
        setlocale(LC_NUMERIC,"C");
        teggo::Hinstance->SetInstanceHandle(hInstance);
        DataSourceScraftFilter->Init();
        DisableThreadLibraryCalls(hInstance);
        ProcessYield();
      }
    else if (dwReason == DLL_PROCESS_DETACH)
      {
        Xdbg( Ldbg << "engine: unloading spritecraft engine" );
        if ( Engine_ComIEngine() )
          Lout << "engine: opps, Engine is not released!";
        teggo::TypeInfoFactory().Term();
        teggo::DataSource->UnregisterAll();
        DeleteCriticalSection(global_critical_section);
      }
    return TRUE;    // ok
  }

struct CoEngineClassFactory :
  TeggoObject<
    TeggoIfs<IClassFactory,TeggoISupportErrorInfo> >
  {
    virtual HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid)
      {
        return IsEqualGUID(TEGGO_GUIDOF(IClassFactory),riid)?S_OK:S_FALSE;
      }

    virtual HRESULT __stdcall CreateInstance(IUnknown* pOuter,const IID& riid, void** ppv)
      {
        rcc_ptr<IEngine> engine;
        HRESULT hr = CreateEngine(SCVersion,&+engine);
        if ( SUCCEEDED(hr) )
          return engine->QueryInterface(riid,ppv);
        return hr;
      }

    virtual HRESULT __stdcall LockServer(BOOL lock)
      {
        return S_OK;
      }
  };

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
  {
    if ( IsEqualCLSID(rclsid,CLSID_Engine) )
      {
        rcc_ptr<IClassFactory> fac ( new CoEngineClassFactory() );
        return fac->QueryInterface(riid,ppv);
      }
    return E_FAIL;
  }

STDAPI DllUnregisterServer(void)
  {
    teggo::ComRegUnregisterTypeLib();
    teggo::ComRegUnregisterCoClass(CLSID_Engine,"SpriteCraft.Engine",SCVerMajor);
    return S_OK;
  }

STDAPI DllRegisterServer(void)
  {
    HRESULT hr = S_OK;

    ::UnRegisterTypeLib(LIBID_SpriteCraft_Old,2,0,0,SYS_WIN32);
    ::UnRegisterTypeLib(LIBID_SpriteCraft_Old,2,1,0,SYS_WIN32);
    ::UnRegisterTypeLib(LIBID_SpriteCraft_Old,2,2,0,SYS_WIN32);
    ::UnRegisterTypeLib(LIBID_SpriteCraft_Old,2,3,0,SYS_WIN32);
    //::UnRegisterTypeLib(LIBID_SpriteCraft_Old,3,0,0,SYS_WIN32);

    hr = ComRegIfSok(hr,teggo::ComRegRegisterTypeLib());
    hr = ComRegIfSok(hr,teggo::ComRegRegisterCoClass(
          CLSID_Engine,"SpriteCraft.Engine",
          "SpriteCraft 2D Engine",SCVerMajor));

    if ( FAILED(hr) )
      DllUnregisterServer();

    return hr;
  }


static long program_version[4];

extern void Logout_LogOsVer();
extern void Logout_LogMemoryInfo();
extern void Logout_LogCPUInfo();

extern "C"
void __stdcall _SetLogging(unsigned on,pwide_t logname)
  {
    static bool stub_written = false;
    if ( on && !LogIsOpen() )
      {
        OpenLog(logname);
        if ( !stub_written )
          {
            Lout << "";
            Lout << _S*"Teggo SpriteCraft 2D Engine, version %?"
              %teggo::GetModuleVersion(Hinstance->GetModulePath());
            BufferT<wchar_t> module(MAX_PATH+1,0);
            GetModuleFileNameW(0,&module[0],MAX_PATH);
            sscanf(teggo::GetModuleVersion(&module[0]).Str(),"%d.%d.%d.%d",
              program_version,program_version+1,program_version+2,program_version+3);
            Lout << "Copyright (c) 2002-2007, Alexey Suda-Chen & Olga Marchevska, Teggo FF";
            Lout << "";
            Logout_LogOsVer();
            Logout_LogCPUInfo();
            Logout_LogMemoryInfo();
            Lout << "";
            stub_written = true;
          }
      }
    else if ( !on )
      CloseLog();
  }

extern "C"
void __cdecl GetProgramVersion(long* version)
  {
    memcpy(version,program_version,sizeof(long)*4);
  }

const IID LIBID_SpriteCraft_Old =
  {0x35E4F96E,0x1B5D,0x4490,{0xB2,0x8B,0xF0,0xA3,0xB2,0x47,0x00,0x20}};
const IID LIBID_SpriteCraft =
  {0x35E4F96E,0x1B5D,0x4490,{0xB2,0x8B,0xF0,0xA3,0xB2,0x47,0x00,0x20}};
const CLSID CLSID_Engine =
  {0x03C4B35B,0x0A4B,0x426c,{0xA7,0x94,0x97,0x18,0xD1,0x3A,0x84,0x2C}};
const IID DIID__IQueueEvents =
  {0x00797D81,0xA843,0x4ffc,{0xB7,0xF1,0x57,0x3C,0x49,0xA2,0x14,0x32}};
const IID DIID__ISpriteEvents =
  {0x0BE1B4F3,0x3652,0x49b6,{0xBF,0x59,0x7F,0xC7,0x83,0xE1,0x89,0x32}};
const CLSID CLSID__CoQueueConnectionPoint =
  {0x38E7A96E,0x445C,0x463a,{0xB1,0x0C,0x84,0x31,0xAA,0x45,0x89,0x61}};
const CLSID CLSID__CoSpriteConnectionPoint =
  {0xEC896096,0x2682,0x4696,{0xBF,0x14,0x9E,0x7A,0xFF,0x69,0x88,0xC7}};
