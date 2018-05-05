/*
 * Variables.h
 *
 * Created: 2/10/2017 5:55:38 PM
 *  Author: ptx6852
 */ 


#ifndef VARIABLES_H_
#define VARIABLES_H_

#define LED1 PORTA & (1<<7)
#define LED1_ON PORTA |= (1<<7)
#define LED1_OFF PORTA &=~ (1<<7)
#define LED1Toggle	PORTA ^= (1<<7)

#define LED2 PORTC & (1<<6)
#define LED2_ON PORTC |= (1<<6)
#define LED2_OFF PORTC &=~ (1<<6)
#define LED2Toggle	PORTC ^= (1<<6)

#define LED3 PORTD & (1<<4)
#define LED3_ON PORTD |= (1<<4)
#define LED3_OFF PORTD &=~ (1<<4)
#define LED3Toggle	PORTD ^= (1<<4)

#define LED4 PORTD & (1<<5)
#define LED4_ON PORTD |= (1<<5)
#define LED4_OFF PORTD &=~ (1<<5)
#define LED4Toggle	PORTD ^= (1<<5)

#define LED5 PORTD & (1<<6)
#define LED5_ON PORTD |= (1<<6)
#define LED5_OFF PORTD &=~ (1<<6)
#define LED5Toggle	PORTD ^= (1<<6)

#define LED6 PORTD & (1<<7)
#define LED6_ON PORTD |= (1<<7)
#define LED6_OFF PORTD &=~ (1<<7)
#define LED6Toggle	PORTD ^= (1<<7)

#define ALL_LEDS (LED1<< 0)|(LED2<< 1)|(LED3 << 2)|(LED4 << 3)| (LED5 << 4)|(LED6 << 5)
extern volatile uint8_t LS1;

extern uint64_t ROBOTS[8];
extern uint8_t LEDValues; 

typedef struct StatusUpdate
{
	uint32_t Mac1;
	uint32_t Mac2;
	uint16_t Voltage;
	uint16_t Xposition;
	uint16_t Yposition;
	int16_t Facing;
} RobotStatus;
RobotStatus specificRobot [6];

extern uint8_t Task;
extern uint16_t Voltage;
extern uint16_t Xposition;
extern uint16_t Yposition;
extern int16_t Facing;




#endif /* VARIABLES_H_ */