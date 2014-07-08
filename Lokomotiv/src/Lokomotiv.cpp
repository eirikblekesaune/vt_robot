#include "Lokomotiv.h"
//motor pins
//const int driverPWM = 10;//The pins for VNHDriver are hardcoed in the LokomotivMotor class
//const int driverINA = 12;
//const int driverINB = 11;
const int driverENDIAG = 6;
const int motorEncoderInterruptIndex = 0;// digital pin 2 on Leonardo implicitly

const long Lokomotiv::_beaconAddressUpdateInterval = 1000;
long lastDistanceUpdate = 0;
long lastDistanceUpdateValue = 0;
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
	_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
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
void Lokomotiv::SetDistancePollingInterval(long val)
{
	if(val == 0)
	{
		_distancePollingEnabled = false;
		_distancePollingInterval = 0;
	} else {
		_distancePollingEnabled = true;
		_distancePollingInterval = max(20, val);
		SendDistanceUpdate();
	}
}
long Lokomotiv::GetDistancePollingInterval()
{
	return _distancePollingInterval;
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
//	int8_t device, command;
//	uint16_t valueMSB, valueLSB;
//	device = static_cast<int8_t>((data >> 24) & 0x000000FF);
//	command = static_cast<int8_t>((data >> 16) & 0x000000FF);
//	valueMSB = static_cast<uint8_t>((data >> 8) & 0x000000FF);
//	valueLSB = static_cast<uint8_t>(data & 0x000000FF);
//	Wire.beginTransmission(device);
//	Wire.write(command);
//	Wire.write(valueMSB);
//	Wire.write(valueLSB);
//	Wire.endTransmission();
}

void Lokomotiv::SetPeripheralRequest(long data)
{
//	int8_t device, command;
//	device = static_cast<int8_t>((data >> 24) & 0x000000FF);
//	command = static_cast<int8_t>((data >> 16) & 0x000000FF);
//	Wire.beginTransmission(device);
//	Wire.write(command);
//	Wire.endTransmission(false);
//	Wire.requestFrom(device, 2, true);
//	while(Wire.available())    // slave may send less than requested
//  { 
//    DebugPrint(Wire.read());         // print the character
//  }
}

void Lokomotiv::SetState(long val){_state = val;}
void Lokomotiv::SetLastDetectedAddress(long val){_lastDetectedAddress = val;}
void Lokomotiv::SetPidPValue(double val){_motor->SetPidPValue(val);}
void Lokomotiv::SetPidIValue(double val){_motor->SetPidIValue(val);}
void Lokomotiv::SetPidDValue(double val){_motor->SetPidDValue(val);}

void Lokomotiv::Init()
{
	_irReader->Init();
	//Wire.begin();
}

void Lokomotiv::Update()
{
	_motor->Update();
	_irReader->Update();
	if(_distancePollingEnabled)
	{
		long dist = GetDistanceFromLastAddress();
		if(
				((millis() - lastDistanceUpdate) >= _distancePollingInterval) &&
				(lastDistanceUpdateValue != dist))
		{
			SendDistanceUpdate(dist);
		}
	}
}

void Lokomotiv::SendDistanceUpdate()
{
	SendDistanceUpdate(GetDistanceFromLastAddress());
}

void Lokomotiv::SendDistanceUpdate(long dist)
{
	ReturnGetValue(CMD_DISTANCE_FROM_LAST_ADDRESS, dist); 
	lastDistanceUpdate = millis();
	lastDistanceUpdateValue = dist; 
}

void Lokomotiv::GotAddr(unsigned char addr)
{
	//We don't need to update repeating beacon address more than once a
	//second.
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
