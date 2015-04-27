#include "pens.h"

enum {
	PARSER_WAITING_FOR_HEADER,//header char is 'P'
	PARSER_WAITING_FOR_PENSE_NUMBER_DIGIT_0,
	PARSER_WAITING_FOR_PENSE_NUMBER_DIGIT_1,
	PARSER_WAITING_FOR_PENSE_COMMAND, //'0' = straight, '1' = turn, '~' = toggle '?' = query
	PARSER_WAITING_FOR_CR,
	PARSER_WAITING_FOR_NL
};

enum {
	PENS_STRAIGHT,
	PENS_TURNED
};

int parserState = PARSER_WAITING_FOR_HEADER;
char pensNumberChars[2];
int currentPenseNumber;
char currentCommand;

#define NUM_PENSER 12
pens_t penser[NUM_PENSER] = {
	{A0, 23, 25, 27, LOW},
	{A1, 29, 31, 33, LOW},
	{A2, 35, 37, 39, LOW},
	{A3, 41, 43, 45, LOW},
	{A4, 47, 49, 51, LOW},
	{A5, 22, 24, 26, LOW},
	{A6, 28, 30, 32, LOW},
	{A7, 34, 36, 38, LOW},
	{A8, 2, 3, 4, LOW},
	{A9, 5, 6, 7, LOW},
	{A10, 8, 9, 10, LOW},
	{A11, 11, 12, 13, LOW}
};

void setup()
{
	Serial.begin(57600);
	for(int i = 0; i < NUM_PENSER; i++) {
		pinMode(penser[i].buttonPin, INPUT);
		pinMode(penser[i].enablePin, OUTPUT);
		pinMode(penser[i].directionPinA, OUTPUT);
		pinMode(penser[i].directionPinB, OUTPUT);
	}
}

void loop()
{
	for(int i = 0; i < NUM_PENSER; i++) {
		int val;
		val = digitalRead(penser[i].buttonPin);
		if(val != penser[i].currentValue) {
			penser[i].currentValue = val;
			sendPensPosition(i + 1);
		}
	}

	if(Serial.available() > 0) {
		int currentByte = Serial.read();
		switch(parserState) {
			case PARSER_WAITING_FOR_HEADER:
				if(currentByte == 'P') {
					parserState = PARSER_WAITING_FOR_PENSE_NUMBER_DIGIT_0;
				} else { 
					parserState = PARSER_WAITING_FOR_HEADER; 
				}
				break;
			case PARSER_WAITING_FOR_PENSE_NUMBER_DIGIT_0:
				if((currentByte >= 48) && (currentByte <= 57)) {
					parserState = PARSER_WAITING_FOR_PENSE_NUMBER_DIGIT_1;
					pensNumberChars[0] = currentByte;
				} else {
					parserState = PARSER_WAITING_FOR_HEADER;
				}
				break;
			case PARSER_WAITING_FOR_PENSE_NUMBER_DIGIT_1:
				if((currentByte >= 48) && (currentByte <= 57)) {
					parserState = PARSER_WAITING_FOR_PENSE_COMMAND;
					pensNumberChars[1] = currentByte;
				} else {
					parserState = PARSER_WAITING_FOR_HEADER;
				}
				break;
			case PARSER_WAITING_FOR_PENSE_COMMAND:
				currentCommand = currentByte;
				parserState = PARSER_WAITING_FOR_CR;
				break;
			case PARSER_WAITING_FOR_CR:
				if(currentByte = 13) {
					parserState = PARSER_WAITING_FOR_NL;
				} else {
					parserState = PARSER_WAITING_FOR_HEADER;
				}
				break;
			case PARSER_WAITING_FOR_NL:
				if(currentByte = 10) {
					parserState = PARSER_WAITING_FOR_NL;
					doCommand(currentCommand);
				}
				parserState = PARSER_WAITING_FOR_HEADER;
				break;
			default:
				parserState = PARSER_WAITING_FOR_HEADER;
		}
	}
}

void sendPensPosition(int penseNumber) {
	Serial.print("P");
	Serial.print(penseNumber);
	Serial.println(penser[penseNumber - 1].currentValue);
}

void doCommand(char cmd) {
	int penseNumber = atoi(pensNumberChars);
	switch(cmd) {
		case '1':
			setPosition(&penser[penseNumber - 1], PENS_TURNED);
			break;
		case '0':
			setPosition(&penser[penseNumber - 1], PENS_STRAIGHT);
			break;
		case '~':
			togglePosition(&penser[penseNumber - 1]);
			break;
		case '?':
			sendPensPosition(penseNumber);
			break;
	}
}

