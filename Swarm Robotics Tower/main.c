#define F_CPU 8000000UL

// Defining number of buttons for ButtonPress.h
#define numberOfButtons 7

// Include statements for all the header files and the atmel libraries
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include "ButtonPress.h"
#include "MenuSystem.h"
#include "LCD.h"
#include "Communications/uart_interface.h"
#include "Communications/xbee_driver.h"
#include "Variables.h"

// Ports being used
#define dataPort PORTA
#define dataOutput DDRA
#define cntrlPort PORTD
#define cntrlOutput DDRD
#define enableLCD 7
#define ReadWrite 4
#define RS 6

// Defining the data pins for the LCD
#define d0 PORTB |= (1<<0)
#define d1 PORTB |= (1<<1)
#define d2 PORTB |= (1<<2)
#define d3 PORTB |= (1<<3)
#define d4 PORTB |= (1<<4)
#define d5 PORTB |= (1<<5)
#define d6 PORTB |= (1<<6)
#define d7 PORTB |= (1<<7)

#define Es PORTC |= (1<<5)
#define RWs PORTC |= (1<<4)
#define RSa PORTC |= (1<<3)

#define Out1 PORTA |= (1<<0)
#define Out2 PORTA |= (1<<1)
#define Out3 PORTA |= (1<<2)
#define Out4 PORTA |= (1<<3)
#define Out5 PORTA |= (1<<4)
#define Out6 PORTA |= (1<<5)

#define Out1OFF PORTA &=~ (1<<0)
#define Out2OFF PORTA &=~ (1<<1)
#define Out3OFF PORTA &=~ (1<<2)
#define Out4OFF PORTA &=~ (1<<3)
#define Out5OFF PORTA &=~ (1<<4)
#define Out6OFF PORTA &=~ (1<<5)


// Structure to keep track of all the menu entries and the cursor values. 
typedef struct MenuSystem
{
	char *text;							// Points to a const char array which holds the Text for the Menu.
	unsigned char num_menuPoints;		// Keeps track of the number of menu items in the current menu being displayed.
	unsigned char up;					// Keeps track of cursor if moved up 
	unsigned char down;					// Keeps track of cursor if moved down 
	unsigned char enter;				// Keeps track of cursor if enter is pressed
	unsigned char functionNumber;		// Keeps track of cursor if a function is to be called
	unsigned char faceNumber;			// Keeps track of which face on the hexagon the sub item is referring to. 
} MenuEntry;



// Variables for the Menu System
unsigned char s = 1;

// Main Menu 
const char menu_000[] = "Main Menu";
const char menu_001[] = "1) Communications   ";
const char menu_002[] = "2) Robot Status     ";
const char menu_003[] = "3) Guide Light      ";
const char menu_004[] = "4) Sensor Status    ";
const char menu_005[] = "5) Charge Pad       ";

// Communications Menu
const char menu_100[] = "Communications Menu ";
const char menu_101[] = "1) Status           ";
const char menu_102[] = "2) Send Commands    ";
const char menu_103[] = "3) View RX Data     ";
const char menu_104[] = "Return              ";

// Robot Status Menu
const char menu_200[] = "Robot Status Menu   ";
const char menu_201[] = "1) Robot 1          ";
const char menu_202[] = "1) Robot 2          ";
const char menu_203[] = "1) Robot 3          ";
const char menu_204[] = "1) Robot 4          ";
const char menu_205[] = "1) Robot 5          ";
const char menu_206[] = "1) Robot 6          ";
const char menu_207[] = "Return              ";

// Guide Light Menu
const char menu_300[] = "Guide Light Menu    ";
const char menu_301[] = "1) LED 1            ";
const char menu_302[] = "2) LED 2            ";
const char menu_303[] = "3) LED 3            ";
const char menu_304[] = "4) LED 4            ";
const char menu_305[] = "5) LED 5            ";
const char menu_306[] = "6) LED 6            ";
const char menu_307[] = "Return              ";

