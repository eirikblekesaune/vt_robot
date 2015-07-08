#include "DCMotor.h"

DCMotor::DCMotor(int pulsePin, int stopButtonPinR, int stopButtonPinL) :
	_pulsePin(pulsePin),
	_stopButtonPinR(stopButtonPinR),
	_stopButtonPinL(stopButtonPinL)
{
	//init pins
	pinMode(_pulsePin, OUTPUT);
	pinMode(_stopButtonPinR, INPUT_PULLUP);
	pinMode(_stopButtonPinL, INPUT_PULLUP);
	Update();
}

bool DCMotor::_canSetSpeed(int speed) {
	bool result = true;
	
	//we can always stop
	if(speed != 0) {
		//negative speed is L direction
		//if it is to be set to R direction (positive values)
		if(GetDirection() == DIRECTION_R) {
			//and the R direction sensor value is PRESSED
			if(_stopButtonR == BUTTON_PRESSED) {
				result = false;//no can move
			}
		} else {
			//if is to be set to L direction (negative values)
			//and the L button is pressed
			if(_stopButtonL == BUTTON_PRESSED) {
				result = false;
			}
		}
	}
	return result;
}

void DCMotor::SetSpeed(int speed) {
	int waitTime;
	if(_canSetSpeed(speed)) {
		if(speed < -512)
			speed = -512;
		if(speed > 512)
			speed = 512;
		_speed = speed;
		waitTime = map(speed, -512, 512, 1000, 2000);
		digitalWrite(_pulsePin, HIGH);
		delayMicroseconds(waitTime);
		digitalWrite(_pulsePin, LOW);
	}
}

void DCMotor::Stop() {
	SetSpeed(0);
}

void DCMotor::Update() {
	_stopButtonR = digitalRead(_stopButtonPinR);
	_stopButtonL = digitalRead(_stopButtonPinL);
	if((_stopButtonR == LOW) && (GetDirection() == DIRECTION_R)) {
		Stop();
	}
	if((_stopButtonL == LOW) && (GetDirection() == DIRECTION_L)) {
		Stop();
	}
}

direction_t DCMotor::GetDirection() {
	direction_t result = DIRECTION_R;
	if(_speed < 0)
		result = DIRECTION_L;
	return result;
}

void DCMotor::_checkSensors() {
}
