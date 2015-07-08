#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "Arduino.h"

enum direction_t {
	DIRECTION_R,
	DIRECTION_L
};

enum buttonState_t {
	BUTTON_RELEASED = HIGH,
	BUTTON_PRESSED = LOW
};

class DCMotor
{
	public:
		DCMotor(int, int, int);
		~DCMotor() {};
		void SetSpeed(int speed);
		void Stop();
		void Update();
		direction_t GetDirection();
	private:
		int _speed;
		void _checkSensors();
		bool _canSetSpeed(int speed);
		int _pulsePin;
		int _stopButtonPinR;
		int _stopButtonPinL;
		int _stopButtonR;
		int _stopButtonL;
};


#endif
