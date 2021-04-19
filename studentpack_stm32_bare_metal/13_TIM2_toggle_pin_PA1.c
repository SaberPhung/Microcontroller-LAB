/*
Toggle pin PA1 (arduino pin A1) with TIM2 output compare (every 10s toggle pin PA1)
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
GPIOA->MODER&=~0x0000000C;	//clear (input reset state for PA1). p184
GPIOA->MODER|=0x00000008; 	//GPIOA pin 1 (PA1) to alternate function. p184
GPIOA->AFR[0]&=~0x000000F0;	//clear AFRL1 bits. p188

//TIM2_CH2 connected to PA1, STM32L152RET6 datasheet p47.
GPIOA->AFR[0]|=0x00000010; 	//AF1 (TIM2) GPIOA pin 1 (PA1). p177, p188

//Timers table can be found form STM32L152RET6 datasheet p28.
//Functional overwiev STM32L152RET6 datasheet p13

//Timer block diagram p384
RCC->APB1ENR|=1; 			//TIM2EN: Timer 2 clock enable. p160
TIM2->PSC=32000-1; 			//TIMx prescaler: 32 000 000 MHz / 32 000 = 1 000 Hz. p435
TIM2->ARR=10000-1; 			//TIMx auto-reload register. 1 000 Hz / 10000 = 0,1 Hz ~ 10s. p435

//CCMR1=TIMx capture/compare mode register 1, Bits 14:12 OC2M[2:0]: Output compare 2 mode
TIM2->CCMR1=0x3000;			//011: Toggle - OC2REF toggles when TIMx_CNT=TIMx_CCR2. p430
TIM2->CCR2=0;				//TIM2 capture/compare register 2. p435

//OC2 signal is output on the corresponding output pin. p434
TIM2->CCER|=0x10;			//Bit 4 CC2E: Capture/Compare 1 output enable. p434
TIM2->CNT=0;				//counter start value = 0
TIM2->CR1=1; 				//TIMx control register 1, TIM2 Counter enabled. p421

  /* Infinite loop */
  while (1)
  {

  }
  return 0;
}
