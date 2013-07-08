//Bird sketch for Arduino Ethernet, 
//Pololu stepper driver DRV8825
//PWM DC Motor driver A3953SBT
//2 servo motors
//potentiometer for position sensing

#include <SPI.h>
#include <Ethernet.h>
#include <ArdOSC.h>
#include <Servo.h>

//Pin connections:
//DC driver: BRAKE pin
const int dcMotorBrakePin = A2;
//DC driver: PHASE pin
const int dcMotorPhasePin = A3;
//DC driver: MODE pin
const int dcMotorModePin = 2;
//DC driver: /ENABLE pin (PWM)
const int dcMotorEnablePin = 3;
//pin 4: NC
//servo motor 1: control
const int bowServoPin = 5;

//stepper enable pin
const int stepperEnablePin = 6;
//7: stepper driver: MS2 pin
//8: stepper driver: MS1 pin
//9: stepper driver: MS0 pin
const int stepperModePins[3] = {9, 8, 7};
//A0: potentiometer voltage
const int potmeterPin = A0;
//A1: NC
//A4: stepper driver: DIR pin
const int stepperDirectionPin = A4;
//A5: stepper driver: STEP pin
const int stepperStepPin = A5;

/*
Connected is a DRV8825 stepper driver from Pololu. 
Using the following pin connections:
ENABLE -xxxx- V+MOTOR
MS0    -xxxx- GND
MS1    -xxxx- 2B (Connected to stepper blue wire, coil D)
MS2    -xxxx- 2A (Connected to stepper red wire, coil B)
RESET  -xxxx- 1A (Connected to stepper black wire, coil A)
SLEEP  -xxxx- 1B (Connected to stepper green wire, coil C)
STEP   -xxxx- NC
DIR    -xxxx- GND
*/

/*
Connected is also a A3953 PWM halfbridge motor driver.
Using the following pin connections:
      /BRAKE -xxxx- LOAD SUPPLY
      REF    -xxxx- MOTOR OUT B
      RC     -xxxx- MODE
      GND    -xxxx- GND
      GND    -xxxx- GND
LOGIC SUPPLY -xxxx- NC (CURRENT SENSE)
      PHASE  -xxxx- MOTOR OUT A
      /ENABLE -xxxx- LOAD SUPPLY
*/



byte mac[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x67};
byte ip[] = {1, 2, 3, 117};
//OSC handler prototypes
void handleStretchDirMsg(OSCMessage *msg);
void handleStretchSpeedMsg(OSCMessage *msg);
void handleStretchRangeBoundsMsg(OSCMessage * msg);
void handleStretchStepperIntervalMsg(OSCMessage *msg);
void handleHeadTurnSpeedMsg(OSCMessage *msg);
void handleHeadTurnDirectionMsg(OSCMessage *msg);
void handleRotatePositionMsg(OSCMessage *msg);
void handleRotateSpeedMsg(OSCMessage *msg);
void handleBowPositionMsg(OSCMessage *msg);
void handleBowSpeedMsg(OSCMessage *msg);
void handleHeiMsg(OSCMessage * msg);



int receivePort = 50004;
int sendPort = 57120;
OSCServer server;
OSCClient client;

//stepper variables
int stepperStretchLimitOut = 0;
int stepperStretchLimitIn = 1023;
enum {STEPPER_IN, STEPPER_OUT};
enum {STEPPER_OFF, STEPPER_ON, STEPPER_AIMING};
unsigned long lastStepTime = 0;
unsigned long stepInterval = 1000;
int stepperDirection = STEPPER_OUT;
int stepperTarget = -1;//neg 1 means no target
int stepperPosition = 0;
int stepperState = STEPPER_OFF;

//These byte masks correspond to the MS pin values
typedef enum {
  FULL_STEP = 0x00,
  HALF_STEP = 0x04,
  QUARTER_STEP = 0x02,
  EIGTH_STEP = 0x06,
  SIXTEENTH_STEP = 0x01,
  THIRTYTWO_STEP = 0x05
} t_stepperSpeed;
t_stepperSpeed stepperSpeed = THIRTYTWO_STEP;


