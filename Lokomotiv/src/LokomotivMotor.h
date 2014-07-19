#ifndef LOKOMOTIV_MOTOR_H
#define LOKOMOTIV_MOTOR_H

#include "PID_v1.h"
#include "MotorDriver.h"
#include "LokomotivSpeedometer.h"

#define NO_TARGET -1
class LokomotivMotor: public MotorDriver{
public:
	LokomotivMotor(LokomotivSpeedometer* speedometer);
	~LokomotivMotor();
	static const int kSpeedMin;
	static const int kSpeedMax;
	static const int kSpeedStop;
	static const long kSpeedUpdateInterval;
	enum {MANUAL_MODE, CRUISE_CONTROL_MODE};
	void init() {};
	void SetSpeed(speed_t speed);
	void Stop(int stopTime);
	void SetEndSpeed(speed_t endSpeed) {_endSpeed = endSpeed;};
	speed_t GetEndSpeed() {return _endSpeed;};
	void GlideToSpeed(int duration);
	speed_t GetStartSpeed() {return _startSpeed;};
	void InterpolateSpeed(speed_t begin, speed_t target, int duration);
	void SetDirection(int direction);
	void SetBrake(speed_t brake) {};
	bool GetMotorMode() {return _motorMode;};
	void SetMotorMode(int val);
	void SetPidTargetSpeed(double val);
	void SetPidPValue(double val);
	void SetPidIValue(double val);
	void SetPidDValue(double val);
	void UserChangedSpeed();
	double GetPidTargetSpeed() {return _setpoint;};
	void Update();
	void ResetPositionCounter();
	void UpdateDirection();
private:
	LokomotivSpeedometer* _speedometer;
	PID *_pid;
	int _motorMode;
	double _setpoint;
	double _input;
	double _output;
	bool _isInterpolating;
	speed_t _beginSpeed;
	speed_t _endSpeed;
	speed_t _startSpeed;
	bool _hasReachedEndSpeed();
	long _lastSpeedUpdateTime;
	float _speedInterpolationDelta;
	unsigned char _INA;
	unsigned char _INB;
	unsigned char _PWM;
	
};
#endif

