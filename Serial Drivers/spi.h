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

void spi_init(void);                                // Initialize SPI comms
void spi1_write(uint8_t *data, uint32_t size);      // Write byte to SPI bus
void cs_set(void);                                  // Set chip select
void cs_clear(void);                                // Clear chip select
void spi1_receive(uint8_t *data, uint32_t size);     // Read bytes from SPI bus
#endif /* spi_h */