#ifndef _STEPPER_DRIVER_H
#define _STEPPER_DRIVER_H
#include <Arduino.h>
class StepperDriver
{
public:
	StepperDriver() {};
	~StepperDriver() {};
	virtual void SetSpeed(const uint16_t&) = 0;
	virtual void SetDirection(uint16_t) = 0;
	virtual int16_t GetSpeed() { return _speed; };
	virtual int16_t GetDirection() { return _direction; };
	virtual int16_t GetMinSpeed() {return _minSpeed;};
	virtual int16_t GetMaxSpeed() {return _maxSpeed;};
	virtual void Stop() { SetSpeed(0); };
	virtual void Update() {};
protected:
	int16_t _speed;
	int16_t _direction;
	int16_t _minSpeed;
	int16_t _maxSpeed;
};

#endif
