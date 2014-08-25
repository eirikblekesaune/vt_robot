#ifndef SERVO_CHANNEL_H
#define SERVO_CHANNEL_H
#include <Arduino.h>
#include <Servo.h>

static const int32_t kUpdateInterval = 50;
static const int32_t kMinPosition = 0;
static const int32_t kMaxPosition = 4095;

class ServoChannel {
public:
	ServoChannel(int8_t pinNumber);
	void SetPosition(int16_t);
	void SetSpeed(int16_t val) {_speed = val;};
	void SetAcceleration(int16_t) {};
	void SetMin(int16_t value) {};
	void SetMax(int16_t value) {};
	int16_t GetPosition() {return _position;};
	int16_t GetSpeed() {return _speed;};
	int16_t GetAcceleration() {return _acceleration;};
	void GoToPosition(int16_t target, int32_t duration);
	void GoToPosition(int16_t target);
	void Update();
private:
	Servo* _servo;
	int16_t _position;
	int16_t _duration;
	int16_t _min;
	int16_t _max;
	int16_t _speed;
	int16_t _acceleration;
	int32_t _interpolationDuration;
	bool _isInterpolating;
	int16_t _positionInterpolationDelta;
	int16_t _beginPosition;
	int16_t _endPosition;	
	void _setServoPosition(int16_t position);
	int32_t _lastPositionUpdateTime;
	int32_t _speedUpdateInterval;
	bool _hasReachedEndPosition();
	void _calculateInterpolationData(int8_t number, int16_t endPosition, int32_t duration) {};
};



#endif
