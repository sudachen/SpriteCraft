
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

#if !defined __SpriteCraft_Impl_Engine_h__
#define __SpriteCraft_Impl_Engine_h__

#include "RBGFilter.h"
#include "Engine_timer.h"

struct Console;
struct CollideMgr;
struct AnimationMgr;
struct MListenMgr;
struct SoundMgr;
struct Engine_DInput;

extern HRESULT __stdcall CreateEngine_(IEngine **pI);

class SpriteCraftEngine : public
  TeggoPyDispImpl< IEngine, 0 /*CreateEngine_*/,
       TeggoIfs< IColorHelper, TeggoISupportErrorInfo > >
  {
public:
    SpriteCraftEngine();
    ~SpriteCraftEngine();

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);

    mrgb_t fill_color_;

    virtual mrgb_t IColorHelper_GetColor() const;
    virtual void IColorHelper_SetColor(mrgb_t c);

    HRESULT __stdcall get_console(/*[out, retval]*/ IConsole **pVal);
    HRESULT __stdcall get_safeDraw(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_safeDraw(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_vSync(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_vSync(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_mouseDifY(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall get_mouseDifX(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall get_mouseMode(/*[out, retval]*/ enum tagMouseMode *pVal);
    HRESULT __stdcall put_mouseMode(/*[in]*/ enum tagMouseMode newVal);
    HRESULT __stdcall SstFindKlass(/*[in]*/ BSTR klass,/*[out,retval]*/ IXdata **pI);
    HRESULT __stdcall SstUndefKlass(/*[in]*/ BSTR klass);
    HRESULT __stdcall SstDefKlass(/*[in]*/ BSTR klass,/*[in]*/ IXdata *info);
    HRESULT __stdcall SstLoad(BSTR source, long flags);
  //  HRESULT __stdcall NewXML(/*[in]*/ BSTR tag,/*[out,retval]*/ IXdata **pI);
    HRESULT __stdcall ParseDEF(/*[in]*/ BSTR source, tagParsFlags flags, /*[out,retval]*/ IXdata **pI);
  //  HRESULT __stdcall SetSpriteKlassData(/*[in]*/ BSTR name,/*[in]*/ long *img,/*[in]*/ long width,/*[in]*/ long height,/*[in]*/ long rowlen,/*[in]*/ long cols,/*[in]*/ long rows,/*[in]*/ enum tagBlendMode,/*[in,defaultvalue(0)]*/ ISpriteKlassDataUpdater *updater);
    HRESULT __stdcall EnumerateParticles(/*[out,retval]*/ IParticlesEnumerator **pI);
    //
    HRESULT __stdcall ParseXML(/*[in]*/ BSTR source, tagParsFlags flags,/*[out,retval]*/ IXdata **pI);
    HRESULT __stdcall GrabMouse(/*[in]*/ ISprite *sprite);
    HRESULT __stdcall ResetMouseNotifierRange();
    HRESULT __stdcall SetMouseNotifierRange(/*[in]*/ long nearLayer,/*[in]*/ long farLayer);
    HRESULT __stdcall get_clearColorBits(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_clearColorBits(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall IncLayerY(long no,/*[in]*/ float val);
    HRESULT __stdcall IncLayerX(long no,/*[in]*/ float val);
    HRESULT __stdcall get_invertStereo(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_invertStereo(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall DisplayEx(/*[in]*/ long millis,/*[out,retval]*/ long *pVal);
    HRESULT __stdcall DisplayExNoWait(/*[in]*/ long millis,/*[out,retval]*/ long *pVal);
    HRESULT __stdcall get_channelMute(long channel,/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_channelMute(long channel,/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_offscreenCollide(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_offscreenCollide(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall StopSound(/*[in]*/ long channel);
    HRESULT __stdcall get_channelPan(long channel, /*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_channelPan(long channel, /*[in]*/ long newVal);
    HRESULT __stdcall get_channelVolume(long channel, /*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_channelVolume(long channel, /*[in]*/ long newVal);
    HRESULT __stdcall get_layerVisibility(long no, /*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_layerVisibility(long no, /*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_layerY(long no, /*[out, retval]*/ float *pVal);
    HRESULT __stdcall put_layerY(long no, /*[in]*/ float newVal);
    HRESULT __stdcall get_layerX(long idx, /*[out, retval]*/ float *pVal);
    HRESULT __stdcall put_layerX(long idx, /*[in]*/ float newVal);
    HRESULT __stdcall DumpObjectsList();
    HRESULT __stdcall SetCollideMgr(/*[in]*/ IUnknown *mgr);
    HRESULT __stdcall DisallowCollide(/*[in]*/ long group1, /*[in]*/ long group2);
    HRESULT __stdcall AllowCollide(/*[in]*/ long group1, /*[in]*/ long group2);
    HRESULT __stdcall NewRandomizer(/*[out,retval]*/ IRandomizer **pI);
    HRESULT __stdcall NewSpritesList(/*[out,retval]*/ ISpritesList **pI);
    HRESULT __stdcall Update();
    HRESULT __stdcall Unload(/*[in]*/ BSTR rscname);
    HRESULT __stdcall UnloadEx(/*[in]*/ BSTR rscname,VARIANT_BOOL removeit);
    HRESULT __stdcall Preload(/*[in]*/ BSTR name);
    HRESULT __stdcall PlaySound(/*[in]*/ BSTR sound, /*[in,defaultvalue(0)]*/ long channel, /*[in,defaultvalue(0)]*/ IUnknown *notifier,long cookie);
    HRESULT __stdcall get_scVersion(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall get_wndframe(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_wndframe(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_HWND(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall NewPredefinedParticles(/*[in]*/ BSTR name,/*[in]*/ BSTR pattern,/*[in]*/ long layer,/*[out,retval]*/ IParticles **pI);
    HRESULT __stdcall get_background(/*[out, retval]*/ IRGBFilter **pVal);
    HRESULT __stdcall NewDummy(/*[in]*/ BSTR name,/*[out,retval]*/ ISprite **pI);
    HRESULT __stdcall __s4(long,long);
    HRESULT __stdcall __s5(long,long);
    HRESULT __stdcall EvtIsQuit(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsESC(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsMouse(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsMouseUp(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsMouseDown(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsKeyUp(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsKeyDown(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsKeyChar(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsMouseMove(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsMouseLeave(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtKey(tagKeyVal *key);
    HRESULT __stdcall EvtEvent(tagEventType *key);
    HRESULT __stdcall EvtKeyEx(long *keyval);
    HRESULT __stdcall EvtKeyChar_(wchar_t *key);
    HRESULT __stdcall EvtKeyChar(BSTR *key);
    HRESULT __stdcall EvtIsLClick(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtIsRClick(VARIANT_BOOL *pVal);
    HRESULT __stdcall IsKeyReleasedNow(tagKeyVal key,VARIANT_BOOL *pVal);
    HRESULT __stdcall IsKeyPressedNow(tagKeyVal key,VARIANT_BOOL *pVal);
    HRESULT __stdcall IsKeyPressed(tagKeyVal key,VARIANT_BOOL *pVal);
    HRESULT __stdcall MapKey(tagKeyVal,tagKeyVal);
    HRESULT __stdcall RefreshInput(long *state);
    HRESULT __stdcall __s0(long,long);
    HRESULT __stdcall __s1(long,long,long);
    HRESULT __stdcall __s2(long,long);
    HRESULT __stdcall __s3(long);
    HRESULT __stdcall get_suspended(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall ResumeGameClock();
    HRESULT __stdcall SuspendGameClock();
    HRESULT __stdcall get_frameTick(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall get_frameTickDelta(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall FindParticles(/*[in]*/ BSTR name,/*[out,retval]*/ IParticles **pI);
    HRESULT __stdcall NewParticles_(/*[in]*/ BSTR klass,/*[in]*/ long layer,/*[out,retval]*/ IParticles **pI);
    HRESULT __stdcall NewParticles(/*[in]*/ BSTR name,/*[in]*/ BSTR klass,/*[in]*/ long layer,/*[out,retval]*/ IParticles **pI);
    HRESULT __stdcall UnMapAllEvents();
    HRESULT __stdcall UnMapEvent(/*[in]*/ tagEventType etype,/*[in]*/ tagKeyVal keyval);
    HRESULT __stdcall MapEvent(/*[in]*/ tagEventType,/*[in]*/ tagKeyVal,/*[in]*/ long uevent);
    HRESULT __stdcall get_executor(/*[out, retval]*/ IExecutor **pVal);
    HRESULT __stdcall NewPredefinedTileMap(/*[in]*/ BSTR name,/*[in]*/ BSTR klass, /*[in]*/ long layer,/*[out,retval]*/ ITileMap **pI);
    HRESULT __stdcall NewTileMap_(/*[in]*/ long rows, /*[in]*/ long columns,/*[in]*/ long tileSize,/*[in]*/ long layer,/*[out,retval]*/ ITileMap **pI);
    HRESULT __stdcall NewTileMap(/*[in]*/ BSTR map_name, /*[in]*/ long rows, /*[in]*/ long columns,/*[in]*/ long tileSize,/*[in]*/ long layer,/*[out,retval]*/ ITileMap **pI);
    HRESULT __stdcall get_enableSound(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_enableSound(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_supportSound(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall get_volume(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_volume(/*[in]*/ long newVal);
    HRESULT __stdcall PlaceWindowAt(/*[in]*/ enum tagPosition);
    HRESULT __stdcall get_windowY(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_windowY(/*[in]*/ long newVal);
    HRESULT __stdcall get_windowX(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_windowX(/*[in]*/ long newVal);
    HRESULT __stdcall LocateWindow(/*[out]*/ long *x,/*[out]*/ long *y);
    HRESULT __stdcall MoveWindowTo(/*[in]*/ long x,/*[in]*/ long y);
    HRESULT __stdcall EnumerateSpritesEx(long nlayer,long flayer,/*[out,retval]*/ ISpritesEnumerator **pI);
    HRESULT __stdcall EnumerateSprites(/*[out,retval]*/ ISpritesEnumerator **pI);
    HRESULT __stdcall get_vMode(/*[out, retval]*/ enum tagVideoMode *pVal);
    HRESULT __stdcall put_vMode(/*[in]*/ enum tagVideoMode newVal);
    HRESULT __stdcall get_scrHeight(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall get_scrWidth(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_SST(/*[in]*/ BSTR newVal);
    HRESULT __stdcall get_softRefresh(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_softRefresh(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall IsSprite(/*[in]*/ IDispatch *pI,/*[out,retval]*/ VARIANT_BOOL *bRet);
    HRESULT __stdcall put_nativeCursor(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall FindSpriteAtMouse(/*[in]*/ long nearlayer,/*[in]*/ long farlayer,/*[out,retval]*/ ISprite **pVal);
    HRESULT __stdcall FindSpriteAtMouseEx(/*[in]*/ long nearlayer,/*[in]*/ long farlayer,/*[out,retval]*/ ISprite **pVal,VARIANT_BOOL *bRet);
    HRESULT __stdcall get_mouseIn(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall get_title(/*[out, retval]*/ BSTR *pVal);
    HRESULT __stdcall put_title(/*[in]*/ BSTR newVal);
    HRESULT __stdcall get_logging(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_logging(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall Log(/*[in]*/ BSTR text);
    HRESULT __stdcall get_showFps(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_showFps(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_rscpath(/*[out, retval]*/ BSTR *pVal);
    HRESULT __stdcall put_rscpath(/*[in]*/ BSTR newVal);
    HRESULT __stdcall get_fullscreen(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall put_fullscreen(/*[in]*/ VARIANT_BOOL newVal);
    HRESULT __stdcall get_mouseRDown(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall get_mouseLDown(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall get_mouseY(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_mouseY(/*[in]*/ long newVal);
    HRESULT __stdcall get_mouseX(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall put_mouseX(/*[in]*/ long newVal);
    HRESULT __stdcall get_millis(/*[out, retval]*/ long *pVal);
    HRESULT __stdcall get_needRefresh(/*[out, retval]*/ VARIANT_BOOL *pVal);
    HRESULT __stdcall Purge();
    HRESULT __stdcall SleepMillis(/*[in]*/ long millis);
    HRESULT __stdcall NextEvent(/*[out,retval]*/ VARIANT_BOOL *bRet);
    HRESULT __stdcall Display();
    HRESULT __stdcall InitEx(/*[in]*/ tagScDevice devno,tagVideoMode vmode);
    HRESULT __stdcall Init(/*[in]*/ tagScDevice devno);
    //void   *__stdcall __python_m_Init(void *args, void *kwd);
    HRESULT __stdcall FindTileMap(/*[in]*/ BSTR tmap_name,/*[out,retval]*/ ITileMap **spr);
    HRESULT __stdcall FindTileMap_(/*[in]*/ BSTR tmap_name,/*[out,retval]*/ ITileMap **spr, VARIANT_BOOL *bRet);
    HRESULT __stdcall FindSprite(/*[in]*/ BSTR spr_name,/*[out,retval]*/ ISprite **spr);
    HRESULT __stdcall FindSprite_(/*[in]*/ BSTR spr_name,/*[out,retval]*/ ISprite **spr, VARIANT_BOOL *bRet);
    HRESULT __stdcall NewSprite_(/*[in]*/ BSTR klass,/*[in]*/ long layer,/*[out,retval]*/ ISprite **pI);
    HRESULT __stdcall NewSprite(/*[int]*/ BSTR spr_name,/*[in]*/ BSTR klass_name,/*[in]*/ long layer,/*[in]*/ int x, /*[in]*/ int y,/*[out,retval]*/ ISprite **bRet);
    HRESULT __stdcall NewSurface(
        /*[in]*/ long surfmt,
        /*[in]*/ long width, /*[in]*/ long height, /*[in]*/ long frcount,
        /*[out,retval]*/ /*ISpriteSurf*/ IUnknown **pI);
    HRESULT __stdcall NewDrawable(/*[out,retval]*/ /*ISpriteDrawable*/ IUnknown **pI);
    HRESULT __stdcall NextEventEx(VARIANT_BOOL processMsgQue,/*[out,retval]*/ VARIANT_BOOL *bRet);
    HRESULT __stdcall AttachTo( long wnd );
    HRESULT __stdcall get_wndWidth(long*);
    HRESULT __stdcall get_wndHeight(long*);

    HRESULT __stdcall get_scrOffsX(float*);
    HRESULT __stdcall get_scrOffsY(float*);
    HRESULT __stdcall put_scrOffsX(float);
    HRESULT __stdcall put_scrOffsY(float);
    HRESULT __stdcall UnloadAll();

    HRESULT __stdcall put_pulseRefresh(VARIANT_BOOL val);
    HRESULT __stdcall get_pulseRefresh(VARIANT_BOOL *val);
    HRESULT __stdcall NewXdata(BSTR,struct IXdata **);
    HRESULT NewSprite_Internal(pwide_t spr, pwide_t klass, int layer, int x, int y, ISprite **pI);
    HRESULT __stdcall OpenLog(BSTR logname);

    HRESULT __stdcall get_windowIsActive(VARIANT_BOOL *rVal);
    HRESULT __stdcall EvtWindowBecomeActive(VARIANT_BOOL *pVal);
    HRESULT __stdcall EvtWindowBecomeInactive(VARIANT_BOOL *pVal);
    HRESULT __stdcall PlaySoundLoop(/*[in]*/ BSTR sound, /*[in,defaultvalue(0)]*/ long channel, /*[in,defaultvalue(0)]*/ IUnknown *notifier,long cookie);
    HRESULT __stdcall NextSound(/*[in]*/ BSTR sound,/*[in,defaultvalue(0)]*/ long channel,/*[in,defaultvalue(0)]*/ long cookie);

//    HRESULT __stdcall NewScriptSpace(IScriptSpace **pI);
//    HRESULT __stdcall RegisterGlobalObject( IScriptObject *o, BSTR name );
//    HRESULT __stdcall UnregisterGlobalObject( BSTR name );
//    HRESULT __stdcall ScriptInterrupt(IScriptObject *o);


    widestring rsc_path_;
    widestring sst_name_;

    rcc_ptr<IExecutor> executor_;
    rcc_ptr<Console>   console_;

    typedef SlistT< rcc_ptr<IOnUpdateListener> > ONUPDATE_LISTENER_SET;

    mem_ptr<ONUPDATE_LISTENER_SET> on_update_set_;

    mem_ptr<CollideMgr>   collidemgr_;
    mem_ptr<AnimationMgr> animamgr_;
    mem_ptr<MListenMgr>   mlistenmgr_;
    mem_ptr<SoundMgr>     sndmgr_;
    mem_ptr<Engine_DInput> dinput_;

    HANDLE timer_event_;
    u32_t timer_id_;
    long fixed_fps_;

    EVENT event_;
    tagMouseMode  mouse_mode_;
    long mouse_X_;
    long mouse_Y_;
    long mouse_DifX_;
    long mouse_DifY_;
    long last_mbright_down_;
    long last_mbleft_down_;
    VARIANT_BOOL mbleft_down_;
    VARIANT_BOOL mbright_down_;

    u32_t last_display_millis_;
    u32_t last_millis_on_frame_;
    u32_t last_update_frametick_;

    bool mouse_in_widow_;
    bool update_was_called_;
    bool update_was_called_after_next_event_;
    bool show_fps_;
    bool soft_refresh_;
    bool successful_initialized_;
    bool timer_pulsar_;
    bool pulse_refresh_;
    bool clearColorBits_;
    bool alive_;

    tagKeyVal keyb_st_up_;
    tagKeyVal keyb_st_right_;
    tagKeyVal keyb_st_down_;
    tagKeyVal keyb_st_left_;
    tagKeyVal keyb_st_fire_;
    tagKeyVal keyb_st_alter_;
    tagKeyVal keyb_st_mode_;
    tagKeyVal keyb_st_esc_;

    bool keys_state_[256];
    bool keys_state_last_[256];

    void ScClearInput();
    void ScRefreshInput();
    bool ScKeyIsPressed(tagKeyVal kv);
    bool ScKeyIsPressedNow(tagKeyVal kv);
    bool ScKeyIsReleasedNow(tagKeyVal kv);

    void __Terminate();
    void InitDefaults_();
    void InitAttributes_();
    void Time_SetZeroValues_();
    void ChangePriority_();
    void CheckTimer_(long fps,bool pulsar);
    void TranslateKeyVal_(tagKeyVal *kv);

    HRESULT SetMousePos_(long x, long y);

    EnTimer_ timer_;
  };

extern SpriteCraftEngine *g_current_engine;

#endif // __SpriteCraft_Impl_Engine_h__
