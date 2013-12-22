#include "KoppRobot.h"

//pin connections
// driver aka motor 1, right driver motor 2
const int driverPWM = 9;
const int driverINA = 11;//Remapped due to interrupt
const int driverINB = 4;
const int motorSlackStopSensor = A5; //todo: cut trace, was current sense pin
const int driverENDIAG = 6;
const int motorEncoderInterruptIndex = 1;// digital pin 3 on Leonardo implicitly
const int motorTopStopSensor = A0;

KoppRobot::KoppRobot()
{
  VNH5019Driver *driver = new VNH5019Driver(driverINA, driverINB, driverENDIAG, driverPWM);
  motor = new KoppMotor(motorTopStopSensor, motorSlackStopSensor, motorEncoderInterruptIndex, driver);
}

void KoppRobot::init()
{
  motor->init();
}

void KoppRobot::update()
{
  motor->UpdateState();
}

void KoppRobot::GoToParkingPosition()
{
  motor->GoToParkingPosition();
}





