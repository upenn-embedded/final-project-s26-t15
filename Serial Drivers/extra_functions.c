/*
 * extra_functions.c
 *  Created on: 04/12/2026
 *  Author: Seth Lee
 *  Description: provides extra functions
 *
 */

#include <stdint.h>
#include "stm32f4xx.h" // Defines aliases from a header file


void uint_to_str(uint16_t val, char *buf) {
    int i = 0;
    char tmp[6];
    if (val == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    while (val > 0) {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }
    for (int j = 0; j < i; j++) buf[j] = tmp[i - 1 - j];
    buf[i] = '\0';
}

// GPIO setup: PB1, PB2, PB3, and PB4 are used as button inputs
// PB1 = Switch between screens
void setup_GPIO(void){
	// disable interrupts for setup
	__disable_irq();

	//Clocks
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // GPIOA clock enable
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // GPIOB
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // SYSCFG does EXTI routing

	// Configure pins as inputs (PB1-PB4)
	GPIOB->MODER &= ~(3U << 2);  // PB1
	GPIOB->MODER &= ~(3U << 4);  // PB2, etc.
	GPIOB->MODER &= ~(3U << 6);
	GPIOB->MODER &= ~(3U << 8);

	// Enable pull up resistors
	GPIOB->PUPDR |=  (1U << 2);   // PB1 pull-up
	GPIOB->PUPDR &= ~(1U << 3);
	GPIOB->PUPDR |=  (1U << 4);   // PB2 pull-up
	GPIOB->PUPDR &= ~(1U << 5);
	GPIOB->PUPDR |=  (1U << 6);   // PB3 pull-up
	GPIOB->PUPDR &= ~(1U << 7);
	GPIOB->PUPDR |=  (1U << 8);   // PB4 pull-up
	GPIOB->PUPDR &= ~(1U << 9);

	// EXTICR[0] is a MUX for EXTI0–3, EXTICR[1] for EXTI4–7. Port B = 0001 in each 4-bit field.
	// EXTICR[0]: EXTI1 = bits[7:4], EXTI2 = bits[11:8], EXTI3 = bits[15:12]
	SYSCFG->EXTICR[0] |= (1U << 4);   // PB1 -> EXTI1
	SYSCFG->EXTICR[0] |= (1U << 8);   // PB2 -> EXTI2
	SYSCFG->EXTICR[0] |= (1U << 12);  // PB3 -> EXTI3
	// EXTICR[1]: EXTI4 = bits[3:0]
	SYSCFG->EXTICR[1] |= (1U << 0);   // PB4 -> EXTI4

	// Unmask EXTI lines 1–4
	EXTI->IMR |= (1U << 1) | (1U << 2) | (1U << 3) | (1U << 4);

	// Falling edge trigger (button is active low)
	EXTI->FTSR |= (1U << 1) | (1U << 2) | (1U << 3) | (1U << 4);

	// Enable interrupt request in NVIC (which manages interrupts) — EXTI1–4 each have their own dedicated handler
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);

	__enable_irq();
	return;
}
