

#if !defined __3C9EF47C_629A_44CE_B6AF_AC4487B19B77__
#define __3C9EF47C_629A_44CE_B6AF_AC4487B19B77__


struct EnTimer_
  {

    u32_t millis_start_tick_;
    u32_t millis_on_frame_;
    u32_t millis_on_frame_delta_;
    u32_t last_millis_on_frame_;
    u32_t suspended_out_;
    u32_t suspended_tick_;
    bool suspeneded_;
    bool millis_on_frame_need_refresh_;

    EnTimer_() :
      millis_start_tick_(0),
      millis_on_frame_(0),
      millis_on_frame_delta_(0),
      millis_on_frame_need_refresh_(true),
      last_millis_on_frame_(0),
      suspended_out_(0),
      suspended_tick_(0),
      suspeneded_(false)
      {}

    ~EnTimer_() {}

    void Start();
    void Suspend();
    bool Suspended();
    void Resume();
    u32_t GetMillis_OnFrame();
    u32_t MillisDelta_OnFrame();
    void EndFrame();

    void SetMillis_OnFrame_();
    void SetUp_();
  };

#endif // __3C9EF47C_629A_44CE_B6AF_AC4487B19B77__