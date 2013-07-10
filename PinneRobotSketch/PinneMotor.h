#ifndef PINNE_MOTOR_H
#define PINNE_MOTOR_H
#include <Arduino.h>
#include "VNH5019Driver.h"
//Notification states

class PinneMotor
{
public:
  PinneMotor(int stopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver);
  
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
  
  typedef int position_t;
  typedef int speed_t;
  typedef int direction_t;
  typedef int brake_t;
  typedef int buttonPosition_t;
  enum DIRECTION { DIRECTION_DOWN, DIRECTION_UP };
  enum BUTTON_POSITION { BUTTON_IN, BUTTON_OUT };
  enum POSITION { POSITION_ALL_UP = 0, POSITION_DEFAULT_MAX = 1023 };
  enum BRAKE { BRAKE_NONE = 0, BRAKE_FULL = 400 };
  enum SPEED { SPEED_STOP = 0, SPEED_MIN = 0, SPEED_MAX = 400};
  const static position_t TARGET_NONE = -1;
  
  void CheckSensor();
  void UpdatePosition();
  void SetSpeed(speed_t speed);
  speed_t GetSpeed();
  void Stop();
  void SetDirection(direction_t direction);
  direction_t GetDirection();
  void SetTargetPosition(position_t pos);
  position_t GetTargetPosition();
  void SetCurrentPosition(position_t pos);
  position_t GetCurrentPosition();
  void SetBrake(speed_t brake);
  speed_t GetBrake();
  void SetMaxPosition(position_t maxPos);
  position_t GetMaxPosition();  
  void SetMinPosition(position_t minPos);
  position_t GetMinPosition();
  

private:
  speed_t _speed;
  speed_t _brake;
  position_t _targetPosition;
  position_t _minPosition;
  position_t _maxPosition;
  direction_t _direction;
  buttonPosition_t _stopButtonValue;
  boolean _blocked;
  int _stopButtonPin;
  void _TargetReached();
  void _StopSensorHit();
  void _CalculateAndSetSpeed();
  void _SetBlocked(boolean block); 
  void _NotifyStateChange(stateChange_t);
};


#endif
