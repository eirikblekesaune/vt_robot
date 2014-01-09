#ifndef LOKOMOTIV_H
#define LOKOMOTIV_H
#include "IRReader.h"
#include "LEDDimmer.h"
#include "LokomotivMotor.h"

class Lokomotiv{
public:
	Lokomotiv();
	~Lokomotiv();
	long GetLastIRBeaconID() {};
	long GetDistanceFromLastIRBeaconDetection() {};
	void Update();
private:
	LokomotivMotor *motor_;
};

#endif
