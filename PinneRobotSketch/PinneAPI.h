#ifndef PINNE_API_H
#define PINNE_API_H

typedef enum command_t
{
  CMD_STOP = 0x00,
  CMD_SPEED = 0x01,
  CMD_DIRECTION = 0x02,
  CMD_TARGET_POSITION = 0x03,
  CMD_CURRENT_POSITION = 0x04,
  CMD_BRAKE = 0x05,
  CMD_STATE = 0x06,
  CMD_INFO = 0x07,//used for debugging, arbitrary numboer of asci characters
  CMD_STATE_CHANGE = 0x08,
  CMD_UNKNOWN
};

enum ADDRESSES { 
  ADDRESS_LEFT = 0x00, 
  ADDRESS_RIGHT = 0x10,
  ADDRESS_ROTATION = 0x20,
  ADDRESS_GLOBAL = 0x30,
  ADDRESS_UNKNOWN 
};
    
typedef enum setGet_t
{
  SET_MESSAGE = 0x00,
  GET_MESSAGE = 0x40,
  SETGET_UNKNOWN
};

typedef enum byteType_t
{
  BYTE_COMMAND = 0x80,
  BYTE_DATA = 0x00,
  BYTE_UNKNOWN
};

typedef enum parseMask_t {
  PARSE_MASK_MESSAGE_TYPE = 0x80,//command byte of data byte
  PARSE_MASK_SETGET = 0x40,//setter or getter
  PARSE_MASK_ADDRESS = 0x30,//which motor is address
  PARSE_MASK_COMMAND = 0x0F,
  PARSE_MASK_UNKNOWN
};

#endif
