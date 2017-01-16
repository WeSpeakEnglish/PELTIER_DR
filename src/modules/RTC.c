#include "RTC.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_bkp.h"

/* Private variables--------------------------------------------------------- */
uint8_t ClockSource;
uint8_t *MonthsNames[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",\
"Sep","Oct","Nov","Dec"};
const uint8_t CalibrationPpm[128]={0,1,2,3,4,5,6,7,8,9,10,10,11,12,13,14,15,16,17,\
                         18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,32,33,34,\
                         35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,51,\
                         52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,\
                         70,71,72,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,\
                         87,88,89,90,91,92,93,93,94,95,96,97,98,99,100,101,102,\
                         103,104,105,106,107,108,109,110,111,112,113,113,114,\
                         115,116,117,118,119,120,121};
/*Structure variable declaration for system time, system date,
alarm time, alarm date */
struct Time_s s_TimeStructVar;
struct AlarmTime_s s_AlarmStructVar;
struct Date_s s_DateStructVar;
struct AlarmDate_s s_AlarmDateStructVar;
u8 TamperEvent = 0;
u16 SummerTimeCorrect = 0x0000;
u8 BatteryRemoved = 0;
/** @addtogroup RTC
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  COnfiguration of RTC Registers, Selection and Enabling of
  *   RTC clock
  * @param  None
  * @retval : None
  */
 // RTC_InitTypeDef   RTC_InitStructure;
//  RTC_AlarmTypeDef  RTC_AlarmStructure;
//  RTC_TimeTypeDef   RTC_TimeStructure;


void RTC_Configuration()
{
  NVIC_InitTypeDef NVIC_InitStructure;

        /* Configure one bit for preemption priority */
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

        /* Enable the RTC Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

        PWR_BackupAccessCmd(ENABLE);
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
        RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
        RTC_WaitForLastTask();
}



/**
  * @brief  Summer Time Correction routine
  * @param  None
  * @retval : None
  */
void SummerTimeCorrection(void)
{
  uint8_t CorrectionPending=0;
  uint8_t CheckCorrect=0;

  if((SummerTimeCorrect & OCTOBER_FLAG_SET)!=0)
  {
    if((s_DateStructVar.Month==10) && (s_DateStructVar.Day >24 ))
    {
      for(CheckCorrect = 25;CheckCorrect <=s_DateStructVar.Day;CheckCorrect++)
      {
        if(WeekDay(s_DateStructVar.Year,s_DateStructVar.Month,CheckCorrect )==0)
        {
          if(CheckCorrect == s_DateStructVar.Day)
          {
            /* Check if Time is greater than equal to 1:59:59 */
            if(RTC_GetCounter()>=7199)
            {
              CorrectionPending=1;
            }
          }
          else
          {
            CorrectionPending=1;
          }
         break;
       }
     }
   }
   else if((s_DateStructVar.Month > 10))
   {
     CorrectionPending=1;
   }
   else if(s_DateStructVar.Month < 3)
   {
     CorrectionPending=1;
   }
   else if(s_DateStructVar.Month == 3)
   {
     if(s_DateStructVar.Day<24)
     {
       CorrectionPending=1;
     }
     else
     {
       for(CheckCorrect=24;CheckCorrect<=s_DateStructVar.Day;CheckCorrect++)
       {
         if(WeekDay(s_DateStructVar.Year,s_DateStructVar.Month,CheckCorrect)==0)
         {
           if(CheckCorrect == s_DateStructVar.Day)
           {
             /*Check if Time is less than 1:59:59 and year is not the same in which
                March correction was done */
             if((RTC_GetCounter() < 7199) && ((SummerTimeCorrect & 0x3FFF) != \
                                   s_DateStructVar.Year))
             {
               CorrectionPending=1;
             }
             else
             {
               CorrectionPending=0;
             }
             break;
            }
            else
            {
              CorrectionPending=1;
            }
          }
        }
      }
    }
  }
  else if((SummerTimeCorrect & MARCH_FLAG_SET)!=0)
  {
    if((s_DateStructVar.Month == 3) && (s_DateStructVar.Day >24 ))
    {
      for(CheckCorrect = 25;CheckCorrect <=s_DateStructVar.Day;\
         CheckCorrect++)
      {
        if(WeekDay(s_DateStructVar.Year,s_DateStructVar.Month,\
           CheckCorrect )==0)
        {
          if(CheckCorrect == s_DateStructVar.Day)
          {
            /*Check if time is greater than equal to 1:59:59 */
            if(RTC_GetCounter()>=7199)
            {
              CorrectionPending=1;
            }
          }
          else
          {
            CorrectionPending=1;
          }
        break;
        }
      }
    }
    else if((s_DateStructVar.Month > 3) && (s_DateStructVar.Month < 10 ))
    {
      CorrectionPending=1;
    }
    else if(s_DateStructVar.Month ==10)
    {
      if(s_DateStructVar.Day<24)
      {
        CorrectionPending=1;
      }
      else
      {
        for(CheckCorrect=24;CheckCorrect<=s_DateStructVar.Day;\
          CheckCorrect++)
        {
          if(WeekDay(s_DateStructVar.Year,s_DateStructVar.Month,\
            CheckCorrect)==0)
          {
            if(CheckCorrect == s_DateStructVar.Day)
            {
              /*Check if Time is less than 1:59:59 and year is not the same in
              which March correction was done */
              if((RTC_GetCounter() < 7199) && \
                ((SummerTimeCorrect & 0x3FFF) != s_DateStructVar.Year))
              {
                CorrectionPending=1;
              }
              else
              {
                CorrectionPending=0;
              }
            break;
            }
          }
        }
      }
    }
  }

  if(CorrectionPending==1)
  {
    if((SummerTimeCorrect & OCTOBER_FLAG_SET)!=0)
    {
      /* Subtract 1 hour from the current time */
      RTC_SetCounter(RTC_GetCounter() - 3599);
      /* Reset October correction flag */
      SummerTimeCorrect &= 0xBFFF;
      /* Set March correction flag  */
      SummerTimeCorrect |= MARCH_FLAG_SET;
      SummerTimeCorrect |= s_DateStructVar.Year;
      BKP_WriteBackupRegister(BKP_DR7,SummerTimeCorrect);
    }
    else if((SummerTimeCorrect & MARCH_FLAG_SET)!=0)
    {
     /* Add 1 hour to current time */
     RTC_SetCounter(RTC_GetCounter() + 3601);
     /* Reset March correction flag */
     SummerTimeCorrect &= 0x7FFF;
     /* Set October correction flag  */
     SummerTimeCorrect |= OCTOBER_FLAG_SET;
     SummerTimeCorrect |= s_DateStructVar.Year;
     BKP_WriteBackupRegister(BKP_DR7,SummerTimeCorrect);
    }
  }
}



/**
  * @brief  Apllication Initialisation Routine
  * @param  None
  * @retval : None
  */
void ApplicationInit(void)
{
  /* System Clocks Configuration */
 // RCC_Configuration();
  /*Enables the clock to Backup and power interface peripherals    */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR,ENABLE);

  /* SysTick Configuration*/
 // SysTickConfig();

  /*Initialisation of TFT LCD */
//  STM3210B_LCD_Init();

  /* Unlock the Flash Program Erase controller */
//  FLASH_Unlock();
  /*RTC_NVIC Configuration */
  RTC_NVIC_Configuration();

  /* RTC Configuration*/
  RTC_Configuration();
  BKP_RTCOutputConfig(BKP_RTCOutputSource_None);

  /* General Purpose I/O Configuration */
  GPIO_Configuration();

  /* Battery Removal Emulation   */
  GPIO_SetBits(GPIOC, GPIO_Pin_8);

  while(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)))
  {
    TamperEvent = 1;
  }

  /* Joystick NVIC Configuration  */

  /* Tamper pin NVIC Configuration  */
  Tamper_NVIC_Configuration();

  /* Configure PVD Supervisor to disable the Tamper Interrupt when voltage drops
  below 2.5 volts*/
  PWR_PVDCmd(ENABLE);
  PWR_PVDLevelConfig(PWR_PVDLevel_2V5);
  PWR_BackupAccessCmd(ENABLE);


  /* Tamper FeatureRTC  -   Enable Tamper Interrupt and configure for Low level */
  BKP_ITConfig(ENABLE);

  /* Enable Tamper Pin for Active low level: Tamper level detected for low level*/
  BKP_TamperPinLevelConfig(BKP_TamperPinLevel_Low);

  /* Enable tamper Pin Interrupt */
  BKP_TamperPinCmd(ENABLE);

  /*  Menu Initialisation  */
  //MenuInit();
}



