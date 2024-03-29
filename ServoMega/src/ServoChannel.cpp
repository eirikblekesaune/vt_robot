#include "ServoChannel.h"

ServoChannel::ServoChannel(int8_t pinNumber, int8_t channelNumber) :
		_position(0),
		_duration(1),
		_acceleration(0),
		_interpolationDuration(1),
		_isInterpolating(false),
		_positionInterpolationDelta(0),
		_beginPosition(0),
		_endPosition(0),
		_min(1000),
		_max(2000),
		_channelNumber(channelNumber),
		_pinNumber(pinNumber)
{
		Servo *newServo = new Servo();
		newServo->attach(pinNumber);
		_servo = newServo;
		_lastPositionUpdateTime = 0;
}

void ServoChannel::GoToPosition(int16_t target)
{
	int16_t targetPosition = constrain(target,kMinPosition, kMaxPosition);
	GoToPosition(targetPosition, _duration);
}

void ServoChannel::GoToPosition(int16_t target, int32_t duration)
{
	if(_position != target)
	{
		if(duration <= kUpdateInterval)
			duration = kUpdateInterval + 1;
		_beginPosition = _position;
		_endPosition = target;
		_positionInterpolationDelta = (_endPosition - _beginPosition) / (duration / kUpdateInterval);
		if(_beginPosition >= _endPosition) // if deaccelarating
		{
			if(_positionInterpolationDelta == 0)
				_positionInterpolationDelta = -1;
		} else {
			if(_positionInterpolationDelta == 0)
				_positionInterpolationDelta = 1;
		}
		_isInterpolating = true;
	} else {
		SetPosition(target);
		_isInterpolating = false;
	}
	
}

void ServoChannel::SetPosition(int16_t pos)
{
	int16_t uSecs;
	_position = constrain(pos,kMinPosition, kMaxPosition);
	uSecs = map(_position, kMinPosition, kMaxPosition, _min, _max);
	Serial.write(4);
	Serial.print("p");
	Serial.println(_position);
	_servo->writeMicroseconds(uSecs);
}

void ServoChannel::Update()
{
	if(_isInterpolating)
	{
		if((_lastPositionUpdateTime + kUpdateInterval) < millis())
		{
			if(_hasReachedEndPosition())
			{
				SetPosition(_endPosition);
				_isInterpolating = false;
			} else {
				SetPosition(_position + _positionInterpolationDelta);
			}
			_lastPositionUpdateTime = millis();
		}
	}
}


bool ServoChannel::_hasReachedEndPosition()
{
	bool result = false;
	if(_beginPosition >= _endPosition)
	{
		if(_position <= _endPosition)
			result = true;
	} else {
		if(_position >= _endPosition)
			result = true;
	}
	return result;
}

