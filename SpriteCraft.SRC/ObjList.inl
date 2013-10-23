
struct ObjListBase : 
    TeggoDispImpl<IObjList,TeggoISupportErrorInfo>
  {
    typedef SlistT< IUnknownPtr > OBJLST;
    
    OBJLST objects_;
    OBJLST::Position curr_;
    
    struct 
      {
        bool started_ :1; 
        bool removed_ :1;
      };

    HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid);
    ObjListBase();
    ~ObjListBase();

    HRESULT __stdcall get_count(long* count);
    HRESULT __stdcall IsBegin(IUnknown* pI,VARIANT_BOOL* ret);
    HRESULT __stdcall IsEnd(IUnknown* pI,VARIANT_BOOL* ret);
    HRESULT __stdcall AtBegin(IUnknown** pI);
    HRESULT __stdcall AtEnd(IUnknown** pI);
    HRESULT __stdcall At(long idx,IUnknown** pI);
	  HRESULT __stdcall Clear();
    HRESULT __stdcall Purge() { return Clear(); }
	  HRESULT __stdcall Find(IUnknown* pI, VARIANT_BOOL* pVal);
	  HRESULT __stdcall Add(IUnknown* pI);
	  HRESULT __stdcall Remove();
	  HRESULT __stdcall Get(IUnknown** pI);
	  HRESULT __stdcall Next(VARIANT_BOOL* pVal);
	  HRESULT __stdcall Reset(VARIANT_BOOL* pVal);
  };
