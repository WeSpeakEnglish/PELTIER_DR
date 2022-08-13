#include "stm32F10x.h"
#include "stm32f10x_i2c.h"
#include "uart.h"
#include "delays.h"
#include "timers.h"
#include "ioconfig.h"
#include "i2c.h"
#include "bmp085.h"
#include "hd44780_driver.h"
#include "LCD.h"
#include "dac.h"
#include "sht1x.h"
#include "flash.h"
#include "ds18b20.h"
//GPIO_TypeDef * PORT_Temp;




int main(void)
{
    u32 Counter = 0;
 //  RCC->CR|=RCC_CR_HSEON; // Включить генератор HSE.
 //  while (!(RCC->CR & RCC_CR_HSERDY)) {} // Ожидание готовности HSE.
 //  RCC->CFGR &=~RCC_CFGR_SW; // Очистить биты SW0, SW1.
 //  RCC->CFGR |= RCC_CFGR_SW_HSE; // Выбрать HSE для тактирования SW0=1.
  RCC->CR   |=  RCC_CR_HSION;            //Включить генератор HSI
  while((RCC->CR & RCC_CR_HSIRDY)==0) {} //Ожидание готовности HSI
  RCC->CFGR &= ~RCC_CFGR_SW;             //Очистка битов выбора источника тактового сигнала
  RCC->CFGR |=  RCC_CFGR_SW_HSI;         //Выбрать источником тактового сигнала HSI
  //Настроить делитель для AHB
  RCC->CFGR &= ~RCC_CFGR_HPRE;           //Очистка битов предделителя "AHB Prescaler"
  RCC->CFGR |=  RCC_CFGR_HPRE_DIV4;      //Установить "AHB Prescaler" равным 8
// RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
  PortAConf();
  PortCConf();
  PortBConf();
 // I2c1_Init();
  ButtonConf(); // button enable/disable signal
 // lcd_init();

  TIM3_init();
  TIM4_init();

  Init_UART1();
  ConfigureDAC1();
  ConfigureLCD();
  //bmp085_init();


///////////////////////////////

    TempsLimits[0] = GetTmin();
    TempsLimits[1] = GetTmax();


////////////////////////////////
  lcd_set_xy(0,0);
  lcd_send(0,DATA); // temp sumbol to LCD
//     lcd_out("      ");
    // lcd_send(4,DATA); // press sumbol to LCD
  lcd_set_xy(0,1);

//     lcd_send(0,DATA); // temp sumbol to LCD
  lcd_send(4,DATA); // MIN sumbol to LCD

  DisplayTemperature(GetTmin());
  lcd_set_xy(8,1);
  lcd_send(5,DATA); // MAX sumbol to LCD

  DisplayTemperature(GetTmax());
     //lcd_send(6,DATA); // hum sumbol to LCD
  ds_start_convert_single();
  TIM4->CNT=0;

  while(TIM4->CNT<990){};

  Temper = ds_read_temperature();   //прочитать результат измерения
//SHT10_Config();
  TIM2_init();  // for TEST SIGNAL

     while(1){

  ds_start_convert_single();
  TIM4->CNT=0;

  while(TIM4->CNT<990){};

  Temper = ds_read_temperature();   //прочитать результат измерения
     }

}
