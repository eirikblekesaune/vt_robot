#ifndef LOKOMOTIV_PARSER_H
#define LOKOMOTIV_PARSER_H

#include <Arduino.h>
#include "Lokomotiv.h"
#include "LokomotivAPI.h"

enum {
	WAITING_FOR_COMMAND_BYTE,
	WAITING_FOR_VALUE_BYTE
};

class LokomotivAPIParser
{
	public:
		LokomotivAPIParser(Lokomotiv* robot);
		void parseIncomingByte(byte inByte);

	private:
		Lokomotiv *_robot;
		command_t _currentCommand;
		setGet_t _currentSetGet;
	dataTypeMark_t _currentDataType;
	int _state;

		void _executeCommand();
	long _decodeIntegerValue(uint32_t val);
	double _decodeDecimalValue(uint32_t val);
	void _reset();
		void _parseValueByte(byte inByte);
	uint32_t _data;
	int _numValueBytesReceived;
};
#endif

