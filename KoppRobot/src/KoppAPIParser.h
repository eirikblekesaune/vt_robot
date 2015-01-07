#ifndef KOPP_API_PARSER_H
#define KOPP_API_PARSER_H

#include <Arduino.h>
#include "KoppRobot.h"
#include "KoppAPI.h"

class KoppAPIParser
{
	public:
		KoppAPIParser(KoppRobot* robot);
		void parseIncomingByte(byte inByte);

	private:
		KoppRobot *_robot;
		command_t _currentCommand;
		setGet_t _currentSetGet;
		char _dataByteBuffer[2];

		void _parseCommand(byte inByte);
		boolean _getDataBytes();
		int _parseDataValue();
};
#endif
