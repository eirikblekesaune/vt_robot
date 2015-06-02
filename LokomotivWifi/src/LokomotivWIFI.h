#ifndef LOKOMOTIV_WIFI_H
#define LOKOMOTIV_WIFI_H
#include <Arduino.h>
//This file contains all global definitions, macros, typedefs, enums etc.
#define SERIAL Serial1

const static byte kXBeeStartDelimiter = 0x7E;
const static byte kXBeeReceivePacketFrame = 0xB0;
const static byte kXBeeSendPacketFrame = 0x20;

static void DebugPrint(const char* msg);

enum command_t
{
	COMMAND_STOP = 'S',//speed fade out time as argument
	COMMAND_BIPOLAR_SPEED = 's',
	COMMAND_TARGET_POSITION = 't',
	COMMAND_DISTANCE_FROM_LAST_ADDRESS = 'F',
	COMMAND_PERIPHERAL = 'p',
	COMMAND_STATE = 'a',
	COMMAND_INFO = 'i',//used for debugging, arbitrary number of asci characters
	COMMAND_MEASURED_SPEED = 'm',
	COMMAND_LAST_DETECTED_ADDRESS = 'l',
	COMMAND_PID_P_VALUE = 'P',
	COMMAND_PID_I_VALUE = 'I',
	COMMAND_PID_D_VALUE = 'D',
	COMMAND_GLIDE_TO_SPEED = 'g',
	COMMAND_END_SPEED = 'e',
	COMMAND_PERIPHERAL_REQUEST = 'r',
	COMMAND_MOTOR_MODE = 'M',
	COMMAND_TRACKING_POLLING_INTERVAL = 'T',
	//COMMAND_PID_TARGET_SPEED = 'G',
	COMMAND_MIN_TARGET_SPEED = 'b',
	COMMAND_MAX_TARGET_SPEED = 'B',
	COMMAND_TRACKING_DATA = 'A',
	COMMAND_PARSER_ERROR = 'X',
	COMMAND_TRACKING_STATE = 'w',
	COMMAND_WHOAREYOU = 'H',
	COMMAND_CLOCK_CALIBRATION = 'x',
	COMMAND_UNKNOWN
};

enum setGet_t
{
	SET_MESSAGE = '>',
	GET_MESSAGE = '<',
	SETGET_UNKNOWN
};

enum oscTypeTag_t {
	TYPE_INTEGER = 'i',
	TYPE_DECIMAL = 'f',
	TYPE_STRING = 's',
	TYPE_NO_VALUE = 0,
	TYPE_UNKNOWN
};

enum direction_t {
	DIRECTION_FORWARD,
	DIRECTION_BACKWARDS
};

enum stateChange_t
{
	STATE_STOPPED,//Stopped manually
	STATE_GOING_FORWARD,//direction set to up
	STATE_GOING_BACKWARDS,//directiom set to down
	STATE_STOPPED_AT_TARGET,//
	STATE_STOPPING,
	STATE_INTERPOLATING_SPEED,
	NUM_STATES//Something is wrong with the driver itself
};

static void Reply(const char* str)
{
	//SERIAL.println(str);
}

static void FrameMessage(byte * msg, int numBytes) {
	static byte destinationIP[4] = {1, 2, 3, 150};
	//static byte destinationIP[4] = {10, 0, 0, 25};
	static byte destinationPort[2] = {0xDF, 0x20};//sclang port 57120
	static byte thisPort[2] = {0x26, 0x16};//the port num for the XBee
	int length = numBytes + 12;//there are 12 bytes in addition to the data being sent
	byte lengthBytes[2];
	int checksum = 0;
	lengthBytes[0] = static_cast<byte>(length >> 8);
	lengthBytes[1] = static_cast<byte>(length);
	int i;

	SERIAL.write(kXBeeStartDelimiter);//XBee start delimiter
	SERIAL.write(lengthBytes[0]); SERIAL.write(lengthBytes[1]); //The length bytes
	SERIAL.write(kXBeeSendPacketFrame); checksum += kXBeeSendPacketFrame;//frame send API byte
	SERIAL.write(0x00);//frame ID 0, to avoid confirm back to MCU, no need to add checksum
	SERIAL.write(destinationIP[0]); checksum += destinationIP[0];
	SERIAL.write(destinationIP[1]); checksum += destinationIP[1];
	SERIAL.write(destinationIP[2]); checksum += destinationIP[2];
	SERIAL.write(destinationIP[3]); checksum += destinationIP[3];
	SERIAL.write(destinationPort[0]); checksum += destinationPort[0];
	SERIAL.write(destinationPort[1]); checksum += destinationPort[1];
	SERIAL.write(thisPort[0]); checksum += thisPort[0];
	SERIAL.write(thisPort[1]); checksum += thisPort[1];
	SERIAL.write(0x00);//Send with UDP, no need to add to checksum when 0x00
	SERIAL.write(0x00);//Reserved/not used, no need to add to checksum when 0x0
	//Transmit the data
	for(i = 0; i < numBytes; i++) {
		SERIAL.write(msg[i]); checksum += msg[i];
	}
	//calculate the checksum
	SERIAL.write(0xFF - static_cast<byte>(checksum));
}

