#ifndef LOKOMOTIV_H
#define LOKOMOTIV_H
#include "IRReader.h"
#include "LokomotivMotor.h"
#include "LokomotivSpeedometer.h"
//Change the default i2c rate
//#define TWI_FREQ 50000L
//#include "Wire.h"

class Lokomotiv{
public:
	Lokomotiv();
	~Lokomotiv();
	unsigned char GetLastIRBeaconID() {};
	long GetDistanceFromLastIRBeaconDetection();
	
	long GetSpeed();
	long GetEndSpeed() {_motor->GetEndSpeed();};
	long GetDirection();
	long GetTargetPosition();
	long GetDistanceFromLastAddress();
	long GetLed();
	long GetState();
	long GetMeasuredSpeed();
	long GetLastDetectedAddress();
	double GetPidPValue();
	double GetPidIValue();
	double GetPidDValue();

	void SetSpeed(long val) {_motor->SetSpeed(static_cast<speed_t>(val));};
	void SetEndSpeed(long val) {_motor->SetEndSpeed(static_cast<speed_t>(val));};
	void SetGlideToSpeed(long val) {_motor->GlideToSpeed(static_cast<int16_t>(val));};
	void SetDirection(long val) {_motor->SetDirection(static_cast<int16_t>(val));};
	void SetTargetPosition(long val);
	void SetDistanceFromLastAddress(long val);
	void SetLed(long val);
	void SetState(long val);
	void SetLastDetectedAddress(long val);
	void SetPidPValue(double val);
	void SetPidIValue(double val);
	void SetPidDValue(double val);
	void Stop(long val) {_motor->Stop(static_cast<int16_t>(val));};
	void Init();
	void Update();
	void GotAddr(unsigned char);

	volatile long _distanceFromLastBeacon;
private:
	LokomotivMotor *_motor;
	IRReader *_irReader;
	LokomotivSpeedometer *_speedometer;
	unsigned char _lastBeaconAddress;
	long _lastBeaconAddressUpdate;
	static const long _beaconAddressUpdateInterval;

	long _targetPosition;
	long _distanceFromLastAddress;
	int16_t _led;
	long _state;
	long _measuredSpeed;
	long _lastDetectedAddress;
	double _pidPValue;
	double _pidIValue;
	double _pidDValue;

};

#endif
