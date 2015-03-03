#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <Arduino.h>
#include "LokomotivAPI.h"

typedef int speed_t;

//Rename to DCMotorDriver?
class MotorDriver
{
	public:
		MotorDriver() {};
		virtual void init() = 0;// Set pin directions etc.

		virtual void SetSpeed(speed_t speed) = 0;
		virtual void SetDirection(int direction) = 0;
		virtual void SetBrake(speed_t brake) = 0;

		virtual speed_t GetSpeed() { return _speed; };
		virtual int GetDirection() { return _direction; };
		virtual speed_t GetBrake() { return _brake; };

		virtual void UpdateDirection();
	protected:
		speed_t _speed;
		int _direction;
		speed_t _brake;
};


#endif
