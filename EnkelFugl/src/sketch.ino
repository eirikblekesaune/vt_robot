//002: Ethernet controlled bird head
//receives message in format:
// /robot/fugl/direction "up"|"down"
// /robot/fugl/speed int16_t 0-5
// /robot/fugl/enable int16_t 0|1
// /robot/fugl/stepinterval int16_t 100-100000

/*
Connected is a A4988 stepper driver from Pololu. 
Using the following pin connections:
ENABLE -xxxx- V+MOTOR
MS1    -xxxx- GND
MS2    -xxxx- 2B (Connected to stepper blue wire, coil D)
MS3    -xxxx- 2A (Connected to stepper red wire, coil B)
RESET  -xxxx- 1A (Connected to stepper black wire, coil A)
SLEEP  -xxxx- 1B (Connected to stepper green wire, coil C)
STEP   -xxxx- Vdd (5v)
DIR    -xxxx- GND
*/

/*
TODO:
-load MAC/ip from EEPROM (or memory card?)
-test getters
-stop buttons
-go num steps function
-info function (include version and sketch name)
-update all function
-state notification to mekanikk
*/


#include <SPI.h>
#include <Ethernet.h>
#include "ArdOSC.h"
#include "EnkelFugl.h"

#define DEBUG_FUGL

byte mac[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
byte ip[] = {1, 2, 3, 112};
int receivePort = 50004;
int sendPort = 57120;
OSCServer server;
OSCClient client;


const int dirPin = 2;
const int stepPin = 3;
const int enablePin = 8;
const int msPins[3] = {7,6,5}; //ms3, ms2, ms1
const int potPin = 0;
state_t currentState = STOPPED;
int16_t currentSpeed = STOP_SPEED;
unsigned long lastStepTime = 0;
unsigned long stepInterval;
int16_t currentDirection = UP;
int16_t enabled = 1;
int16_t currentPosition;
int16_t currentMaxPosition = MAX_POSITION;
int16_t currentMinPosition = MIN_POSITION;
int16_t targetPosition = NO_TARGET;
int16_t stepType = SIXTEENTH_STEP;
long lastPositionReadTime;

void setup() {
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  for(int i = 0; i< 3; i++) {
    pinMode(msPins[i], OUTPUT);
  }
  
	currentPosition = analogRead(potPin);
	setSpeed(STOP_SPEED);
  //Init ethernet/OSC
  Ethernet.begin(mac, ip);
  server.begin(receivePort);
  server.addCallback("/robot/fugl/direction", &handleDirectionMsg);
  server.addCallback("/robot/fugl/speed", &handleSpeedMsg);
  server.addCallback("/robot/fugl/stepType", &handleStepTypeMsg);
  server.addCallback("/robot/fugl/enable", &handleEnableMsg);
	server.addCallback("/robot/fugl/targetPosition", &handleTargetPositionMsg);
  server.addCallback("/robot/fugl/setMax", &handleSetMaxMsg);
  server.addCallback("/robot/fugl/setMin", &handleSetMinMsg);
  
  //init serial
  Serial.begin(9600);
  digitalWrite(enablePin, LOW);
}


void loop() {
//  if(server.aviableCheck()>0){
//     Serial.println("alive! "); 
//  }
	readSensors();
	if((micros() - lastStepTime) <= stepInterval) 
	{
		if(canStep())
		{
			doStep();
		}
	}
}

void readSensors()
{
	if((millis() - lastPositionReadTime) >= 50)
	{
		currentPosition = analogRead(potPin);
		lastPositionReadTime = millis();
		Serial.println(currentPosition);
		if(
				(currentPosition >= currentMaxPosition) && 
				(currentState != AT_MAX_POSITION) &&
				(currentDirection == UP)
				)
		{
			maxPositionReached();
		} else if(
				(currentPosition <= currentMinPosition) && 
				(currentState != AT_MIN_POSITION) &&
				(currentDirection == DOWN)
				)
		{
			minPositionReached();
		}
	}
}

bool canStep()
{
	bool result = true;
	if((currentSpeed == STOP_SPEED) || (currentState >= AT_TARGET))
	{		
		result = false;
	}
	return result;
}

void doStep()
{
	digitalWrite(stepPin, LOW);
	delayMicroseconds(2);
	digitalWrite(stepPin, HIGH);
	lastStepTime = micros();
}

////////////////////////////////////////
///OSC message handler functions
////////////////////////////////////////
void handleDirectionMsg(OSCMessage *_msg) {
  int16_t numargs;
  numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagString) {
      int16_t argSize = _msg->getArgStringSize(0);
      char dirArg[argSize];
      _msg->getArgString(0, dirArg);
			String dirStr = String(dirArg);
			if(dirStr == "up")
			{
				setDirection(UP);
			} else if(dirStr == "down")
			{
				setDirection(DOWN);
			}
    }
  } else if(numargs == 0) {
    updateClient(_msg->getIpAddress(), kParamDirection);
  }
}

void handleSpeedMsg(OSCMessage *_msg) {
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
			int16_t newSpeed = (int16_t)_msg->getArgInt32(0);
      setSpeed(newSpeed);
    }
  }
}