// Sensor Status Menu
const char menu_400[] = "Sensor Status Menu  ";
const char menu_401[] = "1) Sensor 1         ";
const char menu_402[] = "2) Sensor 2         ";
const char menu_403[] = "3) Sensor 3         ";
const char menu_404[] = "4) Sensor 4         ";
const char menu_405[] = "5) Sensor 5         ";
const char menu_406[] = "6) Sensor 6         ";
const char menu_407[] = "Return              ";


// Menu Structure to hold the cursor values and the menu text.
// This corresponds with the menu item that will appear on the screen


/***************************************************************************  MENU SYSTEM OPERATION  ***********************************************************

1) The first item in the structure is the menu text. Therefore a const char pointer is passed to the structure.
2) The second item in the structure is the number of contents in the menu. For example if menu is as follows :

	Main Menu
	1) Communications   
	2) Robot Status     
	3) Guide Light      
	4) Sensor Status    
	5) Charge Pad       
	
	There are 6 items including the title of the menu.
	
3) The third item in the structure is the position as to where the cursor will be if the up button is pressed
4) The forth item in the structure is the position as to where the cursor will be if the down button is pressed
5) The fifth item in the structure is the position as to where the cursor and the menu will jump to if the enter button is pressed 
6) The sixth item in the structure is related to if a function needs to be called for that sub menu
7) The seventh item in the structure is referring to which side of the hexagon the menu is concerned with

***************************************************************************************************************************************************************/

// Array of Menu structures 
MenuEntry menu [] =
{
	{menu_000,6, 0, 0, 0, 0, 0},	// 0
	{menu_001,6, 1, 2, 7, 0, 0},	// 1
	{menu_002,6, 1, 3, 12, 0, 0},	// 2
	{menu_003,6, 2, 4, 20, 0, 0},	// 3
	{menu_004,6, 3, 5, 28, 0, 0},	// 4
	{menu_005,6, 4, 5, 5, 0, 0},	// 5
	
	{menu_100,5, 0, 0, 0, 0, 0},	// 6
	{menu_101,5, 7, 8, 7, 0, 0},	// 7
	{menu_102,5, 7, 9, 8, 0, 0},	// 8
	{menu_103,5, 8, 10, 9, 0, 0},	// 9
	{menu_104,5, 9, 10, 1, 0, 0},	// 10
	
	{menu_200,8, 0, 0, 0, 0, 0},	// 11
	{menu_201,8, 12, 13, 12, 3, 1},	// 12
	{menu_202,8, 12, 14, 13, 3, 2},	// 13
	{menu_203,8, 13, 15, 14, 3, 3},	// 14
	{menu_204,8, 14, 16, 15, 3, 4},	// 15	
	{menu_205,8, 15, 17, 16, 3, 5},	// 16
	{menu_206,8, 16, 18, 17, 3, 6},	// 17
	{menu_207,8, 17, 18, 1, 0, 0},  // 18		
		
	{menu_300,8, 0, 0, 0, 0, 0},	// 19
	{menu_301,8, 20, 21, 20, 1, 1},	// 20
	{menu_302,8, 20, 22, 21, 1, 2},	// 21
	{menu_303,8, 21, 23, 22, 1, 3},	// 22
	{menu_304,8, 22, 24, 23, 1, 4},	// 23
	{menu_305,8, 23, 25, 24, 1, 5},	// 24
	{menu_306,8, 24, 26, 25, 1, 6},	// 25
	{menu_307,8, 25, 26, 1, 0, 0},	// 26
		
	{menu_400,8, 0, 0, 0, 0, 0},	 // 27
	{menu_401,8, 28, 29, 28, 2, 1},	 // 28
	{menu_402,8, 28, 30, 29, 2, 2},	 // 29
	{menu_403,8, 29, 31, 30, 2, 3},	 // 30
	{menu_404,8, 30, 32, 31, 2, 4},	 // 31
	{menu_405,8, 31, 33, 32, 2, 5},	 // 32
	{menu_406,8, 32, 34, 33, 2, 6},	 // 33
	{menu_407,8, 33, 34, 1, 0, 0},	 // 34
};


