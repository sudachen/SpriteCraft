
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

#if !defined SCRAFT_INTERNAL_MARKER
#error "should be included in scraftapi.h only"
#endif

struct Scraft
  {

    typedef void (__cdecl *GetProgramVer_t)(long* v0123);
    static GetProgramVer_t GetProgramVersion;

    static teggo::StringT<char> GetProgramFolder();

    static void           Init(tagScDevice dev = DevAuto,bool logging=true);
    static void           InitEx(tagScDevice dev,u32_t vmode,bool logging=true);
    static void           Term();
    static bool           NextEvent(bool processMsgQue = true);
    static u32_t        DisplayEx(u32_t millis);
    static u32_t        DisplayExNoWait(u32_t millis);
    static void           Display();
    static void           SetBackground(u32_t color);
    static void           SetTitle(pwide_t title);
    static __ScString     GetTitle();
    static void           PlaceWindowAt(enum tagPosition pos);
    static void           MoveWindowTo(long x,long y);
    static void           Purge();
    static bool           EvtIsQuit();
    static bool           EvtIsESC();
    static bool           EvtIsKeyUp();
    static bool           EvtIsKeyDown();
    static bool           EvtIsMouseMove();
    static bool           EvtIsMouseLeave();
    static long           EvtKeyEx();
    static bool           EvtIsLClick();
    static bool           EvtIsRClick();
    static void           MapKey(tagKeyVal key,tagKeyVal stkey);
    static u32_t        RefreshInput();
    static bool           EvtIsMouseUp();
    static bool           EvtIsMouseDown();
    static bool           EvtIsMouse();
    static bool           IsKeyPressed(tagKeyVal key);
    static bool           IsKeyPressedNow(tagKeyVal key);
    static bool           IsKeyReleasedNow(tagKeyVal key);
    static void           SleepMillis(u32_t millis);
    static void           SuspendGameClock();
    static void           ResumeGameClock();
    static tagEventType   EvtEvent();
    static tagKeyVal      EvtKey();
    static IXdataPtr      ParseXML(pwide_t source,long = ParsDefault);
    static IXdataPtr      ParseDEF(pwide_t source,long = ParsDefault);
//    static IXdataPtr      NewXML(pwide_t tag);
    static IXdataPtr      NewXdata(pwide_t tag);
    static IExecutorPtr   GetExecutor();
    static long           GetMouseX();
    static void           SetMouseX(long val);
    static long           GetMouseY();
    static void           SetMouseY(long val);
    static bool           IsMouseLDown();
    static bool           IsMouseRDown();
    static bool           NeedRefresh();
    static u32_t        GetMillis();
    static bool           IsFullscreen();
    static void           SetFullscreen(bool val);
    static void           ShowFps(bool val);
    static bool           IsMouseIn();
    static void           SetRscPath(pwide_t val);
    static void           SetSST(pwide_t val);
    static long           GetScrWidth();
    static long           GetScrHeight();
    static tagVideoMode   GetVMode();
    static void           SetVMode(/*tagVideoMode*/ u32_t val);
    static long           GetWindowX();
    static void           SetWindowX(long val);
    static long           GetWindowY();
    static void           SetWindowY(long val);
    static long           GetVolume();
    static void           SetVolume(LONG val);
    static bool           IsSoundSupported();
    static bool           IsSoundEnabled();
    static void           SetSoundEnabled(bool val);
    static u32_t        GetFrameTick();
    static u32_t        GetFrameTickDelta();

    static ISpritePtr     NewSprite(pwide_t name,pwide_t klass,long layer =0, long x = 0, long y = 0);
    static ISpritePtr     NewSprite_(pwide_t klass,long layer = 0);
    static ISpritePtr     FindSpriteAtMouse(long nearLayer=LONG_MIN,long farLayer=LONG_MAX);
    static bool           FindSpriteAtMouseEx(long nearLayer,long farLayer,ISpritePtr&);
    static ISpritePtr     FindSprite(pwide_t name);
    static ISpriteEnumPtr EnumerateSprites();
    static ISpriteEnumPtr EnumerateSpritesEx(long nearLayer,long farLayer);
    static ITileMapPtr    NewTileMap(pwide_t name,long columns,long rows,long tileSize,long layer);
    static ITileMapPtr    NewTileMap_(long columns,long rows,long tileSize,long layer);
    static ITileMapPtr    FindTileMap(pwide_t name);
    static ITileMapPtr    NewPredefinedTileMap(pwide_t name,pwide_t pattern,long layer);
    static IParticlesPtr  NewParticles(pwide_t name,pwide_t klass,long layer);
    static IParticlesPtr  NewParticles_(pwide_t klass,long layer);
    static IParticlesPtr  FindParticles(pwide_t);

    static ISpritePtr     NewDummy(pwide_t name=0);
    static void           PlaySound(pwide_t sound,long channel=0,IOnStopSound* listener=0,long cookie=0);
    static void           StopSound(long channel);
    static void           Preload(pwide_t rscname);
    static void           Unload(pwide_t rscname);
    static void           AllowCollide(long group1, long group2);
    static void           DisallowCollide(long group1, long group2);
    static void           SetCollideMgr(IOnCollide* mgr);
    static void           SetCollideMgr(const teggo::rcc_ptr<IOnCollide>& mgr) { SetCollideMgr(+mgr); }
    static void           DumpObjectsList();

    static float          GetLayerX(long no);
    static void           SetLayerX(long no, float val);
    static float          GetLayerY(long no);
    static void           SetLayerY(long no, float val);
    static bool           IsLayerVisible(long no);
    static void           SetLayerVisible(long no,bool val);

    static long           GetChannelVolume(long channel);
    static void           SetChannelVolume(long channel,long val);
    static long           GetChannelPan(long channel);
    static void           SetChannelPan(long channel,long val);
    static bool           IsChannelMute(long channel);
    static void           SetChannelMute(long channel,bool val);

    static bool           IsOffscreenCollideAllowed();
    static void           AllowOffscreenCollide();
    static void           DisallowOffscreenCollide();
    static bool           IsInvertStereo();
    static void           SetInvertStereo(bool val);

    static void           IncLayerX(long no,float val);
    static void           IncLayerY(long no,float val);

    static bool           IsClearColorBits();
    static void           SetClearColorBits(bool val);

    static void           SetMouseNotifierRange(long nearLayer,long farLayer);
    static void           ResetMouseNotifierRange();
    static void           GrabMouse(const ISpritePtr& sprite);
    static void           GrabMouse(ScraftSprite* sprite);

    static __ScString     GetRscPath();
    static __ScString     GetSST();
    static void           Log(pwide_t text);
    static void           Log(const teggo::FormatT<wchar_t>& fmt);

    static void           SstDefKlass(pwide_t klass,const IXdataPtr& info);
    static void           SstUndefKlass(pwide_t klass);
    static IXdataPtr        SstFindKlass(pwide_t klass);

    static long           GetMouseDifX();
    static long           GetMouseDifY();
    static tagMouseMode   GetMouseMode();
    static void           SetMouseMode(tagMouseMode);
    static void           SetVSync(bool);
    static bool           GetVSync();
    static void           SetSafeDraw(bool);
    static bool           GetSafeDraw();
    static long           GetScVersion();

    static ISpritesListPtr NewSpritesList();
    static IRandomizerPtr NewRandomizer();
    static IColorPtr      BackgroundColor();
    static void           SetSoftRefresh(bool);
    static bool           IsSoftRefresh();
    static void           SetNativeCursor(bool);
    static bool           IsNativeCursor();
    static bool           IsShowFps();
    static void           Update();
    static void           Minimize();
    static IConsolePtr    GetConsloe();
    static void           AttachTo( HWND hwnd );
    static long           GetWndWidth();
    static long           GetWndHeight();
    static float          GetScrOffsX();
    static float          GetScrOffsY();
    static void           SetScrOffsX(float);
    static void           SetScrOffsY(float);
    static void           UnloadAll();
    static void           UnloadEx(pwide_t rscname,bool removeit);

    static void           SetPulseRefresh(bool);
    static bool           IsPulseRefresh();
    static HWND           GetHWND();

    static void RegisterGlobalObject( IScriptObject *o, pwide_t name);
    static void RegisterGlobalObject( IScriptObjectPtr const& o, pwide_t name);
    static void UnregisterGlobalObject(pwide_t name);
    static IScriptSpacePtr NewScriptSpace();
    static void ScriptInterrupt( IScriptObject *o );
    static void ScriptInterrupt( IScriptObjectPtr const &o );

    template <class T>
      static void KillAtTerminate(T* t)
        {
          if (t)
            terminators_ = new TerminatorT<T>(t,terminators_);
        }

  private:

    struct Terminator
      {
        Terminator* next_;
        Terminator(Terminator* next) : next_(next) {}
        virtual ~Terminator() {}
      };

    template <class T> struct TerminatorT : Terminator
      {
        T* t_;
        TerminatorT(T* t,Terminator* next) : Terminator(next), t_(t) {}
        virtual ~TerminatorT() { delete t_; }
      };

    static Terminator*    terminators_;
    static IEngine*       engine;
    static HINSTANCE      spritecraft_dll;

  public:

    void SetOffScreenCollide(bool val)
      {
        if ( val )
          AllowOffscreenCollide();
        else
          DisallowOffscreenCollide();
      }

    struct Property_title         { TEGGO_S_PROPERTY(Scraft,SetTitle,GetTitle,title) } title;
    struct Property_vMode         { TEGGO_PROPERTY  (Scraft,tagVideoMode,SetVMode,GetVMode,vMode) } vMode;
    struct Property_windowX       { TEGGO_PROPERTY  (Scraft,long,SetWindowX,GetWindowX,windowX) } windowX;
    struct Property_windowY       { TEGGO_PROPERTY  (Scraft,long,SetWindowY,GetWindowY,windowY) } windowY;
    struct Property_scrWidth      { TEGGO_R_PROPERTY(Scraft,long,GetScrWidth,scrWidth) } scrWidth;
    struct Property_scrHeight     { TEGGO_R_PROPERTY(Scraft,long,GetScrHeight,scrHeight) } scrHeight;
    struct Property_scVersion     { TEGGO_R_PROPERTY(Scraft,long,GetScVersion,scVersion) } scVersion;
    struct Property_fullscreen    { TEGGO_PROPERTY (Scraft,bool,SetFullscreen,IsFullscreen,fullscreen) } fullscreen;
    struct Property_softRefresh   { TEGGO_PROPERTY (Scraft,bool,SetSoftRefresh,IsSoftRefresh,softRefresh) } softRefresh;
    struct Property_pulseRefresh  { TEGGO_PROPERTY (Scraft,bool,SetPulseRefresh,IsPulseRefresh,pulseRefresh) } pulseRefresh;
    struct Property_nativeCursor  { TEGGO_W_PROPERTY (Scraft,bool,SetNativeCursor,nativeCursor) } nativeCursor;
    struct Property_clearColorBits{ TEGGO_PROPERTY(Scraft,bool,SetClearColorBits,IsClearColorBits,clearColorBits) } clearColorBits;
    struct Property_background    { TEGGO_R_PROPERTY_ARROW(Scraft,IColorPtr,BackgroundColor,background) } background;
    struct Property_showFps       { TEGGO_PROPERTY  (Scraft,bool,ShowFps,IsShowFps,showFps) } showFps;
    struct Property_rscPath       { TEGGO_S_PROPERTY(Scraft,SetRscPath,GetRscPath,rscPath) } rscPath;
    struct Property_SST           { TEGGO_S_PROPERTY(Scraft,SetSST,GetSST,SST) } SST;
    struct Property_mouseMode     { TEGGO_PROPERTY  (Scraft,tagMouseMode,SetMouseMode,GetMouseMode,mouseMode) } mouseMode;
    struct Property_mouseIn       { TEGGO_R_PROPERTY(Scraft,bool,IsMouseIn,mouseIn) } mouseIn;
    struct Property_mouseLDown    { TEGGO_R_PROPERTY(Scraft,bool,IsMouseLDown,mouseLDown) } mouseLDown;
    struct Property_mouseRDown    { TEGGO_R_PROPERTY(Scraft,bool,IsMouseRDown,mouseRDown) } mouseRDown;
    struct Property_mouseX        { TEGGO_PROPERTY  (Scraft,long,SetMouseX,GetMouseX,mouseX) } mouseX;
    struct Property_mouseY        { TEGGO_PROPERTY  (Scraft,long,SetMouseY,GetMouseY,mouseY) } mouseY;
    struct Property_mouseDifX     { TEGGO_R_PROPERTY(Scraft,long,GetMouseDifX,mouseDifX) } mouseDifX;
    struct Property_mouseDifY     { TEGGO_R_PROPERTY(Scraft,long,GetMouseDifX,mouseDifY) } mouseDifY;
    struct Property_millis        { TEGGO_R_PROPERTY(Scraft,u32_t,GetMillis,millis) } millis;
    struct Property_frameTick     { TEGGO_R_PROPERTY(Scraft,u32_t,GetFrameTick,frameTick) } frameTick;
    struct Property_frameTickDelta{ TEGGO_R_PROPERTY(Scraft,u32_t,GetFrameTickDelta,frameTickDelta) } frameTickDelta;
    struct Property_supportSound  { TEGGO_R_PROPERTY(Scraft,bool,IsSoundSupported,supportSound) } supportSound;
    struct Property_enableSound   { TEGGO_PROPERTY(Scraft,bool,SetSoundEnabled,IsSoundEnabled,enableSound) } enableSound;
    struct Property_invertStereo  { TEGGO_PROPERTY(Scraft,bool,SetInvertStereo,IsInvertStereo,invertStereo) } invertStereo;
    struct Property_volume        { TEGGO_PROPERTY(Scraft,long,SetVolume,GetVolume,volume) } volume;
    struct Property_vSync         { TEGGO_PROPERTY(Scraft,bool,SetVSync,GetVSync,vSync) } vSync;
    struct Property_safeDraw      { TEGGO_PROPERTY(Scraft,bool,SetSafeDraw,GetSafeDraw,safeDraw) } safeDraw;
    struct Property_executor      { TEGGO_R_PROPERTY_ARROW(Scraft,IExecutorPtr,GetExecutor,executor) } executor;
    struct Property_layerX        { TEGGO_PROPERTY_ARR(Scraft,float,SetLayerX,GetLayerX,layerX) } layerX;
    struct Property_layerY        { TEGGO_PROPERTY_ARR(Scraft,float,SetLayerY,GetLayerY,layerY) } layerY;
    struct Property_channelPan    {TEGGO_PROPERTY_ARR(Scraft,long,SetChannelPan,GetChannelPan,channelPan)} channelPan;
    struct Property_channelVolume {TEGGO_PROPERTY_ARR(Scraft,long,SetChannelVolume,GetChannelVolume,channelVolume)} channelVolume;
    struct Property_channelMute   {TEGGO_PROPERTY_ARR(Scraft,bool,SetChannelMute,IsChannelMute,channelMute)} channelMute;
    struct Property_offScreenCollide { TEGGO_PROPERTY(Scraft,bool,SetOffScreenCollide,IsOffscreenCollideAllowed,offScreenCollide) } offScreenCollide;
    struct Property_console       {TEGGO_R_PROPERTY_ARROW(Scraft,IConsolePtr,GetConsloe,console) } console;
    struct Property_wndWidth      { TEGGO_R_PROPERTY(Scraft,long,GetWndWidth,wndWidth) } wndWidth;
    struct Property_wndHeight     { TEGGO_R_PROPERTY(Scraft,long,GetWndHeight,wndHeight) } wndHeight;
    struct Property_scrOffsX      { TEGGO_PROPERTY(Scraft,float,SetScrOffsX,GetScrOffsX,scrOffsX) } scrOffsX;
    struct Property_scrOffsY      { TEGGO_PROPERTY(Scraft,float,SetScrOffsY,GetScrOffsY,scrOffsY) } scrOffsY;
    struct Property_hwnd          { TEGGO_R_PROPERTY(Scraft,HWND,GetHWND,hwnd) } hwnd;
  //  struct Property_suspended { TEGGO_R_PROPERTY(Scraft,bool,IsSuspended,suspended) } suspended;

  };

template < class tTc > inline
  Sclog_ operator << (Sclog_, teggo::FormatT<tTc> const &fmt)
    {
      Scraft::Log( +teggo::StringT<wchar_t,128>(fmt) );
      return Sclog;
    }
