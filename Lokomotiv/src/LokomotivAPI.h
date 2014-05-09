#ifndef LOKOMOTIV_API_H
#define LOKOMOTIV_API_H
#include <Arduino.h>
//This file contains all global definitions, macros, typedefs, enums etc.

static void DebugPrint(const char* msg);
enum command_t
{
  CMD_STOP = 0x00,//speed fade out time as argument
  CMD_SPEED = 0x01,
  CMD_DIRECTION = 0x02,
  CMD_TARGET_POSITION = 0x03,
  CMD_DISTANCE_FROM_LAST_ADDRESS = 0x04,
  CMD_LED = 0x05,
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
  PARSE_MASK_COMMAND = 0x0F,
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
  Serial.println(str);
}


static void ReturnGetValue(command_t command, long value)
{
  unsigned char data[5];

  data[0] = static_cast<unsigned char>(value >> 28) & 0x7F;//bits[31:28]
  data[1] = static_cast<unsigned char>(value >> 21) & 0x7F;//bits[27:21]
  data[2] = static_cast<unsigned char>(value >> 14) & 0x7F;//bits[20:14]
	data[3] = static_cast<unsigned char>(value >> 7) & 0x7F;//bits[13:8]
	data[4] = static_cast<unsigned char>(value & 0x7F);//bits[7:0]
  Serial.write(BYTE_COMMAND | SET_MESSAGE | command);
  Serial.write(data[0] & 0x0F);
  Serial.write(data[1]);
  Serial.write(data[2]);
  Serial.write(data[3]);
  Serial.write(data[4]);//forces four upper bits to zero for cmd byte and int sign
	Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_END_TRANSMISSION);
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
  Serial.write(BYTE_COMMAND | SET_MESSAGE | command);
  Serial.write(data[0] | TYPE_DECIMAL);
  Serial.write(data[1]);
  Serial.write(data[2]);
  Serial.write(data[3]);
  Serial.write(data[4]);//forces four upper bits to zero for cmd byte and int sign
	Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_END_TRANSMISSION);
}

static void ReturnGetValue(command_t command, float value)
{
	ReturnGetValue(command, static_cast<double>(value));
}

static void NotifyStateChange(stateChange_t stateChange)
{
  Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_STATE);
  Serial.write(stateChange);
}

static void DebugUnitPrint(const char* msg)
{
  Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
  Serial.print(msg);
  Serial.write(4);
}

static void DebugUnitPrint(int msg)
{
  Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
  Serial.print(msg);
  Serial.write(4);
}

static void DebugPrint(const char* msg)
{
  Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
  Serial.print(msg);
  Serial.write(4);
}

static void DebugPrint(int msg)
{
  Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
  Serial.print(msg);
  Serial.write(4);
}

static void DebugPrint(float msg)
{
  Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
  Serial.print(msg);
  Serial.write(4);
}

static void DebugPrint(long msg)
{
  Serial.write(BYTE_COMMAND | SET_MESSAGE | CMD_INFO );
  Serial.print(msg);
  Serial.write(4);
}


#define DEBUG
#ifdef DEBUG 
#define DEBUG_PRINT(x) DebugPrint(x)
#define DEBUG_NL
#else
#define DEBUG_PRINT(x)
#define DEBUG_NL
#endif


#endif
