/*3.6 chapter solved
*/

/* Includes */
#include <stddef.h>
#include "stm32l1xx.h"
#include "nucleo152start.h"
#include <stdio.h>

/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
void USART2_Init(void);
void USART2_write(char data);
void delay_Ms(int delay);

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  /* Configure the system clock to 32 MHz and update SystemCoreClock */
  SetSysClock();
  SystemCoreClockUpdate();
  USART2_Init();
  /* TODO - Add your application code here */
char my_array[40];

for(int i=0;i<40;i++)
{
	my_array[i]=i;
}

RCC->AHBENR|=1; //GPIOA ABH bus clock ON. p154
GPIOA->MODER|=0x400; //GPIOA pin 5 to output. p184

RCC->AHBENR|=0x4; //GPIOC enable. p154
GPIOA->MODER|=0x400; //GPIOA pin 5 to output. p184
GPIOC->MODER&=~0xC000000; //PC13 configured to input, C=1100. p184
int a=1;
  /* Infinite loop */
  while (1)
  {
	  if(!(GPIOC->IDR & 0x2000)) 	//if PC13 is low state (button pressed)
	  {
		  delay_Ms(20); //noise
		  while(!(GPIOC->IDR & 0x2000)){} //wait until button released
		  delay_Ms(20); //noise
		  while((GPIOC->IDR & 0x2000)&&a==1) //button released
		  {
			  for(int i=0;i<40;i++)
			  {
				  USART2_write(my_array[i]);
		  	  if(!(GPIOC->IDR & 0x2000))
		  		  a=0;
			  }

			  GPIOA->ODR|=0x20; //0010 0000 set bit 5. p186

		  	  if(!(GPIOC->IDR & 0x2000))
		  		  a=0;

			  delay_Ms(100);

		  	  GPIOA->ODR&=~0x20; //0000 0000 clear bit 5. p186

		  	  if(!(GPIOC->IDR & 0x2000))
		  		  a=0;

		  	  delay_Ms(100);

		  	  if(!(GPIOC->IDR & 0x2000))
		  		  a=0;
		  }
		  while(!(GPIOC->IDR & 0x2000)){}//wait until button released
		  a=1;
	  }


  }
  return 0;
}

void USART2_Init(void)
{
	RCC->APB1ENR|=0x00020000; 	//set bit 17 (USART2 EN)
	RCC->AHBENR|=0x00000001; 	//enable GPIOA port clock bit 0 (GPIOA EN)
	GPIOA->AFR[0]=0x00000700;	//GPIOx_AFRL p.188,AF7 p.177
	GPIOA->AFR[0]|=0x00007000;	//GPIOx_AFRL p.188,AF7 p.177
	GPIOA->MODER|=0x00000020; 	//MODER2=PA2(TX) to mode 10=alternate function mode. p184
	GPIOA->MODER|=0x00000080; 	//MODER2=PA3(RX) to mode 10=alternate function mode. p184

	USART2->BRR = 0x00000341;	//38400 BAUD and crystal 32MHz. p710, D05
	USART2->CR1 = 0x00000008;	//TE bit. p739-740. Enable transmit
	USART2->CR1 |= 0x00000004;	//RE bit. p739-740. Enable receiver
	USART2->CR1 |= 0x00002000;	//UE bit. p739-740. Uart enable
}

void USART2_write(char data)
{
	//wait while TX buffer is empty
	while(!(USART2->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART2->DR=(data);		//p739
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); //measured with oscilloscope
}
