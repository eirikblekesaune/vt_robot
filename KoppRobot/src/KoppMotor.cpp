#include "Motor.h"

volatile int encoderCounter1 = 0;
volatile int encoderCounter2 = 0;
volatile int encoderIncrement1 = 1;
volatile int encoderIncrement2 = 1;

//init static variables
const int KoppMotor::DIRECTION_DOWN = 0;
const int KoppMotor::DIRECTION_UP = 1;
const int KoppMotor::TOP_SENSOR_IN = 0;
const int KoppMotor::TOP_SENSOR_OUT = 1;
const int KoppMotor::SLACK_SENSOR_IN = 0;
const int KoppMotor::SLACK_SENSOR_OUT = 1;
const int KoppMotor::POSITION_ALL_UP = 1;
const int KoppMotor::POSITION_DEFAULT_MAX = 4096;
const int KoppMotor::TARGET_NONE = 0;
const int KoppMotor::DIRECTION_DOWN_INCREMENT = 1;
const int KoppMotor::DIRECTION_UP_INCREMENT = -1;

//variables for Sensor debouncing
const unsigned long topSensorDebounceDelay = 20;
const unsigned long slackSensorDebounceDelay = 550;

void encoderISR1()
{
	encoderCounter1 = max(encoderCounter1 + encoderIncrement1, 0);
}

void encoderISR2()
{
	encoderCounter2 = max(encoderCounter2 + encoderIncrement2, 0);
}

int KoppMotor::GetCurrentPosition()
{
	int value;
	value = *_encoderCounter;
	return value;
};


KoppMotor::KoppMotor(int topStopSensorPin, int slackStopSensorPin, int encoderInterruptIndex, VNH5019Driver* driver) :
_currentPosition(POSITION_ALL_UP),
_targetPosition(TARGET_NONE),
_minPosition(POSITION_ALL_UP),
_maxPosition(POSITION_DEFAULT_MAX),
_topStopSensorPin(topStopSensorPin),
_slackStopSensorPin(slackStopSensorPin),
_driver(driver),
_encoderInterruptIndex(encoderInterruptIndex)
{

}

void KoppMotor::init()
{
	switch(_encoderInterruptIndex)
	{
	case 0:
		attachInterrupt(0, encoderISR1, CHANGE);
		_encoderCounter = &encoderCounter1;
		_encoderIncrement = &encoderIncrement1;
	pinMode(2, INPUT_PULLUP);
		break;
	case 1:
		attachInterrupt(1, encoderISR2, CHANGE);
		_encoderCounter = &encoderCounter2;
		_encoderIncrement = &encoderIncrement2;
	pinMode(3, INPUT_PULLUP);
		break;
	}
	pinMode(_topStopSensorPin, INPUT_PULLUP);
	pinMode(_slackStopSensorPin, INPUT_PULLUP);
	_driver->init();
	_driver->SetDirection(DIRECTION_UP);
	SetDirection(DIRECTION_DOWN);
	Stop();
	_blocked = false;
	_topStopSensorValue = digitalRead(_topStopSensorPin);
	if(_topStopSensorValue == TOP_SENSOR_IN)
		_TopStopSensorIn();
	_slackStopSensorValue = digitalRead(_slackStopSensorPin);
	if(_slackStopSensorValue == SLACK_SENSOR_OUT)
		_SlackStopSensorOut();

	_speedRamper = new SpeedRamping(
		VNH5019Driver::SPEED_MAX * 0.115,
		VNH5019Driver::SPEED_MAX
	);
}

void KoppMotor::Stop()
{
	_driver->SetSpeed(VNH5019Driver::SPEED_STOP);
}

void KoppMotor::SetStop(int value)
{
	_stoppingSpeed = constrain(value, 0, 5000);
	_driver->SetSpeed(VNH5019Driver::SPEED_STOP);
}

void KoppMotor::SetSpeed(int speed)
{
	if(speed <= 0)
	{
		Stop();
	}
	else {
		if(!IsBlocked())
		{
			_driver->SetSpeed(speed);
		}
	}
}

void KoppMotor::SetDirection(int direction)
{
	if(GetDirection() != direction)
	{
		int dir;
		_driver->SetDirection(direction);
		dir = GetDirection();
		if(dir == DIRECTION_DOWN)
		{
			*_encoderIncrement = DIRECTION_DOWN_INCREMENT;
			_GoingDown();
		}
		else if(dir == DIRECTION_UP) {
			*_encoderIncrement = DIRECTION_UP_INCREMENT;
			_GoingUp();
		}
	}
	else {
		_driver->UpdateDirection();
	}
}

