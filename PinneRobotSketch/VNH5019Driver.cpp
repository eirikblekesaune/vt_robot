#include "VNH5019Driver.h"

VNH5019Driver::VNH5019Driver(
            unsigned char INA, unsigned char INB, 
            unsigned char ENDIAG, unsigned char CS, 
            unsigned char PWM
            ) : _INA(INA), _INB(INB), _ENDIAG(ENDIAG), _CS(CS), _PWM(PWM)
{
  
}

void VNH5019Driver::init()
{

}

void VNH5019Driver::SetSpeed(int speed)
{

}

void VNH5019Driver::SetBrake(int brake)
{

}

unsigned int VNH5019Driver::GetCurrentMilliamps()
{
  return 0;
}

unsigned char VNH5019Driver::GetFault()
{
  return 0;
}
            
    
                  
