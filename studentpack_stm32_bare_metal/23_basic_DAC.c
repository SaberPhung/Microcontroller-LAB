/*
basic DAC for PA4 (A2). DAC gives 0-3.3V and uses 12-bit mode.

Atollic: File/New/(C/C++ project)/C Manged build/Executable Embedded C project/
STM32L1 Boards NUCLEO-L152RE/next/next/next/finish
*/

/* Includes */

#include "stm32l1xx.h"
#define HSI_VALUE    ((uint32_t)16000000)
#include "nucleo152start.h"

/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

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

  /* TODO - Add your application code here */

  RCC->AHBENR|=1; //GPIOA ABH bus clock ON. p154
  GPIOA->MODER|=0x400; //GPIOA pin 5 to output. p184

		int data=0;
		GPIOA->MODER |= 0x00000300;		//PA4 analog
		RCC->APB1ENR |=1<<29; 			//enable DAC clock
		DAC->CR|=1;						//enable DAC
		DAC->CR|=2;						//disable Buffer

  /* Infinite loop */
  while (1)
  {
	  DAC->DHR12R1=data++;
	  delay_Ms(10);

	  if(data>=4095)
	  {
		  data=0;
	  }
  }
  return 0;
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); //measured with oscilloscope
}