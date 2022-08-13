#include "peltier.h"
#include "delays.h"
#include "lcd.h"


u8 Curse = StopPelt; // Zero es
#define PA0_OUT (*((volatile unsigned long *) 0x42210180)) //bit-band
#define PA1_OUT (*((volatile unsigned long *) 0x42210184)) //bit-band
#define PA2_OUT (*((volatile unsigned long *) 0x42210188)) //bit-band
#define PA3_OUT (*((volatile unsigned long *) 0x4221018C)) //bit-band

void ControlPeltier(s16 Tmin, s16 Tmax, s16 Temper){


static u8 OldCurse = StopPelt;
static u8 DoStep = 0;

 if(Temper < Tmin) Curse = HotPelt;
 if(Temper > Tmax) Curse = ColdPelt;


 if(Curse == ColdPelt)
     if( Temper > Tmin) Curse = ColdPelt; //frost still get lowest
     else  Curse = StopPelt;
  if(Curse == HotPelt)
     if( Temper < Tmax) Curse = HotPelt; //frost still get lowest
     else  Curse = StopPelt;
 //  if(Temper<=Tmax && Temper>=Tmin) Curse = StopPelt;


 if(OldCurse != Curse){
         DoStep = 0; OldCurse = Curse;
 }
if(!DoStep)
 switch (Curse){
    case 0:
 //      switch (DoStep){
        PA2_OUT = 0;
        PA3_OUT = 0;
        PA0_OUT = 0;
        PA1_OUT = 0;
        lcd_set_xy(8,0);
        lcd_out("Stopped");
        DoStep++;
     break;
    case 1:
        PA2_OUT = 0;
        PA3_OUT = 0;
        PA0_OUT = 0;
        PA1_OUT = 0;
      //  delay_ms(2);
        lcd_set_xy(8,0);
        PA0_OUT = 1;  //GND TO -
        lcd_out("Cooling");
        PA3_OUT = 1;  //PLUS TO +
        DoStep++;
       break;
    case 2:
        PA2_OUT = 0;
        PA3_OUT = 0;
        PA0_OUT = 0;
        PA1_OUT = 0;
        lcd_set_xy(8,0);
        PA1_OUT = 1; //GND TO +
        lcd_out("Warming");
        PA2_OUT = 1; //PLUS TO -
        DoStep++;
     break;



 }


return;
}

