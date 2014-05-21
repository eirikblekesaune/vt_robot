#include "LokomotivMotor.h"

const int LokomotivMotor::kSpeedMin = 0;
const int LokomotivMotor::kSpeedStop = LokomotivMotor::kSpeedMin;
const int LokomotivMotor::kSpeedMax = 511;
const long LokomotivMotor::kSpeedUpdateInterval = 50;

LokomotivMotor::LokomotivMotor() :
	_pidEnabled(false),
	_INA(12),
	_INB(11),
	_PWM(10),
	_endSpeed(50),
	_lastSpeedUpdateTime(0),
	_isInterpolating(false)
{
	pinMode(_INA, OUTPUT);
	pinMode(_INB, OUTPUT);
	pinMode(_PWM, OUTPUT);

//	//Timer1 initialization
//	//Set phase correct mode with ICR1 as TOP
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM13);

	//Set OCR1B to set on timer reset
	TCCR1A |= (1<<COM1B1);

	//Set prescaler to 1
	TCCR1B |= (1<<CS10); 
//	//set max speed
	ICR1 = 0x01FF;

	//pid_ = new PID(&setpoint_, &input_, &output_, 4.0, 0.2, 1.0, DIRECT);
	//pid_->SetOutputLimits(LokomotivMotor::kSpeedMin, LokomotivMotor::kSpeedMax);
	//pid_->SetMode(AUTOMATIC);
}

void LokomotivMotor::Update()
{
	if(_isInterpolating)
	{
		if((_lastSpeedUpdateTime + kSpeedUpdateInterval) < millis())
		{
			if(_hasReachedEndSpeed())
			{
				SetSpeed(_endSpeed);
				_isInterpolating = false;
			} else {
				SetSpeed(_speed + _speedInterpolationDelta);
			}
			_lastSpeedUpdateTime = millis();
		}
	}
//	if(pidEnabled_)
//	{
//	input_ = _speedometer->CalculateSpeed();
//		if(pid_->Compute())
//		{
//			if(driver_->GetSpeed() != kSpeedStop)
//			{
//				driver_->SetSpeed(output_);
//			}
//		}
//	}
}

void LokomotivMotor::SetSpeed(speed_t newSpeed)
{
	if(newSpeed > kSpeedMax)
		newSpeed = kSpeedMax;
	if(newSpeed < kSpeedMin)
		newSpeed = kSpeedMin;
	_speed = newSpeed;
	DebugPrint("speed");
	DebugPrint(_speed);
	OCR1B = _speed;
	if (_speed == 0)
  {
    digitalWrite(_INA, LOW);   // Make the motor coast no
    digitalWrite(_INB, LOW);   // matter which direction it is spinning.
  } else {
    UpdateDirection();
  }
}

void LokomotivMotor::SetDirection(int newDirection)
{
	if(newDirection<0)
		newDirection = 0;
	if(newDirection>1)
		newDirection = 1;
	_direction = newDirection;
	UpdateDirection();
}

void LokomotivMotor::UpdateDirection()
{
	if (!_direction)
  {
    digitalWrite(_INA, LOW);
    digitalWrite(_INB, HIGH);
  }
  else
  {
    digitalWrite(_INA, HIGH);
    digitalWrite(_INB, LOW);
  }
}

void LokomotivMotor::InterpolateSpeed(speed_t begin, speed_t target, int duration)
{
	if(_speed != target)
	{
		if(duration <= kSpeedUpdateInterval)
			duration = kSpeedUpdateInterval + 1;
		_beginSpeed = begin;
		_endSpeed = target;
		_speedInterpolationDelta = (_endSpeed - _beginSpeed) / (duration / kSpeedUpdateInterval);
		if(_beginSpeed >= _endSpeed) // if deaccelarating
		{
			if(_speedInterpolationDelta == 0)
				_speedInterpolationDelta = -1;
		} else {
			if(_speedInterpolationDelta == 0)
				_speedInterpolationDelta = 1;
		}
		_isInterpolating = true;
	} else {
		SetSpeed(target);
		_isInterpolating = false;
	}
}

void LokomotivMotor::Stop(int stopTime)
{
	InterpolateSpeed(_speed, kSpeedStop, stopTime);
}

void LokomotivMotor::GlideToSpeed(int startTime)
{
	InterpolateSpeed(_speed, _endSpeed, startTime);
}

bool LokomotivMotor::_hasReachedEndSpeed()
{
	bool result = false;
	if(_beginSpeed >= _endSpeed)
	{
		if(_speed <= _endSpeed)
			result = true;
	} else {
		if(_speed >= _endSpeed)
			result = true;
	}
	return result;
}


void LokomotivMotor::ResetPositionCounter()
{

}
