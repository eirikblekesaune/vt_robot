#ifndef PINNE_ROBOT_H
#define PINNE_ROBOT_H
#include <Arduino.h>
#include "Motor.h"

class PinneRobot
{
public:
  PinneRobot() {};
  void init();
private:
  PinneMotor *_leftMotor;
  PinneMotor *_rightMotor;
  RotationMotor *_rotationMotor;
  enum stateChange_t
    {
      STOPPED,//
      GOING_UP,
      GOING_DOWN,
      REACHED_TARGET,
      BLOCKED_BY_SENSOR,
      BLOCKED_BY_MIN_POSITION,
      BLOCKED_BY_MAX_POSITION,
      DRIVER_FAULT
    };
  void _NotifyStateChange(stateChange_t);
};


#endif
