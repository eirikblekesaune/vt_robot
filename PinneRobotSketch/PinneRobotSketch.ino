#include "PinneRobot.h"
#include "PinneAPIParser.h"

PinneRobot robot;

void setup(){
  Serial.begin(9600);
  while (!Serial);
  robot.init();
  Serial.println("online");
}

void loop(){
  if(Serial.available()) 
  {
    parseIncomingByte(Serial.read());
  }
  delay(10);
}
