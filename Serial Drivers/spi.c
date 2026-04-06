/*
 * spi.c
 *  Created on: 04/03/2026
 *  Author: Jerry Zhang
 *  Description: SPI driver for LCD display
 * 
 */ 


#include "spi.h"

// Enable clock access to GPIOA pins

/******************************************************************************
* Local Functions
******************************************************************************/



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			void spi_init(void)
* @brief		Initiate SPI bus and configure relevant SPI pins (SPI1 for IMU, SPI4 for LCD)
* @note
*****************************************************************************/
void spi_init(void);                                // Initialize SPI comms
{
	RCC->AHB1ENR |= (1 << GPIOAEN);         // Ungate GPIOA clock (enable A pins access to clk)
    RCC->AHB2ENR |= (1 << SPIO1EN);         // Enable SPI1 module clk access (enable SPI1 access to clk)

    /*
    Initialize the following pins for SPI1 (IMU)
    */
    // PA5 - SCK
    GPIOA->MODER &= ~(1U << 10);            // Set PA5 in alternate mode
    GPIOA->MODER |= (1U << 11);

    // PA6 - MISO
    GPIOA->MODER &= ~(1U << 12);            // Set PA6 in alternate mode
    GPIOA->MODER |= (1U << 13);

    // PA7 - MOSI
    GPIOA->MODER &= ~(1U << 14);            // Set PA7 in alternate mode
    GPIOA->MODER |= (1U << 15);

    // PA9 - CS
    GPIOA->MODER |= (1U << 18);             // Set PA9 as general purpose output for CS
    GPIOA->MODER &= ~(1U << 19);
}