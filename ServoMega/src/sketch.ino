//#include <Arduino.h>
#include "ServoCity.h"
//#include "ServoCityParser.h"

//ServoCity servoCity;
//ServoCityParser parser(servoCity);
ServoChannel *channels[NUM_CHANNELS];
ServoChannel *myChan;
typedef enum _parserState_t {
	WAIT_COMMAND,
	WAIT_CHANNEL,
	WAIT_DATA0,
	WAIT_DATA1,
	WAIT_CR,
	WAIT_LF
} parserState_t;

#define CMD_SET_POSITION 0
#define CMD_GOTO_POSITION 1
#define CMD_SET_SPEED 2
#define CMD_NONE 255
parserState_t state = WAIT_COMMAND;
int8_t currentCommand = CMD_NONE;
int8_t currentChannel;
uint16_t data;

void doCommand(int8_t command, int8_t channel, uint16_t value)
{
	Serial.print("c");
	Serial.print(command);
	Serial.print("h");
	Serial.print(channel);
	Serial.print("v");
	Serial.print(value);
	ServoChannel *myChan;
	myChan = channels[channel - 1];
	switch(command)
	{
		case CMD_SET_POSITION:
			myChan->SetPosition(data);	
			break;
		case CMD_GOTO_POSITION:
			myChan->GoToPosition(data, myChan->GetSpeed());
			break;
		case CMD_SET_SPEED:
			myChan->SetSpeed(data);
			break;
	}
}


void setup()
{
	for(int i = 0; i < NUM_CHANNELS; i++)
	{
		channels[i] = new ServoChannel(channelToPinNumberMappings[i]);
	}
	Serial.begin(9600);	
}


void loop()
{
	if(Serial.available() > 0)
	{
		switch(state)
		{
		case WAIT_COMMAND:
			currentCommand = Serial.read();
			state = WAIT_CHANNEL;
			break;
		case WAIT_CHANNEL:
			currentChannel = Serial.read();
			state = WAIT_DATA0;
			break;
		case WAIT_DATA0:
			data = Serial.read();
			state = WAIT_DATA1;
			break;
		case WAIT_DATA1:
			data |= Serial.read() << 8;
			doCommand(currentCommand, currentChannel, data);
			state = WAIT_COMMAND;
			break;
		default:
			state = WAIT_COMMAND;
		}
	}
	myChan->Update();
}
