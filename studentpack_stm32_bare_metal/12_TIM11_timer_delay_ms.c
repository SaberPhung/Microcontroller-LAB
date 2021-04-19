/*
Accurate ms delay function and led blink (with timer TIM11)
Atollic: File/New/(C/C++ project)/C Manged build/Executable Embedded C project/
STM32L1 Boards NUCLEO-L152RE/next/next/next/finish

TIM10 and TIM11 are based on a 16-bit auto-reload upcounter. 
TIM9 is based on a 16-bit auto-reload up/down counter. 
They include a 16-bit prescaler. 
TIM10 and TIM11 feature one independent channel, 
whereas TIM9 has two independent channels for 
input capture/output compare, PWM or one-pulse mode output. 
They can be synchronized with the TIM2, TIM3, TIM4, 
TIM5 full-featured general-purpose timers.
They can also be used as simple time bases 
and be clocked by the LSE clock source (32.768 kHz) 
to provide time bases independent from the main CPU clock.
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

void delay_ms(unsigned long delay);
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

  RCC->AHBENR|=1; 		//GPIOA ABH bus clock ON. p154
  GPIOA->MODER|=0x400; 	//GPIOA pin 5 to output. p184

  /* Infinite loop */
  while (1)
  {
	  GPIOA->ODR^=0x20; //0010 0000 xor bit 5. p186
	  delay_ms(1000);
  }
  return 0;
}

void delay_ms(unsigned long delay)
{
	unsigned long i=0;
	RCC->APB2ENR|=(1<<4); 	//TIM11EN: Timer 11 clock enable. p160
	TIM11->PSC=32-1; 		//32 000 000 MHz / 32 = 1 000 000 Hz. p435
	TIM11->ARR=1000-1; 		//TIM11 counter. 1 000 000 Hz / 1000 = 1000 Hz ~ 1ms. p435
	TIM11->CNT=0;			//counter start value = 0
	TIM11->CR1=1; 			//TIM11 Counter enabled. p421

	  while(i<delay)
	  {
		  while(!((TIM11->SR)&1)){} //Update interrupt flag. p427
		  i++;
		  TIM11->SR &= ~1; 	//flag cleared. p427
		  TIM11->CNT=0;	  	//counter start value = 0
	  }
	  TIM11->CR1=0; 		//TIM11 Counter disabled. p421
}