// Robot Mac Address for all the robots
uint64_t ROBOTS[8] = {0x0013A2004147F9DD, 0x0013A200415B8C38, 0x0013A200415B8BDD, 0x0013A2004152F256, 0x0013A200415B8C3A, 0x0013A20041065FB3, 0x0013A200415B8C18, 0x0013A200415B8BE5};
uint8_t LEDValues = 0; 


// The variables for the Light sensors that are connected to interrupts. They measure a digital high and low depending on the amount of light in the room
// These variables keep count of how many times the signal is high. Based on if it is greater or lower that a certain sensitivity amount then the LED is turned on.

// Light Sensor variables 
volatile uint8_t LS1;
volatile uint8_t LS2;
volatile uint8_t LS3;
volatile uint8_t LS4;
volatile uint8_t LS5;
volatile uint8_t LS6;

// Light Sensor variable values
volatile uint8_t LS1V;
volatile uint8_t LS2V;
volatile uint8_t LS3V;
volatile uint8_t LS4V;
volatile uint8_t LS5V;
volatile uint8_t LS6V;

// Light Sensor Sensitivity value for each sensor
volatile uint8_t LS1_Sensitivity = 10;
volatile uint8_t LS2_Sensitivity = 10;
volatile uint8_t LS3_Sensitivity = 10;
volatile uint8_t LS4_Sensitivity = 10;
volatile uint8_t LS5_Sensitivity = 10;
volatile uint8_t LS6_Sensitivity = 10;

// Count variables for timer
volatile unsigned char count =0;
volatile unsigned char countms10 =0;
volatile unsigned char countus250 =0;
volatile char fiftymsdelay = 0;

// Flags to interact with the LED
char LS1flag = 0;
char LS2flag = 0;
char LS3flag = 0;
char LS4flag = 0;
char LS5flag = 0;
char LS6flag = 0;
char out1Flag = 0;
char out1Flag2 = 0;


// Menu Flags used to switch between the main and sub menus.
char showMenuflag = 1;
char LEDStatusFlag = 1;
char sensorCalibrationFlag = 0;


// Robot Status
uint8_t Task;
uint16_t Voltage;
uint16_t Xposition;
uint16_t Yposition;
int16_t Facing;

// Flag for button pressed
char buttonPressedFlag = 0;


// The LCD function prototypes
void LCD_checkBusy(void);
void LCD_Peek_A_Boo(void);
void LCD_Send_A_Command(unsigned char command);
void LCD_Send_A_Character(unsigned char character);
void LCD_Send_A_String(char *string);
void setup (void);
void LCD_Int (void);
void LCD_Set_Cursor(char a, char b);
void LCD_Write_String(const char *a);
void LCD__Shift_Left();
void LCD__Shift_Right();
void showMenu (void);
void LCDWriteStringROM(unsigned char x, unsigned char y, const char *textVal, unsigned char clr);
void LCD_Clear();
void LEDStatus (int i, char flag);

// Sensor and robot status function prototypes
void sensorStatus (int i, int LSValue,char flag);
void robotStatus (int i, char flag);

// Char array for the LCD to write characters
char line1 [20];
char line3 [20];
char line2 [20];
char cursorvalue = 1;

// Test function prototype
void test(void);



