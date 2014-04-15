#ifndef _A4988_DRIVER_H
#define _A4988_DRIVER_H

#include <Arduino.h>
#include "StepperDriver.h"

class A4988Driver : public StepperDriver
{
public:
	A4988Driver(int enablePin, int directionPin, int stepPin, int modePin1, int modePin2, int modePin3);
	~A4988Driver();
	const static int16_t MIN_SPEED;
	const static int16_t MAX_SPEED;
	const static int16_t STOP_SPEED;
	enum {FULL_STEP = 0x0, HALF_STEP = 0x1, QUARTER_STEP = 0x2, EIGHT_STEP = 0x3, SIXTEENTH_STEP = 0x7, NO_STEP = 0xF};
	virtual void SetSpeed();
	virtual void SetDirection();
	void SetStepType();
	void SetEnabled(int16_t);
	int16_t GetStepType() {return _stepType;};
	int16_t GetEnabled() {return _enabled;};
	virtual void Update();
private:
	int16_t _stepType;
	uint32_t _lastStepTime;
	uint32_t _stepInterval;
	int16_t _enabled;
	int _enablePin;
	int _directionPin;
	int _stepPin;
	int _msPins[3];
	void _doStep();
};
#endif
