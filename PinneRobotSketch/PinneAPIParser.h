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
    void _processSetDirectionCommand();
    void _processSetTargetPositionCommand();
    void _processSetCurrentPositionCommand();
    void _processSetBrakeCommand();
    void _processSetMinPositionCommand();
    void _processSetMaxPositionCommand();
    void _processGetStateCommand();
    
    void _processGetSpeedCommand();    
    void _processGetDirectionCommand();
    void _processGetTargetPositionCommand();
    void _processGetCurrentPositionCommand();
    void _processGetBrakeCommand();
    void _processGetMinPositionCommand();
    void _processGetMaxPositionCommand();
    
    void _processGoToTargetCommand();
    void _processPValueCommand();
    void _processIValueCommand();
    void _processDValueCommand();
};
#endif
