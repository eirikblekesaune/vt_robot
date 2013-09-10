#include "Motor.h"


const int RotationMotor::DIRECTION_UP = 0;//turning left
const int RotationMotor::DIRECTION_DOWN = 1;//turning right
const int RotationMotor::DIRECTION_LEFT = RotationMotor::DIRECTION_UP;
const int RotationMotor::DIRECTION_RIGHT = RotationMotor::DIRECTION_DOWN;
const int RotationMotor::POSITION_MIN = 200;//max left
const int RotationMotor::POSITION_MAX = 800;//max right
const int RotationMotor::POSITION_LEFT_LIMIT = RotationMotor::POSITION_MIN;//max left
const int RotationMotor::POSITION_RIGHT_LIMIT = RotationMotor::POSITION_MAX;//max right
const int RotationMotor::TURNING_LEFT = GOING_DOWN;
const int RotationMotor::TURNING_RIGHT = GOING_UP;

const int RotationMotor::SPEED_STOP = 0;
const int RotationMotor::TARGET_NONE = 0;


RotationMotor::RotationMotor(unsigned char rotationPotmeterPin, L293Driver* driver, address_t address) :
  _driver(driver),
  _rotationPotmeterPin(rotationPotmeterPin),
  _address(address),
  _minPosition(POSITION_LEFT_LIMIT),
  _maxPosition(POSITION_RIGHT_LIMIT),
  _targetPosition(TARGET_NONE)
{}

void RotationMotor::init()
{
  _driver->init();
  Stop();
  _driver->SetDirection(DIRECTION_LEFT);
  pinMode(_rotationPotmeterPin, INPUT);
  _currentPosition = analogRead(_rotationPotmeterPin);
}

void RotationMotor::SetSpeed(int speed)
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

void RotationMotor::SetDirection(int direction)
{
  if(GetDirection() != direction)
  {
    _driver->SetDirection(direction);
    if(GetDirection() == DIRECTION_LEFT)
    {
      NotifyStateChange((stateChange_t)TURNING_LEFT, _address);
    } else {
      NotifyStateChange((stateChange_t)TURNING_RIGHT, _address);
    }
  } else {
    _driver->UpdateDirection();
  }
}

void RotationMotor::SetBrake(int brake)
{
  _driver->SetBrake(brake);
}

void RotationMotor::SetMinPosition(int minPosition)
{
  _minPosition = max(minPosition, POSITION_LEFT_LIMIT);
}

void RotationMotor::SetMaxPosition(int maxPosition)
{
  _maxPosition = max(maxPosition, POSITION_RIGHT_LIMIT);
}

//Read potmeter and stop if at target or outside boundary positions

boolean RotationMotor::IsBlocked()
{
  int direction = GetDirection();
  switch(_state)
  {
    case BLOCKED_BY_MIN_POSITION:  
      if(direction == DIRECTION_DOWN)
      {
        return true;
      } else {
        return false;
      }
      break;
    case BLOCKED_BY_MAX_POSITION:
      if(direction == DIRECTION_UP)
      {
        return true;
      } else {
        return false;
      }
    default:
      //
      return false;
  }
}

//Read all sensor values and check positions
//Stop motor if needed
void RotationMotor::UpdateState()
{
  _currentPosition = analogRead(_rotationPotmeterPin);
  DebugPrint("rotpot");
  DebugPrint(_currentPosition);
  int currPosition = _currentPosition;
  int minPosition = GetMinPosition();
  if(currPosition < minPosition)
  {
    _MinPositionReached();
  } else if(currPosition > GetMaxPosition())
  {
    _MaxPositionReached();
  } else {
//    int direction = GetDirection();
//    if(direction == DIRECTION_DOWN)
//    {
//      if((_targetPosition != TARGET_NONE) && (currPosition > _targetPosition))
//      {
//        _TargetReached();
//      } else {
//        _TurningLeft();
//      }
//    } else {
//        if((_targetPosition != TARGET_NONE) && (currPosition < _targetPosition))
//        {
//          _TargetReached();
//        } else {
//          _TurningRight();
//        }
//    }
  }
}

void RotationMotor::_TurningRight()
{
  if(_state != TURNING_RIGHT)
  {
    NotifyStateChange((stateChange_t)TURNING_RIGHT, _address);
    _state = TURNING_RIGHT;
  }
}

void RotationMotor::_TurningLeft()
{
  if(_state != TURNING_LEFT)
  {
    NotifyStateChange((stateChange_t)TURNING_LEFT, _address);
    _state = TURNING_LEFT;
  }
}

void RotationMotor::_MinPositionReached()
{
  if(_state != BLOCKED_BY_MIN_POSITION)
  {
    Stop();
    NotifyStateChange(BLOCKED_BY_MIN_POSITION, _address);
    _state = BLOCKED_BY_MIN_POSITION;
  }
}

void RotationMotor::_MaxPositionReached()
{
  if(_state != BLOCKED_BY_MAX_POSITION)
  {
    Stop();
    NotifyStateChange(BLOCKED_BY_MAX_POSITION, _address);
    _state = BLOCKED_BY_MAX_POSITION;
  }
}

void RotationMotor::_TargetReached()
{
  if(_state != STOPPED_AT_TARGET)
  {
    Stop();
    _targetPosition = TARGET_NONE;
    NotifyStateChange(STOPPED_AT_TARGET, _address);
    _state = STOPPED_AT_TARGET;
  }
}

void RotationMotor::SetTargetPosition(int targetPosition)
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
    if(GetDirection() == DIRECTION_UP)
    {
      if(currPosition > _targetPosition)
      {
        SetDirection(DIRECTION_DOWN);
      }
    } else {
      if(currPosition < _targetPosition)
      {
        SetDirection(DIRECTION_UP);
      }
    }
  }
}