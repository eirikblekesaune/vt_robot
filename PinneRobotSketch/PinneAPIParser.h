#ifndef PINNE_API_PARSER_H
#define PINNE_API_PARSER_H

#include <Arduino.h>
#include "PinneRobot.h"

typedef enum _command_t
{
  CMD_STOP = 0x00,
  CMD_SPEED = 0x01,
  CMD_DIRECTION = 0x02,
  CMD_TARGET_POSITION = 0x03,
  CMD_CURRENT_POSITION = 0x04,
  CMD_BRAKE = 0x05,
  CMD_STATE = 0x06,
  CMD_UNKNOWN
} command_t;

typedef enum _parserState
{
  WAITING_FOR_COMMAND_BYTE, 
  WAITING_FOR_DATA_BYTE,
  PARSER_STATE_UNKNOWN
} parserState_t;

typedef enum _address_t
{
  ADDRESS_LEFT = 0x00,
  ADDRESS_RIGHT = 0x10,
  ADDRESS_ROTATION = 0x20,
  ADDRESS_GLOBAL = 0x30,
  ADDRESS_UNKNOWN
} address_t;

typedef enum _setGet_t
{
  SET_MESSAGE = 0x00,
  GET_MESSAGE = 0x40,
  SETGET_UNKNOWN
} setGet_t;

typedef enum _parseMask {
  PARSE_MASK_MESSAGE_TYPE = 0x80,//command byte of data byte
  PARSE_MASK_SETGET = 0x40,//setter or getter
  PARSE_MASK_ADDRESS = 0x30,//which motor is address
  PARSE_MASK_COMMAND = 0x0F,
  PARSE_MASK_UNKNOWN
} parseMask_t;

typedef struct _PinneParser
{
  parserState_t state;    
  command_t currentCommand;
  setGet_t currentSetGet;
  address_t currentAddress;
  PinneRobot *robot;
  char dataByteBuffer[2];
} PinneParser;

//function prototypes
void parseCommand(byte inByte);
void parseIncomingByte(byte inByte);
boolean getDataBytes();
int parseDataValue();
void processSetStopCommand();
void processSetSpeedCommand();
void processGetSpeedCommand();
void processSetDirectionCommand();
void processGetDirectionCommand();
void processSetTargetPositionCommand();
void processGetTargetPositionCommand();
void processSetCurrentPositionCommand();
void processGetCurrentPositionCommand();
void processSetBrakeCommand();
void processGetBrakeCommand();
void processGetStateCommand();


#endif
