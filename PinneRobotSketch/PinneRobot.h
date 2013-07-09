#ifndef PINNE_ROBOT_H
#define PINNE_ROBOT_H
#include <Arduino.h>
#include "DualVNH5019MotorShieldVTMod.h"
#include "lm293.h"

//Pin connections
const int M1_INA = 5;//2;Changed because of interrupt
const int M1_INB = 4;
const int M1_PWM = 9;
const int M1_CS = A0;
const int M1_HALLA = 2;//5;Changed because of interrupt
const int M1_HALLB = 6;
const int M1_STOP = A5;

const int M2_INA = 12;//7;//Changed because of interrupt
const int M2_INB = 8;
const int M2_PWM = 10;
const int M2_CS = A1;
const int M2_HALLA = 3;//11;//Changed because of interrupt
const int M2_HALLB = 7;//12;//Changed because of interrupt
const int M2_STOP = 13;

const int L293_1A = A3;
const int L293_2A = A4;
const int L293_EN = 11;//3;Changed because of interrupt

const int ROT_POT = A2;

typedef unsigned int position_t;
typedef int speed_t;
typedef enum _direction_t { DIRECTION_UP = 1, DIRECTION_DOWN = -1 } direction_t;

typedef struct _pinneMotor
{
  speed_t speed;
  direction_t direction;
  position_t target;
} PinneMotor;

typedef struct _motorData
{
  speed_t speed;
  direction_t direction;
  position_t target;
} MotorData;

class PinneRobot
{
public:
  PinneRobot();
  void init();
  
  void setLeftMotorSpeed(speed_t speed);
  speed_t getLeftMotorSpeed();
  void setLeftMotorDirection(direction_t direction);
  direction_t getLeftMotorDirection();
  void setLeftMotorTargetPosition(position_t pos);
  position_t getLeftMotorTargetPosition();
  void setLeftMotorCurrentPosition(int pos);
  position_t getLeftMotorCurrentPosition();
  
  void setRightMotorSpeed(speed_t speed);
  speed_t getRightMotorSpeed();
  void setRightMotorDirection(direction_t direction);
  direction_t getRightMotorDirection();
  void setRightMotorTargetPosition(position_t pos);
  position_t getRightMotorTargetPosition();
  void setRightMotorCurrentPosition(int pos);
  position_t getRightMotorCurrentPosition();
  
  void setRotationMotorSpeed(speed_t speed);
  speed_t getRotationMotorSpeed();
  void setRotationMotorDirection(direction_t direction);
  direction_t getRotationMotorDirection();
  void setRotationMotorTargetPosition(position_t pos);
  position_t getRotationMotorTargetPosition();
  void setRotationMotorCurrentPosition(int pos);
  position_t getRotationMotorCurrentPosition();
  
private:  
  MotorData motor1;
  MotorData motor2;
  MotorData rotMotor;
};


#endif
