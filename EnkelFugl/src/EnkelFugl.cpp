#include "EnkelFugl.h"
#define DEBUG_FUGL
EnkelFugl::EnkelFugl(StepperMotor *lift_, int positionSensorPin)
	: liftMotor(lift)
{
	liftMotor->init();
}


void EnkelFugl::Update()
{
	liftMotor->Update();
}

//state and events
void EnkelFugl::_stateChanged(state_t newState)
{
	liftState = newState;
#ifdef DEBUG_FUGL
	String stateString;
	switch(liftState) {
		case AT_MAX_POSITION:
		stateString = "AT_MAX_POSITION";
			break;
		case AT_MIN_POSITION:
		stateString = "AT_MIN_POSITION";
			break;
		case AT_TARGET:
		stateString = "AT_TARGET";
			break;
		case GOING_UP:
		stateString = "GOING_UP";
			break;
		case GOING_DOWN:
		stateString = "GOING_DOWN";
			break;
		case GOING_TO_TARGET:
		stateString = "GOING_TO_TARGET";
			break;
	}
	Serial.print("state: ");
	Serial.println(stateString);
#endif

}

