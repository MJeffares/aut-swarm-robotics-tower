/*
* uart_interface.c
*
* Author : Mansel Jeffares/Matthew Witt (pxf5695@autuni.ac.nz)
* Created: 27/08/2017 4:15:14 PM
*
* Project Repository: https://github.xbee/AdamParlane/aut-swarm-robotics
*
* Functions for initialising and accessing the UART interface. Also contains interrupt handlers
* for the UART interface.
*
* More Info:
* Atmel SAM 4N Processor Datasheet:http://www.atmel.com/Images/Atmel-11158-32-bit%20Cortex-M4-Microcontroller-SAM4N16-SAM4N8_Datasheet.pdf
* Relevant reference materials or datasheets if applicable
*
* Functions:
* void uart3Init(void)
* uint8_t uart3Write(uint8_t data)
* void UART3_Handler(void)
*
*/

//////////////[Includes]////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "xbee_driver.h"

//////////////[Global Variables]////////////////////////////////////////////////////////////////////
extern int FrameBufferIn;	//Used in UART3_Handler

//////////////[Functions]///////////////////////////////////////////////////////////////////////////
/*
* Function:
* void uart3Init(void)
*
* Initialises UART3 peripheral, ready for use. Used by the Xbee on the robot.
*
* Inputs:
* none
*
* Returns:
* none
*
* Implementation:
* - Peripheral clock access is supplied to UART3
* - UART3 is given control of PIOB PB10 (RX) and PB11 (TX)
* - No parity mode enabled
* - 9600bps
* - Reset the UART3 control register
* - Enable UART3
* - Enable the RX ready interrupt
*
*/
void uart1Init(void)
{
	DDRD = 0b00001000;
	
	UCSR1A = 0b00000000;
	UCSR1B = 0b10011000;
	UCSR1C = 0b00000110;
	
	UBRR1L = 47; //8MHz clk, 8x prescalar, (double speed U2X =1), 230400 baud
	
	
	
	/*
	REG_PMC_PCER0
	|=	(1<<ID_UART3);				//Enable clock access to UART3
	REG_PIOB_PDR
	|=	PIO_PB10					//Enable peripheral control of PB10 (URXD3)
	|	PIO_PB11;					//And PB11 (UTXD3) both connected to peripheral B
	REG_UART3_MR
	|=	UART_MR_CHMODE_NORMAL 
	|	UART_MR_PAR_NO;				//set as no parity, normal mode
	REG_UART3_BRGR
	//=	651;						//Set Baud rate for 9600 from a 100MHZ clock
	=	27;							//Baud 230400
	REG_UART3_CR
	|=	UART_CR_RSTRX
	|	UART_CR_RSTTX
	|	UART_CR_RSTSTA;				//Reset receiver, transmitter and status bits
	REG_UART3_CR
	|=	UART_CR_RXEN
	|	UART_CR_TXEN;				//Enable receiver and transmitter
	REG_UART3_IER
	|=	UART_IER_RXRDY;				//ENABLE UART RXRDY interrupt
	
	NVIC_EnableIRQ(ID_UART3);		//ENABLE the UART3 system interrupts	
	*/
}

/*
* Function:
* uint8_t uart3Write(uint8_t data)
*
* Allows a byte to be written to UART3
*
* Inputs:
* uint8_t data:
*   The byte to be written to the UART
*
* Returns:
* 0 on success, otherwise non zero if failed
*
* Implementation:
* Calls waitForFlag() to make sure that the UART3 TXRDY flag is set, meaning data can be placed in
* the transmit holding register. If waitForFlag() returns a 0, then place data in the transmit
* holding register. Return the value returned from waitForFlag().
*
*/
uint8_t uart1Write(uint8_t data)
{
	
	while(!(UCSR1A &(1<<5)));
	UDR1 = data;
	
	/*
	//Wait till TXRDY
	uint8_t returnVal = waitForFlag((uint32_t*)&REG_UART3_SR, UART_SR_TXRDY, UART_TXRDY_TIMEOUT); 
	if(!returnVal)
		REG_UART3_THR = data;				//place data in TX register
	return returnVal;
	*/
	return 1;
}

/*
* Function:
* void UART3_Handler(void)
*
* Interrupt handler for UART3
*
* Inputs:
* none
*
* Returns:
* none
*
* Implementation:
* TODO: Mansel, implentation description for UART3 interrupt handler
*
*/
ISR(USART1_RX_vect)
{
	//Receive States
	static enum {START, LENGTH_MSB, LENGTH_LSB, FRAME_TYPE, DATA, CHECKSUM};

	uint8_t temp;	//temporary variable to store received byte
	
	static uint8_t receiveState = START;
	static bool escape = false;		//Flag for escaping received bytes
	static int length;				//Length as reported by XBee Frame
	static int index;				//Number of bytes received that count towards length of XBee Frame
	static int check;				//Checksum calculation
	static int frame_start_index;	//The position in the FrameBuffer where the data of this XBee Frame is stored
	static int frame_type;			//The type of received XBee Frame


	temp = UDR1;	//store the incoming data in a temporary variable
	//xbeeFrameBufferPut(temp);

	if(temp == FRAME_DELIMITER && receiveState != START )//if we receive a start byte out of 															//sequence
		receiveState = START;	//reset back to the start state
	else if(temp == ESCAPE_BYTE) //if the next byte needs to be escaped
		escape = true;	//set the flag
	else if(escape) //if the current byte needs to be escaped
	{
		temp ^= 0x20;	//reverse the escape procedure
		escape = false;	//reset the flag
	}

	if(escape == false)	//we only go through the receive states if the data has been escaped
	{
		switch(receiveState)
		{
			case START:
				if(temp == FRAME_DELIMITER)
				{
					//reset our book-keeping variables and updates the receive state
					length = 0;
					index = 0;
					check = 0;
					receiveState = LENGTH_MSB;
				}
				break;

			case LENGTH_MSB:
				//Calculates the length using the first length byte and updates the receive 
				//state
				length = temp*256;
				receiveState = LENGTH_LSB;
				break;

			case LENGTH_LSB:
				//Calculates the length using the second length byte and updates the receive 
				//state
				length =+ temp;
				receiveState = FRAME_TYPE;
				break;

			case FRAME_TYPE:
				frame_type = temp;			//Receives and stores the Frame type
				check += temp;				//Calculates the checksum over the received byte
				index++;					//Updates the number of received bytes that count
											//towards the XBee frame length
				frame_start_index = FrameBufferIn;//Stores the location of the Frame Data in the
											//FrameBuffer
				receiveState = DATA;		//Updates the receive state
				break;

			case DATA:
				xbeeFrameBufferPut(temp);	//Stores the Received data into the FrameBuffer
				check += temp;				//Calculates the checksum over the received byte
				index++;					//Updates the number of received bytes that count 
											//towards the XBee frame length

				if(index == length)		//Checks if we have received all the data and if we have
										//updates the receive state
				{
					receiveState = CHECKSUM;
				}
				
				break;
				
			case CHECKSUM:
				check += temp;				//Calculates the checksum over the received byte
				check &= 0xFF;				//Final Step of checksum calculation for XBee Frame
				if(check == 0xFF)			//Verifies the calculated checksum value
					//Stores Frame info in buffer
					xbeeFrameBufferInfoPut(frame_start_index, frame_type, index -1); 
				receiveState = START;		//Resets receive state d
				break;
			
		}
	}
}
