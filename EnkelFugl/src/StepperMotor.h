#ifndef _STEPPER_MOTOR_H
#define _STEPPER_MOTOR_H


//a more appropriate name for this class would be
//PositionableMotor composed with a Motor and a
//PositionSensor.
class StepperMotor
{
public:
	StepperMotor(StepperDriver *driver);
	~StepperMotor() {};
	virtual void SetSpeed(int16_t speed) {_driver->SetSpeed(speed);};
	const static int16_t DECREASING_POS_DIRECTION;
	const static int16_t INCREASING_POS_DIRECTION;
	virtual	void SetDirection(int16_t direction) {_driver->SetDirection(direction);};
	virtual void SetMinPosition(int16_t minPos) {_minPosition = minPos;};
	void SetMaxPosition(int16_t maxPos) {_maxPosition = maxPos;};
	void SetTargetPosition(int16_t);
	virtual void Stop() {_driver->Stop();};
	virtual int16_t GetSpeed() {return _driver->_speed};
	virtual int16_t GetDirection() {return _driver->_direction};
	virtual int16_t GetMinPosition() {return _minPosition};
	virtual int16_t GetMaxPosition() {return _maxPosition};
	virtual int16_t GetTargetPosition(int16_t) {return _targetPosition;};
	bool HasTarget() {return _hasTarget;};
	virtual void Update();
private:
	StepperDriver *_driver;
	int16_t _position;
	int16_t _targetPosition;
	int16_t _minPosition;
	int16_t _maxPosition;
	int16_t _targetPostion;
	bool _hasTarget;
	state_t _state;
	int _directionPin;
	int _stepPin;
	int _msPins[3];
	void _stateChanged() {};
};

#endif
