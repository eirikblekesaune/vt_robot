#ifndef PENS_H
#define PENS_H

typedef struct pens_t {
	int penseNumber;
	int buttonPin;
	int enablePin;
	int directionPinA;
	int directionPinB;
	int buttonPosition;
	int targetPosition;
};

#endif