/**
  * @brief  Sets the RTC Current Counter Value
  * @param Hour, Minute and Seconds data
  * @retval : None
  */
void SetTime(uint8_t Hour,uint8_t Minute,uint8_t Seconds)
{
  uint32_t CounterValue;

  CounterValue=((Hour * 3600)+ (Minute * 60)+Seconds);

  RTC_WaitForLastTask();
  RTC_SetCounter(CounterValue);
  RTC_WaitForLastTask();
}



/**
  * @brief  Sets the RTC Alarm Register Value
  * @param Hours, Minutes and Seconds data
  * @retval : None
  */
void SetAlarm(uint8_t Hour,uint8_t Minute, uint8_t Seconds)
{
  uint32_t CounterValue;

  CounterValue=((Hour * 3600)+ (Minute * 60)+Seconds);

  if(CounterValue == 0)
  {
    CounterValue = SECONDS_IN_DAY;
  }

  RTC_WaitForLastTask();
  RTC_SetAlarm(CounterValue);
  RTC_WaitForLastTask();
}



/**
  * @brief  Sets the RTC Date(DD/MM/YYYY)
  * @param DD,MM,YYYY
  * @retval : None
  */
void SetDate(uint8_t Day, uint8_t Month, uint16_t Year)
{
  uint32_t DateTimer;

  //RightLeftIntExtOnOffConfig(DISABLE);
 // UpDownIntOnOffConfig(DISABLE);

  /*Check if the date entered by the user is correct or not, Displays an error
    message if date is incorrect  */
  if((( Month==4 || Month==6 || Month==9 || Month==11) && Day ==31) \
    || (Month==2 && Day==31)|| (Month==2 && Day==30)|| \
      (Month==2 && Day==29 && (CheckLeap(Year)==0)))
  {
//    LCD_Clear(Red);
 //   LCD_SetBackColor(Red);
 //   LCD_DisplayString(Line3,Column2,"INCORRECT DATE");
//    LCD_DisplayString(Line6,Column1,"PLEASE RECONFIGURE");
    DateTimer=RTC_GetCounter();

    while((RTC_GetCounter()-DateTimer)<2)
    {
    }
  }
  /* if date entered is correct then set the date*/
  else
  {

      BKP_WriteBackupRegister(BKP_DR2,Month);
      BKP_WriteBackupRegister(BKP_DR3,Day);
      BKP_WriteBackupRegister(BKP_DR4,Year);


     s_AlarmDateStructVar.Day = Day;
     s_AlarmDateStructVar.Month = Month;
     s_AlarmDateStructVar.Year = Year;
     BKP_WriteBackupRegister(BKP_DR8,Month);
     BKP_WriteBackupRegister(BKP_DR9,Day);
     BKP_WriteBackupRegister(BKP_DR10,Year);

   }
}



