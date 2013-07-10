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

PinneMotor::PinneMotor(int stopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver) :
  _currentPosition(POSITION_ALL_UP), 
  _targetPosition(TARGET_NONE), 
  _minPosition(POSITION_ALL_UP), 
  _maxPosition(POSITION_DEFAULT_MAX),
  _stopButtonPin(stopButtonPin),
  _driver(driver),
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
  pinMode(_stopButtonPin, INPUT);
  _stopButtonValue = digitalRead(_stopButtonPin);
  _driver->init();
  _blocked = false;
}

void PinneMotor::Stop()
{
  DEBUG_PRINT("PinneMotor::Stop");
  _driver->SetSpeed(MotorDriver::SPEED_STOP);
}

void PinneMotor::SetSpeed(speed_t speed)
{
  DEBUG_PRINT("PinneMotor::SetSpeed"); DEBUG_PRINT(speed);
  DEBUG_PRINT("blocked: "); DEBUG_PRINT(_blocked);
  if(speed <= 0)
  {
    Stop();
  } else {
    if(!_blocked)
    {
      _driver->SetSpeed(speed);
    }
  }
}

