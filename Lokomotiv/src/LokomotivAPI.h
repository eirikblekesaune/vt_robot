#ifndef LOKOMOTIV_API_H
#define LOKOMOTIV_API_H
#include <Arduino.h>
//This file contains all global definitions, macros, typedefs, enums etc.
#define SERIAL Serial1
static void DebugPrint(const char* msg);
enum command_t
{
	CMD_STOP = 0x00,//speed fade out time as argument
	CMD_SPEED = 0x01,
	CMD_DIRECTION = 0x02,
	CMD_TARGET_POSITION = 0x03,
	CMD_DISTANCE_FROM_LAST_ADDRESS = 0x04,
	CMD_PERIPHERAL = 0x05,
	CMD_STATE = 0x06,
	CMD_INFO = 0x07,//used for debugging, arbitrary numboer of asci characters
	CMD_MEASURED_SPEED = 0x08,
	CMD_LAST_DETECTED_ADDRESS = 0x09,
	CMD_PID_P_VALUE = 0x0A,
	CMD_PID_I_VALUE = 0x0B,
	CMD_PID_D_VALUE = 0x0C,
	CMD_GLIDE_TO_SPEED = 0x0D,
	CMD_END_SPEED = 0x0E,
	CMD_END_TRANSMISSION = 0x0F,
	CMD_PERIPHERAL_REQUEST = 0x10,
	CMD_MOTOR_MODE = 0x11,
	CMD_TRACKING_POLLING_INTERVAL = 0x12,
	CMD_PID_TARGET_SPEED = 0x13,
	CMD_TRACKING_DATA = 0x14,
	CMD_UNKNOWN
};

enum byteType_t
{
	BYTE_COMMAND = 0x80,
	BYTE_DATA = 0x00,
	BYTE_UNKNOWN
};

enum setGet_t
{
	SET_MESSAGE = 0x00,
	GET_MESSAGE = 0x40,
	SETGET_UNKNOWN
};

enum parseMask_t {
	PARSE_MASK_MESSAGE_TYPE = 0x80,//command byte of data byte
	PARSE_MASK_SETGET = 0x40,//setter or getter
	PARSE_MASK_COMMAND = 0x1F,
	PARSE_MASK_UNKNOWN
};
#define PARSE_MASK_DATA_TYPE 0x40

enum dataTypeMark_t {
	TYPE_INTEGER = 0x00,
	TYPE_DECIMAL = 0x40,
	TYPE_UNKNOWN
};

enum stateChange_t
{
	STOPPED,//Stopped manually
	GOING_FORWARD,//direction set to up
	GOING_BACKWARDS,//directiom set to down
	STOPPED_AT_TARGET,//
	DETECTED_ADDRESS,
	DRIVER_FAULT//Something is wrong with the driver itself
};

static void Reply(const char* str)
{
	SERIAL.println(str);
}

static void ReturnGetValue(command_t command, long value)
{
	unsigned char data[5];

	data[0] = static_cast<unsigned char>(value >> 28) & 0x7F;//bits[31:28]
	data[1] = static_cast<unsigned char>(value >> 21) & 0x7F;//bits[27:21]
	data[2] = static_cast<unsigned char>(value >> 14) & 0x7F;//bits[20:14]
	data[3] = static_cast<unsigned char>(value >> 7) & 0x7F;//bits[13:8]
	data[4] = static_cast<unsigned char>(value & 0x7F);//bits[7:0]
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | command);
	SERIAL.write(data[0] & 0x0F);
	SERIAL.write(data[1]);
	SERIAL.write(data[2]);
	SERIAL.write(data[3]);
	SERIAL.write(data[4]);//forces four upper bits to zero for cmd byte and int sign
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_END_TRANSMISSION);
}

static void ReturnGetValue(command_t command, int value)
{
	ReturnGetValue(command, static_cast<long>(value));
}

static void ReturnGetValue(command_t command, double value)
{
	unsigned char data[5];
	uint32_t numberToSend;
	numberToSend = static_cast<int32_t>(value * 10000.0);
	data[0] = static_cast<unsigned char>(numberToSend >> 28) & 0x0F;//bits[31:28]
	data[1] = static_cast<unsigned char>(numberToSend >> 21) & 0x7F;//bits[27:21]
	data[2] = static_cast<unsigned char>(numberToSend >> 14) & 0x7F;//bits[20:14]
	data[3] = static_cast<unsigned char>(numberToSend >> 7) & 0x7F;//bits[13:8]
	data[4] = static_cast<unsigned char>(numberToSend) & 0x7F;//bits[7:0]
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | command);
	SERIAL.write(data[0] | TYPE_DECIMAL);
	SERIAL.write(data[1]);
	SERIAL.write(data[2]);
	SERIAL.write(data[3]);
	SERIAL.write(data[4]);//forces four upper bits to zero for cmd byte and int sign
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_END_TRANSMISSION);
}

static void ReturnGetValue(command_t command, float value)
{
	ReturnGetValue(command, static_cast<double>(value));
}

static void NotifyStateChange(stateChange_t stateChange)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_STATE);
	SERIAL.write(stateChange);
}

static void DebugUnitPrint(const char* msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}

static void DebugUnitPrint(int msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}

static void DebugPrint(const char* msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}

static void DebugPrint(int msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}

static void DebugPrint(float msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}

static void DebugPrint(double msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}

static void DebugPrint(int32_t msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}

static void DebugPrint(uint32_t msg)
{
	SERIAL.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
	SERIAL.print(msg);
	SERIAL.write(4);
}


#endif
