/*
Accurate millisecond (ms) delay function and led blink by using timer delay
Atollic: File/New/(C/C++ project)/C Manged build/Executable Embedded C project/
STM32L1 Boards NUCLEO-L152RE/next/next/next/finish

This timer (systick) is dedicated to the OS, but could also be used as a 
standard downcounter.It is based on a 24-bit downcounter with autoreload 
capability and a programmable clock source.
It features a maskable system interrupt generation when the counter reaches 0.
STM32L151xE datasheet page 29.

Systick information can be found Programming manual.pdf page 151.
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

  RCC->AHBENR|=1; //GPIOA ABH bus clock ON. p154
  GPIOA->MODER|=0x400; //GPIOA pin 5 to output. p184

  /* Infinite loop */
  while (1)
  {
	  GPIOA->ODR^=0x20; //0010 0000 xor bit 5 = PA5. p186
	  delay_ms(1000);
  }
  return 0;
}

void delay_ms(unsigned long delay)
{
	unsigned long i=0;
	  SysTick->LOAD=32000-1; //32 000 000 = 1s so 32 000 = 1 ms
		  SysTick->VAL=0;
		  SysTick->CTRL=5; //enable counter, use processor clock, M3 Generic User Guide p. 159
		  
	  while(i<delay)
	  {	
	//CTRL register bit 16 returns 1 if timer counted to 0 since last time this was read.
		  while(!((SysTick->CTRL)&0x10000)){} //M3 Generic User Guide p. 159
		  i++;
	  }
}