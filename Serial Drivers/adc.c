/*
 * adc.c
 *
 * Created: 4/9/2026 1:03:45 PM
 *  Author: Jerry Zhang
 */ 

#include "stm32f4xx.h" // Defines aliases
#include "adc.h"

void adc_init(void) {
// 1. Enable ADC1 clock
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

// 2. Configure PA0 as analog input
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
GPIOA->MODER |= (3U << 0); // Set PA0 to analog mode (bits 0 and 1)

// 3. Configure ADC1
ADC1->SQR3 = 0; // Channel 0 is first in regular sequence
ADC1->SQR1 = 0; // Regular sequence length = 1 conversion

ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t adc_read(void) {
    // Start a new conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Wait for conversion to complete
    while (!(ADC1->SR & ADC_SR_EOC)) {}

    volatile uint8_t brightness = ADC1->DR >> 4;
    // Read and return the converted value
    return brightness;
}
