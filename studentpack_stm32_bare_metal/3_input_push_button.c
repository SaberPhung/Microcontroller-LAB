/*Reads PC13 push-button B1 PC13 input in the NUCLEO-L152RE board
and controls led LD2 PA5 (D13) with the push-button
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
void SetSysClock(void);
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

  RCC->AHBENR|=0x1; //GPIOA enable. p154
  RCC->AHBENR|=0x4; //GPIOC enable. p154
  GPIOA->MODER|=0x400; //GPIOA pin 5 to output. p184
  GPIOC->MODER&=~0xC000000; //PC13 configured to input, C=1100. p184

  /* Infinite loop */
  while (1)
  {
	  /*
		  IDR=GPIO port input data register, p186
		  These bits are read-only and can be accessed in word mode only.
		  They contain the input value of the corresponding I/O port.

		  BSRR=GPIO port bit set/reset register
		  Port x reset bit and Port x set bit
		  0: No action on the corresponding ODRx bit
		  1: Resets the corresponding ODRx bit
		  */

		  if(GPIOC->IDR & 0x2000) 	//if PC13 is high state (button released)
		  {
			GPIOA->BSRR=0x200000;	//turn led off (PA5=low state)
		  }
		  else //(button pressed)
		  {
			GPIOA->BSRR=0x20;		//turn led on (PA5=high state)
		  }
  }
  return 0;
}