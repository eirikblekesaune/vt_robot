#ifndef LOKOMOTIV_SPEEDOMETER_H
#define LOKOMOTIV_SPEEDOMETER_H

class LokomotivSpeedometer
{
public:
	LokomotivSpeedometer();
	~LokomotivSpeedometer();
	double GetMeasuredSpeed();
	double GetMeasuredTicks();
	long GetCurrentTicks();
	long GetRawSpeed();
	void DirectionChanged(int newDirection);
private:
	long _measuredSpeed;
	int _isrNumber;
};

#endif
