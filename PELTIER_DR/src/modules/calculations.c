#include "calculations.h"

#define MIDDLELENGTH 16

u32 getMiddle(u32 Value){
static u32 MiddleArray[MIDDLELENGTH];
static u8 subPointer=0;
static u32 outputValue=0;
static u8 index=0;
static u8 counter=0;

MiddleArray[subPointer] = Value;
if(counter < MIDDLELENGTH) counter++;
outputValue = MiddleArray[0];
for(index = 1; index < counter; index++){
outputValue += MiddleArray[index];
}
outputValue++;

subPointer++;

subPointer = subPointer&0x0F;

return outputValue/counter;
}

