#include "Motor.h"

volatile int encoderCounter1 = 0;
volatile int encoderCounter2 = 0;
volatile int encoderIncrement1 = 1;
volatile int encoderIncrement2 = 1;

void encoderISR1()
{
  encoderCounter1 = encoderCounter1 + encoderIncrement1;
}

void encoderISR2()
{
  encoderCounter2 = encoderCounter2 + encoderIncrement2;
}

PinneMotor::PinneMotor(int topStopButtonPin, int slackStopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver, address_t address) :
_currentPosition(POSITION_ALL_UP), 
_targetPosition(TARGET_NONE), 
_minPosition(POSITION_ALL_UP), 
_maxPosition(POSITION_DEFAULT_MAX),
_topStopButtonPin(topStopButtonPin),
_slackStopButtonPin(slackStopButtonPin),
_driver(driver),
_address(address),
_encoderInterruptIndex(_encoderInterruptIndex)
{

}

void PinneMotor::init()
{
  switch(_encoderInterruptIndex)
  {
  case 0:
    attachInterrupt(_encoderInterruptIndex, encoderISR1, CHANGE);
    _encoderCounter = &encoderCounter1;
    _encoderIncrement = &encoderIncrement1;
    break;
  case 1:
    attachInterrupt(_encoderInterruptIndex, encoderISR2, CHANGE);
    _encoderCounter = &encoderCounter2;
    _encoderIncrement = &encoderIncrement2;
    break;
  }
  pinMode(_topStopButtonPin, INPUT);
  pinMode(_slackStopButtonPin, INPUT);
  _topStopButtonValue = digitalRead(_topStopButtonPin);
  _slackStopButtonValue = digitalRead(_slackStopButtonPin);
  _driver->init();
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
    direction_t dir;
    _driver->SetDirection(direction);
    dir = GetDirection();
    DEBUG_PRINT("Direction changed");
    if(dir == DIRECTION_DOWN)
    {
      NotifyStateChange(GOING_DOWN, _address);
    } else if(dir == DIRECTION_UP) {
      NotifyStateChange(GOING_UP, _address);
    }
  } 
  else {
    _driver->UpdateDirection();
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


