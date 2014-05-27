#ifndef LED_DIMMER_SIMPLE_H
#define LED_DIMMER_SIMPLE_H

#include <avr/io.h>
#include <util/delay.h>
#define LED_PIN PORTA5
#define LED_PORT PORTA
#define LED_DDR DDRA

#define LED_MIN_VALUE 0x0000
#define LED_MAX_VALUE 0x0FFF
#define NO_FADE 0xFFFF

void InitLEDDimmer();
void SetLEDValue(uint16_t val);
void FlashLED();
void SetLEDFadeTime(uint16_t);
void SetLEDFadeTarget(uint16_t);

uint16_t GetLEDValue();
uint16_t GetLEDFadeTime();
uint16_t GetLEDFadeTarget();

#endif

