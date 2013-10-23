
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
#include "../ScraftAPI/spritecraft.h"
#include "RBGFilter.h"
#include "Engine.h"
#include "Engine_DInput.h"
#include "CollideMgr.h"
#include "SoundMgr.h"
#include "SpriteHelper.h"
#include "AnimationMgr.h"
#include "MListenMgr.h"
#include "SpritesEnumerator.h"
#include "TileMap.h"
#include "Particles.h"
#include "ParticlesEnumerator.h"
#include "console.h"
#include "SST.h"
#include <limits.h>
#include <mmsystem.h>
#include "XdataHelper.h"
#undef PlaySound

#if CXX_COMPILER__IS_MSVC_COMPATIBLE && defined __SPLICE_CODE_SEG
#pragma code_seg(".text$AAA$SpriteCraftEngine")
#endif

#pragma comment(lib,"winmm.lib")

static wchar_t ENG_NOTINITIALIZED [] = L"SpriteCraft.SpriteCraftEngine is not initialized";

extern IExecutor *CreateGenericExecutor();
SpriteCraftEngine *g_current_engine = 0;

IRefObject    *g_RSCMAN_holder;
IRefObject    *g_SCREEN_holder;
IRefObject    *g_SPRITES_holder;

void SpriteCraftEngine::InitAttributes_()
  {
    successful_initialized_ = false;
    event_ = EVENT::MakeNoneEvent();
    mouse_X_ = 0;
    mouse_Y_ = 0;
    mouse_DifX_ = 0;
    mouse_DifY_ = 0;
    mouse_mode_ = MouseDirectMode;
    mouse_in_widow_ = false;
    last_mbright_down_ = 0;
    last_mbleft_down_ = 0;
    mbleft_down_ = 0;
    mbright_down_ = 0;
    show_fps_ = 0;
    soft_refresh_ = false;
    timer_pulsar_ = false;
    pulse_refresh_ = false;
    clearColorBits_ = true;
    alive_ = false;
    timer_event_ = 0;
    timer_id_ = 0;
    fixed_fps_ = 0;

    keyb_st_up_    = Key_UP;
    keyb_st_right_ = Key_RIGHT;
    keyb_st_down_  = Key_DOWN;
    keyb_st_left_  = Key_LEFT;
    keyb_st_fire_  = Key_Z;
    keyb_st_alter_ = Key_X;
    keyb_st_mode_  = Key_C;
    keyb_st_esc_   = Key_ESC;

    memset(keys_state_,0,sizeof(keys_state_));
    memset(keys_state_last_,0,sizeof(keys_state_last_));


    Time_SetZeroValues_();
  }

void Engine_Common_SetZeroValues()
  {
    g_RSCMAN_holder     = 0;
    g_SCREEN_holder     = 0;
    g_SPRITES_holder    = 0;
  }

void SpriteCraftEngine::Time_SetZeroValues_()
  {
    last_display_millis_ = 0;
    last_millis_on_frame_ = 0;
    update_was_called_ = false;
    update_was_called_after_next_event_ = false;
    last_update_frametick_ = 0;
  }

IEngine* Engine_GetCurrent()
  {
    return g_current_engine;
  }

bool Engine_IsAlive()
  {
    return g_current_engine && g_current_engine->alive_;
  }

void SpriteCraftEngine::ChangePriority_()
  {
    if ( SCREEN::IsFullscreen() )
      SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
    else
      SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
  }

SpriteCraftEngine::SpriteCraftEngine()
  {
    _SYNCHRONIZE_;
    REQUIRE ( !g_current_engine );

    ResetMillis();
    timer_.Start();

    InitAttributes_();
    Time_SetZeroValues_();

    g_RSCMAN_holder  = RSCMAN::Acquire();
    on_update_set_ = MemPtr(new ONUPDATE_LISTENER_SET());
    collidemgr_ = MemPtr(new CollideMgr());
    animamgr_   = MemPtr(new AnimationMgr());
    mlistenmgr_ = MemPtr(new MListenMgr());
    sndmgr_     = MemPtr(new SoundMgr());
    fill_color_ = mRGB_t(0,0,0);
    g_current_engine = this;
    alive_ = true;
  }

void SpriteCraftEngine::__Terminate()
  {
    alive_ = false;
    if ( !!g_SPRITES_holder )
      {
        Log(L"~\nengine: terminate engine....");

        Unrefe(executor_);
        Unrefe(sndmgr_);
        Unrefe(collidemgr_);
        Unrefe(on_update_set_);
        Unrefe(animamgr_);
        Unrefe(mlistenmgr_);

        SndSubsys->Release();
        if ( g_SPRITES_holder ) MSKINSPACE::Release(g_SPRITES_holder);
        Unrefe(dinput_);
        if ( g_SCREEN_holder )  MSKINSPACE::Release(g_SCREEN_holder);
        if ( g_RSCMAN_holder )  MSKINSPACE::Release(g_RSCMAN_holder);

        Log(L"engine: engine terminated\n~");
      }

    if ( g_current_engine == this )
      g_current_engine = 0;

    if ( timer_id_ )
      {
        timeKillEvent( timer_id_ );
        timer_id_ = 0;
      }

    if ( timer_event_ )
      {
        CloseHandle( timer_event_ );
        timer_event_ = 0;
      }

    fixed_fps_ = 0;

    Unrefe(console_);

    successful_initialized_ = false;
  }

SpriteCraftEngine::~SpriteCraftEngine()
  {
    _SYNCHRONIZE_;
    __Terminate();
  }


//extern "C"
HRESULT __stdcall CreateEngine_(IEngine** pI)
  {
    _SYNCHRONIZE_;
    if ( g_current_engine )
      return ((IEngine*)g_current_engine)->
                QueryInterface(TEGGO_GUIDOF(IEngine),(void**)pI);
    *pI = new TeggoObject<SpriteCraftEngine>();
    return S_OK;
  }

extern "C"
HRESULT __stdcall CreateEngine(SpriteCraftVer scver ,IEngine** pI)
  {
    if ( scver > SCVersion || scver < SCVersionMinSupported )
      {
        widestring text;
        text.Set(
          _S*L":AcquireEngineEx:\n required version %d.%d is incompatible with installed %d.%d"
            %(scver/100)
            %(scver%100)
            %SCVerMajor
            %SCVerMinor);

        // MessageBoxW(0,text,L"MoleStudio SpriteCraft",MB_OK|MB_ICONERROR);
        return GenSCWideError(+text,0);
      }
    return CreateEngine_(pI);
  }

HRESULT __stdcall Engine_RegisterPyType()
  {
    return SpriteCraftEngine::RegisterPyType("_Engine");
  }

