/*
 * rtc.h
 * Author: Seth Lee
 */

#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "stm32f411xe.h"

void rtc_init(void);
void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void rtc_get_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

#endif
