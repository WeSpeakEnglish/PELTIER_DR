#include "buttons.h"


u8 Button1Up = 0;
u8 Button1Down = 0;
u8 Button2Up = 0;
u8 Button2Down = 0;
u8 Button3Up = 0;
u8 Button3Down = 0;
u8 Button4Up = 0;
u8 Button4Down = 0;

void EXTI0_IRQHandler(void) //BUTTON2
{
 static u8 InsideCounter = 0;
// static u16 PortCond =0;

//   delay_1s(); //delay button condition stable
 if(GPIOB->IDR & 0x0001) Button2Up = 1;
 else Button2Down = 1;
EXTI->PR|=0x01; // clear interrupt
InsideCounter++;
}

void EXTI1_IRQHandler(void) //
{
 static u8 InsideCounter = 0;
//   delay_1s(); //delay button condition stable

EXTI->PR|=0x02; // clear interrupt
InsideCounter++;
}

void EXTI2_IRQHandler(void) // BUTTON3
{
 static u8 InsideCounter = 0;

if(GPIOB->IDR & 0x0004) Button3Up = 1;
else Button3Down = 1;
delay_ms(100);
EXTI->PR|=0x04; // clear interrupt
InsideCounter++;
}

void EXTI3_IRQHandler(void) //BUTTON4
{
 static u8 InsideCounter = 0;
//   delay_1s(); //delay button condition stable
if(GPIOB->IDR & 0x0008) Button4Up = 1;
else Button4Down = 1;
EXTI->PR|=0x08; // clear interrupt
InsideCounter++;
}

void EXTI4_IRQHandler(void) // BUTTON1
{
 static u8 InsideCounter = 0;
   delay_1s(); //delay button condition stable
EXTI->PR|=0x10; // clear interrupt
InsideCounter++;
}


u8 TestPlusButton(s16* Temp){
static u8 PressedDelay = 8;
static u8 FlagCount =0;
s16 TempIns = *Temp;
        if (!Button4Up && Button4Down){  // if pressed update and updating again after delay
               // do not BLINK
              if(!FlagCount) TempIns++, FlagCount = 1;
              else
                  if(PressedDelay > 0)PressedDelay--;
                  else   TempIns++;
         }
         else{
          PressedDelay = 8;
          FlagCount = 0;
          Button4Up = Button4Down = 0;
         }
 *Temp = TempIns;
 return FlagCount;
}

u8 TestMinusButton(s16* Temp){
static u8 PressedDelay = 8;
static u8 FlagCount =0;
s16 TempIns = *Temp;
        if (!Button3Up && Button3Down){  // if pressed update and updating again after delay
               // do not BLINK
              if(!FlagCount) TempIns--, FlagCount = 1;
              else
                  if(PressedDelay > 0)PressedDelay--;
                  else   TempIns--;
         }
         else{
          PressedDelay = 8;
          FlagCount = 0;
          Button3Up = Button3Down = 0;
         }
 *Temp = TempIns;
 return FlagCount;
}
