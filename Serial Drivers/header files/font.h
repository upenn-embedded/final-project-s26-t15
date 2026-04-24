/*
 * font.h
 *  Created on: 04/12/2026
 *  Author: Seth Lee
 *
 */

#ifndef FONT_H
#define FONT_H

#include <stdint.h>

void draw_char(uint8_t x, uint8_t y, char c);
void draw_string(uint8_t x, uint8_t y, const char *str);
void draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif
