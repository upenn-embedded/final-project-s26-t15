/*
 * timing.h
 *  Created on: 04/18/2026
 *  Author: Seth Lee
 *  Description: Timing header
 */

#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>
#include "stm32f411xe.h"

void     delay_init(void);          // init SysTick for 1ms ticks — call first in main()
uint32_t get_tick(void);            // returns ms elapsed since delay_init()
void     delay_ms_blocking(uint32_t delay);  // blocking delay, avoid in main loop
void TIM2_init(void);
#endif
