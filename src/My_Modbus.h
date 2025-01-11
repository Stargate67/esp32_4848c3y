/*
    My_Modbus.h - ModbusIP Library Implementation for HomIS
    
*/

#include <Arduino.h>
#include "MainScreen.h"
#include <ModbusIP_ESP8266.h>
//#include <esp32_smartdisplay.h>

ModbusIP mb;  //ModbusIP object

#define SERDEBUG true

float fnMin(float fInput);
float fnMax(float fInput); 
float fnAverage(float fInput);

String sExtMaxTimeStp = "00:00";
String sExtMinTimeStp = "00:00";

String sClockHHMMSS;
String sClockHHMM;
String sPrintdate;
String sPrintShortdate;

const int numReadings = 10;
float readings [numReadings];
int iReadIndex = 0;
int iStartIndex = 0;
float total = 0;
int aisVal = 0;

//IPAddress MBremote(192, 168, 0, 105);  // Address of Modbus Slave device
IPAddress MBremote(77, 204, 15, 6);   // Address of Internet Box 
const int START_REG = 12688;       // Starting holding register
const int NUM_REGS = 10;           // Number of holding registers to read
const int MB_READ_INTERVAL = 1000;         // Interval between reads (in milliseconds)

uint16_t MBresult[NUM_REGS];
uint8_t show = NUM_REGS;  // Counter for displaying values
uint32_t LastModbusRequest = 0;  // Variable to track the last Modbus request time


