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

#define DEBUG_FUGL 1

byte mac[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
byte ip[] = {1, 2, 3, 112};
int receivePort = 50004;
int sendPort = 57120;
OSCServer server;
OSCClient client;

EnkelFugl *fugl;
//
const int dirPin = 2;
const int stepPin = 3;
const int enablePin = 8;
const int msPin1 = 5;
const int msPin2 = 6;
const int msPin3 = 7;
const int potPin = 0;

void setup() {
  Serial.begin(9600);
	A4988Driver *stepperDriver = new A4988Driver(
			dirPin,
			stepPin,
			enablePin,
			msPin1, 
			msPin2,
			msPin3		
	);
	StepperMotor *liftMotor = new StepperMotor(
			stepperDriver,
			potPin
	);
	fugl = new EnkelFugl(liftMotor);
   
  //Init ethernet/OSC
  Ethernet.begin(mac, ip);
  server.begin(receivePort);
  server.addCallback("/robot/fugl/direction", &handleDirectionMsg);
  server.addCallback("/robot/fugl/speed", &handleSpeedMsg);
  server.addCallback("/robot/fugl/stepType", &handleStepTypeMsg);
  server.addCallback("/robot/fugl/stepInterval", &handleStepIntervaMsg);
  server.addCallback("/robot/fugl/enable", &handleEnableMsg);
	server.addCallback("/robot/fugl/targetPosition", &handleTargetPositionMsg);
  server.addCallback("/robot/fugl/setMax", &handleSetMaxMsg);
  server.addCallback("/robot/fugl/setMin", &handleSetMinMsg);
  
  //init serial
  ///digitalWrite(enablePin, LOW);
}


void loop() {
	fugl->Update();
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
  }
}

void handleSpeedMsg(OSCMessage *_msg) {
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
			int16_t newSpeed = (int16_t)_msg->getArgInt32(0);
			Serial.println(newSpeed);
      setSpeed(newSpeed);
    }
  }
}

void handleEnableMsg(OSCMessage *_msg) {
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setEnabled((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleSetMaxMsg(OSCMessage *_msg) {
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setMaxPosition((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleSetMinMsg(OSCMessage *_msg) {
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
			setTargetPosition((int16_t)_msg->getArgInt32(0));
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


void handleStepIntervaMsg(OSCMessage *_msg)
{
	int16_t numArgs = _msg->getArgsNum();
	if(numArgs == 1)
	{
		if(_msg->getArgTypeTag(0) == kTagInt32)
		{
			setStepInterval((int16_t)_msg->getArgInt32(0));
		}
	}
}


