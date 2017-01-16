#ifndef _ds18b20
#define _ds18b20
#include "stm32f10x.h"

signed int ds_read_temperature(void);
uint8_t ds_read_data_single(uint8_t *buff);
uint8_t ds_start_convert_single(void);
uint8_t ds_read_data_single(uint8_t *buff);
uint8_t ds_reset_pulse(uint16_t PinMask);
void ds_write_bit(uint8_t bit,uint16_t PinMask);
uint16_t ds_read_bit(uint16_t PinMask);
void ds_write_byte(uint8_t byte, uint16_t PinMask);
uint8_t ds_read_byte(uint16_t PinMask);
void TempPortTimerInit(GPIO_TypeDef * PortName, u8 LineNumber, TIM_TypeDef * Timer);
#endif