//servo motors
Servo bowServo;
Servo rotateServo;

//DC motor variables
int motorSpeed = 0;
enum {DC_RIGHT, DC_LEFT};
int motorDirection = DC_RIGHT;


void setup()
{
  //setup pins
  pinMode(dcMotorBrakePin, OUTPUT);
  pinMode(dcMotorPhasePin, OUTPUT);
  pinMode(dcMotorModePin, OUTPUT);
  pinMode(dcMotorEnablePin, OUTPUT);
  pinMode(bowServoPin, OUTPUT);
  pinMode(stepperEnablePin, OUTPUT);
  pinMode(stepperModePins[0], OUTPUT);
  pinMode(stepperModePins[1], OUTPUT);
  pinMode(stepperModePins[2], OUTPUT);
  pinMode(stepperDirectionPin, OUTPUT);
  pinMode(stepperStepPin, OUTPUT);
  
  //setup OSC stuff
  Ethernet.begin(mac, ip);
  server.begin(receivePort);
  server.addCallback("/stretch/direction", &handleStretchDirMsg);
  server.addCallback("/stretch/speed", &handleStretchSpeedMsg);
  server.addCallback("/stretch:range/bounds", &handleStretchRangeBoundsMsg);
  server.addCallback("/stretch:stepper/interval", &handleStretchStepperIntervalMsg);
  server.addCallback("/head/turn/speed", &handleHeadTurnSpeedMsg);
  server.addCallback("/head/turn/direction", &handleHeadTurnDirectionMsg);
  server.addCallback("/rotate/position", &handleRotatePositionMsg);
  server.addCallback("/rotate/speed", &handleRotateSpeedMsg);
  server.addCallback("/bow/position", &handleBowPositionMsg);
  server.addCallback("/bow/speed", &handleBowSpeedMsg);
  server.addCallback("/hei", &handleHeiMsg);
  
  //setup servo motors
  bowServo.attach(bowServoPin);
  
  //setup Serial stuff
  Serial.begin(9600);
  stopStepper();
}

void loop()
{
  if(server.aviableCheck()>0){
     Serial.println("alive! "); 
  }
  //read sensor value
  stepperPosition = analogRead(potmeterPin);
  
  ////update stepper state
  //stop if stepper is outside limits
  if((stepperPosition <= stepperStretchLimitOut) || (stepperPosition >= stepperStretchLimitIn)) 
  {
      stopStepper();
  } else if (stepperState == STEPPER_AIMING) {
    //check if stepper has reached target
    boolean targetReached = false;
    if(stepperDirection == STEPPER_OUT)
    {
      targetReached = stepperPosition >= stepperStretchLimitOut;
    } else {
      targetReached = stepperPosition <= stepperStretchLimitIn;
    }
    if(targetReached)
    {
      stopStepper();
    }
  }//end update stepper state
  
  //perform step if neccesary
  if(stepperState > STEPPER_OFF)
  {
    if((micros() - lastStepTime) >= stepInterval) {
      digitalWrite(stepperStepPin, LOW);
      delayMicroseconds(2);
      digitalWrite(stepperStepPin, HIGH);
      lastStepTime = micros();
    }
  }
}


