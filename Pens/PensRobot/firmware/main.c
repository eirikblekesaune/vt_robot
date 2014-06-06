/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include "VT_USI.h"

extern SET_CALLBACK *setCommands;
extern GET_CALLBACK *getCommands;
void pause()
{
		char i;
		for(i = 0; i < 10; i++)
		{
			_delay_ms(30);
		}
}

void setPosition(int position)
{
	if(position == 0)
	{
		PORTB &= ~(1<<PORTB4);
		PORTB |= (1<<PORTB3);
	} else if(position == 1)
	{
		PORTB |= (1<<PORTB4);
		PORTB &= ~(1<<PORTB3);
	}
	pause();
	PORTB = 0x00;
}

uint8_t getPosition()
{
	uint8_t result;
	if(PINB & (1<<PORTB1))
	{
		result = 1;
	} else {
		result = 0;
	}
	return result; 
}

enum _commandEnum{
	POSITION,
	NUM_COMMANDS
} commandEnum;

int main(void)
{
	/* insert your hardware initialization here */
	DDRB |= (1 << PORTB4) | (1<< PORTB3);//OUTPUTS FOR MOTOR
	DDRB &= ~(1<<PORTB1);//input for button
	InitUSI(4);
	AddSetCommandCallback(POSITION, setPosition);
	AddGetCommandCallback(POSITION, getPosition);
	for(;;){
		while(HasQueuedCommands())
		{
			uint8_t commandID = GetNextCommandID();
			if(commandID < NUM_COMMANDS)
			{
				
			}
		}
	}
	return 0;
}
