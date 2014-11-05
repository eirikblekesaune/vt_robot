#ifndef SERVO_CHANNEL_H
#define SERVO_CHANNEL_H
#include <Arduino.h>
#include <Servo.h>

static const int32_t kUpdateInterval = 50;
static const int32_t kMinPosition = 0;
static const int32_t kMaxPosition = 4095;

class ServoChannel {
public:
	ServoChannel(int8_t pinNumber, int8_t channelNumber);
	void SetPosition(int16_t);
	void SetDuration(int16_t value) {_duration = value;};
	void SetAcceleration(int16_t value) {_acceleration = value;};
	void SetMin(int16_t value) {_min = value;};
	void SetMax(int16_t value) {_max = value;};
	int16_t GetPosition() {return _position;};
	int16_t GetDuration() {return _duration;};
	int16_t GetAcceleration() {return _acceleration;};
	int16_t GetMin() {return _min;};
	int16_t GetMax() {return _max;};
	void GoToPosition(int16_t target, int32_t duration);
	void GoToPosition(int16_t target);
	void Update();
private:
	Servo* _servo;
	int16_t _position;
	int8_t _pinNumber;
	int16_t _duration;
	int16_t _min;
	int16_t _max;
	int16_t _acceleration;
	int32_t _interpolationDuration;
	int8_t _channelNumber;
	bool _isInterpolating;
	int16_t _positionInterpolationDelta;
	int16_t _beginPosition;
	int16_t _endPosition;	
	int32_t _lastPositionUpdateTime;
	int32_t _speedUpdateInterval;
	bool _hasReachedEndPosition();
};



#endif
