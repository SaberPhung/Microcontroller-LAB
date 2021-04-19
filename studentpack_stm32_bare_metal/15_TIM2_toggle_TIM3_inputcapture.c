/*
This software toggles PA1 (arduino pin A1) with TIM2 output compare 
and captures this signal with TIM3 input capture PA6 (arduino pin D12). 
Connect jumper wire between pins PA1-PA6 (D12-A1).
This software prints (uart 115200 BAUD) PA1 signal period by using unit (ms).

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
#include <stdio.h>

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
	USART2_Init();

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
TIM2->ARR=1000-1; 			//TIMx auto-reload register. 1 000 Hz / 1000 = 1 Hz ~ 1s. p435

//CCMR1=TIMx capture/compare mode register 1, Bits 14:12 OC2M[2:0]: Output compare 2 mode
TIM2->CCMR1=0x3000;			//011: Toggle - OC2REF toggles when TIMx_CNT=TIMx_CCR1. p430
TIM2->CCR2=0;				//TIM2 capture/compare register 2. p435

TIM2->CCER|=0x10;			//Bit 4 CC2E: Capture/Compare 1 output enable. p434
TIM2->CNT=0;				//counter start value = 0
TIM2->CR1=1; 				//TIMx control register 1, TIM2 Counter enabled. p421

//3000 hex = 11 00 00 00 00 00 00, corresponds PA6,PA5,PA4,PA3,PA2,PA1,PA0
GPIOA->MODER&=~0x00003000;	//clear (input reset state for PA6). p184
GPIOA->MODER|=0x00002000; 	//GPIOA pin 1 (PA6) to alternate function. p184
GPIOA->AFR[0]&=~0x0F000000; //clear AFRL6 bits. p188

//TIM3_CH1 connected to PA6 (AFIO2), STM32L152RET6 datasheet p47.
GPIOA->AFR[0]|=0x02000000; 	//AF2 (TIM3) GPIOA pin 1 (PA1). p177, p188

RCC->APB1ENR|=2; 		//TIM3EN: Timer 3 clock enable. p160
TIM3->PSC=32000-1; 		//32 000 000 MHz / 32 000 = 1 000 Hz. p435
TIM3->CCMR1=0x41;		//01: CC1 channel is configured as input, IC1 is mapped on TI1. p431
TIM3->CCER|=0x3;		//A0B1 bits: bit3=A=0,bit1=B=1: falling edge and bit0=1 Capture/Compare 1 output enable, p434
TIM3->CNT=0;			//counter start value = 0
TIM3->CR1=1; 			//TIMx control register 1, TIM3 Counter enabled. p421

GPIOA->MODER|=0x400; 	//GPIOA pin 5 to output. p184
GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186
delay_Ms(1000);
GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186
delay_Ms(1000);

unsigned int captured_value_ms=0;
char buf[6]={0};

  /* Infinite loop */
   while (1)
  {
	while(!((TIM3->SR) & 2)){} 		//This bit is set by hardware on a capture. p427
		TIM3->CNT=0;				//counter start value = 0
		captured_value_ms=TIM3->CCR1;	//CCR1 is the counter value transferred by the last input capture 1 event. p436

		sprintf(buf,"%d",captured_value_ms);

		for(int i=0;i<5;i++)
		{
			USART_write(buf[i]);
		}
		USART_write('\n');
		USART_write('\r');
		GPIOA->ODR^=0x20;		//0010 0000 xor bit 5. p186
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
