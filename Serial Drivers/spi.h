/*
 * spi.h
 *
 * Created: 9/20/2021 6:54:37 PM
 *  Author: Jerry Zhang
 */ 

#include "stm32f4xx.h" // Defines aliases
#define STM32F411xE
#include <stdint.h>

#ifndef SPI_H_
#define SPI_H_

void spi1_init(void);                                // Initialize SPI comms
void spi1_write(uint8_t *data, uint32_t size);      // Write byte to SPI bus
void cs1_enable(void);                               // Set chip select
void cs1_disable(void);                              // Clear chip select
void spi1_read(uint8_t *data, uint32_t size);    // Read bytes from SPI bus
uint8_t spi1_transfer(uint8_t data);                   // Transfer a byte over SPI and receive the response

#endif