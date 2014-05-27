#ifndef LOKOMOTIV_SPEEDOMETER_H
#define LOKOMOTIV_SPEEDOMETER_H

class LokomotivSpeedometer
{
public:
	LokomotivSpeedometer();
	~LokomotivSpeedometer();
	long GetMeasuredSpeed(); 
	long GetCurrentTicks();
	void DirectionChanged(int newDirection);
private:
	long _measuredSpeed;
	int _isrNumber;
};

#endif
