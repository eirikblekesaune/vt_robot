#include "PinneAPI.h"
#include "PinneRobot.h"
#include "PinneAPIParser.h"

PinneRobot *robot;
PinneAPIParser *parser;


void setup()
{ 
  Serial1.begin(9600);
  while(!Serial1);
  
//  Serial.begin(9600);
//  while(!Serial);

  Serial1.println("STARTED");
  Serial.println("USB COMM");
  delay(100);
  robot = new PinneRobot();
  parser = new PinneAPIParser(robot);
  robot->init();
  DEBUG_PRINT("hei");
  robot->leftMotor->SetSpeed(0);
  robot->rightMotor->SetSpeed(0);
  //robot.rotationMotor->SetSpeed(0);
}
void loop()
{
  if(Serial1.available())
  {
    parser->parseIncomingByte(Serial1.read());
  }
  robot->update();
  delay(300);
}