boolean KoppMotor::IsBlocked()
{
	if(_state >= BLOCKED_BY_TOP_SENSOR)
	{
		int direction = GetDirection();
		switch(_state)
		{
			case BLOCKED_BY_MIN_POSITION:
			case BLOCKED_BY_TOP_SENSOR:
			case BLOCKED_BY_ABS_MIN_POSITION:
				if(direction == DIRECTION_UP)
				{
					return true;
				} else {
					return false;
				}
				break;
			case BLOCKED_BY_MAX_POSITION:
			case BLOCKED_BY_SLACK_SENSOR:
				if(direction == DIRECTION_DOWN)
				{
					return true;
				} else {
					return false;
				}
			default:
				//
				return false;
		}
	} else {
		return false;
	}
}

//Read all sensor values and check positions
//Stop motor if needed
void KoppMotor::UpdateState()
{
	ReadTopStopSensor();
	ReadSlackStopSensor();
	if((_state == BLOCKED_BY_TOP_SENSOR) || (_state == BLOCKED_BY_SLACK_SENSOR))
	{//sensors have priority over position lilmits

	} else {
		int currPosition = *_encoderCounter;
		int minPosition = GetMinPosition();
		if(currPosition < minPosition)
		{
			_MinPositionReached();
		} else if(currPosition > GetMaxPosition())
		{
			_MaxPositionReached();
		} else {
			int direction = GetDirection();
			if(direction == DIRECTION_DOWN)
			{
				if((_targetPosition != TARGET_NONE) && (currPosition > _targetPosition))
				{
					_TargetReached();
				} else {
					if(_state == GOING_TO_TARGET)
					{
						_UpdateSpeedRamp();
					} else {
						_GoingDown();
					}
				}
			} else {
					if((_targetPosition != TARGET_NONE) && (currPosition < _targetPosition))
					{
						_TargetReached();
					} else {
						if(_state == GOING_TO_TARGET)
						{
							_UpdateSpeedRamp();
						} else {
							_GoingUp();
						}
				 }
			}
		}
	}
}

void KoppMotor::_UpdateSpeedRamp()
{
	if(_speedRamper->Calculate( GetCurrentPosition() ))
	{
		int newSpeed;
		newSpeed = _speedRamper->GetCurrentValue();
		SetSpeed(newSpeed);
	}
}

void KoppMotor::ReadTopStopSensor()
{
	int newReading;
	newReading = digitalRead(_topStopSensorPin);
	if(newReading != _lastTopSensorReading)
	{
		_lastTopSensorReadTime = millis();
	}
	if((millis() - _lastTopSensorReadTime) > topSensorDebounceDelay)
	{
		if(newReading != _topStopSensorValue)
		{
			_topStopSensorValue = newReading;
			if(_topStopSensorValue == TOP_SENSOR_IN)
			{
				_TopStopSensorIn();
			} else if( _topStopSensorValue == TOP_SENSOR_OUT){
				_TopStopSensorOut();
			}
		}
	}
	_lastTopSensorReading = newReading;
}

void KoppMotor::ReadSlackStopSensor()
{
	int newReading;
	newReading = digitalRead(_slackStopSensorPin);
	if(newReading != _lastSlackSensorReading)
	{
		_lastSlackSensorReadTime = millis();
	}
	if((millis() - _lastSlackSensorReadTime) > slackSensorDebounceDelay)
	{
		if(newReading != _slackStopSensorValue)
		{
			_slackStopSensorValue = newReading;
			if(_slackStopSensorValue == SLACK_SENSOR_IN)
			{
				_SlackStopSensorIn();
			} else if( _topStopSensorValue == SLACK_SENSOR_OUT){
				_SlackStopSensorOut();
			}
		}
	}
	_lastSlackSensorReading = newReading;
}

void KoppMotor::_TopStopSensorIn()
{
	Stop();
	_state = BLOCKED_BY_TOP_SENSOR;
	SetCurrentPosition(POSITION_ALL_UP);
	NotifyStateChange(BLOCKED_BY_TOP_SENSOR);
}

void KoppMotor::_TopStopSensorOut()
{
	int direction;
	direction = GetDirection();
	if(direction == DIRECTION_DOWN)
	{
		_GoingDown();
	} else if(direction == DIRECTION_UP) {
		_GoingUp();
	}
}

void KoppMotor::_SlackStopSensorOut()
{
	Stop();
	_state = BLOCKED_BY_SLACK_SENSOR;
	NotifyStateChange(BLOCKED_BY_SLACK_SENSOR);
}

