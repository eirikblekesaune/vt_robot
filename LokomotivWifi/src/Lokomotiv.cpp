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
#define TARGET_POSITION_NONE 0
#define TARGET_POSITION_ALL -1
//Led dimmming
//const int ledPin = 5;

Lokomotiv::Lokomotiv() :
	_lastDetectedAddressUpdate(0),
	_targetPosition(0),
	_distanceFromLastAddress(0),
	_led(0),
	_state(STATE_STOPPED),
	_measuredSpeed(0),
	_lastDetectedAddress(0),
	_pidPValue(0),
	_pidIValue(0),
	_pidDValue(0),
	_trackingPollingEnabled(false),
	_trackingPollingInterval(0)
{
	_speedometer = new LokomotivSpeedometer();
	_motor = new LokomotivMotor(_speedometer, this);
	_irReader = new IRReader(irReaderReceivePin, this);
	_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
}

void Lokomotiv::Init()
{
	_irReader->Init();
	Wire.begin();
}

//Getters
long Lokomotiv::GetBipolarSpeed() {
	long rawSpeed = static_cast<long>(_motor->GetSpeed());
	if(_motor->GetDirection() == DIRECTION_BACKWARDS) {
		rawSpeed = -rawSpeed;
	}
	return rawSpeed;
}

long Lokomotiv::GetTargetPosition(){return _targetPosition;}

long Lokomotiv::GetDistanceFromLastAddress(){
	return _encoderCounterAtLastAddress - _speedometer->GetCurrentTicks();
}

void Lokomotiv::SetDistanceFromLastAddress(long val){
	_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
}


long Lokomotiv::GetPeripheral(long data){return 0;}
stateChange_t Lokomotiv::GetState(){return _state;}
double Lokomotiv::GetMeasuredSpeed(){return _speedometer->GetMeasuredSpeed();};
long Lokomotiv::GetLastDetectedAddress(){return _lastDetectedAddress;}
double Lokomotiv::GetPidPValue(){return _motor->GetPidPValue();}
double Lokomotiv::GetPidIValue(){return _motor->GetPidIValue();}
double Lokomotiv::GetPidDValue(){return _motor->GetPidDValue();}
//Setters
void Lokomotiv::SetTrackingPollingInterval(long val)
{
	_trackingPollingInterval = max(1, val);
}

long Lokomotiv::GetTrackingPollingInterval()
{
	return _trackingPollingInterval;
}

void Lokomotiv::StateChanged(stateChange_t newState) {
	_state = newState;
	SendMsg(COMMAND_STATE, static_cast<long>(_state));
	if((_state == STATE_STOPPED) && _trackingPollingEnabled) {
		SendTrackingData(GetDistanceFromLastAddress());
	}
}

void Lokomotiv::SetTrackingState(long val) {
	if(val) {
		_trackingPollingEnabled = true;
		SendTrackingData(GetDistanceFromLastAddress());
	} else {
		_trackingPollingEnabled = false;
	}
}

long Lokomotiv::GetTrackingState() {
	if(_trackingPollingEnabled) {
		return 1;
	} else {
		return 0;
	}
}

void Lokomotiv::SetBipolarSpeed(long val) {
	if(val == 0) {
		_motor->SetSpeed(0);
		StateChanged(STATE_STOPPED);
	} else if(val < 0) {
		_motor->SetSpeed(abs(val));
		if((_motor->GetDirection() == DIRECTION_FORWARD) || (_state == STATE_STOPPED)) {
			_motor->SetDirection(DIRECTION_BACKWARDS);
			StateChanged(STATE_GOING_BACKWARDS);
		}
	} else {
		_motor->SetSpeed(val);
		if((_motor->GetDirection() == DIRECTION_BACKWARDS) || (_state == STATE_STOPPED)) {
			_motor->SetDirection(DIRECTION_FORWARD);
			StateChanged(STATE_GOING_FORWARD);
		}
	}
}

void Lokomotiv::SetMotorMode(long val)
{
	_motor->SetMotorMode(val);
}

void Lokomotiv::SetPidTargetSpeed(double val)
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
		SendMsg(COMMAND_PERIPHERAL_REQUEST, reply);
	}
	} else {
	DebugPrint("TWIerr");
	DebugPrint(result);
	}
}

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
		SendTrackingData(dist);
	}
	}
}

void Lokomotiv::SendTrackingData(int32_t dist)
{
	SendTrackingDataMsg(dist, _speedometer->GetMeasuredSpeed());
	lastTrackingDistanceUpdateValue = dist;
	lastTrackingUpdate = millis();
}

void Lokomotiv::GotAddr(long addr)
{
	//We don't need to update repeating beacon address more than once a
	//second.
	_encoderCounterAtLastAddress = _speedometer->GetCurrentTicks();
	SetLastDetectedAddress(addr);
	//if target position is negative 1 it stops on all stations
	if((_targetPosition == TARGET_POSITION_ALL) || (_targetPosition == addr)) {
			//only stop if not already stopped
		if(_motor->GetSpeed() != 0) {
			SetBipolarSpeed(0);
			SendMsg(COMMAND_STATE, STATE_STOPPED_AT_TARGET);
			SendMsg(COMMAND_BIPOLAR_SPEED, 0);

			//Notify computer that the target position has changed to none
			SetTargetPosition(TARGET_POSITION_NONE);
			SendMsg(COMMAND_TARGET_POSITION, TARGET_POSITION_NONE);
		}
	}
	if((_lastDetectedAddressUpdate + _beaconAddressUpdateInterval) < millis())
	{
		SendMsg(COMMAND_LAST_DETECTED_ADDRESS, GetLastDetectedAddress());
		_lastDetectedAddressUpdate = millis();
	}
}
