#ifndef PINNE_ROBOT_H
#define PINNE_ROBOT_H
#include <Arduino.h>
#include "Motor.h"
#include "PinneAPI.h"
typedef byte address_t;

class PinneRobot
{
public:
  PinneRobot();
  void init();
  PinneMotor *leftMotor;
  PinneMotor *rightMotor;
private:
  enum stateChange_t
    {
      STOPPED,//Stopped manually
      GOING_UP,//direction set to up
      GOING_DOWN,//directiom set to down
      STOPPED_AT_TARGET,//
      BLOCKED_BY_SENSOR,//The stop sensor was hit
      BLOCKED_BY_MIN_POSITION,//Position counter is below range
      BLOCKED_BY_MAX_POSITION,//Position counter is above range
      DRIVER_FAULT//Something is wrong with the driver itself
    };
  void _NotifyStateChange(stateChange_t stateChange, address_t address);
};


#endif
