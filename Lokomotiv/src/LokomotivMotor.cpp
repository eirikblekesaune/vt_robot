#include "LokomotivMotor.h"

const int LokomotivMotor::kSpeedMin = 0;
const int LokomotivMotor::kSpeedStop = LokomotivMotor::kSpeedMin;
const int LokomotivMotor::kSpeedMax = 512;

LokomotivMotor::LokomotivMotor(VNH5019Driver *driver, int hallISRIndex) :
	driver_(driver)
{
	encoder_ = new HallEncoder(hallISRIndex);
	pid_ = new PID(&setpoint_, &input_, &output_, 4.0, 0.2, 1.0, DIRECT);
	pid_->SetOutputLimits(LokomotivMotor::kSpeedMin, LokomotivMotor::kSpeedMax);
	pid_->SetMode(AUTOMATIC);
}

void LokomotivMotor::Update()
{
	input_ = encoder_->CalculateSpeed();
	if(pid_->Compute())
	{
		if(driver_->GetSpeed() != kSpeedStop)
		{
			driver_->SetSpeed(output_);
		}
	}
}

void LokomotivMotor::ResetPositionCounter()
{

}
