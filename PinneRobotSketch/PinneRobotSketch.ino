#include "PinneAPI.h"
#include "PinneRobot.h"
#include "PinneAPIParser.h"


PinneRobot robot;
PinneAPIParser parser(&robot);

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  robot.init();
  DEBUG_PRINT("hei");
}
void loop()
{
  if(Serial.available())
  {
    parser.parseIncomingByte(Serial.read());
  }
  delay(1);
}
