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

#define I2C_BUFFER_SIZE 16
#define I2C_BUFFER_MASK ( I2C_BUFFER_SIZE - 1 )
#define TRUE                1
#define FALSE               0

/////////////////////////////////////////////////
////I2C Direction Macros/////////////////////////
/////////////////////////////////////////////////

#define USI_SET_SDA_OUTPUT()	{ DDR_USI |=  (1 << PORT_USI_SDA); }
#define USI_SET_SDA_INPUT() 	{ DDR_USI &= ~(1 << PORT_USI_SDA); }

#define USI_SET_SCL_OUTPUT()	{ DDR_USI |=  (1 << PORT_USI_SCL); }
#define USI_SET_SCL_INPUT() 	{ DDR_USI &= ~(1 << PORT_USI_SCL); }

#define USI_SET_BOTH_OUTPUT()	{ DDR_USI |= (1 << PORT_USI_SDA) | (1 << PORT_USI_SCL); }
#define USI_SET_BOTH_INPUT() 	{ DDR_USI &= ~((1 << PORT_USI_SDA) | (1 << PORT_USI_SCL)); }

//! Functions implemented as macros
#define SET_USI_TO_SEND_ACK()                                                                                 \
{                                                                                                             \
        USIDR    =  0;                                              /* Prepare ACK                         */ \
        DDR_USI |=  (1<<PORT_USI_SDA);                              /* Set SDA as output                   */ \
        USISR    =  (0<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|  /* Clear all flags, except Start Cond  */ \
                    (0x0E<<USICNT0);                                /* set USI counter to shift 1 bit. */ \
}

//command message enum
enum _command_t {
	NO_CMD,
	LED_VALUE_CMD,
	FADE_LED_CMD,
	FLASH_LED_CMD
} command_t;


void InitUSI(uint8_t address);
void StartI2C();
void StopI2C();
void SetI2CAddress(uint8_t address);
void FlushI2CBuffers();

#endif
