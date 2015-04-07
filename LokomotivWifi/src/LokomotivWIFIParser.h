#ifndef LOKOMOTIV_WIFI_PARSER_H
#define LOKOMOTIV_WIFI_PARSER_H

#include <Arduino.h>
#include "Lokomotiv.h"
#include "LokomotivWIFI.h"


enum {
	XBEE_CHECK_START_DELIMITER,
	XBEE_READ_LENGTH,
	XBEE_CHECK_API_FRAME_ID,
	XBEE_READ_SOURCE_IP,
	XBEE_READ_DESTINATION_PORT,
	XBEE_READ_SOURCE_PORT,
	XBEE_CHECK_PROTOCOL,
	XBEE_CHECK_STATUS,
	XBEE_READ_OSC_DATA,
	XBEE_READ_CHECKSUM
};

enum {
	OSC_CHECK_LEADING_SLASH,
	OSC_CHECK_T_CHAR,
	OSC_CHECK_SECOND_SLASH,
	OSC_READ_COMMAND_CHAR,
	OSC_READ_SETGET,
	OSC_CHECK_PATHPADS,
	OSC_CHECK_TYPETAG_COMMA,
	OSC_READ_TYPE_TAG,
	OSC_CHECK_TYPE_TAG_PADS,
	OSC_READ_VALUE_BYTES
};

typedef enum _oscDataParseError_t {
	PARSE_OK,
	PARSE_FAILED,
	PARSE_FAILED_NO_LEADING_SLASH,
	PARSE_FAILED_NO_T_CHAR,
	PARSE_FAILED_NO_SECOND_SLASH,
	PARSE_FAILED_WRONG_TYPETAG
} oscDataParseError_t;

class LokomotivWIFIParser
{
	public:
	LokomotivWIFIParser(Lokomotiv* robot);
	void parseIncomingByte(int incomingByte);

	private:
	Lokomotiv *_robot;
	command_t _currentCommand;
	setGet_t _currentSetGet;
	oscTypeTag_t _currentTypeTag;
	uint16_t _currentLength;
	uint32_t _currentIP;
	uint16_t _currentDestinationPort;
	uint16_t _currentSourcePort;
	uint16_t _currentExpectedDataLength;
	byte _data[256];
	int _state;
	int _oscDataParserState;

	void _executeCommand();
	int32_t _decodeIntegerValue(uint32_t val);
	double _decodeDecimalValue(uint32_t val);
	void _reset();
	void _parseValueByte(byte inByte);
	uint32_t _value;
	int _numValueBytesReceived;
	oscDataParseError_t _parseOSCData();
};
#endif