int main(void)
{
	// Function call to setup
	setup();
	
	// Initially turn all the LEDS on
	LED1_ON;
	LED2_ON;
	LED3_ON;
	LED4_ON;
	LED5_ON;
	LED6_ON;
	
	// Make function flag to 0 
	char functionFlag = 0;
	char val = 0;
	while(1)
	{	
		// Function call to handle incoming communications
		xbeeGetNew();
		
		// IF the robot has docked on edge 1 of the hexagon
		if(LS1flag == 1)
		{
			Out1;							// Output 1 is turned on
			if (out1Flag2 == 1)				// Turn off LED 
			{			
				LED4_OFF;
			}
			if (out1Flag2 == 0)
			{
				LED4_OFF;
			}
			out1Flag = 1;
			LEDStatusFlag = 0;
		}
		
		// IF the robot has un-docked on the hexagon then turn output 1 off and turn LED on.
		if(LS1flag == 0)
		{
			Out1OFF;
			LED4_ON;
			LEDStatusFlag = 1;
		}
		if(LS2flag == 1)
		{
			Out2;
			LEDStatusFlag = 1;
		}
		if(LS2flag == 0)
		{
			Out2OFF;
			LEDStatusFlag = 0;
		}
		if(LS3flag == 1)
		{
			Out3;
			LEDStatusFlag = 1;
		}
		
		if(LS3flag == 0)
		{
			Out3OFF;
			LEDStatusFlag = 0;
		}
		if(LS4flag == 1)
		{
			Out4;
			LEDStatusFlag = 1;
		}
				
		if(LS4flag == 0)
		{
			Out4OFF;
			LEDStatusFlag = 0;
		}
		if(LS5flag == 1)
		{
			Out5;
			LEDStatusFlag = 1;
		}
				
		if(LS5flag == 0)
		{
			Out5OFF;
			LEDStatusFlag = 0;
		}
		if(LS6flag == 1)
		{
			Out6;
			LEDStatusFlag = 1;
		}
		
		if(LS6flag == 0)
		{
			Out6OFF;
			LEDStatusFlag = 0;
		}
		
		
		if((LEDValues>>0) & 0x01)
		{
			LED1_ON;
		}
		if((LEDValues>>1) & 0x01)
		{
			LED2_ON;
		}
		if((LEDValues>>2) & 0x01)
		{
			LED3_ON;
		}
		if((LEDValues>>3) & 0x01)
		{
			LED4_ON;
		}
		if((LEDValues>>4) & 0x01)
		{
			LED5_ON;
		}
		if((LEDValues>>5) & 0x01)
		{
			LED6_ON;
		}
		
		
		
		// Switch case statement for changing the state of the menu from main menu to other sub menus
		
		switch(showMenuflag)
		{
			case 0:
				LCD_Clear();					// Clear menu on LCD Screen
				if(functionFlag == 0)
				{
					showMenuflag = 2;
				}
				if(functionFlag == 1)
				{
					showMenuflag = 3;
				}
				if(functionFlag == 2)
				{
					showMenuflag = 4;
				}
				break;			
			case 1:
				showMenu();	
				break;
			
			case 2:	
				LEDStatus((int)menu[s].faceNumber, LEDStatusFlag);
				break;
			
			case 3:
				if(menu[s].faceNumber == 1)
				{
					val = LS1V;
				}
				if(menu[s].faceNumber == 2)
				{
					val = LS2V;
				}
				if(menu[s].faceNumber == 3)
				{
					val = LS3V;
				}
				if(menu[s].faceNumber == 4)
				{
					val = LS4V;
				}
				if(menu[s].faceNumber == 5)
				{
					val = LS5V;
				}
				if(menu[s].faceNumber == 6)
				{
					val = LS6V;
				}
				sensorStatus ((int)menu[s].faceNumber, (int)val, sensorCalibrationFlag);
			break;
			
			case 4:
				robotStatus ((int)menu[s].faceNumber, LEDStatusFlag);
			break;
		}
			
		if(showMenuflag == 0)
		{
			LCD_Clear();	
		}
		
		if(ButtonPressed(0, PINE, 2, 10))
		{
			s = menu[s].up;
			cursorvalue++;
		}
		
		if(ButtonPressed(2, PINC, 2, 10))
		{
			s = menu[s].down;
		}
		
		if(ButtonPressed(1, PINE, 3, 10))
		{
			s = menu[s].enter;
			cursorvalue = 1;
		}
		
		if(menu[s].functionNumber == 1 && (ButtonPressed(3, PINE, 3, 10)))
		{
			showMenuflag = 0;
			functionFlag = 0;
		}
		
		if(menu[s].functionNumber == 2 && (ButtonPressed(3, PINE, 3, 10)))
		{
			showMenuflag = 0;
			functionFlag = 1;
		}
		
		if(menu[s].functionNumber == 3 && (ButtonPressed(3, PINE, 3, 10)))
		{
			showMenuflag = 0;
			functionFlag = 2;
		}

	}
}


