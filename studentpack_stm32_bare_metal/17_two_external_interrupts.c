/*
Two external interrupts with inputs PC13 and PA10:
Push-button B1 input PC13 creates first falling edge interrupt.
PA10 (D2) has internal pull-up resistor activated and
low-state in PA10 (D2) creates second falling edge interrupt. 
This software prints (UART) text when interrupt detected.
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

//push-button interrupt
GPIOC->MODER&=~0x0C000000;		//clear (input state for PC13). p184
SYSCFG->EXTICR[3] &= ~0x00f0;	//clear port selection for EXTI13. p223
SYSCFG->EXTICR[3] |= 0x0020;	//select port C (PC13) for EXTI13. p223
EXTI->IMR |= 0x2000;			//unmask EXTI13 (MR13), p242
EXTI->FTSR |= 0x2000;			//select falling edge trigger, p243

//PA10 (D2) interrupt
GPIOA->MODER&=~0x00300000;		//clear (input state for PA10). p184
GPIOA->PUPDR|=0x00100000;		//pull-up for PA10 (bit order 01). p185
SYSCFG->EXTICR[2] &= ~0x0f00;	//clear port selection for EXTI10. p222
SYSCFG->EXTICR[2] &= ~0x0f00;	//select port A for EXT10 (clear all 4 bits). p222
EXTI->IMR |= 0x0400;			//unmask EXT10 (MR10)
EXTI->FTSR |= 0x0400;			//select falling edge trigger, p243

NVIC_EnableIRQ(EXTI15_10_IRQn); //enable IRQ M3_Generic_User_Guide p130

__enable_irq();					//global enable IRQs, M3_Generic_User_Guide p135
GPIOA->MODER&=~0x00000C00;		//clear (input reset state for PA5). p184
GPIOA->MODER|=0x400; 			//GPIOA pin 5 to output. p184
GPIOA->ODR^=0x20;				//0010 0000 xor bit 5. p186
delay_Ms(1000);
GPIOA->ODR^=0x20;				//0010 0000 xor bit 5. p186
delay_Ms(1000);

  /* Infinite loop */
   while (1)
  {

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

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR==0x2000)
	{
		char buf[]="PC13 push-button falling edge interrupt!";

		for(int i=0;i<strlen(buf);i++)
		{
			USART_write(buf[i]);
		}

		USART_write('\n');
		USART_write('\r');

		EXTI->PR=0x2000;		//Pending, This bit is cleared by writing a ‘1’ to the bit. p245
	}
	else if(EXTI->PR==0x0400)
	{
		char buf[]="A10 (D2) falling edge interrupt!";

		for(int i=0;i<strlen(buf);i++)
		{
			USART_write(buf[i]);
		}

		USART_write('\n');
		USART_write('\r');

		EXTI->PR=0x0400;		//Pending, This bit is cleared by writing a ‘1’ to the bit. p245
	}

	GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186
	delay_Ms(100);
	GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186
	delay_Ms(100);
}