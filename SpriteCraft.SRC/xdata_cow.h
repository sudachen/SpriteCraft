

#ifndef ___9a7e1bfe_8869_4059_9573_1adf7e93bddf___
#define ___9a7e1bfe_8869_4059_9573_1adf7e93bddf___

struct XDATA_HELPER_COW_ : Refcounted
  {
    u32_t cow_refcnt;
    XDATA_HELPER_COW_ **holder_;
    XDATA_HELPER_COW_(XDATA_HELPER_COW_ **h = 0) : cow_refcnt(1), holder_(h) {}
    ~XDATA_HELPER_COW_() 
      { 
        REQUIRE(holder_ == 0); 
      }
  };

struct UNKNOWN_XDATA_COW_ : InternList<UNKNOWN_XDATA_COW_,XDATA_HELPER_COW_> 
  {
  protected:
    UNKNOWN_XDATA_COW_(); // fake
    ~UNKNOWN_XDATA_COW_(); // fake
  };

struct XDATA_COW_HOLDER_ : XnodeObserver
  {
    typedef GuidT<0x58ffb1d0,0xff01,0x492b,0x8c, 0xe7, 0x2f, 0x90, 0xf7, 0xb1, 0xe8, 0x26> Guid;
    UNKNOWN_XDATA_COW_ *l_;
    XDATA_COW_HOLDER_ **holder_;
    XDATA_COW_HOLDER_(XDATA_COW_HOLDER_ **h) : l_(0), holder_(h) {}
    ~XDATA_COW_HOLDER_(); 
    void UnlinkAll(bool reset_holders = true);
    void Link(UNKNOWN_XDATA_COW_ *p);
    void OnModify(Xnode *, u32_t, pwide_t);
  };

#endif // ___9a7e1bfe_8869_4059_9573_1adf7e93bddf___
