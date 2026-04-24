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
#define NUM_SCREENS 5


// timer variable
volatile uint32_t timer_seconds = 60;
volatile uint32_t timer_running = 0;
volatile uint32_t last_tick = 0;

// brightness of OLED
volatile uint8_t brightness_level = 1; // 0=Low, 1=Medium, 2=High, 3 = auto
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
	        if (!timer_running && timer_seconds < 3599 && current_screen == 3) {
	            if (timer_seconds >= 3539) timer_seconds = 3599;
	            else timer_seconds += 60;
	            if (current_screen == 3) screen_changed = 1;
	        }
	        if (current_screen == 4) {
	        	if (brightness_level < 3) brightness_level++;
	        	else brightness_level = 0;
				screen_changed = 1;
	        }
	 }
}

// PB4 pressed. Decrements timer
void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1U << 4)) {
        EXTI->PR |= (1U << 4);
        if (!timer_running && timer_seconds > 0 && current_screen == 3) {
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

        // Update brightness auto
        if (brightness_level == 3) oled_brightness(adc_read());

        // Update Time
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


// Defining visible are of OLED
// 6 pixels per character
#define VIS_X    45 // X coordinate of the left edge (ends up being flipped horizontally, making it the right edge)
#define VIS_Y    10 // y coordinate of the top edge
#define VIS_W    80 // width of rectangles
#define VIS_H    44 // total height of both rectangles combined
#define TOP_H    (VIS_H / 4)        // 11px - height of top rectangle
#define BOT_H    (VIS_H - TOP_H)    // 33px - height of bottom rectangle
#define VIS_MID_Y (VIS_Y + TOP_H) // y coordinate of the top edge of bottom rectangle

// FOR BOTTOM RECTANGLE: we get 3 lines of text
// VIS_MID_Y + 3
// VIS_MID_Y + 13
// VIS_MID_Y + 23


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

    char buf[12]; // for step count
    char spd_buf[12] = "0.0 MPH";




    while (1) {

    	if (timer_running && (get_tick() - last_tick >= 1000)) {
    	    __disable_irq();
    	    last_tick += 1000;
    	    timer_seconds--;
    	    if (current_screen == 3) screen_changed = 1;
    	    if (timer_seconds == 0) timer_running = 0;
    	    __enable_irq();
    	}

    	// auto-update screen when step count updates
    	if (step_flag) {
    		step_flag = 0;

    		if (current_screen == 2) {
    			calculate_speed_from_steps(spd_buf, sizeof(spd_buf));
    			screen_changed = 1;
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
    				draw_string(64, VIS_Y + 2, "Welcome");
    				draw_string(61, VIS_MID_Y + 3,  "This is");
    				draw_string(67, VIS_MID_Y + 13, "Argus.");
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

					draw_string(73, VIS_Y + 2,      "TIME");
					draw_string(61, VIS_MID_Y + 3,  time_buf);
					draw_string(79, VIS_MID_Y + 13, period);
					break;
			// STEPS
    			case 2:
    				draw_string(70, VIS_Y + 2,      "STEPS");
    				uint_to_str(read_steps(), buf);
    				draw_string(73, VIS_MID_Y + 3,  buf);
    				draw_string(61, VIS_MID_Y + 13, spd_buf);
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
					draw_string(70, VIS_Y + 2,      "TIMER");
					draw_string(70, VIS_MID_Y + 3,  tbuf);
					draw_string(64, VIS_MID_Y + 13, timer_running ? "RUNNING" : "STOPPED");
					break;

			// BRIGHTNESS CONTROL
    			case 4:
					draw_string(55, VIS_Y + 2, "BRIGHTNESS");

					const uint8_t levels[3] = {50, 150, 255};
					if (brightness_level < 3) oled_brightness(levels[brightness_level]);

					// all the brightness types - 4 types: low, medium, high, auto
					draw_string(VIS_X + 4, VIS_MID_Y + 3,  brightness_level == 0 ? ">Low"    : " Low");
					draw_string(VIS_X + 4, VIS_MID_Y + 13, brightness_level == 1 ? ">Medium" : " Medium");
					draw_string(VIS_X + 4, VIS_MID_Y + 23, brightness_level == 2 ? ">High"   : " High");
					// IF none of the options are selected, it means auto.
					break;
    		}
    	}
    }
}
