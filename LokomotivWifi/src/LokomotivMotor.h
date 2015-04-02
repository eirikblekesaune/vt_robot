#ifndef LOKOMOTIV_MOTOR_H
#define LOKOMOTIV_MOTOR_H

#include "PID_v1.h"
#include "MotorDriver.h"
#include "LokomotivWIFI.h"
#include "LokomotivSpeedometer.h"

class Lokomotiv;//forward declaration

class LokomotivMotor: public MotorDriver{
public:
	LokomotivMotor(LokomotivSpeedometer* speedometer, Lokomotiv* lokomotiv);
	~LokomotivMotor();
	static const int kSpeedMin;
	static const int kSpeedMax;
	static const int kSpeedStop;
	static const long kSpeedUpdateInterval;
	enum {MANUAL_MODE, CRUISE_CONTROL_MODE, TARGET_SPEED_MODE};
	enum {SPEED_INCREASING, SPEED_DECREASING};
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
	long GetMotorMode() {return _motorMode;};
	void SetMotorMode(long val);
	void SetPidPValue(double val);
	void SetPidIValue(double val);
	void SetPidDValue(double val);
	double GetPidPValue();
	double GetPidIValue();
	double GetPidDValue();
	void UserChangedSpeed();
	void SetMinTargetSpeed(double val);
	void SetMaxTargetSpeed(double val);
	double GetMinTargetSpeed();
	double GetMaxTargetSpeed();

	void Update();
	void UpdateDirection();
private:
	LokomotivSpeedometer* _speedometer;
	Lokomotiv* _lokomotiv;
	PID *_pid;
	void _setDriverPWM(speed_t val);
	long _motorMode;
	double _setpoint;
	double _input;
	double _output;
	double _minTargetSpeed;
	double _maxTargetSpeed;
	bool _targetSpeedReached;
	double _targetSpeed;
	int _speedTargetingDirection;
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
	bool _cruiseControlActive;
};

#include "Lokomotiv.h"
#endif

