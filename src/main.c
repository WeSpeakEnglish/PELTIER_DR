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

//GPIO_TypeDef * PORT_Temp;

void IWDG_init(void){
 IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
 IWDG_SetPrescaler(IWDG_Prescaler_32); //32000/32 = 1 kHz clock
 IWDG_SetReload(1999); //2 sek
 IWDG_ReloadCounter();
 IWDG_Enable();
}


int main(void)
{
IWDG_init();
   RCC->CR|=RCC_CR_HSEON; // �������� ��������� HSE.
   while (!(RCC->CR & RCC_CR_HSERDY)) {} // �������� ���������� HSE.
   RCC->CFGR &=~RCC_CFGR_SW; // �������� ���� SW0, SW1.
   RCC->CFGR |= RCC_CFGR_SW_HSE; // ������� HSE ��� ������������ SW0=1.
 // RCC->CR   |=  RCC_CR_HSION;            //�������� ��������� HSI
  //while((RCC->CR & RCC_CR_HSIRDY)==0) {} //�������� ���������� HSI
 // RCC->CFGR &= ~RCC_CFGR_SW;             //������� ����� ������ ��������� ��������� �������
  //RCC->CFGR |=  RCC_CFGR_SW_HSI;         //������� ���������� ��������� ������� HSI
  //��������� �������� ��� AHB
  RCC->CFGR &= ~RCC_CFGR_HPRE;           //������� ����� ������������ "AHB Prescaler"
  RCC->CFGR |=  RCC_CFGR_HPRE_DIV4;      //���������� "AHB Prescaler" ������ 8
// RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
  PortAConf();
  PortCConf();
  PortBConf();
  I2c1_Init();
  ButtonConf(); // button enable/disable signal
 // lcd_init();

  TIM2_init();  // for TEST SIGNAL
  TIM3_init();

  Init_UART1();
  ConfigureDAC1();
  ConfigureLCD();
  bmp085_init();

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

//SHT10_Config();

     while(1){

     }

}
