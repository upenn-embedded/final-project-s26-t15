#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "stm32f4xx.h" // Replace with your specific STM32 family header (e.g., stm32l4xx.h)

// Function prototypes
void I2C1_Init(void);
int I2C1_Write(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
int I2C1_Read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t size);

#endif // I2C_H
