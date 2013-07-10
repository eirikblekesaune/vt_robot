#ifndef MotorDriver_h
#define MotorDriver_h

#include <Arduino.h>

typedef int speed_t;
typedef int brake_t;

class MotorDriver
{
  public:  
    MotorDriver() {};
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

class VNH5019Driver: public MotorDriver
{
  public:  
    VNH5019Driver(unsigned char INA, unsigned char INB, unsigned char ENDIAG, 
                  unsigned char CS1, unsigned char PWM);
    unsigned int GetCurrentMilliamps();
    unsigned char GetFault();
    virtual void init();
    virtual void SetSpeed(int speed);
    virtual void SetDirection(int direction);
    virtual void SetBrake(int brake);
  private:
    unsigned char _INA;
    unsigned char _INB;
    unsigned char _PWM;
    unsigned char _ENDIAG;
    unsigned char _CS;    
};
#endif
