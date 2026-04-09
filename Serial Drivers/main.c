/*
 * main.c
 *  Created on: 04/09/2026
 *  Author: Jerry Zhang
 *  Description: Main file
 * 
 */ 


#include "spi.h"
#include "imu.h"

int main(void)
{
    spi1_init(); // Initialize SPI1 for IMU communication
    imu_init();  // Initialize IMU settings

    while (1) {
        uint16_t steps = read_steps(); // Read step count from IMU
    }
}