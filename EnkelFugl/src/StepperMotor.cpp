#include "StepperMotor.h"

const int16_t StepperMotor::DECREASING_POS_DIRECTION = 0;
const int16_t StepperMotor::INCREASING_POS_DIRECTION = 1;

StepperMotor::StepperMotor(StepperDriver *driver) :
	_driver(driver)
{
}

void StepperMotor::Update()
{
	_readSensor();
}

void StepperMotor::SetTargetPosition(int16_t targetPosition)
{
	_targetPosition = clip(targetPostion, _minPosition, _maxPosition);
	//1 is the direction for increasing position values
	_readSensor();
	if(
			(_targetPosition <= _position) && 
			(GetDirection() == StepperMotor::INCREASING_POS_DIRECTION))
	{
		SetDirection(StepperMotor::DECREASING_POS_DIRECTION);
	} else if(
			(_targetPosition > _position) &&
			(GetDirection() == StepperMotor::DECREASING_POS_DIRECTION))
	{
		SetDirection(StepperMotor::INCREASING_POS_DIRECTION);
	}
}

void StepperMotor::_readSensor()
{
	if((millis() - _lastPositionReadTime) >= 50)
	{
		_position = analogRead(_positionSensorPin);
		_lastPositionReadTime = millis();
		//Serial.println(currentPosition);
		if(HasTarget())
		{
			if()
		
		} else {
			if( (_position >= _maxPosition) && 
					(_state != AT_MAX_POSITION) &&
					(_direction == UP_DIRECTION)
					)
			{
				_maxPositionReached();
			} else if( (_position <= _minPosition) && 
					(_liftState != AT_MIN_POSITION) &&
					(_direction == DOWN_DIRECTION)
					)
			{
				_minPositionReached();
			}
		}
	}
}

void StepperMotor::_reachedTargetPosition()
{
	Stop();
	targetPosition = NO_TARGET;
	stateChanged(AT_TARGET);
}



