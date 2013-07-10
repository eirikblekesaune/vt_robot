#include "Motor.h"

PinneMotor::PinneMotor(int stopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver) :
  _targetPosition(TARGET_NONE),
  _minPosition(POSITION_ALL_UP),
  _maxPosition(POSITION_DEFAULT_MAX),
  _stopButtonPin(stopButtonPin)
{
  pinMode(stopButtonPin, INPUT);
  _stopButtonValue = digitalRead(stopButtonPin);
}