void setStepperSpeed(int16_t speed);
void setStepperSpeed(int16_t speed) {
  switch(speed) {
    case 1:
      stepperSpeed = THIRTYTWO_STEP;
      break;
    case 2:
      stepperSpeed = SIXTEENTH_STEP;
      break;
    case 3:
      stepperSpeed = EIGTH_STEP;
      break;
    case 4:
      stepperSpeed = QUARTER_STEP;
      break;
    case 5:
      stepperSpeed = HALF_STEP;
      break;
    case 6:
      stepperSpeed = FULL_STEP;
      break;
    default:
      break;
  }
  if(speed == 0)
  {
    stopStepper();
  } else {
    for(int i = 0; i < 3; i++) 
    {
      int value = LOW;
      if((stepperSpeed & (1 << i)) > 0) { value = HIGH; }
      digitalWrite(stepperModePins[i], value);
      Serial.print("byte:"); Serial.print(i); Serial.print("= ");
      Serial.println(value);
    }
    Serial.print("stepspeed:");
    Serial.println(stepperSpeed);
    if(stepperState == STEPPER_OFF)
    {
      startStepper();
    }
  }
  
}

void stopStepper() {
  stepperState = STEPPER_OFF;
  digitalWrite(stepperEnablePin, HIGH);
  //always clear target when stopping
  stepperTarget = -1;
}

void startStepper() {
  stepperState = STEPPER_ON;
  digitalWrite(stepperEnablePin, LOW);
}

void aimStepper(int target) {
  stepperTarget = target;
  startStepper();
  stepperState = STEPPER_AIMING;
}

void setStepperDirection(int dir)
{
  stepperDirection = dir;
  digitalWrite(stepperDirectionPin, stepperDirection);
}

void startDC(int dir, int speed) {}
void stopDC() {}
void setDCDirection(int dir) {}
void turnDCDirection() {}

void setServo1Position() {}
void setServo1Speed() {}
void setServo2Position() {}
void setServo2Speed() {}

void handleStretchDirMsg(OSCMessage *msg)
{
  int16_t numargs;
  numargs = msg->getArgsNum();
  
  if(numargs == 1)
  {
    if(msg->getArgTypeTag(0) == kTagString)
    {
      int16_t argSize = msg->getArgStringSize(0);
      char dirArg[argSize];
      msg->getArgString(0, dirArg);
      String dir = String(dirArg);
      if(dir == "in") 
      {
        Serial.println("dir:out");
        setStepperDirection(STEPPER_IN);
      } else if(dir == "out")
      {
        setStepperDirection(STEPPER_OUT);
        Serial.println("dir:out");
      }
    }
  }
}



void handleStretchSpeedMsg(OSCMessage *msg)
{
  int16_t numargs = msg->getArgsNum();
  Serial.println("setSpeed");
  if(numargs == 1) {
    if(msg->getArgTypeTag(0) == kTagInt32) {
      setStepperSpeed((int16_t)msg->getArgInt32(0));
    }
  }
}

void handleStretchRangeBoundsMsg(OSCMessage *msg)
{
  int16_t numargs = msg->getArgsNum();
  Serial.println("setStretchRangeBounds");
  if(numargs == 2)
  {
    int16_t newLimitOut = (int16_t)msg->getArgInt32(0);
    int16_t newLimitIn = (int16_t)msg->getArgInt32(1);
    stepperStretchLimitOut = constrain(newLimitOut, 0, 1023);
    stepperStretchLimitIn = constrain(newLimitIn, 0, 1023);
  }
}

void handleStretchStepperIntervalMsg(OSCMessage *msg)
{
  int16_t numargs = msg->getArgsNum();
  if(numargs == 1)
  {
    int16_t newStepperInterval = msg->getArgInt32(0);
    stepInterval = (unsigned long)constrain(newStepperInterval, 10, 2550000);
    Serial.println("stepper interval:");
    Serial.println(stepInterval);
    
  }
}

void handleHeadTurnSpeedMsg(OSCMessage *msg) {}
void handleHeadTurnDirectionMsg(OSCMessage *msg) {}
void handleRotatePositionMsg(OSCMessage *msg) {}
void handleRotateSpeedMsg(OSCMessage *msg) {}
void handleBowPositionMsg(OSCMessage *msg) {}
void handleBowSpeedMsg(OSCMessage *msg) {}

void handleHeiMsg(OSCMessage * msg) 
{
  Serial.print("sensor:");
  Serial.println(stepperPosition);
}


