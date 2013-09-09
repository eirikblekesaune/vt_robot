#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include "MotorDriver.h"
#include "PinneAPI.h"

class PinneMotor
{
  public:
    PinneMotor(int topStopButtonPin, int slackStopButtonPin, int encoderInterruptIndex, VNH5019Driver* driver, address_t address);
    ~PinneMotor() {};
//    enum DIRECTION { DIRECTION_DOWN, DIRECTION_UP };
    const static int DIRECTION_DOWN;
    const static int DIRECTION_UP;
    const static int TOP_SENSOR_IN;
    const static int TOP_SENSOR_OUT;
    const static int SLACK_SENSOR_IN;
    const static int SLACK_SENSOR_OUT;
    const static int POSITION_ALL_UP;
    const static int POSITION_DEFAULT_MAX;
    const static int DIRECTION_DOWN_INCREMENT;
    const static int DIRECTION_UP_INCREMENT;
//    enum BUTTON_POSITION { BUTTON_IN, BUTTON_OUT };
//    enum POSITION { POSITION_ALL_UP = 0, POSITION_DEFAULT_MAX = 65535};
    typedef int position_t;
    const static position_t TARGET_NONE;
    void init();

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
    int GetCurrentPosition();
    int GetBrake() { return static_cast<int>(_driver->GetBrake()); };
    int GetMaxPosition() { return static_cast<int>(_maxPosition); };
    int GetMinPosition() { return static_cast<int>(_minPosition); };
    
    boolean IsBlocked();
    
    void UpdateState();

    volatile int* _encoderCounter;
    volatile int* _encoderIncrement;
  private:
    int _topStopButtonPin;
    int _slackStopButtonPin;
    int _topStopButtonValue;
    int _slackStopButtonValue;
    position_t _currentPosition;
    position_t _targetPosition;
    position_t _minPosition;
    position_t _maxPosition;
    boolean _blocked;
    MotorDriver* _driver;
    address_t _address;
    int _encoderInterruptIndex;
    void _GoingUp();
    void _GoingDown();
    void _TargetReached() {};
    void _TopStopSensorHit() {};
    void _SlackStopSensorHit() {};
    void _AbsMinPositionReached();
    void _MinPositionReached();
    void _MaxPositionReached();
    void _CalculateAndSetSpeed();
    void _SetBlocked(boolean block) {};
    
    int _state;
};


#endif
