/*
 * spi.c
 *  Created on: 04/03/2026
 *  Author: Jerry Zhang
 *  Description: SPI driver for LCD display
 * 
 */ 


#include "spi.h"
#include <stdint.h>

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
void spi1_init(void)                                // Initialize SPI comms
{
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);         // Ungate GPIOA clock (enable A pins access to clk)

    /*
    Initialize the following pins for SPI1 (IMU)
    */
    RCC->APB2ENR |= (1 << RCC_APB2ENR_SPI1EN);          // Enable SPI1 module clk access (enable SPI1 access to clk)
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

    SPI1->CR1 |= (SPI_CR1_MSTR);              // Set as master
    
    SPI1->CR1 |= (SPI_CR1_CPOL);              // Set clock polarity to 1 (idle high
    SPI1->CR1 |= (SPI_CR1_CPHA);              // Set clock phase to 1 (data captured on second edge)

    SPI1->CR1 &= ~(LSBFIRST);         // Set MSB first
    SPI1->CR1 &= ~(RXONLY);           // Set full duplex

    SPI1->CR1 &= ~(DFF);              // Set 8-bit data frame format

    SPI1->CR1 |= (SPI_CR1_SSI);                // Set internal slave select
    SPI1->CR1 |= (SPI_CR1_SSM);                // Enable software slave management (using CS as GPIO)

    SPI1->CR1 |= (SPI_CR1_SPE);               // Enable SPI1
}


/**************************************************************************//**
* @fn			void spi1_write(uint8_t *data, uint32_t size)
* @brief		Write a value to a register in the IMU by sending the register address then the data byte
* @brief        Mostly for configuring settings
* @note
*****************************************************************************/
void spi1_write(uint8_t *data, uint32_t size)
{
    uint8_t temp;
    while (size > 0)
    {
        while (!(SPI1->SR & (SR_TXE))) {} // Wait for TXE flag, transmitter empty
        SPI1->DR = *data; // Write data to data register
        data++;
        size--;
    }
    
    // Wait until TXE is set
    while(!(SPI1->SR & (SR_TXE))){}

    // Wait for BUSY flag to reset
    while((SPI1->SR & (SR_BSY))){}

    // Clear OVR flag
    temp = SPI1->DR;
    temp = SPI1->SR;
}

/**************************************************************************//**
* @fn			void spi1_read(uint8_t *data, uint32_t size)
* @brief		Read a register 
* @brief        Mostly for configuring settings
* @note
*****************************************************************************/
uint8_t spi1_read(uint8_t *data, uint32_t size)
{
    while (size > 0)
    {
        while (!(SPI1->SR & (SR_TXE))) {} // Wait for TXE flag, transmitter empty
        SPI1->DR = 0x00; // Send dummy byte to generate clock
        while (!(SPI1->SR & (SR_RXNE))) {} // Wait for RXNE flag, receiver full
        *data = (uint8_t)(SPI1->DR); // Read received data
        data++;
        size--;
    }
    return data;
}

void cs1_enable(void)
{
    GPIOA->ODR &= ~(1U << 9);
}

void cs1_disable(void)                                  // Set chip select
{
    GPIOA->ODR |= (1U << 9);                 // Set PA9 high to clear CS (active low)
}

/**************************************************************************//**
* @fn			uint8_t spi1_transfer(uint8_t data)
* @brief		Send a byte over SPI and receive one back, typically for reading a register val from IMU
* @note
*****************************************************************************/
uint8_t spi1_transfer(uint8_t data) {
    while(!(SPI1->SR & (SR_TXE))); // Wait for TXE
    SPI1->DR = data;
    while(!(SPI1->SR & (SR_RXNE))); // Wait for RXNE
    return (uint8_t) (SPI1->DR);
}