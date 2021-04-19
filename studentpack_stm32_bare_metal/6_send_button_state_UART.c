/*This code tests NUCLEO-L152RE board UART transmit communication with 9600 BAUD.
The button B1 sends text to UART and toggles LD2 state.

-USART2 connected to APB1
-APB1: USART2. USART2 can be found from block diagram in STM32L152RET6_datasheet.pdf.

-Nucleo board user manual: By default the USART2 communication between the target
STM32 and ST-LINK MCU is enabled, in order to support virtual COM-
-STM32L152RET6 USART2 is connected to GPIOA PA2(TX),PA3(RX) and in Nucleo board D0,D1 (Arduino)

- RCC_APB1ENR bit 17 will enable USART2. p158
- USART registers. p736
- Baud rate register (USART_BRR) and Control register (USART_CR1). p739
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
void USART2_Init(void);
void USART_write(char data);
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
  RCC->AHBENR|=0x1; //GPIOA enable. p154
  RCC->AHBENR|=0x4; //GPIOC enable. p154
  GPIOA->MODER|=0x400; //GPIOA pin 5 to output. p184
  GPIOC->MODER&=~0xC000000; //makes sure that pc13 is input, 0000 1100 etc. p184

  char my_array[7]="led off";
  char my_array2[6]="led on";
  char state=0;
  /* Infinite loop */
  while (1)
  {
	  	if(!(GPIOC->IDR & 0x2000)) 	//if PC13 is low state (button pressed)
		  {
	  		if(state==0)
	  		{
				GPIOA->BSRR=0x200000;	//turn led off (PA5=low state)
				for(int i=0;i<7;i++)
				{
					USART_write(my_array[i]); //send text "led off";

				}
				USART_write('\n');
				USART_write('\r');
				state=1;
	  		}
	  		else if(state==1)
	  		{
				GPIOA->BSRR=0x20;		//turn led on (PA5=high state)
				for(int i=0;i<6;i++)
				{
					USART_write(my_array2[i]); //send text "led on";
				}
				USART_write('\n');
				USART_write('\r');
				state=0;
	  		}
	  		delay_Ms(20); //switch noise
			while(!(GPIOC->IDR & 0x2000)){} //wait until button released
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

	USART2->BRR = 0x00000D05;	//9600 BAUD and crystal 32MHz. p710, D05
	USART2->CR1 = 0x00000008;	//TE bit. p739-740. Enable transmit
	USART2->CR1 |= 0x00000004;	//RE bit. p739-740. Enable receiver
	USART2->CR1 |= 0x00002000;	//UE bit. p739-740. Uart enable
}

void USART_write(char data)
{
	//wait while TX buffer is empty
	while(!(USART2->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART2->DR=(data);			//p739
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); //measured with oscilloscope
}