static void SendMsg(command_t command, oscTypeTag_t typeTag, byte * value)
{
	byte msg[16] = {
		47,116,47,//path start -> '/t/'
		command,//command byte
		SET_MESSAGE,//the setGet byte
		0,0,0,//OSC path padding
		44,typeTag,0,0,//type tag ',xnn'
		value[3], value[2], value[1], value[0]
	};

	FrameMessage(msg, 16);
}

static void SendTrackingDataMsg(int32_t position, int32_t time) {
	byte * posByte = (byte *) &position;
	byte * timestamp = (byte *) &time;
	byte msg[20] = {
		47,116,47,//path start -> '/t/t
		COMMAND_TRACKING_DATA,
		SET_MESSAGE,
		0,0,0,//OSC path padding
		44,105,105,0,//type tags for int and float
		posByte[3], posByte[2], posByte[1], posByte[0],
		timestamp[3], timestamp[2], timestamp[1], timestamp[0]
	};

	FrameMessage(msg, 20);
}

static void SendMsg(command_t command, const char * str)
{
	static int messageOffset = 12;
	//max string size is 244
	static byte msg[256] = {
		47,116,47,//path start -> '/t/'
		command,//command byte
		SET_MESSAGE,//the setGet byte
		0,0,0,//OSC path padding
		44,TYPE_STRING,0,0//type tag ',snn'
	};

	int n = 0;
	while(str[n] > '\0') {
		msg[n + messageOffset] = str[n];
		n++;
	}
	while((n % 32) != 0) {
		msg[n + messageOffset] = '\0';
		n++;
	}
	n += messageOffset;
	FrameMessage(msg, n);
}

static void SendMsg(command_t command, int32_t value)
{
	SendMsg(command, TYPE_INTEGER, (byte *) &value);
}

static void SendMsg(command_t command, double value)
{
	SendMsg(command, TYPE_DECIMAL, (byte *) &value);
}

static void SendMsg(command_t command, int16_t value)
{
	SendMsg(command, static_cast<int32_t>(value));
}

static void SendMsg(command_t command, int8_t value)
{
	SendMsg(command, static_cast<int32_t>(value));
}

static void SendMsg(command_t command, uint8_t value)
{
	SendMsg(command, static_cast<int32_t>(value));
}

static void SendMsg(command_t command, uint16_t value)
{
	SendMsg(command, static_cast<int32_t>(value));
}

static void SendMsg(command_t command, float value)
{
	SendMsg(command, static_cast<double>(value));
}

static void NotifyStateChange(stateChange_t stateChange)
{
	SendMsg(COMMAND_STATE, stateChange);
}

static void DebugUnitPrint(const char* msg)
{
	SendMsg(COMMAND_INFO, msg);
}

static void DebugUnitPrint(int msg)
{
	SendMsg(COMMAND_INFO, msg);
}

static void DebugPrint(const char* msg)
{
	SendMsg(COMMAND_INFO, msg);
}

static void DebugPrint(int msg)
{
	SendMsg(COMMAND_INFO, msg);
}

static void DebugPrint(float msg)
{
	SendMsg(COMMAND_INFO, msg);
}

static void DebugPrint(double msg)
{
	SendMsg(COMMAND_INFO, msg);
}

static void DebugPrint(int32_t msg)
{
	SendMsg(COMMAND_INFO, msg);
}

static void DebugPrint(uint32_t msg)
{
	SendMsg(COMMAND_INFO, static_cast<int32_t>(msg));
}

static void WhoAreYou() {
	SendMsg(COMMAND_WHOAREYOU, "figur1");
}


#endif
