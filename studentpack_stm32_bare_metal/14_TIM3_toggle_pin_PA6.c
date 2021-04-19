/*
Toggle pin PA6 (Arduino pin D12) with TIM3 output compare.
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

Timer system block diagram p384

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
  /* Configure the system clock to 32 MHz and update SystemCoreClock */
  SetSysClock();
  SystemCoreClockUpdate();

  /* TODO - Add your application code here */

RCC->AHBENR|=1; 			//GPIOA ABH bus clock ON. p154

//3000 hex = 11 00 00 00 00 00 00, corresponds PA6,PA5,PA4,PA3,PA2,PA1,PA0
GPIOA->MODER&=~0x00003000;	//clear (input reset state for PA6). p184
GPIOA->MODER|=0x00002000; 	//GPIOA pin 1 (PA6) to alternate function. p184
GPIOA->AFR[0]&=~0x0F000000; //clear AFRL6 bits. p188

//TIM3_CH1 connected to PA6 (AFIO2), STM32L152RET6 datasheet p47.
GPIOA->AFR[0]|=0x02000000; 	//AF2 (TIM3) GPIOA pin 6 (PA6). p177, p188

//Timers table can be found form STM32L152RET6 datasheet p28.
//Functional overwiev STM32L152RET6 datasheet p13

//Timer block diagram p384

RCC->APB1ENR|=2; 			//TIM3EN: Timer 3 clock enable. p160
TIM3->PSC=32000-1; 			//32 000 000 MHz / 32 000 = 1 000 Hz. p435
TIM3->ARR=1000-1; 			//TIMx auto-reload register. 1 000 Hz / 1000 = 1 Hz ~ 1s. p435
TIM3->CCMR1=0x30;			//011: Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1. p430
TIM3->CCR1=0;				//TIM3 capture/compare register 1. p435

//OC1 signal is output on the corresponding output pin. p434
TIM3->CCER|=1;				//Bit 0 CC1E: Capture/Compare 1 output enable. p434
TIM3->CNT=0;				//counter start value = 0
TIM3->CR1=1; 				//TIMx control register 1, TIM3 Counter enabled. p421

  /* Infinite loop */
  while (1)
  {

  }
  return 0;
}