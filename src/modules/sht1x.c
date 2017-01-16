#include "sht1x.h"

void SDA_H(void){

GPIOB->BSRR=GPIO_BSRR_BS11;
}

void SDA_L(void){
GPIOB->BSRR=GPIO_BSRR_BR11;
}

void SCK_H(void){

GPIOB->BSRR=GPIO_BSRR_BS10;
}

void SCK_L(void){
GPIOB->BSRR=GPIO_BSRR_BR10;
}


u32 SDA_R(void){
return (GPIOB->IDR)&(1<<11);
}

void SHT10_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
  
    //PB11 SDA
          GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
          GPIO_Init(GPIOB, &GPIO_InitStructure);
          //PB10 SCK
          GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;    
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
          GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void SHT10_SDAIn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //PB7 SDA 浮动输入，外部有上拉电阻
          GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
          GPIO_Init(GPIOB, &GPIO_InitStructure);   
}


void SHT10_SDAOut(void)
{
          GPIO_InitTypeDef GPIO_InitStructure;
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
          GPIO_Init(GPIOB, &GPIO_InitStructure);
}


uint8_t SHT10_WriteByte(uint8_t value)
{ 
    uint8_t i,error=0;  
    SHT10_SDAOut();
    
    for( i = 0x80 ; i>0 ; i/=2)             
    { 
        if ( i & value) 
            SDA_H();            
        else 
            SDA_L();
        
        SHT10_Delay();                       
        SCK_H();                          
        SHT10_Delay();                       
        SCK_L();
        SHT10_Delay();                     
    }
    

    SHT10_SDAIn();
    SCK_H();                            
    error = SDA_R();                  
    SCK_L(); 
    
    return error;                 
}


uint8_t SHT10_ReadByte(uint8_t Ack)
{ 
    uint8_t i,val=0;
    SHT10_SDAIn();  
    
    for (i=0x80;i>0;i/=2)         
    { 
        SHT10_Delay();  
        SCK_H();   
        SHT10_Delay();  
        if (SDA_R()) 
            val=(val | i);        
        SCK_L();                                           
    }
    
    SHT10_SDAOut();  
    if(Ack)
        SDA_L();                
    else
        SDA_H();

    SHT10_Delay();  
    SCK_H();                     
    SHT10_Delay();  
    SCK_L();
    SHT10_Delay();                                              
    return val;
}

void SHT10_Start(void)
{  
    SHT10_SDAOut();
   
    SCK_L();                   
    SHT10_Delay();          
    SCK_H();
    SHT10_Delay();          
    SDA_L();
    SHT10_Delay();          
    SCK_L();  
    SHT10_Delay();          
    SCK_H();
    SHT10_Delay();          
    SDA_H();                   
    SHT10_Delay();          
    SCK_L();                   
}

void SHT10_ConReset(void)
{
    uint8_t i;
    SHT10_SDAOut();
    
    SDA_H();    
    SCK_L();
    
    for(i = 0 ; i < 9 ; i++)                  
    { 
        SCK_H();
        SHT10_Delay();
        SCK_L();
        SHT10_Delay();
    }
    
    SHT10_Start();                   
}


uint8_t SHT10_SoftReset(void)
{ 
    uint8_t error=0;  
    SHT10_ConReset();              
    error += SHT10_WriteByte(0x1e);       
    return error;                    
}

uint8_t SHT10_Measure(uint16_t* pValue, uint8_t* pCheckSum, uint8_t mode)
{ 
    uint8_t error=0;
    
    uint8_t Value_H = 0;
    uint8_t Value_L = 0;
    SHT10_Start();                
    switch(mode)
    {                     
    case 1: 
        error += SHT10_WriteByte(0x03); 
        break;
    case 2: 
        error += SHT10_WriteByte(0x05); 
        break;
    default: 
        break;         
    }
    SHT10_SDAIn();
    while(SDA_R())
    {
        ;
    }

    Value_H = SHT10_ReadByte(1);    //读高位
    Value_L = SHT10_ReadByte(1);    //读低位
   
    *pCheckSum = SHT10_ReadByte(0);  //读校验结果
    *pValue = (Value_H << 8) | Value_L;    
    
    return error;
}


void SHT10_Cal(uint16_t Temp,uint16_t Hum, float* pTempValue,float* pHumValue)
{ 
    const float d1 = -39.65;
    const float d2 = 0.01;
    float Temp_C;
    const float C1 = -2.0468;           
    const float C2 = +0.0367;           
    const float C3 = -0.0000015955;     
    const float T1 = +0.01;             
    const float T2 = +0.00008;          
    float RH_Lin;
    float RH_True;  
       
    Temp_C = d1 + d2 * Temp;   
    
    RH_Lin = C1 + C2 * Hum + C3 * Hum *Hum;
    RH_True = (Temp_C - 25) * (T1 + T2 * Hum) + RH_Lin;
    if( RH_True > 100 ) RH_True = 100; 
    if( RH_True < 0.01) RH_True = 0.01;
    
    *pTempValue = Temp_C;
    *pHumValue = RH_True;
    
}

void SHT10_Delay(void)
{
    uint16_t i=0;
    for( i = 125 ; i > 0 ; i--)
    {
        ;
    }
}































