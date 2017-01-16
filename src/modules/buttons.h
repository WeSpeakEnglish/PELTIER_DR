#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED
#include "stm32F10x.h"

extern u8 Button1Up;
extern u8 Button1Down;
extern u8 Button2Up;
extern u8 Button2Down;
extern u8 Button3Up;
extern u8 Button3Down;
extern u8 Button4Up;
extern u8 Button4Down;

u8 TestPlusButton(s16* Temp);
u8 TestMinusButton(s16* Temp);
#endif /* BUTTONS_H_INCLUDED */
