/*
 * main.c
 *  Created on: 04/03/2026
 *  Author: Seth Lee
 *  Description: Handles the User interface.
 *
 */

#include "stm32f4xx.h" // Defines aliases from a header file
#include <stdint.h>
#include "spi.h"
#include "oled.h"
#include "font.h"
#include "imu.h"
#include "extra_functions.h"

/* FIRMWARE TODO:
 * Use SysTick to periodically update step count, screen. etc. currently IMU is not auto updating
 *
 * Implement timer counting up.
 */



// Screen state — volatile because ISR modifies it
volatile uint8_t current_screen = 0; // state of screen
volatile uint8_t screen_changed = 1;  // start with 1 to draw initial screen

#define NUM_SCREENS 2

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



int main(void) {

	/*
	 * Initialization code
	 */

	// GPIO
	setup_GPIO();

	// SPI1 - IMU
	spi1_init();
	imu_init();

	// SPI4 - OLED
    spi4_init();
    oled_init();


    /*
     * USER INTERFACE
     */

    char buf[6];

    while (1) {
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
					uint16_t steps = read_steps();
					uint_to_str(steps, buf);
					draw_string(0, 10, buf);
					break;
			}
		}
	}
}


