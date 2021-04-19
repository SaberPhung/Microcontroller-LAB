/*
led LD2 blink PA5 (D13) with BSRR register
*/

/* Includes */
#include <stddef.h>
#include "stm32l1xx.h"
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

  /* Infinite loop */
  while (1)
  {
	  GPIOA->BSRR|=0x20; //set PA5, BSRR bit number 5, BS5 bit set. p186
	  delay_Ms(1000);
	  GPIOA->BSRR|=0x200000; //reset PA5, BSRR bit number 21, BR5 bit set. p186
	  delay_Ms(1000);
  }
  return 0;
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); //measured with oscilloscope
}