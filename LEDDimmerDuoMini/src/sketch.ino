#include <Arduino.h>
#include "LEDDimmerDuoMini.h"
#define LED_FULL 0x07FF
#define START_BYTE 77
#define LED_NUM_MASK 0x8000

void setup()
{
	Serial.begin(9600);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	noInterrupts();
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
	ICR1 = 0x07FF;
	OCR1A = 0x0000;
	OCR1B = 0x07FF;
	interrupts();
}

void setLEDValue(int num, int val)
{
	if(val > ICR1)
		val = ICR1;
	else if(val < 0)
		val = 0;
	switch(num)
	{
		case 0:
			OCR1A = val;
			break;
		case 1:
			OCR1B = val;
			break;
	}
}

void flashLED(int num, int val)
{
	int tempVal;
	switch(num)
	{
		case 0:
			tempVal = OCR1A;
			OCR1A = LED_FULL;
			delay(val);
			OCR1A = tempVal;
		 	break;
		case 1:
			tempVal = OCR1B;
			OCR1B = LED_FULL;	
			delay(val);
			OCR1B = tempVal;
			break;
	}
}

void doCommand(cmd_t cmd, int val)
{
	switch(cmd)
	{
		case CLEAR:
			setLEDValue(0, 0);
			setLEDValue(1, 0);
			break;
		case SET_LED:
			setLEDValue((val & LED_NUM_MASK) >> 15, val & 0x7FF);
			break;
		case FLASH_LED:
			flashLED((val & LED_NUM_MASK) >> 15, val & 0x7FF);
			break;
	}
}

void loop()
{
	static int val;
	static cmd_t cmd;
	static uint8_t state;
	if(Serial.available() > 0)
	{
		uint8_t currentByte = Serial.read();
		switch(state)
		{
			case WAITING_FOR_START_BYTE:
				if(currentByte == START_BYTE)
					state = WAITING_FOR_CMD;
				break;
			case WAITING_FOR_CMD:
				cmd = static_cast<cmd_t>(currentByte);
				state = WAITING_FOR_HI_BYTE;
				break;
			case WAITING_FOR_HI_BYTE:
				val = static_cast<int>(currentByte) << 8;
				state = WAITING_FOR_LO_BYTE; 
				break;
			case WAITING_FOR_LO_BYTE:
				val |= currentByte;
				state = WAITING_FOR_CR;
				break;
			case WAITING_FOR_CR:
				if(currentByte == 13)
					state = WAITING_FOR_LF;
				else 
					state = WAITING_FOR_START_BYTE;
				break;
			case WAITING_FOR_LF:
				if(currentByte == 10)
					doCommand(cmd, val);
				state = WAITING_FOR_START_BYTE;
		}
	}
}