/**
  * @brief  RTC Application runs in while loop
  * @param  None
  * @retval : None
  */
void RTC_Application(void)
{
//  uint8_t ReturnValue;

  CalculateTime();


  if(TamperEvent == 1) /* Tamper event is detected */
  {
  //  TamperNumber = TamperNumber + 1;
 //   BKP_WriteBackupRegister(BKP_DR5,TamperNumber);
 //   BKP_WriteBackupRegister(BKP_DR1, CONFIGURATION_DONE);
 //   BKP_WriteBackupRegister(BKP_DR2,s_DateStructVar.Month);
 //   BKP_WriteBackupRegister(BKP_DR3,s_DateStructVar.Day);
 //   BKP_WriteBackupRegister(BKP_DR4,s_DateStructVar.Year);
 //   BKP_WriteBackupRegister(BKP_DR9,s_AlarmDateStructVar.Day);
 //   BKP_WriteBackupRegister(BKP_DR8,s_AlarmDateStructVar.Month);
 //   BKP_WriteBackupRegister(BKP_DR10,s_AlarmDateStructVar.Year);
//    BKP_WriteBackupRegister(BKP_DR6,ClockSource);
//    BKP_WriteBackupRegister(BKP_DR7,SummerTimeCorrect);
 //   ReturnValue=EE_WriteVariable(TamperNumber);

/*
    ReturnValue=EE_WriteVariable(s_DateStructVar.Day);
    ReturnValue=EE_WriteVariable(s_DateStructVar.Month);
    ReturnValue=EE_WriteVariable((s_DateStructVar.Year)/100);
    ReturnValue=EE_WriteVariable((s_DateStructVar.Year)%100);
    ReturnValue=EE_WriteVariable((s_TimeStructVar.HourHigh*10)+\
                    s_TimeStructVar.HourLow);
    ReturnValue=EE_WriteVariable((s_TimeStructVar.MinHigh*10)+\
                    s_TimeStructVar.MinLow);
    ReturnValue=EE_WriteVariable((s_TimeStructVar.SecHigh*10)+\
                    s_TimeStructVar.SecLow);
    while(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)))
    {
    }

    TamperEvent=0;
    */
    BKP_TamperPinCmd(ENABLE);
  }

  if(BatteryRemoved != 0) /* Battery tamper is detected */
  {
    BKP_TamperPinCmd(ENABLE);
  }
}



