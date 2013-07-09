#include "PinneAPIParser.h"

PinneParser parser = {WAITING_FOR_COMMAND_BYTE, CMD_NONE};

void parseIncomingByte(byte inByte)
{
  pinneCommand_t cmd;
  if(parser.state == WAITING_FOR_COMMAND_BYTE)
  {    
    Serial.println("Got command: ");
    parseCommand(inByte);
  } else if(parser.state == WAITING_FOR_VALUE)
  {
    Serial.print("Got value byte:"); Serial.println(inByte);
  }
}

void parseCommand(byte inByte)
{
  byte cmdByte;
  cmdByte = inByte & 0x0F;
  Serial.println("PROCESSING ");
  switch(cmdByte)
  {
    case CMD_STATE:
      processStateMsg(inByte);
      break;
    case CMD_TARGET_POS:
      processTargetPositionMsg(inByte);
      break;      
    case CMD_POS_COUNTER:
      processPositionCounterMsg(inByte);
      break;      
    case CMD_SPEED:
      processSpeedMsg(inByte);
      break;      
    case CMD_ROT_POS:
      processRotationPositionMsg(inByte);
      break;      
    case CMD_ROT_SPEED:
      processRotationSpeedMsg(inByte);
      break;      
    case CMD_STOP:
      processStopMsg(inByte);
      break;      
    default:
      Serial.print("CMD_NONE");
  }
}

void processStateMsg(byte inByte)
{
  Serial.println("STATE MSG");
}

void processTargetPositionMsg(byte inByte)
{
  Serial.println("TARGET_POSITION_MSG");
}

void processPositionCounterMsg(byte inByte)
{
  Serial.println("POSITION_COUNTER_MSG");
}

void processSpeedMsg(byte inByte)
{
  Serial.println("SPEED MSG");
}

void processRotationPositionMsg(byte inByte)
{
  Serial.println("ROTATION POSITION MSG");
}

void processRotationSpeedMsg(byte inByte)
{
  Serial.println("ROTATION SPEED MSG");
}

void processStopMsg(byte inByte)
{
  Serial.println("STOP MSG");
}
