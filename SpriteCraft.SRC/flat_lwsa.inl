
template<class T> struct LwsArraySprite;

template<class T>
struct LwsArray_WRAPPER : public SpriteSubImpl, private refbase {
  virtual void SetSpritePtr(Sprite* spr) { }
  virtual void Release_() const { refbase::Release_(); }
  virtual void Grab_() const { refbase::Grab_(); }
  virtual void* Adaptate_(const void* id);
  LwsArray_WRAPPER(LwsArraySprite<T>* impl) : impl_(impl) {}
  virtual ~LwsArray_WRAPPER();
  void ZeroImpl() {  impl_ = 0;  }
private:
  LwsArraySprite<T>* impl_;
};

template<class T> 
struct LwsArraySprite : public T, /*private*/ public LwsArrayBasic
{
  typedef LwsArray_WRAPPER<T> Wrapper;
  typedef LWSA_MICROSPRITE MICROSPRITE;
  Wrapper* wrapper_;
  Sprite* spr_;
  void Init(Sprite* spr);

  //ComplexSprite* GetComplexSprite() { return LwsArrayBasic::GetComplexSprite(); }
  using LwsArrayBasic::GetComplexSprite;
  MICROSPRITE*   GetRange(unsigned* count) { return LwsArrayBasic::LwsGetRange(count); }

  LwsArraySprite() {}
  virtual ~LwsArraySprite() { if ( wrapper_ ) wrapper_->ZeroImpl(); }

  virtual SCERROR ChangeKlass(pwide_t klass) { return spr_->ChangeKlass(klass); }
  virtual pwide_t GetKlass() const { return spr_?spr_->GetKlass():L"<dead_sprites_range>";}
  virtual void    SetX(float x) { spr_->MoveX(x); }
  virtual void    SetY(float y) { spr_->MoveY(y); }
  virtual void    SetXScale(float perc) { spr_->ScaleX(perc); }
  virtual void    SetYScale(float perc) { spr_->ScaleY(perc); }
  virtual void    SetAngle(float radians) { spr_->SetAngle(radians); }
  virtual void    SetLuminace(float perc) { spr_->SetLuminance(perc); }
  virtual void    SetTransparency(float perc) { spr_->SetTransparency(perc); }
  virtual void    SetRGB(mrgb_t c) { spr_->SetRGB(c); }
  virtual float   GetX() const { return spr_->GetX(); }
  virtual float   GetY() const { return spr_->GetY(); }
  virtual float   GetXScale() const { return spr_->GetScaleX(); }
  virtual float   GetYScale() const { return spr_->GetScaleY(); }
  virtual float   GetAngle()  const { return spr_->GetAngle(); }
  virtual float   GetLuminace() const { return spr_->GetLuminance(); }
  virtual float   GetTransparency() const { return spr_->GetTransparency(); }
  virtual mrgb_t  GetRGB() const { return spr_->GetRGB(); }
  virtual void    SetParent(Sprite* s) { spr_->SetParent(s); }
  virtual Sprite* GetParent() const { return spr_->GetParent(); }
  virtual u32_t GetParentDeps() const { return spr_->GetParentDeps(); }
  virtual void    SetParentDeps(u32_t d) { spr_->SetParentDeps(d); }
  virtual bool    IsVisible() const { return spr_->IsVisible(); }
  virtual void    SetVisible(bool v) { if ( v ) spr_->Show(); else spr_->Hide(); }
  virtual long    GetLayer() { return spr_->GetLayer(); }
  virtual long    GetSubLayer() { return spr_->GetSubLayer(); }
  virtual void    SetLayer(long layer) { spr_->SetLayer(layer); }
  virtual void    SetSubLayer(long layer) { spr_->SetSubLayer(layer); }
	virtual void    Reset();

  virtual SCERROR Resize(long count) { return LwsArrayBasic::LwsResize(count); }
  
  virtual SCERROR Show(long no,long count,bool on)    { return LwsArrayBasic::LwsShow(no,count,on); }
  virtual SCERROR GetOneXY(long no,float* x,float* y) { return LwsArrayBasic::LwsGetOneXY(no,x,y); }
  virtual SCERROR GetOneAngle(long no,float* angle)   { return LwsArrayBasic::LwsGetOneAngle(no,angle); }
  virtual SCERROR GetOneScale(long no,float* scale)   { return LwsArrayBasic::LwsGetOneScale(no,scale); }
  virtual SCERROR GetOneFrno(long no,long* frno)      { return LwsArrayBasic::LwsGetOneFrno(no,frno); }
  virtual SCERROR GetOneTransparency(long no,float* transparency) { return LwsArrayBasic::LwsGetOneTransparency(no,transparency); }
  
  virtual SCERROR ScaleRangeDif(long no,long count,float dif) { return LwsArrayBasic::LwsScaleRangeDif(no,count,dif); }
  virtual SCERROR BlendRangeDif(long no,long count,float dif) { return LwsArrayBasic::LwsBlendRangeDif(no,count,dif); }
  virtual SCERROR AngleRangeDif(long no,long count,float dif) { return LwsArrayBasic::LwsAngleRangeDif(no,count,dif); }
  virtual SCERROR FrnoRangeDif(long no,long count,long dif)   { return LwsArrayBasic::LwsFrnoRangeDif(no,count,dif); }
  virtual SCERROR MoveRangeDif(long no,long count,float difX,float difY) { return LwsArrayBasic::LwsMoveRangeDif(no,count,difX,difY); }
  
  virtual SCERROR SetRangeXY(long no,long count,float x,float y)  { return LwsArrayBasic::LwsSetRangeXY(no,count,x,y); }
  virtual SCERROR SetRangeAngle(long no,long count,float angle)   { return LwsArrayBasic::LwsSetRangeAngle(no,count,angle); }
  virtual SCERROR SetRangeScale(long no,long count,float scale)   { return LwsArrayBasic::LwsSetRangeScale(no,count,scale); }
  virtual SCERROR SetRangeFrno(long no,long count,long frno)      { return LwsArrayBasic::LwsSetRangeFrno(no,count,frno); }
  virtual SCERROR SetRangeIndep(long no,long count,bool indep)    { return LwsArrayBasic::LwsSetRangeIndep(no,count,indep); }
  virtual SCERROR SetRangeTransparency(long no,long count,float transparency) { return LwsArrayBasic::LwsSetRangeTransparency(no,count,transparency); }
  
  virtual SCERROR CopyRange(long dst,long src,long count) { return LwsArrayBasic::LwsCopyRange(dst,src,count); }
  virtual long        Count() { return LwsArrayBasic::LwsCount(); }
};

template<class T>
void* LwsArray_WRAPPER<T>::Adaptate_(const void* id) {
  if ( id == (const void*)ComplexSprite::TypeID__ ) return impl_?impl_->GetComplexSprite():0;
  return IRefObject::Adaptate_(id);
}

template<class T>
LwsArray_WRAPPER<T>::~LwsArray_WRAPPER() {
  if ( impl_ ) { 
    impl_->spr_ = 0;
    impl_->wrapper_ = 0;
  }
}

template<class T>
void LwsArraySprite<T>::Init(Sprite* spr)
{
  spr_ = spr;
  auto_ref<Wrapper> wrapper (new Wrapper(this));
  spr->SetSubImpl(wrapper);
  wrapper_ = wrapper;
}

template<class T>
void LwsArraySprite<T>::Reset()
{
  spr_->Show();
  this->Show(0,range_.size(),0);
}