/**
  * @brief  This function is executed after wakeup from STOP mode
  * @param  None
  * @retval : None
  */
void ReturnFromStopMode(void)
{
  /* RCC Configuration has to be called after waking from STOP Mode*/
  //RCC_Configuration();
  /*Enables the clock to Backup and power interface peripherals after Wake Up */
 // RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR,ENABLE);
  /* Enable access to Backup Domain */
 // PWR_BackupAccessCmd(ENABLE);

}



/**
  * @brief Updates the Date (This function is called when 1 Day has elapsed
  * @param None
  * @retval :None
  */
void DateUpdate(void)
{
  s_DateStructVar.Month=BKP_ReadBackupRegister(BKP_DR2);
  s_DateStructVar.Year=BKP_ReadBackupRegister(BKP_DR4);
  s_DateStructVar.Day=BKP_ReadBackupRegister(BKP_DR3);

  if(s_DateStructVar.Month == 1 || s_DateStructVar.Month == 3 || \
    s_DateStructVar.Month == 5 || s_DateStructVar.Month == 7 ||\
     s_DateStructVar.Month == 8 || s_DateStructVar.Month == 10 \
       || s_DateStructVar.Month == 12)
  {
    if(s_DateStructVar.Day < 31)
    {
      s_DateStructVar.Day++;
    }
    /* Date structure member: s_DateStructVar.Day = 31 */
    else
    {
      if(s_DateStructVar.Month != 12)
      {
        s_DateStructVar.Month++;
        s_DateStructVar.Day = 1;
      }
     /* Date structure member: s_DateStructVar.Day = 31 & s_DateStructVar.Month =12 */
      else
      {
        s_DateStructVar.Month = 1;
        s_DateStructVar.Day = 1;
        s_DateStructVar.Year++;
      }
    }
  }
  else if(s_DateStructVar.Month == 4 || s_DateStructVar.Month == 6 \
            || s_DateStructVar.Month == 9 ||s_DateStructVar.Month == 11)
  {
    if(s_DateStructVar.Day < 30)
    {
      s_DateStructVar.Day++;
    }
    /* Date structure member: s_DateStructVar.Day = 30 */
    else
    {
      s_DateStructVar.Month++;
      s_DateStructVar.Day = 1;
    }
  }
  else if(s_DateStructVar.Month == 2)
  {
    if(s_DateStructVar.Day < 28)
    {
      s_DateStructVar.Day++;
    }
    else if(s_DateStructVar.Day == 28)
    {
      /* Leap Year Correction */
      if(CheckLeap(s_DateStructVar.Year))
      {
        s_DateStructVar.Day++;
      }
      else
      {
        s_DateStructVar.Month++;
        s_DateStructVar.Day = 1;
      }
    }
    else if(s_DateStructVar.Day == 29)
    {
      s_DateStructVar.Month++;
      s_DateStructVar.Day = 1;
    }
  }

  BKP_WriteBackupRegister(BKP_DR2,s_DateStructVar.Month);
  BKP_WriteBackupRegister(BKP_DR3,s_DateStructVar.Day);
  BKP_WriteBackupRegister(BKP_DR4,s_DateStructVar.Year);
}



