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
#include <ArdOSC.h>


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

const int positionPin = 0;
//speed bytes
#define STOP 0xF
#define FULL_STEP 0x0
#define HALF_STEP 0x1
#define QUARTER_STEP 0x2
#define EIGHT_STEP 0x3
#define SIXTEENTH_STEP 0x7

//parameter enum
enum param {
  kParamDirection,
  kParamSpeed,
  kParamEnable,
  kParamInterval
};

int potValue;
int currentSpeed = STOP;
unsigned long lastStepTime = 0;
unsigned long stepInterval = 3000;
unsigned long lastReadTime = 0;
unsigned long readInterval = 20;
#define UP 0
#define DOWN 1
#define MAX_POSITION 510
#define MIN_POSITION 30
#define WITHIN_RANGE 0
#define AT_MAX 1
#define AT_MIN 2
int state = WITHIN_RANGE;
int16_t currentDirection = UP;
int16_t enabled = 1;
long stepPos = 0;

void setup() {
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  for(int i = 0; i< 3; i++) {
    pinMode(msPins[i], OUTPUT);
  }
  
  //Init ethernet/OSC
  Ethernet.begin(mac, ip);
  server.begin(receivePort);
  server.addCallback("/robot/fugl/direction", &handleDirectionMsg);
  server.addCallback("/robot/fugl/speed", &handleSpeedMsg);
  server.addCallback("/robot/fugl/stepinterval", &handleStepIntervalMsg);
  server.addCallback("/robot/fugl/enable", &handleEnableMsg);
//  server.addCallback("/robot/fugl/setmax", &handleSetMax);
//  server.addCallback("/robot/fugl/setmin", &handleSetMin);
  
  //init serial
  Serial.begin(9600);
  digitalWrite(enablePin, LOW);
}

void loop() {
  if(server.aviableCheck()>0){
     //Serial.println("alive! "); 
  }
  if((millis() - lastReadTime) >= readInterval)
  {
    potValue = analogRead(positionPin);
    //Serial.println(potValue);
    if((potValue < MAX_POSITION) && (potValue > MIN_POSITION))
    {
      state = WITHIN_RANGE;
      //Serial.println("inrange");
    } else if(
            (potValue >= MAX_POSITION) && 
            (state != AT_MAX) &&
            (currentDirection == UP))
    {
      state = AT_MAX;
      currentSpeed = STOP;
      //Serial.println("atmax");
    } else if(
            (potValue <= MIN_POSITION) && 
            (state != AT_MIN) &&
            (currentDirection == DOWN))
    {
      state = AT_MIN;
      currentSpeed = STOP;
      //Serial.println("atmin");
    }
  }
  if(currentSpeed != STOP) {
    if((micros() - lastStepTime) >= stepInterval) {
      if(state == WITHIN_RANGE)
      {
        doStep();
      } else {
        if((currentDirection == UP) && (state == AT_MIN))
        {
          //Serial.println("a");
          doStep();
        } else if((currentDirection == DOWN) && (state == AT_MAX))
        {
          //Serial.println("b");
          doStep();
        }
      }
    }
  }
}

void doStep()
{
  digitalWrite(stepPin, LOW);
  //if(currentDirection == UP) {stepPos--;}
  //if(currentDirection == DOWN) {stepPos++;}
  delayMicroseconds(2);
  digitalWrite(stepPin, HIGH);
  lastStepTime = micros();
}

void handleDirectionMsg(OSCMessage *_msg) {
  int16_t numargs;
  Serial.println("got direction");
  numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagString) {
      int16_t argSize = _msg->getArgStringSize(0);
      char dirArg[argSize];
      _msg->getArgString(0, dirArg);
      setDirection(dirArg);
    }
  } else if(numargs == 0) {
    updateClient(_msg->getIpAddress(), kParamDirection);
  }
}

void handleSpeedMsg(OSCMessage *_msg) {
  Serial.println("got speed");
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setStepperSpeed((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleStepIntervalMsg(OSCMessage *_msg) {
  Serial.println("got interval");
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setStepInterval((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleEnableMsg(OSCMessage *_msg) {
  Serial.println("got enabled");
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setEnabled((int16_t)_msg->getArgInt32(0));
    }
  }
}

void handleSetMax(OSCMessage *_msg) {
  Serial.println("got set max");
  int16_t numargs = _msg->getArgsNum();
  if(numargs == 1) {
    if(_msg->getArgTypeTag(0) == kTagInt32) {
      setEnabled((int16_t)_msg->getArgInt32(0));
    }
  }
}

void setDirection(char *dir) {
  String dirArg = String(dir);
  if(dirArg == "up") { 
    currentDirection = UP;
    digitalWrite(dirPin, HIGH);
  }
  if(dirArg == "down") {
    currentDirection = DOWN;
    digitalWrite(dirPin, LOW);
  }
  Serial.print("set direction");
  Serial.println(currentDirection);
} 

void setStepperSpeed(int16_t spd) {
  int stepType;
  switch(spd) {
    case 0:
      stepType = STOP;
      break;
    case 1:
      stepType = SIXTEENTH_STEP;
      break;
    case 2:
      stepType = EIGHT_STEP;
      break;
    case 3:
      stepType = QUARTER_STEP;
      break;
    case 4:
      stepType = HALF_STEP;
      break;
    case 5:
      stepType = FULL_STEP;
      break;
  }
  if(currentSpeed != stepType) {
    currentSpeed = stepType;
    if(currentSpeed != STOP) PORTD = (PORTD & 0x1F) | (currentSpeed << 5);
  }
  Serial.print("set speed: ");
  Serial.println(currentSpeed);
}

void setEnabled(int16_t enb) {
  if(enb == 0) {
    enabled = 0;
    digitalWrite(enablePin, HIGH);
  } else if(enb == 1) {
    enabled = 1;
    digitalWrite(enablePin, LOW);
  }
  Serial.print("set enabled: ");
  Serial.println(enabled);
}

void setStepInterval(int16_t inr) {
  int16_t intr = constrain(inr, 100, 10000);
  stepInterval = intr;
  Serial.print("set interval: "); Serial.println(stepInterval);
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
