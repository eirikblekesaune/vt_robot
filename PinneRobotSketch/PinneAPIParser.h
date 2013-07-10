#ifndef PINNE_API_PARSER_H
#define PINNE_API_PARSER_H

#include <Arduino.h>
#include "PinneRobot.h"


class PinneAPIParser
{
  public:
    PinneAPIParser(PinneRobot* robot);
    enum command_t
    {
      CMD_STOP = 0x00,
      CMD_SPEED = 0x01,
      CMD_DIRECTION = 0x02,
      CMD_TARGET_POSITION = 0x03,
      CMD_CURRENT_POSITION = 0x04,
      CMD_BRAKE = 0x05,
      CMD_STATE = 0x06,
      CMD_UNKNOWN
    };
    enum address_t { ADDRESS_LEFT = 0x00, ADDRESS_RIGHT = 0x10, ADDRESS_UNKNOWN };
    
    typedef enum setGet_t
    {
      SET_MESSAGE = 0x00,
      GET_MESSAGE = 0x40,
      SETGET_UNKNOWN
    };
    
    typedef enum parseMask_t {
      PARSE_MASK_MESSAGE_TYPE = 0x80,//command byte of data byte
      PARSE_MASK_SETGET = 0x40,//setter or getter
      PARSE_MASK_ADDRESS = 0x30,//which motor is address
      PARSE_MASK_COMMAND = 0x0F,
      PARSE_MASK_UNKNOWN
    };

    void parseIncomingByte(byte inByte);
    
  private:
    PinneRobot *_robot;
    command_t _currentCommand;
    setGet_t _currentSetGet;
    address_t _currentAddress;
    char _dataByteBuffer[2];
    
    void _parseCommand(byte inByte);
    boolean _getDataBytes();
    int _parseDataValue();
    void _processSetStopCommand();
    void _processSetSpeedCommand();
    void _processGetSpeedCommand();
    void _processSetDirectionCommand();
    void _processGetDirectionCommand();
    void _processSetTargetPositionCommand();
    void _processGetTargetPositionCommand();
    void _processSetCurrentPositionCommand();
    void _processGetCurrentPositionCommand();
    void _processSetBrakeCommand();
    void _processGetBrakeCommand();
    void _processGetStateCommand();

};
#endif
