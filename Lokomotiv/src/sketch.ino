#include "Lokomotiv.h"
#include "LokomotivParser.h"
#include "LokomotivAPI.h"

Lokomotiv *lok;
LokomotivParser *parser;
unsigned long interval = 1000;
unsigned long lastTime;

void setup()
{	
	lok = new Lokomotiv();
	Serial.begin(9600);
	while(!Serial) {
		;
	}
	lastTime = millis();
	parser = new LokomotivParser(lok);
	lok->Init();
}

void loop()
{
	if(Serial.available() > 0)
	{
		parser->ParseByte(Serial.read());
	}
	lok->Update();
	if(millis() >= (interval + lastTime))
	{
		Serial.println(lastTime);
		lastTime = millis();
	}
}
