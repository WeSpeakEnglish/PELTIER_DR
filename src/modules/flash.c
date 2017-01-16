#include "flash.h"
#include "stm32f10x_flash.h"


void FLASH_WriteInts(uint32_t addr , uint16_t *p , uint16_t Ints_Num)
 {
 	uint32_t HalfWord;
 	FLASH_Unlock();
 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
 	FLASH_ErasePage(addr);
 	while(Ints_Num --)
 	{
 		HalfWord=*(p++);
 		FLASH_ProgramHalfWord(addr, HalfWord);
 		addr += 2;
 	}
 	FLASH_Lock();
 }

void FLASH_ReadInts(uint32_t addr , uint16_t *p , uint16_t Ints_Num)
 {
   while(Ints_Num--)
   {
    *(p++)=*((uint16_t*)addr);
     addr+=2;
   }
 }

void SetTminTmax(int16_t Tm){
 FLASH_WriteInts(ADDR_TMIN, (uint16_t *)&Tm, 2);
 return;
}
//void SetTmax(int16_t Tmax){
// FLASH_WriteInts(ADDR_TMAX, (uint16_t *)&Tmax, 1);
// return;
//}
int16_t GetTmin(void){
return *((int16_t*)ADDR_TMIN);
}
int16_t GetTmax(void){
return *((int16_t*)ADDR_TMIN + 1);
}
