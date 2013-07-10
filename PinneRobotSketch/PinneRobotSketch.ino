#include "VNH5019Driver.h"
#include "PinneMotor.h"
#include "PinneRobot.h"
#include "PinneAPIParser.h"

//pin connections
const int leftDriverPWM = 9;
const int leftDriverINA = 5;
const int leftDriverINB = 4;
const int leftDriverCS = A0;
const int leftDriverENDIAG = 6;
const int leftMotorEncoderInterruptIndex = 0;// digital pin 2 implicitly
const int leftMotorSTOP = A5;

const int rightDriverPWM = 10;
const int rightDriverINA = 12;
const int rightDriverINB = 8;
const int rightDriverCS = A1;
const int rightDriverENDIAG = 7;
const int rightMotorEncoderInterruptIndex = 1;// digital pin 3 implicitly
const int rightMotorSTOP = 13;

const int rotationDriver1A = A3;
const int rotationDriver2A = A4;
const int rotationDriverPWM = 11;

VNH5019Driver leftDriver(leftDriverINA, leftDriverINB, leftDriverENDIAG, leftDriverCS, leftDriverPWM);
VNH5019Driver rightDriver(rightDriverINA, rightDriverINB, rightDriverENDIAG, rightDriverCS, rightDriverPWM);

PinneMotor leftMotor(leftMotorSTOP, leftMotorEncoderInterruptIndex, &leftDriver);
PinneMotor rightMotor(rightMotorSTOP, rightMotorEncoderInterruptIndex, &rightDriver);

PinneRobot robot(&leftMotor, &rightMotor);

void setup()
{
  Serial.begin(9600);
  while(!Serial);
}
void loop()
{
  delay(3000);
  Serial.print("With direct call");
  delay(3000);
  Serial.print("With func poitner");
  
  
}
