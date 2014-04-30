#ifndef _ENKEL_FUGL_H
#define _ENKEL_FUGL_H
#include <Arduino.h>
#include "StepperMotor.h"

#define LIFT_UP 0
#define LIFT_DOWN 1

class EnkelFugl
{
public:
	EnkelFugl(StepperMotor *stepperMotor);
	~EnkelFugl() {};
	void Update();
	StepperMotor *liftMotor;
private:
};
#endif
