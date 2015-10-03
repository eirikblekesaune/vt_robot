#include "Arduino.h"
#include "DCMotor.h"
//
//parser stuff
enum parserState_t {
	WAITING_COMMAND,
	WAITING_MOTOR_DATA,
	WAITING_MOTOR_VALUE_MSB,
	WAITING_MOTOR_VALUE_LSB,
	WAITING_RELAY_VALUE,
	WAITING_CR,
	WAITING_LF
} _parserState_t;

enum command_t {
	COMMAND_NONE,
	COMMAND_MOTOR,
	COMMAND_RELAY
} _command_t;

enum sign_t {
	SIGN_NONE,
	SIGN_POSITIVE,
	SIGN_NEGATIVE
} _sign_t;

parserState_t parserState;
byte currentCommand;
byte currentSign;
byte currentValue;
int currentMotorValue;
int currentMotorNumber;
int currentRelayValue;


//global motor objects
DCMotor *motor1;
DCMotor *motor2;
DCMotor *motor3;

//relay stuff
int relayPin = 11;
enum relayPosition_t {
	RELAY_NONE,
	RELAY_ON,
	RELAY_OFF
} _relayPosition_t;
int relayPosition = RELAY_OFF;

void setup()
{

	//Start serial connection
	resetParser();
	Serial.begin(57600);
	//Relay pins
	pinMode(relayPin, OUTPUT);
	setRelayPosition(RELAY_OFF);

	//Initialize dcmotors
	motor3 = new DCMotor(8, 9, 10, 3);
	motor2 = new DCMotor(5, 6, 7, 2);
	motor1 = new DCMotor(2, 3, 4, 1);
	
	
	//Wait a moment
	delay(3000);

	//Close relay to power up motor drivers
	setRelayPosition(RELAY_ON);
	//
}

void resetParser(){
	parserState = WAITING_COMMAND;
	currentCommand = COMMAND_NONE;
	currentSign = SIGN_NONE;
	currentMotorValue = 0;
	currentMotorNumber = 0;
	currentRelayValue = RELAY_NONE;
}

void setRelayPosition(int pos) {
	int pinval;
	if(pos == RELAY_ON) {
		digitalWrite(relayPin, LOW);
		relayPosition = pos;
	} else if(pos == RELAY_OFF) {
		digitalWrite(relayPin, HIGH );
		relayPosition = pos;
	}
}

void loop()
{
	//update sensorstates
	motor1->Update();
	motor2->Update();
	motor3->Update();

	//check incoming serial data
	while(Serial.available() > 0) {
		byte inByte = Serial.read();
		switch(parserState) {
			case WAITING_COMMAND:
				if(inByte == 'M') {
					currentCommand = COMMAND_MOTOR;
					parserState = WAITING_MOTOR_VALUE_MSB;
				} else if(inByte == 'R') {
					currentCommand = COMMAND_RELAY;
					parserState = WAITING_RELAY_VALUE;
				} else if(inByte == '?') {
					Serial.print("s1");
					Serial.print(motor1->_state);
					Serial.print("s2");
					Serial.print(motor2->_state);
					Serial.print("s3");
					Serial.print(motor3->_state);
					resetParser();
				} else {
					resetParser();
				}
				break;
			case WAITING_MOTOR_VALUE_MSB:
				if((inByte & 0x80) == 0) {//check if bit 7 is 0
					currentMotorValue = static_cast<int>(inByte & 0x07) << 7;
					//Serial.write(1);
					//Serial.println(currentMotorValue);
					currentMotorNumber = static_cast<int>(inByte & 0x70) >> 4;
					if(inByte & 0x08) {
						currentSign = SIGN_NEGATIVE;
					} else {
						currentSign = SIGN_POSITIVE;
					}
					parserState = WAITING_MOTOR_VALUE_LSB;
				} else {
					resetParser();
				}
				break;
			case WAITING_MOTOR_VALUE_LSB:
				if((inByte & 0x80) == 0) {//check if bit 7 is 0
					currentMotorValue = currentMotorValue | static_cast<int>(inByte);
					//Serial.write(2);
					//Serial.print(currentMotorValue);
					parserState = WAITING_CR;
				} else {
					resetParser();
				}
				break;
			case WAITING_RELAY_VALUE:
				if(inByte == 0) {
					currentRelayValue = RELAY_OFF;
					parserState = WAITING_CR;
				} else if(inByte == 1) {
					currentRelayValue = RELAY_ON;
					parserState = WAITING_CR;
				} else {
					resetParser();
				}
				break;
			case WAITING_CR:
				if(inByte == 13) {
					parserState = WAITING_LF;
				} else {
					resetParser();
				}
				break;
			case WAITING_LF:
				if(inByte == 10) {
					if(currentCommand == COMMAND_MOTOR) {
						int speed = currentMotorValue;
						if(currentSign == SIGN_NEGATIVE) {
							//Serial.write(3);
							speed = -speed;
						} else {
							//Serial.write(4);
						}
						if(currentMotorNumber == 1) {
							///Serial.write(5);
							//Serial.println(speed);
							motor1->SetSpeed(speed);
						} else if(currentMotorNumber == 2) {
							//Serial.write(6);
							//Serial.print(speed);
							motor2->SetSpeed(speed);
						} else if(currentMotorNumber == 3) {
							//Serial.write(6);
							//Serial.print(speed);
							motor3->SetSpeed(speed);
						}
					} else if(currentCommand = COMMAND_RELAY) {
						setRelayPosition(currentRelayValue);
					}
				} 
				resetParser();
				break;
			default:
				resetParser();
		}
		delay(1);
	}
	//Motor speed message:
	//'M' 0xxxynnn 0nnnnnnn 13 10 --  y = sign bit, x = motor number bytes, n = motor value bytes
	//
	//Power relay message:
	//'R' '0'|'1' 13 10
}
