/*
    My_Modbus.h - ModbusIP Library Implementation for HomIS
    
*/

#include <Arduino.h>

extern volatile unsigned long g_case20DurationUs; // Duree mesuree du dernier passage en case 20 (diagnostic saccade)

float fnAverage(float fInput);
void MainModbus();
void loadPLCAddress();
void applyPLCAddress(const IPAddress &newAddr);
