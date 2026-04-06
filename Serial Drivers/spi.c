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

    /*
    Initialize the following pins for SPI1 (IMU)
    */
    RCC->APB2ENR |= (1 << SPI1EN);          // Enable SPI1 module clk access (enable SPI1 access to clk)
    // PA5 - SCK
    GPIOA->MODER &= ~(1U << 10);            // Set PA5 in alternate mode
    GPIOA->MODER |= (1U << 11);
    GPIOA->AFR[0] |=(1U<<20);               // Set AFR bits to 0101 for SPI1
    GPIOA->AFR[0] &= ~(1U<<21);
    GPIOA->AFR[0] |=(1U<<22);
    GPIOA->AFR[0] &= ~(1U<<23);

    // PA6 - MISO
    GPIOA->MODER &= ~(1U << 12);            // Set PA6 in alternate mode
    GPIOA->MODER |= (1U << 13);
    GPIOA->AFR[0] |=(1U<<24);               // Set AFR bits to 0101 for SPI1
    GPIOA->AFR[0] &= ~(1U<<25);
    GPIOA->AFR[0] |=(1U<<26);
    GPIOA->AFR[0] &= ~(1U<<27);

    // PA7 - MOSI
    GPIOA->MODER &= ~(1U << 14);            // Set PA7 in alternate mode
    GPIOA->MODER |= (1U << 15);
    GPIOA->AFR[0] |=(1U<<28);               // Set AFR bits to 0101 for SPI1
    GPIOA->AFR[0] &= ~(1U<<29);
    GPIOA->AFR[0] |=(1U<<30);
    GPIOA->AFR[0] &= ~(1U<<31);

    // PA9 - CS
    GPIOA->MODER |= (1U << 18);             // Set PA9 as general purpose output for CS
    GPIOA->MODER &= ~(1U << 19);

    SPI1->CR1 |=(1U<<3);                    // Set Prescaler to 4 (001)
    SPI1->CR1 &=~(1U<<4);
    SPI1->CR1 &=~(1U<<5);

    SPI1->CR1 |= (1U << MSTR);              // Set as master
    
    SPI1->CR1 |= (1U << CPOL);              // Set clock polarity to 1 (idle high
    SPI1->CR1 |= (1U << CPHA);              // Set clock phase to 1 (data captured on second edge)

    SPI1->CR1 &= ~(1U << LSBFIRST);         // Set MSB first
    SPI1->CR1 &= ~(1U << RXONLY);           // Set full duplex

    SPI1->CR1 &= ~(1U << DFF);              // Set 8-bit data frame format

    SPI1->CR1 |= (1 << SSI);                // Set internal slave select
    SPI1->CR1 |= (1 << SSM);                // Enable software slave management (using CS as GPIO)

    SPI1->CR1 |= (1U << SPE);               // Enable SPI1
}

void spi1_write(uint8_t *data,uint32_t size)
{
    uint32_t i=0;
    uint8_t temp;
    while(i < size)
    {
        // Wait until TXE is set
        while(!(SPI1->SR & (SR_TXE))){}
        // Write data to data register
        SPI1->DR = data[i];
        i++;
    }
    
    // Wait until TXE is set
    while(!(SPI1->SR & (SR_TXE))){}

    // Wait for BUSY flag to reset
    while((SPI1->SR & (SR_BSY))){}

    // Clear OVR flag
    temp = SPI1->DR;
    temp = SPI1->SR;
}

void spi1_receive(uint8_t *data, uint32_t size)
{
    uint32_t i=0;
    uint8_t temp;
    while(i < size)
    {
       
    }
}

void cs_set(void)                                  // Set chip select
{
    GPIOA &= ~(1U << 9);                 // Set PA9 high to clear CS (active low)
}