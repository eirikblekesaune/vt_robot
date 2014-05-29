#include "LEDDimmerSimple.h"
uint16_t ledValue = 0;
uint16_t fadeTime = 0;
uint16_t fadeTarget = NO_FADE;

uint8_t command = 0;
uint8_t address;

double fadeFactor = 1.0;
void InitLEDDimmer()
{
	//set pin direction for PWM pin
	LED_DDR |= (1<<LED_PIN); //set as output
	LED_PORT &= ~(1<<LED_PIN);//clear port bit
	TCCR1A = 0b00100000;
	TCCR1B = 0b00010001;
	ICR1 = 0x0FFF;
	OCR1B = 0x0000;
}

void SetLEDValue(uint16_t val)
{
	if(val < LED_MIN_VALUE)
	{
		val = LED_MIN_VALUE;
	} else if(val > LED_MAX_VALUE)
	{
		val = LED_MAX_VALUE;
	}
	ledValue = val;
	OCR1B = ledValue;
}

void FlashLED(uint16_t val)
{
	int i;
	OCR1B = LED_MAX_VALUE;
	for(i = 0; i < val; i++)
	{
		_delay_ms(10);
	}
	OCR1B = ledValue;
}

void SetLEDFadeTime(uint16_t val)
{
	fadeTime = val;
}

void SetLEDFadeTarget(uint16_t val)
{
	fadeTarget = val;
}

uint16_t GetLEDValue()
{
	return ledValue;
}

uint16_t GetLEDFadeTime()
{
	return fadeTime;
}

uint16_t GetLEDFadeTarget()
{
	return fadeTarget;
}
