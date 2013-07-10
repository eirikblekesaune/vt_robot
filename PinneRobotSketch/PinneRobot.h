#ifndef PINNE_ROBOT_H
#define PINNE_ROBOT_H
#include <Arduino.h>
#include "PinneMotor.h"
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




typedef struct _motorData
{
  speed_t speed;
  speed_t brake;
  direction_t direction;
  position_t target;
} RotationMotor;

class PinneRobot
{
public:
  PinneRobot() {};
  void init();
  
  void checkSensors();
  void updatePositions();
  
  void setLeftMotorSpeed(speed_t speed);
  speed_t getLeftMotorSpeed();
  void leftMotorStop();
  void setLeftMotorDirection(direction_t direction);
  direction_t getLeftMotorDirection();
  void setLeftMotorTargetPosition(position_t pos);
  position_t getLeftMotorTargetPosition();
  void setLeftMotorCurrentPosition(position_t pos);
  position_t getLeftMotorCurrentPosition();
  void setLeftMotorBrake(speed_t brake);
  speed_t getLeftMotorBrake();
  void setLeftMotorMaxPosition(position_t maxPos);
  position_t getLeftMotorMaxPosition();  
  void setLeftMotorMinPosition(position_t minPos);
  position_t getLeftMotorMinPosition();
  
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
  PinneMotor _leftMotor;
  PinneMotor _rightMotor;
  RotationMotor _rotMotor;
  
  void _initLeftMotor();
  void _leftMotorTargetReached();
  void _leftMotorStopSensorHit();
  void _leftMotorCalculateAndSetSpeed();
  void _leftMotorSetBlocked(boolean block);
  
  void _rightMotorTargetReached();
  void _rotationMotorTargetReached();
  
  void _notifyStateChange(stateChange_t);
    
};


#endif
