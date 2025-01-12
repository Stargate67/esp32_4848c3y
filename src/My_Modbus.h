/*
    My_Modbus.h - ModbusIP Library Implementation for HomIS
    
*/

#include <Arduino.h>
//#include "time.h"

float fnMin(float fInput);
float fnMax(float fInput); 
float fnAverage(float fInput);
void ReadModbus();

const IPAddress MBremote(77, 204, 15, 6);   // Address of Internet Box 
const int MB_READ_INTERVAL = 1000; 

const int START_REG = 12688;       // Starting holding register
const int NUM_REGS = 10;           // Number of holding registers to read
