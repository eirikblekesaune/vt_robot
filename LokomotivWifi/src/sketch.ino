#define SERIAL Serial1
#include "Lokomotiv.h"
#include "LokomotivWIFI.h"
#include "LokomotivWIFIParser.h"

Lokomotiv *lok;
LokomotivWIFIParser *parser;
unsigned long updateInterval = 10;
unsigned long lastTime;
unsigned long lastHeartbeat;

void setup()
{
	//shut down the USB clock, which itnerferes with timer1
	//PRR1 |= (1<<PRUSB);
	//Clear timer1 settings so that we know their settings.

	
	TCCR1A = 0x00;
	TCCR1B = 0x00;

	lok = new Lokomotiv();
	
	SERIAL.begin(57600);
	while(!SERIAL)
	{
	;
	}
	
	parser = new LokomotivWIFIParser(lok);
	lok->Init();
	
	lastTime = millis();
}

void loop()
{
//	if((millis() - lastTime) > 1000) {
//		SendMsg(COMMAND_SPEED, "beat");
//		lastTime = millis();
//	}
	
	while(SERIAL.available() > 0)
	{
		int inByte;
		//SendMsg(COMMAND_INFO, SERIAL.read());

		parser->parseIncomingByte(SERIAL.read());
	}
	
	lok->Update();

}
