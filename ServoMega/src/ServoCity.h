#ifndef SERVO_CITY_H
#define SERVO_CITY_H
#include <Arduino.h>
#include <Servo.h>

static const int32_t kUpdateInterval = 50;
static const int32_t kMinPosition = 0;
static const int32_t kMaxPosition = 4095;
#define NUM_CHANNELS 44
static const int8_t channelToPinNumberMappings[NUM_CHANNELS] = {
  2,//channel 1
 	3,//channel 2
 	4,//channel 3
 	5,//channel 4
 	6,//channel 5
 	7,//channel 6
 	8,//channel 7
 	9,//channel 8
 	10,//channel 9
 	11,//channel 10
 	12,//channel 11
 	13,//channel 12
 	22,//channel 13
 	23,//channel 14
 	24,//channel 15
 	25,//channel 16
 	26,//channel 17
 	27,//channel 18
 	28,//channel 19
 	29,//channel 20
 	30,//channel 21
 	31,//channel 22
 	32,//channel 23
 	33,//channel 24
 	34,//channel 25
 	35,//channel 26
 	36,//channel 27
 	37,//channel 28
 	38,//channel 29
 	39,//channel 30
 	40,//channel 31
 	41,//channel 32
 	42,//channel 33
 	43,//channel 34
 	44,//channel 35
 	45,//channel 36
 	46,//channel 37
 	47,//channel 38
 	48,//channel 39
 	49,//channel 40
 	50,//channel 41
 	51,//channel 42
 	52,//channel 43
 	53//channel 44
};

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
	void Update();
private:
	Servo* _servo;
	int16_t _position;
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
