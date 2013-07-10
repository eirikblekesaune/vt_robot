#ifndef PINNE_ROBOT_H
#define PINNE_ROBOT_H
#include <Arduino.h>
#include "Motor.h"
#include "PinneAPI.h"

class PinneRobot
{
public:
  PinneRobot();
  void init();
  PinneMotor *leftMotor;
  PinneMotor *rightMotor;
};


#endif
