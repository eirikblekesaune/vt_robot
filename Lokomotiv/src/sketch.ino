#define SERIAL Serial1
#include "Lokomotiv.h"
#include "LokomotivAPI.h"
#include "LokomotivAPIParser.h"

Lokomotiv *lok;
LokomotivAPIParser *parser;
unsigned long updateInterval = 10;
unsigned long lastTime;
unsigned long lastHeartbeat;

void setup()
{	
	//shut down the USB clock, which itnerferes with timer1
	//PRR1 |= (1<<PRUSB);
	//Clear timer1 settings
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	
	lok = new Lokomotiv();
	SERIAL.begin(9600);
	while(!SERIAL)
	{
		;
	}
	parser = new LokomotivAPIParser(lok);
	lok->Init();
}

void loop()
{
	while(SERIAL.available() > 0)
	{
		parser->parseIncomingByte(SERIAL.read());
	}
//	lok->Update();
//	if(millis() >= (updateInterval + lastTime))
//	{
//		lok->Update();
//		lastTime = millis();
//	}
}
