#ifndef LOKOMOTIV_MOTOR_H
#define LOKOMOTIV_MOTOR_H

#include "PID_v1.h"
#include "VNH5019Driver.h"
#include "HallEncoder.h"

class LokomotivMotor {
public:
	LokomotivMotor(VNH5019Driver *driver, int isrIndex);
	~LokomotivMotor();
	static const int kSpeedMin;
	static const int kSpeedMax;
	static const int kSpeedStop;
	void SetSpeed(int speed) {driver_->SetSpeed(speed);};
	void SetDirection(int direction) {driver_->SetDirection(direction);};
	int GetSpeed() {return driver_->GetSpeed();};
	int GetMeasuredSpeed() {return encoder_->CalculateSpeed();}
	int GetDirection() {return driver_->GetDirection();};
	void Update();
	void ResetPositionCounter();
private:
	VNH5019Driver *driver_;
	PID *pid_;
	HallEncoder *encoder_;
	double setpoint_;
	double input_;
	double output_;
};
#endif

