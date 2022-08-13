#ifndef _i2c_h
#define _i2c_h
#include "stm32f10x.h"

void I2c1_Init(void );
uint8_t mcu_i2c_read_byte(uint8_t device_address);
void mcu_i2c_write_byte(uint8_t device_address, uint8_t value);
void mcu_i2c_write_two_byte(uint8_t device_address, uint8_t value_a, uint8_t value_b);
#endif

