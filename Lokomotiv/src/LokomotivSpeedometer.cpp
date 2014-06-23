#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "LokomotivAPI.h"
#include "LokomotivSpeedometer.h"

volatile int32_t ticks = 0;
volatile int32_t binks = 0;

void registerForwardSpeedTick()
{
	ticks++; 
}

void registerBackwardSpeedTick()
{
	ticks--;
}

ISR(TIMER3_COMPA_vect)
{
	binks = binks + 1;
}

LokomotivSpeedometer::LokomotivSpeedometer() : 
	_isrNumber(4)//pin 7 on Arduino Leonardo
{
	cli();
	//Interrupt no. 4 (INT.6) will register ticks from the encoder
	attachInterrupt(_isrNumber, registerForwardSpeedTick, CHANGE);
	//Using timer3 for calculating speed, i.e. ticks per second.
	//CTC mode with OCR3A as TOP
	TCCR3A = 0;
	TCCR3B = (1<<WGM32);
	//prescaler of 256 - CS3[2:0] 100
	TCCR3B |= (1<<CS32);
	OCR3A = 6520;//Gives overflow every 100th millisecond

	//Activate interrupt flag for timer overflow compare match A
	TIMSK3 |= (1<<OCIE3A);
	sei();
}

long LokomotivSpeedometer::GetMeasuredSpeed()
{
	long result;
	result = ticks;
	return result;
}

long LokomotivSpeedometer::GetCurrentTicks()
{
	long result;
	result = ticks;
	return result;
}

void LokomotivSpeedometer::DirectionChanged(int newDirection)
{
	if(newDirection == 0)
	{
		attachInterrupt(_isrNumber, registerForwardSpeedTick, CHANGE);
	} else {
		attachInterrupt(_isrNumber, registerBackwardSpeedTick, CHANGE);
	}
}
