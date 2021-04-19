/*
This code tests NUCLEO-L152RE board internal temp senson with UART.

The temperature sensor can be used to measure the junction temperature
(TJ) of the device. The temperature sensor is internally connected to
the ADC_IN16 input channel which is used to convert the sensor’s output
voltage to a digital value. When not in use, the sensor can be put in
power down mode. p286

page 287 tells how to read temperature sensor

This code prints raw data from temperature sensor multiplied by 100.
*/

/* Includes */
#include <stddef.h>
#include "stm32l1xx.h"
#include "nucleo152start.h"
#include <stdio.h>

/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
void USART2_Init(void);
void USART2_write(char data);
char USART2_read(void);
void delay_Ms(int delay);

#define TS_CAL1 *((uint16_t*)0x1FF800FA) // TS_CAL1
#define TS_CAL2 *((uint16_t*)0x1FF800FE) // TS_CAL2

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
  int result=0;

  //set up pin PA5 for LED
  RCC->AHBENR |= 1;				//enable GPIOA clock
  GPIOA->MODER&=~0x00000C00;	//clear pin mode
  GPIOA->MODER|=0x00000400;		//set pin PA5 to output model

  //setup ADC1 and temp sensor. p287
  RCC->APB2ENR|=0x00000200;		//enable ADC1 clock
  ADC1->SQR5=16;				//ch16 internal sensor
  ADC1->SMPR2 |= (1<<18)|(1<<19)|(1<<20); //sampling time longest 384 cycles for channel 16
  ADC->CCR |= (1<<23);			//enable temp sensor
  ADC1->CR2|=1;					//bit 0, ADC on/off (1=on, 0=off)

  char buf[100];
  /* Infinite loop */
  while (1)
  {
	  ADC1->CR2|=0x40000000;	//start conversion
	  while(!(ADC1->SR & 2)){}	//wait for conversion complete
	  result=(ADC1->DR);		//read conversion result

	  float result2=(110.0-30.0)/((float)TS_CAL2-(float)TS_CAL1)*((float)result-(float)TS_CAL1)+50.0;
	  result2=result2*100;
	  int result3=(int)result2;
	  sprintf(buf,"%d",result3);

	  int len=0;
	  while(buf[len]!='\0')
	  len++;

		for(int i=0;i<len;i++)
		{
			USART2_write(buf[i]);
		}

		USART2_write('\n');
		USART2_write('\r');
		delay_Ms(1000);
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

	USART2->BRR = 0x00000116;	//11500 BAUD and crystal 32MHz. p710, 116
	USART2->CR1 = 0x00000008;	//TE bit. p739-740. Enable transmit
	USART2->CR1 |= 0x00000004;	//RE bit. p739-740. Enable receiver
	USART2->CR1 |= 0x00002000;	//UE bit. p739-740. Uart enable
}

void USART2_write(char data)
{
	//wait while TX buffer is empty
	while(!(USART2->SR&0x0080)){} 	//TXE: Transmit data register empty. p736-737
		USART2->DR=(data);		//p739
}

char USART2_read()
{
	char data=0;
	//wait while RX buffer data is ready to be read
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