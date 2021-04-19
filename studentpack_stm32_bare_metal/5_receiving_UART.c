/*This code tests NUCLEO-L152RE board UART receive communication by using
115200 BAUD.

Character 'S' ascii will lit LD2 and character 'R' will turn off LD2. 

S=set
R=reset

-USART2 connected to APB1
-APB1: USART2. USART2 can be found from block diagram in STM32L152RET6_datasheet.pdf
page 13.

-Nucleo board user manual: By default the USART2 communication between the target
STM32 and ST-LINK MCU is enabled, in order to support virtual COM-
-STM32L152RET6 USART2 is connected to GPIOA PA2(TX),PA3(RX) 
and in Nucleo board D0,D1 (Arduino)

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
char USART2_read(void);
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
  char received_command=0;
  RCC->AHBENR|=0x1; 	//GPIOA enable. p154
  GPIOA->MODER|=0x400; 	//GPIOA pin 5 to output. p184
  /* Infinite loop */
  GPIOA->BSRR=0x20;		//turn led on (PA5=high state)
  delay_Ms(1000);
  GPIOA->BSRR=0x200000;	//turn led off (PA5=low state)

  while (1)
  {
		received_command=USART2_read();

		if(received_command == 'R')
		{
			GPIOA->BSRR=0x200000;	//turn led off (PA5=low state)

		}

		if(received_command == 'S')
		{
			GPIOA->BSRR=0x20;		//turn led on (PA5=high state)

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

	USART2->BRR = 0x00000116;	//115200 BAUD and crystal 32MHz. p710, D05
	USART2->CR1 = 0x00000008;	//TE bit. p739-740. Enable transmit
	USART2->CR1 |= 0x00000004;	//RE bit. p739-740. Enable receiver
	USART2->CR1 |= 0x00002000;	//UE bit. p739-740. Uart enable
}

char USART2_read()
{
	char data=0;
	//wait while RX buffer is data is ready to be read
	while(!(USART2->SR&0x0020)){} 	//Bit 5 RXNE: Read data register not empty
		data=USART2->DR;			//p739
		return data;
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); 		//measured with oscilloscope
}