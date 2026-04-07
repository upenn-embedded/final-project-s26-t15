/*
 * spi.h
 *
 * Created: 9/20/2021 6:54:37 PM
 *  Author: Jerry Zhang
 */ 

#include "stm32f4xx.h" // Defines aliases
#include <stdint.h>

#ifndef spi_h
#define spi_h

// colors
#define	BLACK     0x0000
#define WHITE     0xFFFF
#define	BLUE      0x001F
#define	RED       0xF800
#define	GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0

void spi_init(void);                                // Initialize SPI comms
void spi1_write(uint8_t *data, uint32_t size);      // Write byte to SPI bus
void cs_set(void);                                  // Set chip select
void cs_clear(void);                                // Clear chip select
void spi1_receive(uint8_t *data, uint32_t size);     // Read bytes from SPI bus
#endif /* spi_h */