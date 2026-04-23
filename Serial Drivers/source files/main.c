#include <stdint.h>
#include <stdio.h>      // Added this for printf
#include "stm32f411xe.h"
#include "spi.h"
#include "imu.h"
#include "oled.h"
#include "font.h"
#include "extra_functions.h"
#include "adc.h"
#include "timing.h"
#include "rtc.h"

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


// Screen state — volatile because ISR modifies it
volatile uint8_t current_screen = 0; // state of screen
volatile uint8_t screen_changed = 1;  // start with 1 to draw initial screen

#define NUM_SCREENS 3

/*
 *  INTERRUPTS
 */

//PB1
void EXTI1_IRQHandler(void) {

	static uint32_t last_press = 0; // lives in .data for lifetime of program

	if ((get_tick() - last_press) > 300) { // prevent debouncing
		if (!(GPIOB->IDR & (1U << 1))) {
			last_press = get_tick();
			current_screen = (current_screen + 1) % NUM_SCREENS;
			screen_changed = 1;
		}
	}

	EXTI->PR = (1U << 1); // write 1 to clear flag otherwise NVIC will keep reentering interrupt handler

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

volatile uint16_t speed = 0;

// Timer 2 interrupt every 100ms
static int second_calibration = 0;
void TIM2_IRQHandler(void) {

    if (TIM2->SR & (1 << 0)) { // check if flag is set
        TIM2->SR &= ~(1 << 0); // clear flag

        //BRIGHTNESS UPDATE
        oled_brightness(adc_read());

        // TIME UPDATE (and also checking speed every second)
        second_calibration += 1;
        if (second_calibration == 9){
        	second_calibration = 0;
        	if (current_screen == 1){
				screen_changed = 1;
			}

			if (current_screen == 2){
				speed = calculate_speed_from_steps();
				screen_changed = 1;
			}
        }
    }
}

// calibrate time from compilation time
static void set_time_from_compile(void) {
    // __TIME__ is a string literal: "HH:MM:SS"
    uint8_t h = (__TIME__[0] - '0') * 10 + (__TIME__[1] - '0');
    uint8_t m = (__TIME__[3] - '0') * 10 + (__TIME__[4] - '0');
    uint8_t s = (__TIME__[6] - '0') * 10 + (__TIME__[7] - '0');
    rtc_set_time(h, m, s);
}

int main(void)
{
	delay_init(); // start counting ticks
    spi1_init(); // SPI1 for IMU communication
    uart_init(); // UART for printing to terminal
    setup_GPIO(); // GPIO setup
    spi4_init(); // SPI4 for OLED
    oled_init(); // OLED
    exti_init(); // Initialize IMU interrupt on STM
    adc_init(); // ADC
    imu_enable_step_interrupt(); // IMU interrupt enable
    TIM2_init(); // timer2 for interrupts
    rtc_init(); // RTC for the time
    set_time_from_compile();

    if (imu_init() == 0) {
        // If IMU fails, print error and halt
    	printf("Step 2: Initializing IMU...\r\n");

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

    	// auto-update screen when step count updates
    	if (step_flag) {
    		step_flag = 0;

    		if (current_screen == 2) {
    			screen_changed = 1;
    		}
    	}

    	// Main screen handler
    	if (screen_changed) {
    		screen_changed = 0;
    		oled_clear();


    		switch (current_screen) {
    		// MAIN SCREEN
    			case 0:
    				draw_string(0, 0, "Hello.");
    				draw_string(0, 10, "This is Argus.");
    				break;
    		// TIME
    			case 1:
    				uint8_t h, m, s;
					rtc_get_time(&h, &m, &s);

					const char *period = (h >= 12) ? "PM" : "AM";
					uint8_t h12 = h % 12;
					if (h12 == 0) h12 = 12;

					char time_buf[9];
					time_buf[0] = '0' + h12 / 10;
					time_buf[1] = '0' + h12 % 10;
					time_buf[2] = ':';
					time_buf[3] = '0' + m / 10;
					time_buf[4] = '0' + m % 10;
					time_buf[5] = ':';
					time_buf[6] = '0' + s / 10;
					time_buf[7] = '0' + s % 10;
					time_buf[8] = '\0';

					draw_string(0, 0, "TIME");
					draw_string(0, 10, time_buf);
					draw_string(0, 20, period);
					break;
			// STEPS
    			case 2:
    				draw_string(0, 0, "STEPS");
    				uint_to_str(read_steps(), buf);
    				draw_string(0, 10, buf);
					draw_string(30, 30, "MPH:");
					uint_to_str(speed, buf);
					draw_string(30, 48, buf);
					break;


    		}
    	}
    }
}
