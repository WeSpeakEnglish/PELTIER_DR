#include "uart.h"



char write_UART1(unsigned char * SMS){
  u32 tmp=0;
     while (SMS[tmp]) // Пока не конец строки...
     {
          while (!(USART1->SR & USART_SR_TXE)) {} // Ждать освобождения буфера.
          USART1->DR=SMS[tmp]; tmp++; // Отправить байт.
     }
return 0;
}

char Init_UART1(void){
     RCC->APB2ENR|= RCC_APB2ENR_IOPAEN; // Разрешить тактирование порта A.
     RCC->APB2ENR|= RCC_APB2ENR_AFIOEN; // Тактирование альтернативных функций GPIO.
     RCC->APB2ENR|= RCC_APB2ENR_USART1EN; // Включение тактирования USART1.
     GPIOA->CRH |= GPIO_CRH_MODE9; // Вывод TX PA.9 - на выход.
     GPIOA->CRH &=~GPIO_CRH_CNF9; GPIOA->CRH |=GPIO_CRH_CNF9_1; // Альтернативный выход.
     USART1->CR1 |=(USART_CR1_RE | USART_CR1_TE); // Разрешить выводы RX, TX - PA10, PA9.
     // Скорость 115.2 kbps. USARTDIV=FSYS/(16*baud) = 8e6/(16*115200) = 4,34.
     USART1->BRR=(4<<4); // Целая часть коэффициента деления USART1. 
     USART1->BRR|=5; // Дробная часть*16 = 0,34*16 = 5 (окр).
     USART1->CR1 |=USART_CR1_UE; // Включение USART1.
return 0;
}