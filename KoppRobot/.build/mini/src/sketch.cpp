#include <Arduino.h>
#include "KoppAPI.h"
#include "KoppRobot.h"
#include "KoppAPIParser.h"
void setup();
void loop();
#line 1 "src/sketch.ino"
//#include "KoppAPI.h"
//#include "KoppRobot.h"
//#include "KoppAPIParser.h"

extern const int LOOP_UPDATE_RATE = 10;
void setup()
{ 
  Serial1.begin(9600);
  while(!Serial1);
  delay(100);
  robot = new KoppRobot();
  parser = new KoppAPIParser(robot);
  robot->init();
}

void loop()
{
  if(Serial1.available())
  {
    parser->parseIncomingByte(Serial1.read());
  }
  robot->update();
  delay(LOOP_UPDATE_RATE);
}