/**
  * @brief  Checks whether the passed year is Leap or not.
  * @param  None
  * @retval : 1: leap year
  *   0: not leap year
  */
uint8_t CheckLeap(uint16_t Year)
{
  if((Year%400)==0)
  {
    return LEAP;
  }
  else if((Year%100)==0)
  {
    return NOT_LEAP;
  }
  else if((Year%4)==0)
  {
    return LEAP;
  }
  else
  {
    return NOT_LEAP;
  }
}



/**
  * @brief Determines the weekday
  * @param Year,Month and Day
  * @retval :Returns the CurrentWeekDay Number 0- Sunday 6- Saturday
  */
uint16_t WeekDay(uint16_t CurrentYear,uint8_t CurrentMonth,uint8_t CurrentDay)
{
  uint16_t Temp1,Temp2,Temp3,Temp4,CurrentWeekDay;

  if(CurrentMonth < 3)
  {
    CurrentMonth=CurrentMonth + 12;
    CurrentYear=CurrentYear-1;
  }

  Temp1=(6*(CurrentMonth + 1))/10;
  Temp2=CurrentYear/4;
  Temp3=CurrentYear/100;
  Temp4=CurrentYear/400;
  CurrentWeekDay=CurrentDay + (2 * CurrentMonth) + Temp1 \
     + CurrentYear + Temp2 - Temp3 + Temp4 +1;
  CurrentWeekDay = CurrentWeekDay % 7;

  return(CurrentWeekDay);
}



/**
  * @brief Displays the Date(DD/MM/YY and DAY ) on LCD
  * @param None
  * @retval :None
  */




/**
  * @brief Displays the Time on LCD
  * @param None
  * @retval :None
  */





/**
  * @brief Calcuate the Time (in hours, minutes and seconds  derived from
  *   COunter value
  * @param None
  * @retval :None
  */
void CalculateTime(void)
{
  uint32_t TimeVar;

  TimeVar=RTC_GetCounter();
  TimeVar=TimeVar % 86400;
  s_TimeStructVar.HourHigh=(uint8_t)(TimeVar/3600)/10;
  s_TimeStructVar.HourLow=(uint8_t)(TimeVar/3600)%10;
  s_TimeStructVar.MinHigh=(uint8_t)((TimeVar%3600)/60)/10;
  s_TimeStructVar.MinLow=(uint8_t)((TimeVar%3600)/60)%10;
  s_TimeStructVar.SecHigh=(uint8_t)((TimeVar%3600)%60)/10;
  s_TimeStructVar.SecLow=(uint8_t)((TimeVar %3600)%60)%10;
}



