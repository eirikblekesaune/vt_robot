#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include "MotorDriver.h"
#include "PinneAPI.h"


typedef int position_t;


class PinneMotor
{
  public:
    PinneMotor(int topStopSensorPin, int slackStopSensorPin, int encoderInterruptIndex, VNH5019Driver* driver, address_t address);
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
//    enum Sensor_POSITION { Sensor_IN, Sensor_OUT };
//    enum POSITION { POSITION_ALL_UP = 0, POSITION_DEFAULT_MAX = 65535};
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
    void ReadTopStopSensor();
    void ReadSlackStopSensor();

    volatile int* _encoderCounter;
    volatile int* _encoderIncrement;
  private:
    int _topStopSensorPin;
    int _slackStopSensorPin;
    int _topStopSensorValue;
    int _slackStopSensorValue;
    unsigned long _lastTopSensorReadTime;
    int _lastTopSensorReading;
    unsigned long _lastSlackSensorReadTime;
    int _lastSlackSensorReading;
    
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
    void _TargetReached();
    void _TopStopSensorIn();
    void _TopStopSensorOut();
    void _SlackStopSensorIn();
    void _SlackStopSensorOut();
    void _AbsMinPositionReached();
    void _MinPositionReached();
    void _MaxPositionReached();
    void _CalculateAndSetSpeed();
    void _SetBlocked(boolean block) {};
    
    int _state;
};


class RotationMotor
{
  public:
    RotationMotor(unsigned char rotationPotmeterPin, L293Driver* driver, address_t address);
    const static position_t TARGET_NONE;
    const static int DIRECTION_RIGHT;
    const static int DIRECTION_LEFT;
    const static int POSITION_MAX_RIGHT;
    const static int POSITION_MAX_LEFT;
    const static int SPEED_STOP;    
    void init();

    void Stop() { SetSpeed(SPEED_STOP); };
    
    void SetSpeed(int speed);
    void SetDirection(int direction);
    void SetTargetPosition(int pos);
    void SetCurrentPosition(int pos) {};
    void SetBrake(int brake);
    void SetMaxPosition(int maxPos);
    void SetMinPosition(int minPos);
    
    int GetSpeed() { return static_cast<int>(_driver->GetSpeed()); };
    int GetDirection() { return static_cast<int>(_driver->GetDirection()); };
    int GetTargetPosition() { return static_cast<int>(_targetPosition); };
    int GetCurrentPosition() { return static_cast<int>(_currentPosition); };
    int GetBrake() { return static_cast<int>(_driver->GetBrake()); };
    int GetMaxPosition() { return static_cast<int>(_rightMaxPosition); };
    int GetMinPosition() { return static_cast<int>(_leftMaxPosition); };
    
    boolean IsBlocked();
    void UpdateState();
    
  private:
    MotorDriver* _driver;
    address_t _address;
    boolean _blocked;
    position_t _currentPosition;
    position_t _targetPosition;
    position_t _rightMaxPosition;
    position_t _leftMaxPosition;
    unsigned char _rotationPotmeterPin;
    
};


#endif
