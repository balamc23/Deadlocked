#ifndef RTC_H
#define RTC_H

#include "i8259.h"
#include "lib.h"

#define RTC_PORT  0x70
#define CMOS_PORT 0x71
#define STATUS_REGISTER_A 0x8A
#define STATUS_REGISTER_B 0x8B
#define STATUS_REGISTER_C 0x0C
#define RTC_RATE_1024 0x6		// 32768 << (6 - 1) = 1024
#define RTC_VEC 0x08
#define DEFAULT_RATE 0x0F

/* initializes rtc */
void rtc_init();
/* handler code for rtc */
void rtc_handler();

/* code called to change frequency */
int rtc_read(int32_t fdr, void* buf, int32_t nbytes);
int rtc_write(int32_t fd, const void* buf, int32_t nbytes);

/* other system calls */
int rtc_open(const int8_t * filename);
int rtc_close(int32_t fd);

#endif
