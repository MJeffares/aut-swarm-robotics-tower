///*
 //* LCD.c
 //*
 //* Created: 30/09/2017 5:34:34 PM
 //*  Author: ptx6852
 //*/ 
//#include <avr/io.h>
//#include <util/delay.h>
//#define dataPort PORTB
//#define dataOutput DDRB
//#define cntrlPort PORTC
//#define cntrlOutput DDRC
//#define enableLCD 5
//#define ReadWrite 4
//#define RS 3
//
//void LCD_Int (void)
//{
	//LCD_Send_A_Command(0x30);
	//_delay_ms(10);
	//LCD_Send_A_Command(0x30);
	//_delay_us(120);
	//LCD_Send_A_Command(0X30);
	//
	//LCD_Send_A_Command(0x38);
	//LCD_Send_A_Command(0x0C);
	//LCD_Send_A_Command(0x01);
	//LCD_Send_A_Command(0x06);
	//_delay_ms(2);
//}
//
//void LCD_checkBusy()
//{
	//dataOutput = 0;
	//cntrlPort |= 1<<ReadWrite;
	//cntrlPort &= ~1<<RS;
//
	//while (dataPort >= 0x80)
	//{
		//LCD_Peek_A_Boo();
	//}
	//dataOutput = 0xFF; //0xFF means 0b11111111
//}
//
//void LCD_Peek_A_Boo()
//{
	//cntrlPort |= 1<<enableLCD;
	//_delay_us(100);
	//cntrlPort &= ~1<<enableLCD;
//}
//
//void LCD_Send_A_Command(unsigned char command)
//{
	//LCD_checkBusy();
	//dataPort = command;
	//cntrlPort &= ~ ((1<<ReadWrite)|(1<<RS));
	//LCD_Peek_A_Boo();
	//dataPort = 0;
//}
//
//void LCD_Send_A_Character(unsigned char character)
//{
	//LCD_checkBusy();
	//dataPort = character;
	//cntrlPort &= ~ (1<<ReadWrite);
	//cntrlPort |= 1<<RS;
	//LCD_Peek_A_Boo();
	//dataPort = 0;
//}
//
//void LCD_Write_String(const char *a)
//{
	//int i;
	//for(i=0;a[i]!='\0';i++)
	//LCD_Send_A_Character(a[i]);
//}
//
//void LCD_Set_Cursor(char a, char b)
//{
	//if(a == 1)
	//LCD_Send_A_Command(0x80 + b);	// Set Cursor to LINE 1 and position to value b.
	//else if(a == 2)
	//LCD_Send_A_Command(0xC0 + b);  // Set Cursor to LINE 2 and position to value b.
	//else if(a == 3)
	//LCD_Send_A_Command(0x94 + b);  // Set Cursor to LINE 3 and position to value b.
	//else if(a == 4)
	//LCD_Send_A_Command(0xD4 + b);  // Set Cursor to LINE 4 and position to value b.
//}
//
//void LCD_Shift_Right()
//{
	//LCD_Send_A_Command(0x1C);
//}
//
//void LCD_Shift_Left()
//{
	//LCD_Send_A_Command(0x18);
//}
//void LCD_Clear()
//{
	//LCD_Send_A_Command(0x01);
//}
//
//void LCDWriteStringROM(unsigned char x, unsigned char y, const char *textVal, unsigned char clr)
//{
	//unsigned char lineValue;
	//unsigned char lcdoffset;
	//unsigned char i;
	//
	//switch (y)
	//{
		//case 0: lineValue = 1; break;
		//case 1: lineValue = 2; break;
		//case 2: lineValue = 3; break;
		//case 3: lineValue = 4; break;
		//default: lineValue = 1; break;
	//}
	//
	//LCD_Set_Cursor(lineValue,x);
	//LCD_Write_String(textVal);
	//lcdoffset = strlen(textVal);
	//
	//if (clr)
	//{
		//for(i; i <= (21 - lcdoffset); i ++)
		//{
			//LCD_Send_A_Character(' ');
		//}
	//}
	//
//}

