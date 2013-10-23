
#if !defined ___529beb67_eac8_499b_acda_b2b047c9265c___
#define ___529beb67_eac8_499b_acda_b2b047c9265c___

struct SST_ 
  {
    Xnode*  GetKlass(pwide_t klass);
    SCERROR UndefKlass(pwide_t klass);
    SCERROR DefKlass(pwide_t klass,Xnode* node);
    SCERROR Load(pwide_t source,u32_t flags);
    
    struct Gate
      {
        SST_ *operator->()
          {
            static SST_ sst;
            return &sst;
          }
      };

    SST_();
    ~SST_();

    Xdocument doc_;
    CollectionT<Xnode*> klasses_;
    CollectionT<Xnode*> base_klasses_;
  };

static SST_::Gate SST;

#endif // ___529beb67_eac8_499b_acda_b2b047c9265c___
