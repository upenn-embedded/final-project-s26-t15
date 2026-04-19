/*
 * oled.h
 *
 *  Author: Seth lee
 */

#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void oled_init(void);
void oled_clear(void);
void oled_brightness(uint8_t brightness);
void draw_pixel(uint8_t x, uint8_t y, uint8_t color);

#endif
