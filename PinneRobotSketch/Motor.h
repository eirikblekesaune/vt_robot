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
    typedef int position_t;
    typedef int buttonPosition_t;
    typedef int direction_t;
    const static position_t TARGET_NONE = -1;
    void init();
    void CheckSensor();
    void UpdatePosition();
    void Stop();
    
    void SetSpeed(int speed);
    void SetDirection(int direction);
    void SetTargetPosition(int pos);
    void SetCurrentPosition(int pos);
    void SetBrake(int brake);
    void SetMaxPosition(int maxPos);
    void SetMinPosition(int minPos);
    
    int GetSpeed() { return static_cast<int>(_driver->GetSpeed()); };
    int GetDirection() { return static_cast<int>(_driver->GetDirection()); };
    int GetTargetPosition() { return static_cast<int>(_targetPosition); };
    int GetCurrentPosition() { return static_cast<int>(_currentPosition); };
    int GetBrake() { return static_cast<int>(_driver->GetBrake()); };
    int GetMaxPosition() { return static_cast<int>(_maxPosition); };
    int GetMinPosition() { return static_cast<int>(_minPosition); };

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
