/*This code tests NUCLEO-L152RE board two Serial Wire Viewer (SWV) communication.

Serial Wire Viewer is a real-time trace technology that
uses the Serial Wire Debugger (SWD) port and the Serial
Wire Output (SWO) pin. Serial Wire Viewer provides advanced
system analysis and real-time tracing without the need to halt
the processor to extract the debug information.

redirect printf to swv console
http://youtu.be/r7Q_YVcnCN8?hd=1

Instruction to swv console 
Atollic_TrueSTUDIO_for_STMicroelectronics_STM32_QuickstartGuide.pdf
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

  /* TODO - Add your application code here */

int i=0;

  /* Infinite loop */
  while (1)
  {
	printf("Hello from the other side %d \n",i);
	delay_Ms(1000);
	i++;
  }
  return 0;
}

void delay_Ms(int delay)
{
	int i=0;
	for(; delay>0;delay--)
		for(i=0;i<2460;i++); //measured with oscilloscope
}