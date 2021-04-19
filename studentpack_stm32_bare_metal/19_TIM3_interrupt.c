/*
TIM3 interrupt and toggle led LD2 (PA5) inside interrupt function.
Atollic: File/New/(C/C++ project)/C Manged build/Executable Embedded C project/
STM32L1 Boards NUCLEO-L152RE/next/next/next/finish

TIM2, TIM3, TIM4 are based on 16-bit auto-reload up/down counter.
TIM5 is based on a 32-bit auto-reload up/down counter.
They include a 16-bit prescaler.
They feature four independent channels each for input
capture/output compare, PWM or one-pulse mode output.
This gives up to 16 input captures/output compares/PWMs on the largest packages.
STM32L151xE datasheet page 28

The general-purpose timers consist of a 16-bit or
32-bit auto-reload counter driven by a programmable prescaler.
They may be used for a variety of purposes,
including measuring the pulse lengths of input
signals (input capture) or generating output waveforms
(output compare and PWM).
Pulse lengths and waveform periods can be modulated
from a few microseconds to several milliseconds using
the timer prescaler and the RCC clock controller prescalers.
p. 383

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

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
	__disable_irq();

  /* Configure the system clock to 32 MHz and update SystemCoreClock */
  SetSysClock();
  SystemCoreClockUpdate();

  /* TODO - Add your application code here */

  RCC->AHBENR|=1; 		//GPIOA ABH bus clock ON. p154
  GPIOA->MODER|=0x400; 	//GPIOA pin 5 to output. p184
  
  RCC->APB1ENR |=2;		//enable TIM3
  TIM3->PSC=32000-1;	//divided by 32000
  TIM3->ARR=1000-1;		//divided by 1000
  TIM3->CR1=1;			//enable counter
  TIM3->DIER|=1;		//enable UIE
  
  NVIC_EnableIRQ(TIM3_IRQn);
  
	__enable_irq();		//global enable IRQs, M3_Generic_User_Guide p135
  /* Infinite loop */
  while (1)
  {
	
  }
  return 0;
}

void TIM3_IRQHandler(void)
{
	TIM3->SR=0;			//clear UIF
	GPIOA->ODR^=0x20; 	//0010 0000 xor bit 5 PA5. p186
}