

#include "stdafx.h"
#include "Engine_timer.h"

void EnTimer_::Start()
  {
    SetUp_();
  }


bool EnTimer_::Suspended()
  {
    return suspeneded_;
  }

void EnTimer_::Suspend()
  {
    if ( !suspeneded_ ) 
      {
        last_millis_on_frame_ = 0;
        suspended_tick_ = GetMillis(); 
        suspeneded_ = true;
      }
  }

void EnTimer_::Resume()
  {
    if ( suspeneded_ ) 
      {
        last_millis_on_frame_ = 0;
        suspended_out_ += GetMillis()-suspended_tick_; 
        suspeneded_ = false;
      }
  }

u32_t EnTimer_::MillisDelta_OnFrame()
  {
    if ( millis_on_frame_need_refresh_ )
      SetMillis_OnFrame_();
    return millis_on_frame_delta_;
  }

u32_t EnTimer_::GetMillis_OnFrame()
  {
    if ( millis_on_frame_need_refresh_ )
      SetMillis_OnFrame_();
    return millis_on_frame_;
  }

void EnTimer_::EndFrame()
  {
    millis_on_frame_need_refresh_ = 1;
  }


void EnTimer_::SetMillis_OnFrame_()
  { 
    u32_t msf = millis_on_frame_;
    millis_on_frame_need_refresh_ = false;
    if ( !suspeneded_ ) 
      {
        u32_t millis = GetMillis();
        if ( last_millis_on_frame_ && millis-last_millis_on_frame_ > 250 ) 
          suspended_out_ += millis-last_millis_on_frame_;
        millis_on_frame_ = millis-suspended_out_/*-millis_start_tick*/; 
        last_millis_on_frame_ = millis;
        REQUIRE(millis_on_frame_ < 0x80000000UL);
      }
    millis_on_frame_delta_ = millis_on_frame_ - msf;
  }

void EnTimer_::SetUp_() 
  { 
    millis_start_tick_ = GetMillis(); 
    suspended_tick_ = 0;
    suspended_out_ = 0;
    last_millis_on_frame_ = 0;
    SetMillis_OnFrame_();
  }
