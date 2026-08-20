#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

#include "STD_TYPES.h"

void LCD_voidInit(void);
void Lcd_SendCmd(u8 Command);
void LCD_voidSenddata(u8 data);
void Lcd_DisplayStr(u8* str);
void Lcd_Goto_Row_Column(u8 row, u8 col);
void Lcd_DisplayChr(u8 chr);
void Lcd_Displaynum(u16 number);
void HLCD_voidClearLCD(void);
void HLCD_voidReturnHome(void);

#endif /* LCD_INTERFACE_H_ */