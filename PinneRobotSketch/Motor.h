#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include "MotorDriver.h"
#include "PinneAPI.h"

class PinneMotor
{
  public:
    PinneMotor(int stopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver, address_t address);
    ~PinneMotor() {};
    enum DIRECTION { DIRECTION_DOWN, DIRECTION_UP };
    enum BUTTON_POSITION { BUTTON_IN, BUTTON_OUT };
    enum POSITION { POSITION_ALL_UP = 0, POSITION_DEFAULT_MAX = 65535};
    typedef int direction_t;
    typedef int position_t;
    typedef int buttonPosition_t;
    const static position_t TARGET_NONE = -1;
    void init();
    void CheckSensor();
    void UpdatePosition();
    void Stop();
    
    void SetSpeed(speed_t speed);
    void SetDirection(int direction);
    void SetTargetPosition(position_t pos);
    void SetCurrentPosition(position_t pos);
    void SetBrake(speed_t brake);
    void SetMaxPosition(position_t maxPos);
    void SetMinPosition(position_t minPos);
    
    speed_t GetSpeed() { return _driver->GetSpeed(); };
    int GetDirection() { return _driver->GetDirection(); };
    position_t GetTargetPosition() { return _targetPosition; };
    position_t GetCurrentPosition() { return _currentPosition; };
    speed_t GetBrake() { return _driver->GetBrake(); };
    position_t GetMaxPosition() { return _maxPosition; };
    position_t GetMinPosition() { return _minPosition; };

    volatile int* _encoderCounter;
    volatile int* _encoderIncrement;
  private:
    int _stopButtonPin;
    buttonPosition_t _stopButtonValue;
    position_t _currentPosition;
    position_t _targetPosition;
    position_t _minPosition;
    position_t _maxPosition;
    boolean _blocked;
    MotorDriver* _driver;
    address_t _address;
    int _encoderInterruptIndex;
    void _TargetReached() {};
    void _StopSensorHit() {};
    void _CalculateAndSetSpeed();
    void _SetBlocked(boolean block) {};
};


#endif
