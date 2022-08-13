#ifndef _sht1x_h
#define _sht1x_h

#include "stm32f10x.h"

void SHT10_Config(void);
void SHT10_SDAIn(void);
void SHT10_SDAOut(void);
uint8_t SHT10_WriteByte(uint8_t value);
uint8_t SHT10_ReadByte(uint8_t Ack);
void SHT10_Start(void);
void SHT10_ConReset(void);
uint8_t SHT10_SoftReset(void);
uint8_t SHT10_Measure(uint16_t* pValue, uint8_t* pCheckSum, uint8_t mode);
void SHT10_Cal(uint16_t Temp,uint16_t Hum, float* pTempValue,float* pHumValue);
void SHT10_Delay(void);


#endif




























