#ifndef SERVO_CITY_H
#define SERVO_CITY_H
#include <Arduino.h>
#include "ServoChannel.h"
#include "ServoChannelMappings.h"

class ServoCity {
public:
	ServoCity();
	void parseByte(uint8_t);
	static const uint8_t kHeaderByte;
	void Update();
private:
	enum{
		CMD_SET_POSITION,
		CMD_GOTO_POSITION,
		CMD_SET_DURATION,
		CMD_SET_ACCELERATION,
		CMD_SET_MIN,
		CMD_SET_MAX
	};
	enum{
		STATE_WAIT_HEADER,
		STATE_WAIT_CMD,
		STATE_WAIT_CHANNEL,
		STATE_WAIT_VALUE_LSB,
		STATE_WAIT_VALUE_MSB,
		STATE_WAIT_CR,
		STATE_WAIT_NL
	};
	ServoChannel* _channels[NUM_CHANNELS];
	int8_t _state;
	void _doCommand();
	uint8_t _currentCommand;
	uint8_t _currentChannel;
	uint16_t _currentValue;
};

#endif
