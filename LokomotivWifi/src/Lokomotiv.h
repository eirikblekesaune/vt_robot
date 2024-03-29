#ifndef LOKOMOTIV_H
#define LOKOMOTIV_H
#include "IRReader.h"
#include "LokomotivMotor.h"
#include "LokomotivSpeedometer.h"
#include "Wire.h"

class Lokomotiv{
public:
	Lokomotiv();
	~Lokomotiv();
	unsigned char GetLastIRBeaconID() {};
	long GetDistanceFromLastIRBeaconDetection();

	long GetBipolarSpeed();
	long GetEndSpeed() {_motor->GetEndSpeed();};
	long GetTargetPosition();
	long GetDistanceFromLastAddress();
	long GetPeripheral(long data);
	stateChange_t GetState();
	double GetMeasuredSpeed();
	long GetLastDetectedAddress();
	double GetPidPValue();
	double GetPidIValue();
	double GetPidDValue();
	long GetTrackingPollingInterval();
	long GetTrackingState();
	long GetTrackingData();
	long GetMotorMode() {return static_cast<long>(_motor->GetMotorMode());};
	double GetMinTargetSpeed() {return _motor->GetMinTargetSpeed();};
	double GetMaxTargetSpeed() {return _motor->GetMaxTargetSpeed();};
	long GetClockCalibration() {return _clockCalibration;};
	long GetTimestamp();

	void SetBipolarSpeed(long val);
	void SetEndSpeed(long val) {_motor->SetEndSpeed(static_cast<speed_t>(val));};
	void SetGlideToSpeed(long val) {_motor->GlideToSpeed(static_cast<int16_t>(val));};
	void SetTargetPosition(long val);
	void SetDistanceFromLastAddress(long val);
	void SetPeripheral(long val);
	void SetPeripheralRequest(long val);
	void SetLastDetectedAddress(long val);
	void SetPidPValue(double val);
	void SetPidIValue(double val);
	void SetPidDValue(double val);
	void SetTrackingPollingInterval(long val);
	void SetTrackingState(long val);
	void SetMotorMode(long val);
	void SetMinTargetSpeed(double val) {_motor->SetMinTargetSpeed(val);};
	void SetMaxTargetSpeed(double val) {_motor->SetMaxTargetSpeed(val);};
	void SetClockCalibration(long val) {_clockCalibration = val;};

	void Stop(long val) {_motor->Stop(static_cast<int16_t>(val));};
	void Init();
	void Update();
	void GotAddr(long val);
	void UserChangedSpeed() {_motor->UserChangedSpeed();};
	void SendTrackingData(int32_t dist);
	void SendTrackingData();
	void StateChanged(stateChange_t newState);

private:
	LokomotivMotor *_motor;
	IRReader *_irReader;
	LokomotivSpeedometer *_speedometer;
	long _lastDetectedAddressUpdate;
	long _encoderCounterAtLastAddress;
	long _clockCalibration;
	static const long _beaconAddressUpdateInterval;

	long _targetPosition;
	long _distanceFromLastAddress;
	int16_t _led;
	stateChange_t _state;
	long _measuredSpeed;
	long _lastDetectedAddress;
	double _pidPValue;
	double _pidIValue;
	double _pidDValue;
	long _trackingPollingInterval;
	bool _trackingPollingEnabled;

};

#endif
