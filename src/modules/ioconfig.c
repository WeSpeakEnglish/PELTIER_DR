#include "ioconfig.h"



void ButtonConf(void){
RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN , ENABLE);
AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI0_PB;
AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI1_PB;
AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI2_PB;
AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI3_PB;
AFIO->EXTICR[1]|=AFIO_EXTICR2_EXTI4_PB;

EXTI->IMR|=(EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR3 | EXTI_IMR_MR4);
EXTI->RTSR|=(EXTI_RTSR_TR0 | EXTI_RTSR_TR1 | EXTI_RTSR_TR2 | EXTI_RTSR_TR3 | EXTI_RTSR_TR4);
EXTI->FTSR|=(EXTI_FTSR_TR0 | EXTI_FTSR_TR1 | EXTI_FTSR_TR2 | EXTI_FTSR_TR3 | EXTI_FTSR_TR4);
NVIC_EnableIRQ (EXTI0_IRQn);
NVIC_EnableIRQ (EXTI1_IRQn);
NVIC_EnableIRQ (EXTI2_IRQn);
NVIC_EnableIRQ (EXTI3_IRQn);
NVIC_EnableIRQ (EXTI4_IRQn);
return;
}



void PortCConf(void){
 RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;            //clock to the GPIOC
 GPIOC->CRL = 0x88333333; // push-pull output 50 MHz (PC0...PC3, PC4,5; )
 GPIOC->ODR &= ~0x00C0; //PC6,7 - pull down input
 return;
}
void PortBConf(void){
 RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;            //clock to the GPIOB

    //PB0 As Input
  GPIOB->CRL &=~GPIO_CRL_MODE0;
  GPIOB->CRL &=~GPIO_CRL_CNF0;
  GPIOB->CRL |=GPIO_CRL_CNF0_1; //дискретный вход с подт€гивающим резистором
  GPIOB->BSRR |=  GPIO_BSRR_BS0; // подт€жка к 1
      //PB1 As Input
  GPIOB->CRL &=~GPIO_CRL_MODE1;
  GPIOB->CRL &=~GPIO_CRL_CNF1;
  GPIOB->CRL |=GPIO_CRL_CNF1_1; //дискретный вход с подт€гивающим резистором
  GPIOB->BSRR |=  GPIO_BSRR_BS1; // подт€жка к 1

      //PB2 As Input
  GPIOB->CRL &=~GPIO_CRL_MODE2;
  GPIOB->CRL &=~GPIO_CRL_CNF2;
  GPIOB->CRL |=GPIO_CRL_CNF2_1; //дискретный вход с подт€гивающим резистором
  GPIOB->BSRR |=  GPIO_BSRR_BS2; // подт€жка к 1


  //disconnect JTAG

RCC -> APB2ENR |= RCC_APB2ENR_AFIOEN;
AFIO -> MAPR |= AFIO_MAPR_SWJ_CFG_1;
//PB3 As Input
  GPIOB->CRL &=~GPIO_CRL_MODE3;
  GPIOB->CRL &=~GPIO_CRL_CNF3;
  GPIOB->CRL |=GPIO_CRL_CNF3_1; //дискретный вход с подт€гивающим резистором
  GPIOB->BSRR |=  GPIO_BSRR_BS3; // подт€жка к 1

      //PB4 As Input
  GPIOB->CRL &=~GPIO_CRL_MODE4;
  GPIOB->CRL &=~GPIO_CRL_CNF4;
  GPIOB->CRL |=GPIO_CRL_CNF4_1; //дискретный вход с подт€гивающим резистором
  GPIOB->BSRR |=  GPIO_BSRR_BS4; // подт€жка к 1



 //outputs is 2 MHz LCD in this case
  GPIOB->CRH &=~(GPIO_CRH_CNF8_0);
  GPIOB->CRH |=(GPIO_CRH_MODE8_1);

  GPIOB->CRH &=~(GPIO_CRH_CNF9_0);
  GPIOB->CRH |=(GPIO_CRH_MODE9_1);

  GPIOB->CRH &=~(GPIO_CRH_CNF12_0);
  GPIOB->CRH |=(GPIO_CRH_MODE12_1);

  GPIOB->CRH &=~(GPIO_CRH_CNF13_0);
  GPIOB->CRH |=(GPIO_CRH_MODE13_1);

  GPIOB->CRH &=~(GPIO_CRH_CNF14_0);
  GPIOB->CRH |=(GPIO_CRH_MODE14_1);

  GPIOB->CRH &=~(GPIO_CRH_CNF15_0);
  GPIOB->CRH |=(GPIO_CRH_MODE15_1);
/////////////////////////////////////////// DRIVE PELTIER //////////////////////////////
   GPIOA->CRL &=~(GPIO_CRL_CNF6|GPIO_CRL_MODE6);
  GPIOA->CRL |= GPIO_CRL_MODE6_1;
  GPIOA->ODR|=GPIO_ODR_ODR6;  // open drain opened

 return;
}
void PortAConf(void){
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;            //тактирование линий GPIOA
  //outputs is 2 MHz

  //PA0
 GPIOA->CRL &= ~GPIO_CRL_CNF0;
 GPIOA->CRL |= (GPIO_CRL_CNF0_0); //open drain
 GPIOA->CRL |=(GPIO_CRL_MODE0_1)|(GPIO_CRL_MODE0_0); //strong (50 MHz)
 //GPIOA->ODR|=GPIO_ODR_ODR0; // Up
   //PA1
 GPIOA->CRL &= ~GPIO_CRL_CNF1;
 GPIOA->CRL |= (GPIO_CRL_CNF1_0); //open drain
 GPIOA->CRL |= (GPIO_CRL_MODE1_1)|(GPIO_CRL_MODE1_0); //strong (50 MHz)
 //GPIOA->ODR |= GPIO_ODR_ODR1; // Up
 //PA2
 GPIOA->CRL &= ~GPIO_CRL_CNF2;
 GPIOA->CRL |= GPIO_CRL_MODE2_0; //push-pull 10MHz
 GPIOA->ODR &= ~GPIO_ODR_ODR2;
 //PA3
 GPIOA->CRL &= ~GPIO_CRL_CNF3;
 GPIOA->CRL |= GPIO_CRL_MODE3_0; //push-pull 10MHz
 GPIOA->ODR &= ~GPIO_ODR_ODR3;




 return;
}
