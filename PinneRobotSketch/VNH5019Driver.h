#ifndef VNH5019Driver_h
#define VNH5019Driver_h

#include <Arduino.h>

class VNH5019Driver
{
  public:  
    VNH5019Driver(unsigned char INA, unsigned char INB, unsigned char ENDIAG, 
                  unsigned char CS1, unsigned char PWM);
    void init(); // Initialize TIMER 1, set the PWM to 20kHZ. 
    void SetSpeed(int speed);
    void SetBrake(int brake);
    unsigned int GetCurrentMilliamps(); // Get current reading
    unsigned char GetFault(); // Get fault reading
    
  private:
    unsigned char _INA;
    unsigned char _INB;
    unsigned char _PWM;
    unsigned char _ENDIAG;
    unsigned char _CS;    
};

#endif
