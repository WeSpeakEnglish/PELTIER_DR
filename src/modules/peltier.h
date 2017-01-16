#ifndef PELTIER_H_INCLUDED
#define PELTIER_H_INCLUDED

#include "stm32F10x.h"
enum {StopPelt =0, ColdPelt, HotPelt};
extern u8 Curse;
void ControlPeltier(s16 Tmin, s16 Tmax, s16 Temper);

#endif /* PELTIER_H_INCLUDED */
