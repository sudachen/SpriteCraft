// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__CB0BC8C6_12E8_4BD5_A4F0_7A50A0E0F7FF__INCLUDED_)
#define AFX_STDAFX_H__CB0BC8C6_12E8_4BD5_A4F0_7A50A0E0F7FF__INCLUDED_

#define NOMINMAX

#define TEGGO_SEPRATE_IMPLEMENTATION

#include "../Classes/classes.h"
#include "../MoleSkin2/moleskin2.h"
#include "../python_over_com/kaa.h"
#include "../python_over_com/python_over_com.h"
// #include "../lib.kaa/python.INC/Python.h"
// #include "../lib.kaa/python.INC/structmember.h"
// #include "../lib.kaa/com_python.h"

#undef PlaySound

//#include "spritecraft.h"
#include "flat_e.h"
#include "spritecraft_interfaces.h"
#include "../ScraftAPI/guids_.h"

template < class T >
  struct XComObject__ : TeggoObject<T>
    {
      virtual void XComObject__check() {} // check method
      virtual u32_t __stdcall Release()
        {
          u32_t rec = TeggoObject<T>::GetRefCount__();
          if ( rec == 2 ) T::CheckDispose();
          return TeggoObject<T>::Release();
        }
    };

extern "C" void __stdcall _SetLogging(unsigned on,pwide_t logname=0);

extern u32_t GetMillis_OnFrame();

//extern "C" char* StrAlloc(pchar_t text,int len);
//extern "C" char* StrAllocW(const wchar_t* text,int len);
//extern "C" char* StrSetAlloc(char** scraftStr,pchar_t text,int len);
//extern "C" char* StrSetAllocW(char** scraftStr,const wchar_t* text,int len);
//extern "C" void  StrFree(char** scraftStr);

/**

  Engine stuff

*/

extern "C" float    Engine_GetMouseX();
extern "C" float    Engine_GetMouseY();
extern "C" bool     Engine_EvtIsRClick();
extern "C" bool     Engine_EvtIsLClick();
extern "C" pwide_t  Engine_GetSSTName();
extern "C" void     Engine_EvtGet(EVENT*);
extern "C" IEngine* Engine_ComIEngine();
extern "C" void     Engine_ComAddOnUpdateListener(IOnUpdateListener* listener);
extern "C" void     Engine_ComAddCollidable(ICollidableObject*,long group,bool active,bool trackable);
extern "C" long     Engine_ComScheduleWorker(ICommand*);
extern "C" void     Engine_ComInterruptWorker(long wid);
extern "C" void     Engine_ComStopSpriteAnimation(IWeakSprite* spr);
extern "C" void     Engine_ComStartSpriteAnimation(IWeakSprite* spr,long startFrno,long stopFrno,float fps,IUnknown* listener,long cookie,bool loop);
extern "C" void     Engine_ComAddSpriteMouseListener(IWeakSpriteEx* spr);
extern "C" void     Engine_ComRemoveSpriteMouseListener(IWeakSpriteEx* spr);
extern "C" void     Engine_Error(unsigned errcode,pchar_t msg);
extern "C" HRESULT  Engine_HErrorIfIs();

extern "C" bool Engine_IsAlive();

#include "../synchronize.h"

inline VARIANT_BOOL VariantBool( bool b ) { return b ? VARIANT_TRUE : VARIANT_FALSE; }

extern char const SCRAFT_ERR[];
extern char const SCRAFT_PFX[];

#include "./errors.h"
#include "../../Media/lib.ddraw/ddraw_inst.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CB0BC8C6_12E8_4BD5_A4F0_7A50A0E0F7FF__INCLUDED)
