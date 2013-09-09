#include "Motor.h"

volatile int encoderCounter1 = 0;
volatile int encoderCounter2 = 0;
volatile int encoderIncrement1 = 1;
volatile int encoderIncrement2 = 1;

//init static variables
const int PinneMotor::DIRECTION_DOWN = 0;
const int PinneMotor::DIRECTION_UP = 1;
const int PinneMotor::TOP_SENSOR_IN = 0;
const int PinneMotor::TOP_SENSOR_OUT = 1;
const int PinneMotor::SLACK_SENSOR_IN = 0;
const int PinneMotor::SLACK_SENSOR_OUT = 1;
const int PinneMotor::POSITION_ALL_UP = 20;
const int PinneMotor::POSITION_DEFAULT_MAX = 8192;
const int PinneMotor::TARGET_NONE = -1;
const int PinneMotor::DIRECTION_DOWN_INCREMENT = 1;
const int PinneMotor::DIRECTION_UP_INCREMENT = -1;

void encoderISR1()
{
  encoderCounter1 = encoderCounter1 + encoderIncrement1;
}

void encoderISR2()
{
  encoderCounter2 = encoderCounter2 + encoderIncrement2;
}

int PinneMotor::GetCurrentPosition()
{ 
  int value;
  uint8_t data[sizeof(int)];
  noInterrupts();
  data[0] = (*_encoderCounter >> 7) & 0x7F;
  data[1] = lowByte(*_encoderCounter) & 0x7F;
  interrupts();
  value = (data[0] << 7) | data[1];
  return value;
};


PinneMotor::PinneMotor(int topStopButtonPin, int slackStopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver, address_t address) :
_currentPosition(POSITION_ALL_UP), 
_targetPosition(TARGET_NONE), 
_minPosition(POSITION_ALL_UP), 
_maxPosition(POSITION_DEFAULT_MAX),
_topStopButtonPin(topStopButtonPin),
_slackStopButtonPin(slackStopButtonPin),
_driver(driver),
_address(address),
_encoderInterruptIndex(encoderInterruptIndex)
{

}

void PinneMotor::init()
{
  switch(_encoderInterruptIndex)
  {
  case 0:
    attachInterrupt(0, encoderISR1, CHANGE);
    _encoderCounter = &encoderCounter1;
    _encoderIncrement = &encoderIncrement1;
    break;
  case 1:
    attachInterrupt(1, encoderISR2, CHANGE);
    _encoderCounter = &encoderCounter2;
    _encoderIncrement = &encoderIncrement2;
    break;
  }
  pinMode(_topStopButtonPin, INPUT);
  pinMode(_slackStopButtonPin, INPUT);
  _driver->init();
  _driver->SetDirection(DIRECTION_DOWN);
  SetDirection(DIRECTION_DOWN);
  Stop();
  _blocked = false;
}

void PinneMotor::Stop()
{
  DEBUG_PRINT("PinneMotor::Stop\n");
  _driver->SetSpeed(VNH5019Driver::SPEED_STOP);
}

void PinneMotor::SetSpeed(int speed)
{
  DEBUG_PRINT("PinneMotor::SetSpeed:"); 
  DEBUG_PRINT(speed);
  DEBUG_PRINT("\n");
  DEBUG_PRINT("blocked: "); 
  DEBUG_PRINT(_blocked); 
  DEBUG_NL;
  if(speed <= 0)
  {
    Stop();
  } 
  else {
    if(!_blocked)
    {
      _driver->SetSpeed(speed);
    }
  }
}

void PinneMotor::SetDirection(int direction)
{
  if(GetDirection() != direction)
  {
    int dir;
    _driver->SetDirection(direction);
    dir = GetDirection();
    DEBUG_PRINT("Direction changed");
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

boolean PinneMotor::IsBlocked()
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
          return false;
        } else {
          return true;
        }
        break;
      case BLOCKED_BY_MAX_POSITION:
        if(direction == DIRECTION_DOWN)
        {
          return false;
        } else {
          return true;
        }
      case BLOCKED_BY_SLACK_SENSOR:
        return false;
        break;
      default:
        //
        return false;
    }    
  } else {
    return true;
  }
}

//Read alle sensor values and check positions
//Stop motor if needed
void PinneMotor::UpdateState()
{
  int currPosition = *_encoderCounter;
  int minPosition = GetMinPosition();
  if(currPosition < minPosition)
  {
    _MinPositionReached();
  } else if(currPosition > GetMaxPosition())
  {
    _MaxPositionReached();
  } else {
//    _topStopButtonValue = digitalRead(_topStopButtonPin);
//    _slackStopButtonValue = digitalRead(_slackStopButtonPin);
    int direction = GetDirection();
    if(direction == DIRECTION_DOWN)
    {
      _GoingUp();
    } else {
      _GoingDown();
    }
  }
}


void PinneMotor::_GoingUp()
{
  if(_state != GOING_UP)
  {
    DEBUG_PRINT("Going up"); DEBUG_NL;
    NotifyStateChange(GOING_UP, _address);
    _state = GOING_UP;
  }
}

void PinneMotor::_GoingDown()
{
  if(_state != GOING_DOWN)
  {
    DEBUG_PRINT("Going down"); DEBUG_NL;
    NotifyStateChange(GOING_DOWN, _address);
    _state = GOING_DOWN;
  }
}

void PinneMotor::_AbsMinPositionReached()
{
  if(_state != BLOCKED_BY_ABS_MIN_POSITION)
  {
    DEBUG_PRINT("Absolute min position reached");DEBUG_NL;
    Stop();
    NotifyStateChange(BLOCKED_BY_ABS_MIN_POSITION, _address);
    _state = BLOCKED_BY_ABS_MIN_POSITION;
  }
}

void PinneMotor::_MinPositionReached()
{
  if(_state != BLOCKED_BY_MIN_POSITION)
  {
    DEBUG_PRINT("Min position reached");DEBUG_NL;
    Stop();
    NotifyStateChange(BLOCKED_BY_MIN_POSITION, _address);
    _state = BLOCKED_BY_MIN_POSITION;
  }
}

void PinneMotor::_MaxPositionReached()
{
  if(_state != BLOCKED_BY_MAX_POSITION)
  {
    DEBUG_PRINT("Max position reached"); DEBUG_NL;
    Stop();
    NotifyStateChange(BLOCKED_BY_MAX_POSITION, _address);
    _state = BLOCKED_BY_MAX_POSITION;
  }
}



void PinneMotor::SetTargetPosition(int targetPosition)
{
  _targetPosition = targetPosition;
}

void PinneMotor::SetCurrentPosition(int currentPosition)
{
  _currentPosition = currentPosition;
}

void PinneMotor::SetBrake(int brake)
{
  _driver->SetBrake(brake);
}

void PinneMotor::SetMinPosition(int minPosition)
{
  _minPosition = minPosition;
}

void PinneMotor::SetMaxPosition(int maxPosition)
{
  _maxPosition = maxPosition;
}



