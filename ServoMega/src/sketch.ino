//#include <Arduino.h>
#include "ServoChannel.h"
#include "ServoCity.h"

ServoCity *city;

void setup()
{
	city = new ServoCity();
	Serial.begin(9600);	
}


void loop()
{
	if(Serial.available() > 0)
	{
		city->parseByte(Serial.read());
	}
	city->Update();
}
