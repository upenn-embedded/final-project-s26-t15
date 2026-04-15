/*
 * imu.h
 *
 * Created: 4/9/2026 1:03:45 PM
 *  Author: Jerry Zhang
 */ 

#include "stm32f4xx.h" // Defines aliases
#include <stdint.h>
#include "spi.h"

#ifndef IMU_H_ // the LSM6DS0 on SPI mode
#define IMU_H_

#define IMU_WHO_AM_I 0x0F           // WHO_AM_I register address, should always return 0x6C
#define IMU_ID 0x6C                 // device address

// define the performance settings of the device
#define IMU_CTRL1_XL 0x10           // Accelerometer control register
#define IMU_ODR 0x20                // set the performance mode to 26hz
#define IMU_FS 0x00                 // 2g reading mode

#define IMU_CTRL10_C 0x19           // Control register for step counter, set bit 5 to enable time stamp
#define STEP_COUNTER_L 0x62         // step counter low bits
#define STEP_COUNTER_H 0x63         // step counter high bits
#define EMB_FUNC_CFG_ACCESS 0x01    // enable access to the embedded functions
#define FUNC_CFG_ACCESS  (1 << 7)   // shift by 7 to get the FUNC_CFG_ACCESS bit

// pedometer enable
#define EMB_FUNC_EN_A 0x04          // enable embedded functions reg A
#define IMU_PEDO_EN (1 << 3)        // enable for pedometer is 3
#define IMU_TIMESTAMP_EN (1 << 5)

uint8_t imu_init(void);             // initialize all imu settings, verify device id
uint16_t read_steps(void);          // read the step counter value
//uint8_t imu_write;
void imu_write(uint8_t reg_addr, uint8_t data); // write a byte to a register address on the IMU
uint8_t imu_read(uint8_t reg_addr); // read a byte from a register address on the IMU
void imu_read_multiple(uint8_t reg_addr, uint8_t *buffer, uint32_t size); // read several bytes from address
void imu_readxyz(uint8_t reg_addr, uint8_t *buffer); // read 6 bytes of data for x, y, z axis readings
void imu_clearsteps(void);		// clear step counter


#endif