#include "timers.h"
#include "LCD.h"
#include "hd44780_driver.h"
#include "sht1x.h"
#include "uart.h"
#include "bmp085.h"
#include "ioconfig.h"
#include "buttons.h"
#include "flash.h"
#include "peltier.h"
#include "ds18b20.h"
//////////////////////////////////////////////
u16 Hum;
u16 Temp;
u8 Crc;
float CalculatedHum;
float CalculatedTemp;
enum {TEMP=1,HUM};

/////////////////////////
s16 TempsLimits[2];


//////////////////////////////////////////////////


void TIM2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //разрешить подачу тактовых импульсов на TIM2
    TIM2->CNT = 0;
    TIM2->SR = 0;
    TIM2->CR2=0x0;          //обнуляем регистр
    TIM2->CR1=0x0;          //обнуляем регистр

    TIM2->PSC = 249;       // предделитель таймера 2
    TIM2->ARR = 1000;      //

    TIM2->CR1 &= ~TIM_CR1_DIR;      //прямой счёт - 0 (обратный - 1) отсчёт
    TIM2->CR1 &= ~TIM_CR1_UDIS;      //запр.-1; разрешаем событие по перепол.таймера -0
    TIM2->CR1 &= ~TIM_CR1_CKD;      //clock division
    TIM2->CR1 &= ~TIM_CR1_ARPE;      //запрещаем 0 - когда прграмно не надо менять значение ARR;разрешаем=1 перзагрузку
    TIM2->CR1 &= ~TIM_CR1_OPM;      //Запрещаем работу в режиме одиночного импульса
    TIM2->DIER |= TIM_DIER_UIE;      //&UPDATE INTERRPUT - прерывание по переполнению &&&

    TIM2->CR1 |= TIM_CR1_URS;
    TIM2->EGR &= ~TIM_EGR_UG;      // сбрасываем 0 ;update enable  ARR и прескалер грузятся из предварительного в теневой регистр
    TIM2->CR1 |= TIM_CR1_CEN;       //запускаем таймер2
    NVIC_EnableIRQ (TIM2_IRQn);    //разрешаем прерывание

}

void TIM3_init(void) // make delays
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   //подать тактирование на TIM3                           /
    TIM3->PSC     = 1;                  //настроить делитель для формирования микросекунд
    TIM3->ARR     = 1000;
    TIM3->CR1     = TIM_CR1_CEN;
}

void TIM4_init(void) // make delays
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;   //подать тактирование на TIM3                           /
    TIM4->PSC     = 1999;                  //настроить делитель для формирования микросекунд
    TIM4->ARR     = 1000;
    TIM4->CR1     = TIM_CR1_CEN;
}

void TIM3_IRQHandler(void)
{
    static u16 InsideCounter = 0;

    if (TIM3->SR & TIM_SR_UIF)
    {
        TIM3->SR &= ~TIM_SR_UIF;        // очищаем флаг прерывания
    }



    InsideCounter++;
}

void TIM2_IRQHandler(void)
{
    static u8 SetCondition = 0;
    static u16 InternalCounter = 0;



    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;        // очищаем флаг прерывания
    }



    if(Button2Up && Button2Down)
    {
        Button2Up = Button2Down = 0;

        lcd_set_xy(1,1);
        DisplayTemperature(TempsLimits[0]);
        lcd_set_xy(9,1);
        DisplayTemperature(TempsLimits[1]);
        SetCondition ++;
    }

    if(!(InternalCounter%4))
    {
        lcd_set_xy(1,0);
        DisplayTemperature(Temper);
    }

    switch (SetCondition)
    {
    case 0:
        if(Temper > -1000)//some delay follow
            if(InternalCounter > 10)ControlPeltier(TempsLimits[0], TempsLimits[1],Temper); // Test and drive PELTIER
        break;
    case 1:
        // bliking string MIN

        if(TestMinusButton(TempsLimits)||TestPlusButton(TempsLimits))
        {
            lcd_set_xy(1,1);
            DisplayTemperature(TempsLimits[0]);
            if(TempsLimits[0] > TempsLimits[1]) TempsLimits[1] = TempsLimits[0];
        }
        else
        {
            if(InternalCounter%8 == 4)
            {
                lcd_set_xy(1,1);
                lcd_out("      ");
            }
            if(InternalCounter%8 == 0)
            {
                lcd_set_xy(1,1);
                DisplayTemperature(TempsLimits[0]);
            }

        }

        break;
    case 2:
        if(TestMinusButton(&TempsLimits[1])||TestPlusButton(&TempsLimits[1]))
        {
            lcd_set_xy(9,1);
            DisplayTemperature(TempsLimits[1]);
            if(TempsLimits[0] > TempsLimits[1]) TempsLimits[0] = TempsLimits[1];
        }
        else
        {
            if(InternalCounter%8 == 4)
            {
                lcd_set_xy(9,1);
                lcd_out("      ");
            }
            if(InternalCounter%8 == 0)
            {
                lcd_set_xy(9,1);
                DisplayTemperature(TempsLimits[1]);
            }

        }
        break;
    case 3:


        FLASH_WriteInts(ADDR_TMIN, (uint16_t *)TempsLimits, 2);

        SetCondition = 0;
        Curse = StopPelt;
        break;
    }
    if(InternalCounter < 256)InternalCounter++;
    else InternalCounter = 16;
}
