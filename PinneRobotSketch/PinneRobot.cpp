#include "PinneRobot.h"

DualVNH5019MotorShield md(
  M1_INA, M1_INB, M1_CS,
  M2_INA, M2_INB, M2_CS
);


volatile long posM1 = 0;
volatile long posM2 = 0;

//Encoder reading ISR code based on:
//http://makeatronics.blogspot.no/2013/02/efficiently-reading-quadrature-with.html
void encoder1_ISR()
{
  static int8_t lookupTable[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t encVal = 0;
  encVal = encVal << 2;
  encVal = encVal | ((PORTD & B01000000) >> 6) | ((PORTD & B00000100) >> 2);
  posM1 = posM1 + lookupTable[encVal & B1111];
}

void encoder2_ISR()
{
  static int8_t lookupTable[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t encVal = 0;
  encVal = encVal << 2;
  encVal = encVal | ((PORTD & B10000000) >> 7) | ((PORTD & B00001000) >> 3);
  posM2 = posM2 + lookupTable[encVal & B1111];
}

void PinneRobot::init()
{
  attachInterrupt(0, encoder1_ISR, CHANGE);
  attachInterrupt(0, encoder2_ISR, CHANGE);
}
