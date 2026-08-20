#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>
#include <stdio.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"

#include "LCD_Private.h"
#include "LCD_Interface.h"
#include "LCD_cnf.h"

static void Enable_Pulse(void)
{
	DIO_voidSetPinValue(E_PORT, E_PIN, PIN_HIGH);
	_delay_us(1);
	DIO_voidSetPinValue(E_PORT, E_PIN, PIN_LOW);
	_delay_ms(2);
}

static void LCD_Send4Bits(u8 data)
{
	DIO_voidSetPinValue(D4_PORT, D4_PIN, GET_BIT(data, 0));
	DIO_voidSetPinValue(D5_PORT, D5_PIN, GET_BIT(data, 1));
	DIO_voidSetPinValue(D6_PORT, D6_PIN, GET_BIT(data, 2));
	DIO_voidSetPinValue(D7_PORT, D7_PIN, GET_BIT(data, 3));

	Enable_Pulse();
}

void Lcd_SendCmd(u8 Command)
{
	DIO_voidSetPinValue(RS_PORT, RS_PIN, PIN_LOW);

	/* High Nibble */
	LCD_Send4Bits(Command >> 4);

	/* Low Nibble */
	LCD_Send4Bits(Command & 0x0F);
}

void LCD_voidSenddata(u8 data)
{
	DIO_voidSetPinValue(RS_PORT, RS_PIN, PIN_HIGH);

	/* High Nibble */
	LCD_Send4Bits(data >> 4);

	/* Low Nibble */
	LCD_Send4Bits(data & 0x0F);
}

void LCD_voidInit(void)
{
	/* Set Control and Data Pins as Output */
	DIO_voidSetPinDirection(RS_PORT, RS_PIN, PIN_OUTPUT);
	DIO_voidSetPinDirection(E_PORT, E_PIN, PIN_OUTPUT);
	DIO_voidSetPinDirection(D4_PORT, D4_PIN, PIN_OUTPUT);
	DIO_voidSetPinDirection(D5_PORT, D5_PIN, PIN_OUTPUT);
	DIO_voidSetPinDirection(D6_PORT, D6_PIN, PIN_OUTPUT);
	DIO_voidSetPinDirection(D7_PORT, D7_PIN, PIN_OUTPUT);

	DIO_voidSetPinValue(RS_PORT, RS_PIN, PIN_LOW);
	DIO_voidSetPinValue(E_PORT, E_PIN, PIN_LOW);

	_delay_ms(50);

	/* Initialization Sequence for 4-Bit Mode */
	LCD_Send4Bits(0x03);
	_delay_ms(5);

	LCD_Send4Bits(0x03);
	_delay_us(150);

	LCD_Send4Bits(0x03);
	_delay_us(150);

	LCD_Send4Bits(0x02);

	/* Configuration Commands */
	Lcd_SendCmd(_LCD_4BIT_MODE_2_LINE);
	Lcd_SendCmd(_LCD_DISPLAY_ON_UNDERLINE_OFF_CURSOR_OFF);
	HLCD_voidClearLCD();
	Lcd_SendCmd(_LCD_ENTRY_MODE_INC_SHIFT_OFF);
}

void Lcd_DisplayChr(u8 chr)
{
	LCD_voidSenddata(chr);
}

void Lcd_DisplayStr(u8* str)
{
	while(*str != '\0')
	{
		Lcd_DisplayChr(*str);
		str++;
	}
}

void Lcd_Goto_Row_Column(u8 row, u8 col)
{
	switch(row)
	{
		case 0: Lcd_SendCmd(0x80 + col); break;
		case 1: Lcd_SendCmd(0xC0 + col); break;
		case 2: Lcd_SendCmd(0x94 + col); break;
		case 3: Lcd_SendCmd(0xD4 + col); break;
		default: break;
	}
}

void HLCD_voidClearLCD(void)
{
	Lcd_SendCmd(_LCD_CLEAR);
	_delay_ms(2);
}

void HLCD_voidReturnHome(void)
{
	Lcd_SendCmd(_LCD_RETURN_HOME);
	_delay_ms(2);
}

void Lcd_Displaynum(u16 number)
{
	u8 str[7];
	sprintf((char*)str, "%d", number);
	Lcd_DisplayStr(str);
}