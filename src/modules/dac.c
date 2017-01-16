#include "dac.h"
#include "stm32f10x.h"
#include "stm32f10x_dac.h"

 #define  RCC_APB1ENR_DACEN                  ((uint32_t)0x20000000)        /*!< DAC interface clock enable */

void ConfigureDAC1(void){
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;     // тактирование линий GPIOA
RCC->APB1ENR |= RCC_APB1ENR_DACEN;      // включить тактирование ЦАПа
//DAC->CR |= DAC_CR_BOFF1;        // выкл выходной буфер.
DAC->CR      |= DAC_CR_EN1;             // включить канал №1
//DAC->CR      |= DAC_CR_EN2;             // включить канал №2
DAC->DHR8R1   = 0x30;                   // на выходе канала 1 
}