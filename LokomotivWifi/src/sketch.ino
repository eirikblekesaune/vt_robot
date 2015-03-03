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
	//Clear timer1 settings so that we know their settings.

	/*
	TCCR1A = 0x00;
	TCCR1B = 0x00;

	lok = new Lokomotiv();
	*/
	SERIAL.begin(57600);
	while(!SERIAL)
	{
	;
	}
	/*
	parser = new LokomotivAPIParser(lok);
	lok->Init();
	*/
}

void loop()
{
	/*
	while(SERIAL.available() > 0)
	{
	parser->parseIncomingByte(SERIAL.read());
	}
	lok->Update();
	*/

	double val = -12399.92;
	byte * out = (byte *) &val;
	delay(1000);
	SERIAL.print("/hei");
	SERIAL.write(COMMAND_STOP);
	SERIAL.write(0);
	SERIAL.write(0);
	SERIAL.write(0);
	
	SERIAL.print(",f");
	SERIAL.write(0);
	SERIAL.write(0);

	SERIAL.write(out[3]);
	SERIAL.write(out[2]);
	SERIAL.write(out[1]);
	SERIAL.write(out[0]);

	//Send a integer
	long intVal = 4321;
	out = (byte *) &intVal;

	delay(1000);
	SERIAL.print("/tof");
	SERIAL.write(0);
	SERIAL.write(0);
	SERIAL.write(0);
	SERIAL.write(0);
	
	SERIAL.print(",i");
	SERIAL.write(0);
	SERIAL.write(0);

	SERIAL.write(out[3]);
	SERIAL.write(out[2]);
	SERIAL.write(out[1]);
	SERIAL.write(out[0]);
}
