#include "PinneRobot.h"
//#include <avr/eeprom.h>
//enum _eepromSlots {
//  EEPROM_LEFT_POSITION_SLOT,
//  EEPROM_LEFT_MIN_POSITION_SLOT,
//  EEPROM_LEFT_MAX_POSITION_SLOT,
//  EEPROM_RIGHT_POSITION_SLOT,
//  EEPROM_RIGHT_MIN_POSITION_SLOT,
//  EEPROM_RIGHT_MAX_POSITION_SLOT,
//  EEPROM_ROTATION_MIN_POSITION_SLOT,
//  EEPROM_ROTATION_MAX_POSITION_SLOT,
//  NUM_EEPROM_SLOTS  
//} eepromSlots;

//void writeByteToEEPROMSlot(int address, uint8_t dataByte)
//{
   //eeprom_write_byte((unsigned char *) address, dataByte);
//}

//uint8_t readByteFromEEPROMSlot(int address)
//{
  //return eeprom_read_byte((unsigned char *) address);
//}

//pin connections
//left driver aka motor 1, right driver motor 2
const int leftDriverPWM = 9;
const int leftDriverINA = 11;//Remapped due to interrupt
const int leftDriverINB = 4;
const int leftMotorSlackStopSensor = A5; //todo: cut trace, was current sense pin
const int leftDriverENDIAG = 6;
const int leftMotorEncoderInterruptIndex = 1;// digital pin 3 on Leonardo implicitly
const int leftMotorTopStopSensor = A0;

const int rightDriverPWM = 10;
const int rightDriverINA = 7;
const int rightDriverINB = 8;
const int rightMotorSlackStopSensor = 13;//todo cut trace, was current sense pin
const int rightDriverENDIAG = 12;
const int rightMotorEncoderInterruptIndex = 0;// digital pin 2 on Leonardo implicitly
const int rightMotorTopStopSensor = A1;

const int rotationDriver1A = A3;
const int rotationDriver2A = A2;
const int rotationDriverPWM = 5;
const int rotationPotmeterPin = A4;

PinneRobot::PinneRobot()
{
  VNH5019Driver *leftDriver = new VNH5019Driver(leftDriverINA, leftDriverINB, leftDriverENDIAG, leftDriverPWM);
  VNH5019Driver *rightDriver = new VNH5019Driver(rightDriverINA, rightDriverINB, rightDriverENDIAG, rightDriverPWM);
  L293Driver *rotationDriver = new L293Driver(rotationDriver1A, rotationDriver2A, rotationDriverPWM);
  leftMotor = new PinneMotor(leftMotorTopStopSensor, leftMotorSlackStopSensor, leftMotorEncoderInterruptIndex, leftDriver, ADDRESS_LEFT);
  rightMotor = new PinneMotor(rightMotorTopStopSensor, rightMotorSlackStopSensor, rightMotorEncoderInterruptIndex, rightDriver, ADDRESS_RIGHT);
  rotationMotor = new RotationMotor(rotationPotmeterPin, rotationDriver, ADDRESS_ROTATION);
}

void PinneRobot::init()
{
  leftMotor->init();
  rightMotor->init();
  rotationMotor->init();
}

void PinneRobot::update()
{
  leftMotor->UpdateState();
  rightMotor->UpdateState();
  rotationMotor->UpdateState();
}

void PinneRobot::GoToParkingPosition()
{
  leftMotor->GoToParkingPosition();
  rightMotor->GoToParkingPosition();
  rotationMotor->GoToParkingPosition();  
}

void PinneRobot::storeSettingsToEEPROM()
{
//  int i;
//  int value;
//  int slotSize = sizeof(int);
//  int currentSlotNum, currentDataByte;
  //verbose code but semantically clear.
//  noInterrupts();
//  
//  //Storing left motor data
//  value = leftMotor->GetCurrentPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    currentSlotNum = (EEPROM_LEFT_POSITION_SLOT * slotSize) + i;
//    currentDataByte = (value >> (i * 8)) & 0xFF;
//    writeByteToEEPROMSlot(currentSlotNum, currentDataByte);
//    DebugPrint("currentSlotNum");
//    DebugPrint(currentSlotNum);
//    DebugPrint("currentDataByte");
//    DebugPrint(currentDataByte);
//  }
//  
//  value = leftMotor->GetMinPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    DebugPrint((EEPROM_LEFT_MIN_POSITION_SLOT * slotSize) + i);
//    DebugPrint((value >> (i * 8)) & 0xFF);
//  }
//  
//  value = leftMotor->GetMaxPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    DebugPrint((EEPROM_LEFT_MAX_POSITION_SLOT * slotSize) + i);
//    DebugPrint((value >> (i * 8)) & 0xFF);
//  }
//  
//  
//  //Storing right motor data
//  value = rightMotor->GetCurrentPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    DebugPrint((EEPROM_RIGHT_POSITION_SLOT * slotSize) + i);
//    DebugPrint((value >> (i * 8)) & 0xFF);
//  }
//  
//  value = rightMotor->GetMinPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    DebugPrint((EEPROM_RIGHT_MIN_POSITION_SLOT * slotSize) + i);
//    DebugPrint((value >> (i * 8)) & 0xFF);
//  }
//  
//  value = rightMotor->GetMaxPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    DebugPrint((EEPROM_RIGHT_MAX_POSITION_SLOT * slotSize) + i);
//    DebugPrint((value >> (i * 8)) & 0xFF);
//  }
//  
//  //Storing rotation motor data  
//  value = rotationMotor->GetMinPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    DebugPrint((EEPROM_ROTATION_MIN_POSITION_SLOT * slotSize) + i);
//    DebugPrint((value >> (i * 8)) & 0xFF);
//  }
//  
//  value = rotationMotor->GetMaxPosition();
//  for(i = 0; i < slotSize; i++)
//  {
//    DebugPrint((EEPROM_ROTATION_MAX_POSITION_SLOT * slotSize) + i);
//    DebugPrint((value >> (i * 8)) & 0xFF);
//  }
//  interrupts();
//  DebugPrint("Settings written");
    DebugPrint("EEPROM writing not implemented");
}

void PinneRobot::loadSettingsFromEEPROM()
{
//  int i;
//  int currentSlotNum;
//  int currentValue;
//  int slotSize = sizeof(int);
//  DebugPrint("Loading seetings");
//  //noInterrupts();
//  for(i = 0; i < slotSize; i++)
//  {
//    currentSlotNum = (EEPROM_LEFT_POSITION_SLOT * slotSize) + i;
//    currentValue = (int)readByteFromEEPROMSlot(currentSlotNum) << (i * 8);
//    DebugPrint("Slotnum:");
//    DebugPrint(currentSlotNum);
//    DebugPrint("val:");
//  }
  //interrupts();
  DebugPrint("EEPROM reading not implemented");
}




