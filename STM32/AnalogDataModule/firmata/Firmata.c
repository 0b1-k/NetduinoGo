#include "Firmata.h"
#include "usart.h"
#include "timeouts.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "LedController.h"

// firmware name and version
static uint8_t  firmwareVersionCount = 0;
static char     firmwareVersionVector[MAX_DATA_BYTES];

// input message handling
static uint8_t waitForData = 0; // this flag says the next serial input will be data
static uint8_t executeMultiByteCommand = 0; // execute this after getting multi-byte data
static uint8_t multiByteChannel = 0; // channel data for multiByteCommands
static uint8_t storedInputData[MAX_DATA_BYTES]; // multi-byte data

// sysex
static uint8_t parsingSysex;
static int sysexBytesRead;

// callback functions
static callbackFunction                 currentAnalogCallback = NULL;
static callbackFunction                 currentDigitalCallback = NULL;
static callbackFunction                 currentReportAnalogCallback = NULL;
static callbackFunction                 currentReportDigitalCallback = NULL;
static callbackFunction                 currentPinModeCallback = NULL;
static systemResetCallbackFunction      currentSystemResetCallback = NULL;
static stringCallbackFunction           currentStringCallback = NULL;
static sysexCallbackFunction            currentSysexCallback = NULL;

// Support Functions
void sendValueAsTwo7bitBytes(uint16_t value){
  usartPut(value & 0x7F); // LSB
  usartPut((value >> 7) & 0x7F); // MSB
}
void startSysex(void){
  usartPut(START_SYSEX);
}
void endSysex(void){
  usartPut(END_SYSEX);
}
void processSysexMessage(){
  switch(storedInputData[0]){ //first byte in buffer is command
  case REPORT_FIRMWARE:
    printFirmwareVersion();
    break;
  case STRING_DATA:
    if(currentStringCallback) {
      uint8_t bufferLength = (sysexBytesRead - 1) / 2;
      char *buffer = (char*)malloc(bufferLength * sizeof(char));
      uint8_t i = 1;
      uint8_t j = 0;
      while(j < bufferLength) {
        buffer[j] = (char)storedInputData[i];
        i++;
        buffer[j] += (char)(storedInputData[i] << 7);
        i++;
        j++;
      }
      (*currentStringCallback)(buffer);
    }
    break;
  default:
    if(currentSysexCallback)
      (*currentSysexCallback)(storedInputData[0], sysexBytesRead - 1, storedInputData + 1);
  }
}
void systemReset(){
  waitForData = 0; // this flag says the next serial input will be data
  executeMultiByteCommand = 0; // execute this after getting multi-byte data
  multiByteChannel = 0; // channel data for multiByteCommands

  for(uint8_t i = 0; i < MAX_DATA_BYTES; i++) {
    storedInputData[i] = 0;
  }

  parsingSysex = false;
  sysexBytesRead = 0;

  if(currentSystemResetCallback){
    (*currentSystemResetCallback)();
  }
}
void pin13strobe(int count, int onInterval, int offInterval){
  for(int i = 0; i < count; i++) {
    SleepMilliseconds(offInterval);
    LedControllerSet(Digital, 0, Bit_SET);
    SleepMilliseconds(onInterval);
    LedControllerSet(Digital, 0, Bit_RESET);
  }
}
// Arduino constructors
void begin(long baudrate){
  initUSART(57600);
  blinkVersion();
  printVersion();
  printFirmwareVersion();
}
// querying functions
void printVersion(){
  usartPut(REPORT_VERSION);
  usartPut(FIRMATA_MAJOR_VERSION);
  usartPut(FIRMATA_MINOR_VERSION);
}
void blinkVersion(){
  pin13strobe(FIRMATA_MAJOR_VERSION, 40, 210);
  SleepMilliseconds(250);
  pin13strobe(FIRMATA_MINOR_VERSION, 40, 210);
  SleepMilliseconds(125);
}
void printFirmwareVersion(){
  if(firmwareVersionCount) { // make sure that the name has been set before reporting
    startSysex();
    usartPut(REPORT_FIRMWARE);
    usartPut(firmwareVersionVector[0]); // major version number
    usartPut(firmwareVersionVector[1]); // minor version number
    for(uint8_t i = 2; i < firmwareVersionCount; ++i) {
      sendValueAsTwo7bitBytes(firmwareVersionVector[i]);
    }
    endSysex();
  }
}
//void setFirmwareVersion(byte major, byte minor);  // see macro below
void setFirmwareNameAndVersion(const char *name, uint8_t major, uint8_t minor){
  snprintf(firmwareVersionVector, MAX_DATA_BYTES, "%c%c%s", major, minor, name);
}
// serial receive handling
int available(){
  return usartTest();
}
void processInput(){
  int inputData = usartGet(); // this is 'int' to handle -1 when no data
  int command;

  // TODO make sure it handles -1 properly

  if (parsingSysex) {
    if(inputData == END_SYSEX) {
      //stop sysex byte
      parsingSysex = false;
      //fire off handler function
      processSysexMessage();
    } else {
      //normal data byte - add to buffer
      storedInputData[sysexBytesRead] = inputData;
      sysexBytesRead++;
    }
  } else if( (waitForData > 0) && (inputData < 128) ) {
    waitForData--;
    storedInputData[waitForData] = inputData;
    if( (waitForData==0) && executeMultiByteCommand ) { // got the whole message
      switch(executeMultiByteCommand) {
      case ANALOG_MESSAGE:
        if(currentAnalogCallback) {
          (*currentAnalogCallback)(multiByteChannel,
                                   (storedInputData[0] << 7)
                                   + storedInputData[1]);
        }
        break;
      case DIGITAL_MESSAGE:
        if(currentDigitalCallback) {
          (*currentDigitalCallback)(multiByteChannel,
                                    (storedInputData[0] << 7)
                                    + storedInputData[1]);
        }
        break;
      case SET_PIN_MODE:
        if(currentPinModeCallback)
          (*currentPinModeCallback)(storedInputData[1], storedInputData[0]);
        break;
      case REPORT_ANALOG:
        if(currentReportAnalogCallback)
          (*currentReportAnalogCallback)(multiByteChannel,storedInputData[0]);
        break;
      case REPORT_DIGITAL:
        if(currentReportDigitalCallback)
          (*currentReportDigitalCallback)(multiByteChannel,storedInputData[0]);
        break;
      }
      executeMultiByteCommand = 0;
    }
  } else {
    // remove channel info from command byte if less than 0xF0
    if(inputData < 0xF0) {
      command = inputData & 0xF0;
      multiByteChannel = inputData & 0x0F;
    } else {
      command = inputData;
      // commands in the 0xF* range don't use channel data
    }
    switch (command) {
    case ANALOG_MESSAGE:
    case DIGITAL_MESSAGE:
    case SET_PIN_MODE:
      waitForData = 2; // two data bytes needed
      executeMultiByteCommand = command;
      break;
    case REPORT_ANALOG:
    case REPORT_DIGITAL:
      waitForData = 1; // two data bytes needed
      executeMultiByteCommand = command;
      break;
    case START_SYSEX:
      parsingSysex = true;
      sysexBytesRead = 0;
      break;
    case SYSTEM_RESET:
      systemReset();
      break;
    case REPORT_VERSION:
      printVersion();
      break;
    }
  }
}
// serial send handling
void sendAnalog(uint8_t pin, uint16_t value){
  // pin can only be 0-15, so chop higher bits
  usartPut(ANALOG_MESSAGE | (pin & 0xF));
  sendValueAsTwo7bitBytes(value);
}
// send 14-bits in a single digital message (protocol v1)
// send an 8-bit port in a single digital message (protocol v2)
void sendDigitalPort(uint8_t portNumber, uint16_t portData){
  usartPut(DIGITAL_MESSAGE | (portNumber & 0xF));
  usartPut((uint8_t)portData % 128); // Tx bits 0-6
  usartPut(portData >> 7);  // Tx bits 7-13
}
void sendString(const char* string){
  sendStringCommand(STRING_DATA, string);
}
void sendStringCommand(uint8_t command, const char* string){
  sendSysex(command, strlen(string), (uint8_t*)string);
}
void sendSysex(uint8_t command, uint8_t bytec, uint8_t* bytev){
  startSysex();
  usartPut(command);
  for(uint8_t i = 0; i < bytec; i++) {
    sendValueAsTwo7bitBytes(bytev[i]);
  }
  endSysex();
}
// attach & detach callback functions to messages
void attachCallbackFunction(uint8_t command, callbackFunction newFunction){
  switch(command) {
  case ANALOG_MESSAGE:
    currentAnalogCallback = newFunction;
    break;
  case DIGITAL_MESSAGE:
    currentDigitalCallback = newFunction;
    break;
  case REPORT_ANALOG:
    currentReportAnalogCallback = newFunction;
    break;
  case REPORT_DIGITAL:
    currentReportDigitalCallback = newFunction;
    break;
  case SET_PIN_MODE:
    currentPinModeCallback = newFunction;
    break;
  }
}
void attachSystemResetCallbackFunction(uint8_t command, systemResetCallbackFunction newFunction){
  switch(command) {
  case SYSTEM_RESET:
    currentSystemResetCallback = newFunction;
    break;
  }
}
void attachStringCallbackFunction(uint8_t command, stringCallbackFunction newFunction){
  switch(command) {
  case STRING_DATA:
    currentStringCallback = newFunction;
    break;
  }
}
void attachSysexCallbackFunction(uint8_t command, sysexCallbackFunction newFunction){
  currentSysexCallback = newFunction;
}
void detach(uint8_t command){
  switch(command) {
  case SYSTEM_RESET:
    currentSystemResetCallback = NULL;
    break;
  case STRING_DATA:
    currentStringCallback = NULL;
    break;
  case START_SYSEX:
    currentSysexCallback = NULL;
    break;
  default:
    attachCallbackFunction(command, (callbackFunction)NULL);
  }
}
