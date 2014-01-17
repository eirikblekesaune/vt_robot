#include "Lokomotiv.h"
//motor pins
const int driverPWM = 10;
const int driverINA = 11;
const int driverINB = 12;
const int driverENDIAG = 6;
const int motorEncoderInterruptIndex = 0;// digital pin 2 on Leonardo implicitly

const long Lokomotiv::beaconAddressUpdateInterval_ = 1000;
	
//IR reader
const int irReaderReceivePin = 8;

//LED dimmming
//const int ledPin = 5;

Lokomotiv::Lokomotiv() :
	lastBeaconAddress_(0),
	lastBeaconAddressUpdate_(0)
{
	//VNH5019Driver *driver = new VNH5019Driver(driverPWM, driverINA, driverINB, driverENDIAG);
	//motor_ = new LokomotivMotor(driver, motorEncoderInterruptIndex);
	irReader_ = new IRReader(irReaderReceivePin, this); 
}

void Lokomotiv::Init()
{
	irReader_->Init();
}

void Lokomotiv::Update()
{
	//motor_->Update();
	irReader_->Update();
}

void Lokomotiv::GotAddr(unsigned char addr)
{
	//We don't need to update repeating beacon addres more than once a
	//second.
	if(addr != lastBeaconAddress_)
	{
		Serial.print("New address:"); Serial.println(addr, HEX);
		lastBeaconAddress_ = addr;
		lastBeaconAddressUpdate_ = millis();
	}

	if((lastBeaconAddressUpdate_ + beaconAddressUpdateInterval_) < millis())
	{
		Serial.print("Addr update:"); Serial.println(lastBeaconAddress_, HEX);
		lastBeaconAddressUpdate_ = millis();
	}
	distanceFromLastBeacon_ = 0;
}
