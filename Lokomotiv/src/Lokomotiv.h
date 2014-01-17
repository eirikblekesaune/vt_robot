#ifndef LOKOMOTIV_H
#define LOKOMOTIV_H
#include "IRReader.h"
#include "LEDDimmer.h"
#include "LokomotivMotor.h"

class Lokomotiv{
public:
	Lokomotiv();
	~Lokomotiv();
	unsigned char GetLastIRBeaconID() {};
	long GetDistanceFromLastIRBeaconDetection() {};
	void Init();
	void Update();
	void GotAddr(unsigned char);
	volatile long distanceFromLastBeacon_;
private:
	LokomotivMotor *motor_;
	IRReader *irReader_;
	unsigned char lastBeaconAddress_;
	long lastBeaconAddressUpdate_;
	static const long beaconAddressUpdateInterval_;
};

#endif
