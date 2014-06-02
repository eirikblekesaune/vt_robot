#ifndef LED_DIMMER_USI_H
#define LED_DIMMER_USI_H

#include "avr/io.h"
#include "avr/interrupt.h"

#define DDR_USI			DDRA
#define PORT_USI		PORTA
#define PIN_USI			PINA
#define PORT_USI_SDA	PORTA6
#define PORT_USI_SCL	PORTA4
#define PIN_USI_SDA		PINA6
#define PIN_USI_SCL		PINA4

#define TWI_BUFFER_SIZE 16
#define TWI_BUFFER_MASK ( TWI_BUFFER_SIZE - 1 )
#define TRUE                1
#define FALSE               0

/////////////////////////////////////////////////
////TWI Direction Macros/////////////////////////
/////////////////////////////////////////////////

#define USI_SET_SDA_OUTPUT()	{ DDR_USI |=  (1 << PORT_USI_SDA); }
#define USI_SET_SDA_INPUT() 	{ DDR_USI &= ~(1 << PORT_USI_SDA); }

#define USI_SET_SCL_OUTPUT()	{ DDR_USI |=  (1 << PORT_USI_SCL); }
#define USI_SET_SCL_INPUT() 	{ DDR_USI &= ~(1 << PORT_USI_SCL); }

#define USI_SET_BOTH_OUTPUT()	{ DDR_USI |= (1 << PORT_USI_SDA) | (1 << PORT_USI_SCL); }
#define USI_SET_BOTH_INPUT() 	{ DDR_USI &= ~((1 << PORT_USI_SDA) | (1 << PORT_USI_SCL)); }

//! Functions implemented as macros
#define SET_USI_TO_SEND_ACK()                                                                   \
{                                                                                               \
	USIDR    =  0;                                /* Prepare ACK                         */ \
	DDR_USI |=  (1<<PORT_USI_SDA);                /* Set SDA as output                   */ \
	USISR    =  0xEE; /* Clear all flags, except Start Cond  */ \
}

#define SET_USI_TO_READ_ACK()\
{\
	DDU_USI &= ~(1<<PORT_USI_SDA);/*Set SDA as input*/\
	USIDR = 0;\
	USISR = 0xEE;/*Clear all flags execpt start condition, set counter to count 1 bit*/\
}\


#define SET_USI_TO_RECEIVE_START_CONDITION_MODE() \
{\
	USIDR = 0;\
	DDR_USI |= (1<<PORT_USI_SDA);\
	DDR_USI &= ~(1<<PORT_USI_SCL);\
	USICR = 0b10101000;/*enable start cond interrrupt and disable overflow interrupt*/\
	USISR = 0b11110000;/*clear all flags and set counter to 0*/\
}\

#define SET_USI_TO_RECEIVE_DATA_BYTES_MODE()\
{\
	USIDR = 0;/*Clear data*/\
	USICR = 0b11100000;/*Enable counter overflow interrupt, */\
	USISR = 0b01110000; /*Clear flags execpt start cond, set counter to 0*/\
}\
	



uint8_t GetNextCommand();
uint16_t GetDataAtTail();
uint8_t HasQueuedCommands();
void InitUSI(uint8_t address);
void StartTWI();
void StopTWI();
void SetTWIAddress(uint8_t address);
void FlushTWIBuffers();

#endif