void LEDStatus (int i, char flag)
{
	LCD_Set_Cursor(1,0);
	sprintf(line1, "Guide %d Status:", i);
	LCD_Write_String(line1);
	
	if(ButtonPressed(4, PINE, 3, 10))
	{
		if(i == 1)
		{
			LED1Toggle;
		}
		
		if(i == 2)
		{
			LED2Toggle;
		}

		if(i == 3)
		{
			LED3Toggle;
		}
		
		if(i == 4)
		{
			LED4Toggle;
		}
		
		if(i == 5)
		{
			LED5Toggle;
		}
		
		if(i == 6)
		{
			LED6Toggle;
		}
		LEDStatusFlag++;
		
	}
	if(LEDStatusFlag == 0 && ButtonPressed(4, PINE, 3, 10))
	{
		if(out1Flag == 1)
		{
			LED4_ON;
			out1Flag2 = 1;
		}
	}
	
	if(LEDStatusFlag % 2 == 1)
	{
		LCD_Set_Cursor(1,16);
		LCD_Write_String("ON ");
		
		LCD_Set_Cursor(2,0);
		LCD_Write_String("Guide OFF ---> Enter");
	}
	
	if(LEDStatusFlag % 2 == 0)
	{
		LCD_Set_Cursor(1,16);
		LCD_Write_String("OFF");
		
		LCD_Set_Cursor(2,0);
		LCD_Write_String("Guide ON ---> Enter");
	}
	
	LCD_Set_Cursor(4,16);
	LCD_Write_String("Back");
	
	if(ButtonPressed(2, PINC, 2, 10))
	{
		s = 20;
		showMenuflag = 1;
	}
	
}

