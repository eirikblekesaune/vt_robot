#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "Arduino.h"

enum direction_t {
	DIRECTION_UP,
	DIRECTION_DOWN
};

enum buttonState_t {
	BUTTON_RELEASED = HIGH,
	BUTTON_PRESSED = LOW
};

enum motorState_t {
	STATE_AT_TOP,
	STATE_AT_BOTTOM,
	STATE_STOPPED,
	STATE_GOING_UP,
	STATE_GOING_DOWN
};

class DCMotor
{
	public:
		DCMotor(int, int, int, int);
		~DCMotor() {};
		void SetSpeed(int speed);
		void Stop();
		void Update();
		int _state;
		direction_t GetDirection();
	private:
		int _speed;
		int _number;
		bool _isBlocked(int speed);
		void _readButtons();
		int _pulsePin;
		int _topStopButtonPin;
		int _bottomStopButtonPin;
		int _topStopButton;
		int _bottomStopButton;
};

#endif
