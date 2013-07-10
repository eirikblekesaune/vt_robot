#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>

class Motor
{
  public:
    Motor() {};
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
    
    virtual void CheckSensor();
    virtual void UpdatePosition();
    virtual void SetSpeed(speed_t speed);
    virtual speed_t GetSpeed();
    virtual void Stop();
    virtual void SetDirection(direction_t direction);
    virtual direction_t GetDirection();
    virtual void SetTargetPosition(position_t pos);
    virtual position_t GetTargetPosition();
    virtual void SetCurrentPosition(position_t pos);
    virtual position_t GetCurrentPosition();
    virtual void SetBrake(speed_t brake);
    virtual speed_t GetBrake();
    virtual void SetMaxPosition(position_t maxPos);
    virtual position_t GetMaxPosition();  
    virtual void SetMinPosition(position_t minPos);
    virtual position_t GetMinPosition();
  protected:
    speed_t _speed;
    speed_t _brake;
    position_t _targetPosition;
    position_t _minPosition;
    position_t _maxPosition;
    direction_t _direction;
    boolean _blocked;
    virtual void _TargetReached();
    virtual void _StopSensorHit();
    virtual void _CalculateAndSetSpeed();
    virtual void _SetBlocked(boolean block); 
    virtual void _NotifyStateChange(stateChange_t);
};


class PinneMotor: public Motor
{
  public:
    PinneMotor(int stopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver);
  private:
    int _stopButtonPin;
    buttonPosition_t _stopButtonValue;
    void _encoderISR();
    volatile position_t _encoderCounter;
    volatile int _encoderIncrement;
};

class RotationMotor: public Motor
{
  RotationMotor(){};
};



#endif
