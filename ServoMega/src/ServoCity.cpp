#include "ServoCity.h"
const uint8_t ServoCity::kHeaderByte = 0xAA;
#define CR 13
#define NL 10

ServoCity::ServoCity(ServoChannel *channels) :
	_state(STATE_WAIT_HEADER),
{
	for(int i = 0; i < NUM_CHANNELS; i++)
	{
		channels[i] = new ServoChannel(channelToPinNumberMappings[i]);
	}
	
}

void ServoCity::parseByte(uint8_t newByte)
{
		switch(_state)
		{
		case STATE_WAIT_HEADER:
			_state = STATE_WAIT_CMD;
			break;
		case STATE_WAIT_CMD:
			_currentCommand = Serial.read();
			_state = STATE_WAIT_CHANNEL;
			break;
		case STATE_WAIT_CHANNEL:
			_currentChannel = Serial.read();
			_state = STATE_WAIT_VALUE_LSB;
			break;
		case STATE_WAIT_VALUE_LSB:
			_currentValue = Serial.read();
			_state = STATE_WAIT_VALUE_MSB;
			break;
		case STATE_WAIT_VALUE_MSB:
			_currentValue |= Serial.read() << 8;
			_state = STATE_WAIT_CR;
			break;
		case STATE_WAIT_CR:
			if(Serial.read() == CR)
			{
				_state = STATE_WAIT_NL;
			} else {
				_state = STATE_WAIT_HEADER;
			}
		case STATE_WAIT_NL:
			if(Serial.read() == NL)
			{
				_doCommand();
			} else {
				_state = STATE_WAIT_HEADER;
			}
		default:
			_state = STATE_WAIT_HEADER;
		}
}

void ServoCity::_doCommand()
{
	switch(_currentCommand)
	{
	case CMD_SET_POSITION:
		//_channels[_currentChannel - 1].SetPosition(_currentValue);
		Serial.print("pos");
		Serial.println(_currentValue);
		break;
	case CMD_GOTO_POSITION:
		//_channels[_currentChannel - 1].GoToPosition(_currentValue);
		Serial.print("goto");
		Serial.println(_currentValue);
		break;
	case CMD_SET_SPEED:
		//_channels[_currentChannel - 1].SetSpeed(_currentValue);
		Serial.print("speed");
		Serial.println(_currentValue);
		break;
	case CMD_SET_ACCELERATION:
		//_channels[_currentChannel - 1].SetAcceleration(_currentValue);
		Serial.print("accel");
		Serial.println(_currentValue);
		break;
	}
}
