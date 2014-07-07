/*
 * PWMDimmer.c
 *
 * Created: 27.01.2014 18:36:22
 *  Author: Eirik Blekesaune
 *
 */ 

#define F_CPU 20000000
#include <avr/io.h>
#include <util/delay.h>
#include "LEDDimmerSimple.h"
#include "LEDDimmer_USI.h"

#define ADDR_PINS PINA
#define ADDR_PIN_MASK 0x0F

uint8_t readTWIAddressPins()
{
	int8_t pinValues, result;
	pinValues = ADDR_PINS & ADDR_PIN_MASK;
	pinValues = ~(pinValues | 0xF0);
	result = ((pinValues & 0x08) >> 3) |
					((pinValues & 0x04) >> 1) |
					((pinValues & 0x02) << 1) |
					((pinValues & 0x01) << 3);
	return result;
}

void doCommand(uint8_t commandKey, uint16_t commandData)
{
	switch(commandKey)
	{
		case LED_VALUE_CMD:
			SetLEDValue(commandData);
			break;
		case FADE_TARGET_LED_CMD:
			SetLEDFadeTarget(commandData);
			break;
		case FADE_TIME_LED_CMD:
			SetLEDFadeTime(commandData);
			break;
		case FLASH_LED_CMD:
			FlashLED(commandData);
			break;
	}
}

int main(void)
{
	cli();
	//init dip switch pin pullups
	PORTA |= 0x0F;
	//set switch pins as inputs
	InitLEDDimmer();
	//InitUSI(readTWIAddressPins());
	SetTWIAddress(2);	
	
	sei();
	while(1)
	{
		//Find the TWI address by reading the dip switch
		while(HasQueuedCommands())
		{
			uint8_t nextCommand = GetNextCommand();
			uint16_t nextCommandData = GetDataAtTail();
			doCommand(nextCommand, nextCommandData);
		}
		asm volatile ("nop");
		UpdateLEDDimmer();
		//SetTWIAddress(readTWIAddressPins());
	}
}
