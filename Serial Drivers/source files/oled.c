/*
 * oled.c
 *  Created on: 04/12/2026
 *  Author: Seth Lee
 *  Description: OLED Display Driver
 *
 */

// Quick description:
// SSD1306 128x64 OLED driver
// SPI4 pins: PA1 (MOSI), PB13 (SCK), PB12 (CS)
// GPIO pins: PB0 (D/C)

#include "oled.h"
#include "spi.h"

// local framebuffer — 128 columns x 8 pages, one byte per column per page
// we can't read back from the OLED so we keep a copy in RAM
static uint8_t framebuffer[8][128];


// static functions

static void oled_send_cmd(uint8_t cmd) {
    GPIOB->ODR &= ~(1U << 0);    // D/C# LOW = command
    for (volatile int i = 0; i < 10; i++); // small delay setup
    cs2_enable();
    spi4_write(&cmd, 1);
    cs2_disable();
}

static void oled_send_data(uint8_t data) {
    GPIOB->ODR |=  (1U << 0);    // D/C# HIGH = data
    for (volatile int i = 0; i < 10; i++);
    cs2_enable();
    spi4_write(&data, 1);
    cs2_disable();
}

void oled_brightness(uint8_t brightness) {
	oled_send_cmd(0x81);
	oled_send_cmd(brightness);
}

// set write cursor to a specific page and column using page addressing mode
static void oled_set_cursor(uint8_t page, uint8_t col) {
    oled_send_cmd(0xB0 | page);           // set page (0–7)
    oled_send_cmd(0x00 | (col & 0x0F));   // lower nibble of column
    oled_send_cmd(0x10 | (col >> 4));     // upper nibble of column
}

void oled_init(void) {
    // enable GPIOB clock - probably not needed bc it's redundant with spi.c
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB0 = D/C#, output
    GPIOB->MODER |=  (1U << 0);
    GPIOB->MODER &= ~(1U << 1);

    // init command sequence (charge pump variant per the module datasheet)
    oled_send_cmd(0xAE);        // display off

    oled_send_cmd(0xD5);        // set display clock divide ratio
    oled_send_cmd(0x80);

    oled_send_cmd(0xA8);        // set multiplex ratio
    oled_send_cmd(0x3F);        // 64 rows

    oled_send_cmd(0xD3);        // set display offset
    oled_send_cmd(0x00);

    oled_send_cmd(0x40);        // set display start line = 0

    oled_send_cmd(0x8D);        // charge pump setting
    oled_send_cmd(0x14);        // enable charge pump

    oled_send_cmd(0x20);        // memory addressing mode
    oled_send_cmd(0x02);        // page addressing mode

    oled_send_cmd(0xA1);        // segment remap (col 127 -> SEG0)
    oled_send_cmd(0xC8);        // COM scan remapped (top to bottom)

    oled_send_cmd(0xDA);        // COM pins hardware config
    oled_send_cmd(0x12);

    oled_send_cmd(0x81);        // contrast
    oled_send_cmd(0xCF);

    oled_send_cmd(0xD9);        // pre-charge period
    oled_send_cmd(0xF1);

    oled_send_cmd(0xDB);        // VCOMH deselect level
    oled_send_cmd(0x40);

    oled_send_cmd(0xA4);        // output follows RAM content
    oled_send_cmd(0xA6);        // normal display (not inverted)

    oled_send_cmd(0xAF);        // display ON

    // clear framebuffer and display
    oled_clear();
}

void oled_clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_set_cursor(page, 0);
        for (uint8_t col = 0; col < 128; col++) {
            framebuffer[page][col] = 0x00;
            oled_send_data(0x00);
        }
    }
}

void draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= 128 || y >= 64) return;   // bounds check

    uint8_t page = y / 8;              // which page (0-7)
    uint8_t bit  = y % 8;              // which bit within that page's byte

    if (color) {
        framebuffer[page][x] |=  (1U << bit);   // set pixel ON
    } else {
        framebuffer[page][x] &= ~(1U << bit);   // set pixel OFF
    }

    // write only the affected byte to the display
    oled_set_cursor(page, x);
    oled_send_data(framebuffer[page][x]);
}
