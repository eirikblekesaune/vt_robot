#include "Lokomotiv.h"
//motor pins
const int driverPWM = 9;
const int driverINA = 11;//Remapped due to interrupt
const int driverINB = 4;
const int driverENDIAG = 6;
const int motorEncoderInterruptIndex = 1;// digital pin 3 on Leonardo implicitly

//IR reader
const int irReaderInterruptIndex = 4; //digital pin 7 on Leonardo implicitly 

//LED dimmming
const int ledPin = 10;



Lokomotiv::Lokomotiv()
{
	VNH5019Driver *driver = new VNH5019Driver(driverPWM, driverINA, driverINB, driverENDIAG);
	motor_ = new LokomotivMotor(driver, motorEncoderInterruptIndex);
}

void Lokomotiv::Update()
{
	motor_->Update();
}