void sensorStatus (int i, int LSValue,char flag)
{
	char exit = 0;
	char enter = 0;
	char facenumber = 0;
	if(flag == 0)
	{
		LCD_Set_Cursor(1,0);
		sprintf(line1, "Sensor %d Status: %d  ", i, (int)LSValue);
		LCD_Write_String(line1);
		if(i == 1)
		{
			LCD_Set_Cursor(2,0);
			sprintf(line2, "Sensitivity:%d", (int)LS1_Sensitivity);
			LCD_Write_String(line2);
		}
				if(i == 2)
		{
			LCD_Set_Cursor(2,0);
			sprintf(line2, "Sensitivity:%d", (int)LS2_Sensitivity);
			LCD_Write_String(line2);
		}
				if(i == 3)
		{
			LCD_Set_Cursor(2,0);
			sprintf(line2, "Sensitivity:%d", (int)LS3_Sensitivity);
			LCD_Write_String(line2);
		}
				if(i == 4)
		{
			LCD_Set_Cursor(2,0);
			sprintf(line2, "Sensitivity:%d", (int)LS4_Sensitivity);
			LCD_Write_String(line2);
		}
				if(i == 5)
		{
			LCD_Set_Cursor(2,0);
			sprintf(line2, "Sensitivity:%d", (int)LS5_Sensitivity);
			LCD_Write_String(line2);
		}
				if(i == 6)
		{
			LCD_Set_Cursor(2,0);
			sprintf(line2, "Sensitivity:%d", (int)LS6_Sensitivity);
			LCD_Write_String(line2);
		}

		
		LCD_Set_Cursor(3,0);
		LCD_Write_String("1)Calibrate Sensor <");
		exit =1;
		enter = 1;
	}
	
	if(flag == 1)
	{
		LCD_Set_Cursor(1,0);
		sprintf(line1,"Sensor %d Calibration", i);
		LCD_Write_String(line1);
		
		LCD_Set_Cursor(2,0);
		LCD_Write_String("Min Sensitivity : 10");
		
		if(LS1_Sensitivity<10)
		{
			LS1_Sensitivity = 10;
		}
		
		if(i == 1)
		{

			LCD_Set_Cursor(3,0);
			sprintf(line1,"Max Sensitivity : %d ", (int)LS1_Sensitivity);
			LCD_Write_String(line1);
		}

		if(i == 2)
		{
			LCD_Set_Cursor(3,0);
			sprintf(line1,"Max Sensitivity : %d ", (int)LS2_Sensitivity);
			LCD_Write_String(line1);
		}
		
		if(i == 3)
		{
			LCD_Set_Cursor(3,0);
			sprintf(line1,"Max Sensitivity : %d ", (int)LS3_Sensitivity);
			LCD_Write_String(line1);
		}
		
		if(i == 4)
		{
			LCD_Set_Cursor(3,0);
			sprintf(line1,"Max Sensitivity : %d ", (int)LS4_Sensitivity);
			LCD_Write_String(line1);
		}
		
		if(i == 5)
		{
			LCD_Set_Cursor(3,0);
			sprintf(line1,"Max Sensitivity : %d ", (int)LS5_Sensitivity);
			LCD_Write_String(line1);
		}
		if(i == 6)
		{
			LCD_Set_Cursor(3,0);
			sprintf(line1,"Max Sensitivity : %d ", (int)LS6_Sensitivity);
			LCD_Write_String(line1);
		}

		
		if(i == 1 && ButtonPressed(5, PINE, 3, 10))
		{
			LS1_Sensitivity--;
		}

		if(i == 1 && ButtonPressed(0, PINE, 2, 10))
		{
			LS1_Sensitivity++;	
		}
		
		if(i == 2 && ButtonPressed(5, PINE, 3, 10))
		{
			LS2_Sensitivity--;
		}

		if(i == 2 && ButtonPressed(0, PINE, 2, 10))
		{
			LS2_Sensitivity++;	
		}
		
		if(i == 3 && ButtonPressed(5, PINE, 3, 10))
		{
			LS3_Sensitivity--;
		}

		if(i == 3 && ButtonPressed(0, PINE, 2, 10))
		{
			LS3_Sensitivity++;	
		}
		
		if(i == 4 && ButtonPressed(5, PINE, 3, 10))
		{
			LS4_Sensitivity--;
		}

		if(i == 4 && ButtonPressed(0, PINE, 2, 10))
		{
			LS4_Sensitivity++;	
		}
		
		if(i == 5 && ButtonPressed(5, PINE, 3, 10))
		{
			LS5_Sensitivity--;
		}

		if(i == 5 && ButtonPressed(0, PINE, 2, 10))
		{
			LS5_Sensitivity++;	
		}
		
		if(i == 6 && ButtonPressed(5, PINE, 3, 10))
		{
			LS6_Sensitivity--;
		}

		if(i == 6 && ButtonPressed(0, PINE, 2, 10))
		{
			LS6_Sensitivity++;	
		}		
		
		exit = 0;
		enter = 0;
	}
	
	if(ButtonPressed(2, PINC, 2, 10))
	{
		showMenuflag = 0;
		sensorCalibrationFlag =0;
	}
	
	if(enter == 1 && ButtonPressed(5, PINE, 3, 10))
	{
		showMenuflag = 0;
		sensorCalibrationFlag =1;
		
	}
	if(exit == 1 && ButtonPressed(2, PINC, 2, 10))
	{
		s = 28;
		showMenuflag = 1;
	}
}


