#ifndef _STEPPER_DRIVER_H
#define _STEPPER_DRIVER_H
class StepperDriver
{
public:
	StepperDriver() {};
	~StepperDriver() {};
	static const int16_t MIN_SPEED;
	static const int16_t MAX_SPEED;
	static const int16_t STOP_SPEED;
	virtual void SetSpeed(uint16_t) = 0;
	virtual void SetDirection(uint16_t) = 0;
	virtual int16_t GetSpeed() { return _speed; };
	virtual int16_t GetDirection() { return _direction; };
	virtual void Stop() { SetSpeed(STOP_SPEED); };
	virtual void Update() {};
protected:
	int16_t _speed;
	int16_t _direction;
};

#endif
