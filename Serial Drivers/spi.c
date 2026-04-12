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
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;         // Enable GPIOA clock

    /*
    Initialize the following pins for SPI1 (IMU)
    */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;          // Enable SPI1 module clock access (enable SPI1 access to clk)
    // PA5 - SCK
    GPIOA->MODER &= ~(1U << 10);            // Set PA5 in alternate mode for SPI
    GPIOA->MODER |= (1U << 11);
    GPIOA->AFR[0] |=(1U<<20);               // AFR = alternate function register. Set AFR bits to 0101 for SPI1.
    GPIOA->AFR[0] &= ~(1U<<21);				// In this case, we are using PA5.
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

    SPI1->CR1 |= (1U << 3);                    // Set Prescaler to 4 (001). 16Mhz / 4 = 4Mhz.
    SPI1->CR1 &= ~(1U << 4);
    SPI1->CR1 &= ~(1U << 5);

    SPI1->CR1 |= (SPI_CR1_MSTR);              // SPI1 on STM32 controls the SPI bus (i.e peripherals)

    SPI1->CR1 &= ~(SPI_CR1_CPOL);              // Set clock polarity to 0 (the clock is low when idle)
    SPI1->CR1 &= ~(SPI_CR1_CPHA);              // Set clock phase to 0 (data captured on first edge)

    SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);         // SPI will transmit MSB first
    SPI1->CR1 &= ~(SPI_CR1_RXONLY);           // Set full duplex

    SPI1->CR1 &= ~(SPI_CR1_DFF);              // Set SPI data frame to 8-bit

    SPI1->CR1 |= (SPI_CR1_SSI);                // Set internal slave select
    SPI1->CR1 |= (SPI_CR1_SSM);                // Enable software slave management (control CS using GPIO in software)

    SPI1->CR1 |= (SPI_CR1_SPE);               // Enable SPI1
}

void spi4_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // Enable GPIOA (i.e PORT A) clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   // Enable GPIOB (i.e PORT B) clock
    RCC->APB2ENR |= (1U << 13);             // Enable SPI4 module clock access

    // PA1 - MOSI (AF5)
    GPIOA->MODER &= ~(1U << 2);             // Set PA1 alternate function mode.
    GPIOA->MODER |=  (1U << 3);
    GPIOA->AFR[0] |=  (1U << 4);            // AF5 = 0101 for bits [7:4] for SPI4. Note that AFR register controls each Pin's MUX for alternate function.
    GPIOA->AFR[0] &= ~(1U << 5);
    GPIOA->AFR[0] |=  (1U << 6);
    GPIOA->AFR[0] &= ~(1U << 7);

    // PB13 - SCK (AF6)
    GPIOB->MODER &= ~(1U << 26);            // Set PB13 alternate function mode
    GPIOB->MODER |=  (1U << 27);
    GPIOB->AFR[1] &= ~(1U << 20);           // AF6 = 0110 for bits [23:20]
    GPIOB->AFR[1] |=  (1U << 21);
    GPIOB->AFR[1] |=  (1U << 22);
    GPIOB->AFR[1] &= ~(1U << 23);

    // PB12 - CS (plain GPIO output)
    GPIOB->MODER |=  (1U << 24);            // Set PB12 as output
    GPIOB->MODER &= ~(1U << 25);
    GPIOB->ODR   |=  (1U << 12);            // CS default HIGH

    // SPI4 control registers
    SPI4->CR1 |=  (1U << 3);                // Prescaler to divide by 4.
    SPI4->CR1 &= ~(1U << 4);
    SPI4->CR1 &= ~(1U << 5);

    SPI4->CR1 |=  SPI_CR1_MSTR;             // STM32 controls peripherals via clock
    SPI4->CR1 &= ~SPI_CR1_CPOL;             // Clock idle low
    SPI4->CR1 &= ~SPI_CR1_CPHA;             // Sample on first edge
    SPI4->CR1 &= ~SPI_CR1_LSBFIRST;         // MSB first
    SPI4->CR1 &= ~SPI_CR1_RXONLY;           // Full duplex
    SPI4->CR1 &= ~SPI_CR1_DFF;              // 8-bit frames
    SPI4->CR1 |=  SPI_CR1_SSI;
    SPI4->CR1 |=  SPI_CR1_SSM;              // Software CS management
    SPI4->CR1 |=  SPI_CR1_SPE;              // Enable SPI4
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
        while (!(SPI1->SR & (SPI_SR_TXE))) {} // Wait for TXE flag, transmitter empty
        SPI1->DR = *data; // Write data to data register
        data++;
        size--;
    }

    // Wait until TXE is set
    while(!(SPI1->SR & (SPI_SR_TXE))){}

    // Wait for BUSY flag to reset
    while((SPI1->SR & (SPI_SR_BSY))){}

    // Clear OVR flag
    temp = SPI1->DR;
    temp = SPI1->SR;
}


void spi4_write(uint8_t *data, uint32_t size) {
    uint8_t temp;
    while (size > 0) { // loop until all bytes sent
        while (!(SPI4->SR & SPI_SR_TXE)) {} // TX buffer must be ready to receive new byte before sending data to TX buffer
        SPI4->DR = *data;
        data++; // advance pointer to the next byte
        size--; // decrement the remaining count
    }
    while (!(SPI4->SR & SPI_SR_TXE)) {} // wait for the TX buffer to empty
    while  (SPI4->SR & SPI_SR_BSY)  {} // SPI_SR_BSY is high while shift register is still transmitting
    temp = SPI4->DR; // Clear OVR flag
    temp = SPI4->SR;
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
        while (!(SPI1->SR & (SPI_SR_TXE))) {} // Wait for TXE flag, transmitter empty
        SPI1->DR = 0x00; // Send dummy byte to generate clock
        while (!(SPI1->SR & (SPI_SR_RXNE))) {} // Wait for RXNE flag, receiver full
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



void cs2_enable(void)  { GPIOB->ODR &= ~(1U << 12); } // pull PB12 Low
void cs2_disable(void) { GPIOB->ODR |=  (1U << 12); } // Pull PB12 High -> inactive

/**************************************************************************//**
* @fn			uint8_t spi1_transfer(uint8_t data)
* @brief		Send a byte over SPI and receive one back, typically for reading a register val from IMU
* @note
*****************************************************************************/
uint8_t spi1_transfer(uint8_t data) {
    while(!(SPI1->SR & (SPI_SR_TXE))); // Wait for TXE
    SPI1->DR = data;
    while(!(SPI1->SR & (SPI_SR_RXNE))); // Wait for RXNE
    uint8_t received_data = (uint8_t) SPI1->DR;
    uint8_t status_burner = SPI1->SR;
    return received_data;
}
