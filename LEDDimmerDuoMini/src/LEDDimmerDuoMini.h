#ifndef LED_DIMMER_DUO_MINI_H
#define LED_DIMMER_DUO_MINI_H
//#include "LEDLookup.h"
enum parserState_t {
	WAITING_FOR_START_BYTE,
	WAITING_FOR_CMD,
	WAITING_FOR_HI_BYTE,
	WAITING_FOR_LO_BYTE,
	WAITING_FOR_CR,
	WAITING_FOR_LF
};

enum cmd_t {
	CLEAR,
	SET_LED_1,
	FLASH_LED_1,
	FADETIME_LED_1,
	FADE_LED_1,
	SET_LED_2,
	FLASH_LED_2,
	FADETIME_LED_2,
	FADE_LED_2,
	SET_MICRO_LED,
	NUM_COMMANDS
};

enum ledState_t {
	LED_READY,
	LED_INTERPOLATING
};

struct led_t {
	int value;
	int targetValue;
	int delta;
	bool isInterpolating;
	int fadeTime;
};

#define NUM_LEDS 2

led_t led[NUM_LEDS];

parserState_t parserState = WAITING_FOR_START_BYTE;
#endif
