#include <stdint.h>
#include <stdio.h>
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



// Screen state — volatile because ISR modifies it
volatile uint8_t current_screen = 0; // state of screen
volatile uint8_t screen_changed = 1;  // start with 1 to draw initial screen
#define NUM_SCREENS 4


// timer variable
volatile uint32_t timer_seconds = 60;
volatile uint32_t timer_running = 0;
volatile uint32_t last_tick = 0;


/*
 *  INTERRUPTS
 */

// PB1 pressed. Handles screen changes
void EXTI1_IRQHandler(void) {

	static uint32_t last_press = 0; // lives in .data for lifetime of program

	if ((get_tick() - last_press) > 300) { // prevent debouncing
		if (!(GPIOB->IDR & (1U << 1))) {
			if (!timer_running){
				last_press = get_tick();
				current_screen = (current_screen + 1) % NUM_SCREENS;
				screen_changed = 1;
			}
		}
	}

	EXTI->PR = (1U << 1); // write 1 to clear flag otherwise NVIC will keep reentering interrupt handler

}


// PB2 pressed. Handles starting/stopping the timer.
void EXTI2_IRQHandler(void) {
	 if (EXTI->PR & (1U << 2)) {
	        EXTI->PR |= (1U << 2);
	        if (timer_seconds > 0) {
	            timer_running ^= 1;
	            last_tick = get_tick();
	            if (current_screen == 3) {
	            	screen_changed = 1;
	            }
	        }
	    }
}

// PB3 pressed. Increments timer
void EXTI3_IRQHandler(void) {
	 if (EXTI->PR & (1U << 3)) {
	        EXTI->PR |= (1U << 3);
	        if (!timer_running && timer_seconds < 3599) {
	            if (timer_seconds >= 3539) timer_seconds = 3599;
	            else timer_seconds += 60;
	            if (current_screen == 3) screen_changed = 1;
	        }
	    }
}

// PB4 pressed. Decrements timer
void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1U << 4)) {
        EXTI->PR |= (1U << 4);
        if (!timer_running && timer_seconds > 0) {
            if (timer_seconds <= 59) timer_seconds = 0;
            else timer_seconds -= 60;
            if (current_screen == 3) screen_changed = 1;
        }
    }
}




// Timer 2 interrupt every 100ms
static int second_calibration = 0;
void TIM2_IRQHandler(void) {

    if (TIM2->SR & (1 << 0)) { // check if flag is set
        TIM2->SR &= ~(1 << 0); // clear flag

        //BRIGHTNESS UPDATE
        oled_brightness(adc_read());

        // TIME UPDATE
        second_calibration += 1;
        if (second_calibration == 9){
        	second_calibration = 0;
        	if (current_screen == 1){
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


// speed string
volatile uint16_t speed;

// Defining visible are of OLED
#define VIS_X    18
#define VIS_Y    10
#define VIS_W    90
#define VIS_H    44
#define TOP_H    (VIS_H / 4)        // 11px  — 1/3 of bottom
#define BOT_H    (VIS_H - TOP_H)    // 33px
#define VIS_MID_Y (VIS_Y + TOP_H)

int main(void)
{
	delay_init(); // start counting ticks
    spi1_init(); // SPI1 for IMU communication
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
        // If IMU fails, halt
        //while(1);
    }

    char buf[6]; // for step count


    while (1) {

    	// auto-update screen when step count updates
    	if (step_flag) {
    		step_flag = 0;

    		if (current_screen == 2) {
    			screen_changed = 1;
				speed = calculate_speed_from_steps();
    		}
    	}

    	// Main screen handler
    	if (screen_changed) {
    		screen_changed = 0;
    		oled_clear();

    		// GUI made from rectangles
    		draw_rect(VIS_X, VIS_Y, VIS_W, TOP_H);   // small header box
    		draw_rect(VIS_X, VIS_MID_Y, VIS_W, BOT_H);   // large content box


    		switch (current_screen) {
    		// MAIN SCREEN
    			case 0:
					draw_string(47, VIS_Y + 3, "Welcome");
					draw_string(30, VIS_MID_Y + 5, "This is Argus.");
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
					draw_string(46, VIS_Y + 3, "STEPS");
					uint_to_str(read_steps(), buf);
					draw_string(52, VIS_MID_Y + 1, buf);
					draw_string(28, VIS_MID_Y + 10, "MPH:");
					uint_to_str(speed, buf);
					draw_string(52, VIS_MID_Y + 10, buf);
					break;
			// TIMER
    			case 3:
    				char tbuf[6];
					uint32_t timer_minute = timer_seconds / 60;
					uint32_t timer_second = timer_seconds % 60;
					tbuf[0] = '0' + timer_minute / 10; tbuf[1] = '0' + timer_minute % 10;
					tbuf[2] = ':';
					tbuf[3] = '0' + timer_second / 10; tbuf[4] = '0' + timer_second % 10;
					tbuf[5] = '\0';
					draw_string(47, VIS_Y + 3, "TIMER");
					draw_string(49, VIS_MID_Y + 2, tbuf);
					draw_string(28, VIS_MID_Y + 11, timer_running ? "RUNNING" : "STOPPED");
					break;
    		}
    	}
    }
}
