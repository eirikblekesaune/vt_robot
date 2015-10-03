#include "DCMotor.h"

DCMotor::DCMotor(int pulsePin, int topStopButtonPin, int bottomStopButtonPin, int number) :
	_pulsePin(pulsePin),
	_topStopButtonPin(topStopButtonPin),
	_bottomStopButtonPin(bottomStopButtonPin),
	_number(number),
	_speed(0)
{
	//init pins
	pinMode(_pulsePin, OUTPUT);
	pinMode(_topStopButtonPin, INPUT_PULLUP);
	pinMode(_bottomStopButtonPin, INPUT_PULLUP);
	Update();
}

bool DCMotor::_isBlocked(int speed) {
	bool result = false;
	
	//we can always stop
	if(speed != 0) {
		//negative speed is L direction, or down
		//if it is to be set to R direction (positive values)
		if(speed > 0) {//going upwards
			//and the R direction sensor value is PRESSED
			if(_state == STATE_AT_TOP) {
				result = true;//is blocked indeed
			}
		} else if(speed < 0) {//going downwards
			//if is to be set to L direction (negative values), or down direction
			//and the L button is pressed
			if(_state == STATE_AT_BOTTOM) {
				result = true;
			}
		}
	}
	return result;
}

void DCMotor::SetSpeed(int speed) {
	int waitTime;
	if(!_isBlocked(speed)) {
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
	_readButtons();
	int newState;
	if(_topStopButton == BUTTON_PRESSED)
	{
		if(_state == STATE_GOING_UP) {
			Stop();
		}
		newState = STATE_AT_TOP;
	} else if(_bottomStopButton == BUTTON_PRESSED) {
		if(_state == STATE_GOING_DOWN) {
			Stop();
		}
		newState = STATE_AT_BOTTOM;
	} else {
		if(_speed == 0) {
			newState = STATE_STOPPED;
		} else if(_speed > 0) {
			newState = STATE_GOING_UP;
		} else if(_speed < 0) {
			newState = STATE_GOING_DOWN;
		}
	}
	//check if state has changed
	if(_state != newState) {
		_state = newState;
		Serial.print("s");
		Serial.print(_number);
		Serial.print(_state);
	}
}

direction_t DCMotor::GetDirection() {
	direction_t result = DIRECTION_UP;
	if(_speed < 0)
		result = DIRECTION_DOWN;
	return result;
}

void DCMotor::_readButtons()
{
	int newTopVal;
	int newBottomVal;
	newTopVal = digitalRead(_topStopButtonPin);
	newBottomVal= digitalRead(_bottomStopButtonPin);
	if(newTopVal != _topStopButton) {
		_topStopButton = newTopVal;
		Serial.print("bT");
		Serial.print(_number);
		Serial.print(_topStopButton);
	}
	if(newBottomVal != _bottomStopButton) {
		_bottomStopButton = newBottomVal;
		Serial.print("bB");
		Serial.print(_number);
		Serial.print(_bottomStopButton);
	}
	//_topStopButton = digitalRead(_topStopButtonPin);
	//_bottomStopButton = digitalRead(_bottomStopButtonPin);
}
