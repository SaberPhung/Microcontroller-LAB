/*
Push-button B1 (PC13) creates risign and falling edge interrupt.
This software prints (UART, 115200 BAUD) text when interrupt detected.
*/

/* Includes */

#include "stm32l1xx.h"
#define HSI_VALUE    ((uint32_t)16000000)
#include "nucleo152start.h"
#include <stdio.h>
#include <string.h>

/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
void delay_Ms(int delay);
void USART_write(char data);
void USART2_Init(void);
/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
	__disable_irq();			//global disable IRQs, M3_Generic_User_Guide p135.
	USART2_Init();

  /* Configure the system clock to 32 MHz and update SystemCoreClock */
  SetSysClock();
  SystemCoreClockUpdate();

  /* TODO - Add your application code here */

RCC->AHBENR|=1; 				//GPIOA ABH bus clock ON. p154
RCC->AHBENR|=4;					//GPIOC ABH bus clock ON. p154
RCC->APB2ENR |=1;				//Bit 0 SYSCFGEN: System configuration controller clock enable. p157

GPIOA->MODER&=~0x00000C00;		//clear (input reset state for PA5). p184
GPIOA->MODER|= 0x00000400; 		//GPIOA pin 5 (PA5) to output. p184

GPIOA->MODER&=~0x00000300;		//clear (input reset state for PA4). p184
GPIOA->MODER|=0x00000300; 		//GPIOA pin 4 (PA4) to ANALOG MODE. p184

GPIOA->MODER&=~((1<<15)|(1<<14));	//GPIOA pin 7 (PA7) to GPIO. p184

GPIOA->MODER &= ~0x00003000;	//clear (input state for PA6). p184

SYSCFG->EXTICR[1] &= ~0x0F00;	//clear port selection for EXTI6. p222

EXTI->IMR |= 0x0040;			//unmask EXTI6 (MR6), p242
//EXTI->FTSR |= 0x0040;			//select falling edge trigger, p243
EXTI->RTSR |= 0x0040;			//select risign edge trigger, p243

NVIC_EnableIRQ(EXTI9_5_IRQn); //enable IRQ M3_Generic_User_Guide p130

//some info about interrupt system for stm32f4
//https://stm32f4-discovery.net/2014/08/stm32f4-external-interrupts-tutorial/

__enable_irq();			//global enable IRQs, M3_Generic_User_Guide p135

  /* Infinite loop */
   while (1)
  {
		GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186
		delay_Ms(1000);
  }
  return 0;
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); 	//measured with oscilloscope
}

void USART2_Init(void)
{
	RCC->APB1ENR|=0x00020000; 	//set bit 17 (USART2 EN)
	RCC->AHBENR|=0x00000001; 	//enable GPIOA port clock bit 0 (GPIOA EN)
	GPIOA->AFR[0]=0x00000700;	//GPIOx_AFRL p.188,AF7 p.177
	GPIOA->MODER|=0x00000020; 	//MODER2=PA2(TX) to mode 10=alternate function mode. p184

	USART2->BRR = 0x00000116;	//115200 BAUD and crystal 32MHz. p710, 116
	USART2->CR1 = 0x00000008;	//TE bit. p739-740. Enable transmit
	USART2->CR1 |= 0x00002000;	//UE bit. p739-740. Uart enable
}

void USART_write(char data)
{
	//wait while TX buffer is empty
	while(!(USART2->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART2->DR=(data);			//p739
}

void EXTI9_5_IRQHandler(void)
{
	//delay_Ms(20); //switch noise
	if((GPIOA->IDR & 0x0080)==0) //if input PA7 state logical 0 = 0V
	{
		char buf[]="PA7 is in low state";

		for(int i=0;i<strlen(buf);i++)
		{
			USART_write(buf[i]);
		}

		USART_write('\n');
		USART_write('\r');
	}
	else
	{
		char buf[]="PA7 is in high state";

		for(int i=0;i<strlen(buf);i++)
		{
			USART_write(buf[i]);
		}

		USART_write('\n');
		USART_write('\r');
	}
	GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186
	delay_Ms(10);
	GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186

	EXTI->PR=0x0040;		//Pending, This bit is cleared by writing a �1� to the bit. p245
}
