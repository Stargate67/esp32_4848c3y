/*
    My_Modbus.h - ModbusIP Library Implementation for HomIS
    
*/

#include <Arduino.h>

float fnMin(float fInput);
float fnMax(float fInput); 
float fnAverage(float fInput);
void ReadModbus();

const IPAddress MBremote(77, 204, 15, 6);   // Address of Internet Box 
const int MB_READ_INTERVAL = 1000; 

const int START_REG = 12688;       // Starting holding register
const int NUM_REGS = 10;           // Number of holding registers to read

const int START_OUTPUT1 = 513;     // Starting Output bits
const int NUM_OUTPUT1 = 16;         // Number of Output Bits


//Lecture des Bits de Sortie 
//$OutPuts = $Plc->ReadModbus( "000513", 16 ); 

//$OutPuts["000518"] =  Chaudière
//$OutPuts["000519"] =  Boot Chaudière
//$OutPuts["000514"] =  Ppe Radiateur
//$OutPuts["000515"] =  Ppe Plancher
//$OutPuts["000516"] =  Vanne Ouvre
//$OutPuts["000517"] =  Vanne ferme

