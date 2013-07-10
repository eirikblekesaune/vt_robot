#ifndef PINNE_API_PARSER_H
#define PINNE_API_PARSER_H

#include <Arduino.h>
#include "PinneRobot.h"
#include "PinneAPI.h"

class PinneAPIParser
{
  public:
    PinneAPIParser(PinneRobot* robot);
    void parseIncomingByte(byte inByte);
    void Reply(const char* str);
    
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