extern "C"
HRESULT WINAPI IsSprite(IDispatch* pI,VARIANT_BOOL* bRet)
  {
    _SYNCHRONIZE_;
    ISprite* spI = 0;
    if ( pI && S_OK == pI->QueryInterface(*teggo_guidof<ISprite>(),(void**)&spI) && spI )
      {
        *bRet = VARIANT_TRUE;
        spI->Release();
      }
    else
      {
        *bRet = VARIANT_FALSE;
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::InterfaceSupportsErrorInfo(REFIID riid)
  {
    return IsEqualGUID(TEGGO_GUIDOF(IEngine),riid)?S_OK:S_FALSE;
  }

mrgb_t SpriteCraftEngine::IColorHelper_GetColor() const
  {
    return fill_color_;
  }

void SpriteCraftEngine::IColorHelper_SetColor(mrgb_t c)
  {
    fill_color_ = c;
    SCREEN::SetFillColor(fill_color_);
  };

void SpriteCraftEngine::InitDefaults_()
  {
    SPRITES::NewFontKlassFrom(L"$spritecraft$arial/14$",L"@arial14a.bmp",false,0,SPRITE_COLORKEY,0);
    SPRITES::NewFontKlassFrom(L"$spritecraft$font$",L"@arial14a.bmp",false,0,SPRITE_COLORKEY,0);
    SPRITES::NewFontKlassFrom(L"$spritecraft$console$font$",L"@arial14a.bmp",false,0,SPRITE_COLORKEY,0);
    SPRITES::NewDummyKlass(L"$spritecraft$dummy$");
    SPRITES::NewDummyKlass(L"$spritecraft$blank$",true);
    console_->Reinit();
  }

HRESULT __stdcall SpriteCraftEngine::Init(tagScDevice devno)
  {
    return InitEx(devno,tagVideoMode(VMODE_640x480|VMODE_WINDOWED));
  }

static void ConvertVModeToGfx(
  tagVideoMode newVal,GFX_VALUE* vmode,GFX_VALUE* bpp,GFX_VALUE* wndmod)
  {
    *vmode = GFX_UNKNOWN;
    switch ( newVal & VMODE_BITS )
      {
      case VMODE_320x240:   *vmode = GFX_320x240; break;
      case VMODE_640x480:   *vmode = GFX_640x480; break;
      case VMODE_800x600:   *vmode = GFX_800x600; break;
      case VMODE_1024x768:  *vmode = GFX_1024x768; break;
      case VMODE_1280x1024: *vmode = GFX_1280x1024; break;
      case VMODE_1600x1200: *vmode = GFX_1600x1200; break;
      }
    *vmode =
      GFX_VALUE(
      *vmode |
      (( newVal & VMODE_NOWINFRAME ) ? GFX_WIN32NOFRAME : 0 ) |
      (( newVal & VMODE_NOSYSMENU ) ? GFX_WIN32NOMENU : 0 ) );

    *bpp = ((newVal & VMODE_32BIT) ? GFX_BPP32 : GFX_BPP16);

    if ( (newVal&VMODE_WINDOW_BITS) == VMODE_WINDOWED )
      *wndmod = GFX_WINDOWED;
    else if ( (newVal&VMODE_WINDOW_BITS) == VMODE_FULLSCREEN )
      *wndmod = GFX_FULLSCREEN;
    else
      *wndmod = GFX_UNKNOWN;
  }

HRESULT __stdcall SpriteCraftEngine::InitEx(tagScDevice devno,tagVideoMode init_vmode)
  {
    _SYNCHRONIZE_;

    if ( !successful_initialized_ )
      {
        ResetErrorLog();
        rsc_path_ = L":/";
        DataSourceScraftFilter->SetDefaultSearchPath(+rsc_path_);
        Lout << "~\nengine: initialize engine....";

        //MCORE::ReStart();
        clearColorBits_ = true;
        console_ = RccPtr(new Console());

        if ( !g_RSCMAN_holder ) return GenSCWideError(0);

        GFX_VALUE vmode;
        GFX_VALUE bpp;
        GFX_VALUE wndmod;
        ConvertVModeToGfx(init_vmode,&vmode,&bpp,&wndmod);

        Lout << "engine: using Direct3D7 device";
        if ( !(g_SCREEN_holder  =
          SCREEN::Acquire(
            GFX_DEVICE(GFX_D3D7_DEVICE|(devno&DevDisableDrvInfo?GFX_DEVICE_DISABLE_DRVINFO:0)),
            wndmod,vmode,bpp)) )
              goto lb_reason;
        if ( !(g_SPRITES_holder = SPRITES::Acquire() ) )
          goto lb_reason;

        //if ( !(g_SOUNDS_holder  = SOUNDS::Acquire(&global_cs)) )
        if ( FAILED( SndSubsys->AcquireDxS(&global_critical_section) ) )
          goto lb_reason;

        InitDefaults_();
        FPS_StartClock();

        if ( GfxControl* ctl = SCREEN::GetControl() )
          ctl->SetTitle(L"Teggo SpriteCraft 2D Engine (http://spritecraft.teggo.com)");

        executor_ = RccPtr(CreateGenericExecutor());

        if ( devno & 0x100 )
          {
            Lout << "engine: using DirectInput";
            dinput_ = new Engine_DInput();
          }
        else
          {
            Lout << "engine: using WinAPI Input";
          }

        successful_initialized_ = true;
        timer_.SetUp_();

        SCREEN::SetFillColor(fill_color_);
        ScClearInput();

        Log(L"engine: engine initialized\n~\n");

        ChangePriority_();
        Sleep(10);

        return S_OK;
      }
     else
      {
        return GenSCWideError(L"already initialized",L":InitDevice: ");
      }

  lb_reason:
    return GenSCWideError(0,L":InitDevice:");
  }

HRESULT __stdcall SpriteCraftEngine::AttachTo( long wnd )
  {
    _SYNCHRONIZE_;

    HWND hwnd = (HWND)wnd;

    if ( GfxControl* gfxc = SCREEN::GetControl() )
      if ( S_OK == gfxc->AttachTo(&hwnd) )
        return S_OK;
    return GenSCWideError(0,L":AttachTo:");
  }

struct in_display_guard
  {
    static bool flag;
    in_display_guard() { flag = true; }
    ~in_display_guard() { flag = false; }
  };
bool in_display_guard::flag = false;

HRESULT __stdcall SpriteCraftEngine::get_windowIsActive(VARIANT_BOOL *rVal)
  {
    _SYNCHRONIZE_;
    return (*rVal = SCREEN::IsActive()?VARIANT_TRUE:VARIANT_FALSE), S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::Display()
  {
    _SYNCHRONIZE_;

    if ( in_display_guard::flag )
      return S_OK;
    in_display_guard _;

    if ( !SCREEN::IsActive() )
      {
        Sleep(10);
        if ( SCREEN::IsFullscreen() )
          return S_OK;
      }

    if ( !update_was_called_ )
      Update();

    g_bitmap_dc_pool.UnpoolAll();

    if( ((soft_refresh_?SPRITES::NeedRefresh():true) || SCREEN::IsRubbish())
        && SCREEN::BeginDraw() == SCE_OK )
      {
        if ( clearColorBits_ )
          SCREEN::Clear();
        else
          SCREEN::ClearDepth();

        SPRITES::Display();
        if ( show_fps_ )
          {
            static int w = SCREEN::TextWidth(L" FPS 8888.888 ");
            SCREEN::Print(SCREEN::WndWidth()-w,0,
                         (_S*L" FPS %4.2f " %FPS_GetFPS()).Str());
          }

        console_->Display();
        SCREEN::EndDraw();
        FPS_Update();
      }

    timer_.EndFrame();
    update_was_called_ = false;

    return S_OK;
  }


HRESULT __stdcall SpriteCraftEngine::SleepMillis(long millis)
  {
    ::Sleep(millis);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::NewSprite(BSTR spr_name, BSTR klass_name, long layer, int x, int y, ISprite **pI)
  {
    return NewSprite_Internal(spr_name,klass_name,layer,x,y,pI);
  }

HRESULT SpriteCraftEngine::NewSprite_Internal(pwide_t spr, pwide_t klass, int layer, int x, int y, ISprite **pI)
  {
    _SYNCHRONIZE_;

    pwide_t e_info = 0;
    SpriteHelper* helper;
    Sprite* sprite;

    ResetErrorLog();

    if ( !spr ) spr = L"";
    if ( !klass || !*klass ) klass = L"$spritecraft$dummy$";

    if ( !SPRITES::HasKlass(klass) )
      if ( SCE_OK != SPRITES::NewKlass(klass) )
        goto lb_reason;

    sprite = SPRITES::NewSprite(spr,klass,layer,x,y);
    if ( !sprite )
      goto lb_reason;

    helper = new XComObject__<SpriteHelper>;
    sprite->SetSubImpl(helper);
    helper->noname_ = *spr == 0;
    if ( helper->noname_ )
      sprite->SetParentDeps(sprite->GetParentDeps()|SPRITE_DEPS_DISPOSE);
    *pI = helper;
    return S_OK;

  lb_reason:
    return GenSCWideError(e_info,_S*L":NewSprite of'%?': " %klass);
  }

HRESULT __stdcall SpriteCraftEngine::FindSprite(BSTR spr_name,ISprite **pI)
  {
    VARIANT_BOOL bRet;
    return FindSprite_(spr_name,pI,&bRet);
  }

HRESULT __stdcall SpriteCraftEngine::FindSprite_(BSTR spr_name,ISprite **pI,VARIANT_BOOL* bRet)
  {
    _SYNCHRONIZE_;
    *bRet = VARIANT_FALSE;
    *pI = 0;
    Sprite* sprite = SPRITES::FindSprite(spr_name);
    if ( sprite )
      {
        SpriteHelper* helper = Adaptate<SpriteHelper>(sprite->GetSubImpl());
        if ( helper )
          {
            *bRet = VARIANT_TRUE;
            *pI = Refe((ISprite*)helper);
          }
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::Purge()
  {
    _SYNCHRONIZE_;

    executor_->_Purge();
    collidemgr_ = MemPtr(new CollideMgr());
    collidemgr_ = MemPtr(new CollideMgr());
    sndmgr_     = MemPtr(new SoundMgr());
    on_update_set_ = MemPtr(new ONUPDATE_LISTENER_SET());
    animamgr_   = MemPtr(new AnimationMgr());
    mlistenmgr_ = MemPtr(new MListenMgr());
    TileMapManager::Purge();
    SPRITES::Purge();
    SndSubsys->Purge();
    InitDefaults_();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_needRefresh(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SCREEN::IsRubbish()||SPRITES::NeedRefresh()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_millis(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = GetMillis();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_fullscreen(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SCREEN::IsFullscreen()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_fullscreen(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( SCREEN::IsFullscreen() != !!newVal )
      SCREEN::SetMode(newVal!=VARIANT_FALSE?GFX_FULLSCREEN:GFX_WINDOWED);
    ChangePriority_();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_rscpath(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SysAllocString(rsc_path_.Str());
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_rscpath(BSTR newVal)
  {
    _SYNCHRONIZE_;

    if ( !successful_initialized_ )
      return GenSCWideError(ENG_NOTINITIALIZED);

    rsc_path_ = newVal;
    RSCMAN::SetSubfolder(+rsc_path_);
    DataSourceScraftFilter->SetDefaultSearchPath(rsc_path_);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_showFps(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = show_fps_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_showFps(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    bool foo = (newVal!=VARIANT_FALSE);
    if ( foo != show_fps_ )
      {
        SCREEN::MakeRubbish();
        show_fps_ = foo;
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsLClick(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = Engine_EvtIsLClick()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsRClick(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = Engine_EvtIsRClick()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::Log(BSTR text)
  {
    _SYNCHRONIZE_;
    Lout << text;
    //g_console->Println(text);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_logging(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    //*pVal = MCORE::IsLogging()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_logging(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    //MCORE::SetLogging(newVal != FALSE);
    _SetLogging(newVal != FALSE );
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_title(BSTR *pVal)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* ctl =  SCREEN::GetControl() )
      {
        *pVal = SysAllocString(+ctl->GetTitle());
      }
    else
      {
        *pVal = SysAllocString(L"");
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_title(BSTR newVal)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* ctl =  SCREEN::GetControl() )
      {
        ctl->SetTitle(newVal);
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EvtIsMouseLeave(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = event_.type == EVENT::POINTER_LEAVE?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_mouseIn(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = mouse_in_widow_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::FindSpriteAtMouse(long nearlayer, long farlayer,ISprite **pVal)
  {
    VARIANT_BOOL bRet;
    return FindSpriteAtMouseEx(nearlayer,farlayer,pVal,&bRet);
  }

HRESULT __stdcall SpriteCraftEngine::FindSpriteAtMouseEx(long nearlayer, long farlayer,ISprite **pVal,VARIANT_BOOL* bRet)
  {
    _SYNCHRONIZE_;
    if ( !successful_initialized_ )
      return GenSCWideError(ENG_NOTINITIALIZED);

    if ( nearlayer < 0 ) nearlayer = 0;
    if ( farlayer < 0 ) farlayer = 0;
    *bRet = VARIANT_FALSE;
    *pVal = 0;

    if ( Sprite* spr = SPRITES::FindSpriteAt(mouse_X_,mouse_Y_,nearlayer,farlayer) )
      if ( SpriteHelper* helper = Adaptate<SpriteHelper>(spr->GetSubImpl()) )
        {
          *bRet = VARIANT_TRUE;
          *pVal = Refe((ISprite*)helper);
        }

    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_nativeCursor(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;

    if ( GfxControl* ctl =  SCREEN::GetControl() )
      {
        ctl->NativeCursor(newVal!=VARIANT_FALSE);
      }

    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::IsSprite(IDispatch *pI, VARIANT_BOOL *bRet)
  {
    _SYNCHRONIZE_;
    return ::IsSprite(pI,bRet);
  }

HRESULT __stdcall SpriteCraftEngine::get_softRefresh(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = soft_refresh_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_softRefresh(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    soft_refresh_ = newVal != VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::SstLoad(BSTR source, long flags)
  {
    __synchronize__
      {
        if ( !successful_initialized_ )
          return ScraftError(ENG_NOTINITIALIZED);

        if ( SCE_OK != SST->Load(source,flags) )
          return ScraftError(_E.value?_E.reason:0);

        return S_OK;
      }
  }

HRESULT __stdcall SpriteCraftEngine::put_SST(BSTR newVal)
  {
    return SstLoad(newVal,SstLoadBack|SstLoadRewrite);
  }

HRESULT __stdcall SpriteCraftEngine::NewSprite_(BSTR klass, long layer, ISprite **pI)
  {
    return NewSprite_Internal(0,klass,layer,0,0,pI);
  }

HRESULT __stdcall SpriteCraftEngine::get_scrWidth(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SCREEN::GetWidth();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_scrHeight(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SCREEN::GetHeight();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_vMode(enum tagVideoMode *pVal)
  {
    _SYNCHRONIZE_;
    GFX_VALUE vmode = GFX_UNKNOWN;
    GFX_VALUE bpp = GFX_UNKNOWN;

    if ( SCREEN::GetMode(0,&vmode,&bpp) != SCE_OK )
      return GenSCWideError(0,L":retrive vMode: ");

    tagVideoMode vmode_f = bpp == GFX_BPP32 ? tagVideoMode(0) : VMODE_32BIT;

    switch ( vmode )
      {
      case GFX_320x240:   *pVal = tagVideoMode(VMODE_320x240|vmode_f); break;
      case GFX_640x480:   *pVal = tagVideoMode(VMODE_640x480|vmode_f); break;
      case GFX_800x600:   *pVal = tagVideoMode(VMODE_800x600|vmode_f); break;
      case GFX_1024x768:  *pVal = tagVideoMode(VMODE_1024x768|vmode_f); break;
      case GFX_1280x1024: *pVal = tagVideoMode(VMODE_1280x1024|vmode_f); break;
      case GFX_1600x1200: *pVal = tagVideoMode(VMODE_1600x1200|vmode_f); break;
      default:
        *pVal = tagVideoMode(-1);
      };

    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_vMode(tagVideoMode newVal)
  {
    _SYNCHRONIZE_;
    GFX_VALUE vmode;
    GFX_VALUE bpp;
    GFX_VALUE wndmod;

    ConvertVModeToGfx(newVal,&vmode,&bpp,&wndmod);

    if ( vmode == GFX_UNKNOWN )
      return GenSCWideError(L"invalid videomode",L":set vMode: ");

    ResetErrorLog();
    if ( SCREEN::SetMode(wndmod,vmode,bpp) != SCE_OK )
      return GenSCWideError(0,L":set vMode: ");

    ChangePriority_();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EnumerateSprites(ISpritesEnumerator **pI)
  {
    _SYNCHRONIZE_;
    *pI = new SpritesEnumerator();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EnumerateSpritesEx(long lnear,long lfar,ISpritesEnumerator **pI)
  {
    _SYNCHRONIZE_;
    *pI = new SpritesEnumerator(lnear,lfar);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::MoveWindowTo(long x, long y)
  {
    _SYNCHRONIZE_;
    if ( !SCREEN::IsFullscreen() ) {
      HWND hwnd = 0;
      GfxControl* ctl = SCREEN::GetControl();
      if ( !ctl
           || ctl->GetWindowHandle(&hwnd,sizeof(hwnd)) != SCE_OK
           || hwnd == 0 )
        return E_FAIL;
      RECT rect = {0};
      GetWindowRect(hwnd,&rect);
      SetWindowPos(hwnd,0,x,y,0,0,SWP_NOSIZE);
      mouse_X_ -= x-rect.left;
      mouse_Y_ -= y-rect.top;
    }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_windowX(long *pVal)
  {
    _SYNCHRONIZE_;
    return LocateWindow(pVal,0);
  }

HRESULT __stdcall SpriteCraftEngine::put_windowX(long newVal)
  {
    _SYNCHRONIZE_;
    long valY;
    if ( LocateWindow(0,&valY) != S_OK ) return E_FAIL;
    return MoveWindowTo(newVal,valY);
  }

HRESULT __stdcall SpriteCraftEngine::get_windowY(long *pVal)
  {
    _SYNCHRONIZE_;
    return LocateWindow(0,pVal);
  }

HRESULT __stdcall SpriteCraftEngine::put_windowY(long newVal)
  {
    _SYNCHRONIZE_;
    long valX;
    if ( LocateWindow(&valX,0) != S_OK ) return E_FAIL;
    return MoveWindowTo(valX,newVal);
  }

HRESULT __stdcall SpriteCraftEngine::PlaceWindowAt(enum tagPosition position)
  {
    _SYNCHRONIZE_;
    RECT rect = {0};
    SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
    long scrW = rect.right-rect.left;
    long scrH = rect.bottom-rect.top;
    long scrX = rect.left;
    long scrY = rect.top;
    switch ( position ) {
    case PositionLeftTop:     return MoveWindowTo(scrX,scrY);
    case PositionCenterTop:   return MoveWindowTo(scrX+(scrW-SCREEN::WndWidth())/2,scrY);
    case PositionRightTop:    return MoveWindowTo(scrX+scrW-SCREEN::WndWidth(),scrY);
    case PositionRightCenter: return MoveWindowTo(scrX+scrW-SCREEN::WndWidth(),scrY+(scrH-SCREEN::WndHeight())/2);
    case PositionRightBottom: return MoveWindowTo(scrX+scrW-SCREEN::WndWidth(),scrY+scrH-SCREEN::WndHeight());
    case PositionCenterBottom:return MoveWindowTo(scrX+(scrW-SCREEN::WndWidth())/2,scrY+scrH-SCREEN::WndHeight());
    case PositionLeftBottom:  return MoveWindowTo(scrX,scrY+scrH-SCREEN::WndHeight());
    case PositionLeftCenter:  return MoveWindowTo(scrX,scrY+(scrH-SCREEN::WndHeight())/2);
    case PositionCenter:      return MoveWindowTo(scrX+(scrW-SCREEN::WndWidth())/2,scrY+(scrH-SCREEN::WndHeight())/2);
    }
    return S_OK;
  }

#define Engine_STDMETHOD(x) HRESULT __stdcall SpriteCraftEngine::x

HRESULT __stdcall SpriteCraftEngine::get_volume(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = 0;
    return SndSubsys->GetVolume(pVal);
  }

HRESULT __stdcall SpriteCraftEngine::put_volume(long newVal)
  {
    _SYNCHRONIZE_;
    return SndSubsys->SetVolume(newVal);
  }

HRESULT __stdcall SpriteCraftEngine::get_supportSound(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    return SndSubsys->IsSupported(pVal);
  }

HRESULT __stdcall SpriteCraftEngine::get_enableSound(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    return SndSubsys->IsEnabled(pVal);
  }

HRESULT __stdcall SpriteCraftEngine::put_enableSound(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    return SndSubsys->Enable(newVal);
  }

static pwide_t SST_RECORD_HAS_NO_PROPERTY_s = L"SST record has no property '%s'";
HRESULT __stdcall SpriteCraftEngine::NewPredefinedTileMap(BSTR name, BSTR pattern, long layer, ITileMap **pI)
  {
    _SYNCHRONIZE_;
    pwide_t e_info = 0;
    long symmap[256];
    HRESULT hr;
    long cols,rows;
    widestring ws;
    auto_ref<Properties> prop;
    rcc_ptr<ITileMap> tmap;
    Xnode *xmln = 0;

    ResetErrorLog();

    //if ( !g_sst_name ) { e_info = L"SST is not set"; goto lb_reason; }

    xmln = SST->GetKlass(pattern);
    if ( !xmln  ) goto lb_reason;

    if ( !xmln->GetParam(L"rows") )   { e_info = ws.Set(_S*SST_RECORD_HAS_NO_PROPERTY_s %"rows");    goto lb_reason; }
    if ( !xmln->GetParam(L"cols") )   { e_info = ws.Set(_S*SST_RECORD_HAS_NO_PROPERTY_s %"cols");    goto lb_reason; }
    if ( !xmln->GetParam(L"size") )   { e_info = ws.Set(_S*SST_RECORD_HAS_NO_PROPERTY_s %"tilesize");goto lb_reason; }
    if ( !xmln->GetParam(L"tiles") )  { e_info = ws.Set(_S*SST_RECORD_HAS_NO_PROPERTY_s %"tiles");   goto lb_reason; }

    cols = teggo::ToLong(xmln->GetParam(L"cols"));
    rows = teggo::ToLong(xmln->GetParam(L"rows"));
    hr = NewTileMap(name,cols,rows,teggo::ToLong(xmln->GetParam(L"size")),layer,&+tmap);
    if ( FAILED(hr) ) return hr;

    {
      widesplitter splitter(+widestring(xmln->GetParam(L"tiles")).Trim(),L',');
      while ( splitter.Next() ) {
        widesplitter vals(splitter.Get(),L':');
        long count=1,first=0,no;
        if ( vals.Count() > 2 ) swscanf(vals.Get(2),L"%d",&count);
        if ( vals.Count() > 1 ) swscanf(vals.Get(1),L"%d",&first);
        else count = 0;
        HRESULT hr = tmap->AddTilesFrom((BSTR)vals.Get(0),first,count,&no);
        if ( FAILED(hr) ) {
          tmap ->Dispose();
          return hr;
        }
      }
    }

    {for ( int i = 'a'; i < 'z'; ++i ) {
      if ( pwide_t val = xmln->GetParam(_S*L".%c"%i) )
        symmap[i] = teggo::ToLong(val);
      else symmap[i] = -1;
    }}

    {for ( int row=0; row<rows; ++row ) {
      asciistring rowS ( xmln->GetParam(teggo::Itow(row)) );
      for ( int col=0, colE = teggo_min<long>(rowS.Length(),cols); col < colE; ++col )
      {
        char c = rowS[col];
        long tile = -1;
        if ( c >= '0' && c <= '9' ) tile = c-'0';
        else if ( c >= 'a' && c <= 'z' ) tile = symmap[c];
        if (tile >= 0 ) {
          tmap->SetTile(col,row,tile);
        }
      }
    }}

    if ( teggo::ToBool(xmln->GetParam(L"xcycled")) ) tmap->put_xCycled(VARIANT_TRUE);
    if ( teggo::ToBool(xmln->GetParam(L"ycycled")) ) tmap->put_yCycled(VARIANT_TRUE);

    *pI = tmap.forget_();
    return S_OK;
  lb_reason:
    return GenSCWideError(e_info,_S*L":NewPredefinedTileMap of '%?':" %pattern);
  }

HRESULT __stdcall SpriteCraftEngine::NewTileMap(BSTR map_name, long columns, long rows, long tileSize, long layer, ITileMap **pI)
  {
    _SYNCHRONIZE_;
    if ( layer < 0 ) layer = 0;
    if ( rows <= 0 ) rows = 1;
    if ( columns <= 0 ) columns = 1;
    if ( tileSize <= 0 ) tileSize = 16;

    rcc_ptr<TileMapHelper> tilmap;
    if ( SUCCEEDED(TileMapManager::DoMake(map_name,rows,columns,tileSize,layer,&tilmap)))
      {
        *pI = tilmap.forget_();
        return S_OK;
      }
    else return E_FAIL;
  }

HRESULT __stdcall SpriteCraftEngine::NewTileMap_(long columns, long rows, long tileSize, long layer, ITileMap **pI)
  {
    return NewTileMap(0,columns,rows,tileSize,layer,pI);
  }

HRESULT __stdcall SpriteCraftEngine::FindTileMap(BSTR tmap_name,ITileMap **pI)
  {
    VARIANT_BOOL bRet;
    return FindTileMap_(tmap_name,pI,&bRet);
  }

HRESULT __stdcall SpriteCraftEngine::FindTileMap_(BSTR tmap_name,ITileMap **pI,VARIANT_BOOL* bRet)
  {
    _SYNCHRONIZE_;
    *bRet = VARIANT_FALSE;
    *pI = 0;
    TileMapHelper* tilmap = TileMapManager::Find(tmap_name);
    if ( tilmap )
      {
        *bRet = VARIANT_TRUE;
        *pI = Refe((ITileMap*)tilmap);
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_executor(IExecutor **pVal)
  {
    return executor_->QueryInterface(TEGGO_GUIDOF(IExecutor),(void**)pVal);
  }

HRESULT __stdcall SpriteCraftEngine::NewParticles(BSTR name, BSTR klass, long layer, IParticles** pI)
  {
    _SYNCHRONIZE_;
    pwide_t e_info = 0;
    Sprite* pp = 0;

    ResetErrorLog();

    if ( !SPRITES::HasKlass(klass) )
      {
        if ( SCE_OK != SPRITES::NewKlass(klass) ) goto lb_reason;
      }

    pp = SPRITES::NewSprite(name,klass,layer);
    if ( !pp ) goto lb_reason;

    return CreateParticlesHalper(pp,pI,!( name && *name != 0) );

  lb_reason:
    return GenSCWideError(e_info,_S*L":NewParticles of '%?': " %klass);
  }

HRESULT __stdcall SpriteCraftEngine::NewParticles_(BSTR klass, long layer, IParticles **pI)
  {
    return NewParticles(0,klass,layer,pI);
  }

HRESULT __stdcall SpriteCraftEngine::FindParticles(BSTR name, IParticles **pI)
  {
    _SYNCHRONIZE_;
    *pI = 0;
    Sprite* particles = SPRITES::FindSprite(name);
    if ( particles )
      {
        ParticlesHelper* helper = Adaptate<ParticlesHelper>(particles->GetSubImpl());
        if ( helper )
          {
            *pI = Refe((IParticles*)helper);
          }
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_frameTick(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = GetMillis_OnFrame();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_frameTickDelta(long *pVal)
  {
    _SYNCHRONIZE_;
    GetMillis_OnFrame();
    *pVal = timer_.MillisDelta_OnFrame();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::SuspendGameClock()
  {
    _SYNCHRONIZE_;
    timer_.Suspend();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::ResumeGameClock()
  {
    _SYNCHRONIZE_;
    timer_.Resume();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_suspended(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = timer_.Suspended()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::LocateWindow(long *x, long *y)
  {
    _SYNCHRONIZE_;
    HWND hwnd = 0;
    GfxControl* ctl = SCREEN::GetControl();
    if ( !ctl
         || ctl->GetWindowHandle(&hwnd,sizeof(hwnd)) != SCE_OK
         || hwnd == 0 )
      return E_FAIL;
    RECT rect = {0};
    GetWindowRect(hwnd,&rect);
    if (x) *x = rect.left;
    if (y) *y = rect.top;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::NewDummy(BSTR name, ISprite **pI)
  {
    _SYNCHRONIZE_;
    if ( Sprite* sprite = SPRITES::NewSprite(name,L"$spritecraft$dummy$",0,0,0) )
      {
        SpriteHelper* helper = new XComObject__<SpriteHelper>;
        sprite->SetSubImpl(helper);
        helper->noname_ = !name || *name == 0;
        *pI = helper;
        return S_OK;
      }
    else
      {
        return GenSCWideError(L"failed to create dummy sprite");
      }
  }

ISprite* Engine_NewDummy()
  {
    ISprite* p = 0;
    g_current_engine->NewDummy(0,&p);
    return p;
  }

HRESULT __stdcall SpriteCraftEngine::get_background(IRGBFilter **pVal)
  {
    _SYNCHRONIZE_;
    *pVal = new ColorObject(Refe((IColorHelper*)this));
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::NewPredefinedParticles(BSTR name, BSTR pattern, long layer, IParticles **pI)
  {
    _SYNCHRONIZE_;
    return GenSCWideError(L"unimplemented");// S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_HWND(long *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = 0;
    SCREEN::GetWindowHandle(pVal,sizeof(HWND));
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_wndframe(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    return GenSCWideError(L"unimplemented");//S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_wndframe(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    return GenSCWideError(L"unimplemented");//S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_scVersion(long *pVal)
  {
    *pVal = SCVersion;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::PlaySound(BSTR sound, long channel, IUnknown *notifier,long cookie)
  {
    _SYNCHRONIZE_;

    if ( !sndmgr_ )
      GenSCWideError(L"internal error, sound manager is not available");
    return sndmgr_->PlaySoundOn(channel,sound,notifier,cookie,false);

  }

HRESULT __stdcall SpriteCraftEngine::PlaySoundLoop(BSTR sound, long channel, IUnknown *notifier,long cookie)
  {
    _SYNCHRONIZE_;

    if ( !sndmgr_ )
      GenSCWideError(L"internal error, sound manager is not available");
    return sndmgr_->PlaySoundOn(channel,sound,notifier,cookie,true);

  }

HRESULT __stdcall SpriteCraftEngine::NextSound(BSTR sound, long channel, long cookie)
  {
    _SYNCHRONIZE_;

    if ( !sndmgr_ )
      GenSCWideError(L"internal error, sound manager is not available");
    return sndmgr_->NextSoundOn(channel,sound,cookie);
  }

HRESULT __stdcall SpriteCraftEngine::Preload(BSTR name)
  {
    _SYNCHRONIZE_;
    if ( Xnode *node = SST->GetKlass(name) )
      {
        if ( wcscmp(L"sprite",node->GetContent()) || wcscmp(L"font",node->GetContent()))
          {
            ResetErrorLog();
            if ( !SPRITES::HasKlass(name) && SCE_OK != SPRITES::NewKlass(name))
              GenSCWideError(0,_S*L":Preload: klass %?" %name);
            if ( SCE_OK != SPRITES::RestoreKlass(name) )
              GenSCWideError(0,_S*L":Preload: klass %?" %name);
          }
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::Unload(BSTR rscname)
  {
    _SYNCHRONIZE_;
    if ( 0 == wcscmp(rscname,L"$spritecraft$dummy$")
      ||0 == wcscmp(rscname,L"$spritecraft$blank$")
      )
      return GenSCWideError(0,_S*L":Unload: klass '%?' is unloadable" %rscname);
    if ( SPRITES::HasKlass(rscname) )
      SPRITES::UnloadKlass(rscname);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::UnloadEx(BSTR rscname,VARIANT_BOOL removeit)
  {
    _SYNCHRONIZE_;
    if ( 0 == wcscmp(rscname,L"$spritecraft$dummy$")
      ||0 == wcscmp(rscname,L"$spritecraft$blank$")
      )
      return GenSCWideError(0,_S*L":Unload: klass '%?' is unloadable" %rscname);
    if ( SPRITES::HasKlass(rscname) )
      SPRITES::UnloadKlass(rscname,removeit!=VARIANT_FALSE);
    return S_OK;
  }

void Engine_UpdateScheduledScripts();

HRESULT __stdcall SpriteCraftEngine::Update()
  {
    _SYNCHRONIZE_;
    if ( update_was_called_ ) return S_OK;
    update_was_called_ = true;
    update_was_called_after_next_event_ = true;
    u32_t millis_on_frame = GetMillis_OnFrame();
    if ( !last_update_frametick_ ) last_update_frametick_ = millis_on_frame;
    long tickdelta = millis_on_frame-last_update_frametick_;
    if ( sndmgr_ ) sndmgr_->CheckForStopped();
    if ( tickdelta > 0 )
      {

        last_update_frametick_ = millis_on_frame;

        mlistenmgr_->Update();
        animamgr_->Update();

        if (!on_update_set_->Empty())
          {
            ONUPDATE_LISTENER_SET::Position i = on_update_set_->Begin();
            unsigned count = on_update_set_->Count();
            while ( i && count-- )
              {
                if ( !(*i)->OnUpdateListener(tickdelta) )
                  on_update_set_->Erase(i);
                else
                  ++i;
              }
          }

        executor_->_Execute(0);
        collidemgr_->CollideCheck();
      }
    executor_->_Execute(1);
    return S_OK;
  }

extern HRESULT __stdcall NewSpritesList_(ISpritesList** pI);
HRESULT __stdcall SpriteCraftEngine::NewSpritesList(ISpritesList **pI)
  {
    _SYNCHRONIZE_;
    return NewSpritesList_(pI);
  }

extern HRESULT __stdcall NewRandomizer_(IRandomizer** pI);
HRESULT __stdcall SpriteCraftEngine::NewRandomizer(IRandomizer **pI)
  {
    _SYNCHRONIZE_;
    return NewRandomizer_(pI);
  }

static pwide_t ALLOW_COLLIDE_BAD_GROUP_d_d_d = L":AllowCollide: invalid collition group %d, must be in [%d..%d]";
static pwide_t DISALLOW_COLLIDE_BAD_GROUP_d_d_d = L":DisallowCollide: invalid collition group %d, must be in [%d..%d]";
HRESULT __stdcall SpriteCraftEngine::AllowCollide(long group1, long group2)
  {
    _SYNCHRONIZE_;
    if ( !collidemgr_ ) return GenSCWideError(ENG_NOTINITIALIZED);
    group1 &= 0x0fff; --group1;
    group2 &= 0x0fff; --group2;
    if ( group1 < 0 || group1 >= CollideMgr::MAX_GROUPS )
      return GenSCWideError(_S*ALLOW_COLLIDE_BAD_GROUP_d_d_d %(group1+1) %1 %(CollideMgr::MAX_GROUPS+1));
    if ( group2 < 0 || group2 >= CollideMgr::MAX_GROUPS )
      return GenSCWideError(_S*ALLOW_COLLIDE_BAD_GROUP_d_d_d %(group2+1) %1 %(CollideMgr::MAX_GROUPS+1));
    collidemgr_->AllowCollide(group1,group2);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::DisallowCollide(long group1, long group2)
  {
    _SYNCHRONIZE_;
    if ( !collidemgr_ ) return GenSCWideError(ENG_NOTINITIALIZED);
    group1 &= 0x0fff; --group1;
    group2 &= 0x0fff; --group2;
    if ( group1 < 0 || group1 >= CollideMgr::MAX_GROUPS )
      return GenSCWideError(_S*DISALLOW_COLLIDE_BAD_GROUP_d_d_d %(group1+1) %1 %CollideMgr::MAX_GROUPS);
    if ( group2 < 0 || group2 >= CollideMgr::MAX_GROUPS )
      return GenSCWideError(_S*DISALLOW_COLLIDE_BAD_GROUP_d_d_d %(group2+1) %1 %CollideMgr::MAX_GROUPS);
    collidemgr_->DisallowCollide(group1,group2);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::SetCollideMgr(IUnknown *mgr)
  {
    _SYNCHRONIZE_;
    if ( !collidemgr_ ) return GenSCWideError(ENG_NOTINITIALIZED);
    collidemgr_->SetListener(mgr);
    return S_OK;
  }

template < class T >
  struct NameIsLess
    {
      bool operator()(const T* a,const T* b) const
        { return wcscmp(a->GetName(),b->GetName()) < 0; }
    };

HRESULT __stdcall SpriteCraftEngine::DumpObjectsList()
  {
    _SYNCHRONIZE_;
    Lout << "";
    Lout << "%%================================================================";
    Lout << "%% Dumping engine state ...";
    Lout << "";
    Lout << "engine: -- SYSTEM INFO --";
    if ( GfxControl* gfxctl = SCREEN::GetControl() )
      gfxctl->LogVmemSize();
      {
        MEMORYSTATUS memstat;
        memstat.dwLength = sizeof(memstat);
        GlobalMemoryStatus(&memstat);
        Lout << _S*"engine: pagefile free %4ldM of %4ldM" %(memstat.dwAvailPageFile/(1024*1024)) %(memstat.dwTotalPageFile/(1024*1024));
        Lout << _S*"engine: memory   free %4ldM of %4ldM" %(memstat.dwAvailPhys/(1024*1024)) %(memstat.dwTotalPhys/(1024*1024));
      }

    Lout << "";
    Lout << "engine: -- OBJECTS LIST --";
    {
      ArrayT<Sprite*> sprites;
      if ( SCE_OK == SPRITES::EnumerateSprites(sprites) )
        {
          Qsort(sprites.Begin(),sprites.End(),NameIsLess<Sprite>());
          Sprite **i = sprites.Begin(), **iE = sprites.End();
          if ( i == iE )
            {
              Lout << "engine: *SPRITES*";
              Lout << "engine:   opps, none sprites";
            }
          Lout << "engine: *USER_SPRITES*";
          for ( ; i != iE; ++i )
            {
              Sprite* s = (*i);
              if ( Adaptate<SpriteHelper>(s->GetSubImpl()) )
                {
                  Lout << _S*"engine: [%s] '%s' of %s at %d,%d layer %d@%d"
                    %((s->IsVisible()&&SPRITES::IsLayerVisible(s->GetLayer())?L"+":L"-"))
                    %s->GetName() %s->GetKlass() %long(s->GetScrX()) %long(s->GetScrY())
                    %s->GetLayer() %s->GetSubLayer();
                }
            }
          Lout << "engine: *INTERNAL_SPRITES*";
          i = sprites.Begin();
          for ( ; i != iE; ++i )
            {
              Sprite* s = (*i);
              if ( !Adaptate<SpriteHelper>(s->GetSubImpl()) )
                {
                  Lout << _S*"engine: [%s] '%s' of %s at %d,%d layer %d@%d"
                    %(s->IsVisible()?L"+":L"-")
                    %s->GetName() %s->GetKlass() %long(s->GetScrX()) %long(s->GetScrY())
                    %s->GetLayer() %s->GetSubLayer();
                }
            }
        }
      else
        Lout << "engine: failed on enumerating sprites";
    }
    {
      if ( sndmgr_ ) sndmgr_->LogSoundsList();
    }
    Lout << "engine: *TILEMAPS*";
    TileMapManager::LogTileMaps();
    {
      Lout << "engine: *PARTICLES*";
      ArrayT<Sprite*> particls;
      if ( SCE_OK == SPRITES::EnumerateSprites(particls) )
        {
          Qsort(particls.Begin(),particls.End(),NameIsLess<Sprite>());
          Sprite **i = particls.Begin(), **iE = particls.End();
          bool has_particles = false;
          for ( ; i != iE; ++i )
            {
              Sprite* p = (*i);
              ParticlesHelper* ph = Adaptate<ParticlesHelper>(p->GetSubImpl());
              if ( ph )
                {
                  Lout << _S*"engine: [%s] '%s' of %s at %d,%d count %d "
                    %(ph->IsActive()?L"+":L"-")
                    %p->GetName() %p->GetKlass()
                    %long(p->GetX()) %long(p->GetY())
                    %ph->Count_();
                }
            }
        }
      else
        Lout << "engine: failed on enumerationg particles";
    }
    {
      if ( executor_ ) executor_->_LogQueues();
    }
    Lout << "";
    Lout << "%% end engine state dump";
    Lout << "%%================================================================";
    Lout << "";
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_layerX(long no, float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SPRITES::GetLayerX(no);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_layerX(long no, float newVal)
  {
    _SYNCHRONIZE_;
    SPRITES::SetLayerX(no,newVal);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_layerY(long no, float *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SPRITES::GetLayerY(no);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_layerY(long no, float newVal)
  {
    _SYNCHRONIZE_;
    SPRITES::SetLayerY(no,newVal);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_layerVisibility(long no, VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = SPRITES::IsLayerVisible(no)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_layerVisibility(long no, VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    SPRITES::SetLayerVisibility(no,newVal!=VARIANT_FALSE);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_channelVolume(long channel, long *pVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_ ) *pVal = sndmgr_->GetChannelVolume(channel);
    else *pVal = 100;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_channelVolume(long channel, long newVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_ ) sndmgr_->SetChannelVolume(channel,newVal);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_channelPan(long channel, long *pVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_ ) *pVal = sndmgr_->GetChannelPan(channel);
    else *pVal = 0;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_channelPan(long channel, long newVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_ ) sndmgr_->SetChannelPan(channel,newVal);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::StopSound(long channel)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_) sndmgr_->StopSound(channel);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_offscreenCollide(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( collidemgr_ ) *pVal = collidemgr_->GetOffscreenCollideStatus()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_offscreenCollide(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( collidemgr_ ) collidemgr_->SetOffscreenCollideStatus(newVal!=VARIANT_FALSE);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_channelMute(long channel,VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_) *pVal = sndmgr_->GetMuteStatus(channel)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_channelMute(long channel,VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_) sndmgr_->Mute(channel,newVal!=VARIANT_FALSE);
    return S_OK;
  }

void SpriteCraftEngine::CheckTimer_(long fps,bool pulsar)
  {
    if ( fixed_fps_ != fps || timer_pulsar_ != pulsar)
      {
        if ( timer_id_ ) timeKillEvent(timer_id_), timer_id_ = 0;
        u32_t fps_timeout = 1000/fps;
        fixed_fps_ = fps;
        timer_pulsar_ = pulsar;
        if ( fps_timeout < 1000 )
          {
            if ( !timer_event_ ) timer_event_ = CreateEvent(0,0,0,0);
            if ( timer_event_ && !timer_id_ )
              timer_id_ = timeSetEvent(fps_timeout,0,(LPTIMECALLBACK)timer_event_,0,
                TIME_PERIODIC|
                (pulsar ? TIME_CALLBACK_EVENT_PULSE : TIME_CALLBACK_EVENT_SET) );
          }
      }
  }

HRESULT __stdcall SpriteCraftEngine::DisplayEx(long fps, long *pVal)
  {
    _SYNCHRONIZE_;
    u32_t millis = GetMillis();
    CheckTimer_(fps,pulse_refresh_);
    if ( timer_id_ ) WaitForSingleObject(timer_event_,1000);
    //if ( FAILED(Update()) ) return E_FAIL;
    if ( FAILED(Display()) ) return E_FAIL;
    *pVal = GetMillis() - millis;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::DisplayExNoWait(long fps, long *pVal)
  {
    _SYNCHRONIZE_;
    u32_t millis = GetMillis();
    CheckTimer_(fps,false);
    if ( timer_id_ && WaitForSingleObject(timer_event_,0) == WAIT_OBJECT_0 )
      {
        if ( FAILED(Display()) ) return E_FAIL;
        *pVal = GetMillis() - millis;
      }
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_invertStereo(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_ ) *pVal = sndmgr_->GetInvertStereoStatus()?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_invertStereo(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( sndmgr_ ) sndmgr_->InvertStereo(newVal!=VARIANT_FALSE);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::IncLayerX(long no, float val)
  {
    _SYNCHRONIZE_;
    SPRITES::SetLayerX(no,SPRITES::GetLayerX(no) + val);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::IncLayerY(long no, float val)
  {
    _SYNCHRONIZE_;
    SPRITES::SetLayerY(no,SPRITES::GetLayerY(no) + val);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_clearColorBits(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    *pVal = clearColorBits_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_clearColorBits(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    clearColorBits_ = newVal!=VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::SetMouseNotifierRange(long nearLayer, long farLayer)
  {
    _SYNCHRONIZE_;
    if ( mlistenmgr_ ) mlistenmgr_->SetMouseNotifierRange(nearLayer,farLayer);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::ResetMouseNotifierRange()
  {
    _SYNCHRONIZE_;
    if ( mlistenmgr_ ) mlistenmgr_->SetMouseNotifierRange(LONG_MIN,LONG_MAX);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::GrabMouse(ISprite *sprite)
  {
    _SYNCHRONIZE_;
    rcc_ptr<IWeakSpriteEx> iwsx;
    if ( sprite )
      {
        rcc_ptr<IWeakSprite> iws;
        sprite->get_weakref(&+iws);
        if ( +iws ) iws->QueryInterface(TEGGO_GUIDOF(IWeakSpriteEx),(void**)&+iwsx);
      }
    if ( mlistenmgr_ ) mlistenmgr_->GrabMouse(+iwsx);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::EnumerateParticles(IParticlesEnumerator **pI)
  {
    _SYNCHRONIZE_;
    *pI = new ParticlesEnumerator();
    return S_OK;
  }

//HRESULT SpriteCraft_ParseXML(pwide_t source, long flags, IXdata** node);
HRESULT __stdcall SpriteCraftEngine::ParseXML(BSTR source, tagParsFlags flags, IXdata **pI)
  {
    return E_FAIL;
  }

HRESULT SpriteCraft_ParseDEF(pwide_t source,long flags,IXdata** node);
HRESULT __stdcall SpriteCraftEngine::ParseDEF(BSTR source, tagParsFlags flags, IXdata **pI)
  {
    return SpriteCraft_ParseDEF(source, flags, pI);
  }

HRESULT SpriteCraft_NewXML(BSTR tag, IXdata **pI);
HRESULT __stdcall SpriteCraftEngine::NewXdata(BSTR tag,struct IXdata **pI)
  {
    return SpriteCraft_NewXML(tag,pI);
  }

HRESULT __stdcall SpriteCraftEngine::SstDefKlass(BSTR klass, IXdata *info)
  {
    _SYNCHRONIZE_;

    rcc_ptr<XdataHelper> helper;
    if ( SUCCEEDED(info->QueryInterface(TEGGO_GUIDOF(XdataHelper),(void**)&+helper)) )
      {
        ResetErrorLog();
        if ( SCE_OK == SST->DefKlass(klass,helper->GetRawNode()) ) return S_OK;
        return GenSCWideError(0,_S*L":SstDefKlass: '%?'" %klass);
      }
    return E_FAIL;
  }

HRESULT __stdcall SpriteCraftEngine::SstUndefKlass(BSTR klass)
  {
    _SYNCHRONIZE_;
    if ( 0 == wcscmp(klass,L"$spritecraft$dummy$")
       ||0 == wcscmp(klass,L"$spritecraft$blank$")
       )
       return GenSCWideError(0,_S*L":SstUndefKlass: klass '%?' is unloadable" %klass);

    if ( SPRITES::HasKlass(klass) )
      SPRITES::UnloadKlass(klass,true);
    ResetErrorLog();
    if ( SCE_OK == SST->UndefKlass(klass) ) return S_OK;
    return GenSCWideError(0,_S*L":SstUndefKlass: '%?'" %klass);
  }

HRESULT __stdcall SpriteCraftEngine::SstFindKlass(BSTR klass, IXdata **pI)
  {
    _SYNCHRONIZE_;

    ResetErrorLog();
    Xnode* node = SST->GetKlass(klass);
    if ( node )
      {
        *pI = new XdataHelper( node->GetDocument(), node );
        return S_OK;
      }
    return GenSCWideError(0,_S*L":SstFindKlass: '%?'" %klass);
  }

HRESULT __stdcall SpriteCraftEngine::get_vSync(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* gfxctl = SCREEN::GetControl() )
      {
        bool value;
        *pVal = (SCE_OK == gfxctl->GetVSync(&value) && value) ? VARIANT_TRUE : VARIANT_FALSE;
      }
    else
      *pVal = VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_vSync(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* gfxctl = SCREEN::GetControl() ) gfxctl->SetVSync(newVal!=VARIANT_FALSE);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_safeDraw(VARIANT_BOOL *pVal)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* gfxctl = SCREEN::GetControl() )
      {
        bool value;
        *pVal = (SCE_OK == gfxctl->GetSafe(&value) && value) ? VARIANT_TRUE : VARIANT_FALSE;
      }
    else
      *pVal = VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_safeDraw(VARIANT_BOOL newVal)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* gfxctl = SCREEN::GetControl() ) gfxctl->SetSafe(newVal!=VARIANT_FALSE);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_console(IConsole** conRet)
  {
    _SYNCHRONIZE_;
    *conRet = Refe(console_);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::NewSurface(
    /*[in]*/ long surfmt,
    /*[in]*/ long width, /*[in]*/ long height, /*[in]*/ long frcount,
    /*[out,retval]*/ /*ISpriteSurf*/ IUnknown** pI)
  {
    _SYNCHRONIZE_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::NewDrawable(/*[out,retval]*/ /*ISpriteDrawable*/ IUnknown** pI)
  {
    _SYNCHRONIZE_;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_wndWidth(long* val)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* ctl = SCREEN::GetControl() )
      if ( SCE_OK == ctl->GetWindowSize(val,0) )
        return S_OK;
    *val = SCREEN::GetWidth();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_wndHeight(long* val)
  {
    _SYNCHRONIZE_;
    if ( GfxControl* ctl = SCREEN::GetControl() )
      if ( SCE_OK == ctl->GetWindowSize(0,val) )
        return S_OK;
    *val = SCREEN::GetHeight();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_scrOffsX(float* pVal)
  {
    _SYNCHRONIZE_;
    *pVal = -SCREEN::OffsetX();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_scrOffsY(float* pVal)
  {
    _SYNCHRONIZE_;
    *pVal = -SCREEN::OffsetY();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_scrOffsX(float x)
  {
    _SYNCHRONIZE_;
    SCREEN::SetOffsetX(-x);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_scrOffsY(float y)
  {
    _SYNCHRONIZE_;
    SCREEN::SetOffsetY(-y);
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::__s0(long,long)  { return GenSCWideError(L"unimplemented"); }
HRESULT __stdcall SpriteCraftEngine::__s1(long,long,long)  { return GenSCWideError(L"unimplemented"); }
HRESULT __stdcall SpriteCraftEngine::__s2(long,long)  { return GenSCWideError(L"unimplemented"); }
HRESULT __stdcall SpriteCraftEngine::__s3(long)       { return GenSCWideError(L"unimplemented"); }
HRESULT __stdcall SpriteCraftEngine::__s4(long,long)  { return GenSCWideError(L"unimplemented"); }
HRESULT __stdcall SpriteCraftEngine::__s5(long,long)  { return GenSCWideError(L"unimplemented"); }

HRESULT __stdcall SpriteCraftEngine::UnloadAll()
  {
    _SYNCHRONIZE_;
    SPRITES::UnloadAll();
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::put_pulseRefresh(VARIANT_BOOL val)
  {
    _SYNCHRONIZE_;
    pulse_refresh_ = val != VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::get_pulseRefresh(VARIANT_BOOL* val)
  {
    _SYNCHRONIZE_;
    *val = pulse_refresh_?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
  }

HRESULT __stdcall SpriteCraftEngine::OpenLog(BSTR logname)
  {
    if ( logname && !*logname ) logname = 0;
    _SetLogging(true,logname);
    return S_OK;
  }


// void *__stdcall SpriteCraftEngine::__python_m_Init(void *args, void *kwds)
//   {
//     int opts;
//     int fullscreen = 0,
//         show = 1,
//         vsync = 0,
//         native_cursor = 0,
//         quality = QualitySmart,
//         wstyle = 0,
//         nodriverinfo = 0,
//         raster_w = 0,
//         raster_h = 0,
//         raster_bpp = 0;
//     if ( _PoC_Args((PyObject*)args,(PyObject*)kwds,
//       "|i|i{fullscreen}i{show}i{vsync}i{nativecursor}i{quality}"
//       //"(ii|i){raster}"
//       "i{nodrvinfo}i{wstyle}",
//       &opts,&fullscreen,&show,&vsync,&native_cursor,&quality,
//       //&raster_w,&raster_h,&raster_bpp,
//       &nodriverinfo,&wstyle
//       ))
//       {
//         if ( nodriverinfo ) opts |= DevDisableDrvInfo;
//         int vmode = 0;
//         if ( SUCCEEDED(InitEx(opts,vmode)) )
//           {
//             return Refe(_PoC_None);
//           }
//         _PoC_RaiseComExcept();
//       }
//     return 0;
//   }
