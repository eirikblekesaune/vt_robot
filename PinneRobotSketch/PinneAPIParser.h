#ifndef PINNE_API_PARSER_H
#define PINNE_API_PARSER_H

#include <Arduino.h>
typedef enum _pinneCommand_t
{
  CMD_STATE = 0x01,
  CMD_TARGET_POS = 0x02,
  CMD_POS_COUNTER = 0x04,
  CMD_SPEED = 0x08,
  CMD_ROT_POS = 0x05,
  CMD_ROT_SPEED = 0x0C,
  CMD_STOP = 0x00,
  CMD_NONE = 0xFF
} pinneCommand_t;


typedef enum _parserState
{
  WAITING_FOR_COMMAND_BYTE, 
  WAITING_FOR_VALUE
} parserState_t;

typedef enum _whichPinne
{
  LEFT,
  RIGHT
} whichPinne_t;

typedef enum _isFinalByte
{
  EXPECTING_MORE_BYTES,
  NOT_EXPECTING_MORE_BYTES
} isFinalByte_t;

typedef enum _messageType
{
  SET_MESSAGE,
  GET_MESSAGE
} messageType_t;

typedef enum _parsingMask {
  messageByteType = 0x80,
  commandHasMoreBytes = 0x04,
  whichPinne = 0x20,
  setOrGet = 0x10
} parsingMask_t;

typedef struct _PinneParser
{
  parserState_t state;    
  pinneCommand_t currentCmd;
} PinneParser;

//function prototypes
void parseCommand(byte inByte);
void parseIncomingByte(byte inByte);
void processStateMsg(byte inByte);
void processStateMsg(byte inByte);
void processTargetPositionMsg(byte inByte);
void processPositionCounterMsg(byte inByte);
void processSpeedMsg(byte inByte);
void processRotationPositionMsg(byte inByte);
void processRotationSpeedMsg(byte inByte);
void processStopMsg(byte inByte);

#endif
