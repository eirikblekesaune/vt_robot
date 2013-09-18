#include "PinneAPI.h"
#include "PinneRobot.h"
#include "PinneAPIParser.h"

PinneRobot *robot;
PinneAPIParser *parser;


void setup()
{ 
  Serial1.begin(9600);
  while(!Serial1);
  delay(100);
  robot = new PinneRobot();
  parser = new PinneAPIParser(robot);
  robot->init();
//  robot->leftMotor->SetSpeed(0);//hmm not needed?
//  robot->rightMotor->SetSpeed(0);
//  robot->rotationMotor->SetSpeed(0);
}
void loop()
{
  if(Serial1.available())
  {
    parser->parseIncomingByte(Serial1.read());
  }
  robot->update();
  delay(10);
}
