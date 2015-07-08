#include "pens.h"

num {
	PARSER_WAITING_FOR_COMMAND,
	PARSER_WAITING_FOR_PENS_DATA,
	PARSER_WAITING_FOR_CR,
	PARSER_WAITING_FOR_NL
};

const int kBUTTON_UP = HIGH;
const int kBUTTON_DOWN = LOW;
const int kPENS_STRAIGHT = kBUTTON_DOWN;
const int kPENS_TURNED = kBUTTON_UP;
const int kNUM_PENSER = 12;

int parserState = PARSER_WAITING_FOR_HEADER;
char pensNumberChars[2];
const char kNO_COMMAND = '0';
const int kNO_PENSE_NUMBER = 0;
const int kNO_VALUE = -1;
int currentPenseNumber;
char currentCommand;
int currentValue;
const unsigned long kMaxEnableTime = 1000;

//buttonPin, enablePin directionPinA, directionPinB, buttonPosition, targetPosition, enableTime;
pens_t penser[kNUM_PENSER] = {
	{1, A0, 23, 25, 27, kBUTTON_DOWN, kPENS_STRAIGHT},
	{2, A1, 29, 31, 33, kBUTTON_DOWN, kPENS_STRAIGHT},
	{3, A2, 35, 37, 39, kBUTTON_DOWN, kPENS_STRAIGHT},
	{4, A3, 41, 43, 45, kBUTTON_DOWN, kPENS_STRAIGHT},
	{5, A4, 47, 49, 51, kBUTTON_DOWN, kPENS_STRAIGHT},
	{6, A5, 22, 24, 26, kBUTTON_DOWN, kPENS_STRAIGHT},
	{7, A6, 28, 30, 32, kBUTTON_DOWN, kPENS_STRAIGHT},
	{8, A7, 34, 36, 38, kBUTTON_DOWN, kPENS_STRAIGHT},
	{9, A8, 2, 3, 4, kBUTTON_DOWN, kPENS_STRAIGHT},
	{10, A9, 5, 6, 7, kBUTTON_DOWN, kPENS_STRAIGHT},
	{11, A10, 8, 9, 10, kBUTTON_DOWN, kPENS_STRAIGHT},
	{12, A11, 11, 12, 13, kBUTTON_DOWN, kPENS_STRAIGHT}
};

void setup()
{
	Serial.begin(57600);
	for(int i = 0; i < kNUM_PENSER; i++) {
		pinMode(penser[i].buttonPin, INPUT_PULLUP);
		pinMode(penser[i].enablePin, OUTPUT);
		pinMode(penser[i].directionPinA, OUTPUT);
		pinMode(penser[i].directionPinB, OUTPUT);
		penser[i].buttonPosition = digitalRead(penser[i].buttonPin);
		penser[i].targetPosition = penser[i].buttonPosition;
	}
}

void setPosition(pens_t *pens, int position) {
	//Start targeting new position if target is different that the button position
	if(pens->buttonPosition != position)
	{
		if(position == kPENS_STRAIGHT) {
			digitalWrite(pens->directionPinA, HIGH);
			digitalWrite(pens->directionPinB, LOW);
		} else if(position == kPENS_TURNED) {
			digitalWrite(pens->directionPinA, LOW);
			digitalWrite(pens->directionPinB, HIGH);
		}
		pens->targetPosition = position;
		digitalWrite(pens->enablePin, HIGH);
		pens->enableTime = millis();
		pens->isTargeting = true;
	}
}

void readButton(pens_t *pens)
{
	int val;
	val = digitalRead(pens->buttonPin);
	//if position has changed, notify computer
	if(val != pens->buttonPosition) {
		penser->buttonPosition = val;
		sendPensPosition(pens);
	}

	//if device is targeting and button show that target position has
	//been reached, turn off the targeting and the enable pin.
	if(pens->isTargeting)
	{
		if(pens->targetPosition == pens->buttonPosition)
		{
			digitalWrite(pens->enablePin, LOW);
			pens->isTargeting = false;
		} else{
			//If target position has not been reached we need to check if the enable pins
			//has been on for more than the maximum on time.
			if((millis() - pens->enableTime) > kMaxEnableTime )
			{
				digitalWrite(pens->enablePin, LOW);
				//Report that setting the position failed.
				Serial.print("P");
				Serial.print(pens->penseNumber);
				Serial.print('_');
				Serial.println("F");//F signifies failed
				pens->isTargeting = false;
			}
		}
	}
}

void togglePosition(pens_t *pens) {
	int pos = pens->buttonPosition;
	if(pos == kPENS_STRAIGHT)
	{
		setPosition(pens, kPENS_TURNED);
	} else if(pos == kPENS_TURNED)
	{
		setPosition(pens, kPENS_STRAIGHT);
	}
}

//turns off enabled pins after 100 ms
//read sensors
void update()
{
	//check all penser positions
	for(int i = 0; i < kNUM_PENSER; i++) {
		if(penser[i].buttonPosition != penser[i].targetPosition)
		{
			readButton(&penser[i]);
		}
	}
}

void loop()
{
	//update();
	//togglePosition(&penser[0]);

	if(Serial.available() > 0) {
		int currentByte = Serial.read();
		switch(parserState) {
			case PARSER_WAITING_FOR_HEADER:
				if(currentByte == 'P') {
					//get the pense number
					Serial.write(11);
					currentPenseNumber = Serial.parseInt();
					Serial.write(22);
					if(Serial.read() == '_')
					{
						Serial.write(33);
						currentCommand = Serial.read();
						if(Serial.read() == 13)
						{
							Serial.write(44);
							if(Serial.read() == 10)
							{
								Serial.write(55);
								doCommand(&penser[currentPenseNumber - 1], currentCommand);
								resetParser();
							} else {
								resetParser();
							}
						} else{
							resetParser();
						}
					} else {
						resetParser();
					}
				} else { 
					resetParser();
				}
				break;
			default:
				resetParser();
		}
	}
}

void sendPensPosition(pens_t *pens) {
	Serial.print("P");
	Serial.print(pens->penseNumber);
	Serial.print('_');
	Serial.println(pens->buttonPosition);
}

void doCommand(pens_t *pens, char cmd) {
	Serial.println("DOCMD");
	switch(cmd) {
		case '1':
			setPosition(pens, kPENS_TURNED);
			break;
		case '0':
			setPosition(pens, kPENS_STRAIGHT);
			break;
		case '~':
			togglePosition(pens);
			break;
		case '?':
			sendPensPosition(pens);
			break;
	}
}

void resetParser()
{
	parserState = PARSER_WAITING_FOR_HEADER;
	currentPenseNumber = kNO_PENSE_NUMBER;
	currentCommand = kNO_COMMAND;
	currentValue = kNO_VALUE;
	Serial.write(99);
}

