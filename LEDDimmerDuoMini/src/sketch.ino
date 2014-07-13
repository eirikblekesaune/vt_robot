#include <Arduino.h>
#include <Wire.h>
#include "LEDDimmerDuoMini.h"
#define LED_FULL 0x07FF
#define START_BYTE 77
#define LED_NUM_MASK 0x8000
#define TWI_ADDRESS 2

cmd_t requestedCommand;
long lastUpdate = 0;
long updateInterval = 5;

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
			val = led[0].value;
			break;
		case SET_LED_2:
			val = led[1].value;
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
	for(int i = 0; i < 2; i++)
	{
		led[i].value = 0;
		led[i].targetValue = 0;
		led[i].delta = 0;
		led[i].isInterpolating = false;
		led[i].fadeTime = 250;
	}
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
	led_t *myLed = &led[num];
	myLed->value = val;
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
	led_t * myLed = &led[num];
	switch(num)
	{
		case 0:
			OCR1A = LED_FULL;
			delay(val);
			OCR1A = myLed->value;
		 	break;
		case 1:
			OCR1B = LED_FULL;	
			delay(val);
			OCR1B = myLed->value;
			break;
	}
}

void setLEDFadetime(int num, int val)
{
	led[num].fadeTime = val;
}

void fadeLED(int num, int val)
{
	led_t *myLed;
	int fadeTime, startValue, endValue, delta; 
	myLed = &led[num];
	startValue = myLed->value;
	if(startValue != val)
	{
		endValue = val;
		fadeTime = myLed->fadeTime;
		if(fadeTime <= updateInterval)
			fadeTime = updateInterval + 1;
		int distance;
		int numSteps;
		distance = endValue - startValue;
		numSteps = fadeTime / static_cast<int>(updateInterval);
		delta = distance / numSteps; 
		if(distance < 0)
		{
			if(delta == 0)
				delta = -1;
		} else {
			if(delta == 0)
				delta = 1;	
		}
		//store values in object
		myLed->delta = delta;
		myLed->isInterpolating = true;
		myLed->targetValue = endValue;

	}
}

void updateLEDInterpolationValue(int ledNum)
{
	led_t *myLed;
	int nextVal, lastValue, delta, targetValue;
	uint16_t result;
	myLed = &led[ledNum];
	lastValue = myLed->value;
	delta = myLed->delta;
	targetValue = myLed->targetValue;
	nextVal = lastValue + delta;
	if(delta < 0)
	{
		nextVal = max(nextVal, targetValue);
		if(nextVal == targetValue)
			myLed->isInterpolating = false;
	} else if(delta > 0)
	{
		nextVal = min(nextVal, targetValue);
		if(nextVal == targetValue)
			myLed->isInterpolating = false;
	}
	setLEDValue(ledNum, nextVal); 
}

void doCommand(cmd_t cmd, int val)
{
	switch(cmd)
	{
		case CLEAR:
			setLEDValue(0, 0);
			led[0].isInterpolating = false;
			setLEDValue(1, 0);
			led[1].isInterpolating = false;
			break;
		case SET_LED_1://cmd num 1
			setLEDValue(0, val);
			//cancel any ongoing interpolation
			led[0].isInterpolating = false;
			break;
		case FLASH_LED_1://cmd num 2
			flashLED(0, val);
			break;
		case FADETIME_LED_1://cmd num 3
			setLEDFadetime(0, val);
			break;
		case FADE_LED_1://cmd num 4
			fadeLED(0, val);
			break;
		case SET_LED_2://cmd num 5
			setLEDValue(1, val);
			//cancel any ongoing interpolation
			led[1].isInterpolating = false;
			break;
		case FLASH_LED_2://cmd num 6
			flashLED(1, val);
			break;
		case FADETIME_LED_2://cmd num 7
			setLEDFadetime(1, val);
			break;
		case FADE_LED_2://cmd num 8
			fadeLED(1, val);
			break;
	}
}

void loop()
{
	static int val;
	static cmd_t cmd;
	static uint8_t serialParserState;
	if(Serial.available() > 0)
	{
		uint8_t currentByte = Serial.read();
		switch(serialParserState)
		{
			case WAITING_FOR_START_BYTE:
				if(currentByte == START_BYTE)
					serialParserState = WAITING_FOR_CMD;
				break;
			case WAITING_FOR_CMD:
				cmd = static_cast<cmd_t>(currentByte);
				serialParserState = WAITING_FOR_HI_BYTE;
				break;
			case WAITING_FOR_HI_BYTE:
				val = static_cast<int>(currentByte) << 8;
				serialParserState = WAITING_FOR_LO_BYTE; 
				break;
			case WAITING_FOR_LO_BYTE:
				val |= currentByte;
				serialParserState = WAITING_FOR_CR;
				break;
			case WAITING_FOR_CR:
				if(currentByte == 13)
					serialParserState = WAITING_FOR_LF;
				else 
					serialParserState = WAITING_FOR_START_BYTE;
				break;
			case WAITING_FOR_LF:
				if(currentByte == 10)
					doCommand(cmd, val);
				serialParserState = WAITING_FOR_START_BYTE;
		}
	}
	if(millis() >= (lastUpdate + updateInterval))
	{
		for(int i = 0; i < NUM_LEDS; i++)
		{
			if(led[i].isInterpolating)
			{
				updateLEDInterpolationValue(i);
			}
		}
		lastUpdate = millis();
	}
}