void handleEnableMsg(OSCMessage *_msg) {
#ifdef DEBUG_FUGL
	Serial.println("got enabled");
#endif
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setEnabled((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleSetMaxMsg(OSCMessage *_msg) {
#ifdef DEBUG_FUGL
	Serial.println("got setMax");
#endif
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setMaxPosition((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleSetMinMsg(OSCMessage *_msg) {
#ifdef DEBUG_FUGL
	Serial.println("got setMin");
#endif
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setMinPosition((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleTargetPositionMsg(OSCMessage *_msg)
{
	int16_t numArgs = _msg->getArgsNum();
	if(numArgs == 1)
	{
		if(_msg->getArgTypeTag(0) == kTagInt32)
		{
			goToPosition((int16_t)_msg->getArgInt32(0));
		}
	}
}

void handleStepTypeMsg(OSCMessage *_msg)
{
	int16_t numArgs = _msg->getArgsNum();
	if(numArgs == 1)
	{
		if(_msg->getArgTypeTag(0) == kTagInt32)
		{
			setStepType((int16_t)_msg->getArgInt32(0));
		}
	}
}

/////////Setters and getters

void setTargetPosition(int16_t pos)
{
	targetPosition = constrain(pos, MIN_POSITION, MAX_POSITION);
	if(currentPosition < targetPosition)
	{
		setDirection(UP);
	} else {
		setDirection(DOWN);
	}
	stateChanged(GOING_TO_TARGET);
}


void setDirection(int16_t dir) {
  if(dir == UP) { 
    currentDirection = UP;
    digitalWrite(dirPin, HIGH);
  }
  if(dir == DOWN) {
    currentDirection = DOWN;
    digitalWrite(dirPin, LOW);
  }
} 

void stop()
{
	setSpeed(STOP_SPEED);
}

void setMaxPosition(int16_t pos)
{
	currentMaxPosition = constrain(pos, MIN_POSITION, MAX_POSITION);
}

void setMinPosition(int16_t pos)
{
	currentMinPosition = constrain(pos, MIN_POSITION, MAX_POSITION);
}

void setSpeed(int16_t newSpeed)
{

	currentSpeed = constrain(newSpeed, MIN_SPEED, MAX_SPEED);
	if(newSpeed == 0)
	{
		stateChanged(STOPPED);
	} else if(currentSpeed < 0)
	{
		setDirection(DOWN);
	} else if(currentSpeed > 0) {
		setDirection(UP);
	}
	stepInterval = map(currentSpeed, 0, 4096, 10000, 10);
}

void setEnabled(int16_t enb) {
  if(enb == 0) {
    enabled = 0;
    digitalWrite(enablePin, HIGH);
  } else if(enb == 1) {
    enabled = 1;
    digitalWrite(enablePin, LOW);
  }
  //Serial.print("set enabled: ");
  //Serial.println(enabled);
}

//state and events
void stateChanged(state_t newState)
{
	String stateString;
	currentState = newState;
#ifdef DEBUG_FUGL
	switch(currentState) {
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

void reachedTarget()
{
	stop();
	targetPosition = NO_TARGET;
	stateChanged(AT_TARGET);
}

void minPositionReached()
{
	stop();
	stateChanged(AT_MIN_POSITION);
}

void maxPositionReached()
{
	stop();
	stateChanged(AT_MAX_POSITION);
}
void setStepType(int16_t spd) {
  switch(spd) {
    case 16:
      stepType = SIXTEENTH_STEP;
      break;
    case 8:
      stepType = EIGHT_STEP;
      break;
    case 4:
      stepType = QUARTER_STEP;
      break;
    case 2:
      stepType = HALF_STEP;
      break;
    case 1:
      stepType = FULL_STEP;
      break;
  }
  if(currentSpeed != stepType) {
    currentSpeed = stepType;
    if(currentSpeed != STOP) PORTD = (PORTD & 0x1F) | (currentSpeed << 5);
  }
  //Serial.print("set speed: ");
  //Serial.println(currentSpeed);
}




void updateClient(uint8_t *ip, int16_t parameter) {
  OSCMessage updateMsg;
  updateMsg.setAddress(ip, sendPort);
  switch(parameter) {
    case kParamDirection:
      updateMsg.beginMessage("/robot/fugl/direction");
      if(currentDirection == UP) updateMsg.addArgString("up");
      if(currentDirection == DOWN) updateMsg.addArgString("down");
      break;
    case kParamSpeed:
      updateMsg.beginMessage("/robot/fugl/speed");
      updateMsg.addArgInt32((int32_t)currentSpeed);
      break;
    case kParamEnable:
      updateMsg.beginMessage("/robot/fugl/enabled");
      updateMsg.addArgInt32(enabled);
      break;
    case kParamInterval:
      updateMsg.beginMessage("/robot/fugl/interval");
      updateMsg.addArgInt32((int32_t)stepInterval);
      break;
    default:
      break;
  }
  client.send(&updateMsg);
}

