#ifndef PINNE_ROBOT_H
#define PINNE_ROBOT_H
#include <Arduino.h>
#include "PinneMotor.h"

class PinneRobot
{
public:
  PinneRobot(PinneMotor* m1, PinneMotor* m2) : _leftMotor(m1), _rightMotor(m2) {};
  void init();
private:  
  PinneMotor *_leftMotor;
  PinneMotor *_rightMotor;
};


#endif
