/*
This code tests NUCLEO-L152RE board ADC inputs PA0,PA1 (A0,A1) with UART.
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
void read_and_print_A0(void);
void read_and_print_A1(void);
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

  //set up pin PA5 for LED
  RCC->AHBENR |= 1;				//enable GPIOA clock
  GPIOA->MODER&=~0x00000C00;	//clear pin mode
  GPIOA->MODER|=0x00000400;		//set pin PA5 to output model

  //set up pin PA0 and PA1 for analog input
  RCC->AHBENR|=1;				//enable GPIOA clock
  GPIOA->MODER|=0x3;			//PA0 analog (A0)
  GPIOA->MODER|=0xC;			//PA1 analog (A1)

  //setup ADC1. p272
  RCC->APB2ENR|=0x00000200;		//enable ADC1 clock
  ADC1->CR2=0;					//bit 1=0: Single conversion mode
  ADC1->SMPR3=7;				//384 cycles sampling time for channel 0 (longest)
  ADC1->CR1&=~0x03000000;		//resolution 12-bit

  /* Infinite loop */
  while (1)
  {
	  read_and_print_A0();
	  delay_Ms(1000);
	  read_and_print_A1();
	  delay_Ms(1000);
  }
  return 0;
}

void read_and_print_A0(void)
{
	char buf[100];
	int result=0;
	ADC1->SQR5=0;				//conversion sequence starts at ch0
	ADC1->CR2|=1;				//bit 0, ADC on/off (1=on, 0=off)
	ADC1->CR2|=0x40000000;		//start conversion
	while(!(ADC1->SR & 2)){}	//wait for conversion complete
	result=ADC1->DR;			//read conversion result
	sprintf(buf,"A0=%d",result);

	int len=0;
	while(buf[len]!='\0')
	len++;

	for(int i=0;i<len;i++)
	{
		USART2_write(buf[i]);
	}

	USART2_write('\n');
	USART2_write('\r');
	ADC1->CR2&=~1;				//bit 0, ADC on/off (1=on, 0=off)
}

void read_and_print_A1(void)
{
	char buf[100];
	int result=0;
	ADC1->SQR5=1;				//conversion sequence to ch1
	ADC1->CR2|=1;				//bit 0, ADC on/off (1=on, 0=off)
	ADC1->CR2|=0x40000000;		//start conversion
	while(!(ADC1->SR & 2)){}	//wait for conversion complete
	result=ADC1->DR;			//read conversion result
	sprintf(buf,"A1=%d",result);

	int len=0;
	while(buf[len]!='\0')
	len++;

	for(int i=0;i<len;i++)
	{
		USART2_write(buf[i]);
	}

	USART2_write('\n');
	USART2_write('\r');
	ADC1->CR2&=~1;				//bit 0, ADC on/off (1=on, 0=off)
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