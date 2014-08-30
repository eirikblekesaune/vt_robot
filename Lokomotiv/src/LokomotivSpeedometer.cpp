#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "LokomotivAPI.h"
#include "LokomotivSpeedometer.h"

volatile int32_t ticks = 0;
volatile int32_t measuredTicks = 0;
volatile int32_t lastMeasuredTicks = 0;
volatile int32_t ticksDelta = 1;
volatile int32_t rawSpeed = 0;
double measuredSpeed = 0.0;
double lastMeasuredSpeed = 0.0;

ISR(INT6_vect) {
	ticks = ticks + ticksDelta;
}

ISR(TIMER3_COMPA_vect, ISR_NOBLOCK)
{
	measuredTicks = ticks - lastMeasuredTicks;
	lastMeasuredTicks = ticks;
	lastMeasuredSpeed = measuredSpeed;
	rawSpeed = measuredSpeed;
	measuredSpeed = (static_cast<double>(measuredTicks) * 0.5) + (lastMeasuredSpeed * 0.5);
}

LokomotivSpeedometer::LokomotivSpeedometer() : 
	_isrNumber(4)//pin 7 on Arduino Leonardo
{
	cli();
	//Interrupt no. 4 (INT.6) will register ticks from the encoder
	//attachInterrupt(_isrNumber, registerForwardSpeedTick, CHANGE);
	EICRB = (EICRB & ~((1<<ISC60) | (1<<ISC61))) | (CHANGE << ISC60);
	EIMSK |= (1<<INT6);
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

double LokomotivSpeedometer::GetMeasuredSpeed()
{
	return measuredSpeed;
}

long LokomotivSpeedometer::GetRawSpeed()
{
	return rawSpeed;
}

double LokomotivSpeedometer::GetMeasuredTicks()
{
	long result;
	result = measuredTicks;
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
		//attachInterrupt(_isrNumber, registerForwardSpeedTick, CHANGE);
		ticksDelta = 1;
	} else {
		//attachInterrupt(_isrNumber, registerBackwardSpeedTick, CHANGE);
		ticksDelta = -1;
	}
}
