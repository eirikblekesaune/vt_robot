#ifndef MotorDriver_h
#define MotorDriver_h

#include <Arduino.h>
#include "PinneAPI.h"

typedef int speed_t;
typedef int brake_t;

class MotorDriver
{
  public:  
    MotorDriver() : _speed(SPEED_STOP), _direction(0), _brake(BRAKE_NONE) {};
    
    enum BRAKE { BRAKE_NONE = 0, BRAKE_FULL = 400 };
    enum SPEED { SPEED_STOP = 0, SPEED_MIN = 0, SPEED_MAX = 400};
    
    virtual void init() = 0;// Set pin directions etc.
    
    virtual void SetSpeed(speed_t speed) = 0;
    virtual void SetDirection(int direction) = 0;
    virtual void SetBrake(speed_t brake) = 0;
    
    virtual int GetSpeed() { return _speed; };
    virtual int GetDirection() { return _direction; };
    virtual int GetBrake() { return _brake; };
  protected:
    int _speed;
    int _direction;
    int _brake;
};

class L293Driver: public MotorDriver
{
   L293Driver(){};
};


//The main task for the driver is to set the proper pins
//It clip values to within ranges, but it doesn't take the motors state into account.
//The latter is the sole responsibility of the Motor class itself
class VNH5019Driver: public MotorDriver
{
  public:  
    VNH5019Driver(unsigned char INA, unsigned char INB, unsigned char ENDIAG, 
                  unsigned char CS1, unsigned char PWM);
    unsigned int GetCurrentMilliamps();
    unsigned char GetFault();
    void init();
    void SetSpeed(int speed);
    void SetDirection(int direction);
    void SetBrake(int brake);
  private:
    unsigned char _INA;
    unsigned char _INB;
    unsigned char _PWM;
    unsigned char _ENDIAG;
    unsigned char _CS;
    
    void _UpdateDirection();
};
#endif
