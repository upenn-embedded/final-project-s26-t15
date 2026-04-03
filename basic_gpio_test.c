#include "stm32f4xx.h" // Defines aliases
#include <stdint.h>


void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1 << 0)) {
        EXTI->PR |= (1 << 0);        // clear pending flag
        GPIOA->ODR |= (1 << 5);      // turn on LED (PA5)
    }
}

int main(void) {
    // 1. Enable clocks
    RCC->AHB1ENR |= (1 << 0);        // GPIOA port clock enable
    RCC->APB2ENR |= (1 << 14);       // SYSCFG clock (needed for EXTI routing)

    // 2. PA0 as input & PA5 (LED) as output
    GPIOA->MODER &= ~(3 << 0);      // sets PA0 to input
    GPIOA->MODER &= ~(3 << 10); // PA5 clear bits
    GPIOA->MODER |= (1 << 10); // 01 = output

    // 3. Route PA0 to EXTI0 via SYSCFG
    SYSCFG->EXTICR[0] &= ~(0xF);     // bits [3:0] = 0000 = GPIOA

    // 4. Enable internal pull-up
    GPIOA->PUPDR &= ~(3 << 0);       // clear
    GPIOA->PUPDR |=  (1 << 0);       // 01 = pull-up

    // 5. Configure EXTI0, aka the interrupt
    EXTI->IMR  |= (1 << 0);          // unmask line 0
    EXTI->FTSR |= (1 << 0);          // falling edge trigger

    // 6. Enable in NVIC
    NVIC_EnableIRQ(EXTI0_IRQn);  // EXTI13 shares handler with lines 10-15

    while(1){
    }
}


