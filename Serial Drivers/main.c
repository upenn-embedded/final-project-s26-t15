#include <stdint.h>
#include <stdio.h>      // Added this for printf
#include "stm32f411xe.h"
#include "spi.h"
#include "imu.h"
#include "oled.h"
#include "font.h"
#include "extra_functions.h"
#include "adc.h"

#define GPIOAEN     (1U << 0)
#define UART2EN     (1U << 17)
#define CR1_TE      (1U << 3)
#define CR1_UE      (1U << 13)
#define SR_TXE      (1U << 7)

void uart_init(void) {
    // 1. Enable clock access to GPIOA (for PA2)
    RCC->AHB1ENR |= GPIOAEN;

    // 2. Set PA2 to Alternate Function mode
    GPIOA->MODER &= ~(1U << 4);
    GPIOA->MODER |=  (1U << 5);

    // 3. Set PA2 alternate function type to AF7 (USART2_TX)
    GPIOA->AFR[0] |=  (1U << 8);
    GPIOA->AFR[0] |=  (1U << 9);
    GPIOA->AFR[0] |=  (1U << 10);
    GPIOA->AFR[0] &= ~(1U << 11);

    // 4. Enable clock access to UART2
    RCC->APB1ENR |= UART2EN;

    // 5. Configure baud rate (115200 @ 16MHz default)
    USART2->BRR = 0x8B;

    // 6. Enable transmitter and the UART module
    USART2->CR1 = CR1_TE;
    USART2->CR1 |= CR1_UE;
}

// Redirect printf by implementing __io_putchar
int __io_putchar(int ch) {
    // Wait until transmit data register is empty
    while (!(USART2->SR & SR_TXE)) {}
    // Write character to data register
    USART2->DR = (ch & 0xFF);
    return ch;
}


/* FIRMWARE TODO:
 * Use SysTick to periodically update step count, screen. etc. currently IMU is not auto updating
 *
 * Implement timer counting up.
 */



// Screen state — volatile because ISR modifies it
volatile uint8_t current_screen = 0; // state of screen
volatile uint8_t screen_changed = 1;  // start with 1 to draw initial screen

#define NUM_SCREENS 3

/*
 *  INTERRUPTS
 */

//PB1
void EXTI1_IRQHandler(void) {
    EXTI->PR = (1U << 1); // write 1 to clear flag otherwise NVIC will keep re-entering interrupt handler
    current_screen = (current_screen + 1) % NUM_SCREENS;
    screen_changed = 1;
}


// PB2 pressed
void EXTI2_IRQHandler(void) {
    EXTI->PR = (1U << 2);


}

// PB3 pressed
void EXTI3_IRQHandler(void) {
    EXTI->PR = (1U << 3);


}


// PB4 pressed
void EXTI4_IRQHandler(void) {
    EXTI->PR = (1U << 4);
}

int main(void)
{
    // Important: Many projects require a delay or system clock init
    // before peripherals. If it hangs, check your clock settings.

    spi1_init(); // Initialize SPI1 for IMU communication
    uart_init();
    setup_GPIO();
    spi4_init();
    oled_init();
    exti_init();
    adc_init();
    imu_enable_step_interrupt();

    if (imu_init() == 0) {
        // If IMU fails, print error and halt
    	printf("Step 2: Initializing IMU...\r\n");

    	    // Let's see what the IMU is actually sending back
    	    uint8_t id = imu_read(IMU_WHO_AM_I);

    	    if (id != IMU_ID) {
    	        printf("IMU Init Failed! Expected: 0x%02X, but Read: 0x%02X\r\n", IMU_ID, id);
    	        while(1);
    	    }
        while(1);
    }

    printf("Smart Glasses System: Online\r\n");
    char buf[6];

    while (1) {
    	if (step_flag) {
    		printf("Steps: %d\r\n", step_count);
    		step_flag = 0;

    		if (current_screen == 1) {
    			screen_changed = 1;
    		}
    	}

    	// change adc value and brightness
    	if (current_screen == 2) {
    		screen_changed = 1;
    	}

    	if (screen_changed) {
    		screen_changed = 0;
    		oled_clear();

    		switch (current_screen) {
    			case 0:
    				draw_string(0, 0, "Hello.");
    				draw_string(0, 10, "This is Argus.");
    				break;
    			case 1:
    				draw_string(0, 0, "STEPS");
    				uint_to_str(read_steps(), buf);
    				draw_string(0, 10, buf);
    				break;
    			case 2:
    				draw_string(0, 0, "ADC");
    				uint_to_str(adc_read(), buf);
    				draw_string(0 , 10, buf);
    				oled_brightness(1);
    				break;
    		}
    	}
    }
}
