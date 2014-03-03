#include "Lokomotiv.h"
#include "LokomotivAPI.h"
#include "LokomotivAPIParser.h"

Lokomotiv *lok;
LokomotivAPIParser *parser;
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
	parser = new LokomotivAPIParser(lok);
	lok->Init();
}

void loop()
{
	while(Serial.available() > 0)
	{
		parser->parseIncomingByte(Serial.read());
	}
	lok->Update();
	if(millis() >= (interval + lastTime))
	{
		lastTime = millis();
	}
}
