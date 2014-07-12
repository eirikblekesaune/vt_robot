#include <Arduino.h>
#include <Wire.h>
#include "LEDDimmerDuoMini.h"
#define LED_FULL 0x07FF
#define START_BYTE 77
#define LED_NUM_MASK 0x8000
#define TWI_ADDRESS 2
cmd_t requestedCommand;
void parseTWICommand(int numBytes) 
{
	cmd_t cmd;
	int val;
	//number of bytes determine setter(3 bytes) or getter(1 byte)
	if(numBytes == 3)
	{
		cmd = static_cast<cmd_t>(Wire.read());
		val = Wire.read() << 8;
		val |= Wire.read();
		doCommand(cmd, val);
	} else if(numBytes == 1) {
		requestedCommand = static_cast<cmd_t>(Wire.read());
	} 
}
void replyToRequest();
void replyToRequest(cmd_t cmd);
void replyToRequest(cmd_t cmd)
{
	int val = -1;
	uint8_t payload[3];
	switch(cmd)
	{
		case SET_LED_1:
			val = OCR1A;
			break;
		case SET_LED_2:
			val = OCR1B;
			break;
	}
	if(val != -1)
	{
		payload[0] = static_cast<uint8_t>(cmd);
		payload[1] = static_cast<uint8_t>(val >> 8);
		payload[2] = static_cast<uint8_t>(val);
		Wire.write(payload, 3);
	}
}

void replyToRequest()
{
	cmd_t cmd;
	cmd = requestedCommand;  
	replyToRequest(cmd);
}

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
	OCR1B = 0x0000;
	interrupts();
	Wire.begin(TWI_ADDRESS);
	Wire.onReceive(parseTWICommand);
	Wire.onRequest(replyToRequest);
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
		case SET_LED_1:
			setLEDValue(0, val);
			break;
		case FLASH_LED_1:
			flashLED(0, val);
			break;
		case SET_LED_2:
			setLEDValue(1, val);
			break;
		case FLASH_LED_2:
			flashLED(1, val);
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
