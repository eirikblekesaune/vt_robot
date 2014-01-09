#include "Lokomotiv.h"
#include "LokomotivParser.h"
#include "LokomotivAPI.h"

LokomotivParser *parser;

void setup()
{	
	Lokomotiv *lok = new Lokomotiv();
	Serial.begin(9600);
	parser = new LokomotivParser(lok);
}

void loop()
{
	if(Serial.available() > 0)
	{
		parser->ParseByte(Serial.read());
	}
	lok->Update();
}