/**
  * @brief Chaeks is counter value is more than 86399 and the number of
  *   elapsed and updates date that many times
  * @param None
  * @retval :None
  */
void CheckForDaysElapsed(void)
{
  uint8_t DaysElapsed;

  if((RTC_GetCounter() / SECONDS_IN_DAY) != 0)
  {
    for(DaysElapsed = 0; DaysElapsed < (RTC_GetCounter() / SECONDS_IN_DAY)\
         ;DaysElapsed++)
    {
      DateUpdate();
    }

    RTC_SetCounter(RTC_GetCounter() % SECONDS_IN_DAY);
  }
}



/**
  * @brief  Calibration of External crystal oscillator manually
  * @param  None
  * @retval : None
  */


/**
  * @brief  Configures the Tamper Interrupts
  * @param  None
  * @retval : None
  */
void Tamper_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TAMPER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



/**
  * @brief  Configures RTC Interrupts
  * @param  None
  * @retval : None
  */
void RTC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  EXTI_DeInit();

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI Line17 Interrupt */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  EXTI_ClearITPendingBit(EXTI_Line16 );
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line16;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


 void TAMPER_IRQHandler(void)
{
  BKP_TamperPinCmd(DISABLE);

  TamperEvent = 1;

  BKP_ClearITPendingBit();

  BKP_ClearFlag();

  NVIC_ClearPendingIRQ(TAMPER_IRQn);
}

/**
  * @brief  This function handles RTC_IRQHandler .
  * @param  None
  * @retval : None
  */
void RTC_IRQHandler(void)
{
  uint8_t Month,Day;
  uint16_t Year;

  Month = BKP_ReadBackupRegister(BKP_DR2);
  Day = BKP_ReadBackupRegister(BKP_DR3);
  Year = BKP_ReadBackupRegister(BKP_DR4);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  RTC_ClearITPendingBit(RTC_IT_SEC);

  /* If counter is equal to 86399: one day was elapsed */
  /* This takes care of date change and resetting of counter in case of
  power on - Run mode/ Main supply switched on condition*/
  if(RTC_GetCounter() == 86399)
  {
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Reset counter value */
    RTC_SetCounter(0x0);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Increment the date */
    DateUpdate();
  }

  if((RTC_GetCounter()/3600 == 1)&&(((RTC_GetCounter()%3600)/60) == 59)&&
     (((RTC_GetCounter()%3600)%60) == 59))
  {
    /* March Correction */
    if((Month == 3) && (Day >24))
    {
      if(WeekDay(Year,Month,Day)==0)
      {
        if((SummerTimeCorrect & 0x8000) == 0x8000)
        {
          RTC_SetCounter(RTC_GetCounter() + 3601);

          /* Reset March correction flag */
          SummerTimeCorrect &= 0x7FFF;

          /* Set October correction flag  */
          SummerTimeCorrect |= 0x4000;
          SummerTimeCorrect |= Year;
          BKP_WriteBackupRegister(BKP_DR7,SummerTimeCorrect);
        }
      }
    }
      /* October Correction */
    if((Month == 10) && (Day >24))
    {
      if(WeekDay(Year,Month,Day)==0)
      {
        if((SummerTimeCorrect & 0x4000) == 0x4000)
        {
          RTC_SetCounter(RTC_GetCounter() - 3599);

          /* Reset October correction flag */
          SummerTimeCorrect &= 0xBFFF;

          /* Set March correction flag  */
          SummerTimeCorrect |= 0x8000;
          SummerTimeCorrect |= Year;
          BKP_WriteBackupRegister(BKP_DR7,SummerTimeCorrect);
        }
      }
    }
  }
}



