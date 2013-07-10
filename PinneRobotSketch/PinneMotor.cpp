#include "PinneMotor.h"

PinneMotor::PinneMotor(int stopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver) :
  _speed(SPEED_STOP),
  _brake(BRAKE_FULL),
  _targetPosition(TARGET_NONE),
  _minPosition(POSITION_ALL_UP),
  _maxPosition(POSITION_DEFAULT_MAX),
  _direction(DIRECTION_DOWN),
  _stopButtonPin(stopButtonPin)
{
  pinMode(stopButtonPin, INPUT);
  _stopButtonValue = digitalRead(stopButtonPin);
}
