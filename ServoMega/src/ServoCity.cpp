#include "ServoCity.h"
const uint8_t ServoCity::kHeaderByte = 0xAA;
#define CR 13
#define NL 10

ServoCity::ServoCity() :
	_state(STATE_WAIT_HEADER)
{
	for(int i = 0; i < NUM_CHANNELS; i++)
	{
		_channels[i] = new ServoChannel(channelToPinNumberMappings[i], i + 1);
	}
}

void ServoCity::Update()
{
	for(int i = 0; i < NUM_CHANNELS; i++)
	{
		_channels[i]->Update();
	}
}

void ServoCity::parseByte(uint8_t newByte)
{
		switch(_state)
		{
		case STATE_WAIT_HEADER:
			if(newByte == kHeaderByte)
				_state = STATE_WAIT_CMD;
			break;
		case STATE_WAIT_CMD:
			_currentCommand = newByte;
			_state = STATE_WAIT_CHANNEL;
			break;
		case STATE_WAIT_CHANNEL:
			_currentChannel = newByte;
			_state = STATE_WAIT_VALUE_MSB;
			break;
		case STATE_WAIT_VALUE_MSB:
			_currentValue = newByte << 8; 
			_state = STATE_WAIT_VALUE_LSB;
			break;
		case STATE_WAIT_VALUE_LSB:
			_currentValue |= newByte;
			_state = STATE_WAIT_CR;
			break;
		case STATE_WAIT_CR:
			if(newByte == CR)
			{
				_state = STATE_WAIT_NL;
			} else {
				_state = STATE_WAIT_HEADER;
			}
			break;
		case STATE_WAIT_NL:
			if(newByte == NL)
			{
				_doCommand();
			}
			_state = STATE_WAIT_HEADER;
			break;
		default:
			_state = STATE_WAIT_HEADER;
		}
}

void ServoCity::_doCommand()
{
	switch(_currentCommand)
	{
	case CMD_SET_POSITION:
		_channels[_currentChannel - 1]->SetPosition(_currentValue);
		break;
	case CMD_GOTO_POSITION:
		_channels[_currentChannel - 1]->GoToPosition(_currentValue);
		break;
	case CMD_SET_DURATION:
		_channels[_currentChannel - 1]->SetDuration(_currentValue);
		break;
	case CMD_SET_ACCELERATION:
		_channels[_currentChannel - 1]->SetAcceleration(_currentValue);
		break;
	case CMD_SET_MIN:
		_channels[_currentChannel - 1]->SetMin(_currentValue);
		break;
	case CMD_SET_MAX:
		_channels[_currentChannel - 1]->SetMax(_currentValue);
		break;
	}
}
