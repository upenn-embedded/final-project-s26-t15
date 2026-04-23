/*
 * timing.c
 *  Created on: 04/18/2026
 *  Author: Seth Lee
 *  Description: Timing file
 */

#include "timing.h"

/*
 * SysTick
 *
 */
#define CTRL_ENABLE    (1U << 0)
#define CTRL_TICKINT   (1U << 1) // for interrupt (SysTick_Handler)
#define CTRL_CLCKSRC   (1U << 2)
#define CTRL_COUNTFLAG (1U << 16)

#define ONE_MSEC_LOAD 16000  // 16 MHz clock, 16000 cycles = 1ms

// Global tick counter — volatile because SysTick_Handler writes it
// and main context reads it
volatile uint32_t tick_ms = 0;

// Configure SysTick to fire an interrupt every 1ms
void delay_init(void) {
    SysTick->LOAD = ONE_MSEC_LOAD - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = CTRL_ENABLE | CTRL_TICKINT | CTRL_CLCKSRC;
}

// Fires every 1ms automatically — just increment the counter
void SysTick_Handler(void) {
    tick_ms++;
}

// Returns current millisecond count since delay_init()
uint32_t get_tick(void) {
    return tick_ms;
}


/*
 *
 * General Timers
 *
 */


void TIM2_init(void) {
    // Enable TIM2 clock on APB1
    RCC->APB1ENR |= (1 << 0);

    // Set prescaler: divides timer clock by (PSC+1) = 16000
    // Timer clock after prescaler = 16MHz / 16000 = 1kHz
    TIM2->PSC = 15999;

    // counts 0 to ARR, so (ARR+1) = 100 ticks
    //    Period = 100 ticks at 1kHz = 100ms
    TIM2->ARR = 99;

    // Enable Update Interrupt
    TIM2->DIER |= (1 << 0);

    // Enable TIM2 interrupt in NVIC (IRQ 28)
    NVIC->ISER[0] |= (1 << 28);

    // DIRECTION BIT = 0, count up
    TIM2->CR1 &= ~(1 << 4);

    // Start the counter
    TIM2->CR1 |= (1 << 0);

}

