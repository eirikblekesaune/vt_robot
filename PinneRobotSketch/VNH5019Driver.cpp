///Much of this code is based on Pololu's DualVNH5019Motorshield.h code from:
//https://github.com/pololu/Dual-VNH5019-Motor-Shield

#include "MotorDriver.h"

VNH5019Driver::VNH5019Driver(
            unsigned char INA, unsigned char INB, 
            unsigned char ENDIAG, unsigned char CS, 
            unsigned char PWM) 
   : _INA(INA), _INB(INB), _ENDIAG(ENDIAG), _CS(CS), _PWM(PWM)
{
  
}

void VNH5019Driver::init()
{
  pinMode(_INA,OUTPUT);
  pinMode(_INB,OUTPUT);
  pinMode(_PWM,OUTPUT);
  pinMode(_ENDIAG,INPUT);
  pinMode(_CS, INPUT);
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
  TCCR1A = 0b10100000;
  TCCR1B = 0b00010001;
  ICR1 = 400;
  #endif
  SetSpeed(SPEED_STOP);
  SetDirection(0);
}

void VNH5019Driver::SetSpeed(int speed)
{
  if (speed < 0)
    speed = 0;
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;
  Serial.println("VNH5019Driver::SetSpeed");Serial.println(speed);
  _speed = speed;
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  //temp hack..
  if(_PWM == 9)
  {
    OCR1A = speed;
  } else if(_PWM == 10)
  {
    OCR1B = speed;
  }
  #else
  analogWrite(_PWM, speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
  #endif
  if (speed == 0)
  {
    digitalWrite(_INA, LOW);   // Make the motor coast no
    digitalWrite(_INB, LOW);   // matter which direction it is spinning.
  } else {
    _UpdateDirection();
  }
}

void VNH5019Driver::SetDirection(int direction)
{
  if(direction < 0)
    direction = 0;
  if(direction > 1)
    direction = 1;
  _direction = direction;
  _UpdateDirection();
}

void VNH5019Driver::_UpdateDirection()
{
  if (!_direction)
  {
    digitalWrite(_INA, LOW);
    digitalWrite(_INB, HIGH);
  }
  else
  {
    digitalWrite(_INA, HIGH);
    digitalWrite(_INB, LOW);
  }
}

void VNH5019Driver::SetBrake(int brake)
{
  if (brake < 0)
    brake = 0;
  if (brake > 400)  // Max brake
    brake = 400;
  digitalWrite(_INA, LOW);
  digitalWrite(_INB, LOW);
  _brake = brake;
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  //temp hack..
  if(_PWM == 9)
  {
    OCR1A = brake;
  } else if(_PWM == 10)
  {
    OCR1B = brake;
  }
  #else
  analogWrite(_PWM, brake * 51 / 80); // default to using analogWrite, mapping 400 to 255
  #endif
}

unsigned int VNH5019Driver::GetCurrentMilliamps()
{
  return analogRead(_CS) * 34;
}

unsigned char VNH5019Driver::GetFault()
{
  return !digitalRead(_ENDIAG);
}
            
    
                  
