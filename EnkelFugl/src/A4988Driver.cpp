#include "A4988Driver.h"

const int16_t A4988Driver::MIN_SPEED = 0;
const int16_t A4988Driver::MAX_SPEED = 5;
const int16_t A4988Driver::STOP_SPEED = 0;

A4988Driver::A4988Driver(
		int enablePin, int directionPin, int stepPin,
		int modePin1, int modePin2, int modePin3
		) :
	_enablePin(enablePin),
	_directionPin(directionPin),
	_stepPin(stepPin),
	//_msPins( {modePin1, modePin2, modePin3}),
	_stepInterval(1000)
{
	_minSpeed = A4988Driver::MIN_SPEED;
	_maxSpeed = A4988Driver::MAX_SPEED;
	pinMode(_enablePin, OUTPUT);
	//temporary diable the diver while initializing
  digitalWrite(_enablePin, HIGH);
  pinMode(_stepPin, OUTPUT);
  pinMode(_directionPin, OUTPUT);


  for(int i = 0; i< 3; i++) {
    pinMode(_msPins[i], OUTPUT);
  }
	SetSpeed(A4988Driver::STOP_SPEED);
	SetDirection(0);
	//re enable the driver
	SetEnabled(1);
}

void A4988Driver::SetSpeed(const int16_t& newSpeed)
{	
	int16_t newStepType;
	switch(newSpeed)
	{
		case 0:
			newStepType = NO_STEP;
			break;
		case 1:
			newStepType = SIXTEENTH_STEP;
			break;
		case 2:
			newStepType = EIGHT_STEP;
			break;
		case 3:
			newStepType = QUARTER_STEP;
			break;
		case 4:
			newStepType = HALF_STEP;
			break;
		case 5:
			newStepType = FULL_STEP;
			break;
		default:
			newStepType = -1;
	}
	if(newStepType != -1)
	{
		_setStepType(newStepType);
		_speed = newSpeed;
	}
}

void A4988Driver::SetDirection(int16_t newDirection)
{
	if(newDirection == 0)
	{
		_direction = newDirection;
		digitalWrite(dirPin, LOW);
	} else if(newDirection == 1)
	{
		_direction = newDirection;
		digitalWrite(dirPin, HIGH);
	}
}

void A4988Driver::SetStepType(int16_t stepType)
{
	int16_t newStepType;
	switch(stepType) {
			case 16:
				newStepType = SIXTEENTH_STEP;
				break;
			case 8:
				newStepType = EIGHT_STEP;
				break;
			case 4:
				newStepType = QUARTER_STEP;
				break;
			case 2:
				newStepType = HALF_STEP;
				break;
			case 1:
				newStepType = FULL_STEP;
				break;
			case 0:
				newStepType = NO_STEP;
				break;
			default:
				newStepType = -1;
		}
	if(newStepType != -1)
	{
		_stepType = newStepType;
		for(int i = 2; i > -1; i--)
		{
			digitalWrite(msPins[i], (_stepType >> i) & 0x01);
		}
	}
}

void A4988Driver::SetEnabled(int16_t newEnabled) {
  if(newEnabled == 0) {
    _enabled = 0;
    digitalWrite(_enablePin, HIGH);
  } else if(enb == 1) {
    _enabled = 1;
    digitalWrite(_enablePin, LOW);
  }
}

void A4988Driver::Update()
{
	if((micros() - _lastStepTime) >= _stepInterval)
	{
		_doStep();
	}
}

void A4988Driver::_doStep()
{
	//TODO:
	//Use hardware waveform generator for this instead of
	//delay in the code
	digitalWrite(_stepPin, LOW);
	delayMicroseconds(2);
	digitalWrite(_stepPin, HIGH);
	lastStepTime = micros();
}


