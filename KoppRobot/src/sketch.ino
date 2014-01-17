//#include "KoppAPI.h"
//#include "KoppRobot.h"
//#include "KoppAPIParser.h"
//
//KoppRobot *robot;
//KoppAPIParser *parser;

//extern const int LOOP_UPDATE_RATE = 1000;

void setup()
{ 
  Serial.begin(9600);
  while(!Serial) { 
		; 
	};
//  delay(100);
//  robot = new KoppRobot();
//  parser = new KoppAPIParser(robot);
//  robot->init();
}

void loop()
{
  //if(Serial.available())
  //{
  //  parser->parseIncomingByte(Serial.read());
  //}
  //robot->update();
  delay(1000);
	Serial.println("Hellof");
}

