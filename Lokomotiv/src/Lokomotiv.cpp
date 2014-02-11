#include "Lokomotiv.h"
//motor pins
const int driverPWM = 10;
const int driverINA = 12;
const int driverINB = 11;
const int driverENDIAG = 6;
const int motorEncoderInterruptIndex = 0;// digital pin 2 on Leonardo implicitly

const long Lokomotiv::_beaconAddressUpdateInterval = 1000;
	
//IR reader
const int irReaderReceivePin = 8;

//Led dimmming
//const int ledPin = 5;

Lokomotiv::Lokomotiv() :
	_lastBeaconAddress(0),
	_lastBeaconAddressUpdate(0),
	_speed(0),
	_direction(0),
	_targetPosition(0),
	_distanceFromLastAddress(0),
	_led(0),
	_state(0),
	_measuredSpeed(0),
	_lastDetectedAddress(0),
	_pidPValue(0),
	_pidIValue(0),
	_pidDValue(0)
{
	//VNH5019Driver *driver = new VNH5019Driver(driverPWM, driverINA, driverINB, driverENDIAG);
	//_motor = new LokomotivMotor(driver, motorEncoderInterruptIndex);
	_irReader = new IRReader(irReaderReceivePin, this);
}

//Getters
long Lokomotiv::GetSpeed(){return _speed;}
long Lokomotiv::GetDirection(){return _direction;}
long Lokomotiv::GetTargetPosition(){return _targetPosition;}
long Lokomotiv::GetDistanceFromLastAddress(){return _distanceFromLastAddress;}
long Lokomotiv::GetLed(){return _led;}
long Lokomotiv::GetState(){return _state;}
long Lokomotiv::GetMeasuredSpeed(){return _measuredSpeed;}
long Lokomotiv::GetLastDetectedAddress(){return _lastDetectedAddress;}
double Lokomotiv::GetPidPValue(){return _pidPValue;}
double Lokomotiv::GetPidIValue(){return _pidIValue;}
double Lokomotiv::GetPidDValue(){return _pidDValue;}
//Setters
void Lokomotiv::SetSpeed(long val) {
	int16_t newSpeed = static_cast<int16_t>(val);
	if(newSpeed != _speed)
	{
		_speed = newSpeed;
		Wire.beginTransmission(4);
		Wire.write(0xFF);//speed command
		Wire.write(static_cast<byte>(_speed >> 8) );
		Wire.write(static_cast<byte>(_speed));
		Wire.endTransmission();
	}
}

void Lokomotiv::SetDirection(long val) {
	int16_t newDirection = static_cast<int16_t>(val);
	if(newDirection != _direction)
	{
		_direction = newDirection;
		Wire.beginTransmission(4);
		Wire.write(0x0F);//direction command
		Wire.write(static_cast<byte>(_direction));
		Wire.endTransmission();
	}
}

void Lokomotiv::SetTargetPosition(long val){_targetPosition = val;}
void Lokomotiv::SetDistanceFromLastAddress(long val){_distanceFromLastAddress = val;}
void Lokomotiv::SetLed(long val)
{
	int16_t newLed= static_cast<int16_t>(val);
	if(1)//newLed != _led)
	{
		_led = newLed;
		Wire.beginTransmission(2);
		Wire.write(0x00);
		Wire.write(static_cast<byte>(_led));
		Wire.endTransmission();
	}
}
void Lokomotiv::SetState(long val){_state = val;}
void Lokomotiv::SetMeasuredSpeed(long val){_measuredSpeed = val;}
void Lokomotiv::SetLastDetectedAddress(long val){_lastDetectedAddress = val;}
void Lokomotiv::SetPidPValue(double val){_pidPValue = val;}
void Lokomotiv::SetPidIValue(double val){_pidIValue = val;}
void Lokomotiv::SetPidDValue(double val){_pidDValue = val;}

void Lokomotiv::Init()
{
	_irReader->Init();
	Wire.begin();
}

void Lokomotiv::Update()
{
	//_motor->Update();
	_irReader->Update();
}

void Lokomotiv::GotAddr(unsigned char addr)
{
	//We don't need to update repeating beacon addres more than once a
	//second.
	if(addr != _lastBeaconAddress)
	{
		Serial.print("New address:"); Serial.println(addr, HEX);
		_lastBeaconAddress = addr;
		_lastBeaconAddressUpdate = millis();
	}

	if((_lastBeaconAddressUpdate + _beaconAddressUpdateInterval) < millis())
	{
		Serial.print("Addr update:"); Serial.println(_lastBeaconAddress, HEX);
		_lastBeaconAddressUpdate = millis();
	}
	_distanceFromLastBeacon = 0;
}
