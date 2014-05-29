#include <Servo.h>

uint8_t liftSpeed;
uint8_t liftDirection;
int16_t headPosition;
const uint8_t directionPinA = 6;
const uint8_t directionPinB = 7;
const uint8_t liftMotorPWMPin = 11;
const uint8_t headRotationServoPin = 10;
const uint8_t topStopPin = 5;
const uint8_t bottomStopPin = 4;
#define HEAD_ROTATE_PIN 9
#define MAX_LIFT_SPEED 255
#define MIN_LIFT_SPEED 0
#define MAX_HEAD_ROTATION_POSITION 175 
#define MIN_HEAD_ROTATION_POSITION 0
#define UP 1
#define DOWN 0

Servo headServo;

enum serialCommand{
	NO_CMD,
	LIFT_SPEED_CMD,
	LIFT_DIRECTION_CMD,
	HEAD_POSITION_CMD
};

enum _liftMotorPosition
{
	IN_BETWEEN,
	AT_TOP,
	AT_BOTTOM
} liftMotorPosition;

void SetLiftMotorSpeed(uint8_t val)
{
	if(val > MAX_LIFT_SPEED)
	{
		val = MAX_LIFT_SPEED;
	} else if(val < MIN_LIFT_SPEED)
	{
		val = MIN_LIFT_SPEED;
	}
	liftSpeed = val;
	analogWrite(liftMotorPWMPin, liftSpeed);
}

void SetLiftMotorDirection(uint8_t val)
{
	liftDirection = val;
	if(liftDirection == DOWN)
	{
		digitalWrite(directionPinA, LOW);
		digitalWrite(directionPinB, HIGH);
	} else {
		digitalWrite(directionPinA, HIGH);
		digitalWrite(directionPinB, LOW);
	}
}

void SetHeadRotationServoPosition(int16_t val)
{
	if(val > MAX_HEAD_ROTATION_POSITION)
	{
		val = MAX_HEAD_ROTATION_POSITION;
	} else if(val < MIN_HEAD_ROTATION_POSITION)
	{
		val = MIN_HEAD_ROTATION_POSITION;
	}
	headPosition = val;
	headServo.write(headPosition);
}


void test()
{
  for(int i = MIN_HEAD_ROTATION_POSITION; i < MAX_HEAD_ROTATION_POSITION; i += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
		SetHeadRotationServoPosition(i);
    delay(5);                       // waits 15ms for the servo to reach the headPositionition 
  } 
  for(int i = MAX_HEAD_ROTATION_POSITION; i>=MIN_HEAD_ROTATION_POSITION; i-=1)     // goes from 180 degrees to 0 degrees 
  {                                
		SetHeadRotationServoPosition(i);
    delay(30);                       // waits 15ms for the servo to reach the headPositionition 
  } 
}

void doCommand(uint8_t command, uint8_t value)
{
	switch(command)
	{
		case LIFT_SPEED_CMD:
			SetLiftMotorSpeed(value);
			break;
		case LIFT_DIRECTION_CMD:
			SetLiftMotorDirection(value);
			break;
		case HEAD_POSITION_CMD:
			SetHeadRotationServoPosition(static_cast<int16_t>(value));
			break;
		default:
			Serial.print(TCCR2A);
			Serial.print(TCCR2B);
	}
}

void CheckLiftMotorPosition()
{
	if((liftDirection == UP) && (digitalRead(topStopPin) == LOW))
	{
		SetLiftMotorSpeed(0);
		if(liftMotorPosition != AT_TOP)
		{
			Serial.println("T");
		}
		liftMotorPosition = AT_TOP;
	} else if((liftDirection == DOWN) && (digitalRead(bottomStopPin) == LOW))
	{
		SetLiftMotorSpeed(0);
		if(liftMotorPosition != AT_BOTTOM)
		{
			Serial.println("B");
		}
		liftMotorPosition = AT_BOTTOM;
	} else {
		liftMotorPosition = IN_BETWEEN;
	}
}

enum _parserState{
	WAITING_FOR_COMMAND,
	WAITING_FOR_VALUE,
	WAITING_FOR_LR,
	WAITING_FOR_CR
} parserState;


uint8_t currentCommand = NO_CMD;
uint8_t currentValue;
void loop() 
{ 
	static uint8_t parserState = WAITING_FOR_COMMAND;
	while(Serial.available() > 0)
	{
		switch(parserState)
		{
			case WAITING_FOR_COMMAND:
				currentCommand = Serial.read();
				parserState = WAITING_FOR_VALUE;
				break;
			case WAITING_FOR_VALUE:
				currentValue = Serial.read();
				parserState = WAITING_FOR_CR;
				break;
			case WAITING_FOR_CR:
				if(Serial.read() == 13)
				{
					parserState = WAITING_FOR_LR;
				} else {
					parserState = WAITING_FOR_COMMAND;
				}
				break;
			case WAITING_FOR_LR:
				if(Serial.read() == 10)
				{
					parserState = WAITING_FOR_COMMAND;
					doCommand(currentCommand, currentValue);
				}
				break;
			default:
				parserState = WAITING_FOR_COMMAND;
		}
	}
	CheckLiftMotorPosition();
} 


void setup()
{
	Serial.begin(9600);
	pinMode(directionPinA, OUTPUT);
	pinMode(directionPinB, OUTPUT);
	pinMode(liftMotorPWMPin, OUTPUT);
	pinMode(headRotationServoPin, OUTPUT);
	pinMode(topStopPin, INPUT_PULLUP); 
	pinMode(bottomStopPin, INPUT_PULLUP);

	headServo.attach(HEAD_ROTATE_PIN);
	//init timer2 register for headServo
	//phase correct, prescale 32, OCR2A/OCR2B output
	//TCCR2A = 0b10100001;
	//TCCR2B = 0b00001001;
	SetLiftMotorDirection(UP);
	SetLiftMotorSpeed(0);
	SetHeadRotationServoPosition(90);
}

