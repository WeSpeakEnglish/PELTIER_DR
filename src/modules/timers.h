#ifndef _timers_h
#define _timers_h
#include "stm32f10x.h"

s16 TempsLimits[2];

void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM2_init(void);
void TIM3_init(void);

#endif


