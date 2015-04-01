#include "LokomotivMotor.h"

const int LokomotivMotor::kSpeedMin = 0;
const int LokomotivMotor::kSpeedStop = LokomotivMotor::kSpeedMin;
const int LokomotivMotor::kSpeedMax = 511;
const long LokomotivMotor::kSpeedUpdateInterval = 50;
unsigned long lastSpeedSetTime = 0;
unsigned long millisBeforePIDEnable = 1000;

LokomotivMotor::LokomotivMotor(LokomotivSpeedometer* speedometer, Lokomotiv* lokomotiv) :
	_speedometer(speedometer),
	_lokomotiv(lokomotiv),
	_INA(12),
	_INB(11),
	_PWM(10),
	_endSpeed(50),
	_lastSpeedUpdateTime(0),
	_isInterpolating(false),
	_cruiseControlActive(false)
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

	SetDirection(DIRECTION_FORWARD);
	SetSpeed(0);
	//Since we are doing automatic startup of PID we are using conservative
	//PID parameters here.
	_pid = new PID(&_input, &_output,&_setpoint, 1.3, 0.3, 0.01, DIRECT);
	_pid->SetOutputLimits(
		static_cast<double>(LokomotivMotor::kSpeedMin),
		static_cast<double>(LokomotivMotor::kSpeedMax)
	);
	_pid->SetSampleTime(100);
	SetMotorMode(MANUAL_MODE);
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
				if(_endSpeed == 0) {
					_lokomotiv->StateChanged(STATE_STOPPED);
					SendMsg(COMMAND_BIPOLAR_SPEED, 0);
					if(_motorMode != MANUAL_MODE) {
						_setpoint = 0;
					}
				} else {
					if(_direction == DIRECTION_FORWARD) {
						_lokomotiv->StateChanged(STATE_GOING_FORWARD);
						SendMsg(COMMAND_BIPOLAR_SPEED, GetSpeed());
					} else {
						_lokomotiv->StateChanged(STATE_GOING_BACKWARDS);
						SendMsg(COMMAND_BIPOLAR_SPEED, -GetSpeed());
					}
				}
			} else {
				SetSpeed(_speed + _speedInterpolationDelta);
			}
			_lastSpeedUpdateTime = millis();
		}
	} else {
		_input = abs(_speedometer->GetMeasuredSpeed());
		switch(_motorMode) {
			case MANUAL_MODE:
				break;
			case CRUISE_CONTROL_MODE:
				if(!_cruiseControlActive)
				{
					if((lastSpeedSetTime + millisBeforePIDEnable) <= millis()) {
						//store current speed as target speed
						//no need to activate pid when motor is not moving
						if((_speed != 0) && (_input > 1.5))
						{
							SetPidTargetSpeed(_input);
							_output = _speed;
							_pid->SetMode(AUTOMATIC);
							_pid->Compute();
							_cruiseControlActive = true;
						}
					}
				} else {
					//dynamic pid tuning
					if((abs(_input - _setpoint)) > 10.0)
					{
						_pid->SetTunings(1.3, 5.0, 0.01);
					} else {
						_pid->SetTunings(1.3, 0.3, 0.01);
					}
					if(_pid->Compute())
					{
						SetSpeed(static_cast<long>(_output));
					}
				}
				break;
			case TARGET_SPEED_MODE:
				if(_pid->Compute())
				{
					if(_setpoint > 1.5)
					{
						SetSpeed(static_cast<long>(_output));
					} else {
						SetSpeed(0);
					}
				}
				break;
		}
	}
	/*
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
	} else {
	if(_pid->GetMode())//if mode is automatic
	{
		//dynamic pid tuning
		if((abs(_input - _setpoint)) > 10.0)
		{
		_pid->SetTunings(1.3, 5.0, 0.01);
		} else {
		_pid->SetTunings(1.3, 0.3, 0.01);
		}
		if(_pid->Compute())
		{
		long newSpeed;
		newSpeed = static_cast<long>(_output);
		//DebugPrint(_output);
		SetSpeed(newSpeed);
		}
	} else {
		if((_motorMode == CRUISE_CONTROL_MODE) && ((lastSpeedSetTime + millisBeforePIDEnable) <= millis()))
		{
		//no need to activate pid when motor is not moving
		if((_speed != 0) && (_input > 1.5))
		{
			SetPidTargetSpeed(_input);
			//DebugPrint("PID activated");
			//DebugPrint(_input);
			_output = _speed;
			_pid->SetMode(AUTOMATIC);
			_pid->Compute();
		}
		}
	}
	}
	*/
}

void LokomotivMotor::SetSpeed(speed_t newSpeed)
{
	if(newSpeed > kSpeedMax)
		newSpeed = kSpeedMax;
	if(newSpeed < kSpeedMin)
		newSpeed = kSpeedMin;
	_speed = newSpeed;
	OCR1B = _speed;
	if (_speed == 0)
	{
		digitalWrite(_INA, LOW);	 // Make the motor coast no
		digitalWrite(_INB, LOW);	 // matter which direction it is spinning.
	} else {
		UpdateDirection();
	}
}

void LokomotivMotor::UserChangedSpeed()
{
	lastSpeedSetTime = millis();
	_cruiseControlActive = false;
}

void LokomotivMotor::SetPidPValue(double val)
{
	_pid->SetTunings(val, _pid->GetKi(), _pid->GetKd());
}

void LokomotivMotor::SetPidIValue(double val)
{
	_pid->SetTunings(_pid->GetKp(), val, _pid->GetKd());
}

void LokomotivMotor::SetPidDValue(double val)
{
	_pid->SetTunings(_pid->GetKp(), _pid->GetKi(), val);
}

double LokomotivMotor::GetPidPValue() { return _pid->GetKp(); }
double LokomotivMotor::GetPidIValue() { return _pid->GetKi(); }
double LokomotivMotor::GetPidDValue() { return _pid->GetKd(); }

void LokomotivMotor::SetPidTargetSpeed(double val)
{
	_setpoint = val;
}

void LokomotivMotor::SetMotorMode(long val)
{
	//The internal mode for PID is set here.
	switch(val) {
		case MANUAL_MODE:
			_motorMode = MANUAL_MODE;
			_pid->SetMode(MANUAL);
			break;
		case CRUISE_CONTROL_MODE:
			_motorMode = CRUISE_CONTROL_MODE;
			_pid->SetMode(MANUAL);
			_cruiseControlActive = false;
			break;
		case TARGET_SPEED_MODE:
			_motorMode = TARGET_SPEED_MODE;
			_pid->SetMode(AUTOMATIC);
			break;
	}
}

void LokomotivMotor::SetDirection(int newDirection)
{
	if(newDirection<0)
	newDirection = 0;
	if(newDirection>1)
	newDirection = 1;
	
	_direction = newDirection;
	_speedometer->DirectionChanged(newDirection);
//	if(_direction)
//	_pid->SetControllerDirection(REVERSE);
//	else
//	_pid->SetControllerDirection(DIRECT);
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
	_lokomotiv->StateChanged(STATE_STOPPING);
}

void LokomotivMotor::GlideToSpeed(int startTime)
{
	InterpolateSpeed(_speed, _endSpeed, startTime);
	_lokomotiv->StateChanged(STATE_INTERPOLATING_SPEED);
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
