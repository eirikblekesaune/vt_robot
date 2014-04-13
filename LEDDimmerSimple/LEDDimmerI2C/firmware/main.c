/*
 * PWMDimmer.c
 *
 * Created: 27.01.2014 18:36:22
 *  Author: Eirik
 */ 

#define F_CPU 20000000
#include <avr/io.h>
#include <util/delay.h>
#include "usi_i2c_slave.h"

#define LED_PIN PORTA5
#define LED_PORT PORTA
#define LED_DDR DDRA
#define DEVICE_ADDRESS 2

//command message enum
enum _command_t {
	NO_CMD,
	LED_VALUE_CMD,
	FADE_LED_CMD,
	FLASH_LED_CMD
} command_t;

uint32_t inData = 0;
uint8_t command = 0;
extern char* USI_Slave_register_buffer[];

uint16_t ledValue = 0;
double fadeFactor = 1.0;
//set pullups for dip dwitch pins
//
//
void setFadeValues(uint16_t targetValue, uint16_t fadeTime)
{
}

int main(void)
{
	//set pin direction for PWM pin
	LED_DDR |= (1<<LED_PIN);
	LED_PORT &= ~(1<<LED_PIN);
	TCCR1A = 0b00100000;
	TCCR1B = 0b00010001;
	ICR1 = 0x0FFF;
	OCR1B = 0x0000;
	sei();
	//initialize I2C module
	USI_I2C_Init(DEVICE_ADDRESS);
	USI_Slave_register_buffer[0] = (char*)&command;
	USI_Slave_register_buffer[1] = (char*)&inData;
	USI_Slave_register_buffer[2] = (char*)&inData + 1; 
	USI_Slave_register_buffer[3] = (char*)&inData + 2;
	USI_Slave_register_buffer[4] = (char*)&inData + 3;

	while(1)
    {
		
		//Find the I2C address by reading the dip switch
		////if this has changed
		////reinit the i2c connection	
		if(command != NO_CMD)
		{
			switch(command)
			{
				case LED_VALUE_CMD:
					OCR1B = inData;
					break;
				case FADE_LED_CMD:
					setFadeValues((uint16_t)&inData, (uint16_t)&inData + 2);
			}
			command = NO_CMD;
		}


		asm volatile ("nop");	
		_delay_ms(10);
	}
}
