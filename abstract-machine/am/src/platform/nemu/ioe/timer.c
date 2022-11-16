#include <am.h>
#include <nemu.h>
#include <stdio.h>

static uint64_t BOOT_TIME = 0;

void __am_timer_init() {
  volatile uint32_t lo = inl(RTC_ADDR), hi = inl(RTC_ADDR + 4);
  BOOT_TIME = (uint64_t)hi << 32 | lo;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  volatile uint32_t lo = inl(RTC_ADDR), hi = inl(RTC_ADDR + 4);
  //if (BOOT_TIME == 0) { __am_timer_init(); }
  uptime->us = ((uint64_t)hi << 32 | lo) - BOOT_TIME;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
