#include "PinneAPI.h"
#include "PinneRobot.h"
#include "PinneAPIParser.h"

PinneRobot robot;
PinneAPIParser parser(&robot);

void setup()
{ 
  Serial1.begin(9600);
  while(!Serial1);
  Serial1.println("STARTED");
  robot.init();
  DEBUG_PRINT("hei");
  robot.leftMotor->SetSpeed(0);
  robot.rightMotor->SetSpeed(0);
  //robot.rotationMotor->SetSpeed(0);
}
void loop()
{
  if(Serial1.available())
  {
    parser.parseIncomingByte(Serial1.read());
  }
  delay(1);
}
