/*
 * PWMDimmer.c
 *
 * Created: 27.01.2014 18:36:22
 *  Author: Eirik Blekesaune
 *
 *  Much of the USI implementation has been based on
 *  Adam Honse's usi_i2c_slave.c/h (). 
 */ 

#define F_CPU 20000000
#include <avr/io.h>
#include <util/delay.h>
#include "LEDDimmerSimple.h"
#include "LEDDimmer_USI.h"

#define ADDR_PINS PINA
#define ADDR_PIN0 PINA3
#define ADDR_PIN1 PINA2
#define ADDR_PIN2 PINA1
#define ADDR_PIN3 PINA0

uint8_t readI2CAddressPins()
{
	uint8_t result;
	result = ADDR_PINS & (1<<ADDR_PIN0);
	result |= ADDR_PINS & (1<<ADDR_PIN1);
	result |= ADDR_PINS & (1<<ADDR_PIN2);
	result |= ADDR_PINS & (1<<ADDR_PIN3);
	return result;
}

int main(void)
{
	//init dip switch pin pullups
	PORTA |= 0x0F;
	//set switch pins as inputs
	DDRA = 0x00;

	InitLEDDimmer();
	InitUSI(readI2CAddressPins());
	sei();

	while(1)
	{
		//Find the I2C address by reading the dip switch

		asm volatile ("nop");
		SetI2CAddress(readI2CAddressPins());
		_delay_ms(10);
	}
}
