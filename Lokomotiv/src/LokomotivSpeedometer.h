#ifndef LOKOMOTIV_SPEEDOMETER_H
#define LOKOMOTIV_SPEEDOMETER_H

class LokomotivSpeedometer
{
public:
	LokomotivSpeedometer();
	~LokomotivSpeedometer();
	long GetMeasuredSpeed(); 
private:
	long _measuredSpeed;
	int _isrNumber;
};

#endif
