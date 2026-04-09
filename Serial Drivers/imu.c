/*
 * spi.c
 *  Created on: 04/03/2026
 *  Author: Jerry Zhang
 *  Description: SPI driver for LCD display
 * 
 */ 


#include "imu.h"

/**************************************************************************//**
* @fn			uint8_t imu_init(void)
* @brief		Initialize all IMU settings to turn on step counter
* @note
*****************************************************************************/
uint8_t imu_init(void) {
    if (imu_read(IMU_WHO_AM_I) != IMU_ID) return 0; // verify device ID

    imu_write(IMU_CTRL1_XL, IMU_ODR | IMU_FS); // set accelerometer performance settings
    imu_write(IMU_CTRL10_C, IMU_TIMESTAMP_EN); // enable time stamp for step counter

    imu_write(EMB_FUNC_CFG_ACCESS, FUNC_CFG_ACCESS); // enable access to embedded functions
    imu_write(EMB_FUNC_EN_A, IMU_PEDO_EN); // enable pedometer
    imu_write(EMB_FUNC_CFG_ACCESS, 0x99); // disable access to embedded functions

    return 1;                               // success
}

/**************************************************************************//**
* @fn			void imu_write(uint8_t reg_addr, uint8_t data)
* @brief		Write a byte of data into a register address on the IMU
* @note
*****************************************************************************/
void imu_write(uint8_t reg_addr, uint8_t data) {
    cs1_enable();
    spi1_transfer(reg_addr & ~0x80); // MSB is 0 to Write
    spi1_transfer(data);     // send the data byte
    cs1_disable();
}

/**************************************************************************//**
* @fn			uint8_t imu_read(uint8_t reg_addr)
* @brief		Read the byte at the register address specified
* @note
*****************************************************************************/
uint8_t imu_read(uint8_t reg_addr) {
    uint8_t result;
    cs1_enable();
    spi1_transfer(reg_addr | 0x80); // Set MSB to 1 to read
    result = spi1_transfer(0x00);   // Send dummy byte to receive data
    cs1_disable();
    return result;
}

/**************************************************************************//**
* @fn			uint8_t imu_read_multiple(uint8_t reg_addr, uint8_t *buffer, uint32_t size)
* @brief		Read size number of bytes into a buffer starting from register address
* @note
*****************************************************************************/
uint8_t imu_read_multiple(uint8_t reg_addr, uint8_t *buffer, uint32_t size) {
    cs1_enable();
    spi1_transfer(reg_addr | 0x80); // Set MSB to 1 to read
    spi1_read(buffer, size);        // Read multiple bytes into buffer
    cs1_disable();
    return buffer;
}

/**************************************************************************//**
* @fn			uint8_t imu_readxyz(uint8_t reg_addr, uint8_t *buffer)
* @brief		Read 6 bytes of data for x, y, z axis readings
* @note
*****************************************************************************/
uint8_t imu_readxyz(uint8_t reg_addr, uint8_t *buffer) {
    return imu_read_multiple(reg_addr, buffer, 6); // Read 6 bytes for x, y, z
}


/**************************************************************************//**
* @fn			uint16_t read_steps(void)
* @brief		Read the high and low bits of step counter array, combine them, then return step count
* @note
*****************************************************************************/
uint16_t read_steps(void) {
    cs1_enable();
    uint8_t stepbuffer[2];
    spi1_transfer(STEP_COUNTER_L | 0x80);       // read the device
    spi1_read (stepbuffer, 2);                  // read consecutive L and H bits
    cs1_disable();
    return (stepbuffer[1] << 8) | stepbuffer[0]; // return combined bytes
}


