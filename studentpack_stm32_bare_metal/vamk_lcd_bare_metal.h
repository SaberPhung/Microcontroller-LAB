
#include <stdio.h>
#define LCD_ADDR 0x27
#define LCD_LINE_1 0x00
#define LCD_LINE_2 0x40
/* LCD prototypes  */
void DisplayOnLCD(int value, uint8_t data_line1[], uint8_t data_line2[]);
void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendChar(uint8_t c);
void LCD_SendString(uint8_t *str);
void LCD_GoToXY(uint8_t x, uint8_t y);
void LCD_ClearAll(void);
void LCD_ClearFromPos(uint8_t x, uint8_t y);
extern void I2C1_Write_LCD(uint8_t address, int n, uint8_t* data);

void LCD_Init(void)
{
	/*
	Datasheet for commands/instructions
	https://panda-bg.com/datasheet/2134-091834-LCD-module-TC1602D-02WA0-16x2-STN.pdf
	https://www.electronicwings.com/sensors-modules/lcd-16x2-display-module
	https://mil.ufl.edu/3744/docs/lcdmanual/commands.html
	https://www.robofun.ro/docs/rc1602b-biw-esx.pdf
	*/
	LCD_SendCommand(0x02);	// set the LCD in 4-bit mode (D4-D7)
	LCD_SendCommand(0x28);	// 2 lines, 5x8 matrix, 4-bit mode
	LCD_SendCommand(0x0C);	// Display ON, cursor off
	LCD_SendCommand(0x80);	// Force the cursor to position (0,0)
}

void LCD_SendChar(uint8_t c)
{
	uint8_t nibble_r, nibble_l;
	uint8_t data[4];
	nibble_l = c & 0xf0;
	nibble_r = (c << 4) & 0xf0;
	data[0] = nibble_l | 0x0D;
	data[1] = nibble_l | 0x09;
	data[2] = nibble_r | 0x0D;
	data[3] = nibble_r | 0x09;
	I2C1_Write_LCD(LCD_ADDR, 4,(uint8_t *)data);
}
void LCD_SendCommand(uint8_t cmd)
{
	uint8_t nibble_r, nibble_l;
	uint8_t data[4];
	nibble_l = cmd & 0xf0;
	nibble_r = (cmd << 4) & 0xf0;
	data[0] = nibble_l | 0x0C;
	data[1] = nibble_l | 0x08;
	data[2] = nibble_r | 0x0C;
	data[3] = nibble_r | 0x08;
	I2C1_Write_LCD(LCD_ADDR, 4,(uint8_t *)data);
}

void LCD_SendString(uint8_t *str)
{
	while (*str)
		LCD_SendChar(*str++);
}

void DisplayOnLCD(int value, uint8_t data_line1[], uint8_t data_line2[])
{
	sprintf((char *)data_line2, "%d", value);
	LCD_ClearFromPos(0,0);
	LCD_GoToXY(0, LCD_LINE_1);
	LCD_SendString(data_line1);
	LCD_GoToXY(0, LCD_LINE_2);
	LCD_SendString(data_line2);
}

void LCD_GoToXY(uint8_t x, uint8_t y)
{
	uint8_t LCD_DDRAM_ADDR = 0x80;

	if (y == 0)
		LCD_SendCommand(LCD_DDRAM_ADDR | (LCD_LINE_1 + x));
	else
		LCD_SendCommand(LCD_DDRAM_ADDR | (LCD_LINE_2 + x));
}

void LCD_ClearAll(void)
{
	LCD_SendCommand(0x01); // 0x01 is the command to clear the LCD display
}
void LCD_ClearFromPos(uint8_t x, uint8_t y)
{
	uint8_t str[32] = "";
	LCD_GoToXY(x, y);
	LCD_SendString(str);
}
