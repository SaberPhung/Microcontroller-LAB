/*This code tests NUCLEO-L152RE board two SWV and UART communication.

Serial Wire Viewer is a real-time trace technology that
uses the Serial Wire Debugger (SWD) port and the Serial
Wire Output (SWO) pin. Serial Wire Viewer provides advanced
system analysis and real-time tracing without the need to halt
the processor to extract the debug information.

-USART2 connected to APB1
-APB1: USART2. USART2 can be found from block diagram in STM32L152RET6_datasheet.pdf.

-Nucleo board user manual: By default the USART2 communication between the target
STM32 and ST-LINK MCU is enabled, in order to support virtual COM-
-STM32L152RET6 USART2 is connected to GPIOA PA2(TX),PA3(RX) and in Nucleo board D0,D1 (Arduino)

- RCC_APB1ENR bit 17 will enable USART2. p158
- USART registers. p736
- Baud rate register (USART_BRR) and Control register (USART_CR1). p739

redirect printf to swv console
http://youtu.be/r7Q_YVcnCN8?hd=1

Instruction to swv console Atollic_TrueSTUDIO_for_STMicroelectronics_STM32_QuickstartGuide.pdf
page 54

floats:
If floating point not needed very often then:
int i = 4532;
printf("Result is: %d.%d", i/100, i%100);

*/

/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32l1xx.h"
#include "nucleo152start.h"


/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
void USART2_Init(void);
int USART2_write(int data);
int USART2_read(void);
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

int i=0;

  /* Infinite loop */
  while (1)
  {
	  printf("Hello from the other side %d \n\r",i);
	  delay_Ms(1000);
	  i++;
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

int USART2_write(int data)
{
	//wait while TX buffer is empty
	while(!(USART2->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART2->DR=(data);		//p739
		return data;
}

int USART2_read(void)
{
	int data=0;
	//wait while RX buffer is data is ready to be read
	while(!(USART2->SR&0x0020)){} //Bit 5 RXNE: Read data register not empty
		data=USART2->DR;			//p739
		return data;
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); //measured with oscilloscope
}


