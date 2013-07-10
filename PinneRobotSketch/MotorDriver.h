#ifndef MotorDriver_h
#define MotorDriver_h

#include <Arduino.h>

class MotorDriver
{
  public:  
    Motor(){};
    virtual void init(); // Set pin directions etc.
    virtual void SetSpeed(int speed);
    virtual void SetDirection(int direction);
    virtual void SetBrake(int brake);  
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
    void init(); // Initialize TIMER 1, set the PWM to 20kHZ. 
    void SetSpeed(int speed);
    void SetBrake(int brake);
    unsigned int GetCurrentMilliamps();
    unsigned char GetFault();
    
  private:
    unsigned char _INA;
    unsigned char _INB;
    unsigned char _PWM;
    unsigned char _ENDIAG;
    unsigned char _CS;    
};
#endif
