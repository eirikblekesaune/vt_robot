/*
 * IRBeacon.c
 *
 * Created: 14.01.2014 22:25:46
 *  Author: Eirik
 */ 

#define F_CPU 8000000UL
#define IR_LED PORTB2
#define START_BIT_ON_PERIOD 9000
#define START_BIT_OFF_PERIOD 4500
#define ONE_BIT_ON_PERIOD 560
#define ONE_BIT_OFF_PERIOD 1690
#define ZERO_BIT_ON_PERIOD 560
#define ZERO_BIT_OFF_PERIOD 560
#define STOP_BIT_ON_PERIOD 560
#define STOP_BIT_OFF_PERIOD 1690
#define NUM_BITS 32
#define TOPBIT 0x80000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SEQUENCE_LENGTH 3
unsigned long leadSequence[SEQUENCE_LENGTH] = {
	0x11111111,
	0x22222222,
	0x33333333
};
unsigned long magicNumber = 0xABCDEF00;

int sequenceCounter = 0;
unsigned long address;
int i = 0;

#define ON (DDRB = 0x04)
#define OFF (DDRB = 0x00)

void delayMicros(unsigned int us)
{
	if (--us == 0)
		return;
	if (--us == 0)
		return;
	us <<= 1;
	us--;
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t"
		"brne 1b" : "=w" (us) : "0" (us)
	);
}

void sendNECByte(unsigned long data)
{	
	//enable PWM output
	ON;
	delayMicros(START_BIT_ON_PERIOD);	
	
	//disable PWM output
	OFF;
	delayMicros(START_BIT_OFF_PERIOD);
	
	//iterate data
	for(i = 0; i < 32; i++)
	{
		if(data & TOPBIT)
		{
			//enable PWM output
			ON;
			delayMicros(ONE_BIT_ON_PERIOD);
			
			//disable PWM output
			OFF;
			delayMicros(ONE_BIT_OFF_PERIOD);
		} else {
			//enable PWM output
			ON;
			delayMicros(ZERO_BIT_ON_PERIOD);
			
			//disable PWM output
			OFF;
			delayMicros(ZERO_BIT_OFF_PERIOD);
		}
		//pop topbyte and shift left <<=
		data <<= 1;
	}

		//enable PWM output
		ON;
		delayMicros(STOP_BIT_ON_PERIOD);
		
		//disable PWM output
		OFF;
		delayMicros(STOP_BIT_OFF_PERIOD);
}


int main(void)
{
	//initialize pin directions and pull-ups
	DDRA = 0x00;//all pins on PORTA are inputs
	PORTA = 0xFF;//activate all internal pull-ups
	
	//read initial button mask address
	address = 0x00;
	
	DDRB |= (1<<IR_LED);
	//setup timer registers
	TCCR0A |= (1 << WGM01) | (1<<COM0A0);//set fast PWM with OCRA top
	TCCR0B |= (1 << CS00); //set prescaler to 1
	OCR0A = 104;//output compare for getting 38kHz
	sei();
	
	//start timer interrupts
    while(1)
    {
			sendNECByte(leadSequence[0]);
			_delay_ms(50);
			sendNECByte(leadSequence[1]);
			_delay_ms(50);
			sendNECByte(leadSequence[2]);
			_delay_ms(50);
			sendNECByte(address | magicNumber);
			//read address bytes again
			address = (unsigned long)(PINA);
			_delay_ms(100);
    }
}

