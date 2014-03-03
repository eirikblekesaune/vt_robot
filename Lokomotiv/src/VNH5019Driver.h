#ifndef VNH5019DRIVER_H
#define VNH5019DRIVER_H
#include "MotorDriver.h"
//The main task for the driver is to set the proper pins
//It clip values to within ranges, but it doesn't take the motors state into account.
//The latter is the sole responsibility of the Motor class itself
class VNH5019Driver: public MotorDriver
{
  public:  
    VNH5019Driver(unsigned char INA, unsigned char INB, unsigned char ENDIAG, unsigned char PWM);
    //enum BRAKE { BRAKE_NONE = 0, BRAKE_FULL = 400 };
    //enum SPEED { SPEED_STOP = 0, SPEED_MIN = 0, SPEED_MAX = 400};
    const static speed_t BRAKE_NONE;
    const static speed_t BRAKE_FULL;
    const static speed_t SPEED_STOP;
    const static speed_t SPEED_MIN;
    const static speed_t SPEED_MAX;
    unsigned char GetFault();
    void init();
    void SetSpeed(speed_t speed);
    void SetDirection(int direction);
    void SetBrake(speed_t brake);
    void UpdateDirection();
  private:
    unsigned char _INA;
    unsigned char _INB;
    unsigned char _PWM;
    unsigned char _ENDIAG;
};
#endif
