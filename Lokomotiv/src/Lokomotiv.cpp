#include "Lokomotiv.h"
//motor pins
//const int driverPWM = 10;//The pins for VNHDriver are hardcoed in the LokomotivMotor class
//const int driverINA = 12;
//const int driverINB = 11;
const int driverENDIAG = 6;
const int motorEncoderInterruptIndex = 0;// digital pin 2 on Leonardo implicitly

const long Lokomotiv::_beaconAddressUpdateInterval = 1000;
	
//IR reader
const int irReaderReceivePin = 8;

//Led dimmming
//const int ledPin = 5;

Lokomotiv::Lokomotiv() :
	_lastDetectedAddressUpdate(0),
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
	_speedometer = new LokomotivSpeedometer();
	_motor = new LokomotivMotor(_speedometer);
	_irReader = new IRReader(irReaderReceivePin, this);
}

//Getters
long Lokomotiv::GetSpeed(){return static_cast<long>(_motor->GetSpeed());}
long Lokomotiv::GetDirection(){return static_cast<long>(_motor->GetDirection());}
long Lokomotiv::GetTargetPosition(){return _targetPosition;}

long Lokomotiv::GetDistanceFromLastAddress(){
	return _encoderCounterAtLastAddress - _speedometer->GetCurrentTicks();
}

long Lokomotiv::GetPeripheral(long data){return 0;}
long Lokomotiv::GetState(){return _state;}
long Lokomotiv::GetMeasuredSpeed(){return _speedometer->GetMeasuredSpeed();};
long Lokomotiv::GetLastDetectedAddress(){return _lastDetectedAddress;}
double Lokomotiv::GetPidPValue(){return _pidPValue;}
double Lokomotiv::GetPidIValue(){return _pidIValue;}
double Lokomotiv::GetPidDValue(){return _pidDValue;}
//Setters

void Lokomotiv::SetTargetPosition(long val){_targetPosition = val;}
void Lokomotiv::SetDistanceFromLastAddress(long val){
	_distanceFromLastAddress = val;
}

void Lokomotiv::SetPeripheral(long data)
{
	uint8_t device, command;
	uint16_t value;
	device = static_cast<uint8_t>(data >> 24);
	command = static_cast<uint8_t>((data >> 16) & 0x00FF);
	value = static_cast<uint16_t>(data & 0x00FF);
	Wire.beginTransmission(device);
	Wire.write(command);
	Wire.write(value);
	Wire.endTransmission();
}
void Lokomotiv::SetState(long val){_state = val;}
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
	_motor->Update();
	_irReader->Update();
}

void Lokomotiv::GotAddr(unsigned char addr)
{
	//We don't need to update repeating beacon addres more than once a
	//second.
	//DebugPrint("Got Address");
	//DebugPrint(addr);
	long newAddress = static_cast<long>(addr);
	
	if(newAddress != _lastDetectedAddress)
	{
		_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
		_lastDetectedAddress = newAddress;
		//Serial.print("New address:"); Serial.println(addr);
		ReturnGetValue(CMD_LAST_DETECTED_ADDRESS, GetLastDetectedAddress());
		_lastDetectedAddress = addr;
		_lastDetectedAddressUpdate = millis();
	}

	if((_lastDetectedAddressUpdate + _beaconAddressUpdateInterval) < millis())
	{
		//Serial.print("Addr update:"); Serial.println(_lastDetectedAddress);
	//	Serial.print("Addr update:"); Serial.println(addr);
		ReturnGetValue(CMD_LAST_DETECTED_ADDRESS, GetLastDetectedAddress());
		_lastDetectedAddressUpdate = millis();
	}
	_distanceFromLastBeacon = 0;
}
