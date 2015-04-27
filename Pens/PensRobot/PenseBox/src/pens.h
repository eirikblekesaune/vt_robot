#ifndef PENS_H
#define PENS_H

typedef struct pens_t {
	int buttonPin;
	int enablePin;
	int directionPinA;
	int directionPinB;
	int currentValue;
};

void setPosition(pens_t *pens, int position) {
	if(position == 0) {
		digitalWrite(pens->directionPinA, HIGH);
		digitalWrite(pens->directionPinB, LOW);
	} else if(position == 1) {
		digitalWrite(pens->directionPinA, LOW);
		digitalWrite(pens->directionPinB, HIGH);
	}
	digitalWrite(pens->enablePin, HIGH);
	delay(50);
}

void togglePosition(pens_t *pens) {
	setPosition(pens, !pens->currentValue);
}

#endif