void robotStatus (int i, char flag)
{
	uint8_t dataBuffer[30];
	
	if(i == 1)
	{
		dataBuffer[0] = 0xF3;
		dataBuffer[1] = 0x01;
		dataBuffer[2] = ROBOTS[0];
		dataBuffer = {0xF3, 0x01, ROBOTS[0]};
		xbeeSendAPITransmitRequest(COORDINATOR_64, UNKNOWN_16, dataBuffer , 20);
		LCD_Set_Cursor(1,0);
		LCD_Write_String("Chosen 1");
	}
	
	if(i == 2)
	{
		LCD_Set_Cursor(1,0);
		LCD_Write_String("Chosen 2");
	}
	
	if(i == 3)
	{
		LCD_Set_Cursor(1,0);
		LCD_Write_String("Chosen 3");
	}

	if(i == 4)
	{
		LCD_Set_Cursor(1,0);
		LCD_Write_String("Chosen 4");
	}
	
	if(i == 5)
	{
		LCD_Set_Cursor(1,0);
		LCD_Write_String("Chosen 5");
	}	

	if(i == 6)
	{
		LCD_Set_Cursor(1,0);
		LCD_Write_String("Chosen 6");
	}
	
	if(ButtonPressed(2, PINC, 2, 10))
	{
		s = 12;
		showMenuflag = 1;
	}
	
}


void setup (void)
{	
//	xbeeInit();
	//DDRA = 0b10111111;
	DDRE = 0b00011100;	// Push buttons and UART0 for USB
	
	EICRA = 0b00001111;
	EICRB = 0b00001111;
	EIMSK = 0b00110011;

	TCCR3A = 0b10000000;
	TCCR3B = 0b00001011;
	
	OCR3A = 250;
	ETIMSK = 0b00010000;
	
	DDRD = 0b11010000;
	LCD_Int();
	
	sei();

}


// Interrupt to count all the high inputs from each sensor
ISR(INT0_vect)
{
	LS1++;
}

ISR(INT1_vect)
{
	LS6++;
}

ISR(INT4_vect)
{
	LS3++;
}

ISR(INT5_vect)
{
	LS2++;
}

ISR(INT6_vect)
{
	LS5++;
}

ISR(INT7_vect)
{
	LS4++;
}


// The timer interrupt 
ISR(TIMER3_COMPA_vect)
{
	count ++;
	if(count > 20)
	{
		if(LS1 < LS1_Sensitivity)
		{	
			LS1flag = 1;
			LS1V = LS1;
		}
		else
		{
			LS1flag = 0;
		}
		
		if(LS2 < LS2_Sensitivity)
		{
			LS2flag = 1;
			LS2V = LS2;
		}
		else
		{
			LS2flag = 0;
		}
		
		if(LS3 < LS3_Sensitivity)
		{
			LS3flag = 1;
			LS3V = LS3;
		}
		else
		{
			LS3flag = 0;
		}

		if(LS4 < LS4_Sensitivity)
		{
			LS4flag = 1;
			LS4V = LS4;
		}
		else
		{
			LS4flag = 0;
		}
		if(LS5 < LS5_Sensitivity)
		{
			LS5flag = 1;
			LS5V = LS5;
		}
		else
		{
			LS5flag = 0;
		}
		if(LS6 < LS6_Sensitivity)
		{
			LS6flag = 1;
			LS6V = LS6;
		}
		else
		{
			LS6flag = 0;
		}
		LS1 = 0;
		LS2 = 0;
		LS3 = 0;
		LS4 = 0;
		LS5 = 0;
		LS6 = 0;
		count = 0;
	}
	
}



// LCD Initialize function, refer to the LCD data sheet for further understanding. 
void LCD_Int (void)
{
	LCD_Send_A_Command(0x30);
	_delay_ms(10);
	LCD_Send_A_Command(0x30);
	_delay_us(120);
	LCD_Send_A_Command(0X30);
	
	LCD_Send_A_Command(0x38);
	LCD_Send_A_Command(0x0C);
	LCD_Send_A_Command(0x01);
	LCD_Send_A_Command(0x06);
	_delay_ms(2);
}

