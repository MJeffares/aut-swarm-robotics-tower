/*
 * LCD.h
 *
 * Created: 30/09/2017 5:07:44 PM
 *  Author: ptx6852
 */ 


#ifndef LCD_H_
#define LCD_H_

void LCD_Int (void);
void LCD_checkBusy(void);
void LCD_Peek_A_Boo(void);
void LCD_Send_A_Command(unsigned char command);
void LCD_Send_A_Character(unsigned char character);
void LCD_Send_A_String(char *string);
void LCD_Set_Cursor(char a, char b);
void LCD__Shift_Left();
void LCD__Shift_Right();
void LCD_Clear();
void LCDWriteStringROM(unsigned char x, unsigned char y, const char *textVal, unsigned char clr);

#endif /* LCD_H_ */