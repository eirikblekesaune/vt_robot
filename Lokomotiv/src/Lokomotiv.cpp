#include "Lokomotiv.h"
//motor pins
//const int driverPWM = 10;//The pins for VNHDriver are hardcoed in the LokomotivMotor class
//const int driverINA = 12;
//const int driverINB = 11;
const int driverENDIAG = 6;
const int motorEncoderInterruptIndex = 0;// digital pin 2 on Leonardo implicitly
const int twiStartingAddress = 0x08; //all VT TWI modules have the same starting address
const long Lokomotiv::_beaconAddressUpdateInterval = 1000;
long lastTrackingUpdate = 0;
long lastTrackingDistanceUpdateValue = 0;
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
	_pidDValue(0),
	_trackingPollingEnabled(false),
	_trackingPollingInterval(0)
{
	_speedometer = new LokomotivSpeedometer();
	_motor = new LokomotivMotor(_speedometer);
	_irReader = new IRReader(irReaderReceivePin, this);
	_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
}

void Lokomotiv::Init()
{
	_irReader->Init();
	Wire.begin();
}

//Getters
long Lokomotiv::GetSpeed(){return static_cast<long>(_motor->GetSpeed());}
long Lokomotiv::GetDirection(){return static_cast<long>(_motor->GetDirection());}
long Lokomotiv::GetTargetPosition(){return _targetPosition;}

long Lokomotiv::GetDistanceFromLastAddress(){
	return _encoderCounterAtLastAddress - _speedometer->GetCurrentTicks();
}

void Lokomotiv::SetDistanceFromLastAddress(long val){
	_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
}


long Lokomotiv::GetPeripheral(long data){return 0;}
long Lokomotiv::GetState(){return _state;}
double Lokomotiv::GetMeasuredSpeed(){return _speedometer->GetMeasuredSpeed();};
long Lokomotiv::GetLastDetectedAddress(){return _lastDetectedAddress;}
double Lokomotiv::GetPidPValue(){return _pidPValue;}
double Lokomotiv::GetPidIValue(){return _pidIValue;}
double Lokomotiv::GetPidDValue(){return _pidDValue;}
//Setters
void Lokomotiv::SetTrackingPollingInterval(long val)
{
	if(val == 0)
	{
		_trackingPollingEnabled = false;
		_trackingPollingInterval = 0;
	} else {
		_trackingPollingEnabled = true;
		_trackingPollingInterval = max(20, val);
		SendTrackingData();
	}
}
long Lokomotiv::GetTrackingPollingInterval()
{
	return _trackingPollingInterval;
}

long Lokomotiv::GetTrackingData()
{
	int32_t result;
	result = static_cast<int32_t>(_speedometer->GetMeasuredSpeed() * 100.0) << 16;
	result = (result & 0xFFFF0000) | (0x0000FFFF & GetDistanceFromLastAddress());
	return result;
}

void Lokomotiv::SetMotorMode(long val)
{
	_motor->SetMotorMode(static_cast<int>(val));
}

void Lokomotiv::SetPidTargetSpeed(long val)
{
	_motor->SetPidTargetSpeed(static_cast<double>(val));
}

void Lokomotiv::SetTargetPosition(long val){_targetPosition = val;}

void Lokomotiv::SetPeripheral(long data)
{
	int8_t device, command;
	int result;
	int8_t valueMSB, valueLSB;
	device = static_cast<int8_t>((data >> 24) & 0x000000FF);
	command = static_cast<int8_t>((data >> 16) & 0x000000FF);
	valueMSB = static_cast<uint8_t>((data >> 8) & 0x000000FF);
	valueLSB = static_cast<uint8_t>(data & 0x000000FF);
	Wire.beginTransmission(device + twiStartingAddress);
	Wire.write(command);
	Wire.write(valueMSB);
	Wire.write(valueLSB);
	result = Wire.endTransmission();
	if(result > 0)
	{
		DebugPrint("TWIerr");
		DebugPrint(result);
	}
}

void Lokomotiv::SetPeripheralRequest(long data)
{
	int8_t device, command;
  int numReceived;
	byte result;
	device = static_cast<int8_t>((data >> 24) & 0x000000FF);
	device += twiStartingAddress;
	command = static_cast<int8_t>((data >> 16) & 0x000000FF);
	Wire.beginTransmission(device);
	Wire.write(command);
	result = Wire.endTransmission();
	//expecting cmd byte and 2 value bytes
	if(result == 0)
	{
		numReceived = Wire.requestFrom(device, 3);
		if(numReceived == 3)
		{
			long reply;
			reply = static_cast<long>(device) << 24;
			reply |= static_cast<long>(Wire.read()) << 16;
			reply |= static_cast<long>(Wire.read()) << 8;
			reply |= static_cast<long>(Wire.read());
			ReturnGetValue(CMD_PERIPHERAL_REQUEST, reply);
		}
	} else {
		DebugPrint("TWIerr");
		DebugPrint(result);
	}
}

void Lokomotiv::SetState(long val){_state = val;}
void Lokomotiv::SetLastDetectedAddress(long val){_lastDetectedAddress = val;}
void Lokomotiv::SetPidPValue(double val){_motor->SetPidPValue(val);}
void Lokomotiv::SetPidIValue(double val){_motor->SetPidIValue(val);}
void Lokomotiv::SetPidDValue(double val){_motor->SetPidDValue(val);}


void Lokomotiv::Update()
{
	_motor->Update();
	_irReader->Update();
	if(_trackingPollingEnabled)
	{
		long dist = GetDistanceFromLastAddress();
		if(
				((millis() - lastTrackingUpdate) >= _trackingPollingInterval) &&
				(lastTrackingDistanceUpdateValue != dist)
				)
		{
			SendTrackingData();
		}
	}
}

void Lokomotiv::SendTrackingData()
{
	ReturnGetValue(CMD_TRACKING_DATA, GetTrackingData()); 
	lastTrackingDistanceUpdateValue = GetDistanceFromLastAddress(); 
	lastTrackingUpdate = millis();
}

void Lokomotiv::GotAddr(long addr)
{
	//We don't need to update repeating beacon address more than once a
	//second.
	_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
	SetLastDetectedAddress(addr);
	if((_lastDetectedAddressUpdate + _beaconAddressUpdateInterval) < millis())
	{
		ReturnGetValue(CMD_LAST_DETECTED_ADDRESS, GetLastDetectedAddress());
		_lastDetectedAddressUpdate = millis();
	}
}