void KoppMotor::_SlackStopSensorIn()
{
 int direction;
	direction = GetDirection();
	if(direction == DIRECTION_DOWN)
	{
		_GoingDown();
	} else if(direction == DIRECTION_UP) {
		_GoingUp();
	}
}


void KoppMotor::_GoingUp()
{
	if(_state != GOING_UP)
	{
		NotifyStateChange(GOING_UP);
		_state = GOING_UP;
	}
}

void KoppMotor::_GoingDown()
{
	if(_state != GOING_DOWN)
	{
		NotifyStateChange(GOING_DOWN);
		_state = GOING_DOWN;
	}
}

void KoppMotor::_AbsMinPositionReached()
{
	if(_state != BLOCKED_BY_ABS_MIN_POSITION)
	{
		Stop();
		NotifyStateChange(BLOCKED_BY_ABS_MIN_POSITION);
		_state = BLOCKED_BY_ABS_MIN_POSITION;
	}
}

void KoppMotor::_MinPositionReached()
{
	if(_state != BLOCKED_BY_MIN_POSITION)
	{
		Stop();
		NotifyStateChange(BLOCKED_BY_MIN_POSITION);
		_state = BLOCKED_BY_MIN_POSITION;
	}
}

void KoppMotor::_MaxPositionReached()
{
	if(_state != BLOCKED_BY_MAX_POSITION)
	{
		Stop();
		NotifyStateChange(BLOCKED_BY_MAX_POSITION);
		_state = BLOCKED_BY_MAX_POSITION;
	}
}

void KoppMotor::_TargetReached()
{
	if(_state != STOPPED_AT_TARGET)
	{
		Stop();
		_targetPosition = TARGET_NONE;
		NotifyStateChange(STOPPED_AT_TARGET);
		_state = STOPPED_AT_TARGET;
	}
}

void KoppMotor::_GoingToTarget()
{
	_state = GOING_TO_TARGET;
	NotifyStateChange(GOING_TO_TARGET);
}

void KoppMotor::SetTargetPosition(int targetPosition)
{
	int value;
	if(targetPosition == TARGET_NONE)
	{
		_targetPosition = TARGET_NONE;
	} else {
		value = constrain(targetPosition, GetMinPosition(), GetMaxPosition());
		_targetPosition = value;
		int currPosition = GetCurrentPosition();

		//change the direction if target in the opposite direction
		if(GetDirection() == DIRECTION_DOWN)
		{
			if(currPosition > _targetPosition)
			{
				SetDirection(DIRECTION_UP);
			}
		} else {
			if(currPosition < _targetPosition)
			{
				SetDirection(DIRECTION_DOWN);
			}
		}
	}
}

void KoppMotor::SetCurrentPosition(int currentPosition)
{
	currentPosition = constrain(currentPosition, GetMinPosition(), GetMaxPosition());
	noInterrupts();
	*_encoderCounter = currentPosition;
	interrupts();
}

void KoppMotor::SetBrake(int brake)
{
	_driver->SetBrake(brake);
}

void KoppMotor::SetMinPosition(int minPosition)
{
	_minPosition = max(minPosition, POSITION_ALL_UP);
}

void KoppMotor::SetMaxPosition(int maxPosition)
{
	_maxPosition = min(maxPosition, POSITION_DEFAULT_MAX);
}

void KoppMotor::GoToParkingPosition()
{
	//Check if already at top and do parking only if the sensor is out
	_topStopSensorValue = digitalRead(_topStopSensorPin);
	if(_topStopSensorValue == TOP_SENSOR_OUT)
	{
		//fake the currentPosition to default max
		SetCurrentPosition(POSITION_DEFAULT_MAX);
		SetDirection(DIRECTION_UP);
		SetSpeed(VNH5019Driver::SPEED_MAX / 4);
	}
}

void KoppMotor::GoToTargetPosition(int value)
{
	if(value > 0)
	{
		if(GetTargetPosition() != TARGET_NONE)
		{
			_speedRamper->Start(GetCurrentPosition(), GetTargetPosition(), value);
			_GoingToTarget();
		}
	} else {
		if(GetDirection() == DIRECTION_DOWN)//cancel goto target when speed is set
		{
			_GoingDown();
		} else {
			_GoingUp();
		}
	}
}

void KoppMotor::SetGoToSpeedRampUp(int value)
{
	_speedRamper->SetRampUp(static_cast<float>(value));
}

void KoppMotor::SetGoToSpeedRampDown(int value)
{
	_speedRamper->SetRampDown(static_cast<float>(value));
}

void KoppMotor::SetGoToSpeedScaling(int value)
{
	_speedRamper->SetSpeedScaling(static_cast<float>(value) / 1000.0);
}
