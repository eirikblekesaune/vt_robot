#include "PinneRobot.h"
#include "PinneAPIParser.h"

PinneRobot robot;
PinneParser parser = {
  WAITING_FOR_COMMAND_BYTE, 
  CMD_UNKNOWN,
  SETGET_UNKNOWN,
  ADDRESS_UNKNOWN,
  &robot,
  {0x00, 0x00}
};


void setup(){
  Serial.begin(115200);
  Serial.setTimeout(100);
  while (!Serial);
  robot.init();
  Serial.println("online");
}

void loop(){
  if(Serial.available() > 0)
  {
    parseIncomingByte(Serial.read());
  }
  delay(10);
}