// Show menu function
void showMenu (void)
{
	unsigned char linecount = 0;
	unsigned char from = 0;
	unsigned int till = 0;
	unsigned char temp = 0;
	
	while (till <= s)
	{
		till += menu[till].num_menuPoints;
	}
	from = till - menu[s].num_menuPoints;
	till--;
	
	temp = from;
	
	if ((s>= (from +2)) && (s <=(till -1)))
	{
		from = s - 2;
		till = from + 3;
		
		for (from; from <= till; from ++)
		{
			LCDWriteStringROM (0,linecount,menu[from].text, 1);
			linecount++;
			
		}
		
		LCD_Set_Cursor (3, 19);
		LCD_Send_A_Character('<');
		
		LCD_Set_Cursor (2, 19);
		LCD_Send_A_Character(' ');
		
		LCD_Set_Cursor (4, 19);
		LCD_Send_A_Character(' ');
		
	}
	
	else
	{
		if(s<(from + 2))
		{
			till = from +3;
			for (from; from <= till; from ++)
			{
				LCDWriteStringROM (0,linecount,menu[from].text, 1);
				linecount++;
				
			}
			LCD_Set_Cursor ((s+1) - temp, 19);
			LCD_Send_A_Character('<');
			
			LCD_Set_Cursor (3, 19);
			LCD_Send_A_Character(' ');
			
			LCD_Set_Cursor (4, 19);
			LCD_Send_A_Character(' ');
			
		}
		if(s == till)
		{
			from = till-3;
			for (from; from <= till; from ++)
			{
				LCDWriteStringROM (0,linecount,menu[from].text, 1);
				linecount++;
				
			}
			LCD_Set_Cursor (4, 19);
			LCD_Send_A_Character('<');
			
			LCD_Set_Cursor (3, 19);
			LCD_Send_A_Character(' ');
		}
	}
}


void LCDWriteStringROM(unsigned char x, unsigned char y, const char *textVal, unsigned char clr)
{
	unsigned char lineValue;
	unsigned char lcdoffset;
	unsigned char i;
	
	switch (y)
	{
		case 0: lineValue = 1; break;
		case 1: lineValue = 2; break;
		case 2: lineValue = 3; break;
		case 3: lineValue = 4; break;
		default: lineValue = 1; break;
	}
	
	LCD_Set_Cursor(lineValue,x);
	LCD_Write_String(textVal);
	lcdoffset = strlen(textVal);
	
	if (clr)
	{
		for(i; i <= (21 - lcdoffset); i ++)
		{
			LCD_Send_A_Character(' ');
		}
	}
	
}

void LCD_checkBusy()
{
	dataOutput = 0;
	cntrlPort |= 1<<ReadWrite;
	cntrlPort &= ~1<<RS;

	while (dataPort >= 0x80)
	{
		LCD_Peek_A_Boo();
	}
	dataOutput = 0xFF; //0xFF means 0b11111111
}

void LCD_Peek_A_Boo()
{
	cntrlPort |= 1<<enableLCD;
	_delay_us(2);
	cntrlPort &= ~1<<enableLCD;
}

void LCD_Send_A_Command(unsigned char command)
{
	LCD_checkBusy();
	dataPort = command;
	cntrlPort &= ~ ((1<<ReadWrite)|(1<<RS));
	LCD_Peek_A_Boo();
	dataPort = 0;
}

void LCD_Send_A_Character(unsigned char character)
{
	LCD_checkBusy();
	dataPort = character;
	cntrlPort &= ~ (1<<ReadWrite);
	cntrlPort |= 1<<RS;
	LCD_Peek_A_Boo();
	dataPort = 0;
}

void LCD_Write_String(const char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	LCD_Send_A_Character(a[i]);
}

void LCD_Set_Cursor(char a, char b)
{
	if(a == 1)
	LCD_Send_A_Command(0x80 + b);	// Set Cursor to LINE 1 and position to value b.
	else if(a == 2)
	LCD_Send_A_Command(0xC0 + b);  // Set Cursor to LINE 2 and position to value b.
	else if(a == 3)
	LCD_Send_A_Command(0x94 + b);  // Set Cursor to LINE 3 and position to value b.
	else if(a == 4)
	LCD_Send_A_Command(0xD4 + b);  // Set Cursor to LINE 4 and position to value b.
}

void LCD_Shift_Right()
{
	LCD_Send_A_Command(0x1C);
}

void LCD_Shift_Left()
{
	LCD_Send_A_Command(0x18);
}
void LCD_Clear()
{
	LCD_Send_A_Command(0x01);
}