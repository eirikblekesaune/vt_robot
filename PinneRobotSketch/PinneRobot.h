#ifndef PINNE_ROBOT_H
#define PINNE_ROBOT_H
#include <Arduino.h>
#include "PinneMotor.h"
#include "RotationMotor.h"

class PinneRobot
{
public:
  PinneRobot(PinneMotor* m1, PinneMotor* m2, RotationMotor* m3) : _leftMotor(m1), _rightMotor(m2), _rotationMotor(m3) {};
  void init();
private:
  PinneMotor *_leftMotor;
  PinneMotor *_rightMotor;
  RotationMotor *_rotationMotor;
};


#endif
