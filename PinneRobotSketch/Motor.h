#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include "MotorDriver.h"

class Motor
{
  public:
    Motor() : {};
    typedef int position_t;
    typedef int buttonPosition_t;
    enum BUTTON_POSITION { BUTTON_IN, BUTTON_OUT };
    enum POSITION { POSITION_ALL_UP = 0, POSITION_DEFAULT_MAX = 1023 };
    enum BRAKE { BRAKE_NONE = 0, BRAKE_FULL = 400 };
    enum SPEED { SPEED_STOP = 0, SPEED_MIN = 0, SPEED_MAX = 400};
    const static position_t TARGET_NONE = -1;
    
    virtual void CheckSensor();
    virtual void UpdatePosition();
    virtual void SetSpeed(speed_t speed);
    virtual speed_t GetSpeed() { return _driver->GetSpeed(); };
    virtual void Stop();
    virtual void SetDirection(int direction);
    virtual int GetDirection() { return _driver->GetDirection(); };
    virtual void SetTargetPosition(position_t pos);
    virtual position_t GetTargetPosition() { return _targetPosition; };
    virtual void SetCurrentPosition(position_t pos);
    virtual position_t GetCurrentPosition() { return _currentPosition; };
    virtual void SetBrake(speed_t brake);
    virtual speed_t GetBrake() { return _driver->GetBrake(); };
    virtual void SetMaxPosition(position_t maxPos);
    virtual position_t GetMaxPosition();  
    virtual void SetMinPosition(position_t minPos);
    virtual position_t GetMinPosition();
  protected:
    MotorDriver* _driver;
    position_t _currentPosition;
    position_t _targetPosition;
    position_t _minPosition;
    position_t _maxPosition;
    boolean _blocked;
    virtual void _TargetReached();
    virtual void _StopSensorHit();
    virtual void _CalculateAndSetSpeed();
    virtual void _SetBlocked(boolean block);
};


class PinneMotor: public Motor
{
  public:
    PinneMotor(int stopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver);
    enum DIRECTION { DIRECTION_DOWN, DIRECTION_UP };
    
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
  enum DIRECTION { DIRECTION_RIGHT, DIRECTION_LEFT };
};



#endif
